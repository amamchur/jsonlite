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
#import "JsonLiteObjC/JsonLiteObjC.h"

@interface Model : NSObject

@property (nonatomic, copy) NSString *string;
@property (nonatomic, copy) NSNumber *number;
@property (nonatomic, copy) NSArray *array;

@end

@implementation Model

- (void)dealloc {
    self.string = nil;
    self.number = nil;
    self.array = nil;
    [super dealloc];
}

@end

int main(int argc, const char * argv[]) {
    @autoreleasepool {
        NSString *json = @"{\"string\": \"hello\", \"number\" : 100, \"array\": [1, 2, 3]}";
        NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
        JsonLiteParser *parser = [JsonLiteParser parserWithDepth:8];
        JsonLiteDeserializer *des = [JsonLiteDeserializer deserializerWithRootClass:[Model class]];
        parser.delegate = des;
        [parser parse:data];
        
        Model *model = [des object];
        NSLog(@"String - %@", model.string);
        NSLog(@"Number - %@", model.number);
        NSLog(@"Array - %@", model.array);
        
        model.string = @"Hello World";
        model.number = [NSNumber numberWithInt:256];
        model.array = [NSArray arrayWithObjects:@"Test", [NSNull null], nil];
        JsonLiteSerializer *serializer = [JsonLiteSerializer serializer];
        data = [serializer serializeObject:model];
        json = [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
        NSLog(@"%@", json);     
    }
    return 0;
}
