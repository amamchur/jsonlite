//
//  Copyright 2012-2013, Andrii Mamchur
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
#include "jsonlite.h"

int main(int argc, const char * argv[]) {
    const int JSON_DEPTH = 4;                                                   // Limitation of JSON depth
    char json[] = "[-1, 0, 1, true, false, null]";                              // JSON to validate
    size_t mem_used = jsonlite_parser_estimate_size(JSON_DEPTH);                // Estimate memory usage
    printf("jsonlite will use %zd bytes of RAM for JSON validation", mem_used);
    jsonlite_parser p = jsonlite_parser_init(JSON_DEPTH);                       // Init parser with specified depth
    jsonlite_result result = jsonlite_parser_tokenize(p, json, sizeof(json));   // Check JSON
    assert(result == jsonlite_result_ok);                                       // Check result
    jsonlite_parser_release(p);                                                 // Free resources
    return 0;
}
