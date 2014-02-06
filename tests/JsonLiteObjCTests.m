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

#include "jsonlite.h"

#import "JsonLiteObjCTests.h"
#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteObjC.h"

@interface JsonLiteObjCTests()<JsonLiteAccumulatorDelegate>

@end

@implementation JsonLiteObjCTests

- (void)testSingleObject {
    id object = [self parseObjectFromFile:@"single_object" inDir:@"success"];
    XCTAssertTrue(object != nil, @"Object is nil.");
    XCTAssertTrue([object isEqual:[NSDictionary dictionary]], @"Incorect parse object.");
}

- (void)testSingleArray {
    id object = [self parseObjectFromFile:@"single_array" inDir:@"success"];
    XCTAssertTrue(object != nil, @"Object is nil.");
    XCTAssertTrue([object isEqual:[NSArray array]], @"Incorect parse object."); 
}

- (void)testTokenPool {
    id object = [self parseObjectFromFile:@"random" inDir:@"success"];
    XCTAssertTrue(object != nil, @"Object is nil.");
}

- (void)testArrayValues {
    id object = [self parseObjectFromFile:@"array_values" inDir:@"success"];
    XCTAssertTrue(object != nil, @"Object is nil.");
    NSMutableArray *array = [NSMutableArray arrayWithCapacity:13];
    [array addObject:[NSNull null]];
    [array addObject:[NSNumber numberWithBool:YES]];
    [array addObject:[NSNumber numberWithBool:NO]];
    [array addObject:@"string"];
    [array addObject:[NSNumber numberWithInt:0]];
    [array addObject:[NSNumber numberWithInt:1]];
    [array addObject:[NSNumber numberWithInt:-1]];
    [array addObject:[NSNumber numberWithInt:123456789]];
    [array addObject:[NSNumber numberWithInt:-123456789]];
    [array addObject:[NSNumber numberWithDouble:0.5]];
    [array addObject:[NSNumber numberWithDouble:-0.5]];
    [array addObject:[NSNumber numberWithDouble:1.5]];
    [array addObject:[NSNumber numberWithDouble:-1.5]];
    [array addObject:[NSNumber numberWithDouble:1.5e5]];
    [array addObject:[NSNumber numberWithDouble:-1.5e5]];
    [array addObject:[NSNumber numberWithDouble:1.5e5]];
    [array addObject:[NSNumber numberWithDouble:-1.5e5]];
    [array addObject:[NSNumber numberWithDouble:1.5e5]];
    [array addObject:[NSNumber numberWithDouble:-1.5e5]];
    [array addObject:[NSNumber numberWithDouble:1.5e-5]];
    [array addObject:[NSNumber numberWithDouble:-1.5e-5]];
    [array addObject:[NSNumber numberWithDouble:0.5e5]];
    [array addObject:[NSNumber numberWithDouble:-0.5e5]];
    [array addObject:[NSNumber numberWithDouble:0.5e5]];
    [array addObject:[NSNumber numberWithDouble:-0.5e5]];
    [array addObject:[NSNumber numberWithDouble:0.5e-5]];
    [array addObject:[NSNumber numberWithDouble:-0.5e-5]];
    [array addObject:[NSNumber numberWithDouble:0.5e-5]];
    [array addObject:[NSNumber numberWithDouble:-0.5e-5]];
    XCTAssertTrue([array count] == [object count], @"Different length %lu and %lu.", (unsigned long)[array count], (unsigned long)[object count]);
    for (NSInteger i = 0; i < [array count]; i++) {
        id obj1 = [array objectAtIndex:i];
        id obj2 = [object objectAtIndex:i];
        XCTAssertTrue([obj1 isEqual:obj2], @"#%ld Object is not equals: %@ == %@", (long)i, obj1, obj2);
    }
    
    NSException *exc = nil;
    @try {
        [object objectAtIndex:[array count]];
    }
    @catch (NSException *exception) {
        exc = [[exception retain] autorelease];
    }
    XCTAssertTrue([[exc name] isEqualToString:NSRangeException], @"Bad exception");
}

