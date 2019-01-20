cmake_minimum_required(VERSION 3.6)
project(jsonlite LANGUAGES C VERSION 1.2.3.0)

option(BUILD_SHARED_LIBS "Build libraries as shared" ON)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        message(STATUS "Setting build type to 'Release' as none was specified.")
        set_property(CACHE CMAKE_BUILD_TYPE PROPERTY VALUE "Release")
    endif()
endif()

if(MSVC)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS -D_CRT_NONSTDC_NO_DEPRECATE)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

if (BUILD_SHARED_LIBS)
	set(JSONLITE_LIB_TYPE SHARED)
elseif()
	set(JSONLITE_LIB_TYPE STATIC)
endif()

include_directories(./jsonlite/include)

add_library(jsonlite ${JSONLITE_LIB_TYPE}
        ./jsonlite/src/jsonlite.c
        ./jsonlite/src/jsonlite_parser.c
        ./jsonlite/src/jsonlite_builder.c
        ./jsonlite/src/jsonlite_buffer.c
        ./jsonlite/src/jsonlite_stream.c
        ./jsonlite/src/jsonlite_token.c
        ./jsonlite/src/jsonlite_token_pool.c
        )

add_executable(jsonlite_beautifier examples/beautifier/main.c)
target_link_libraries(jsonlite_beautifier jsonlite)

add_executable(jsonlite_adder examples/adder/main.c)
target_link_libraries(jsonlite_adder jsonlite)

add_executable(jsonlite_validator examples/validator/main.c)
target_link_libraries(jsonlite_validator jsonlite)

add_executable(jsonlite_chunk examples/chunk/main.c)
target_link_libraries(jsonlite_chunk jsonlite)
