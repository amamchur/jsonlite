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

#import "JsonLiteObjCUnicodeTests.h"
#import "JsonLiteSenTestCaseExt.h"

#import "JsonLiteAccumulator.h"
#import "JsonLiteSerializer.h"

typedef struct UnicodeTestCtx {
    NSDictionary *dict;
    JsonLiteObjCUnicodeTests *test;
} UnicodeTestCtx;

static void string_found(jsonlite_callback_context *, jsonlite_token *);
static void string_token_found(jsonlite_callback_context *, jsonlite_token *);

@implementation JsonLiteObjCUnicodeTests

- (void)testUnicode {
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    [dict setObject:@"Quizdeltagerne spiste jordbær med fløde, mens cirkusklovnen Wolther spillede på xylofon. \n"
             forKey:@"Danish"];
    [dict setObject:@"Heizölrückstoßabdämpfung \n"
             forKey:@"German"];
    [dict setObject:@"Γαζέες καὶ μυρτιὲς δὲν θὰ βρῶ πιὰ στὸ χρυσαφὶ ξέφωτο \n"
             forKey:@"Greek"];
    [dict setObject:@"El pingüino Wenceslao hizo kilómetros bajo exhaustiva lluvia y frío, añoraba a su querido cachorro. \n"
             forKey:@"Spanish"];
    [dict setObject:@"Portez ce vieux whisky au juge blond qui fume sur son île intérieure, à côté de l'alcôve ovoïde, où les bûches se consument dans l'âtre, ce qui lui permet de penser à la cænogenèse de l'être dont il est question     dans la cause ambiguë entendue à Moÿ, dans un capharnaüm qui, pense-t-il, diminue çà et là la qualité de son œuvre \n"
             forKey:@"French"];
    [dict setObject:@"D'fhuascail Íosa, Úrmhac na hÓighe Beannaithe, pór Éava agus Ádhaimh"
             forKey:@"Irish Gaelic"];
    [dict setObject:@"Árvíztűrő tükörfúrógép \n"
             forKey:@"Hungarian"];
    [dict setObject:@"The quick brown fox jumps over the lazy dog \n"
             forKey:@"English"];
    [dict setObject:@"いろはにほへとちりぬるを \n"
             forKey:@"Hiragana"];
    [dict setObject:@"イロハニホヘト チリヌルヲ ワカヨタレソ ツネナラム \n"
             forKey:@"Katakana"];
    [dict setObject:@"? דג סקרן שט בים מאוכזב ולפתע מצא לו חברה איך הקליטה \n"
             forKey:@"Hebrew"];
    [dict setObject:@"Pchnąć w tę łódź jeża lub ośm skrzyń fig \n"
             forKey:@"Polish"];
    [dict setObject:@"Съешь же ещё этих мягких французских булок да выпей чаю \n"
             forKey:@"Russian"];
    [dict setObject:@"๏ เป็นมนุษย์สุดประเสริฐเลิศคุณค่า  กว่าบรรดาฝูงสัตว์เดรัจฉาน \n"
             forKey:@"Thai"];
    [dict setObject:@"Pijamalı hasta, yağız şoföre çabucak güvendi. \n"
             forKey:@"Turkish"];
    [dict setObject:@"𝄞 ♔♩ ♪ ♫ ♬ ♭ ♮ ♯ \n"
             forKey:@"Miscellaneous Symbols"];
    [dict setObject:@"\n \t \\ \" \b \f \t \r"
             forKey:@"Escape"];
    
    JsonLiteSerializer *jls =[[JsonLiteSerializer alloc] init];
    jls.indentation = 4;
    NSData *data = [jls serializeObject:dict];
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteAccumulator *delegate = [[JsonLiteAccumulator alloc] init];
    parser.delegate = delegate;
    
    [parser parse:data];
    id object = [delegate object];
    
    [delegate release];
    [parser release];
    [jls release];
    
    BOOL equal = [self compareDictionary:dict withDictionary:object];
    STAssertTrue(equal, @"Not equal");
    
    UnicodeTestCtx ctx = { dict, self};
    jsonlite_parser p = jsonlite_parser_init(256);
    jsonlite_parser_callbacks cbs = jsonlite_default_callbacks;
    cbs.context.client_state = &ctx;
    cbs.string_found = string_found;
    
    jsonlite_result result = jsonlite_parser_set_callback(NULL, &cbs);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Bad error");
    
    result = jsonlite_parser_set_callback(p, NULL);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Bad error");
    
    result = jsonlite_parser_set_callback(NULL, NULL);
    STAssertTrue(result == jsonlite_result_invalid_argument, @"Bad error");
    
    result = jsonlite_parser_set_callback(p, &cbs);
    STAssertTrue(result == jsonlite_result_ok, @"Bad error");

    jsonlite_parser_tokenize(p, [data bytes], [data length]);
    jsonlite_parser_release(p);
}

