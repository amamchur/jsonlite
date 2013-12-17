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
    XCTAssertNotNil(obj, @"Object is nil");
    XCTAssertTrue([obj count] == 1001, @"Incorrect count");
}

- (void)testCheckCapacityForFalseTokens {
    NSMutableString *json = [NSMutableString stringWithString:@"[false"];
    for (int i = 0; i < 1000; i++) {
        [json appendString:@",false"];
    }
    [json appendString:@"]"];
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:4];
    XCTAssertNotNil(obj, @"Object is nil");
    XCTAssertTrue([obj count] == 1001, @"Incorrect count");
}

- (void)testCheckCapacityForNullTokens {
    NSMutableString *json = [NSMutableString stringWithString:@"[null"];
    for (int i = 0; i < 1000; i++) {
        [json appendString:@",null"];
    }
    [json appendString:@"]"];
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:4];
    XCTAssertNotNil(obj, @"Object is nil");
    XCTAssertTrue([obj count] == 1001, @"Incorrect count");
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
    XCTAssertNotNil(obj, @"Object is nil");
    XCTAssertTrue([obj count] == 1001, @"Incorrect count");
}

- (void)testDifferentStrings {
    NSMutableString *json = [NSMutableString stringWithString:@"[\"\""];
    for (int i = 0; i < 1000; i++) {
        [json appendFormat:@",\"%d\"", i];
    }
    [json appendString:@"]"];
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:4];
    XCTAssertNotNil(obj, @"Object is nil");
    XCTAssertTrue([obj count] == 1001, @"Incorrect count");
}

- (void)testDifferentNumbers {
    NSMutableString *json = [NSMutableString stringWithString:@"[-1"];
    for (int i = 0; i < 1000; i++) {
        [json appendFormat:@",%d", i];
    }
    [json appendString:@"]"];
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id obj = [JsonLiteAccumulator objectFromData:data withMaxDepth:4];
    XCTAssertNotNil(obj, @"Object is nil");
    XCTAssertTrue([obj count] == 1001, @"Incorrect count");
}

- (void)testChunks {
    char json1[] = "[{\"key1\" : 1},{\"key2";
    char json2[] = "\" : 2},{\"key3\" : 3},{\"key";
    char json3[] = "4\" : 4}]";
    
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:32];
    JsonLiteAccumulator *acc = [JsonLiteAccumulator accumulatorWithDepth:32];
    parser.delegate = acc;
    [parser parse:[NSData dataWithBytes:json1 length:sizeof(json1) - 1]];
    [parser parse:[NSData dataWithBytes:json2 length:sizeof(json2) - 1]];
    [parser parse:[NSData dataWithBytes:json3 length:sizeof(json3) - 1]];
    
    NSMutableArray *array = [NSMutableArray array];
    [array addObject:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:1]
                                                 forKey:@"key1"]];
    [array addObject:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:2]
                                                 forKey:@"key2"]];
    [array addObject:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:3]
                                                 forKey:@"key3"]];
    [array addObject:[NSDictionary dictionaryWithObject:[NSNumber numberWithInt:4]
                                                 forKey:@"key4"]];
    XCTAssertEqualObjects(array, [acc object], @"Not equal!");
}

@end
