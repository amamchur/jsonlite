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

+ (id)allocDictionaryWithValue:(const id *)values
                          keys:(const id *)keys
                        hashes:(const CFHashCode *)hashes
                         count:(NSUInteger)cnt {
    size_t size = class_getInstanceSize(self);
    size_t totalSize = size + cnt * sizeof(JsonLiteDictionaryBucket);
    uint8_t *mem = calloc(1, totalSize);
    JsonLiteDictionary *dict = (JsonLiteDictionary *)mem;
    object_setClass(dict, self);
    dict = [dict init];
    dict->buffer = (JsonLiteDictionaryBucket *)(mem + size);
    dict->count = cnt;
    
    JsonLiteDictionaryBucket *b = dict->buffer;
    for (int i = 0; i < cnt; i++, b++) {
        CFHashCode hash = hashes[i];
        int index = hash & JsonLiteDictionaryFrontMask;
        b->hash = hash;
        b->key = keys[i];
        b->value = values[i];
        b->next = dict->buckets[index];
        dict->buckets[index] = b;
    }
    
    return (id)mem;
}

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

- (id)initWithObjects:(const id *)objects count:(NSUInteger)cnt {
    self = [super init];
    if (self != nil) {
        count = cnt;
        if (cnt > 0) {
            values = malloc(sizeof(id) * cnt);
            memcpy(values, objects, sizeof(id) * cnt);
        }
    }
    return self;
}

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
    free(values);
    [super dealloc];
}

@end
