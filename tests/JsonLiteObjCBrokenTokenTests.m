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
    NSData *data = [self dataFromFile:@"tokens" inDir:@"tokens"];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
       
    size_t size = [data length];
    uint8_t *b = malloc(size);
    jsonlite_parser ps = jsonlite_parser_init(4);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
    
    for (size_t i = size - 1; i > 0; i--) {
        ps = jsonlite_parser_init(4);
        memset(b, 0, size);
        memcpy(b, buffer, size - i);
        
        result = jsonlite_parser_tokenize(ps, b, size - i);
        STAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
        jsonlite_parser_release(ps);
    }
    
    free(b);
}

- (void)testTrueToken {
    char json1[] = "[t";
    char json2[] = "[tr";
    char json3[] = "[tru";
    jsonlite_parser ps = jsonlite_parser_init(4);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json1, sizeof(json1) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_parser_release(ps);
    
    ps = jsonlite_parser_init(4);
    result = jsonlite_parser_tokenize(ps, json2, sizeof(json2) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_parser_release(ps);
    
    ps = jsonlite_parser_init(4);
    result = jsonlite_parser_tokenize(ps, json3, sizeof(json3) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testFalseToken {
    char json1[] = "[f";
    char json2[] = "[fa";
    char json3[] = "[fal";
    char json4[] = "[fals";
    jsonlite_parser ps = jsonlite_parser_init(4);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json1, sizeof(json1) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_parser_release(ps);
    
    ps = jsonlite_parser_init(4);
    result = jsonlite_parser_tokenize(ps, json2, sizeof(json2) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_parser_release(ps);
    
    ps = jsonlite_parser_init(4);
    result = jsonlite_parser_tokenize(ps, json3, sizeof(json3) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_parser_release(ps);
    
    ps = jsonlite_parser_init(4);
    result = jsonlite_parser_tokenize(ps, json4, sizeof(json4) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Bad error code");
    jsonlite_parser_release(ps);
}


@end
