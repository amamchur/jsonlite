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
    char memory[jsonlite_parser_estimate_size(4)];
    
    NSData *data = [self dataFromFile:@"tokens" inDir:@"tokens"];
    XCTAssertNotNil(data, @"Data is nil");
    XCTAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
       
    size_t size = [data length];
    uint8_t *b = malloc(size);
    jsonlite_parser ps = jsonlite_parser_init_memory(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    
    for (size_t i = size - 1; i > 0; i--) {
        ps = jsonlite_parser_init_memory(memory, sizeof(memory), jsonlite_null_buffer);
        memset(b, 0, size);
        memcpy(b, buffer, size - i);
        
        result = jsonlite_parser_tokenize(ps, b, size - i);
        XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    }
    
    free(b);
}

- (void)testTrueToken {
    char memory[jsonlite_parser_estimate_size(4)];
    char json1[] = "[t";
    char json2[] = "[tr";
    char json3[] = "[tru";

    jsonlite_parser ps = jsonlite_parser_init_memory(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json1, sizeof(json1) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    
    ps = jsonlite_parser_init_memory(memory, sizeof(memory), jsonlite_null_buffer);
    result = jsonlite_parser_tokenize(ps, json2, sizeof(json2) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    
    ps = jsonlite_parser_init_memory(memory, sizeof(memory), jsonlite_null_buffer);
    result = jsonlite_parser_tokenize(ps, json3, sizeof(json3) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
}

- (void)testFalseToken {
    char memory[jsonlite_parser_estimate_size(4)];
    char json1[] = "[f";
    char json2[] = "[fa";
    char json3[] = "[fal";
    char json4[] = "[fals";
    
    jsonlite_parser ps = jsonlite_parser_init_memory(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json1, sizeof(json1) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    
    ps = jsonlite_parser_init_memory(memory, sizeof(memory), jsonlite_null_buffer);
    result = jsonlite_parser_tokenize(ps, json2, sizeof(json2) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    
    ps = jsonlite_parser_init_memory(memory, sizeof(memory), jsonlite_null_buffer);
    result = jsonlite_parser_tokenize(ps, json3, sizeof(json3) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    
    ps = jsonlite_parser_init_memory(memory, sizeof(memory), jsonlite_null_buffer);
    result = jsonlite_parser_tokenize(ps, json4, sizeof(json4) - 1);
    XCTAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
}


@end
