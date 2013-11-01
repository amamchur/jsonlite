//  Copyright 2012, Andrii Mamchur
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

#import "iPhoneJsonLiteViewController.h"
#import "TWTimeline.h"

#import <mach/mach.h>
#import <mach/clock.h>
#import <sys/resource.h>

#import "JsonLiteObjC/JsonLiteObjC.h"

@interface iPhoneJsonLiteViewController()<JsonLiteAccumulatorDelegate>
@end

@implementation iPhoneJsonLiteViewController

@synthesize obj;
@synthesize data;

- (void)accumulator:(JsonLiteAccumulator *)accumulator didAccumulateArray:(NSArray *)array {
    if (accumulator.currentDepth > 2) {
        NSLog(@"%@", array);
    }
}

- (void)accumulator:(JsonLiteAccumulator *)accumulator didAccumulateDictionary:(NSDictionary *)dictionary {
    if (accumulator.currentDepth > 2) {
        NSLog(@"%@", dictionary);
    }
}

- (void)dealloc
{
    self.obj = nil;
    self.data = nil;
#if !__has_feature(objc_arc)
    [super dealloc];
#endif
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad {    
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"mesh" ofType:@"json"];
    self.data = [NSData dataWithContentsOfFile:filePath];
    [super viewDidLoad];
}

- (IBAction)parse:(id)sender {
    clock_serv_t cclock;
    mach_timespec_t start, end;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &start);

    size_t l = [data length];
    const int COUNT = 100;
    for (int i = 0; i < COUNT; i++) {
        @autoreleasepool {
            JsonLiteParser *parser = [JsonLiteParser parserWithDepth:512];
            JsonLiteAccumulator *acc = [JsonLiteAccumulator accumulatorWithDepth:512];
            parser.delegate = acc;
            [parser parse:data];
        }
    }
    
    clock_get_time(cclock, &end);
        
    double time = (double)(end.tv_sec - start.tv_sec);
    time += (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
    double speed = (double)l * COUNT / 1024 / 1024 / time;
    
    NSString *jsonlite = [NSString stringWithFormat:@"Time - %f µs\nSpeed - %.2f MBps", time, speed];
    UIAlertView *av = [[UIAlertView alloc] initWithTitle:@""
                                                 message:jsonlite
                                                delegate:nil
                                       cancelButtonTitle:@"OK"
                                       otherButtonTitles:nil];
    [av show];
#if !__has_feature(objc_arc)
    [av release];
#endif
}

- (IBAction)parse1:(id)sender {
    const char *buffer = [data bytes];
    size_t l = [data length];
    
    clock_serv_t cclock;
    mach_timespec_t start, end;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &start);
    
    const int COUNT = 100;
    char parser[1024];
    for (int i = 0; i < COUNT; i++) {
        jsonlite_parser p = jsonlite_parser_init_memory(parser, sizeof(parser));
        jsonlite_parser_tokenize(p, buffer, l);
        jsonlite_parser_cleanup(p);
    }
    
    clock_get_time(cclock, &end);
    
    double time = (double)(end.tv_sec - start.tv_sec);
    time += (double)(end.tv_nsec - start.tv_nsec) / 1000000000;
    double speed = (double)l * COUNT / 1024 / 1024 / time;
    
    NSString *jsonlite = [NSString stringWithFormat:@"Time - %f µs\nSpeed - %.2f MBps", time, speed];
    UIAlertView *av = [[UIAlertView alloc] initWithTitle:@""
                                                 message:jsonlite
                                                delegate:nil
                                       cancelButtonTitle:@"OK"
                                       otherButtonTitles:nil];
    [av show];
#if !__has_feature(objc_arc)
    [av release];
#endif
}


- (IBAction)build:(id)sender {
    if (obj == nil) {
        return;
    }
    
    JsonLiteSerializer *serializer = [JsonLiteSerializer serializer];
    serializer.indentation = 4;
    serializer.converter = [JsonLiteConverter converters];
    NSData *jsonData = [serializer serializeObject:obj];
    
    NSString *str = [[NSString alloc] initWithUTF8String:[jsonData bytes]];
    NSLog(@"\n%@", str);
#if !__has_feature(objc_arc)
    [str release];
#endif
}

@end
