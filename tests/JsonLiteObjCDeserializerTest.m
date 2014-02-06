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

#import "JsonLiteObjCDeserializerTest.h"
#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteParser.h"
#import "JsonLiteSerializer.h"
#import "JsonLiteDeserializer.h"
#import "JsonLiteMetaData.h"
#import "JsonLiteObjC.h"

@interface JsonLiteTestGetterSetter : NSObject {
    NSString *someString;
    NSNumber *number;
    Class cls;
    NSString *name;
    NSString *someOtherVar;
    NSString *dynamic;
}

@property (nonatomic, copy, getter = getSomeString, setter = setSomeString:) NSString *str;
@property (nonatomic, readonly, getter = getSomeNumber) NSNumber *number;
@property (nonatomic, retain) Class cls;
@property (nonatomic, readonly) NSString *name;
@property (nonatomic, copy) NSString *redefVariable;
@property (nonatomic, copy) NSString *dynamic;

@end

@interface JsonLiteTestObj1 : NSObject {
    NSString *strValue;
    NSNumber *intValue;
    NSArray *array;
    JsonLiteTestObj1 *child;
}

@property (nonatomic, copy) NSString *strValue;
@property (nonatomic, copy) NSNumber *intValue;
@property (nonatomic, copy) NSArray *array;
@property (nonatomic, retain) JsonLiteTestObj1 *child;

@end

@interface JsonLiteTestObj2 : NSObject {
    NSArray *array;
}

@property (nonatomic, copy) NSArray *array;

@end

@interface JsonLiteTestObj3 : NSObject {
    NSString *identifier;
    NSArray *array;
}

@property (nonatomic, copy) NSString *identifier;
@property (nonatomic, copy) NSArray *array;

@end

@interface JsonLiteTestMatrix : NSObject {
    NSString *name;
    NSArray *matrix;
}

@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy) NSArray *matrix;

@end

@interface JsonLiteTestMatrix2 : NSObject {
    NSString *name;
    NSArray *matrix;
}


@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy) NSArray *matrix;

@end

@implementation JsonLiteTestGetterSetter

@dynamic dynamic;
@synthesize redefVariable = someOtherVar;
@synthesize str = someString;
@synthesize cls;

- (NSString *)name {
    return NSStringFromClass([self class]);
}

- (NSString *)getSomeString {
    return someString;
}

- (void)setSomeString:(NSString *)str {
    if (someString != str) {
        [someString release];
        someString = [str copy];
    }
}

- (NSNumber *)getSomeNumber {
    return [NSNumber numberWithInt:1];
}

- (void)dealloc {
    self.str = nil;
    self.cls = nil;
    self.redefVariable = nil;
    [super dealloc];
}


@end

@implementation JsonLiteTestObj1

@synthesize strValue;
@synthesize intValue;
@synthesize array;
@synthesize child;

- (BOOL)isEqual:(id)object {
    JsonLiteTestObj1 *obj = object;
    BOOL equal = [self.strValue isEqual:obj.strValue];  
    equal = equal && [self.intValue isEqual:obj.intValue];    
    equal = equal && [self.array isEqual:obj.array];
    return equal && (self.child == obj.child || [self.child isEqual:obj.child]);
}

- (void)dealloc {
    [strValue release];
    [intValue release];
    [array release];
    [child release];
    [super dealloc];
}

@end

@implementation JsonLiteTestObj2

@synthesize array;


+ (NSArray *)jsonLiteBindingRules {
    return [NSArray arrayWithObjects:[JsonLiteBindRule ruleForKey:@"array" elementClass:[JsonLiteTestObj1 class]], nil];
}

- (void)dealloc {
    [array release];
    [super dealloc];
}

@end

@implementation JsonLiteTestObj3

@synthesize identifier;
@synthesize array;

+ (NSArray *)jsonLiteBindingRules {
    id b1 = [JsonLiteBindRule ruleForKey:@"id" bindTo:@"identifier"];
    id b2 = [JsonLiteBindRule ruleForKey:@"while" bindTo:@"array" elementClass:[JsonLiteTestObj1 class]];    
    return [NSArray arrayWithObjects:b1, b2, nil];
}

