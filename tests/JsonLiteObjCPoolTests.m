//
//  Copyright 2012-2014, Andrii Mamchur
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
    jsonlite_token_pool pools[1];
    uint8_t pools_mem[jsonlite_token_pool_estimate_size(1)];
    jsonlite_token_bucket *b = NULL;
    
    jsonlite_token_pool_init_memory(pools_mem, sizeof(pools_mem), pools);
    
    jsonlite_token token;
    token.start = (uint8_t *)"test";
    token.end = token.start + 4;

    
    b = jsonlite_token_pool_get_bucket(pools[0], &token);
    XCTAssertTrue(b != NULL, @"Bucket is null");
    
    jsonlite_token_pool_cleanup(pools, 1, NULL);
}

@end
