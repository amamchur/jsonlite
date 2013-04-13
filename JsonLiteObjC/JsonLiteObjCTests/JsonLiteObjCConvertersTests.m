//
//  Copyright 2012, Andrii Mamchur
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

@interface JsonLiteConvert : NSObject

@property (nonatomic, copy) NSString *str;
@property (nonatomic, copy) NSNumber *number;
@property (nonatomic, copy) NSURL *url;
@property (nonatomic, copy) NSDecimalNumber *decimal;
@property (nonatomic, copy) NSDate *date;

@end

@implementation JsonLiteConvert

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
    STAssertNil(error, @"Can not open file");
    
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
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteConvert *obj = [deserializer object];
    
    STAssertNotNil(obj.url, @"URL is null");
    STAssertTrue([[obj.url absoluteString] isEqual:@"http://code.google.com/p/jsonlite/"], @"Bad url");
    
    STAssertNotNil(obj.decimal, @"decimal is null");
    STAssertTrue([obj.decimal isEqual:[NSDecimalNumber decimalNumberWithString:@"123456789987654321"]], @"Bad decimal");
   
    STAssertNotNil(obj.date, @"date is null");
    
    JsonLiteSerializer *ser = [JsonLiteSerializer serializer];
    ser.converter = converter;
    
    data = [ser serializeObject:obj];
    
    [deserializer reset];
    [parser reset];
    [parser parse:data];
    JsonLiteConvert *obj1 = [deserializer object];
    
    STAssertTrue([obj.str isEqual:obj1.str], @"Not equal");
    STAssertTrue([obj.number isEqual:obj1.number], @"Not equal");
    STAssertTrue([obj.url isEqual:obj1.url], @"Not equal");
    STAssertTrue([obj.decimal isEqual:obj1.decimal], @"Not equal");
    STAssertTrue([obj.date isEqual:obj1.date], @"Not equal");
    
    [pool release];
}

@end