- (void)dealloc {
    [identifier release];
    [array release];
    [super dealloc];
}

@end

@implementation JsonLiteTestMatrix

@synthesize matrix;
@synthesize name;

+ (NSArray *)jsonLiteBindingRules {
    id b1 = [JsonLiteBindRule ruleForKey:@"matrix" elementClass:[JsonLiteTestObj1 class]];
    return [NSArray arrayWithObjects:b1, nil];
}

- (void)dealloc {
    [name release];
    [matrix release];
    [super dealloc];
}

@end

@implementation JsonLiteTestMatrix2

@synthesize matrix;
@synthesize name;

+ (NSArray *)jsonLiteBindingRules {
    id b1 = [JsonLiteBindRule ruleForKey:@"matrix" elementClass:NULL];
    return [NSArray arrayWithObjects:b1, nil];
}

- (void)dealloc {
    [name release];
    [matrix release];
    [super dealloc];
}

@end

@interface JsonLiteObjCDeserializerTest()<JsonLiteDeserializerDelegate>
@end

@implementation JsonLiteObjCDeserializerTest

- (void)deserializer:(JsonLiteDeserializer *)deserializer didDeserializeObject:(id)object {
    XCTAssertNotNil(object, @"Object is nil");
}

- (void)deserializer:(JsonLiteDeserializer *)deserializer didDeserializeArray:(NSArray *)array {
    XCTAssertNotNil(array, @"Array is nil");
}

- (void)testArrays {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"array_values"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error];
    XCTAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:nil];
    XCTAssertNil(deserializer.delegate, @"Delegate is not nil");
    
    deserializer.delegate = self;
    XCTAssertTrue(deserializer.delegate == self, @"Delegate is other object or nil");
    
    deserializer.delegate = self;
    XCTAssertTrue(deserializer.delegate == self, @"Delegate is other object or nil");
    
    parser.delegate = deserializer;
    [parser parse:data];
    NSMutableArray *array = [deserializer object];
    
    XCTAssertTrue([array count] > 0, @"Array is empty");
    XCTAssertNil(error, @"Parse error");

    deserializer.delegate = nil;
    XCTAssertNil(deserializer.delegate, @"Delegate is not nil");
    
  
    [deserializer release];
    deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:nil];
    parser.delegate = deserializer;
    [parser reset];
    [parser parse:data];
    NSMutableArray *array1 = [deserializer object];
    
    XCTAssertTrue([array1 count] > 0, @"Array is empty");
    XCTAssertNil(error, @"Parse error");
    
    [deserializer release];
    [parser release];
    
    XCTAssertTrue([array isEqualToArray:array1], @"Arrays are not equal");
}

- (void)testObj1 {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"obj1"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error];
    XCTAssertNil(error, @"Can not open file");
 
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:[JsonLiteTestObj1 class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteTestObj1 *obj = [deserializer object];
    
    [deserializer release];
    [parser release];
    
    XCTAssertNotNil(obj.strValue, @"strValue was not binded");
    XCTAssertNotNil(obj.intValue, @"intValue was not binded");
    XCTAssertNotNil(obj.array, @"array was not binded");
    XCTAssertTrue([obj.array count] == 2, @"array has invalid size");
    XCTAssertNotNil(obj.child, @"child was not bilded");
    
    JsonLiteTestObj1 *child = obj.child;
    XCTAssertNotNil(child.strValue, @"strValue was not binded");
    XCTAssertNotNil(child.intValue, @"intValue was not binded");
    XCTAssertNotNil(child.array, @"array was not binded");
    XCTAssertTrue([child.array count] == 2, @"array has invalid size");
    XCTAssertNil(child.child, @"child was not binded");
}

