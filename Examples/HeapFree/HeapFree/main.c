//
//  main.c
//  HeapFree
//
//  Created by Andrii Mamchur on 2/2/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#include <stdio.h>
#include <assert.h>
#import "JsonLiteObjC/jsonlite.h"

uint8_t parser_memory[jsonlite_parser_estimate_size(32)];
uint8_t buffer_memory[jsonlite_static_buffer_min_size() + 64];

char json1[] = "{\"a\":null,\"b\":[1,2,3],";
char json2[] = "\"c\":true,\"d\":{\"a\":1,";
char json3[] = "\"b\":[]},\"e\":false,\"f\":[\"a\",\"a\",\"a\"]}";

int main(int argc, const char * argv[]) {
    jsonlite_buffer buffer = jsonlite_static_buffer_init_memory(buffer_memory);
    jsonlite_parser parser = jsonlite_parser_init_memory(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(parser, json1, sizeof(json1) - 1);
    assert(result == jsonlite_result_end_of_stream);
    result = jsonlite_parser_tokenize(parser, json2, sizeof(json2) - 1);
    assert(result == jsonlite_result_end_of_stream);
    result = jsonlite_parser_tokenize(parser, json3, sizeof(json3) - 1);
    assert(result == jsonlite_result_ok);
    jsonlite_parser_cleanup(parser);
    return 0;
}

