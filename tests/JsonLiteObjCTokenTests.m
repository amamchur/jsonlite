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

#import "JsonLiteObjCTokenTests.h"
#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteObjC.h"

@implementation JsonLiteObjCTokenTests

- (void)testToken {
    id obj = [self parseObjectFromFile:@"tokens" inDir:@"tokens"];
    XCTAssertNotNil(obj, @"Object is nil");
}

- (void)testZeroTerminatedByTab {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[0\t]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testZeroTerminatedByLF {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[0\n]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testZeroTerminatedByCR {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[0\r]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testZeroTerminatedBySpace {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[0 ]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testZeroTerminatedByComma {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[0, 0]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testZeroTerminatedBySquareBraket {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[0]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testZeroTerminatedByBrace {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "{\"key\":0}";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testIntTerminatedByTab {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[12345\t]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testIntTerminatedByLF {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[12345\n]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testIntTerminatedByCR {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[12345\r]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testIntTerminatedBySpace {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[12345 ]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testIntTerminatedByComma {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[12345, 12345]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testIntTerminatedBySquareBraket {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[12345]";

    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testIntTerminatedByBrace {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "{\"key\":12345}";

    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testExpTerminatedByTab {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234e5\t]";

    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testExpTerminatedByLF {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234e5\n]";

    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testExpTerminatedByCR {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234e5\r]";

    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testExpTerminatedBySpace {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234e5 ]";

    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testExpTerminatedByComma {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234e5, 1234e5]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testExpTerminatedBySquareBraket {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234e5]";

    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testExpTerminatedByBrace {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "{\"key\":1234e5}";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testFracTerminatedByTab {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234.5\t]";

    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testFracTerminatedByLF {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234.5\n]";

    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testFracTerminatedByCR {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234.5\r]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testFracTerminatedBySpace {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234.5 ]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testFracTerminatedByComma {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234.5, 1234.5]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testFracTerminatedBySquareBraket {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "[1234.5]";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testFracTerminatedByBrace {
    char memory[jsonlite_parser_estimate_size(4)];
    char json[] = "{\"key\":1234.5}";
    
    jsonlite_parser ps = jsonlite_parser_init(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json, sizeof(json));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad error code");
}

- (void)testNumber {
    NSNumber *n1 = [NSNumber numberWithLongLong:4278190080];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1, n1]];
    NSNumber *n2 = [[JsonLiteObjC objectFromData:data depth:16] lastObject];
    XCTAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMaxLong {
    NSNumber *n1 = [NSNumber numberWithLong:LONG_MAX];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteObjC objectFromData:data depth:16] lastObject];
    XCTAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMinLong {
    NSNumber *n1 = [NSNumber numberWithLong:LONG_MIN];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteObjC objectFromData:data depth:16] lastObject];
    XCTAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMaxLongP1 {
    NSNumber *n1 = [NSNumber numberWithLongLong:(long long)LONG_MAX + 1ll];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteObjC objectFromData:data depth:16] lastObject];
    XCTAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMinLongM1 {
    NSNumber *n1 = [NSNumber numberWithLongLong:LONG_MIN - 1ll];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteObjC objectFromData:data depth:16] lastObject];
    XCTAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMaxLongLong {
    NSNumber *n1 = [NSNumber numberWithLongLong:LONG_LONG_MAX];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteObjC objectFromData:data depth:16] lastObject];
    XCTAssertEqualObjects(n1, n2, @"Object not equals");
}

- (void)testMinLongLong {
    NSNumber *n1 = [NSNumber numberWithLongLong:LONG_LONG_MIN];
    NSData *data  = [[JsonLiteSerializer serializer] serializeObject:@[n1]];
    NSNumber *n2 = [[JsonLiteObjC objectFromData:data depth:16] lastObject];
    XCTAssertEqualObjects(n1, n2, @"Object not equals");
}

@end
