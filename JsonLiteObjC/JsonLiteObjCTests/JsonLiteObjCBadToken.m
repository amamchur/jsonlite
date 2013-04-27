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

@end
