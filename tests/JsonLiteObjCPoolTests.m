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

#import "JsonLiteObjCPoolTests.h"
#import "JsonLiteSenTestCaseExt.h"
#include "jsonlite.h"

@implementation JsonLiteObjCPoolTests

- (void)testPool {
    jsonlite_token_pool pool = jsonlite_token_pool_create(NULL);
    jsonlite_token_bucket *b = jsonlite_token_pool_get_bucket(NULL, NULL);
    STAssertTrue(b == NULL, @"Buckut is not null");
    
    jsonlite_token token;
    memset(&token, 0, sizeof(token));
    
    b = jsonlite_token_pool_get_bucket(NULL, NULL);
    STAssertTrue(b == NULL, @"Bucket is not null");
    
    b = jsonlite_token_pool_get_bucket(pool, NULL);
    STAssertTrue(b == NULL, @"Bucket is not null");
    
    token.start = NULL;
    token.end = NULL;
    b = jsonlite_token_pool_get_bucket(pool, &token);
    STAssertTrue(b == NULL, @"Bucket is not null");
    
    token.start = (uint8_t *)0xDEADBEEF;
    token.end = NULL;
    b = jsonlite_token_pool_get_bucket(pool, &token);
    STAssertTrue(b == NULL, @"Bucket is not null");
    
    token.start = NULL;
    token.end = (uint8_t *)0xDEADBEEF;
    b = jsonlite_token_pool_get_bucket(pool, &token);
    STAssertTrue(b == NULL, @"Bucket is not null");
    
    token.start = (uint8_t *)"test";
    token.end = token.start + 4;
    
    b = jsonlite_token_pool_get_bucket(pool, &token);
    STAssertTrue(b != NULL, @"Bucket is null");
    
    jsonlite_token_pool_release(NULL);
    jsonlite_token_pool_release(pool);
}

@end
