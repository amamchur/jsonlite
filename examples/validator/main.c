//
//  Copyright 2012-2019, Andrii Mamchur
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//  http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License

#include <assert.h>
#include <jsonlite.h>

#define MAX_JSON_DEPTH 4

const char json[] = "[-1, 0, 1, true, false, null]";

// Reserving memory for jsonlite parser
// Use jsonlite_parser_estimate_size marco to determinate
// minimum requires mem size
uint8_t parser_memory[jsonlite_parser_estimate_size(MAX_JSON_DEPTH)];

int main(int argc, const char *argv[]) {
    size_t mem_used = jsonlite_parser_estimate_size(MAX_JSON_DEPTH);
    printf("jsonlite uses %d bytes of RAM for JSON validation\n", (int)mem_used);

    // We are not going to use chunk processing in this example
    // so we don't need extra buffer for incomplete tokens
    // use jsonlite_null_buffer is such case
    jsonlite_parser p = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer);
    assert(p != NULL);

    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json));
    assert(result == jsonlite_result_ok);

    return result;
}