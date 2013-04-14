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

#import "JsonLiteObjCTokenTests.h"
#import "jsonlite_token.h"

@implementation JsonLiteObjCTokenTests

- (void)testTokenFunctions {
    uint8_t *uft8_buffer = NULL;
    uint16_t *uft16_buffer = NULL;
    size_t size = jsonlite_token_decode_size_for_uft8(NULL);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_to_uft8(NULL, NULL);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_to_uft8(NULL, &uft8_buffer);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_size_for_uft16(NULL);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_to_uft16(NULL, NULL);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_token_decode_to_uft16(NULL, &uft16_buffer);
    STAssertTrue(size == 0, @"Size is not 0");
    
    size = jsonlite_hex_char_to_uint8('Q');
    STAssertTrue(size == 0xFF, @"Is not 0XFF");

    size = jsonlite_hex_char_to_uint8('7');
    STAssertTrue(size == 7, @"Is not 7");
    
    size = jsonlite_hex_char_to_uint8('a');
    STAssertTrue(size == 10, @"Is not 10");
    
    size = jsonlite_hex_char_to_uint8('A');
    STAssertTrue(size == 10, @"Is not 10");
}

@end
