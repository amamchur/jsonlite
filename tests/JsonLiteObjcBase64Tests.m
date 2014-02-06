//
//  JsonLiteObjCBase64Tests.m
//  JsonLiteObjC
//
//  Created by Andrii Mamchur on 11/14/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteObjCBase64Tests.h"
#include "jsonlite.h"
#include "JsonLiteParser.h"
#include "JsonLiteDeserializer.h"
#include "JsonLiteSerializer.h"
#include "JsonLiteConverters.h"

static char img[] = "iVBORw0KGgoAAAANSUhEUgA"
"AAAoAAAAKCAYAAACNMs+9AAAABmJLR0QA/wD/AP+gvaeTAAAAB3RJ"
"TUUH1ggDCwMADQ4NnwAAAFVJREFUGJWNkMEJADEIBEcbSDkXUnfSg"
"nBVeZ8LSAjiwjyEQXSFEIcHGP9oAi+H0Bymgx9MhxbFdZE2a0s9kT"
"Zdw01ZhhYkABSwgmf1Z6r1SNyfFf4BZ+ZUExcNUQUAAAAASUVORK5"
"CYII=";

static char img_scaped[] = "iVBORw0KGgoAAAANSUhEUgA"
"AAAoAAAAKCAYAAACNMs+9AAAABmJLR0QA\\/wD\\/AP+gvaeTAAAAB3RJ"
"TUUH1ggDCwMADQ4NnwAAAFVJREFUGJWNkMEJADEIBEcbSDkXUnfSg"
"nBVeZ8LSAjiwjyEQXSFEIcHGP9oAi+H0Bymgx9MhxbFdZE2a0s9kT"
"Zdw01ZhhYkABSwgmf1Z6r1SNyfFf4BZ+ZUExcNUQUAAAAASUVORK5"
"CYII=";

static uint8_t img_bytes[] = {
    0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a, 0x00, 0x00, 0x00, 0x0d, 0x49, 0x48, 0x44, 0x52,
    0x00, 0x00, 0x00, 0x0a, 0x00, 0x00, 0x00, 0x0a, 0x08, 0x06, 0x00, 0x00, 0x00, 0x8d, 0x32, 0xcf,
    0xbd, 0x00, 0x00, 0x00, 0x06, 0x62, 0x4b, 0x47, 0x44, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0xa0,
    0xbd, 0xa7, 0x93, 0x00, 0x00, 0x00, 0x07, 0x74, 0x49, 0x4d, 0x45, 0x07, 0xd6, 0x08, 0x03, 0x0b,
    0x03, 0x00, 0x0d, 0x0e, 0x0d, 0x9f, 0x00, 0x00, 0x00, 0x55, 0x49, 0x44, 0x41, 0x54, 0x18, 0x95,
    0x8d, 0x90, 0xc1, 0x09, 0x00, 0x31, 0x08, 0x04, 0x47, 0x1b, 0x48, 0x39, 0x17, 0x52, 0x77, 0xd2,
    0x82, 0x70, 0x55, 0x79, 0x9f, 0x0b, 0x48, 0x08, 0xe2, 0xc2, 0x3c, 0x84, 0x41, 0x74, 0x85, 0x10,
    0x87, 0x07, 0x18, 0xff, 0x68, 0x02, 0x2f, 0x87, 0xd0, 0x1c, 0xa6, 0x83, 0x1f, 0x4c, 0x87, 0x16,
    0xc5, 0x75, 0x91, 0x36, 0x6b, 0x4b, 0x3d, 0x91, 0x36, 0x5d, 0xc3, 0x4d, 0x59, 0x86, 0x16, 0x24,
    0x00, 0x14, 0xb0, 0x82, 0x67, 0xf5, 0x67, 0xaa, 0xf5, 0x48, 0xdc, 0x9f, 0x15, 0xfe, 0x01, 0x67,
    0xe6, 0x54, 0x13, 0x17, 0x0d, 0x51, 0x05, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4e, 0x44, 0xae,
    0x42, 0x60, 0x82
};

@interface JsonLiteBase64Model : NSObject {
    NSData *base64;
    NSNumber *number;
}

@property (copy, nonatomic) NSData *base64;
@property (copy, nonatomic) NSNumber *number;

@end

@implementation JsonLiteBase64Model

@synthesize base64;
@synthesize number;

- (void)dealloc {
    self.base64 = nil;
    self.number = nil;
    [super dealloc];
}

@end

@implementation JsonLiteObjCBase64Tests

