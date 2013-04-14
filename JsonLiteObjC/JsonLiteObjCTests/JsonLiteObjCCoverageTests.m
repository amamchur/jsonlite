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

#import "JsonLiteObjCCoverageTests.h"
#import "JsonLiteSenTestCaseExt.h"

#include "jsonlite.h"
#include "jsonlite_hash.h"

#import "JsonLiteParser.h"
#import "JsonLiteAccumulator.h"
#import "JsonLiteSerializer.h"

#import <objc/runtime.h>
#import <objc/message.h>

static void state_suspend(jsonlite_callback_context *ctx) {
    jsonlite_parser_suspend(ctx->parser);
}

static void value_suspend(jsonlite_callback_context *ctx, jsonlite_token *token) {
    jsonlite_parser_suspend(ctx->parser);
}

@interface ParseSuspender : NSObject<JsonLiteParserDelegate>
@end

@implementation ParseSuspender

- (void)parser:(JsonLiteParser *)parser didFinishParsingWithError:(NSError *)errors {
}

- (void)parserDidStartObject:(JsonLiteParser *)parser {
    [parser suspend];
}

- (void)parserDidEndObject:(JsonLiteParser *)parser {
    [parser suspend];
}

- (void)parserDidStartArray:(JsonLiteParser *)parser {
    [parser suspend];
}

- (void)parserDidEndArray:(JsonLiteParser *)parser {
    [parser suspend];
}

- (void)parserFoundTrueToken:(JsonLiteParser *)parser {
    [parser suspend];
}

- (void)parserFoundFalseToken:(JsonLiteParser *)parser {
    [parser suspend];
}

- (void)parserFoundNullToken:(JsonLiteParser *)parser {
    [parser suspend];
}

- (void)parser:(JsonLiteParser *)parser foundKeyToken:(JsonLiteStringToken *)token {
    [parser suspend];
}

- (void)parser:(JsonLiteParser *)parser foundStringToken:(JsonLiteStringToken *)token {
    [parser suspend];
}

- (void)parser:(JsonLiteParser *)parser foundNumberToken:(JsonLiteNumberToken *)token {
    [parser suspend];
}

@end


@interface JsonLiteObjCCoverageTests()<JsonLiteParserDelegate>

@end

@implementation JsonLiteObjCCoverageTests

- (void)parser:(JsonLiteParser *)parser didFinishParsingWithError:(NSError *)error  {
}

- (void)parserDidStartObject:(JsonLiteParser *)parser {
}

- (void)parserDidEndObject:(JsonLiteParser *)parser {
}

- (void)parserDidStartArray:(JsonLiteParser *)parser {
}

- (void)parserDidEndArray:(JsonLiteParser *)parser {
}

- (void)parserFoundTrueToken:(JsonLiteParser *)parser {
}

- (void)parserFoundFalseToken:(JsonLiteParser *)parser {
}

- (void)parserFoundNullToken:(JsonLiteParser *)parser {
}

- (void)parser:(JsonLiteParser *)parser foundKeyToken:(JsonLiteStringToken *)token {
    [token retain];
    [token retain];
    [token release];
    [token autorelease];
    STAssertTrue([token retainCount] > 0, @"Bad retainCount");
    STAssertTrue([token description] != nil, @"Bad description");
}

- (void)parser:(JsonLiteParser *)parser foundStringToken:(JsonLiteStringToken *)token {
    [token retain];
    [token retain];
    [token release];
    [token autorelease];
    STAssertTrue([token retainCount] > 0, @"Bad retainCount");
    STAssertTrue([token description] != nil, @"Bad description");
}

- (void)parser:(JsonLiteParser *)parser foundNumberToken:(JsonLiteNumberToken *)token {
    [token retain];
    [token retain];
    [token release];
    [token autorelease];
    STAssertTrue([token retainCount] > 0, @"Bad retainCount");
    STAssertTrue([token description] != nil, @"Bad description");
}

- (void)testIncorrectInitialization {
    jsonlite_result result = jsonlite_parser_get_result(NULL);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Bad error");

    jsonlite_parser ps = jsonlite_parser_init(100);
    char json[] = "{}";
    result = jsonlite_parser_tokenize(NULL, json, sizeof(json));
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Bad error");
    
    result = jsonlite_parser_tokenize(ps, NULL, sizeof(json));
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Bad error");
    
    result = jsonlite_parser_tokenize(ps, json, 0);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Bad error");
    
    result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    STAssertTrue(result == jsonlite_result_ok, @"Bad error");
    
    jsonlite_parser_release(ps);
}

