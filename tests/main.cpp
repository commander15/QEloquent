#include <gtest/gtest.h>

#include <QCoreApplication>
#include <QLoggingCategory>

#include <core/mytest.h>

int main(int argc, char *argv[])
{
    testing::InitGoogleTest(&argc, argv);

    QCoreApplication app(argc, argv);

    static QLoggingCategory::CategoryFilter oldFilter = nullptr;

    auto newFilter = [](QLoggingCategory *cat) {
        const QString name(cat->categoryName());

        if (name == "qeloquent") {
            cat->setEnabled(QtMsgType::QtDebugMsg, false);
            cat->setEnabled(QtMsgType::QtCriticalMsg, false);
            cat->setEnabled(QtMsgType::QtFatalMsg, false);
            cat->setEnabled(QtMsgType::QtInfoMsg, false);
            cat->setEnabled(QtMsgType::QtWarningMsg, false);
            return;
        }

        if (oldFilter == nullptr)
            return;

        oldFilter(cat);
    };

    oldFilter = QLoggingCategory::installFilter(newFilter);

    MyTest::cacheFile(QStringLiteral(TEST_DATA_DIR) + "/store/structure.sql");
    MyTest::cacheFile(QStringLiteral(TEST_DATA_DIR) + "/store/content.sql");

    return RUN_ALL_TESTS();
}
