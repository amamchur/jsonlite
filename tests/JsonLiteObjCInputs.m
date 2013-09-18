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
    STAssertTrue(size > 0, @"Size can must be greater zero.");
    
    // jsonlite_parser_init
    jsonlite_parser parser = jsonlite_parser_init(0);
    STAssertTrue(parser != NULL, @"Parser is NULL");
    jsonlite_parser_release(parser);
    
    parser = jsonlite_parser_init(16);
    STAssertTrue(parser != NULL, @"Parser is NULL");
    
    // jsonlite_parser_set_callback
    jsonlite_result result = jsonlite_parser_set_callback(NULL, NULL);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_set_callback(NULL, &jsonlite_default_callbacks);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_set_callback(parser, NULL);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_set_callback(parser, &jsonlite_default_callbacks);
    STAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    // jsonlite_parser_get_result
    result = jsonlite_parser_get_result(parser);
    STAssertTrue(result == jsonlite_result_unknown, @"Incorrect result");
    
    result = jsonlite_parser_get_result(NULL);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    // jsonlite_parser_resume
    result = jsonlite_parser_resume(NULL);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_resume(parser);
    STAssertTrue(result == jsonlite_result_not_allowed, @"Incorrect result");
    
    // jsonlite_parser_suspend
    result = jsonlite_parser_suspend(NULL);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_suspend(parser);
    STAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    // jsonlite_parser_tokenize
    result = jsonlite_parser_tokenize(NULL, NULL, 0);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(NULL, NULL, json_size);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(parser, NULL, 0);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(parser, NULL, json_size);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(NULL, json, 0);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(NULL, json, json_size);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(parser, json, 0);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect result");
    
    result = jsonlite_parser_tokenize(parser, json, json_size);
    STAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    // jsonlite_parser_release
    jsonlite_parser_release(parser);
    jsonlite_parser_release(NULL);
}

- (void)testJsonLiteParserTokenC {
    uint16_t *unicode = NULL;
    uint8_t *buffer = NULL;
    uint8_t str[] = "qwerty";
    uint16_t str16[] = {'q', 'w', 'e', 'r', 't', 'y', 0};
    jsonlite_token token;
    token.start = str;
    token.end = str + sizeof(str) - 1;
    token.string_type = jsonlite_string_ascii;
    
    size_t size = jsonlite_token_decode_size_for_uft8(NULL);
    STAssertTrue(size == 0, @"Size is not zero");
    
    size = jsonlite_token_decode_size_for_uft8(&token);
    STAssertTrue(size == sizeof(str), @"Incorrect size - %d", size);
    
    size = jsonlite_token_decode_to_uft8(NULL, NULL);
    STAssertTrue(size == 0, @"Size is not zero");
    
    size = jsonlite_token_decode_to_uft8(NULL, &buffer);
    STAssertTrue(size == 0, @"Size is not zero");
    free(buffer);
    buffer = NULL;
    
    size = jsonlite_token_decode_to_uft8(&token, NULL);
    STAssertTrue(size == 0, @"Size is not zero");
    
    size = jsonlite_token_decode_to_uft8(&token, &buffer);
    STAssertTrue(buffer != NULL, @"Buffer is null");
    STAssertTrue(size == strlen((char *)str), @"Size is not zero");
    STAssertTrue(memcmp(str, buffer, sizeof(str)) == 0, @"String are not equal");
    
    free(buffer);
    buffer = NULL;
    
    size = jsonlite_token_decode_size_for_uft16(NULL);
    STAssertTrue(size == 0, @"Size is not zero");
    
    size = jsonlite_token_decode_size_for_uft16(&token);
    STAssertTrue(size == sizeof(str) * 2, @"Incorrect size - %d", size);
    
    size = jsonlite_token_decode_to_uft16(NULL, NULL);
    STAssertTrue(size == 0, @"Size is not zero");
    
    size = jsonlite_token_decode_to_uft16(NULL, &unicode);
    STAssertTrue(size == 0, @"Size is not zero");
    free(unicode);
    unicode = NULL;
    
    size = jsonlite_token_decode_to_uft16(&token, &unicode);
    STAssertTrue(unicode != NULL, @"Buffer is null");
    STAssertTrue(size == sizeof(str16) - sizeof(uint16_t), @"Size is not zero");
    STAssertTrue(memcmp(str16, unicode, sizeof(str16)) == 0, @"String are not equal");
    
    free(unicode);
    unicode = NULL;
    
    uint8_t n = jsonlite_hex_char_to_uint8('0');
    STAssertTrue(n == 0, @"Incorrect value");
    
    n = jsonlite_hex_char_to_uint8('9');
    STAssertTrue(n == 9, @"Incorrect value");
    
    n = jsonlite_hex_char_to_uint8('5');
    STAssertTrue(n == 5, @"Incorrect value");
    
    n = jsonlite_hex_char_to_uint8('a');
    STAssertTrue(n == 10, @"Incorrect value");
    
    n = jsonlite_hex_char_to_uint8('f');
    STAssertTrue(n == 15, @"Incorrect value");
    
    n = jsonlite_hex_char_to_uint8('c');
    STAssertTrue(n == 12, @"Incorrect value");
    
    n = jsonlite_hex_char_to_uint8('A');
    STAssertTrue(n == 10, @"Incorrect value");
    
    n = jsonlite_hex_char_to_uint8('F');
    STAssertTrue(n == 15, @"Incorrect value");
    
    n = jsonlite_hex_char_to_uint8('C');
    STAssertTrue(n == 12, @"Incorrect value");
    
    n = jsonlite_hex_char_to_uint8('\0');
    STAssertTrue(n == 0xFF, @"Incorrect value");

    n = jsonlite_hex_char_to_uint8('z');
    STAssertTrue(n == 0xFF, @"Incorrect value");
    
    n = jsonlite_hex_char_to_uint8('Z');
    STAssertTrue(n == 0xFF, @"Incorrect value");

}