- (void)testEmptyCallbacks {
    char json[] = "{\"key\" : 12345, \"array\": [null, true, false, \"string\"]}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    STAssertTrue(result == jsonlite_result_ok, @"Parse fails");
    jsonlite_parser_release(ps);
}

- (void)testInvalidEscape {
    char json[] = "{\"key\" : \"\\u000Q\"}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    STAssertTrue(result == jsonlite_result_invalid_escape, @"Parse fails");
    jsonlite_parser_release(ps);
}

- (void)testInvalidNegativeNumber {
    char json[] = "{\"key\" : -w}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    STAssertTrue(result == jsonlite_result_invalid_number, @"Parse fails");
    jsonlite_parser_release(ps);
}

- (void)testInvalidHexNumber {
    char json[] = "{\"key\" : -0x0011}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    STAssertTrue(result == jsonlite_result_unsupported_hex, @"Parse fails");
    jsonlite_parser_release(ps);
}

- (void)testInvalidNumberEnding {
    char json[] = "{\"key\" : 123456w}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    STAssertTrue(result == jsonlite_result_invalid_number, @"Parse fails");
    jsonlite_parser_release(ps);
}

- (void)testInvalidFracStart {
    char json[] = "{\"key\" : 12345.A132}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    STAssertTrue(result == jsonlite_result_invalid_number, @"Parse fails");
    jsonlite_parser_release(ps);
}

- (void)testInvalidFracEnding {
    char json[] = "{\"key\" : 12345.132A}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    STAssertTrue(result == jsonlite_result_invalid_number, @"Parse fails");
    jsonlite_parser_release(ps);
}

- (void)testInvalidExpEnding {
    char json[] = "{\"key\" : 12345.132e01A}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    STAssertTrue(result == jsonlite_result_invalid_number, @"Parse fails");
    jsonlite_parser_release(ps);
}

- (void)testJsonChunk {
    char json1[] = "{\"key\" : 12345, \"obj\": {}, \"arr";
    char json2[] = "ay\":[null, true, false, \"string\"]}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json1, sizeof(json1) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Parse fails");
    result = jsonlite_parser_tokenize(ps, json2, sizeof(json2) - 1);
    STAssertTrue(result == jsonlite_result_ok, @"Parse fails");
    jsonlite_parser_release(ps);
}

- (void)testHashCollision {
    NSString *str1 = @"Test_#83410_Collision";
    NSString *str2 = @"Test_#152260_Collision";
    NSString *str3 = @"                u-1(l";
    const char *ch1 = [str1 UTF8String];
    const char *ch2 = [str2 UTF8String];
    const char *ch3 = [str3 UTF8String];
    uint32_t h1 = jsonlite_hash((const uint8_t*)ch1, strlen(ch1));
    uint32_t h2 = jsonlite_hash((const uint8_t*)ch2, strlen(ch2));
    uint32_t h3 = jsonlite_hash((const uint8_t*)ch3, strlen(ch3));
    STAssertTrue(h1 == h2, @"NO Collision");
    STAssertTrue(h2 == h3, @"NO Collision");
    
    NSMutableArray *array = [NSMutableArray arrayWithCapacity:3];
    [array addObject:str1];
    [array addObject:str2];
    [array addObject:str3];

    JsonLiteSerializer *ser = [JsonLiteSerializer serializer];
    NSData *data = [ser serializeObject:array];
    
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:32];
    STAssertTrue([array isEqual:obj], @"Not Equal");
}

- (void)testParser {
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:32];
    JsonLiteAccumulator *acc = [JsonLiteAccumulator accumulatorWithDepth:32];
    parser.delegate = acc;
    NSData *data = [NSData data];
    NSError *error = nil;
    
    [parser reset];
    [acc reset];
    [parser parse:data];
    STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Bad error");
    
    [parser reset];
    [acc reset];
    [parser parse:nil];
    STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Bad error");
    
    [parser reset];
    [acc reset];
    [parser parse:nil];
    STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Bad error");
    
    char json1[] = "     ";
    char json2[] = "{\"key\" : 12345, \"obj\": {}, \"arr";
    char json3[] = "ay\":[null, true, false, \"string\", {\"acb\" : 1}],";
    char json4[] = "\"child\": {\"key\" : 12345, \"array\":[]} }";
 
    [parser reset];
    [acc reset];
    parser.delegate = acc;
    [parser parse:[NSData dataWithBytes:json1 length:sizeof(json1) - 1]];
    [parser parse:[NSData dataWithBytes:json2 length:sizeof(json2) - 1]];
    [parser parse:[NSData dataWithBytes:json3 length:sizeof(json3) - 1]];
    [parser parse:[NSData dataWithBytes:json4 length:sizeof(json4) - 1]];
    STAssertTrue(error == nil, @"Bad error");
    
    [parser reset];
    [acc reset];
    parser.delegate = self;
    [parser parse:[NSData dataWithBytes:json1 length:sizeof(json1) - 1]];
    [parser parse:[NSData dataWithBytes:json2 length:sizeof(json2) - 1]];
    [acc reset];
    
    Class cls = [JsonLiteToken class];
    IMP imp = class_getMethodImplementation(cls, @selector(allocValue));
    imp(nil, nil);
}

