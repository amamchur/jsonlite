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

#import <Foundation/Foundation.h>
#import <stdio.h>
#import "JsonLiteConverters.h"
#import "JsonLiteDeserializer.h"
#import "JsonLiteSerializer.h"
#import "jsonlite_token.h"

@implementation JsonLiteConverter

@synthesize nextDeserializerChain;
@synthesize nextSerializerChain;

- (BOOL)getData:(NSData **)data
       forValue:(id)value 
     serializer:(JsonLiteSerializer *)serializer {
    return [nextSerializerChain getData:data forValue:value serializer:serializer];
    
}

- (BOOL)getValue:(id *)value
         ofClass:(Class)cls
        forToken:(JsonLiteToken *)token 
    deserializer:(JsonLiteDeserializer *)deserializer {
    return [nextDeserializerChain getValue:value ofClass:cls forToken:token deserializer:deserializer];
}

- (void)dealloc {
    self.nextSerializerChain = nil;
    self.nextDeserializerChain = nil;
    [super dealloc];
}

+ (id<JsonLiteDeserializerChain, JsonLiteSerializerChain>)converters {
    JsonLiteDecimal *dec = [[[JsonLiteDecimal alloc] init] autorelease];
    JsonLiteTwitterDate *date = [[[JsonLiteTwitterDate alloc] init] autorelease];
    JsonLiteURL *url = [[[JsonLiteURL alloc] init] autorelease];
    dec.nextDeserializerChain = date;
    dec.nextSerializerChain = date;
    date.nextDeserializerChain = url;
    date.nextSerializerChain = url;
    return dec;
}

@end

@implementation JsonLiteDecimal

- (BOOL)getData:(NSData **)data 
       forValue:(id)value 
     serializer:(JsonLiteSerializer *)serializer {
    if ([value isKindOfClass:[NSDecimalNumber class]]) {
        NSString *str = [value stringValue];
        *data = [str dataUsingEncoding:NSUTF8StringEncoding]; 
        return YES;
    }
    return [nextSerializerChain getData:data forValue:value serializer:serializer];
    
}

- (BOOL)getValue:(id *)value
         ofClass:(Class)cls
        forToken:(JsonLiteToken *)token 
    deserializer:(JsonLiteDeserializer *)deserializer {
    if ([cls isSubclassOfClass:[NSDecimalNumber class]] && [token isKindOfClass:[JsonLiteNumberToken class]]) {
        *value = [(JsonLiteNumberToken *)token decimal]; 
        return YES;
    }
    return [nextDeserializerChain getValue:value ofClass:cls forToken:token deserializer:deserializer];
}

@end

@implementation JsonLiteURL

- (BOOL)getData:(NSData **)data 
       forValue:(id)value 
     serializer:(JsonLiteSerializer *)serializer {
    if ([value isKindOfClass:[NSURL class]]) {
        NSString *str = [[NSString alloc] initWithFormat:@"\"%@\"", [value absoluteString]];
        *data = [str dataUsingEncoding:NSUTF8StringEncoding];
        [str release];
        return YES;
    }
    return [nextSerializerChain getData:data forValue:value serializer:serializer];    
}

- (BOOL)getValue:(id *)value
         ofClass:(Class)cls
        forToken:(JsonLiteToken *)token 
    deserializer:(JsonLiteDeserializer *)deserializer {
    if ([cls isSubclassOfClass:[NSURL class]] && [token isKindOfClass:[JsonLiteStringToken class]]) {
        jsonlite_token *t = (jsonlite_token *)token;
        uint8_t *buffer = NULL;
        size_t size = jsonlite_token_decode_to_uft8(t, &buffer);
        NSURL *url = (NSURL *)CFURLCreateWithBytes(NULL, (const UInt8 *)buffer, size, kCFStringEncodingUTF8, NULL);
        *value = [url autorelease];
        free(buffer);
        return YES;
    }
    return [nextDeserializerChain getValue:value ofClass:cls forToken:token deserializer:deserializer];
}

@end

@implementation JsonLiteTwitterDate

- (id)init {
    self = [super init];
    if (self != nil) {
        formatter = [[NSDateFormatter alloc] init];
        [formatter setDateFormat:@"EEE MMM d H:m:s Z Y"];
    }
    return self;
}

- (void)dealloc {
    [formatter release];
    [super dealloc];
}

- (BOOL)getData:(NSData **)data 
       forValue:(id)value 
     serializer:(JsonLiteSerializer *)serializer {
    if ([value isKindOfClass:[NSDate class]]) {
        NSString *str = [[NSString alloc] initWithFormat:@"\"%@\"", [formatter stringFromDate:value]];
        *data = [str dataUsingEncoding:NSUTF8StringEncoding];
        [str release];
        return YES;
    }
    return [nextSerializerChain getData:data forValue:value serializer:serializer];
    
}

- (BOOL)getValue:(id *)value
         ofClass:(Class)cls
        forToken:(JsonLiteToken *)token 
    deserializer:(JsonLiteDeserializer *)deserializer {
    if ([cls isSubclassOfClass:[NSDate class]] && [token isKindOfClass:[JsonLiteStringToken class]]) {
        JsonLiteStringToken *t = (JsonLiteStringToken *)token;
        NSString *str = [t allocNoCopyValue];
        NSRange range = NSMakeRange(0, [str length]);
        [formatter getObjectValue:value 
                                   forString:str 
                                       range:&range 
                                       error:nil];
        [str release];
        return YES;
    }
    return [nextDeserializerChain getValue:value ofClass:cls forToken:token deserializer:deserializer];
}

@end
