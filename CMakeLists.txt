cmake_minimum_required(VERSION 3.6)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

add_library(jsonlite STATIC
        ./jsonlite/src/jsonlite.c
        ./jsonlite/src/jsonlite_parser.c
        ./jsonlite/src/jsonlite_builder.c
        ./jsonlite/src/jsonlite_buffer.c
        ./jsonlite/src/jsonlite_stream.c
        ./jsonlite/src/jsonlite_token.c
        ./jsonlite/src/jsonlite_token_pool.c
        )
include_directories(./jsonlite/include)

add_executable(jsonlite_beautifier examples/beautifier/main.c)
target_link_libraries(jsonlite_beautifier jsonlite)

add_executable(jsonlite_adder examples/adder/main.c)
target_link_libraries(jsonlite_adder jsonlite)

add_executable(jsonlite_validator examples/validator/main.c)
target_link_libraries(jsonlite_validator jsonlite)

add_executable(jsonlite_chunk examples/chunk/main.c)
target_link_libraries(jsonlite_chunk jsonlite)
