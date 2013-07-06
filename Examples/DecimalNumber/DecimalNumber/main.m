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

#import <Foundation/Foundation.h>
#import <Foundation/Foundation.h>
#import "JsonLiteParser.h"
#import "JsonLiteDeserializer.h"
#import "JsonLiteConverters.h"

@interface Model : NSObject

@property (nonatomic, copy) NSString *string;
@property (nonatomic, copy) NSDecimalNumber *number;

@end

@implementation Model

- (void)dealloc {
    self.string = nil;
    self.number = nil;
    [super dealloc];
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSString *json = @"{\"string\": \"This is Decimal!\", \"number\" : 95673465936453649563978.99 }";
        NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
        JsonLiteParser *parser = [JsonLiteParser parserWithDepth:8];
        JsonLiteDeserializer *des = [JsonLiteDeserializer deserializerWithRootClass:[Model class]];
        des.converter = [[[JsonLiteDecimal alloc] init] autorelease];
        parser.delegate = des;
        [parser parse:data];
        
        Model *model = [des object];
        NSLog(@"String - %@", model.string);
        NSLog(@"Number - %@", model.number);
    }
    return 0;
}