- (void)testObj1Chunk {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"obj1"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                    error:&error];
    XCTAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:[JsonLiteTestObj1 class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    
    NSUInteger length = [data length];
    char buffer[1];
    for (NSUInteger i = 0; i < length; ++i) {
        NSRange range = NSMakeRange(i, sizeof(buffer));
        [data getBytes:buffer range:range];
        
        NSData *d = [[NSData alloc] initWithBytesNoCopy:buffer length:range.length freeWhenDone:NO];
        [parser parse:d];
        error = parser.parseError;
        XCTAssertTrue(error == nil || [error code] == JsonLiteCodeEndOfStream, @"Incorrect error");        
        [d release];        
    }
    
    JsonLiteTestObj1 *obj = [deserializer object];
    [deserializer release];
    [parser release];
    
    XCTAssertNotNil(obj.strValue, @"strValue was not binded");
    XCTAssertNotNil(obj.intValue, @"intValue was not binded");
    XCTAssertNotNil(obj.array, @"array was not binded");
    XCTAssertTrue([obj.array count] == 2, @"array has invalid size");
    XCTAssertNotNil(obj.child, @"child was not bilded");
    
    JsonLiteTestObj1 *child = obj.child;
    XCTAssertNotNil(child.strValue, @"strValue was not binded");
    XCTAssertNotNil(child.intValue, @"intValue was not binded");
    XCTAssertNotNil(child.array, @"array was not binded");
    XCTAssertTrue([child.array count] == 2, @"array has invalid size");
    XCTAssertNil(child.child, @"child was not binded");
}

- (void)testObj2 {
    JsonLiteClassMetaData *metadata = [JsonLiteClassMetaData metaDataForClass:[JsonLiteTestObj2 class]];
    NSArray *keys = metadata.keys;
    XCTAssertEqualObjects(keys, @[@"array"], @"Incorrect keys");
    
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"obj2"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error];
    XCTAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:[JsonLiteTestObj2 class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteTestObj2 *obj2 = [deserializer object];
    
    [deserializer release];
    [parser release];
    
    XCTAssertNotNil(obj2.array, @"array was not binded");
    XCTAssertTrue([obj2.array count] == 2, @"array has invalid size");
    
    JsonLiteTestObj1 *obj = [obj2.array lastObject];
    XCTAssertNotNil(obj.strValue, @"strValue was not binded");
    XCTAssertNotNil(obj.intValue, @"intValue was not binded");
    XCTAssertNotNil(obj.array, @"array was not binded");
    XCTAssertTrue([obj.array count] == 2, @"array has invalid size");
    XCTAssertNotNil(obj.child, @"child was not bilded");
    
    JsonLiteTestObj1 *child = obj.child;
    XCTAssertNotNil(child.strValue, @"strValue was not binded");
    XCTAssertNotNil(child.intValue, @"intValue was not binded");
    XCTAssertNotNil(child.array, @"array was not binded");
    XCTAssertTrue([child.array count] == 2, @"array has invalid size");
    XCTAssertNil(child.child, @"child was not binded");
}

- (void)testObj3 {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"obj3"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error];
    XCTAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:[JsonLiteTestObj3 class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteTestObj3 *obj3 = [deserializer object];
    XCTAssertNotNil(obj3.identifier, @"id not binded");
    XCTAssertNotNil(obj3.array, @"array not binded");

    JsonLiteTestObj1 *obj = [obj3.array lastObject];
    XCTAssertNotNil(obj.strValue, @"strValue was not binded");
    XCTAssertNotNil(obj.intValue, @"intValue was not binded");
    XCTAssertNotNil(obj.array, @"array was not binded");
    XCTAssertTrue([obj.array count] == 2, @"array has invalid size");
    XCTAssertNotNil(obj.child, @"child was not bilded");
    
    [deserializer release];
    [parser release];
}