- (void)testObjectValues {
    id object = [self parseObjectFromFile:@"object_values" inDir:@"success"];
    XCTAssertTrue(object != nil, @"Object is nil.");
    NSMutableDictionary *dict = [NSMutableDictionary dictionary];
    [dict setObject:[NSNull null] forKey:@"v0"];
    [dict setObject:[NSNumber numberWithBool:YES] forKey:@"v1"];
    [dict setObject:[NSNumber numberWithBool:NO] forKey:@"v2"];
    [dict setObject:@"string" forKey:@"v3"];
    [dict setObject:[NSNumber numberWithInt:0] forKey:@"v4"];
    [dict setObject:[NSNumber numberWithInt:1] forKey:@"v5"];
    [dict setObject:[NSNumber numberWithInt:-1] forKey:@"v6"];
    [dict setObject:[NSNumber numberWithInt:123456789] forKey:@"v7"];
    [dict setObject:[NSNumber numberWithInt:-123456789] forKey:@"v8"];
    [dict setObject:[NSNumber numberWithDouble:0.5] forKey:@"v9"];
    [dict setObject:[NSNumber numberWithDouble:-0.5] forKey:@"v10"];
    [dict setObject:[NSNumber numberWithDouble:1.5] forKey:@"v11"];
    [dict setObject:[NSNumber numberWithDouble:-1.5] forKey:@"v12"];
    [dict setObject:[NSNumber numberWithDouble:1.5e5] forKey:@"v13"];
    [dict setObject:[NSNumber numberWithDouble:-1.5e5] forKey:@"v14"];
    [dict setObject:[NSNumber numberWithDouble:1.5e5] forKey:@"v15"];
    [dict setObject:[NSNumber numberWithDouble:-1.5e5] forKey:@"v16"];
    [dict setObject:[NSNumber numberWithDouble:1.5e5] forKey:@"v17"];
    [dict setObject:[NSNumber numberWithDouble:-1.5e5] forKey:@"v18"];
    [dict setObject:[NSNumber numberWithDouble:1.5e-5] forKey:@"v19"];
    [dict setObject:[NSNumber numberWithDouble:-1.5e-5] forKey:@"v20"];
    [dict setObject:[NSNumber numberWithDouble:0.5e5] forKey:@"v21"];
    [dict setObject:[NSNumber numberWithDouble:-0.5e5] forKey:@"v22"];
    [dict setObject:[NSNumber numberWithDouble:0.5e5] forKey:@"v23"];
    [dict setObject:[NSNumber numberWithDouble:-0.5e5] forKey:@"v24"];
    [dict setObject:[NSNumber numberWithDouble:0.5e-5] forKey:@"v25"];
    [dict setObject:[NSNumber numberWithDouble:-0.5e-5] forKey:@"v26"];
    [dict setObject:[NSNumber numberWithDouble:0.5e-5] forKey:@"v27"];
    [dict setObject:[NSNumber numberWithDouble:-0.5e-5] forKey:@"v28"];
    [dict setObject:[NSNumber numberWithDouble:-123.1234e04] forKey:@"v29"];
    XCTAssertTrue([dict count] == [object count], @"Different length %lu and %lu.", (unsigned long)[dict count], (unsigned long)[object count]);
    NSArray *array1 = [[dict allKeys] sortedArrayUsingSelector:@selector(compare:)];
    NSArray *array2 = [[object allKeys] sortedArrayUsingSelector:@selector(compare:)];
    
    XCTAssertNotNil([object objectForKey:@"v1"], @"Object is nil");
    XCTAssertNil([object objectForKey:@"v1????"], @"Object not is nil");
    XCTAssertTrue([array1 isEqualToArray:array2], @"Array not equals");
    
    for (id key in object) {
        XCTAssertTrue([array1 indexOfObject:key] != NSNotFound, @"No key");
    }
    
    NSEnumerator *en = [object keyEnumerator];
    id obj;
    while (obj = [en nextObject]) {
        XCTAssertNotNil(obj, @"Is nil");
    }
    XCTAssertNotNil([en allObjects], @"Is nil");
    
    
    for (id key in array1) {
        id obj1 = [dict objectForKey:key];
        id obj2 = [object objectForKey:key];
        XCTAssertTrue([obj1 isEqual:obj2], @"#%@ Object is not equals: %@ == %@", key, obj1, obj2);
    }
}

