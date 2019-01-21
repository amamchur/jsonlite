cmake_minimum_required(VERSION 3.6)
project(jsonlite LANGUAGES C CXX VERSION 1.2.3.0)

find_package(GTest)

option(BUILD_SHARED_LIBS "Build libraries as shared" ON)
option(BUILD_EXAMPLES "Build jsonlite examples" ON)
option(BUILD_TESTS "Build jsonlite examples" ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if (NOT CMAKE_CONFIGURATION_TYPES)
    if (NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
    endif ()
endif ()

if (MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
    set(CMAKE_DEBUG_POSTFIX "d")
endif ()

if (BUILD_SHARED_LIBS)
    set(JSONLITE_LIB_TYPE SHARED)
elseif ()
    set(JSONLITE_LIB_TYPE STATIC)
endif ()

include_directories(include)

add_library(jsonlite ${JSONLITE_LIB_TYPE}
        src/jsonlite.c
        src/jsonlite_parser.c
        src/jsonlite_builder.c
        src/jsonlite_buffer.c
        src/jsonlite_stream.c
        src/jsonlite_token.c
        src/jsonlite_token_pool.c
        )

if (BUILD_EXAMPLES)
    add_executable(jsonlite_beautifier examples/beautifier/main.c)
    target_link_libraries(jsonlite_beautifier jsonlite)

    add_executable(jsonlite_adder examples/adder/main.c)
    target_link_libraries(jsonlite_adder jsonlite)

    add_executable(jsonlite_validator examples/validator/main.c)
    target_link_libraries(jsonlite_validator jsonlite)

    add_executable(jsonlite_chunk examples/chunk/main.c)
    target_link_libraries(jsonlite_chunk jsonlite)
endif ()

if (BUILD_TESTS AND GTEST_FOUND)
    enable_testing()
    add_executable(jsonlite_tests
            src/jsonlite.c
            src/jsonlite_parser.c
            src/jsonlite_builder.c
            src/jsonlite_buffer.c
            src/jsonlite_stream.c
            src/jsonlite_token.c
            src/jsonlite_token_pool.c

            tests/callback_recorder.cpp
            tests/success_parsing.cpp
            tests/error_handling.cpp
            tests/chunk_processing.cpp
            tests/parse_control.cpp
            tests/version.cpp
            )
    target_compile_definitions(jsonlite_tests PRIVATE TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests")
    target_compile_options(jsonlite_tests PRIVATE -O0 -g --coverage -fprofile-arcs -ftest-coverage)
    target_link_libraries(jsonlite_tests PRIVATE GTest::GTest GTest::Main --coverage)

    add_test(NAME jsonlite_tests COMMAND jsonlite_tests)
endif ()
