//
//  TwitterJsonLiteViewController.m
//  TwitterJsonLite
//
//  Created by Andriy Mamchur on 6/9/12.
//  Copyright 2012 soft serve. All rights reserved.
//

#import "TwitterJsonLiteViewController.h"
#import "JsonLiteParser.h"
#import "JsonLiteDeserializer.h"
#import "JsonLiteConverters.h"
#import "TWTimeline.h"
#import "TWUser.h"

@interface TwitterJsonLiteViewController()<JsonLiteDeserializerDelegate>

@property (nonatomic, retain) NSMutableArray *timelines;
@property (nonatomic, retain) NSURLConnection *connection;
@property (nonatomic, retain) JsonLiteParser *parser;
@property (nonatomic, retain) JsonLiteDeserializer *deserializer;

@end

@implementation TwitterJsonLiteViewController

@synthesize table;
@synthesize timelines;
@synthesize connection;
@synthesize parser;
@synthesize deserializer;

- (void)deserializer:(JsonLiteDeserializer *)deserializer didDeserializeObject:(id)object {
    if ([object isKindOfClass:[TWTimeline class]]) {
        [timelines addObject:object];
        _label.text = [NSString stringWithFormat:@"%d", [timelines count]];
        [table reloadData];
    }
}

- (void)deserializer:(JsonLiteDeserializer *)deserializer didDeserializeArray:(NSArray *)array {    
}

- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response {
    self.parser = [JsonLiteParser parserWithDepth:16];
    self.deserializer = [[[JsonLiteDeserializer alloc] initWithRootClass:[TWTimeline class]] autorelease];
    deserializer.converter = [JsonLiteConverter converters];
    deserializer.delegate = self;
    parser.delegate = deserializer;
}

- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data {
    [parser parse:data];
}

- (void)connectionDidFinishLoading:(NSURLConnection *)connection {
    self.parser = nil;
    self.deserializer = nil;
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error {
    self.parser = nil;
    self.deserializer = nil;
}

- (void)awakeFromNib {
    [super awakeFromNib];
}

- (void)dealloc {
    self.table = nil;
    self.timelines = nil;
    self.connection = nil;
    [_label release];
    [super dealloc];
}

#pragma mark - View lifecycle

- (void)viewDidUnload {
    self.table = nil;
    [self setLabel:nil];
    [super viewDidUnload];
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return [timelines count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *cellId = @"timeline";
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellId];
    if (cell == nil) {
        cell = [[UITableViewCell alloc] initWithStyle:UITableViewCellStyleDefault
                                      reuseIdentifier:cellId];
        [cell autorelease];
    }
    TWTimeline *timeline = [timelines objectAtIndex:indexPath.row];
    cell.textLabel.text = timeline.text;
    return cell;
}

- (IBAction)onLoad:(id)sender {
    self.timelines = [NSMutableArray array];
    [table reloadData];
    
    NSURL *url = [[NSURL alloc] initWithString:@"https://api.twitter.com/1/statuses/user_timeline.json?include_entities=true&include_rts=true&screen_name=twitterapi&count=5000"];
    NSURLRequest *request = [[NSURLRequest alloc] initWithURL:url];
    NSURLConnection *c = [[NSURLConnection alloc] initWithRequest:request
                                                         delegate:self];
    self.connection = c;
    [c release];
    [request release];
    [url release];    
}

@end