- (void)testJsonLiteParserObjC {
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    STAssertNotNil(parser, @"Parser is nil");
    [parser release];
    
    parser = [[JsonLiteParser alloc] initWithDepth:0];
    STAssertNotNil(parser, @"Parser is nil");
    [parser release];
    
    parser = [[JsonLiteParser alloc] initWithDepth:512];
    STAssertNotNil(parser, @"Parser is nil");
    STAssertTrue(parser.depth == 512, @"Depth is not equal");
    STAssertNil(parser.delegate, @"Delegate is NOT nil");
    STAssertNil(parser.parseError, @"ParseError is NOT nil");
    
    JsonLiteObjCInputsParserDelegate *del = [[JsonLiteObjCInputsParserDelegate alloc] init];
    parser.delegate = del;
    STAssertTrue(parser.delegate == del, @"Delegate is not equal");
    parser.delegate = nil;
    STAssertTrue(parser.delegate == nil, @"Delegate is not nil");    
    parser.delegate = del;
    STAssertTrue(parser.delegate == del, @"Delegate is not equal");
    
    NSData *data = [self dataFromFile:@"random" inDir:@"success"];
    STAssertNotNil(data, @"Data is nil");
    
    [parser parse:NULL length:1];
    STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Incorrect error");
    
    [parser parse:[data bytes] length:0];
    STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Incorrect error");
    
    [parser parse:data];
    STAssertNil(parser.parseError, @"Parsing fails %@", parser.parseError);
    
    [del release];
    [parser release];
}

- (void)testJsonLiteAccumulator {
    JsonLiteAccumulator *acc = [[JsonLiteAccumulator alloc] init];
    STAssertNotNil(acc, @"Accumulator is nil");
    [acc release];
    
    acc = [JsonLiteAccumulator accumulatorWithDepth:0];
    STAssertNotNil(acc, @"Accumulator is nil");
    
    acc = [JsonLiteAccumulator accumulatorWithDepth:64];
    STAssertNotNil(acc, @"Accumulator is nil");
    
    acc = [[JsonLiteAccumulator alloc] initWithDepth:0];
    STAssertNotNil(acc, @"Accumulator is nil");
    [acc release];
    
    acc = [[JsonLiteAccumulator alloc] initWithDepth:512];
    STAssertNotNil(acc, @"Accumulator is nil");
    STAssertTrue(acc.depth == 512, @"Depth is not equal");
    STAssertNil(acc.delegate, @"Delegate is NOT nil");
    
    JsonLiteObjCInputsAccumulatorDelegate *del = [[JsonLiteObjCInputsAccumulatorDelegate alloc] init];
    acc.delegate = del;
    STAssertTrue(acc.delegate == del, @"Delegate is not equal");
    acc.delegate = nil;
    STAssertTrue(acc.delegate == nil, @"Delegate is not nil");
    
    acc.delegate = del;
    STAssertTrue(acc.delegate == del, @"Delegate is not equal");
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] initWithDepth:512];
    parser.delegate = acc;
    STAssertTrue(parser.delegate == acc, @"Delegate is not equal");
    
    NSData *data = [self dataFromFile:@"random" inDir:@"success"];
    STAssertNotNil(data, @"Data is nil");
    
    [parser parse:data];
    STAssertNil(parser.parseError, @"Parsing fails %@", parser.parseError);
    
    [del release];
    [acc release];
    [parser release];
}

@end
