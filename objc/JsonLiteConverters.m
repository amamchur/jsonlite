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

#import "JsonLiteConverters.h"

@implementation JsonLiteConverter

@synthesize nextDeserializerChain;
@synthesize nextSerializerChain;

- (BOOL)writeConvertedValue:(id)value serializer:(JsonLiteSerializer *)serializer {
    return [nextSerializerChain writeConvertedValue:value serializer:serializer];
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
    JsonLiteEpochDateTime *epoch = [[[JsonLiteEpochDateTime alloc] init] autorelease];
    dec.nextDeserializerChain = date;
    dec.nextSerializerChain = date;
    date.nextDeserializerChain = url;
    date.nextSerializerChain = url;
    url.nextDeserializerChain = epoch;
    url.nextSerializerChain = epoch;
    return dec;
}

@end

@implementation JsonLiteDecimal

- (BOOL)writeConvertedValue:(id)value serializer:(JsonLiteSerializer *)serializer {
    if ([value isKindOfClass:[NSDecimalNumber class]]) {
        NSString *str = [value stringValue];
        const char *c_str = [str UTF8String];
        jsonlite_builder_raw_value(serializer.builder, c_str, strlen(c_str));
        return YES;
    }
    return [nextSerializerChain writeConvertedValue:value serializer:serializer];
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

- (BOOL)writeConvertedValue:(id)value serializer:(JsonLiteSerializer *)serializer {
    if ([value isKindOfClass:[NSURL class]]) {
        NSString *str = [value absoluteString];
        const char *c_str = [str UTF8String];
        jsonlite_builder_string(serializer.builder, c_str, strlen(c_str));
        return YES;
    }
    return [nextSerializerChain writeConvertedValue:value serializer:serializer];
}

- (BOOL)getValue:(id *)value
         ofClass:(Class)cls
        forToken:(JsonLiteToken *)token 
    deserializer:(JsonLiteDeserializer *)deserializer {
    if ([cls isSubclassOfClass:[NSURL class]] && [token isKindOfClass:[JsonLiteStringToken class]]) {
        NSString *str = [token value];
        NSURL *url = (NSURL *)CFURLCreateWithString(NULL, (CFStringRef)str, NULL);
        *value = [url autorelease];
        return YES;
    }
    return [nextDeserializerChain getValue:value ofClass:cls forToken:token deserializer:deserializer];
}

@end

@implementation JsonLiteEpochDateTime

- (BOOL)writeConvertedValue:(id)value serializer:(JsonLiteSerializer *)serializer {
    if ([value isKindOfClass:[NSDate class]]) {
        long long epoch = (long long)[value timeIntervalSince1970];
        jsonlite_builder_int(serializer.builder, epoch);
        return YES;
    }
    return [nextSerializerChain writeConvertedValue:value serializer:serializer];
}

- (BOOL)getValue:(id *)value
         ofClass:(Class)cls
        forToken:(JsonLiteToken *)token
    deserializer:(JsonLiteDeserializer *)deserializer {
    if ([cls isSubclassOfClass:[NSDate class]] && [token isKindOfClass:[JsonLiteNumberToken class]]) {
        NSNumber *number = [token copyValue];
        *value = [NSDate dateWithTimeIntervalSince1970:[number doubleValue]];
        [number release];
        return YES;
    }
    return [nextDeserializerChain getValue:value ofClass:cls forToken:token deserializer:deserializer];
}

@end

@implementation JsonLiteBase64

- (BOOL)writeConvertedValue:(id)value serializer:(JsonLiteSerializer *)serializer {
    if ([value isKindOfClass:[NSData class]]) {
        NSData *data = value;
        jsonlite_builder_base64_value(serializer.builder, [data bytes], [data length]);
        return YES;
    }
    return [nextSerializerChain writeConvertedValue:value serializer:serializer];
}

- (BOOL)getValue:(id *)value
         ofClass:(Class)cls
        forToken:(JsonLiteToken *)token
    deserializer:(JsonLiteDeserializer *)deserializer {
    if ([cls isSubclassOfClass:[NSData class]] && [token isKindOfClass:[JsonLiteStringToken class]]) {
        *value = [(JsonLiteStringToken *)token base64Data];
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
        [formatter setDateFormat:@"eee MMM dd HH:mm:ss ZZZZ yyyy"];
    }
    return self;
}

- (void)dealloc {
    [formatter release];
    [super dealloc];
}

- (BOOL)writeConvertedValue:(id)value serializer:(JsonLiteSerializer *)serializer {
    if ([value isKindOfClass:[NSDate class]]) {
        NSString *str = [formatter stringFromDate:value];
        const char *c_str = [str UTF8String];
        jsonlite_builder_string(serializer.builder, c_str, strlen(c_str));
        return YES;
    }
    return [nextSerializerChain writeConvertedValue:value serializer:serializer];
}

- (BOOL)getValue:(id *)value
         ofClass:(Class)cls
        forToken:(JsonLiteToken *)token 
    deserializer:(JsonLiteDeserializer *)deserializer {
    if ([cls isSubclassOfClass:[NSDate class]] && [token isKindOfClass:[JsonLiteStringToken class]]) {
        JsonLiteStringToken *t = (JsonLiteStringToken *)token;
        NSString *str = [t copyStringWithBytesNoCopy];
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
