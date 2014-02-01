//
//  JsonLiteObjCInputParams.m
//  JsonLiteObjC
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteObjCInputs.h"
#import "JsonLiteSenTestCaseExt.h"

#include "jsonlite.h"

#import "JsonLiteParser.h"
#import "JsonLiteAccumulator.h"
#import "JsonLiteSerializer.h"


@interface JsonLiteObjCInputsAccumulatorDelegate : NSObject<JsonLiteAccumulatorDelegate>

@end

@implementation JsonLiteObjCInputsAccumulatorDelegate

- (void)accumulator:(JsonLiteAccumulator *)accumulator didAccumulateArray:(NSArray *)array {}
- (void)accumulator:(JsonLiteAccumulator *)accumulator didAccumulateDictionary:(NSDictionary *)dictionary {}

@end

@interface JsonLiteObjCInputsParserDelegate : NSObject<JsonLiteParserDelegate>

@end

@implementation JsonLiteObjCInputsParserDelegate

- (void)parserDidStartObject:(JsonLiteParser *)parser {}
- (void)parserDidEndObject:(JsonLiteParser *)parser {}
- (void)parserDidStartArray:(JsonLiteParser *)parser {}
- (void)parserDidEndArray:(JsonLiteParser *)parser {}
- (void)parser:(JsonLiteParser *)parser foundKeyToken:(JsonLiteStringToken *)token {}
- (void)parser:(JsonLiteParser *)parser foundStringToken:(JsonLiteStringToken *)token {}
- (void)parser:(JsonLiteParser *)parser foundNumberToken:(JsonLiteNumberToken *)token {}
- (void)parserFoundTrueToken:(JsonLiteParser *)parser {}
- (void)parserFoundFalseToken:(JsonLiteParser *)parser {}
- (void)parserFoundNullToken:(JsonLiteParser *)parser {}
- (void)parser:(JsonLiteParser *)parser didFinishParsingWithError:(NSError *)error {}

@end

@implementation JsonLiteObjCInputs