- (void)testInvalidUnicodeEscape {
    // "[\"\\uD835\\uDEE2\"]"
    char json1[] = "[\"\\uD835       \"]";
    jsonlite_parser ps = jsonlite_parser_init(4);
    jsonlite_result result = jsonlite_parser_tokenize(ps, json1, sizeof(json1));
    STAssertTrue(result == jsonlite_result_invalid_escape, @"Bad error code");
    jsonlite_parser_release(ps);
    
    char json2[] = "[\"\\uD835\\     \"]";
    ps = jsonlite_parser_init(4);
    result = jsonlite_parser_tokenize(ps, json2, sizeof(json2));
    STAssertTrue(result == jsonlite_result_invalid_escape, @"Bad error code");
    jsonlite_parser_release(ps);
    
    char json3[] = "[\"\\uD835\\uQEE2\"]";
    ps = jsonlite_parser_init(4);
    result = jsonlite_parser_tokenize(ps, json3, sizeof(json3));
    STAssertTrue(result == jsonlite_result_invalid_escape, @"Bad error code");
    jsonlite_parser_release(ps);
    
    char json4[] = "[\"\\uD835\\u0020\"]";
    ps = jsonlite_parser_init(4);
    result = jsonlite_parser_tokenize(ps, json4, sizeof(json4));
    STAssertTrue(result == jsonlite_result_invalid_escape, @"Bad error code");
    jsonlite_parser_release(ps);
    
    char json5[] = "[\"\\uD835\\uE000\"]";
    ps = jsonlite_parser_init(4);
    result = jsonlite_parser_tokenize(ps, json5, sizeof(json5));
    STAssertTrue(result == jsonlite_result_invalid_escape, @"Bad error code");
    jsonlite_parser_release(ps);
}

- (void)testInvalidUTF8 {
    NSMutableData *data = [NSMutableData data];
    NSString *header = @"{\"obj\": \"Text ";
    NSString *footer = @"\"}";
    
    [data appendData:[header dataUsingEncoding:NSUTF8StringEncoding]];
    [data appendData:[footer dataUsingEncoding:NSUTF8StringEncoding]];
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    NSError *error = nil;
    [parser parse:data];
    STAssertNil(error, @"Parse error");
    {
        data = [NSMutableData data];
        [data appendData:[header dataUsingEncoding:NSUTF8StringEncoding]];
        const char CONTROL_CHARS[] = {0x41 , 0x1B};
        [data appendBytes:CONTROL_CHARS length:sizeof(CONTROL_CHARS)];
        [data appendData:[footer dataUsingEncoding:NSUTF8StringEncoding]];
        
        error = nil;
        [parser reset];
        [parser parse:data];
        
        STAssertTrue([parser.parseError code] == JsonLiteCodeNotSupportedControlChar, @"Bad error!");
    }
    {
        data = [NSMutableData data];
        [data appendData:[header dataUsingEncoding:NSUTF8StringEncoding]];
        const char BAD_SEQ_1[] = {0xDD};
        [data appendBytes:BAD_SEQ_1 length:sizeof(BAD_SEQ_1)];
        [data appendData:[footer dataUsingEncoding:NSUTF8StringEncoding]];
        
        error = nil;
        [parser reset];
        [parser parse:data];
        
        STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidUTF8, @"Bad error!");
    }
    {
        data = [NSMutableData data];
        [data appendData:[header dataUsingEncoding:NSUTF8StringEncoding]];
        const char BAD_SEQ_2_1[] = {0xEA, 0xEE};
        [data appendBytes:BAD_SEQ_2_1 length:sizeof(BAD_SEQ_2_1)];
        [data appendData:[footer dataUsingEncoding:NSUTF8StringEncoding]];
        
        error = nil;
        [parser reset];
        [parser parse:data];
        
        STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidUTF8, @"Bad error!");
    }
    {
        data = [NSMutableData data];
        [data appendData:[header dataUsingEncoding:NSUTF8StringEncoding]];
        const char BAD_SEQ_3_1[] = {0xE5, 0xEE};
        [data appendBytes:BAD_SEQ_3_1 length:sizeof(BAD_SEQ_3_1)];
        [data appendData:[footer dataUsingEncoding:NSUTF8StringEncoding]];
        
        error = nil;
        [parser reset];
        [parser parse:data];
        
        STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidUTF8, @"Bad error!");
    }
    {
        data = [NSMutableData data];
        [data appendData:[header dataUsingEncoding:NSUTF8StringEncoding]];
        const char BAD_SEQ_3_2[] = {0xE5, 0x99, 0xEE};
        [data appendBytes:BAD_SEQ_3_2 length:sizeof(BAD_SEQ_3_2)];
        [data appendData:[footer dataUsingEncoding:NSUTF8StringEncoding]];
        
        error = nil;
        [parser reset];
        [parser parse:data];
        
        STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidUTF8, @"Bad error!");
    }
    {
        data = [NSMutableData data];
        [data appendData:[header dataUsingEncoding:NSUTF8StringEncoding]];
        const char BAD_SEQ_4_1[] = {0xF0, 0xEE};
        [data appendBytes:BAD_SEQ_4_1 length:sizeof(BAD_SEQ_4_1)];
        [data appendData:[footer dataUsingEncoding:NSUTF8StringEncoding]];
        
        error = nil;
        [parser reset];
        [parser parse:data];
        
        STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidUTF8, @"Bad error!");
    }
    {
        data = [NSMutableData data];
        [data appendData:[header dataUsingEncoding:NSUTF8StringEncoding]];
        const char BAD_SEQ_4_2[] = {0xF0, 0x99, 0xEE};
        [data appendBytes:BAD_SEQ_4_2 length:sizeof(BAD_SEQ_4_2)];
        [data appendData:[footer dataUsingEncoding:NSUTF8StringEncoding]];
        
        error = nil;
        [parser reset];
        [parser parse:data];
        
        STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidUTF8, @"Bad error!");
    }
    {
        data = [NSMutableData data];
        [data appendData:[header dataUsingEncoding:NSUTF8StringEncoding]];
        const char BAD_SEQ_4_3[] = {0xF0, 0x99, 0x99, 0xEE};
        [data appendBytes:BAD_SEQ_4_3 length:sizeof(BAD_SEQ_4_3)];
        [data appendData:[footer dataUsingEncoding:NSUTF8StringEncoding]];
        
        error = nil;
        [parser reset];
        [parser parse:data];
        
        STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidUTF8, @"Bad error!");
    }
    {
        data = [NSMutableData data];
        [data appendData:[header dataUsingEncoding:NSUTF8StringEncoding]];
        const char BAD_SEQ_5_1[] = {0xF8, 0x99, 0x99, 0xEE};
        [data appendBytes:BAD_SEQ_5_1 length:sizeof(BAD_SEQ_5_1)];
        [data appendData:[footer dataUsingEncoding:NSUTF8StringEncoding]];
        
        error = nil;
        [parser reset];
        [parser parse:data];
        
        STAssertTrue([parser.parseError code] == JsonLiteCodeInvalidUTF8, @"Bad error!");
    }
    
    [parser release];
};


