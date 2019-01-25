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

#ifndef JSONLITE_BUFFER_DYNAMIC__H
#define JSONLITE_BUFFER_DYNAMIC__H

#ifndef JSONLITE_AMALGAMATED
#include "jsonlite_buffer.h"
#endif

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif
    #define jsonlite_heap_buffer_size() (sizeof(jsonlite_buffer_struct))
    jsonlite_buffer jsonlite_heap_buffer_init(void *mem);
    void jsonlite_heap_buffer_cleanup(jsonlite_buffer buffer);
    
    jsonlite_buffer jsonlite_null_buffer();
    
#ifdef __cplusplus
}
#endif

#endif
