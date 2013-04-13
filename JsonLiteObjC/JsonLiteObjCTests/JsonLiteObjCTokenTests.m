//
//  JsonLiteObjCTokenTests.m
//  JsonLiteObjC
//
//  Created by admin on 4/13/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteObjCTokenTests.h"
#import "jsonlite_token.h"

@implementation JsonLiteObjCTokenTests

- (void)testTokenFunctions {
    size_t size = jsonlite_token_decode_size_for_uft8(NULL);
    STAssertTrue(size == 0, @"Size id not 0");
    
    size = jsonlite_token_decode_size_for_uft16(NULL);
    STAssertTrue(size == 0, @"Size id not 0");
}

@end
