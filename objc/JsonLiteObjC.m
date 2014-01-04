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

#import "JsonLiteObjC.h"

@implementation JsonLiteObjC

+ (id)objectFromData:(const void *)data
              length:(NSUInteger)length
               depth:(NSUInteger)depth
               error:(NSError **)error {
    JsonLiteAccumulator *acc = [[JsonLiteAccumulator alloc] initWithDepth:depth];
    JsonLiteParser *parser = [[JsonLiteParser alloc] initWithDepth:depth];
    parser.delegate = acc;
    [parser parse:data length:length];
    
    id obj = [acc object];
    if (error != NULL) {
        *error = parser.parseError;
    }
    
    [acc release];
    [parser release];
    return obj;
}

+ (id)objectFromData:(const void *)data length:(NSUInteger)length depth:(NSUInteger)depth {
    return [self objectFromData:data length:length depth:depth error:NULL];
}

+ (id)objectFromData:(const void *)data length:(NSUInteger)length {
    return [self objectFromData:data length:length depth:32 error:NULL];
}

+ (id)objectFromData:(NSData *)data depth:(NSUInteger)depth {
    return [self objectFromData:[data bytes] length:[data length] depth:depth error:NULL];
}

+ (id)objectFromData:(NSData *)data {
    return [self objectFromData:[data bytes] length:[data length] depth:32 error:NULL];
}

+ (id)objectFromData:(const void *)data
              length:(NSUInteger)length
             rootCls:(Class)cls
     convertersChain:(id<JsonLiteDeserializerChain>)chain
               depth:(NSUInteger)depth
               error:(NSError **)error {
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:cls];
    JsonLiteParser *parser = [[JsonLiteParser alloc] initWithDepth:depth];
    parser.delegate = deserializer;
    deserializer.converter = chain;
    [parser parse:data length:length];
    
    id obj = [deserializer object];
    if (error != NULL) {
        *error = parser.parseError;
    }
    
    [deserializer release];
    [parser release];
    return obj;
}

+ (id)objectFromData:(const void *)data
              length:(NSUInteger)length
             rootCls:(Class)cls
     convertersChain:(id<JsonLiteDeserializerChain>)chain
               depth:(NSUInteger)depth {
    return [self objectFromData:data
                         length:length
                        rootCls:cls
                convertersChain:chain
                          depth:depth
                          error:NULL];
}

+ (id)objectFromData:(NSData *)data
             rootCls:(Class)cls
     convertersChain:(id<JsonLiteDeserializerChain>)chain
               depth:(NSUInteger)depth
               error:(NSError **)error {
    return [self objectFromData:[data bytes]
                         length:[data length]
                        rootCls:cls
                convertersChain:chain
                          depth:depth
                          error:error];
}

+ (id)objectFromData:(NSData *)data
             rootCls:(Class)cls
     convertersChain:(id<JsonLiteDeserializerChain>)chain
               depth:(NSUInteger)depth {
    return [self objectFromData:[data bytes]
                         length:[data length]
                        rootCls:cls
                convertersChain:chain
                          depth:depth
                          error:NULL];
}

+ (id)objectFromData:(NSData *)data
             rootCls:(Class)cls
     convertersChain:(id<JsonLiteDeserializerChain>)chain {
    return [self objectFromData:[data bytes]
                         length:[data length]
                        rootCls:cls
                convertersChain:chain
                          depth:32
                          error:NULL];
}

+ (NSData *)dataFromObject:(id)object convertersChain:(id<JsonLiteSerializerChain>)chain {
    JsonLiteSerializer *serializer = [[JsonLiteSerializer alloc] init];
    serializer.converter = chain;
    NSData *data = [serializer serializeObject:object];
    [serializer release];
    return data;
}

+ (NSData *)dataFromObject:(id)object {
    return [self dataFromObject:object convertersChain:nil];
}

+ (NSString *)version {
    return [NSString stringWithUTF8String:jsonlite_version];
}

@end