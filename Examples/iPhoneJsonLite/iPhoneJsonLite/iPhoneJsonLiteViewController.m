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
#import "JsonLiteParser.h"
#import "JsonLiteAccumulator.h"
#import "JsonLiteSerializer.h"
#import "JsonLiteDeserializer.h"
#import "JsonLiteConverters.h"
#import "TWTimeline.h"

#include "jsonlite.h"
#import <mach/mach.h>
#import <mach/clock.h>
#include <sys/resource.h>

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
    [super dealloc];
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle


// Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
- (void)viewDidLoad
{    
    NSString *filePath = [[NSBundle mainBundle] pathForResource:@"repeat" ofType:@"json"];
    self.data = [NSData dataWithContentsOfFile:filePath];
    [super viewDidLoad];
}


- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    return (interfaceOrientation == UIInterfaceOrientationPortrait);
}

- (IBAction)parse:(id)sender {
    clock_serv_t cclock;
    mach_timespec_t start, end;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &start);

    const int COUNT = 1000;
    for (int i = 0; i < COUNT; i++) {
        JsonLiteParser *parser = [[JsonLiteParser alloc] initWithDepth:512];
        JsonLiteAccumulator *acc = [[JsonLiteAccumulator alloc] initWithDepth:512];
        parser.delegate = acc;
        [parser parse:data];
        // [acc object];
        [acc release];
        [parser release];
    }
    
    clock_get_time(cclock, &end);
        
    long long time = (end.tv_sec - start.tv_sec) * 1000000000;
    time += (end.tv_nsec - start.tv_nsec);
    
    size_t l = [data length];
    double speed = ((double)l * COUNT / 1024 / 1024) / ((double)time / 1000000000.0);
    
    NSString *jsonlite = [NSString stringWithFormat:@"Time - %lld µs; speed - %.0f MBps", time, speed];
    UIAlertView *av = [[UIAlertView alloc] initWithTitle:@""
                                                 message:jsonlite
                                                delegate:nil
                                       cancelButtonTitle:@"OK"
                                       otherButtonTitles:nil];
    [av show];
    [av release];
}

- (IBAction)parse1:(id)sender {
    const char *buffer = [data bytes];
    size_t l = [data length];
    
    clock_serv_t cclock;
    mach_timespec_t start, end;
    host_get_clock_service(mach_host_self(), SYSTEM_CLOCK, &cclock);
    clock_get_time(cclock, &start);
    
    const int COUNT = 1;
    for (int i = 0; i < COUNT; i++) {
        jsonlite_parser p = jsonlite_parser_init(512);
        jsonlite_result res = jsonlite_parser_tokenize(p, buffer, l);
        NSAssert(res == jsonlite_result_ok, @"");
        jsonlite_parser_release(p);
    }
    
    clock_get_time(cclock, &end);
    
    long long time = (end.tv_sec - start.tv_sec) * 1000000000;
    time += (end.tv_nsec - start.tv_nsec);
    
    double speed = ((double)l * COUNT / 1024 / 1024) / ((double)time / 1000000000.0);
    
    NSString *jsonlite = [NSString stringWithFormat:@"Time - %lld µs; speed - %.0f MBps", time, speed];
    UIAlertView *av = [[UIAlertView alloc] initWithTitle:@""
                                                 message:jsonlite
                                                delegate:nil
                                       cancelButtonTitle:@"OK"
                                       otherButtonTitles:nil];
    [av show];
    [av release];
}


- (IBAction)build:(id)sender {
    if (obj == nil) {
        return;
    }
    
    JsonLiteSerializer *serializer = [[JsonLiteSerializer alloc] init];
    serializer.indentation = 4;
    serializer.converter = [JsonLiteConverter converters];
    NSData *jsonData = [serializer serializeObject:obj];
    [serializer release];
    
    NSString *str = [[NSString alloc] initWithUTF8String:[jsonData bytes]];
    NSLog(@"\n%@", str);
    [str release];
}

@end
