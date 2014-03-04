//
//  JsonLiteOutputStream.h
//  JsonLiteObjC
//
//  Created by Andrii Mamchur on 3/4/14.
//  Copyright (c) 2014 Andrii Mamchur. All rights reserved.
//

#import <Foundation/Foundation.h>

@class JsonLiteAccumulator;
@class JsonLiteDeserializer;
@class JsonLiteParser;

@interface JsonLiteOutputStream : NSOutputStream {
    JsonLiteAccumulator *accumulator;
    JsonLiteDeserializer *deserializer;
    JsonLiteParser *parser;
    Class rootClass;
    NSUInteger depth;
    NSStreamStatus streamStatus;
    NSError *streamError;
}

- (id)object;
- (id)initWithRootClass:(Class)rootCls depth:(NSUInteger)theDepth;

@end
