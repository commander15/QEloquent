set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(TEST_DATA ${PROJECT_SOURCE_DIR}/testdata)

find_package(GTest REQUIRED)

qt_add_executable(QEloquentTest main.cpp
    test.h test.cpp
    simplecrudtest.h simplecrudtest.cpp
    querygenerationtest.h querygenerationtest.cpp
    testmodel.h testmodel.cpp
)

target_link_libraries(QEloquentTest PRIVATE GTest::gtest QEloquent Qt::Test)

target_compile_definitions(QEloquentTest
    PRIVATE
        TEST_DATA_DIR="${TEST_DATA}"
)

add_test(NAME QEloquentTest COMMAND QEloquentTest CONFIGURATIONS)
