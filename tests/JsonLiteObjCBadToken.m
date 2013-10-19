//
//  JsonLiteObjCBadToken.m
//  JsonLiteObjC
//
//  Created by admin on 4/27/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteObjCBadToken.h"
#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteParser.h"
#import "jsonlite.h"

static int noncharacters_count = 0;

static void check_noncharacters(jsonlite_callback_context *ctx, jsonlite_token *t) {
    if ((t->string_type & jsonlite_string_unicode_noncharacter) != 0) {
        noncharacters_count++;
    }
}

@implementation JsonLiteObjCBadToken

- (void)testBadTrueToken {
    NSError *error = [self parseErrorFromFile:@"bad_true_token" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadTrueToken2 {
    NSError *error = [self parseErrorFromFile:@"bad_true_token_2" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadTrueToken3 {
    NSError *error = [self parseErrorFromFile:@"bad_true_token_3" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadFalseToken {
    NSError *error = [self parseErrorFromFile:@"bad_false_token" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadFalseToken2 {
    NSError *error = [self parseErrorFromFile:@"bad_false_token_2" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadFalseToken3 {
    NSError *error = [self parseErrorFromFile:@"bad_false_token_3" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadFalseToken4 {
    NSError *error = [self parseErrorFromFile:@"bad_false_token_4" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadNullToken {
    NSError *error = [self parseErrorFromFile:@"bad_null_token" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadNullToken2 {
    NSError *error = [self parseErrorFromFile:@"bad_null_token_2" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadNullToken3 {
    NSError *error = [self parseErrorFromFile:@"bad_null_token_3" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadIntToken {
    NSError *error = [self parseErrorFromFile:@"bad_int_token" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Incorrect error");
}

- (void)testBadIntToken2 {
    NSError *error = [self parseErrorFromFile:@"bad_int_token2" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Incorrect error");
}

- (void)testBadIntToken3 {
    NSError *error = [self parseErrorFromFile:@"bad_int_token3" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Incorrect error");
}

- (void)testBadIntToken4 {
    NSError *error = [self parseErrorFromFile:@"bad_int_token4" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Incorrect error");
}

- (void)testBadOctToken {
    NSError *error = [self parseErrorFromFile:@"bad_oct_token" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Incorrect error");
}

- (void)testBadFractionToken {
    NSError *error = [self parseErrorFromFile:@"bad_fraction_token" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Incorrect error");
}

- (void)testBadUnicodeNonchar {
    NSData *data = [self dataFromFile:@"unicode_noncharacter" inDir:@"bad_token"];
    STAssertNotNil(data, @"Data is nil");
    noncharacters_count = 0;
    jsonlite_parser parser = jsonlite_parser_init(4);
    jsonlite_parser_callbacks c = jsonlite_default_callbacks;
    c.key_found = &check_noncharacters;
    c.string_found = &check_noncharacters;
    jsonlite_parser_set_callback(parser, &c);
    jsonlite_result result = jsonlite_parser_tokenize(parser, [data bytes], [data length]);
    STAssertTrue(result == jsonlite_result_ok, @"Incorrect error");
    STAssertTrue(noncharacters_count == 1, @"Incorrect noncharacters_count");
    jsonlite_parser_release(parser);
}

- (void)testBadUnicodeNonchar1 {
    NSData *data = [self dataFromFile:@"unicode_noncharacter_1" inDir:@"bad_token"];
    STAssertNotNil(data, @"Data is nil");
    noncharacters_count = 0;
    jsonlite_parser parser = jsonlite_parser_init(4);
    jsonlite_parser_callbacks c = jsonlite_default_callbacks;
    c.key_found = &check_noncharacters;
    c.string_found = &check_noncharacters;
    jsonlite_parser_set_callback(parser, &c);
    jsonlite_result result = jsonlite_parser_tokenize(parser, [data bytes], [data length]);
    STAssertTrue(result == jsonlite_result_ok, @"Incorrect error");
    STAssertTrue(noncharacters_count == 1, @"Incorrect noncharacters_count");
    jsonlite_parser_release(parser);
}

- (void)testBadUnicodeNonchar2 {
    NSData *data = [self dataFromFile:@"unicode_noncharacter_2" inDir:@"bad_token"];
    STAssertNotNil(data, @"Data is nil");
    noncharacters_count = 0;
    jsonlite_parser parser = jsonlite_parser_init(4);
    jsonlite_parser_callbacks c = jsonlite_default_callbacks;
    c.key_found = &check_noncharacters;
    c.string_found = &check_noncharacters;
    jsonlite_parser_set_callback(parser, &c);
    jsonlite_result result = jsonlite_parser_tokenize(parser, [data bytes], [data length]);
    STAssertTrue(result == jsonlite_result_ok, @"Incorrect error");
    STAssertTrue(noncharacters_count == 1, @"Incorrect noncharacters_count");
    jsonlite_parser_release(parser);
}

- (void)testBadUnicodeNonchar3 {
    char data[] = "[\"????\"]";
    data[2] = 0xF0;
    data[3] = 0xBF;
    data[4] = 0xBF;
    data[5] = 0xBF;
    
    noncharacters_count = 0;
    jsonlite_parser parser = jsonlite_parser_init(4);
    jsonlite_parser_callbacks c = jsonlite_default_callbacks;
    c.key_found = &check_noncharacters;
    c.string_found = &check_noncharacters;
    jsonlite_parser_set_callback(parser, &c);
    jsonlite_result result = jsonlite_parser_tokenize(parser, data, sizeof(data));
    STAssertTrue(result == jsonlite_result_ok, @"Incorrect error");
    STAssertTrue(noncharacters_count == 1, @"Incorrect noncharacters_count");
    jsonlite_parser_release(parser);
}

- (void)testBadUnicodeNonchar4 {
    char data[] = "[\"???\"]";
    data[2] = 0xEF;
    data[3] = 0xB7;
    data[4] = 0x90;
    
    noncharacters_count = 0;
    jsonlite_parser parser = jsonlite_parser_init(4);
    jsonlite_parser_callbacks c = jsonlite_default_callbacks;
    c.key_found = &check_noncharacters;
    c.string_found = &check_noncharacters;
    jsonlite_parser_set_callback(parser, &c);
    jsonlite_result result = jsonlite_parser_tokenize(parser, data, sizeof(data));
    STAssertTrue(result == jsonlite_result_ok, @"Incorrect error");
    STAssertTrue(noncharacters_count == 1, @"Incorrect noncharacters_count");
    jsonlite_parser_release(parser);
}

@end