- (void)testMatrix {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"object_matrix"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                    error:&error];
    XCTAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [JsonLiteParser parser];
    JsonLiteDeserializer *deserializer = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteTestMatrix class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteTestMatrix *matrix = [deserializer object];
    XCTAssertNotNil(matrix, @"matrix not binded");
    XCTAssertNotNil(matrix.name, @"name not binded");
    XCTAssertTrue([matrix.matrix count] == 2, @"matrix not binded");
    
    NSArray *array = [matrix.matrix lastObject];
    JsonLiteTestObj1 *obj = [array lastObject];
    XCTAssertNotNil(obj, @"matrix not binded");
    XCTAssertNotNil(obj.strValue, @"strValue was not binded");
    XCTAssertNotNil(obj.intValue, @"intValue was not binded");
    XCTAssertNotNil(obj.array, @"array was not binded");
    XCTAssertTrue([obj.array count] == 2, @"array has invalid size");
    XCTAssertNil(obj.child, @"child was not bilded");
    
    for (NSArray *a in matrix.matrix) {
        for (JsonLiteTestObj1 *o in a) {
            XCTAssertTrue([o isEqual:o], @"Not equal");
        }
    }
}

- (void)testMatrix2 {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"object_matrix"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                    error:&error];
    XCTAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [JsonLiteParser parser];
    JsonLiteDeserializer *deserializer = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteTestMatrix2 class]];
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteTestMatrix2 *matrix = [deserializer object];
    XCTAssertNotNil(matrix, @"matrix not binded");
    XCTAssertNotNil(matrix.name, @"name not binded");
    XCTAssertTrue([matrix.matrix count] == 2, @"matrix not binded");
    XCTAssertTrue([[matrix.matrix objectAtIndex:0] count] == 2, @"");
    XCTAssertTrue([[[matrix.matrix objectAtIndex:0] objectAtIndex:0] isKindOfClass:[NSNull class]], @"");
}

