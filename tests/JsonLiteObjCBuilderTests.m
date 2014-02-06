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


#import "JsonLiteObjCBuilderTests.h"
#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteAccumulator.h"
#import "JsonLiteSerializer.h"

@implementation JsonLiteObjCBuilderTests

- (id)parseObjectFromFile:(NSString *)file inDir:(NSString *)dir {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:file
                                                                      ofType:@"json"
                                                                 inDirectory:dir];
    NSError *error = nil;
    NSData *data = [[[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error] autorelease];
    XCTAssertNil(error, @"Cann't read file %@, error - %@.", file, error);
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteAccumulator *delegate = [[JsonLiteAccumulator alloc] init];
    parser.delegate = delegate;
    
    BOOL complete = [parser parse:data];
    XCTAssertTrue(error == nil, @"Parsing error %@.", error);
    XCTAssertTrue(complete, @"Parsing not complate.");
    
    id object = [delegate object];
    
    [delegate release];
    [parser release];
    
    return object;
}

- (void)testParams {
    char builder_memory[jsonlite_builder_estimate_size(32)];
    
    jsonlite_stream stream = jsonlite_mem_stream_alloc(0x100);
    jsonlite_builder bs = jsonlite_builder_init(NULL, sizeof(builder_memory), stream);
    XCTAssertTrue(bs == NULL, @"Builder was created");
    
    bs = jsonlite_builder_init(builder_memory, sizeof(builder_memory), NULL);
    XCTAssertTrue(bs == NULL, @"Builder was created");
    
    bs = jsonlite_builder_init(builder_memory, 0, stream);
    XCTAssertTrue(bs == NULL, @"Builder was created");
}

- (void)testBuilder {
    id object = [self parseObjectFromFile:@"pass1" inDir:@"fail"];
    XCTAssertTrue(object != nil, @"Parsing error.");
    
    JsonLiteSerializer *jls =[[JsonLiteSerializer alloc] init];
    NSData *data = [jls serializeObject:object];
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteAccumulator *delegate = [[JsonLiteAccumulator alloc] init];
    parser.delegate = delegate;
    
    [parser parse:data];
    id object1 = [delegate object];
    
    [delegate release];
    [parser release]; 
    [jls release];
    
    XCTAssertTrue([self compareArray:object withArray:object1], @"Not equal");
}

- (void)testBuilderBeauty {
    id object = [self parseObjectFromFile:@"twitter" inDir:@"success"];
    XCTAssertTrue(object != nil, @"Parsing error.");
    
    JsonLiteSerializer *jls =[[JsonLiteSerializer alloc] init];
    jls.indentation = 4;
    XCTAssertEqual(jls.indentation, (NSInteger)4, @"Indentations are not equal");
    NSData *data = [jls serializeObject:object];
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteAccumulator *delegate = [[JsonLiteAccumulator alloc] init];
    parser.delegate = delegate;
    
    [parser parse:data];
    id object1 = [delegate object];
    
    [delegate release];
    [parser release];
    [jls release];
    
    XCTAssertTrue([self compareArray:object withArray:object1], @"Not equal");
}

- (void)testBuilderInitialization {
    jsonlite_stream stream = jsonlite_mem_stream_alloc(0x100);
    uint8_t *buffer = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &buffer, 0);
    
    XCTAssertTrue(buffer == NULL, @"Buffer not null");
    XCTAssertTrue(size == 0, @"Size not 0");
    jsonlite_mem_stream_free(stream);
}

