//
//  JsonLiteCollections.m
//  JsonLiteObjC
//
//  Created by Andrii Mamchur on 12/10/12.
//  Copyright (c) 2012 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteCollections.h"

#import <objc/runtime.h>
#import <objc/message.h>

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

+ (id)enumeratorForDictionary:(JsonLiteDictionary *)dict;

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

- (id)initForDictionary:(JsonLiteDictionary *)dict {
    self = [super init];
    if (self != nil) {
        buffer = dict->buffer;
        count = dict->count;
        index = 0;
    }
    return self;
}

+ (id)enumeratorForDictionary:(JsonLiteDictionary *)dict {
    return [[[self alloc] initForDictionary:dict] autorelease];
}

- (NSArray *)allObjects {
    NSMutableArray *array = [NSMutableArray arrayWithCapacity:count];
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
    // TODO: Implement test for CFHash collision case.
    
    CFHashCode hash = CFHash((CFTypeRef)aKey);
    int index = hash & JsonLiteDictionaryFrontMask;
    for (JsonLiteDictionaryBucket *b = buckets[index]; b != NULL; b = b->next) {
        if (b->hash != hash) {
            continue;
        }
        
        if ([b->key isEqual:aKey]) {
            return b->value;
        }
    }
    return nil;
}

- (NSEnumerator *)keyEnumerator {
    return [JsonLiteDictionaryEnumerator enumeratorForDictionary:self];
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
    if (index >= count) {
        // gcov do not show code coverage for exceptions.
        id cls = NSStringFromClass([self class]);
        id sel = NSStringFromSelector(_cmd);
        [NSException raise:NSRangeException
                    format:@"*** -[%@ %@]: index (%d) beyond bounds (%d)", cls, sel, index, count];
    }
    
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
