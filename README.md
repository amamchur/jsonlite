jsonlite
========
![Image](../master/JsonLiteObjC/cov.png?raw=true)

JsonLite ObjC
====================
JsonLite ObjC - the fastest JSON parser for Objective-C/Cocoa.

If you:

* Need fastest JSON parser
* Are limited to memory
* Need chunk/streaming processing
* Need binding JSON to model
* Need to build JSON from model
* Deal with decimal, url or dates
* Deal with raw tokens

**Try JsonLite Objective-C**

### Getting Started
Current example shows how to quick tokenize and accumulate results to Cocoa collection.

``` objective-c
#import "JsonLiteAccumulator.h"

// ...

- (void)parse {
    NSString *json = @"[\"hello\", null, 1234567890]";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id result = [JsonLiteAccumulator objectFromData:data withMaxDepth:8];
    NSLog(@"%@", result);
}

// ...
```
Lets play with chunks :)
``` objective-c
#import "JsonLiteParser.h"
#import "JsonLiteAccumulator.h"

// ...

- (void)parseChunks {
    NSString *json_part1 = @"[\"hello\", nu";
    NSString *json_part2 = @"ll, 1234567890]";
    NSData *data1 = [json_part1 dataUsingEncoding:NSUTF8StringEncoding];
    NSData *data2 = [json_part2 dataUsingEncoding:NSUTF8StringEncoding];
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:8];
    JsonLiteAccumulator *acc = [JsonLiteAccumulator accumulatorWithDepth:8];
    parser.delegate = acc;
    [parser parse:data1];
    [parser parse:data2];    
    NSLog(@"Full object - %@", [acc object]);
}

// ...
```

Model rules!!! It's really hard to deal with Cocoa collections. The best way is to bind JSON to some model.
``` objective-c
#import "JsonLiteParser.h"
#import "JsonLiteDeserializer.h"

@interface Model : NSObject

@property (nonatomic, copy) NSString *string;
@property (nonatomic, copy) NSNumber *number;
@property (nonatomic, copy) NSArray *array;

@end

@implementation Model

- (void)dealloc {
    self.string = nil;
    self.number = nil;
    self.array = nil;
    [super dealloc];
}

@end
// ...
- (void)parseToModel {
    NSString *json = @"{\"string\": \"hello\", \"number\" : 100, \"array\": [1, 2, 3]}";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:8];
    JsonLiteDeserializer *des = [JsonLiteDeserializer deserializerWithRootClass:[Model class]];
    parser.delegate = des;
    [parser parse:data];
    
    Model *model = [des object];
    NSLog(@"String - %@", model.string);
    NSLog(@"Number - %@", model.number);
    NSLog(@"Array - %@", model.array);
}
// ...
```

And model to JSON.
``` objective-c

#import "JsonLiteSerializer.h"

- (void)buildFromModel {
    Model *model = [[[Model alloc] init] autorelease];
    model.string = @"Hello World";
    model.number = [NSNumber numberWithInt:256];
    model.array = [NSArray arrayWithObjects:@"Test", [NSNull null], nil];
    JsonLiteSerializer *serializer = [JsonLiteSerializer serializer];
    NSData *data = [serializer serializeObject:model];
    NSString *json = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    NSLog(@"%@", json);
    [json release];
}
```

Decimal & JSON - now it is not problem! Just use converters (JsonLiteDecimal in our case).

``` objective-c
@interface Model : NSObject

@property (nonatomic, copy) NSString *string;
@property (nonatomic, copy) NSDecimalNumber *number;

@end

@implementation Model

- (void)dealloc {
    self.string = nil;
    self.number = nil;
    [super dealloc];
}

@end

- (void)parseDecimalToModel {
    NSString *json = @"{\"string\": \"This is Decimal!\", \"number\" : 95673465936453649563978.99 }";
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    JsonLiteParser *parser = [JsonLiteParser parserWithDepth:8];
    JsonLiteDeserializer *des = [JsonLiteDeserializer deserializerWithRootClass:[Model class]];
    des.converter = [[[JsonLiteDecimal alloc] init] autorelease]; // Look here
    parser.delegate = des;
    [parser parse:data];
    
    Model *model = [des object];
    NSLog(@"String - %@", model.string);
    NSLog(@"Number - %@", model.number);
}
```
