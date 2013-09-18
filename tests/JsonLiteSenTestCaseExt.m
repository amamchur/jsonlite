//
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

#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteAccumulator.h"
#import "JsonLiteParser.h"

@implementation SenTestCase(JsonLiteSenTestCaseExt)

- (BOOL)compareNumber:(NSNumber *)n1 withNumber:(NSNumber *)n2 {
    double d1 = [n1 doubleValue];
    double d2 = [n2 doubleValue];
    double dx = fabs(d1 - d2);
    return dx <= 0.00000005;
}

- (BOOL)compareDictionary:(NSDictionary *)d1 withDictionary:(NSDictionary *)d2 {
    NSInteger c1 = [d1 count];
    NSInteger c2 = [d2 count];
    BOOL equal = c1 == c2;
    if (!equal) {
        return NO;
    }
    
    for (id key in d1) {
        id obj1 = [d1 objectForKey:key];
        id obj2 = [d2 objectForKey:key];
        if ([obj1 isKindOfClass:[NSArray class]] && [obj2 isKindOfClass:[NSArray class]]) {
            equal = [self compareArray:obj1 withArray:obj2];
            if (!equal) {
                return NO;
            }
            continue;
        }
        
        if ([obj1 isKindOfClass:[NSDictionary class]] && [obj2 isKindOfClass:[NSDictionary class]]) {
            [self compareDictionary:obj1 withDictionary:obj2];
            continue;
        }
        
        if ([obj1 isKindOfClass:[NSNumber class]] && [obj2 isKindOfClass:[NSNumber class]]) {
            equal = [self compareNumber:obj1 withNumber:obj2];
            if (!equal) {
                return NO;
            }
            continue;
        }
        
        if (obj1 != NULL && obj2 != NULL) {
            if (![obj1 isEqual:obj2]) {
                return NO;
            }
        }
    }
    return YES;
}

- (BOOL)compareArray:(NSArray *)a1 withArray:(NSArray *)a2 {
    NSInteger c1 = [a1 count];
    NSInteger c2 = [a2 count];
    BOOL equal = c1 == c2;
    if (!equal) {
        return NO;
    }
    
    for (NSInteger i = 0; i < c1; i++) {
        id obj1 = [a1 objectAtIndex:i];
        id obj2 = [a2 objectAtIndex:i];
        if ([obj1 isKindOfClass:[NSArray class]] && [obj2 isKindOfClass:[NSArray class]]) {
            equal = [self compareArray:obj1 withArray:obj2];
            if (!equal) {
                return NO;
            }
            continue;
        }
        
        if ([obj1 isKindOfClass:[NSDictionary class]] && [obj2 isKindOfClass:[NSDictionary class]]) {
            equal = [self compareDictionary:obj1 withDictionary:obj2];
            if (!equal) {
                return NO;
            }
            continue;
        }
        
        if ([obj1 isKindOfClass:[NSNumber class]] && [obj2 isKindOfClass:[NSNumber class]]) {
            equal = [self compareNumber:obj1 withNumber:obj2];
            if (!equal) {
                return NO;
            }
            continue;
        }
        if (![obj1 isEqual:obj2]) {
            return NO;
        }
    }
    
    return YES;
}

