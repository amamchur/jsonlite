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
#import "jsonlite_token.h"
#import "JsonLiteSenTestCaseExt.h"

#include "jsonlite.h"
#include "jsonlite_hash.h"

@implementation JsonLiteObjCTokenTests

- (void)testTokenFunctions {
    char token_string = "abc";
    jsonlite_token token;
    token.start = token_string;
    token.end = token_string + sizeof(token_string) - 1;
    
    uint8_t *uft8_buffer = NULL;
    uint16_t *uft16_buffer = NULL;
    size_t size = jsonlite_token_decode_size_for_uft8(NULL);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_to_uft8(NULL, NULL);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_to_uft8(NULL, &uft8_buffer);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_to_uft8(&token, NULL);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_size_for_uft16(NULL);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_to_uft16(NULL, NULL);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_to_uft16(NULL, &uft16_buffer);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_to_uft16(&token, NULL);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_hex_char_to_uint8('Q');
    STAssertTrue(size == 0xFF, @"Is not 0XFF");

    size = jsonlite_hex_char_to_uint8('7');
    STAssertTrue(size == 7, @"Is not 7");
    
    size = jsonlite_hex_char_to_uint8('a');
    STAssertTrue(size == 10, @"Is not 10");
    
    size = jsonlite_hex_char_to_uint8('A');
    STAssertTrue(size == 10, @"Is not 10");
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

@end
