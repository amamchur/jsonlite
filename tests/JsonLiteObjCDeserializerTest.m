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

#import "JsonLiteObjCDeserializerTest.h"
#import "JsonLiteSenTestCaseExt.h"
#import "JsonLiteParser.h"
#import "JsonLiteSerializer.h"
#import "JsonLiteDeserializer.h"
#import "JsonLiteMetaData.h"

@interface JsonLiteTestGetterSetter : NSObject {
    NSString *someString;
}

@property (nonatomic, copy, getter = getSomeString, setter = setSomeString:) NSString *str;
@property (nonatomic, readonly, getter = getSomeNumber) NSNumber *number;
@property (nonatomic, retain) Class cls;
@property (nonatomic, readonly) NSString *name;
@property (nonatomic, copy) NSString *redefVariable;
@property (nonatomic, copy) NSString *dynamic;

@end

@interface JsonLiteTestObj1 : NSObject

@property (nonatomic, copy) NSString *strValue;
@property (nonatomic, copy) NSNumber *intValue;
@property (nonatomic, copy) NSArray *array;
@property (nonatomic, retain) JsonLiteTestObj1 *child;

@end

@interface JsonLiteTestObj2 : NSObject

@property (nonatomic, copy) NSArray *array;

@end

@interface JsonLiteTestObj3 : NSObject

@property (nonatomic, copy) NSString *identifier;
@property (nonatomic, copy) NSArray *array;

@end

@interface JsonLiteTestMatrix : NSObject

@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy) NSArray *matrix;

@end

@interface JsonLiteTestMatrix2 : NSObject

@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy) NSArray *matrix;

@end

@implementation JsonLiteTestGetterSetter

@dynamic dynamic;
@synthesize redefVariable = someOtherVar;
@synthesize str = someString;

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

- (BOOL)isEqual:(id)object {
    JsonLiteTestObj1 *obj = object;
    BOOL equal = [self.strValue isEqual:obj.strValue];  
    equal = equal && [self.intValue isEqual:obj.intValue];    
    equal = equal && [self.array isEqual:obj.array];
    return equal && (self.child == obj.child || [self.child isEqual:obj.child]);
}

- (void)dealloc {
    [_strValue release];
    [_intValue release];
    [_array release];
    [_child release];
    [super dealloc];
}

@end

@implementation JsonLiteTestObj2

+ (NSArray *)jsonLiteBindingRules {
    return [NSArray arrayWithObjects:[JsonLiteBindRule ruleForKey:@"array" elementClass:[JsonLiteTestObj1 class]], nil];
}

- (void)dealloc {
    [_array release];
    [super dealloc];
}

@end

@implementation JsonLiteTestObj3

+ (NSArray *)jsonLiteBindingRules {
    id b1 = [JsonLiteBindRule ruleForKey:@"id" bindTo:@"identifier"];
    id b2 = [JsonLiteBindRule ruleForKey:@"while" bindTo:@"array" elementClass:[JsonLiteTestObj1 class]];    
    return [NSArray arrayWithObjects:b1, b2, nil];
}

- (void)dealloc {
    [_identifier release];
    [_array release];
    [super dealloc];
}

@end

@implementation JsonLiteTestMatrix

+ (NSArray *)jsonLiteBindingRules {
    id b1 = [JsonLiteBindRule ruleForKey:@"matrix" elementClass:[JsonLiteTestObj1 class]];
    return [NSArray arrayWithObjects:b1, nil];
}

- (void)dealloc {
    [_name release];
    [_matrix release];
    [super dealloc];
}

@end

@implementation JsonLiteTestMatrix2

+ (NSArray *)jsonLiteBindingRules {
    id b1 = [JsonLiteBindRule ruleForKey:@"matrix" elementClass:NULL];
    return [NSArray arrayWithObjects:b1, nil];
}

- (void)dealloc {
    [_name release];
    [_matrix release];
    [super dealloc];
}

@end

@interface JsonLiteObjCDeserializerTest()<JsonLiteDeserializerDelegate>
@end

@implementation JsonLiteObjCDeserializerTest

- (void)deserializer:(JsonLiteDeserializer *)deserializer didDeserializeObject:(id)object {
    STAssertNotNil(object, @"Object is nil");
}

- (void)deserializer:(JsonLiteDeserializer *)deserializer didDeserializeArray:(NSArray *)array {
    STAssertNotNil(array, @"Array is nil");
}

