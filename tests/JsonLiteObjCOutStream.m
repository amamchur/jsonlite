//
//  JsonLiteObjCOutStream.m
//  JsonLiteObjC
//
//  Created by Andrii Mamchur on 10/25/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteObjCOutStream.h"
#import "jsonlite_stream.h"

@implementation JsonLiteObjCOutStream

- (void)testMemEmptyStream {
//    jsonlite_out_stream stream = jsonlite_mem_stream_init(0xFF);
//    STAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
//    
//    uint8_t *data = NULL;
//    size_t size = jsonlite_mem_stream_data(stream, &data);
//    STAssertTrue(data == NULL, @"Data is not NULL");
//    STAssertTrue(size == 0, @"Size is not zero");
    
//    jsonlite_mem_stream_release(stream);
}

- (void)testMemHelloStream {
//    char hello[] = "Hello mem stream!";
    
//    jsonlite_out_stream stream = jsonlite_mem_stream_init(0xFF);
//    STAssertTrue(stream != NULL, @"jsonlite_mem_stream_init return NULL stream");
    
//    int size = jsonlite_out_stream_write(stream, hello, sizeof(hello));
//    STAssertTrue(size == sizeof(hello), @"Incorrect size");
//    
//    uint8_t *data = NULL;
//    size = jsonlite_mem_stream_data(stream, &data);
//    STAssertTrue(data != NULL, @"Data is NULL");
//    STAssertTrue(size == sizeof(hello), @"Size is not sizeof(hello)");
//    
//    int cmp = strcmp(hello, (char *)data);
//    STAssertTrue(cmp == 0, @"Data is not equals to original value");
    
//    jsonlite_mem_stream_release(stream);
}

@end
