set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(TEST_DATA ${PROJECT_SOURCE_DIR}/testdata)

find_package(GTest REQUIRED)

qt_add_executable(QEloquentTest
    main.cpp
    core/mytest.h core/mytest.cpp
    models/invalidmodels.h models/invalidmodels.cpp
    models/simplemodels.h models/simplemodels.cpp
    models/complexmodels.h models/complexmodels.cpp
    metadata.h metadata.cpp
    querygenerator.h querygenerator.cpp
    simplemodel.h simplemodel.cpp
)

target_include_directories(QEloquentTest PRIVATE . core models)

target_link_libraries(QEloquentTest PRIVATE GTest::gtest QEloquent Qt::Test)

target_compile_definitions(QEloquentTest
    PRIVATE
        TEST_DATA_DIR="${TEST_DATA}"
)

add_test(NAME QEloquentTest COMMAND QEloquentTest CONFIGURATIONS)