- (void)testBase64IncorrectInputs {
    jsonlite_token token;
    char buffer[256];
    size_t size = 0;
    
    token.start = (uint8_t *)"";
    token.end = token.start + 0;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 0, @"Incorrect size");
    
    token.start = (uint8_t *)"=";
    token.end = token.start + 1;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 0, @"Incorrect size");
    
    token.start = (uint8_t *)" ";
    token.end = token.start + 1;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 0, @"Incorrect size");
    
    token.start = (uint8_t *)"}";
    token.end = token.start + 1;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 0, @"Incorrect size");
    
    token.start = (uint8_t *)"TWF";
    token.end = token.start + 3;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 0, @"Incorrect size");
    
    token.start = (uint8_t *)"TWF{";
    token.end = token.start + 4;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 0, @"Incorrect size");
    
    token.start = (uint8_t *)"TWF ";
    token.end = token.start + 4;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 0, @"Incorrect size");
    
    token.start = (uint8_t *)"SGVsbG8===";
    token.end = token.start + 10;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 0, @"Incorrect size");
    
    token.start = (uint8_t *)"aaaa\\ba";
    token.end = token.start + 8;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 0, @"Incorrect size");
}

- (void)testBase64Decoding {
    char buffer[256];
    jsonlite_token token;
    token.start = (uint8_t *)"TWFu";
    token.end = token.start + 4;
    
    size_t size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 3, @"Incorrect size");
    XCTAssertTrue(memcmp(buffer, "Man", 3) == 0, @"Incorect decoding");
    
    token.start = (uint8_t *)"SGVsbG8=";
    token.end = token.start + 8;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 5, @"Incorrect size");
    XCTAssertTrue(memcmp(buffer, "Hello", 5) == 0, @"Incorect decoding");
    
    token.start = (uint8_t *)"SGVsbG8hIQ==";
    token.end = token.start + 12;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == 7, @"Incorrect size");
    XCTAssertTrue(memcmp(buffer, "Hello!!", 7) == 0, @"Incorect decoding");
    
    token.start = (uint8_t *)img;
    token.end = token.start + sizeof(img) - 1;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == sizeof(img_bytes), @"Incorrect size");
    XCTAssertTrue(memcmp(buffer, img_bytes, sizeof(img_bytes)) == 0, @"Incorect decoding");

    token.start = (uint8_t *)img_scaped;
    token.end = token.start + sizeof(img_scaped) - 1;
    size = jsonlite_token_base64_to_binary(&token, buffer);
    XCTAssertTrue(size == sizeof(img_bytes), @"Incorrect size");
    XCTAssertTrue(memcmp(buffer, img_bytes, sizeof(img_bytes)) == 0, @"Incorect decoding");
}

- (void)testBase64Man {
    char builder_memory[jsonlite_builder_estimate_size(32)];
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x100);
    jsonlite_builder builder = jsonlite_builder_init_memory(builder_memory, sizeof(builder_memory), stream);
    jsonlite_result result = jsonlite_builder_object_begin(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_key(builder, "base64", 6);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_base64_value(builder, "Man", sizeof("Man") - 1);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_object_end(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    uint8_t *json = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &json, 1);
    XCTAssertTrue(json != NULL, @"Json is null");
    XCTAssertTrue(size != 0, @"Size is zero");
    json[size] = 0;
    
    char *pos =  strstr((char *)json, "TWFu");
    XCTAssertTrue(pos != NULL, @"Encoding fails");
    
    jsonlite_stream_release(stream);
    free(json);
}

- (void)testBase64Hello {
    char builder_memory[jsonlite_builder_estimate_size(32)];
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x100);
    jsonlite_builder builder = jsonlite_builder_init_memory(builder_memory, sizeof(builder_memory), stream);
    jsonlite_result result = jsonlite_builder_array_begin(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_base64_value(builder, "Hello", sizeof("Hello") - 1);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_array_end(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    uint8_t *json = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &json, 1);
    XCTAssertTrue(json != NULL, @"Json is null");
    XCTAssertTrue(size != 0, @"Size is zero");
    json[size] = 0;
    
    char *pos =  strstr((char *)json, "SGVsbG8=");
    XCTAssertTrue(pos != NULL, @"Encoding fails");
    
    jsonlite_stream_release(stream);
    free(json);
}

- (void)testBase64Hello1 {
    char builder_memory[jsonlite_builder_estimate_size(32)];
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x100);
    jsonlite_builder builder = jsonlite_builder_init_memory(builder_memory, sizeof(builder_memory), stream);
    jsonlite_result result = jsonlite_builder_array_begin(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_base64_value(builder, "Hello!!", sizeof("Hello!!") - 1);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_array_end(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    uint8_t *json = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &json, 1);
    XCTAssertTrue(json != NULL, @"Json is null");
    XCTAssertTrue(size != 0, @"Size is zero");
    json[size] = 0;
    
    char *pos =  strstr((char *)json, "SGVsbG8hIQ==");
    XCTAssertTrue(pos != NULL, @"Encoding fails");
    
    jsonlite_stream_release(stream);
    free(json);
}

