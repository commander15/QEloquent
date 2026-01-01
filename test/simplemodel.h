#ifndef SIMPLEMODEL_H
#define SIMPLEMODEL_H

#include <core/mytest.h>
#include <models/simplemodels.h>

#include <QEloquent/result.h>

template<typename V, typename E>
using Result = QEloquent::Result<V, E>;

template<typename E>
using unexpected = QEloquent::unexpected<E>;

// We need DB facilities: only a valid connection in fact ;)
class SimpleModel : public MyTest {
public:
    static Result<bool, QString> isReallyAnApple(const SimpleProduct &product, const std::function<void(SimpleProduct &)> &alter = nullptr) {
        SimpleProduct apple;
        apple.id = 1;
        apple.name = "Apple";
        apple.description = "Fresh red apple";
        apple.price = 0.5;
        apple.barcode = "1234567890123";
        apple.createdAt = QDateTime::currentDateTime();
        apple.setProperty("categoryId", 1);

        if (alter) alter(apple);
        return isReally(product, apple);
    }

    static Result<bool, QString> isReally(const SimpleProduct &product, const SimpleProduct &expected)
    {
        // Expected values - defined once and reused
        const qint64 expectedId = expected.id;
        const QString expectedName = expected.name;
        const QString expectedDescription = expected.description;
        const double expectedPrice = expected.price;
        const QString expectedBarcode = expected.barcode;
        const int expectedCategoryId = expected.property("categoryId").toInt();
        const QDateTime expectedCreationTime = expected.createdAt;
        const QDateTime expectedUpdateTime = expected.updatedAt;

        // Checking id
        if (product.id != expectedId) {
            return unexpected(QString("Expected id %1, got %2").arg(expectedId).arg(product.id));
        }

        // Checking fillable fields
        if (product.name != expectedName) {
            return unexpected(QString("Expected name '%1', got '%2'").arg(expectedName, product.name));
        }
        if (product.description != expectedDescription) {
            return unexpected(QString("Expected description '%1', got '%2'").arg(expectedDescription, product.description));
        }
        if (!qFuzzyCompare(product.price, expectedPrice)) {
            return unexpected(QString("Expected price %1, got %2").arg(expectedPrice).arg(product.price));
        }
        if (product.barcode != expectedBarcode) {
            return unexpected(QString("Expected barcode '%1', got '%2'").arg(expectedBarcode, product.barcode));
        }

        // Checking timestamps (within last 10 seconds)
        static const int timingTolerance = 10;
        if (qAbs<int>(product.createdAt.secsTo(expectedCreationTime)) > timingTolerance) {
            return unexpected(QString("createdAt timestamp too old or in future"));
        }
        if (qAbs<int>(product.updatedAt.secsTo(expectedUpdateTime)) > timingTolerance) {
            return unexpected(QString("updatedAt timestamp too old or in future"));
        }

        // Checking appended property
        static const QString expectedSincePrefix = "since ";
        QString expectedSince = expectedSincePrefix + QString::number(expectedCreationTime.date().year());
        QString actualSince = product.property("since").toString();
        if (actualSince != expectedSince) {
            return unexpected(QString("Expected appended 'since' = '%1', got '%2'").arg(expectedSince, actualSince));
        }

        // Checking dynamic property
        QVariant categoryIdVar = product.property("categoryId");
        if (!categoryIdVar.isValid() || categoryIdVar.toInt() != expectedCategoryId) {
            return unexpected(QString("Expected dynamic categoryId = %1, got %2")
                                  .arg(expectedCategoryId)
                                  .arg(categoryIdVar.toInt()));
        }

        // All checks passed
        return true;
    }
};

#endif // SIMPLEMODEL_H
