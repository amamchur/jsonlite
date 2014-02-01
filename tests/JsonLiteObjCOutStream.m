//
//  JsonLiteObjCOutStream.m
//  JsonLiteObjC
//
//  Created by Andrii Mamchur on 10/25/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteObjCOutStream.h"
#import "jsonlite.h"

@implementation JsonLiteObjCOutStream

- (void)testStaticMemStream {
    char json[] = "{\"key\":\"hello\"}";
    char buffer[512] = {0};
    
    jsonlite_stream stream = jsonlite_static_mem_stream_init(buffer, sizeof(buffer));
    jsonlite_builder builder = jsonlite_builder_init(0xFF, stream);
    
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, "key", sizeof("key") - 1);
    jsonlite_builder_string(builder, "hello", sizeof("hello") - 1);
    jsonlite_builder_object_end(builder);
    
    jsonlite_builder_release(builder);
    jsonlite_stream_release(stream);
    
    size_t buffer_size = jsonlite_static_mem_stream_written_bytes(stream);
    size_t json_size = strlen(json);
    
    XCTAssertTrue(buffer_size == json_size, @"Incorrect size");
    
    size_t min = MIN(buffer_size, json_size);
    for (int i = 0; i < min; i++) {
        XCTAssertTrue(json[i] == buffer[i], @"Bad chars %c %c", json[i], buffer[i]);
    }
}

- (void)testSmallStaticMemStream {
    char json[] = "{\"key\":\"hello\",\"long\":\"longlonglonglonglonglong\"}";
    char buffer[8] = {0};
    
    jsonlite_stream stream = jsonlite_static_mem_stream_init(buffer, sizeof(buffer));
    jsonlite_builder builder = jsonlite_builder_init(0xFF, stream);
    
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, "key", sizeof("key") - 1);
    jsonlite_builder_string(builder, "hello", sizeof("hello") - 1);
    jsonlite_builder_key(builder, "long", sizeof("long") - 1);
    jsonlite_builder_string(builder, "longlonglonglonglonglong", sizeof("longlonglonglonglonglong") - 1);
    jsonlite_builder_object_end(builder);
    
    jsonlite_builder_release(builder);
    jsonlite_stream_release(stream);
    
    size_t buffer_size = jsonlite_static_mem_stream_written_bytes(stream);
    size_t json_size = strlen(json);
    
    size_t min = MIN(buffer_size, json_size);
    for (int i = 0; i < min; i++) {
        XCTAssertTrue(json[i] == buffer[i], @"Bad chars %c %c", json[i], buffer[i]);
    }
}

- (void)testSmallStaticMemStream1 {
    char json[] = "{\"long\":\"longlonglonglonglonglong\"}";
    char buffer[16] = {0};
    
    jsonlite_stream stream = jsonlite_static_mem_stream_init(buffer, sizeof(buffer));
    jsonlite_builder builder = jsonlite_builder_init(0xFF, stream);
    
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, "long", sizeof("long") - 1);
    jsonlite_builder_string(builder, "longlonglonglonglonglong", sizeof("longlonglonglonglonglong") - 1);
    jsonlite_builder_object_end(builder);
    
    jsonlite_builder_release(builder);
    jsonlite_stream_release(stream);
    
    size_t buffer_size = jsonlite_static_mem_stream_written_bytes(stream);
    size_t json_size = strlen(json);
    
    size_t min = MIN(buffer_size, json_size);
    for (int i = 0; i < min; i++) {
        XCTAssertTrue(json[i] == buffer[i], @"Bad chars %c %c", json[i], buffer[i]);
    }
}

#if JSONLITE_FILE_SYSTEM_ENABLED

- (void)testFileStream {
    char json[] = "{\"key\":\"hello\"}";
    char buffer[512] = {0};
    
    FILE *file = tmpfile ();
    jsonlite_stream stream = jsonlite_file_stream_init(file);
    jsonlite_builder builder = jsonlite_builder_init(0xFF, stream);
    
    jsonlite_builder_object_begin(builder);
    jsonlite_builder_key(builder, "key", sizeof("key") - 1);
    jsonlite_builder_string(builder, "hello", sizeof("hello") - 1);
    jsonlite_builder_object_end(builder);
    
    jsonlite_builder_release(builder);
    jsonlite_stream_release(stream);
    
    fseek(file, 0, SEEK_SET);
    size_t buffer_size = fread(buffer, 1, sizeof(buffer), file);
    size_t json_size = strlen(json);
    
    XCTAssertTrue(buffer_size == json_size, @"Incorrect size");
    
    size_t min = MIN(buffer_size, json_size);
    for (int i = 0; i < min; i++) {
        XCTAssertTrue(json[i] == buffer[i], @"Bad chars %c %c", json[i], buffer[i]);
    }
        
    fclose(file);
}

#endif

- (void)testNullStream {
    char hello[] = "Hello mem stream!";
    
    jsonlite_stream stream = jsonlite_null_stream;
    XCTAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
    int written = jsonlite_stream_write(stream, hello, sizeof(hello));
    XCTAssertTrue(written == sizeof(hello), @"Incorrect size");
    
    jsonlite_stream_release(stream);
    jsonlite_stream_release(NULL);
}

- (void)testStdoutStream {
    char hello[] = "Hello mem stream\n!";
    
    jsonlite_stream stream = jsonlite_stdout_stream;
    XCTAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
    int written = jsonlite_stream_write(stream, hello, sizeof(hello) - 1);
    XCTAssertTrue(written == sizeof(hello) - 1, @"Incorrect size");
    
    jsonlite_stream_release(stream);
}

- (void)testMemEmptyStream {    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x100);
    XCTAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
    uint8_t *data = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &data, 0);
    XCTAssertTrue(data == NULL, @"Data is not NULL");
    XCTAssertTrue(size == 0, @"Size is not zero");

    jsonlite_stream_release(stream);
}

- (void)testMemHelloStream {
    char hello[] = "Hello mem stream!";
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0xFF);
    XCTAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
    int written = jsonlite_stream_write(stream, hello, sizeof(hello));
    XCTAssertTrue(written == sizeof(hello), @"Incorrect size");
    
    uint8_t *data = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &data, 0);
    XCTAssertTrue(data != NULL, @"Data is NULL");
    XCTAssertTrue(size == sizeof(hello), @"Size is not sizeof(hello)");
    
    int cmp = strcmp(hello, (char *)data);
    XCTAssertTrue(cmp == 0, @"Data is not equals to original value");
    
    jsonlite_stream_release(stream);
}

- (void)testMemHelloBytePerBlockStream {
    char hello[] = "Hello mem stream!";
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x1);
    XCTAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
    int written = jsonlite_stream_write(stream, hello, sizeof(hello));
    XCTAssertTrue(written == sizeof(hello), @"Incorrect size");
    
    uint8_t *data = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &data, 0);
    XCTAssertTrue(data != NULL, @"Data is NULL");
    XCTAssertTrue(size == sizeof(hello), @"Size is not sizeof(hello)");
    
    int cmp = strcmp(hello, (char *)data);
    XCTAssertTrue(cmp == 0, @"Data is not equals to original value");
    
    jsonlite_stream_release(stream);
}

@end