- (void)testBase64 {
    char builder_memory[jsonlite_builder_estimate_size(32)];
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x100);
    jsonlite_builder builder = jsonlite_builder_init_memory(builder_memory, sizeof(builder_memory), stream);
    jsonlite_result result = jsonlite_builder_object_begin(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_key(builder, "base64", 6);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_base64_value(builder, img_bytes, sizeof(img_bytes));
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_object_end(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    uint8_t *json = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &json, 1);
    XCTAssertTrue(json != NULL, @"Json is null");
    XCTAssertTrue(size != 0, @"Size is zero");
    json[size] = 0;
    
    char *pos =  strstr((char *)json, img);
    XCTAssertTrue(pos != NULL, @"Encoding fails");
    
    jsonlite_stream_release(stream);
    free(json);
}

- (void)testBase64Deserialization {
    char builder_memory[jsonlite_builder_estimate_size(32)];
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x100);
    jsonlite_builder builder = jsonlite_builder_init_memory(builder_memory, sizeof(builder_memory), stream);
    jsonlite_result result = jsonlite_builder_object_begin(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");

    result = jsonlite_builder_key(builder, "number", 6);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");

    result = jsonlite_builder_int(builder, 12345);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_key(builder, "base64", 6);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_base64_value(builder, img_bytes, sizeof(img_bytes));
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_object_end(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    uint8_t *json = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &json, 0);
    XCTAssertTrue(json != NULL, @"Json is null");
    XCTAssertTrue(size != 0, @"Size is zero");
    
    jsonlite_stream_release(stream);
    
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:16];
    JsonLiteDeserializer *des = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteBase64Model class]];
    parser.delegate = des;
    des.converter = [[[JsonLiteBase64 alloc] init] autorelease];
    [parser parse:json length:size];
    JsonLiteBase64Model *model = [des object];
    XCTAssertTrue(model.base64 != nil, @"base64 is nil");
    
    NSData *originalData = [NSData dataWithBytes:img_bytes length:sizeof(img_bytes)];
    XCTAssertEqualObjects(model.base64, originalData, @"Object are not equals");
    
    free(json);
}

- (void)testBase64DeserializationError {
    char builder_memory[jsonlite_builder_estimate_size(32)];
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x100);
    jsonlite_builder builder = jsonlite_builder_init_memory(builder_memory, sizeof(builder_memory), stream);
    jsonlite_result result = jsonlite_builder_object_begin(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_key(builder, "base64", 6);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_raw_string(builder, "SGVsbG8===", strlen("SGVsbG8==="));
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_object_end(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    uint8_t *json = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &json, 0);
    XCTAssertTrue(json != NULL, @"Json is null");
    XCTAssertTrue(size != 0, @"Size is zero");
    
    jsonlite_stream_release(stream);
    
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:16];
    JsonLiteDeserializer *des = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteBase64Model class]];
    parser.delegate = des;
    des.converter = [[[JsonLiteBase64 alloc] init] autorelease];
    [parser parse:json length:size];
    JsonLiteBase64Model *model = [des object];
    XCTAssertTrue(model.base64 == nil, @"base64 is not nil");

    free(json);
}

- (void)testBase64DeserializationBadTokenType {
    char builder_memory[jsonlite_builder_estimate_size(32)];
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x100);
    jsonlite_builder builder = jsonlite_builder_init_memory(builder_memory, sizeof(builder_memory), stream);
    jsonlite_result result = jsonlite_builder_object_begin(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_key(builder, "base64", 6);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_int(builder, 321654);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    result = jsonlite_builder_object_end(builder);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
    
    uint8_t *json = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &json, 0);
    XCTAssertTrue(json != NULL, @"Json is null");
    XCTAssertTrue(size != 0, @"Size is zero");
    
    jsonlite_stream_release(stream);
    
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:16];
    JsonLiteDeserializer *des = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteBase64Model class]];
    parser.delegate = des;
    des.converter = [[[JsonLiteBase64 alloc] init] autorelease];
    [parser parse:json length:size];
    JsonLiteBase64Model *model = [des object];
    XCTAssertTrue(model.base64 == nil, @"base64 is not nil");
    
    free(json);
}

- (void)testBase64Serialization {
    JsonLiteBase64Model *original = [[[JsonLiteBase64Model alloc] init] autorelease];
    original.base64 = [NSData dataWithBytes:img_bytes length:sizeof(img_bytes)];
    original.number = [NSNumber numberWithInt:1234];
    
    JsonLiteSerializer *ser = [JsonLiteSerializer serializer];
    ser.converter = [[[JsonLiteBase64 alloc] init] autorelease];
    
    NSData *json = [ser serializeObject:original];
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:16];
    JsonLiteDeserializer *des = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteBase64Model class]];
    parser.delegate = des;
    des.converter = [[[JsonLiteBase64 alloc] init] autorelease];
    [parser parse:json];
    
    JsonLiteBase64Model *model = [des object];
    XCTAssertEqualObjects(model.base64, original.base64, @"Object are not equals");
    XCTAssertEqualObjects(model.number, original.number, @"Object are not equals");
}

@end