- (void)testCoverUnicodeFull {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"sample"
                                                                      ofType:@"json"
                                                                 inDirectory:@"success"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error];
    STAssertNil(error, @"Cann't read file %@, error - %@.", @"sample", error);
    
    
    jsonlite_parser p = jsonlite_parser_init(512);
    jsonlite_parser_callbacks cbs = jsonlite_default_callbacks;
    cbs.context.client_state = self;
    cbs.string_found = string_token_found;
    cbs.key_found = string_token_found;
    jsonlite_parser_set_callback(p, &cbs);

    jsonlite_result result = jsonlite_parser_tokenize(p, [data bytes], [data length]);
    STAssertTrue(result == jsonlite_result_ok, @"Parse error");
    jsonlite_parser_release(p);
}

- (void)fastFails:(BOOL)fail {
    STAssertFalse(fail, @"Failed test case from C code callback");
}

@end

static void string_found(jsonlite_callback_context *ctx, struct jsonlite_token *token) {
    UnicodeTestCtx *context = (UnicodeTestCtx *)ctx->client_state;
    NSDictionary *dict = context->dict;
    uint8_t *buffer = NULL;
    size_t size = jsonlite_token_decode_to_uft8(token, &buffer);
    
    NSString *str = [[NSString alloc] initWithBytes:buffer length:size encoding:NSUTF8StringEncoding];
    [str autorelease];
    free(buffer);
    
    BOOL fails = YES;
    for (id key in dict) {
        id value = [dict objectForKey:key];
        if ([value isEqual:str]) {
            fails = NO;
            break;
        }
    }
    
    [context->test fastFails:fails];
}

static void string_token_found(jsonlite_callback_context *ctx, struct jsonlite_token *token) {
    uint8_t *buffer_utf8 = NULL;
    uint16_t *buffer_utf16 = NULL;
    size_t size = jsonlite_token_decode_to_uft8(token, &buffer_utf8);
    
    NSString *str1 = [[NSString alloc] initWithBytes:buffer_utf8 length:size encoding:NSUTF8StringEncoding];
    [str1 autorelease];
    free(buffer_utf8);
    
    size = jsonlite_token_decode_to_uft16(token, &buffer_utf16);
    NSString *str2 = (NSString *)CFStringCreateWithBytesNoCopy(NULL,
                                                               (const UInt8 *)buffer_utf16,
                                                               size,
                                                               kCFStringEncodingUnicode,
                                                               NO,
                                                               kCFAllocatorDefault);
    [str2 autorelease];
    
    JsonLiteObjCUnicodeTests *testCase = (JsonLiteObjCUnicodeTests *)ctx->client_state;
    [testCase fastFails:![str1 isEqualToString:str2]];
}
