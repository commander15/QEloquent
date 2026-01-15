option(QELOQUENT_TESTS_USE_IN_MEMORY_DB "Use in memory DB for tests" ON)
if (QELOQUENT_TESTS_USE_IN_MEMORY_DB)
    set(TEST_DB ":memory:")
else()
    set(TEST_DB "${CMAKE_CURRENT_SOURCE_DIR}/database.db")
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(TEST_DATA ${PROJECT_SOURCE_DIR}/testdata)

# Get Google Test
set(BUILD_GMOCK OFF)
include(FetchContent)

FetchContent_Declare(GTest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
    FIND_PACKAGE_ARGS NAMES GTest 1.14.0
)

FetchContent_MakeAvailable(GTest)

qt_add_executable(QEloquentTest
    main.cpp
    core/mytest.h core/mytest.cpp
    models/invalidmodels.h models/invalidmodels.cpp
    models/simplemodels.h models/simplemodels.cpp
    models/complexmodels.h models/complexmodels.cpp
    metadata.h metadata.cpp
    querygenerator.h querygenerator.cpp
    serialization.h serialization.cpp
    simplemodel.h simplemodel.cpp
    complexmodel.h complexmodel.cpp
)

if (TRUE)
    target_sources(QEloquentTest
        PRIVATE
            schema.h schema.cpp
            migration.h migration.cpp
    )
endif()

target_include_directories(QEloquentTest PRIVATE . core models)

target_link_libraries(QEloquentTest PRIVATE gtest QEloquent)

target_compile_definitions(QEloquentTest
    PRIVATE
        TEST_DATA_DIR="${TEST_DATA}"
        TEST_DB_NAME="${TEST_DB}"
)

add_test(NAME QEloquentTest COMMAND QEloquentTest CONFIGURATIONS)
