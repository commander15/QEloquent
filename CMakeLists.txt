cmake_minimum_required(VERSION 3.30)

project(
    QEloquent
    VERSION 1.0.0
    DESCRIPTION "Qt most flexible ORM."
    LANGUAGES CXX C
)

option(QELOQUENT_BUILD_TESTS "Build tests" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Utils.cmake)

# Finding Qt
set(REQUIRED_MODULES Core Sql)
if (QELOQUENT_BUILD_TESTS)
    set(OPTIONAL_MODULES Test)
endif()
find_package(Qt6 REQUIRED COMPONENTS ${REQUIRED_MODULES} OPTIONAL_COMPONENTS ${OPTIONAL_MODULES})

qt_standard_project_setup()

# Findind expected
include(FetchContent)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    Expected
    GIT_REPOSITORY https://github.com/TartanLlama/expected.git
    GIT_TAG v1.3.1
)

FetchContent_MakeAvailable(Expected)

add_subdirectory(src)

if (QELOQUENT_BUILD_TESTS AND Qt6Test_FOUND)
    enable_testing()
    set(ENV{CTEST_OUTPUT_ON_FAILURE} ON)
    add_subdirectory(test)
endif()
