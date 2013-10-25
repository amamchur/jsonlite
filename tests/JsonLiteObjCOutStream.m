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

- (void)testNullStream {
    char hello[] = "Hello mem stream!";
    
    jsonlite_stream stream = jsonlite_null_stream;
    STAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
    int wrote = jsonlite_stream_write(stream, hello, sizeof(hello));
    STAssertTrue(wrote == sizeof(hello), @"Incorrect size");
    
    jsonlite_stream_release(stream);
    jsonlite_stream_release(NULL);
}

- (void)testStdoutStream {
    char hello[] = "Hello mem stream\n!";
    
    jsonlite_stream stream = jsonlite_stdout_stream;
    STAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
    int wrote = jsonlite_stream_write(stream, hello, sizeof(hello) - 1);
    STAssertTrue(wrote == sizeof(hello) - 1, @"Incorrect size");
    
    jsonlite_stream_release(stream);
}

- (void)testMemStreamParams {
    jsonlite_stream stream = jsonlite_mem_stream_init(0x100);
    int res = jsonlite_stream_write(NULL, NULL, 0);
    STAssertTrue(res == -1, @"Incorrect result");
    
    res = jsonlite_stream_write(NULL, "", 0);
    STAssertTrue(res == -1, @"Incorrect result");
    
    res = jsonlite_stream_write(stream, NULL, 0);
    STAssertTrue(res == -1, @"Incorrect result");
    
    res = jsonlite_stream_write(stream, "", 0);
    STAssertTrue(res == 0, @"Incorrect result");
    
    jsonlite_stream_release(stream);
}

- (void)testMemEmptyStream {    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x100);
    STAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
    uint8_t *data = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &data);
    STAssertTrue(data == NULL, @"Data is not NULL");
    STAssertTrue(size == 0, @"Size is not zero");

    jsonlite_stream_release(stream);
}

- (void)testMemHelloStream {
    char hello[] = "Hello mem stream!";
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0xFF);
    STAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
    int wrote = jsonlite_stream_write(stream, hello, sizeof(hello));
    STAssertTrue(wrote == sizeof(hello), @"Incorrect size");
    
    uint8_t *data = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &data);
    STAssertTrue(data != NULL, @"Data is NULL");
    STAssertTrue(size == sizeof(hello), @"Size is not sizeof(hello)");
    
    int cmp = strcmp(hello, (char *)data);
    STAssertTrue(cmp == 0, @"Data is not equals to original value");
    
    jsonlite_stream_release(stream);
}

- (void)testMemHelloBytePerBlockStream {
    char hello[] = "Hello mem stream!";
    
    jsonlite_stream stream = jsonlite_mem_stream_init(0x1);
    STAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
    int wrote = jsonlite_stream_write(stream, hello, sizeof(hello));
    STAssertTrue(wrote == sizeof(hello), @"Incorrect size");
    
    uint8_t *data = NULL;
    size_t size = jsonlite_mem_stream_data(stream, &data);
    STAssertTrue(data != NULL, @"Data is NULL");
    STAssertTrue(size == sizeof(hello), @"Size is not sizeof(hello)");
    
    int cmp = strcmp(hello, (char *)data);
    STAssertTrue(cmp == 0, @"Data is not equals to original value");
    
    jsonlite_stream_release(stream);
}

@end
