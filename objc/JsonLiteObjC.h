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

#import "jsonlite.h"
#import "JsonLiteAccumulator.h"
#import "JsonLiteConverters.h"
#import "JsonLiteDeserializer.h"
#import "JsonLiteMetaData.h"
#import "JsonLiteParser.h"
#import "JsonLiteSerializer.h"

@interface JsonLiteObjC : NSObject

+ (id)objectFromData:(const void *)data
              length:(NSUInteger)length
               depth:(NSUInteger)depth
               error:(NSError **)error;
+ (id)objectFromData:(const void *)data length:(NSUInteger)length depth:(NSUInteger)depth;
+ (id)objectFromData:(const void *)data length:(NSUInteger)length;
+ (id)objectFromData:(NSData *)data depth:(NSUInteger)depth;
+ (id)objectFromData:(NSData *)data;

+ (id)objectFromData:(const void *)data
              length:(NSUInteger)length
             rootCls:(Class)cls
     convertersChain:(id<JsonLiteDeserializerChain>)chain
               depth:(NSUInteger)depth
               error:(NSError **)error;
+ (id)objectFromData:(const void *)data
              length:(NSUInteger)length
             rootCls:(Class)cls
    convertersChain:(id<JsonLiteDeserializerChain>)chain
               depth:(NSUInteger)depth;
+ (id)objectFromData:(NSData *)data
             rootCls:(Class)cls
     convertersChain:(id<JsonLiteDeserializerChain>)chain
               depth:(NSUInteger)depth
               error:(NSError **)error;
+ (id)objectFromData:(NSData *)data
             rootCls:(Class)cls
     convertersChain:(id<JsonLiteDeserializerChain>)chain
               depth:(NSUInteger)depth;
+ (id)objectFromData:(NSData *)data
             rootCls:(Class)cls
     convertersChain:(id<JsonLiteDeserializerChain>)chain;

+ (NSData *)dataFromObject:(id)object convertersChain:(id<JsonLiteSerializerChain>)chain;
+ (NSData *)dataFromObject:(id)object;

+ (NSString *)version;

@end