- (void)testSerAndDeser {
    JsonLiteTestObj1 *obj = [[JsonLiteTestObj1 alloc] init];
    obj.strValue = @"Hello";
    obj.intValue = [NSNumber numberWithInt:12345];
    obj.array = [NSArray arrayWithObjects:[NSNumber numberWithInt:321], @"test", nil];
    obj.child =[[[JsonLiteTestObj1 alloc] init] autorelease];
    JsonLiteTestObj1 *child = obj.child;
    child.strValue = @"Hello";
    child.intValue = [NSNumber numberWithInt:12345];
    child.array = [NSArray arrayWithObjects:[NSNumber numberWithInt:321], @"test", nil];
    
    JsonLiteSerializer *ser = [[JsonLiteSerializer alloc] initWithDepth:32];
    NSData *data = [ser serializeObject:obj];
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:[JsonLiteTestObj1 class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    [parser parse:data];
    JsonLiteTestObj1 *obj1 = [deserializer object];
    [ser serializeObject:obj];
    JsonLiteTestObj1 *obj2 = [deserializer object];
    
    XCTAssertTrue(ser.depth == 32, @"Incorrect depth");
    XCTAssertTrue([obj isEqual:obj1], @"Objects not equal");
    XCTAssertTrue([obj1 isEqual:obj2], @"Objects not equal");
    XCTAssertTrue([obj isEqual:obj2], @"Objects not equal");
    [parser release];
    [deserializer release];
    [ser release];
    [obj release];
}

- (void)testMetaData {
    JsonLiteClassMetaData *metaData = [JsonLiteClassMetaData metaDataForClass:nil];
    XCTAssertNil(metaData, @"metaData is not nil");    
    
    metaData = [[JsonLiteClassMetaData alloc] initWithClass:[JsonLiteTestObj1 class]];
    XCTAssertTrue(metaData.objectClass == [JsonLiteTestObj1 class], @"Bad class");
    XCTAssertTrue([metaData.binding count] == 0, @"Bad binding");
    XCTAssertTrue([metaData.keys count] == 4, @"Bad keys");
    XCTAssertNil([metaData propertyToBindKey:@"some key"], @"Bad property");    
    
    NSDictionary *properties = metaData.properties;
    XCTAssertTrue([properties count] == 4, @"Bad count");
        
    JsonLiteClassProperty *prop = [properties objectForKey:@"strValue"];
    XCTAssertTrue([prop.name isEqualToString:@"strValue"], @"Bad name");
    XCTAssertTrue(prop.objectClass == [NSString class], @"Bad class");
    
    prop = [properties objectForKey:@"intValue"];
    XCTAssertTrue([prop.name isEqualToString:@"intValue"], @"Bad name");
    XCTAssertTrue(prop.objectClass == [NSNumber class], @"Bad class");
    
    prop = [properties objectForKey:@"array"];
    XCTAssertTrue([prop.name isEqualToString:@"array"], @"Bad name");
    XCTAssertTrue(prop.objectClass == [NSArray class], @"Bad class");
    
    prop = [properties objectForKey:@"child"];
    XCTAssertTrue([prop.name isEqualToString:@"child"], @"Bad name");
    XCTAssertTrue(prop.objectClass == [JsonLiteTestObj1 class], @"Bad class");
    [metaData release];
    
    metaData = [[JsonLiteClassMetaData alloc] initWithClass:[JsonLiteTestObj3 class]];
    XCTAssertTrue(metaData.objectClass == [JsonLiteTestObj3 class], @"Bad class");
    XCTAssertTrue([metaData.binding count] == 2, @"Bad binding");
    XCTAssertTrue([metaData.keys count] == 2, @"Bad keys");

    JsonLiteBindRule *rule = [metaData.binding objectForKey:@"id"];
    XCTAssertTrue([rule.key isEqualToString:@"id"], @"Bad rule");
    XCTAssertTrue([rule.property isEqualToString:@"identifier"], @"Bad rule");
    XCTAssertTrue(rule.elementClass == nil, @"Bad rule");
                  
    rule = [metaData.binding objectForKey:@"while"];
    XCTAssertTrue([rule.key isEqualToString:@"while"], @"Bad rule");
    XCTAssertTrue([rule.property isEqualToString:@"array"], @"Bad rule");
    XCTAssertTrue(rule.elementClass == [JsonLiteTestObj1 class], @"Bad rule");
    
    [metaData release];
}

- (void)testMetaDataGetterSetter {
    JsonLiteTestGetterSetter *obj = [[JsonLiteTestGetterSetter alloc] init];
    JsonLiteClassMetaData *metaData = [[JsonLiteClassMetaData alloc] initWithClass:[JsonLiteTestGetterSetter class]];
    JsonLiteClassProperty *prop = [metaData.properties objectForKey:@"str"];
    NSString *str = @"hello";
    NSMutableString *mstr =[NSMutableString stringWithString:@"hello"];
    
    [prop setValue:str forObject:obj];
    XCTAssertTrue(str == [prop valueOfObject:obj], @"Bad assigment");
    
    [prop setValue:mstr forObject:obj];
    XCTAssertTrue(mstr != [prop valueOfObject:obj], @"Bad assigment");
    XCTAssertTrue([mstr isEqualToString:[prop valueOfObject:obj]], @"Bad assigment");

    NSNumber *number = [NSNumber numberWithInt:1];
    prop = [metaData.properties objectForKey:@"number"];
    XCTAssertTrue([number isEqualToNumber:[prop valueOfObject:obj]], @"Bad assigment");
    
    prop = [metaData.properties objectForKey:@"cls"];
    [prop setValue:[NSString class] forObject:obj];
    XCTAssertNil([prop valueOfObject:obj], @"Bad assigment");

    prop = [metaData.properties objectForKey:@"name"];
    NSString *name = [prop valueOfObject:obj];
    XCTAssertTrue([name isEqualToString:NSStringFromClass([JsonLiteTestGetterSetter class])], @"Bad getter");
    
    prop = [metaData.properties objectForKey:@"dynamic"];
    XCTAssertNotNil(prop, @"No property getter");
    
    prop = [metaData.properties objectForKey:@"redefVariable"];
    [prop setValue:str forObject:obj];
    XCTAssertTrue(str == [prop valueOfObject:obj], @"Bad assigment");
    
    [obj release];
    [metaData release];
}

- (void)testHelpers{
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"obj1"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                    error:&error];

    JsonLiteTestObj1 *obj = [JsonLiteObjC objectFromData:data
                                                 rootCls:[JsonLiteTestObj1 class]
                                         convertersChain:nil];
    
    XCTAssertNotNil(obj.strValue, @"strValue was not binded");
    XCTAssertNotNil(obj.intValue, @"intValue was not binded");
    XCTAssertNotNil(obj.array, @"array was not binded");
    XCTAssertTrue([obj.array count] == 2, @"array has invalid size");
    XCTAssertNotNil(obj.child, @"child was not bilded");
    
    JsonLiteTestObj1 *child = obj.child;
    XCTAssertNotNil(child.strValue, @"strValue was not binded");
    XCTAssertNotNil(child.intValue, @"intValue was not binded");
    XCTAssertNotNil(child.array, @"array was not binded");
    XCTAssertTrue([child.array count] == 2, @"array has invalid size");
    XCTAssertNil(child.child, @"child was not binded");
    
    obj = [JsonLiteObjC objectFromData:data
                               rootCls:[JsonLiteTestObj1 class]
                       convertersChain:nil
                                 depth:16];
    
    XCTAssertNotNil(obj.strValue, @"strValue was not binded");
    XCTAssertNotNil(obj.intValue, @"intValue was not binded");
    XCTAssertNotNil(obj.array, @"array was not binded");
    XCTAssertTrue([obj.array count] == 2, @"array has invalid size");
    XCTAssertNotNil(obj.child, @"child was not bilded");
    
    child = obj.child;
    XCTAssertNotNil(child.strValue, @"strValue was not binded");
    XCTAssertNotNil(child.intValue, @"intValue was not binded");
    XCTAssertNotNil(child.array, @"array was not binded");
    XCTAssertTrue([child.array count] == 2, @"array has invalid size");
    XCTAssertNil(child.child, @"child was not binded");
    
    obj = [JsonLiteObjC objectFromData:data
                               rootCls:[JsonLiteTestObj1 class]
                       convertersChain:nil
                                 depth:16
                                 error:&error];
    XCTAssertNil(error, @"Error occurs");
    XCTAssertNotNil(obj.strValue, @"strValue was not binded");
    XCTAssertNotNil(obj.intValue, @"intValue was not binded");
    XCTAssertNotNil(obj.array, @"array was not binded");
    XCTAssertTrue([obj.array count] == 2, @"array has invalid size");
    XCTAssertNotNil(obj.child, @"child was not bilded");
    
    child = obj.child;
    XCTAssertNotNil(child.strValue, @"strValue was not binded");
    XCTAssertNotNil(child.intValue, @"intValue was not binded");
    XCTAssertNotNil(child.array, @"array was not binded");
    XCTAssertTrue([child.array count] == 2, @"array has invalid size");
    XCTAssertNil(child.child, @"child was not binded");
    
    obj = [JsonLiteObjC objectFromData:[data bytes]
                                length:[data length]
                               rootCls:[JsonLiteTestObj1 class]
                       convertersChain:nil
                                 depth:16];
    XCTAssertNil(error, @"Error occurs");
    XCTAssertNotNil(obj.strValue, @"strValue was not binded");
    XCTAssertNotNil(obj.intValue, @"intValue was not binded");
    XCTAssertNotNil(obj.array, @"array was not binded");
    XCTAssertTrue([obj.array count] == 2, @"array has invalid size");
    XCTAssertNotNil(obj.child, @"child was not bilded");
    
    child = obj.child;
    XCTAssertNotNil(child.strValue, @"strValue was not binded");
    XCTAssertNotNil(child.intValue, @"intValue was not binded");
    XCTAssertNotNil(child.array, @"array was not binded");
    XCTAssertTrue([child.array count] == 2, @"array has invalid size");
    XCTAssertNil(child.child, @"child was not binded");
    
    NSData *d1 = [JsonLiteObjC dataFromObject:obj];
    NSData *d2 = [JsonLiteObjC dataFromObject:obj convertersChain:nil];
    XCTAssertEqualObjects(d1, d2, @"Different results");
}

@end