- (void)manualBuildingWithIndentation:(int)indentation {
    char builder_memory[jsonlite_builder_estimate_size(32)];
    
    jsonlite_stream stream = jsonlite_mem_stream_alloc(0x100);
    jsonlite_builder bs = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    XCTAssertTrue(bs != NULL, @"Builder not created");
    
    jsonlite_result result = jsonlite_result_ok;
    
    result = jsonlite_builder_set_indentation(NULL, indentation);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_set_double_format(NULL, "%.10f");
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_set_double_format(bs, NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");

    result = jsonlite_builder_object_begin(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_object_end(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_array_begin(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_array_end(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_key(NULL, "key", strlen("key"));
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_key(bs, NULL, strlen("key"));
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_raw_key(NULL, "key", strlen("key"));
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_raw_key(bs, NULL, strlen("key"));
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_raw_key(bs, "key", 0);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_raw_value(NULL, "\"key\"", strlen("\"key\""));
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_raw_value(bs, NULL, strlen("\"key\""));
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_raw_value(bs, "\"key\"", 0);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_raw_string(NULL, "key", strlen("key"));
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_raw_string(bs, NULL, strlen("key"));
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_raw_string(bs, "key", 0);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_base64_value(NULL, "key", 3);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_base64_value(bs, NULL, 3);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_base64_value(bs, "key", 0);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_string(NULL, "key", strlen("key"));
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_string(bs, NULL, strlen("key"));
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_null(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_true(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_false(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_int(NULL, 100);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_double(NULL, 100.0);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_object_end(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_array_end(NULL);
    XCTAssertTrue(result == jsonlite_result_invalid_argument, @"Incorrect error");
    
    result = jsonlite_builder_set_indentation(bs, indentation);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect error");
    
    result = jsonlite_builder_key(bs, "key", strlen("key"));
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_string(bs, "key", strlen("key"));
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_raw_key(bs, "\"key\"", strlen("\"key\""));
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Incorrect error");
    
    result = jsonlite_builder_raw_value(bs, "\"key\"", strlen("\"key\""));
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Incorrect error");
    
    result = jsonlite_builder_raw_string(bs, "key", strlen("key"));
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Incorrect error");
    
    result = jsonlite_builder_base64_value(bs, "base64", strlen("base64"));
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Incorrect error");
    
    result = jsonlite_builder_null(bs);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_true(bs);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_false(bs);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_int(bs, 100);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_double(bs, 100.0);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_object_end(bs);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_array_end(bs);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_set_indentation(bs, indentation);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect error");
        
    result = jsonlite_builder_object_begin(bs);
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");

    result = jsonlite_builder_object_begin(bs);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_array_begin(bs);
    XCTAssertTrue(result == jsonlite_result_not_allowed, @"Parse state error");
    
    result = jsonlite_builder_raw_key(bs, "key1", strlen("key1"));
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_raw_value(bs, "\"value1\"", strlen("\"value1\""));
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_raw_key(bs, "key3", strlen("key3"));
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_raw_string(bs, "string", strlen("string"));
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_raw_key(bs, "key4", strlen("key4"));
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_array_begin(bs);
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_array_begin(bs);
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_array_begin(bs);
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");

    result = jsonlite_builder_raw_value(bs, "\"value1\"", strlen("\"value1\""));
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_raw_string(bs, "value1", strlen("value1"));
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_array_end(bs);
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_array_end(bs);
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_raw_value(bs, "123", strlen("123"));
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_array_end(bs);
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    result = jsonlite_builder_object_end(bs);
    XCTAssertTrue(result == jsonlite_result_ok, @"Parse state error");
    
    uint8_t *buffer = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &buffer, 0);
    
    XCTAssertTrue(buffer != NULL, @"Buffer is NULL");
    XCTAssertTrue(size != 0, @"Size is zero");
    
    free(buffer);
    
    jsonlite_mem_stream_free(stream);
    XCTAssertTrue(result == jsonlite_result_ok, @"Release error");
}

- (void)testDepth {
    char builder_memory[jsonlite_builder_estimate_size(2)];
    
    jsonlite_stream stream = jsonlite_mem_stream_alloc(0x100);
    jsonlite_builder bs = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    jsonlite_result result = jsonlite_builder_object_begin(bs);
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad result");
    result = jsonlite_builder_key(bs, "a", sizeof("a"));
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad result");
    result = jsonlite_builder_object_begin(bs);
    XCTAssertTrue(result == jsonlite_result_depth_limit, @"Bad result");
    jsonlite_mem_stream_free(stream);
    
    stream = jsonlite_mem_stream_alloc(0x100);
    bs = jsonlite_builder_init(builder_memory, sizeof(builder_memory), stream);
    result = jsonlite_builder_array_begin(bs);
    XCTAssertTrue(result == jsonlite_result_ok, @"Bad result");
    result = jsonlite_builder_array_begin(bs);
    XCTAssertTrue(result == jsonlite_result_depth_limit, @"Bad result");
    jsonlite_mem_stream_free(stream);
}

- (void)testManualBuildingBeautifier {
    [self manualBuildingWithIndentation:4];
}

- (void)testManualBuilding {
    [self manualBuildingWithIndentation:0];
}

@end


