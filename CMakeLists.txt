cmake_minimum_required(VERSION 3.30)

project(
    QEloquent
    VERSION 1.0.0
    DESCRIPTION "Qt most flexible ORM."
    LANGUAGES CXX C
)

option(QELOQUENT_BUILD_TESTS    "Build tests"         OFF)
option(QELOQUENT_BUILD_DOC      "Build documentation" OFF)
option(QELOQUENT_BUILD_EXAMPLES "Build examples"      OFF)

# Qt specifics
set(CMAKE_AUTOMOC ON)

# C++ 20
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(cmake/Utils.cmake)

# Finding Qt
set(REQUIRED_MODULES Core Sql)
if (QELOQUENT_BUILD_EXAMPLES)
    list(APPEND OPTIONAL_MODULES Widgets)
endif()
find_package(Qt6 REQUIRED COMPONENTS ${REQUIRED_MODULES} OPTIONAL_COMPONENTS ${OPTIONAL_MODULES})

# Findind expected
set(EXPECTED_BUILD_PACKAGE OFF)
set(EXPECTED_BUILD_TESTS   OFF)
include(FetchContent)

FetchContent_Declare(
    Expected
    GIT_REPOSITORY https://github.com/TartanLlama/expected.git
    GIT_TAG v1.3.1
)

FetchContent_MakeAvailable(Expected)

add_subdirectory(src)

if (QELOQUENT_BUILD_TESTS)
    set(ENV{CTEST_OUTPUT_ON_FAILURE} ON)
    enable_testing()
    add_subdirectory(tests)
endif()

if (QELOQUENT_BUILD_DOC)
    add_subdirectory(doc)
endif()

if (QELOQUENT_BUILD_EXAMPLES AND Qt6Widgets_FOUND)
    add_subdirectory(examples/store)
endif()

# CI/CD workflow

set(CMAKE_CONFIG_ARGS
    -DQELOQUENT_BUILD_TESTS=ON
    -DQELOQUENT_BUILD_DOC=ON
)
string(JOIN " " CMAKE_CONFIG_ARGS ${CMAKE_CONFIG_ARGS})

set(QT_VERSION 6.2.0)

file(READ notes/RELEASE-${PROJECT_VERSION}.md RELEASE_NOTE)

set(MAIL_BODY
    "Hello, QEloquent ${PACKAGE_VERSION} has just been released, look at the new features !"
    "# QEloquent ${PACKAGE_VERSION} Released 🚀"
    "${RELEASE_NOTE}\n\nDownload and try: https://github.com/commander15/QEloquent/releases/latest"
)
string(JOIN "\\n\\n" MAIL_BODY ${MAIL_BODY})
string(REPLACE "\"" "\\\"" MAIL_BODY ${MAIL_BODY})
string(REPLACE "\n" "\\n" MAIL_BODY ${MAIL_BODY})

configure_file(.github/workflows/ci-cd.yml.in ${CMAKE_CURRENT_SOURCE_DIR}/.github/workflows/ci-cd.yml @ONLY)

# Packaging
set(CPACK_PACKAGE_NAME QEloquent)
set(CPACK_PACKAGE_VENDOR Amadou Benjamain)
set(CPACK_PACKAGE_DIRECTORY packages)
set(CPACK_GENERATOR ZIP)
if (LINUX)
    list(APPEND CPACK_GENERATOR TGZ)
endif()
include(CPack)
