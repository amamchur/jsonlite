//
//  JsonLiteObjCStream.m
//  JsonLiteObjC
//
//  Created by admin on 7/12/13.
//  Copyright (c) 2013 Andrii Mamchur. All rights reserved.
//

#import "JsonLiteObjCStream.h"
#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteParser.h"
#import "JsonLiteAccumulator.h"
#import "JsonLiteSerializer.h"
#import "JsonLiteOutputStream.h"

@interface OutputStreamModel : NSObject {
    NSNumber *key;
}

@property (copy, nonatomic) NSNumber *key;

@end

@implementation OutputStreamModel

@synthesize key;

- (void)dealloc {
    self.key = nil;
    [super dealloc];
}

@end

@interface JsonLiteObjCStream()<JsonLiteParserDelegate>
@end

@implementation JsonLiteObjCStream

- (void)parserDidStartObject:(JsonLiteParser *)parser {
}

- (void)parserDidEndObject:(JsonLiteParser *)parser {
}

- (void)parserDidStartArray:(JsonLiteParser *)parser {
}

- (void)parserDidEndArray:(JsonLiteParser *)parser {
}

- (void)parser:(JsonLiteParser *)parser foundKeyToken:(JsonLiteStringToken *)token {
}

- (void)parser:(JsonLiteParser *)parser foundStringToken:(JsonLiteStringToken *)token {
}

- (void)parser:(JsonLiteParser *)parser foundNumberToken:(JsonLiteNumberToken *)token {
}

- (void)parserFoundTrueToken:(JsonLiteParser *)parser {
}

- (void)parserFoundFalseToken:(JsonLiteParser *)parser {
}

- (void)parserFoundNullToken:(JsonLiteParser *)parser {
}

- (void)parser:(JsonLiteParser *)parser didFinishParsingWithError:(NSError *)error {
    finished = [error code] != JsonLiteCodeEndOfStream;
}

- (void)testStream {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"random"
                                                                      ofType:@"json"
                                                                 inDirectory:@"success"];
    NSInputStream *stream = [NSInputStream inputStreamWithFileAtPath:path];
    JsonLiteParser *parser = [JsonLiteParser parser];
    parser.delegate = self;
    NSRunLoop *runLoop = [NSRunLoop currentRunLoop];
    [parser parse:stream inRunLoop:runLoop];
    
    finished = NO;
    while (!finished) {
        NSDate *date = [NSDate dateWithTimeIntervalSinceNow:1];
        [runLoop runMode:NSDefaultRunLoopMode beforeDate:date];
    }
    
    XCTAssertNil(parser.parseError, @"Error occurs %@", parser.parseError);
}

- (void)testInputs {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"random"
                                                                      ofType:@"json"
                                                                 inDirectory:@"success"];
    NSInputStream *stream = [NSInputStream inputStreamWithFileAtPath:path];
    JsonLiteParser *parser = [JsonLiteParser parser];
    [parser parse:nil inRunLoop:nil];
    XCTAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Incorrect error");
    
    [parser parse:stream inRunLoop:nil];
    XCTAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Incorrect error");
    
    [parser parse:nil inRunLoop:[NSRunLoop currentRunLoop]];
    XCTAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Incorrect error");
}

- (void)testOutputStreamAcc {
    JsonLiteOutputStream *stream = [[JsonLiteOutputStream alloc] initWithRootClass:nil depth:32];
    NSStreamStatus status = [stream streamStatus];
    XCTAssertTrue(status == NSStreamStatusNotOpen, @"Incorrect stream status");
    
    BOOL hasSpace = [stream hasSpaceAvailable];
    XCTAssertFalse(hasSpace, @"Incorrect hasSpaceAvailable");
    
    NSError *error = [stream streamError];
    XCTAssertNil(error, @"Incorrect error!");
    
    id obj = [stream object];
    XCTAssertNil(obj, @"Object is not nil");
    
    char json[] = "{\"key\":1234}";
    [stream open];
    
    hasSpace = [stream hasSpaceAvailable];
    XCTAssertTrue(hasSpace, @"Incorrect hasSpaceAvailable");
    
    status = [stream streamStatus];
    XCTAssertTrue(status == NSStreamStatusOpen, @"Incorrect stream status");
    
    error = [stream streamError];
    XCTAssertNil(error, @"Incorrect error!");
    
    [stream write:(const uint8_t *)json maxLength:sizeof(json)];
    [stream close];
    
    status = [stream streamStatus];
    XCTAssertTrue(status == NSStreamStatusClosed, @"Incorrect stream status");
    
    hasSpace = [stream hasSpaceAvailable];
    XCTAssertFalse(hasSpace, @"Incorrect hasSpaceAvailable");
    
    NSDictionary *dict = [stream object];
    XCTAssertNotNil(dict, @"Result is nil!");
    
    NSNumber *number = [dict objectForKey:@"key"];
    XCTAssertEqualObjects(number, [NSNumber numberWithInt:1234], @"Incorrect value");
    
    [stream release];
}

- (void)testOutputStreamDes {
    JsonLiteOutputStream *stream = [[JsonLiteOutputStream alloc] initWithRootClass:[OutputStreamModel class]
                                                                             depth:32];
    NSStreamStatus status = [stream streamStatus];
    XCTAssertTrue(status == NSStreamStatusNotOpen, @"Incorrect stream status");
    
    BOOL hasSpace = [stream hasSpaceAvailable];
    XCTAssertFalse(hasSpace, @"Incorrect hasSpaceAvailable");
    
    NSError *error = [stream streamError];
    XCTAssertNil(error, @"Incorrect error!");
    
    id obj = [stream object];
    XCTAssertNil(obj, @"Object is not nil");
    
    char json[] = "{\"key\":1234}";
    [stream open];
    
    hasSpace = [stream hasSpaceAvailable];
    XCTAssertTrue(hasSpace, @"Incorrect hasSpaceAvailable");
    
    status = [stream streamStatus];
    XCTAssertTrue(status == NSStreamStatusOpen, @"Incorrect stream status");
    
    error = [stream streamError];
    XCTAssertNil(error, @"Incorrect error!");
    
    [stream write:(const uint8_t *)json maxLength:sizeof(json)];
    [stream close];
    
    status = [stream streamStatus];
    XCTAssertTrue(status == NSStreamStatusClosed, @"Incorrect stream status");
    
    hasSpace = [stream hasSpaceAvailable];
    XCTAssertFalse(hasSpace, @"Incorrect hasSpaceAvailable");
    
    OutputStreamModel *model = [stream object];
    XCTAssertNotNil(model, @"Result is nil!");
    
    XCTAssertEqualObjects(model.key, [NSNumber numberWithInt:1234], @"Incorrect value");
    
    [stream release];
}

- (void)testOutputError {
    JsonLiteOutputStream *stream = [[JsonLiteOutputStream alloc] initWithRootClass:[OutputStreamModel class]
                                                                             depth:32];
    char json[] = "asdas";
    [stream open];
    [stream write:(const uint8_t *)json maxLength:sizeof(json)];
    
    NSError *error = [stream streamError];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedObjectOrArray, @"Incorrect error");
    
    [stream write:(const uint8_t *)json maxLength:sizeof(json)];
    error = [stream streamError];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedObjectOrArray, @"Incorrect error");
    
    [stream close];
    [stream release];
}

@end