- (void)testCompareMethods {
    NSNumber *n1 = [NSNumber numberWithInt:1];
    NSNumber *n2 = [NSNumber numberWithInt:2];
    STAssertFalse([self compareNumber:n1 withNumber:n2], @"Number are equal");
    STAssertTrue([self compareNumber:n1 withNumber:n1], @"Number are not equal");
    
    NSDictionary *d1 = [NSDictionary dictionaryWithObject:@"a" forKey:@"b"];
    NSDictionary *d2 = [NSDictionary dictionary];
    STAssertFalse([self compareDictionary:d1 withDictionary:d2], @"Dictionaries are equal");
    
    d1 = [NSDictionary dictionaryWithObject:[NSArray array] forKey:@"a"];
    d2 = [NSDictionary dictionaryWithObject:[NSArray arrayWithObject:@"2"] forKey:@"a"];
    STAssertFalse([self compareDictionary:d1 withDictionary:d2], @"Dictionaries are equal");
    
    d1 = [NSDictionary dictionaryWithObject:n1 forKey:@"a"];
    d2 = [NSDictionary dictionaryWithObject:n2 forKey:@"a"];
    STAssertFalse([self compareDictionary:d1 withDictionary:d2], @"Dictionaries are equal");
    
    d1 = [NSDictionary dictionaryWithObject:@"v1" forKey:@"a"];
    d2 = [NSDictionary dictionaryWithObject:@"v2" forKey:@"a"];
    STAssertFalse([self compareDictionary:d1 withDictionary:d2], @"Dictionaries are equal");
    
    NSArray *a1 = [NSArray arrayWithObject:[NSArray arrayWithObject:@"v1"]];
    NSArray *a2 = [NSArray arrayWithObject:[NSArray arrayWithObject:@"v2"]];
    STAssertFalse([self compareArray:a1 withArray:a2], @"Arrays are equal");
    
    a1 = [NSArray arrayWithObject:d1];
    a2 = [NSArray arrayWithObject:d2];
    STAssertFalse([self compareArray:a1 withArray:a2], @"Arrays are equal");
    
    a1 = [NSArray arrayWithObject:n1];
    a2 = [NSArray arrayWithObject:n2];
    STAssertFalse([self compareArray:a1 withArray:a2], @"Arrays are equal");
    
    a1 = [NSArray arrayWithObject:@"v1"];
    a2 = [NSArray arrayWithObject:@"v2"];
    STAssertFalse([self compareArray:a1 withArray:a2], @"Arrays are equal");
}

- (id)parseObjectFromFile:(NSString *)file inDir:(NSString *)dir {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:file
                                                                      ofType:@"json"
                                                                 inDirectory:dir];
    NSError *error = nil;
    NSData *data = [[[NSData alloc] initWithContentsOfFile:path
                                                   options:0
                                                     error:&error] autorelease];
    STAssertNil(error, @"Cann't read file %@, error - %@.", file, error);
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteAccumulator *accumulator  = [[JsonLiteAccumulator alloc] init];
    parser.delegate = accumulator;
    
    BOOL complete = [parser parse:data];
    STAssertTrue(error == nil, @"Parsing error %@.", error);
    STAssertTrue(complete, @"Parsing not complate.");
    STAssertTrue(parser.parseError == nil, @"Parsing not complate.");
    STAssertTrue(parser.delegate == accumulator, @"Parsing not complate.");
    STAssertTrue(parser.depth > 0, @"Parsing not complate.");
    STAssertTrue(accumulator.currentDepth == 0, @"Parsing not complate.");
    STAssertTrue(accumulator.depth > 0, @"Parsing not complate.");
    
    accumulator.delegate = nil;
    STAssertTrue(accumulator.delegate == nil, @"Parsing not complate.");
    
    accumulator.delegate = nil;
    STAssertTrue(accumulator.delegate == nil, @"Parsing not complate.");
    
    id object = [accumulator object];
    
    parser.delegate = nil;
    [accumulator release];
    [parser release];
    
    return object;
}

- (NSError *)parseErrorFromFile:(NSString *)file inDir:(NSString *)dir {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:file
                                                                      ofType:@"json"
                                                                 inDirectory:dir];
    NSError *error = nil;
    NSData *data = [[[NSData alloc] initWithContentsOfFile:path
                                                   options:0
                                                     error:&error] autorelease];
    STAssertNil(error, @"Cann't read file %@, error - %@.", file, error);
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteAccumulator *delegate = [[JsonLiteAccumulator alloc] init];
    parser.delegate = delegate;
    [parser parse:data];
    [delegate release];
    [parser autorelease];
    
    return parser.parseError;
}

- (NSData *)dataFromFile:(NSString *)file inDir:(NSString *)dir {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:file
                                                                      ofType:@"json"
                                                                 inDirectory:dir];
    NSError *error = nil;
    NSData *data = [[[NSData alloc] initWithContentsOfFile:path
                                                   options:0
                                                     error:&error] autorelease];
    return data;
}

@end
