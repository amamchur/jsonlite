//
//  JsonLiteOutputStream.m
//  JsonLiteObjC
//
//  Created by Andrii Mamchur on 3/4/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteOutputStream.h"
#import "JsonLiteAccumulator.h"
#import "JsonLiteDeserializer.h"
#import "JsonLiteParser.h"

@implementation JsonLiteOutputStream

- (id)initWithRootClass:(Class)rootCls depth:(NSUInteger)theDepth {
    self = [super init];
    if (self != nil) {
        depth = theDepth;
        rootClass = rootCls;
        streamStatus = NSStreamStatusNotOpen;
    }
    return self;
}

- (void)dealloc {
    [parser release];
    [deserializer release];
    [accumulator release];
    [streamError release];
    [super dealloc];
}

- (NSStreamStatus)streamStatus {
    return streamStatus;
}

- (NSError *)streamError {
    return streamError;
}

- (BOOL)hasSpaceAvailable {
    return streamStatus == NSStreamStatusOpen;
}

- (NSInteger)write:(const uint8_t *)buffer maxLength:(NSUInteger)length {
    if (streamError == nil) {
        [parser parse:buffer length:length];
        NSError *error = parser.parseError;
        if (error != nil) {
            [self willChangeValueForKey:@"streamError"];
            streamError = [error retain];
            [self didChangeValueForKey:@"streamError"];
            return -1;
        }
        
        return length;
    }
    
    return -1;
}

- (void)open {
    [self willChangeValueForKey:@"streamStatus"];
    streamStatus = NSStreamStatusOpen;
    [self didChangeValueForKey:@"streamStatus"];
    
    parser = [[JsonLiteParser alloc] initWithDepth:depth];
    if (rootClass != nil) {
        deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:rootClass];
        parser.delegate = deserializer;
    } else {
        accumulator = [[JsonLiteAccumulator alloc] initWithDepth:depth];
        parser.delegate = accumulator;
    }
}

- (void)close {
    [self willChangeValueForKey:@"streamStatus"];
    streamStatus = NSStreamStatusClosed;
    [self didChangeValueForKey:@"streamStatus"];
}

- (id)object {
    if (accumulator != nil) {
        return [accumulator object];
    }
    
    if (deserializer != nil) {
        return [deserializer object];
    }
    
    return nil;
}

@end
