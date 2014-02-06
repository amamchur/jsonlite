//
//  JsonLiteObjCBrokenTokenTests.m
//  JsonLiteObjC
//
//  Created by admin on 6/22/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteObjCBrokenTokenTests.h"
#import "JsonLiteSenTestCaseExt.h"

#include "jsonlite.h"

@implementation JsonLiteObjCBrokenTokenTests

- (void)test {
    char parser_memory[jsonlite_parser_estimate_size(4)];
    char buffer_memory[jsonlite_heap_buffer_size()];
    
    NSData *data = [self dataFromFile:@"tokens" inDir:@"tokens"];
    XCTAssertNotNil(data, @"Data is nil");
    XCTAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
       
    size_t size = [data length];
    uint8_t *b = malloc(size);
    jsonlite_parser ps = jsonlite_parser_init(parser_memory, sizeof(parser_memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    
    for (size_t i = size - 1; i > 0; i--) {
        jsonlite_buffer buf = jsonlite_heap_buffer_init(buffer_memory);
        ps = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buf);
        memset(b, 0, size);
        memcpy(b, buffer, size - i);
        
        result = jsonlite_parser_tokenize(ps, b, size - i);
        XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
        
        jsonlite_heap_buffer_cleanup(buf);
    }
    
    free(b);
}

- (void)testTrueToken {
    char parser_memory[jsonlite_parser_estimate_size(4)];
    char buffer_memory[jsonlite_heap_buffer_size()];
    
    char json1[] = "[t";
    char json2[] = "[tr";
    char json3[] = "[tru";

    jsonlite_buffer buffer = jsonlite_heap_buffer_init(buffer_memory);
    jsonlite_parser ps = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json1, sizeof(json1) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_heap_buffer_cleanup(buffer);
    
    buffer = jsonlite_heap_buffer_init(buffer_memory);
    ps = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(ps, json2, sizeof(json2) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_heap_buffer_cleanup(buffer);
    
    buffer = jsonlite_heap_buffer_init(buffer_memory);
    ps = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(ps, json3, sizeof(json3) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_heap_buffer_cleanup(buffer);
}

- (void)testFalseToken {
    char parser_memory[jsonlite_parser_estimate_size(4)];
    char buffer_memory[jsonlite_heap_buffer_size()];
    
    char json1[] = "[f";
    char json2[] = "[fa";
    char json3[] = "[fal";
    char json4[] = "[fals";
    
    jsonlite_buffer buffer = jsonlite_heap_buffer_init(buffer_memory);
    jsonlite_parser ps = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json1, sizeof(json1) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_heap_buffer_cleanup(buffer);
    
    buffer = jsonlite_heap_buffer_init(buffer_memory);
    ps = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(ps, json2, sizeof(json2) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_heap_buffer_cleanup(buffer);
    
    buffer = jsonlite_heap_buffer_init(buffer_memory);
    ps = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(ps, json3, sizeof(json3) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_heap_buffer_cleanup(buffer);
    
    buffer = jsonlite_heap_buffer_init(buffer_memory);
    ps = jsonlite_parser_init(parser_memory, sizeof(parser_memory), buffer);
    result = jsonlite_parser_tokenize(ps, json4, sizeof(json4) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_heap_buffer_cleanup(buffer);
}


@end
