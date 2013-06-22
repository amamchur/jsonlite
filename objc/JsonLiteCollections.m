//
//  JsonLiteCollections.m
//  JsonLiteObjC
//
//  Created by Andrii Mamchur on 12/10/12.
//  Copyright (c) 2012 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteCollections.h"

#import <objc/runtime.h>

typedef struct JsonLiteDictionaryBucket {
    CFHashCode hash;
    id key;
    id value;
    struct JsonLiteDictionaryBucket *next;
} JsonLiteDictionaryBucket;

@interface JsonLiteDictionaryEnumerator : NSEnumerator {
    NSInteger index;
    NSInteger count;
    JsonLiteDictionaryBucket *buffer;
}

@end

#define JsonLiteDictionaryFront     0x10
#define JsonLiteDictionaryFrontMask 0x0F

@interface JsonLiteDictionary() {
@public
    NSUInteger count;
    JsonLiteDictionaryBucket *buffer;
    JsonLiteDictionaryBucket *buckets[JsonLiteDictionaryFront];
}

@end

@interface JsonLiteArray() {
@public
    NSUInteger count;
    id *values;
}

@end

@implementation JsonLiteDictionaryEnumerator

- (id)initWithBuffer:(JsonLiteDictionaryBucket *)aBuffer count:(NSUInteger)aCount {
    self = [self init];
    if (self != nil) {
        buffer = aBuffer;
        count = aCount;
        index = 0;
    }
    return self;
}

- (NSArray *)allObjects {
    NSMutableArray *array = [NSMutableArray arrayWithCapacity:(NSUInteger)count];
    for (int i = 0; i < count; i++) {
        [array addObject:(buffer + i)->key];
    }
    return array;
}

- (id)nextObject {
    if (index < count) {
        return (buffer + index++)->key;
    }
    return nil;
}

@end

@implementation JsonLiteDictionary

- (void)dealloc {
    for (int i = 0; i < count; i++) {
        CFRelease(buffer[i].key);
        CFRelease(buffer[i].value);
    }
    [super dealloc];
}

- (NSUInteger)count {
    return count;
}

- (id)objectForKey:(id)aKey {
    CFHashCode hash = CFHash((CFTypeRef)aKey);
    int index = hash & JsonLiteDictionaryFrontMask;
    for (JsonLiteDictionaryBucket *b = buckets[index]; b != NULL; b = b->next) {
        // TODO: Create test for hash collisions
        if (b->hash == hash && [b->key isEqual:aKey]) {
            return b->value;
        }
    }
    return nil;
}

- (NSEnumerator *)keyEnumerator {
    JsonLiteDictionaryEnumerator *e = [[JsonLiteDictionaryEnumerator alloc] initWithBuffer:buffer
                                                                                     count:count];
    return [e autorelease];
}

@end

@implementation JsonLiteArray

- (NSUInteger)count  {
    return count;
}

- (id)objectAtIndex:(NSUInteger)index {
    NSException *exc = nil;
    if (index >= count) {
        id cls = NSStringFromClass([self class]);
        id sel = NSStringFromSelector(_cmd);
        NSString *str = [NSString stringWithFormat:@"*** -[%@ %@]: index (%d) beyond bounds (%d)", cls, sel, index, count];
        exc = [NSException exceptionWithName:NSRangeException
                                      reason:str
                                    userInfo:nil];
    }
    [exc raise];
    return values[index];
}

- (void)dealloc {
    for (int i = 0; i < count; i++) {
        CFRelease(values[i]);
    }
    [super dealloc];
}

@end

id JsonLiteCreateDictionary(const id *values, const id *keys, const CFHashCode *hashes, NSUInteger count) {
    static Class cls = nil;
    static size_t size = 0;
    if (cls == nil) {
        cls = [JsonLiteDictionary class];
        size = class_getInstanceSize(cls);
    }

    JsonLiteDictionary *dict = class_createInstance(cls, count * sizeof(JsonLiteDictionaryBucket));
    dict->buffer = (JsonLiteDictionaryBucket *)((uint8_t *)dict  + size);
    dict->count = count;
    
    JsonLiteDictionaryBucket *b = dict->buffer;
    for (int i = 0; i < count; i++, b++) {
        CFHashCode hash = hashes[i];
        int index = hash & JsonLiteDictionaryFrontMask;
        b->hash = hash;
        b->key = keys[i];
        b->value = values[i];
        b->next = dict->buckets[index];
        dict->buckets[index] = b;
    }
    
    return dict;
}

id JsonLiteCreateArray(const id *objects, NSUInteger count) {
    static Class cls = nil;
    static size_t size = 0;
    if (cls == nil) {
        cls = [JsonLiteArray class];
        size = class_getInstanceSize(cls);
    }

    JsonLiteArray *array = class_createInstance(cls, count * sizeof(id));
    array = [array init];
    array->values = (id *)((uint8_t *)array + size);
    array->count = count;
    if (count > 0) {
        array->values = (id *)((uint8_t *)array + size);
        memcpy(array->values, objects, sizeof(id) * count); // LCOV_EXCL_LINE
    }
    return array;
}