- (void)testArrays {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"array_values"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error];
    STAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:nil];
    STAssertNil(deserializer.delegate, @"Delegate is not nil");
    
    deserializer.delegate = self;
    STAssertTrue(deserializer.delegate == self, @"Delegate is other object or nil");
    
    deserializer.delegate = self;
    STAssertTrue(deserializer.delegate == self, @"Delegate is other object or nil");
    
    parser.delegate = deserializer;
    [parser parse:data];
    NSMutableArray *array = [deserializer object];
    
    STAssertTrue([array count] > 0, @"Array is empty");
    STAssertNil(error, @"Parse error");

    deserializer.delegate = nil;
    STAssertNil(deserializer.delegate, @"Delegate is not nil");
    
  
    [deserializer release];
    deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:nil];
    parser.delegate = deserializer;
    [parser reset];
    [parser parse:data];
    NSMutableArray *array1 = [deserializer object];
    
    STAssertTrue([array1 count] > 0, @"Array is empty");
    STAssertNil(error, @"Parse error");
    
    [deserializer release];
    [parser release];
    
    STAssertTrue([array isEqualToArray:array1], @"Arrays are not equal");
}

- (void)testObj1 {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"obj1"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error];
    STAssertNil(error, @"Can not open file");
 
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:[JsonLiteTestObj1 class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteTestObj1 *obj = [deserializer object];
    
    [deserializer release];
    [parser release];
    
    STAssertNotNil(obj.strValue, @"strValue was not binded");
    STAssertNotNil(obj.intValue, @"intValue was not binded");
    STAssertNotNil(obj.array, @"array was not binded");
    STAssertTrue([obj.array count] == 2, @"array has invalid size");
    STAssertNotNil(obj.child, @"child was not bilded");
    
    JsonLiteTestObj1 *child = obj.child;
    STAssertNotNil(child.strValue, @"strValue was not binded");
    STAssertNotNil(child.intValue, @"intValue was not binded");
    STAssertNotNil(child.array, @"array was not binded");
    STAssertTrue([child.array count] == 2, @"array has invalid size");
    STAssertNil(child.child, @"child was not binded");
}

- (void)testObj1Chunk {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"obj1"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                    error:&error];
    STAssertNil(error, @"Can not open file");
    
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
        STAssertTrue(error == nil || [error code] == JsonLiteCodeEndOfStream, @"Incorrect error");        
        [d release];        
    }
    
    JsonLiteTestObj1 *obj = [deserializer object];
    [deserializer release];
    [parser release];
    
    STAssertNotNil(obj.strValue, @"strValue was not binded");
    STAssertNotNil(obj.intValue, @"intValue was not binded");
    STAssertNotNil(obj.array, @"array was not binded");
    STAssertTrue([obj.array count] == 2, @"array has invalid size");
    STAssertNotNil(obj.child, @"child was not bilded");
    
    JsonLiteTestObj1 *child = obj.child;
    STAssertNotNil(child.strValue, @"strValue was not binded");
    STAssertNotNil(child.intValue, @"intValue was not binded");
    STAssertNotNil(child.array, @"array was not binded");
    STAssertTrue([child.array count] == 2, @"array has invalid size");
    STAssertNil(child.child, @"child was not binded");
}

- (void)testObj2 {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"obj2"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error];
    STAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:[JsonLiteTestObj2 class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteTestObj2 *obj2 = [deserializer object];
    
    [deserializer release];
    [parser release];
    
    STAssertNotNil(obj2.array, @"array was not binded");
    STAssertTrue([obj2.array count] == 2, @"array has invalid size");
    
    JsonLiteTestObj1 *obj = [obj2.array lastObject];
    STAssertNotNil(obj.strValue, @"strValue was not binded");
    STAssertNotNil(obj.intValue, @"intValue was not binded");
    STAssertNotNil(obj.array, @"array was not binded");
    STAssertTrue([obj.array count] == 2, @"array has invalid size");
    STAssertNotNil(obj.child, @"child was not bilded");
    
    JsonLiteTestObj1 *child = obj.child;
    STAssertNotNil(child.strValue, @"strValue was not binded");
    STAssertNotNil(child.intValue, @"intValue was not binded");
    STAssertNotNil(child.array, @"array was not binded");
    STAssertTrue([child.array count] == 2, @"array has invalid size");
    STAssertNil(child.child, @"child was not binded");
}

