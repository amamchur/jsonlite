//
//  JsonLiteObjCAcc.m
//  JsonLiteObjC
//
//  Created by admin on 6/22/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteObjCAcc.h"
#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteAccumulator.h"

@implementation JsonLiteObjCAcc

- (void)testCheckCapacityForTrueTokens {
    NSMutableString *json = [NSMutableString stringWithString:@"[true"];
    for (int i = 0; i < 1000; i++) {
        [json appendString:@",true"];
    }
    [json appendString:@"]"];
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:4];
    STAssertNotNil(obj, @"Object is nil");
    STAssertTrue([obj count] == 1001, @"Incorrect count");
}

- (void)testCheckCapacityForFalseTokens {
    NSMutableString *json = [NSMutableString stringWithString:@"[false"];
    for (int i = 0; i < 1000; i++) {
        [json appendString:@",false"];
    }
    [json appendString:@"]"];
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:4];
    STAssertNotNil(obj, @"Object is nil");
    STAssertTrue([obj count] == 1001, @"Incorrect count");
}

- (void)testCheckCapacityForNullTokens {
    NSMutableString *json = [NSMutableString stringWithString:@"[null"];
    for (int i = 0; i < 1000; i++) {
        [json appendString:@",null"];
    }
    [json appendString:@"]"];
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:4];
    STAssertNotNil(obj, @"Object is nil");
    STAssertTrue([obj count] == 1001, @"Incorrect count");
}

- (void)testDifferentKeys {
    NSMutableString *json = [NSMutableString stringWithString:@"{\"Start\":null"];
    for (int i = 0; i < 1000; i++) {
        [json appendFormat:@",\"%d\"", i];
        [json appendString:@":null"];
    }
    [json appendString:@"}"];
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:4];
    STAssertNotNil(obj, @"Object is nil");
    STAssertTrue([obj count] == 1001, @"Incorrect count");
}

- (void)testDifferentStrings {
    NSMutableString *json = [NSMutableString stringWithString:@"[\"\""];
    for (int i = 0; i < 1000; i++) {
        [json appendFormat:@",\"%d\"", i];
    }
    [json appendString:@"]"];
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:4];
    STAssertNotNil(obj, @"Object is nil");
    STAssertTrue([obj count] == 1001, @"Incorrect count");
}

- (void)testDifferentNumbers {
    NSMutableString *json = [NSMutableString stringWithString:@"[-1"];
    for (int i = 0; i < 1000; i++) {
        [json appendFormat:@",%d", i];
    }
    [json appendString:@"]"];
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:4];
    STAssertNotNil(obj, @"Object is nil");
    STAssertTrue([obj count] == 1001, @"Incorrect count");
}

@end
