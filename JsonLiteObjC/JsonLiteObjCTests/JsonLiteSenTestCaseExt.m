//
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

#import "JsonLiteSenTestCaseExt.h"

@implementation SenTestCase(JsonLiteSenTestCaseExt)

- (void)compareNumber:(NSNumber *)n1 withNumber:(NSNumber *)n2 {
    double d1 = [n1 doubleValue];
    double d2 = [n2 doubleValue];
    double dx = fabs(d1 - d2);
    if (dx > 0.00000005) {
        STAssertTrue(NO, @"Not equal");
    }
}

- (void)compareDictionary:(NSDictionary *)d1 withDictionary:(NSDictionary *)d2 {
    NSInteger c1 = [d1 count];
    NSInteger c2 = [d2 count];
    if (c1 != c2) {
        STAssertTrue(c1 == c2, @"Length is not equal");
        return;
    }
    
    for (id key in d1) {
        id obj1 = [d1 objectForKey:key];
        id obj2 = [d2 objectForKey:key];
        if ([obj1 isKindOfClass:[NSArray class]] && [obj2 isKindOfClass:[NSArray class]]) {
            [self compareArray:obj1 withArray:obj2];
            continue;
        }
        
        if ([obj1 isKindOfClass:[NSDictionary class]] && [obj2 isKindOfClass:[NSDictionary class]]) {
            [self compareDictionary:obj1 withDictionary:obj2];
            continue;
        }
        
        if ([obj1 isKindOfClass:[NSNumber class]] && [obj2 isKindOfClass:[NSNumber class]]) {
            [self compareNumber:obj1 withNumber:obj2];
            continue;
        }
        
        if (obj1 != NULL && obj2 != NULL) {
            STAssertTrue([obj1 isEqual:obj2], @"Objects are not equal");
        } else {
            STAssertTrue(NO, @"Objects are not equal");
        }
    }
}

- (void)compareArray:(NSArray *)a1 withArray:(NSArray *)a2 {
    NSInteger c1 = [a1 count];
    NSInteger c2 = [a2 count];
    if (c1 != c2) {
        STAssertTrue(c1 == c2, @"Length is not equal");
        return;
    }
    
    for (NSInteger i = 0; i < c1; i++) {
        id obj1 = [a1 objectAtIndex:i];
        id obj2 = [a2 objectAtIndex:i];
        if ([obj1 isKindOfClass:[NSArray class]] && [obj2 isKindOfClass:[NSArray class]]) {
            [self compareArray:obj1 withArray:obj2];
            continue;
        }
        
        if ([obj1 isKindOfClass:[NSDictionary class]] && [obj2 isKindOfClass:[NSDictionary class]]) {
            [self compareDictionary:obj1 withDictionary:obj2];
            continue;
        }
        
        if ([obj1 isKindOfClass:[NSNumber class]] && [obj2 isKindOfClass:[NSNumber class]]) {
            [self compareNumber:obj1 withNumber:obj2];
            continue;
        }
        
        STAssertTrue([obj1 isEqual:obj2], @"Objects are not equal");
    }
}

@end
