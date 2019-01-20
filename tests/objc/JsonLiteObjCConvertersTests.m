//
//  Copyright 2012-2016, Andrii Mamchur
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


#import "JsonLiteObjCConvertersTests.h"
#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteParser.h"
#import "JsonLiteSerializer.h"
#import "JsonLiteDeserializer.h"
#import "JsonLiteConverters.h"

@interface JsonLiteConvert : NSObject {
   NSString *str;
   NSNumber *number;
   NSURL *url;
   NSDecimalNumber *decimal;
   NSDate *date;
}

@property (nonatomic, copy) NSString *str;
@property (nonatomic, copy) NSNumber *number;
@property (nonatomic, copy) NSURL *url;
@property (nonatomic, copy) NSDecimalNumber *decimal;
@property (nonatomic, copy) NSDate *date;

@end

@implementation JsonLiteConvert

@synthesize str;
@synthesize number;
@synthesize url;
@synthesize decimal;
@synthesize date;

- (void)dealloc {
    self.str = nil;
    self.number = nil;
    self.url = nil;
    self.decimal = nil;
    self.date = nil;
    [super dealloc];
}

@end

@implementation JsonLiteObjCConvertersTests

- (void)testConverters {
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"convert"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error];
    XCTAssertNil(error, @"Can not open file");
    
    JsonLiteConverter *dummy = [[[JsonLiteConverter alloc] init] autorelease];
    JsonLiteConverter *converter = [JsonLiteConverter converters];
    JsonLiteConverter *cnv = converter;
    while (cnv != nil) {
        if (cnv.nextDeserializerChain == nil || cnv.nextSerializerChain == nil) {
            cnv.nextDeserializerChain = dummy;
            cnv.nextSerializerChain = dummy;
            break;
        }
        
        cnv = (id)cnv.nextSerializerChain;
    }
    
    
    JsonLiteParser *parser = [JsonLiteParser parser];
    JsonLiteDeserializer *deserializer = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteConvert class]];
    deserializer.converter = converter;
    XCTAssertEqualObjects(deserializer.converter, converter, @"Object are not equals");
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteConvert *obj = [deserializer object];
    
    XCTAssertNotNil(obj.url, @"URL is null");
    XCTAssertTrue([[obj.url absoluteString] isEqual:@"https://github.com/amamchur/jsonlite"], @"Bad url");
    
    XCTAssertNotNil(obj.decimal, @"decimal is null");
    XCTAssertTrue([obj.decimal isEqual:[NSDecimalNumber decimalNumberWithString:@"123456789987654321"]], @"Bad decimal");
   
    XCTAssertNotNil(obj.date, @"date is null");
    
    JsonLiteSerializer *ser = [JsonLiteSerializer serializer];
    ser.converter = converter;
    XCTAssertEqualObjects(ser.converter, converter, @"converters are not equals");
    data = [ser serializeObject:obj];
    
    [deserializer reset];
    [parser reset];
    [parser parse:data];
    JsonLiteConvert *obj1 = [deserializer object];
    
    XCTAssertTrue([obj.str isEqual:obj1.str], @"Not equal");
    XCTAssertTrue([obj.number isEqual:obj1.number], @"Not equal");
    XCTAssertTrue([obj.url isEqual:obj1.url], @"Not equal");
    XCTAssertTrue([obj.decimal isEqual:obj1.decimal], @"Not equal");
    XCTAssertTrue([obj.date isEqual:obj1.date], @"Not equal");
    
    [pool release];
}

- (void)testEpochTime {    
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"convert_epoch"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                    error:&error];
    
    JsonLiteEpochDateTime *epoch = [[JsonLiteEpochDateTime alloc] init];
    epoch.nextDeserializerChain = [JsonLiteConverter converters];
    epoch.nextSerializerChain = [JsonLiteConverter converters];
    
    JsonLiteParser *parser = [JsonLiteParser parser];
    JsonLiteDeserializer *deserializer = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteConvert class]];
    parser.delegate = deserializer;
    deserializer.converter = epoch;

    [parser parse:data];
    JsonLiteConvert *obj = [deserializer object];
    
    XCTAssertNotNil(obj.url, @"URL is null");
    XCTAssertTrue([[obj.url absoluteString] isEqual:@"https://github.com/amamchur/jsonlite"], @"Bad url");
    
    XCTAssertNotNil(obj.decimal, @"decimal is null");
    XCTAssertTrue([obj.decimal isEqual:[NSDecimalNumber decimalNumberWithString:@"123456789987654321"]], @"Bad decimal");
    
    XCTAssertEqualObjects(obj.date, [NSDate dateWithTimeIntervalSince1970:1234657890], @"");
    
    JsonLiteSerializer *serializer = [JsonLiteSerializer serializer];
    serializer.converter = epoch;
    data = [serializer serializeObject:obj];
    
    [deserializer reset];
    [parser reset];
    [parser parse:data];
    JsonLiteConvert *obj1 = [deserializer object];
    
    XCTAssertTrue([obj.str isEqual:obj1.str], @"Not equal");
    XCTAssertTrue([obj.number isEqual:obj1.number], @"Not equal");
    XCTAssertTrue([obj.url isEqual:obj1.url], @"Not equal");
    XCTAssertTrue([obj.decimal isEqual:obj1.decimal], @"Not equal");
    XCTAssertTrue([obj.date isEqual:obj1.date], @"Not equal");
    
    [epoch release];
}

- (void)testEpochTimeIncorrectToken {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"convert"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                    error:&error];
    
    JsonLiteEpochDateTime *epoch = [[JsonLiteEpochDateTime alloc] init];
    epoch.nextDeserializerChain = [JsonLiteConverter converters];
    epoch.nextSerializerChain = [JsonLiteConverter converters];
    
    JsonLiteParser *parser = [JsonLiteParser parser];
    JsonLiteDeserializer *deserializer = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteConvert class]];
    parser.delegate = deserializer;
    deserializer.converter = epoch;
    
    [parser parse:data];
    JsonLiteConvert *obj = [deserializer object];
    
    XCTAssertNotNil(obj.url, @"URL is null");
    XCTAssertTrue([[obj.url absoluteString] isEqual:@"https://github.com/amamchur/jsonlite"], @"Bad url");
    
    XCTAssertNotNil(obj.decimal, @"decimal is null");
    XCTAssertTrue([obj.decimal isEqual:[NSDecimalNumber decimalNumberWithString:@"123456789987654321"]], @"Bad decimal");
    
    XCTAssertNotNil(obj.date, @"");
    
    [epoch release];
}

@end
