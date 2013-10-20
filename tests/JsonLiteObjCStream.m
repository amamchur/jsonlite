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
    
    STAssertNil(parser.parseError, @"Error occurs %@", parser.parseError);
}

- (void)testInputs {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"random"
                                                                      ofType:@"json"
                                                                 inDirectory:@"success"];
    NSInputStream *stream = [NSInputStream inputStreamWithFileAtPath:path];
    JsonLiteParser *parser = [JsonLiteParser parser];
    [parser parse:nil inRunLoop:nil];
    STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Incorrect error");
    
    [parser parse:stream inRunLoop:nil];
    STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Incorrect error");
    
    [parser parse:nil inRunLoop:[NSRunLoop currentRunLoop]];
    STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidArgument, @"Incorrect error");
}

@end