- (void)testStrings {
    const char MATHEMATICAL_ITALIC_CAPITAL_ALPHA[] = {0xf0, 0x9d, 0x9b, 0xa2, 0};
    const char MUSICAL_SYMBOL_G_CLEF[] = {0xf0, 0x9d, 0x84, 0x9e, 0};
    
    id object = [self parseObjectFromFile:@"strings" inDir:@"success"];
    XCTAssertTrue(object != nil, @"Object is nil.");
    NSMutableArray *array = [NSMutableArray arrayWithCapacity:13];
    [array addObject:@""];
    [array addObject:@"Test"];
    [array addObject:@"\nTe\nst\n"];
    [array addObject:@"\r\n\t\\/\b\f\""];
    [array addObject:@" spaces "];
    [array addObject:@"/"];
    [array addObject:@"/unescaped/slashes"];
    [array addObject:@"/escaped/slashes"];
    
    [array addObject:[NSString stringWithUTF8String:MATHEMATICAL_ITALIC_CAPITAL_ALPHA]];
    [array addObject:@"\u00E9"];
    [array addObject:[NSString stringWithUTF8String:MUSICAL_SYMBOL_G_CLEF]];
    [array addObject:@"42\u00e942\u226542"];
    XCTAssertTrue([array count] == [object count], @"Different length %lu and %lu.", (unsigned long)[array count], (unsigned long)[object count]);
    for (NSInteger i = 0; i < [array count]; i++) {
        id obj1 = [array objectAtIndex:i];
        id obj2 = [object objectAtIndex:i];
        XCTAssertTrue([obj1 isEqual:obj2], @"#%ld Object is not equals: %@ == %@", (long)i, obj1, obj2);
    }
}

- (void)testFail1 {
    NSError *error = [self parseErrorFromFile:@"fail1" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedObjectOrArray, @"Test failed.");
}

- (void)testFail2 {
    NSError *error = [self parseErrorFromFile:@"fail2" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeEndOfStream, @"Test failed.");
}

- (void)testFail3 {
    NSError *error = [self parseErrorFromFile:@"fail3" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedKeyOrEnd, @"Test failed.");
}

- (void)testFail4 {
    NSError *error = [self parseErrorFromFile:@"fail4" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedValue, @"Test failed.");
}

- (void)testFail5 {
    NSError *error = [self parseErrorFromFile:@"fail5" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedValue, @"Test failed.");
}

- (void)testFail6 {
    NSError *error = [self parseErrorFromFile:@"fail6" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedValue, @"Test failed.");
}

- (void)testFail7 {
    NSError *error = [self parseErrorFromFile:@"fail7" inDir:@"fail"];
    XCTAssertTrue(error == nil, @"Test failed.");
}

- (void)testFail8 {
    NSError *error = [self parseErrorFromFile:@"fail8" inDir:@"fail"];
    XCTAssertTrue(error == nil, @"Test failed.");
}

- (void)testFail9 {
    NSError *error = [self parseErrorFromFile:@"fail9" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedKey, @"Test failed.");
}

- (void)testFail10 {
    NSError *error = [self parseErrorFromFile:@"fail10" inDir:@"fail"];
    XCTAssertTrue(error == nil, @"Test failed.");
}

- (void)testFail11 {
    NSError *error = [self parseErrorFromFile:@"fail11" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedCommaOrEnd, @"Test failed.");
}

- (void)testFail12 {
    NSError *error = [self parseErrorFromFile:@"fail12" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedValue, @"Test failed.");
}

