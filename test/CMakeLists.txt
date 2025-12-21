set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(TEST_DATA ${PROJECT_SOURCE_DIR}/testdata)

find_package(GTest REQUIRED)

qt_add_executable(QEloquentTest
    main.cpp
    core/mytest.h core/mytest.cpp
    models/basemodel.h models/basemodel.cpp
    models/testmodel.h models/testmodel.cpp
    models/user.h models/user.cpp
    models/product.h models/product.cpp
    models/usergroup.h models/usergroup.cpp
    models/sale.h models/sale.cpp
    querygenerationtest.h querygenerationtest.cpp
    simplecrudtest.h simplecrudtest.cpp
    relationtest.h relationtest.cpp
)

target_include_directories(QEloquentTest PRIVATE . core models)

target_link_libraries(QEloquentTest PRIVATE GTest::gtest QEloquent Qt::Test)

target_compile_definitions(QEloquentTest
    PRIVATE
        TEST_DATA_DIR="${TEST_DATA}"
)

add_test(NAME QEloquentTest COMMAND QEloquentTest CONFIGURATIONS)
