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
    set(JSONLITE_LIB_TYPE STATIC src/jsonlite_stream_static.c include/jsonlite_utf.h examples/stack_usage/main.c src/jsonlite_stack_check.c include/jsonlite_stack_check.h)
endif ()

include_directories(include)

set(JSONLIE_SRC
        src/jsonlite_parser.c
        src/jsonlite_builder.c
        src/jsonlite_buffer.c
        src/jsonlite_buffer_dynamic.c
        src/jsonlite_buffer_static.c
        src/jsonlite_stream.c
        src/jsonlite_stream_dynamic.c
        src/jsonlite_stream_file.c
        src/jsonlite_stream_static.c
        src/jsonlite_stream_stdout.c
        src/jsonlite_token.c
        src/jsonlite_token_pool.c
        src/jsonlite_stack_check.c
        )
add_library(jsonlite ${JSONLITE_LIB_TYPE} ${JSONLIE_SRC})

if (BUILD_EXAMPLES)
    add_executable(jsonlite_beautifier examples/beautifier/main.c)
    target_link_libraries(jsonlite_beautifier jsonlite)

    add_executable(jsonlite_adder examples/adder/main.c)
    target_link_libraries(jsonlite_adder jsonlite)

    add_executable(jsonlite_validator examples/validator/main.c)
    target_link_libraries(jsonlite_validator jsonlite)

    add_executable(jsonlite_chunk examples/chunk/main.c)
    target_link_libraries(jsonlite_chunk jsonlite)

    add_executable(jsonlite_stack_usage examples/stack_usage/main.c ${JSONLIE_SRC})
    target_compile_definitions(jsonlite_stack_usage PRIVATE JSONLIE_STACK_CHECK)
endif ()

if (BUILD_TESTS AND GTEST_FOUND)
    add_executable(amalgamated-test-mcu
            amalgamated/mcu/jsonlite.c
            examples/validator/main.c
            )
    add_executable(amalgamated-test-all
            amalgamated/all/jsonlite.c
            examples/beautifier/main.c
            )

    add_executable(collision tests/collision.cpp)
    target_link_libraries(collision jsonlite pthread)

    enable_testing()
    add_executable(jsonlite_tests
            src/jsonlite_parser.c
            src/jsonlite_builder.c
            src/jsonlite_buffer.c
            src/jsonlite_buffer_dynamic.c
            src/jsonlite_buffer_static.c
            src/jsonlite_stream.c
            src/jsonlite_stream_dynamic.c
            src/jsonlite_stream_file.c
            src/jsonlite_stream_static.c
            src/jsonlite_stream_stdout.c
            src/jsonlite_token.c
            src/jsonlite_token_pool.c

            tests/callback_recorder.cpp
            tests/parsing_success.cpp
            tests/parsing_error_handling.cpp
            tests/parsing_chunk.cpp
            tests/parsing_control.cpp
            tests/buffer_dynamic.cpp
            tests/buffer_static.cpp
            tests/tokens.cpp
            tests/version.cpp
            tests/builder.cpp
            tests/builder_error_handling.cpp
            tests/stream_null.cpp
            tests/stream_static.cpp
            tests/stream_stdout.cpp
            tests/stream_dynamic.cpp
            tests/stream_file.cpp
            tests/token_pool.cpp
            tests/parsing_state.cpp
            tests/parsing_depth.cpp
            tests/builder_depth.cpp
            )
    target_compile_definitions(jsonlite_tests PRIVATE TEST_DIR="${CMAKE_CURRENT_SOURCE_DIR}/tests")

    if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        target_compile_options(jsonlite_tests PRIVATE -fprofile-instr-generate -fcoverage-mapping)
        target_link_libraries(jsonlite_tests PRIVATE GTest::GTest GTest::Main --coverage)
    else()
        target_compile_options(jsonlite_tests PRIVATE -O0 -g --coverage -fprofile-arcs -ftest-coverage)
        target_link_libraries(jsonlite_tests PRIVATE GTest::GTest GTest::Main --coverage)
    endif()

    add_test(NAME jsonlite_tests COMMAND jsonlite_tests)
endif ()