- (void)testFail13 {
    NSError *error = [self parseErrorFromFile:@"fail13" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Test failed.");
}

- (void)testFail14 {
    NSError *error = [self parseErrorFromFile:@"fail14" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Test failed.");
}

- (void)testFail15 {
    NSError *error = [self parseErrorFromFile:@"fail15" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidEscape, @"Test failed.");
}

- (void)testFail16 {
    NSError *error = [self parseErrorFromFile:@"fail16" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedValue, @"Test failed.");
}

- (void)testFail17 {
    NSError *error = [self parseErrorFromFile:@"fail17" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidEscape, @"Test failed.");
}

- (void)testFail18 {
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:6];
    NSData *data = [self dataFromFile:@"fail18" inDir:@"fail"];
    [parser parse:data];
    XCTAssertTrue([parser.parseError code] == JsonLiteCodeDepthLimit, @"Test failed.");
}

- (void)testFail19 {
    NSError *error = [self parseErrorFromFile:@"fail19" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedColon, @"Test failed.");
}

- (void)testFail20 {
    NSError *error = [self parseErrorFromFile:@"fail20" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedValue, @"Test failed.");
}

- (void)testFail21 {
    NSError *error = [self parseErrorFromFile:@"fail21" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedColon, @"Test failed.");
}

- (void)testFail22 {
    NSError *error = [self parseErrorFromFile:@"fail22" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedCommaOrEnd, @"Test failed.");
}

- (void)testFail23 {
    NSError *error = [self parseErrorFromFile:@"fail23" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Test failed.");
}

- (void)testFail24 {
    NSError *error = [self parseErrorFromFile:@"fail24" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedValue, @"Test failed.");
}

- (void)testFail25 {
    NSError *error = [self parseErrorFromFile:@"fail25" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Test failed.");
}

- (void)testFail26 {
    NSError *error = [self parseErrorFromFile:@"fail26" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidEscape, @"Test failed.");
}

- (void)testFail27 {
    NSError *error = [self parseErrorFromFile:@"fail27" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidToken, @"Test failed.");
}

- (void)testFail28 {
    NSError *error = [self parseErrorFromFile:@"fail28" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidEscape, @"Test failed.");
}

- (void)testFail29 {
    NSError *error = [self parseErrorFromFile:@"fail29" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Test failed.");
}

- (void)testFail30 {
    NSError *error = [self parseErrorFromFile:@"fail30" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Test failed.");
}

- (void)testFail31 {
    NSError *error = [self parseErrorFromFile:@"fail31" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeInvalidNumber, @"Test failed.");
}

- (void)testFail32 {
    NSError *error = [self parseErrorFromFile:@"fail32" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeEndOfStream, @"Test failed.");
}

- (void)testFail33 {
    NSError *error = [self parseErrorFromFile:@"fail33" inDir:@"fail"];
    XCTAssertTrue([error code] == JsonLiteCodeExpectedCommaOrEnd, @"Test failed.");
}

- (void)testParserInStack {
    NSData *data = [self dataFromFile:@"random" inDir:@"success"];
    uint8_t memory[200];
    jsonlite_parser parser = jsonlite_parser_init_memory(memory, sizeof(memory), jsonlite_null_buffer);
    jsonlite_result result = jsonlite_parser_tokenize(parser, [data bytes], [data length]);
    XCTAssertTrue(result == jsonlite_result_ok, @"Incorrect result");
}

- (void)testHelpers{
    NSData *data = [self dataFromFile:@"single_object" inDir:@"success"];
    id obj = [JsonLiteObjC objectFromData:data];
    XCTAssertEqualObjects(obj, [NSDictionary dictionary], @"Incorrect result");
    
    obj = [JsonLiteObjC objectFromData:data depth:4];
    XCTAssertEqualObjects(obj, [NSDictionary dictionary], @"Incorrect result");
    
    obj = [JsonLiteObjC objectFromData:[data bytes] length:[data length]];
    XCTAssertEqualObjects(obj, [NSDictionary dictionary], @"Incorrect result");
    
    obj = [JsonLiteObjC objectFromData:[data bytes] length:[data length] depth:4];
    XCTAssertEqualObjects(obj, [NSDictionary dictionary], @"Incorrect result");
    
    NSError *error = nil;
    obj = [JsonLiteObjC objectFromData:[data bytes] length:[data length] depth:4 error:&error];
    XCTAssertNil(error, @"Error occurs");
    XCTAssertEqualObjects(obj, [NSDictionary dictionary], @"Incorrect result");
}

@end
