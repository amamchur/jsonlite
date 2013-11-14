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

#import "JsonLiteObjCTokenTests.h"
#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteAccumulator.h"

#include "jsonlite.h"
#import "JsonLiteSerializer.h"
#import "JsonLiteAccumulator.h"

@implementation JsonLiteObjCTokenTests

- (void)testToken {
    id obj = [self parseObjectFromFile:@"tokens" inDir:@"tokens"];
    STAssertNotNil(obj, @"Object is nil");
}

- (void)testZeroTerminatedByTab {
    NSString *json = @"[0\t]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testZeroTerminatedByLF {
    NSString *json = @"[0\n]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testZeroTerminatedByCR {
    NSString *json = @"[0\r]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testZeroTerminatedBySpace {
    NSString *json = @"[0 ]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testZeroTerminatedByComma {
    NSString *json = @"[0, 0]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testZeroTerminatedBySquareBraket {
    NSString *json = @"[0]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testZeroTerminatedByBrace {
    NSString *json = @"{\"key\":0}";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testIntTerminatedByTab {
    NSString *json = @"[12345\t]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testIntTerminatedByLF {
    NSString *json = @"[12345\n]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testIntTerminatedByCR {
    NSString *json = @"[12345\r]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testIntTerminatedBySpace {
    NSString *json = @"[12345 ]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testIntTerminatedByComma {
    NSString *json = @"[12345, 12345]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testIntTerminatedBySquareBraket {
    NSString *json = @"[12345]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testIntTerminatedByBrace {
    NSString *json = @"{\"key\":12345}";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testExpTerminatedByTab {
    NSString *json = @"[1234e5\t]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testExpTerminatedByLF {
    NSString *json = @"[1234e5\n]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testExpTerminatedByCR {
    NSString *json = @"[1234e5\r]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testExpTerminatedBySpace {
    NSString *json = @"[1234e5 ]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testExpTerminatedByComma {
    NSString *json = @"[1234e5, 1234e5]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testExpTerminatedBySquareBraket {
    NSString *json = @"[1234e5]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testExpTerminatedByBrace {
    NSString *json = @"{\"key\":1234e5}";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testFracTerminatedByTab {
    NSString *json = @"[1234.5\t]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testFracTerminatedByLF {
    NSString *json = @"[1234.5\n]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testFracTerminatedByCR {
    NSString *json = @"[1234.5\r]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testFracTerminatedBySpace {
    NSString *json = @"[1234.5 ]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testFracTerminatedByComma {
    NSString *json = @"[1234.5, 1234.5]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testFracTerminatedBySquareBraket {
    NSString *json = @"[1234.5]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testFracTerminatedByBrace {
    NSString *json = @"{\"key\":1234.5}";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    STAssertNotNil(data, @"Data is nil");
    STAssertTrue([data length] > 0, @"Data is empty");
    
    const void *buffer = [data bytes];
    size_t size = [data length];
    jsonlite_parser ps = jsonlite_parser_init(8);
    jsonlite_result result = jsonlite_parser_tokenize(ps, buffer, size);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testNumber {
    NSNumber *n1 = [NSNumber numberWithLongLong:4278190080];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1]];
    NSNumber *n2 = [[JsonLiteAccumulator objectFromData:data withMaxDepth:16] lastObject];
    STAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMaxLong {
    NSNumber *n1 = [NSNumber numberWithLong:LONG_MAX];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteAccumulator objectFromData:data withMaxDepth:16] lastObject];
    STAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMinLong {
    NSNumber *n1 = [NSNumber numberWithLong:LONG_MIN];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteAccumulator objectFromData:data withMaxDepth:16] lastObject];
    STAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMaxLongP1 {
    NSNumber *n1 = [NSNumber numberWithLongLong:(long long)LONG_MAX + 1ll];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteAccumulator objectFromData:data withMaxDepth:16] lastObject];
    STAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMinLongM1 {
    NSNumber *n1 = [NSNumber numberWithLongLong:LONG_MIN - 1ll];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteAccumulator objectFromData:data withMaxDepth:16] lastObject];
    STAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMaxLongLong {
    NSNumber *n1 = [NSNumber numberWithLongLong:LONG_LONG_MAX];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteAccumulator objectFromData:data withMaxDepth:16] lastObject];
    STAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMinLongLong {
    NSNumber *n1 = [NSNumber numberWithLongLong:LONG_LONG_MIN];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteAccumulator objectFromData:data withMaxDepth:16] lastObject];
    STAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testBase64 {
    jsonlite_token token;
    token.start = (uint8_t *)"TWFu";
    token.end = token.start + 4;
    
    char *buffer = NULL;
    size_t size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 3, @"Incorrect size");
    STAssertTrue(memcmp(buffer, "Man", 3) == 0, @"Incorect decoding");    
    free(buffer);
    buffer = NULL;
    
    token.start = (uint8_t *)"SGVsbG8=";
    token.end = token.start + 8;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 5, @"Incorrect size");
    STAssertTrue(memcmp(buffer, "Hello", 5) == 0, @"Incorect decoding");
    free(buffer);
    buffer = NULL;
    
    token.start = (uint8_t *)"SGVsbG8hIQ==";
    token.end = token.start + 12;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 7, @"Incorrect size");
    STAssertTrue(memcmp(buffer, "Hello!!", 7) == 0, @"Incorect decoding");
    free(buffer);
    buffer = NULL;
    
    char img[] = "iVBORw0KGgoAAAANSUhEUgA"
    "AAAoAAAAKCAYAAACNMs+9AAAABmJLR0QA/wD/AP+gvaeTAAAAB3RJ"
    "TUUH1ggDCwMADQ4NnwAAAFVJREFUGJWNkMEJADEIBEcbSDkXUnfSg"
    "nBVeZ8LSAjiwjyEQXSFEIcHGP9oAi+H0Bymgx9MhxbFdZE2a0s9kT"
    "Zdw01ZhhYkABSwgmf1Z6r1SNyfFf4BZ+ZUExcNUQUAAAAASUVORK5"
    "CYII=";
    
    token.start = (uint8_t *)img;
    token.end = token.start + sizeof(img) - 1;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 179, @"Incorrect size");
    
    free(buffer);
    buffer = NULL;
    
    token.start = (uint8_t *)"";
    token.end = token.start + 0;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 0, @"Incorrect size");
    STAssertTrue(buffer == NULL, @"Incorect decoding");
    
    token.start = (uint8_t *)"=";
    token.end = token.start + 1;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 0, @"Incorrect size");
    STAssertTrue(buffer == NULL, @"Incorect decoding");
    
    token.start = (uint8_t *)" ";
    token.end = token.start + 1;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 0, @"Incorrect size");
    STAssertTrue(buffer == NULL, @"Incorect decoding");
    
    token.start = (uint8_t *)"}";
    token.end = token.start + 1;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 0, @"Incorrect size");
    STAssertTrue(buffer == NULL, @"Incorect decoding");
    
    token.start = (uint8_t *)"TWF";
    token.end = token.start + 3;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 0, @"Incorrect size");
    STAssertTrue(buffer == NULL, @"Incorect decoding");
    
    token.start = (uint8_t *)"TWF{";
    token.end = token.start + 4;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 0, @"Incorrect size");
    STAssertTrue(buffer == NULL, @"Incorect decoding");
    
    token.start = (uint8_t *)"TWF ";
    token.end = token.start + 4;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 0, @"Incorrect size");
    STAssertTrue(buffer == NULL, @"Incorect decoding");
    
    token.start = (uint8_t *)"SGVsbG8===";
    token.end = token.start + 10;
    size = jsonlite_token_base64_to_binary(&token, (void **)&buffer);
    STAssertTrue(size == 0, @"Incorrect size");
    STAssertTrue(buffer == NULL, @"Incorect decoding");
}

@end