- (void)testJsonLiteParserC {
    char json[] = "{}";
    size_t json_size = sizeof(json) - 1;
    
    // jsonlite_parser_estimate_size
    size_t size = jsonlite_parser_estimate_size(0);
    XCTAssertTrue(size > 0, @"Size can must be greater zero.");
    
    // jsonlite_parser_init
    jsonlite_parser parser = jsonlite_parser_init(0, jsonlite_null_buffer);
    XCTAssertTrue(parser != NULL, @"Parser is NULL");
    jsonlite_parser_release(parser);
    
    parser = jsonlite_parser_init(16, jsonlite_null_buffer);
    XCTAssertTrue(parser != NULL, @"Parser is NULL");
    
    // jsonlite_parser_set_callback
    jsonlite_result result = jsonlite_parser_set_callback(NULL, NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_set_callback(NULL, &jsonlite_default_callbacks);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_set_callback(parser, NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_set_callback(parser, &jsonlite_default_callbacks);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    // jsonlite_parser_get_result
    result = jsonlite_parser_get_result(parser);
    XCTAssertTrue(result == jsonlite_result_unknown, @"Incorrect result");
    
    result = jsonlite_parser_get_result(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    // jsonlite_parser_resume
    result = jsonlite_parser_resume(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_resume(parser);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Incorrect result");
    
    // jsonlite_parser_suspend
    result = jsonlite_parser_suspend(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_suspend(parser);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Incorrect result");
    
    // jsonlite_parser_tokenize
    result = jsonlite_parser_tokenize(NULL, NULL, 0);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(NULL, NULL, json_size);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(parser, NULL, 0);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(parser, NULL, json_size);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(NULL, json, 0);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(NULL, json, json_size);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(parser, json, 0);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(parser, json, json_size);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    // jsonlite_parser_release
    jsonlite_parser_release(parser);
    jsonlite_parser_release(NULL);
    
    parser = jsonlite_parser_init_memory(NULL, 123, jsonlite_null_buffer);
    XCTAssertTrue(parser == NULL, @"Parser in not NULL");
    
    char mem[256];
    parser = jsonlite_parser_init_memory(mem, 5, jsonlite_null_buffer);
    XCTAssertTrue(parser == NULL, @"Parser in not NULL");
}

- (void)testJsonLiteParserTokenC {
    uint16_t *unicode = NULL;
    uint8_t *buffer = NULL;
    uint8_t str[] = "qwerty";
    uint16_t str16[] = {'q', 'w', 'e', 'r', 't', 'y', 0};
    jsonlite_token token;
    token.start = str;
    token.end = str + sizeof(str) - 1;
    token.type.string = jsonlite_string_ascii;
    
    size_t size = jsonlite_token_size_of_uft8(&token);
    XCTAssertTrue(size == sizeof(str), @"Incorrect size - %zu", size);
        
    size = jsonlite_token_to_uft8(&token, &buffer);
    XCTAssertTrue(buffer != NULL, @"Buffer is null");
    XCTAssertTrue(size == strlen((char *)str), @"Size is not zero");
    XCTAssertTrue(memcmp(str, buffer, sizeof(str)) == 0, @"String are not equal");
    
    free(buffer);
    buffer = NULL;
    
    size = jsonlite_token_size_of_uft16(&token);
    XCTAssertTrue(size == sizeof(str) * 2, @"Incorrect size - %zu", size);
        
    size = jsonlite_token_to_uft16(&token, &unicode);
    XCTAssertTrue(unicode != NULL, @"Buffer is null");
    XCTAssertTrue(size == sizeof(str16) - sizeof(uint16_t), @"Size is not zero");
    XCTAssertTrue(memcmp(str16, unicode, sizeof(str16)) == 0, @"String are not equal");
    
    free(unicode);
    unicode = NULL;
}

- (void)testJsonLiteParserObjC {
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    XCTAssertNotNil(parser, @"Parser is nil");
    [parser release];
    
    parser = [[JsonLiteParser alloc] initWithDepth:0];
    XCTAssertNotNil(parser, @"Parser is nil");
    [parser release];
    
    parser = [[JsonLiteParser alloc] initWithDepth:512];
    XCTAssertNotNil(parser, @"Parser is nil");
    XCTAssertTrue(parser.depth == 512, @"Depth is not equal");
    XCTAssertNil(parser.delegate, @"Delegate is NOT nil");
    XCTAssertNil(parser.parseError, @"ParseError is NOT nil");
    
    JsonLiteObjCInputsParserDelegate *del = [[JsonLiteObjCInputsParserDelegate alloc] init];
    parser.delegate = del;
    XCTAssertTrue(parser.delegate == del, @"Delegate is not equal");
    parser.delegate = nil;
    XCTAssertTrue(parser.delegate == nil, @"Delegate is not nil");    
    parser.delegate = del;
    XCTAssertTrue(parser.delegate == del, @"Delegate is not equal");
    
    NSData *data = [self dataFromFile:@"random" inDir:@"success"];
    XCTAssertNotNil(data, @"Data is nil");
    
    [parser parse:NULL length:1];
    XCTAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Incorrect error");
    
    [parser parse:[data bytes] length:0];
    XCTAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Incorrect error");
    
    [parser parse:data];
    XCTAssertNil(parser.parseError, @"Parsing fails %@", parser.parseError);
    
    [del release];
    [parser release];
}

- (void)testJsonLiteAccumulator {
    JsonLiteAccumulator *acc = [[JsonLiteAccumulator alloc] init];
    XCTAssertNotNil(acc, @"Accumulator is nil");
    [acc release];
    
    acc = [JsonLiteAccumulator accumulatorWithDepth:0];
    XCTAssertNotNil(acc, @"Accumulator is nil");
    
    acc = [JsonLiteAccumulator accumulatorWithDepth:64];
    XCTAssertNotNil(acc, @"Accumulator is nil");
    
    acc = [[JsonLiteAccumulator alloc] initWithDepth:0];
    XCTAssertNotNil(acc, @"Accumulator is nil");
    [acc release];
    
    acc = [[JsonLiteAccumulator alloc] initWithDepth:512];
    XCTAssertNotNil(acc, @"Accumulator is nil");
    XCTAssertTrue(acc.depth == 512, @"Depth is not equal");
    XCTAssertNil(acc.delegate, @"Delegate is NOT nil");
    
    JsonLiteObjCInputsAccumulatorDelegate *del = [[JsonLiteObjCInputsAccumulatorDelegate alloc] init];
    acc.delegate = del;
    XCTAssertTrue(acc.delegate == del, @"Delegate is not equal");
    acc.delegate = nil;
    XCTAssertTrue(acc.delegate == nil, @"Delegate is not nil");
    
    acc.delegate = del;
    XCTAssertTrue(acc.delegate == del, @"Delegate is not equal");
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] initWithDepth:512];
    parser.delegate = acc;
    XCTAssertTrue(parser.delegate == acc, @"Delegate is not equal");
    
    NSData *data = [self dataFromFile:@"random" inDir:@"success"];
    XCTAssertNotNil(data, @"Data is nil");
    
    [parser parse:data];
    XCTAssertNil(parser.parseError, @"Parsing fails %@", parser.parseError);
    
    [acc release];
    [del release];
    [parser release];
}

@end