- (void)testEmptyJsonChunk {
    STAssertTrue(jsonlite_parser_estimate_size(16) == 164, @"");
    
    char json1[] = "     ";
    char json2[] = "{\"key\" : 12345, \"obj\": {}, \"arr";
    char json3[] = "ay\":[null, true, false, \"string\"]}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json1, sizeof(json1) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Parse fails");
    result = jsonlite_parser_tokenize(ps, json2, sizeof(json2) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Parse fails");
    
    result = jsonlite_parser_tokenize(ps, json3, sizeof(json3) - 1);
    STAssertTrue(result == jsonlite_result_ok, @"Parse fails");
    
    jsonlite_parser_release(ps);
}

- (void)testJsonChunkIncorrectInitialization {
    char json1[] = "{\"key\" : 12345, \"obj\": {}, \"arr";
    char json2[] = "ay\":[null, true, false, \"string\"]}";
    jsonlite_parser ps = jsonlite_parser_init(100);
    STAssertTrue(ps != NULL, @"jsonlite_init_parser return NULL");
    jsonlite_result result = jsonlite_parser_tokenize(ps, json1, sizeof(json1) - 1);
    STAssertTrue(result == jsonlite_result_end_of_stream, @"Parse fails");
    jsonlite_parser next = jsonlite_parser_init(100);
    
    result = jsonlite_parser_tokenize(NULL, json2, sizeof(json2) - 1);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Bad error");

    result = jsonlite_parser_tokenize(ps, json2, sizeof(json2) - 1);
    STAssertTrue(result == jsonlite_result_ok, @"Parse fails");
    
    jsonlite_parser_release(ps);
    jsonlite_parser_release(next);
}

- (void)testSuspendResume {
    char json1[] = "{\"key\" : 12345, \"obj\": {}, \"array\":[null, true, false, \"string\"]}";
    jsonlite_parser p = jsonlite_parser_init(100);
    jsonlite_parser_callbacks cbs = {
        &state_suspend,
        &state_suspend,
        &state_suspend,
        &state_suspend,
        &state_suspend,
        &state_suspend,
        &state_suspend,
        &state_suspend,
        &value_suspend,
        &value_suspend,
        &value_suspend,
        NULL
    };
    jsonlite_parser_set_callback(p, &cbs);
    
    jsonlite_result result = jsonlite_parser_resume(p);
    STAssertTrue(result == jsonlite_result_not_allowed, @"Bad result");
    
    result = jsonlite_parser_resume(NULL);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Bad result");
    
    result = jsonlite_parser_suspend(NULL);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Bad result");
    
    result = jsonlite_parser_suspend(p);
    STAssertTrue(result == jsonlite_result_ok, @"Bad result");
    
    result = jsonlite_parser_tokenize(p, json1, sizeof(json1));
    STAssertTrue(result == jsonlite_result_suspended, @"Bad result");
    
    while (result == jsonlite_result_suspended) {
        result = jsonlite_parser_resume(p);
    }
    
    STAssertTrue(result == jsonlite_result_ok, @"Bad result");
    
    result = jsonlite_parser_suspend(p);
    STAssertTrue(result == jsonlite_result_not_allowed, @"Bad result");
    
    jsonlite_parser_release(p);
}

- (void)testSuspendResumeObjC {
    char json1[] = "{\"key\" : 12345, \"obj\": {}, \"array\":[null, true, false, \"string\"]}";
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:32];
    ParseSuspender *del = [[ParseSuspender alloc] init];
    parser.delegate = del;
    
    [parser resume];
    
    BOOL run = ![parser parse:[NSData dataWithBytes:json1 length:sizeof(json1) - 1]];
    while (run) {
        run = ![parser resume] && parser.parseError == nil;
    }

    [del release];
}

@end
