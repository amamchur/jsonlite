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

@implementation JsonLiteObjCBadToken

- (void)testBadTrueToken {
    NSError *error = [self parseErrorFromFile:@"bad_true_token" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadFalseToken {
    NSError *error = [self parseErrorFromFile:@"bad_false_token" inDir:@"bad_token"];
    STAssertNotNil(error, @"Error is nil");
    STAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Incorrect error");
}

- (void)testBadNullToken {
    NSError *error = [self parseErrorFromFile:@"bad_null_token" inDir:@"bad_token"];
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

@end