- (void)testObj3 {
    NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"obj3"
                                                                      ofType:@"json"
                                                                 inDirectory:@"deserializer"];
    NSError *error = nil;
    NSData *data = [[NSData alloc] initWithContentsOfFile:path
                                                  options:0
                                                   error:&error];
    STAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:[JsonLiteTestObj3 class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteTestObj3 *obj3 = [deserializer object];
    STAssertNotNil(obj3.identifier, @"id not binded");
    STAssertNotNil(obj3.array, @"array not binded");

    JsonLiteTestObj1 *obj = [obj3.array lastObject];
    STAssertNotNil(obj.strValue, @"strValue was not binded");
    STAssertNotNil(obj.intValue, @"intValue was not binded");
    STAssertNotNil(obj.array, @"array was not binded");
    STAssertTrue([obj.array count] == 2, @"array has invalid size");
    STAssertNotNil(obj.child, @"child was not bilded");
    
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
    STAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [JsonLiteParser parser];
    JsonLiteDeserializer *deserializer = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteTestMatrix class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteTestMatrix *matrix = [deserializer object];
    STAssertNotNil(matrix, @"matrix not binded");
    STAssertNotNil(matrix.name, @"name not binded");
    STAssertTrue([matrix.matrix count] == 2, @"matrix not binded");
    
    NSArray *array = [matrix.matrix lastObject];
    JsonLiteTestObj1 *obj = [array lastObject];
    STAssertNotNil(obj, @"matrix not binded");
    STAssertNotNil(obj.strValue, @"strValue was not binded");
    STAssertNotNil(obj.intValue, @"intValue was not binded");
    STAssertNotNil(obj.array, @"array was not binded");
    STAssertTrue([obj.array count] == 2, @"array has invalid size");
    STAssertNil(obj.child, @"child was not bilded");
    
    for (NSArray *a in matrix.matrix) {
        for (JsonLiteTestObj1 *o in a) {
            STAssertTrue([o isEqual:o], @"Not equal");
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
    STAssertNil(error, @"Can not open file");
    
    JsonLiteParser *parser = [JsonLiteParser parser];
    JsonLiteDeserializer *deserializer = [JsonLiteDeserializer deserializerWithRootClass:[JsonLiteTestMatrix2 class]];
    parser.delegate = deserializer;
    [parser parse:data];
    
    JsonLiteTestMatrix2 *matrix = [deserializer object];
    STAssertNotNil(matrix, @"matrix not binded");
    STAssertNotNil(matrix.name, @"name not binded");
    STAssertTrue([matrix.matrix count] == 2, @"matrix not binded");
    STAssertTrue([[matrix.matrix objectAtIndex:0] count] == 2, @"");
    STAssertTrue([[[matrix.matrix objectAtIndex:0] objectAtIndex:0] isKindOfClass:[NSNull class]], @"");
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
    
    JsonLiteSerializer *ser = [[JsonLiteSerializer alloc] init];
    NSData *data = [ser serializeObject:obj];
    JsonLiteParser *parser = [[JsonLiteParser alloc] init];
    JsonLiteDeserializer *deserializer = [[JsonLiteDeserializer alloc] initWithRootClass:[JsonLiteTestObj1 class]];
    deserializer.delegate = self;
    parser.delegate = deserializer;
    [parser parse:data];
    JsonLiteTestObj1 *obj1 = [deserializer object];
    [ser serializeObject:obj];
    JsonLiteTestObj1 *obj2 = [deserializer object];
    
    STAssertTrue([obj isEqual:obj1], @"Objects not equal");
    STAssertTrue([obj1 isEqual:obj2], @"Objects not equal");
    STAssertTrue([obj isEqual:obj2], @"Objects not equal");
    [parser release];
    [deserializer release];
    [ser release];
    [obj release];
}

- (void)testMetaData {
    JsonLiteClassMetaData *metaData = [JsonLiteClassMetaData metaDataForClass:nil];
    STAssertNil(metaData, @"metaData is not nil");    
    
    metaData = [[JsonLiteClassMetaData alloc] initWithClass:[JsonLiteTestObj1 class]];
    STAssertTrue(metaData.objectClass == [JsonLiteTestObj1 class], @"Bad class");
    STAssertTrue([metaData.binding count] == 0, @"Bad binding");
    STAssertTrue([metaData.keys count] == 4, @"Bad keys");
    STAssertNil([metaData propertyToBindKey:@"some key"], @"Bad property");    
    
    NSDictionary *properties = metaData.properties;
    STAssertTrue([properties count] == 4, @"Bad count");
        
    JsonLiteClassProperty *prop = [properties objectForKey:@"strValue"];
    STAssertTrue([prop.name isEqualToString:@"strValue"], @"Bad name");
    STAssertTrue(prop.objectClass == [NSString class], @"Bad class");
    
    prop = [properties objectForKey:@"intValue"];
    STAssertTrue([prop.name isEqualToString:@"intValue"], @"Bad name");
    STAssertTrue(prop.objectClass == [NSNumber class], @"Bad class");
    
    prop = [properties objectForKey:@"array"];
    STAssertTrue([prop.name isEqualToString:@"array"], @"Bad name");
    STAssertTrue(prop.objectClass == [NSArray class], @"Bad class");
    
    prop = [properties objectForKey:@"child"];
    STAssertTrue([prop.name isEqualToString:@"child"], @"Bad name");
    STAssertTrue(prop.objectClass == [JsonLiteTestObj1 class], @"Bad class");
    [metaData release];
    
    metaData = [[JsonLiteClassMetaData alloc] initWithClass:[JsonLiteTestObj3 class]];
    STAssertTrue(metaData.objectClass == [JsonLiteTestObj3 class], @"Bad class");
    STAssertTrue([metaData.binding count] == 2, @"Bad binding");
    STAssertTrue([metaData.keys count] == 2, @"Bad keys");

    JsonLiteBindRule *rule = [metaData.binding objectForKey:@"id"];
    STAssertTrue([rule.key isEqualToString:@"id"], @"Bad rule");
    STAssertTrue([rule.property isEqualToString:@"identifier"], @"Bad rule");
    STAssertTrue(rule.elementClass == nil, @"Bad rule");
                  
    rule = [metaData.binding objectForKey:@"while"];
    STAssertTrue([rule.key isEqualToString:@"while"], @"Bad rule");
    STAssertTrue([rule.property isEqualToString:@"array"], @"Bad rule");
    STAssertTrue(rule.elementClass == [JsonLiteTestObj1 class], @"Bad rule");
    
    [metaData release];
}

- (void)testMetaDataGetterSetter {
    JsonLiteTestGetterSetter *obj = [[JsonLiteTestGetterSetter alloc] init];
    JsonLiteClassMetaData *metaData = [[JsonLiteClassMetaData alloc] initWithClass:[JsonLiteTestGetterSetter class]];
    JsonLiteClassProperty *prop = [metaData.properties objectForKey:@"str"];
    NSString *str = @"hello";
    NSMutableString *mstr =[NSMutableString stringWithString:@"hello"];
    
    [prop setValue:str forObject:obj];
    STAssertTrue(str == [prop valueOfObject:obj], @"Bad assigment");
    
    [prop setValue:mstr forObject:obj];
    STAssertTrue(mstr != [prop valueOfObject:obj], @"Bad assigment");
    STAssertTrue([mstr isEqualToString:[prop valueOfObject:obj]], @"Bad assigment");

    NSNumber *number = [NSNumber numberWithInt:1];
    prop = [metaData.properties objectForKey:@"number"];
    STAssertTrue([number isEqualToNumber:[prop valueOfObject:obj]], @"Bad assigment");
    
    prop = [metaData.properties objectForKey:@"cls"];
    [prop setValue:[NSString class] forObject:obj];
    STAssertTrue([NSString class] == [prop valueOfObject:obj], @"Bad assigment");

    prop = [metaData.properties objectForKey:@"name"];
    NSString *name = [prop valueOfObject:obj];
    STAssertTrue([name isEqualToString:NSStringFromClass([JsonLiteTestGetterSetter class])], @"Bad getter");
    
    prop = [metaData.properties objectForKey:@"dynamic"];
    STAssertNotNil(prop, @"No property getter");
    
    prop = [metaData.properties objectForKey:@"redefVariable"];
    [prop setValue:str forObject:obj];
    STAssertTrue(str == [prop valueOfObject:obj], @"Bad assigment");
    
    [obj release];
    [metaData release];
}

@end
