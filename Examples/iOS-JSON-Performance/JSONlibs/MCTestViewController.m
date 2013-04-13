//
//  MCTestViewController.m
//  JSONlibs
//
//  Created by Junior Bontognali on 6.12.11.
//  Copyright (c) 2011 Mocha Code. All rights reserved.
//

#import "MCTestViewController.h"
#import "JSONKit.h"
#import "SBJson.h"
#import "CJSONDeserializer.h"
#import "NXJsonParser.h"
#import "JsonLiteAccumulator.h"

@implementation MCTestViewController

@synthesize selectedLibraries = _selectedLibraries;
@synthesize selectedFile;
@synthesize hud = _hud;
@synthesize results = _results;
@synthesize repeats;
@synthesize graphView = _graphView;
@synthesize completed;

- (id)initWithStyle:(UITableViewStyle)style
{
    self = [super initWithStyle:style];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

- (void)dealloc {
    self.hud = nil;
    self.selectedFile = nil;
    [_graphView release];
    [_results release];
    [_selectedLibraries release];
    [super dealloc];
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];

    // Uncomment the following line to preserve selection between presentations.
    // self.clearsSelectionOnViewWillAppear = NO;
 
    // Uncomment the following line to display an Edit button in the navigation bar for this view controller.
    // self.navigationItem.rightBarButtonItem = self.editButtonItem;
    self.title = self.selectedFile;
    self.completed = NO;
    
    _results = [[NSMutableDictionary alloc] initWithCapacity:[_selectedLibraries count]];
    
    _graphView = [[UIWebView alloc] initWithFrame:self.navigationController.view.bounds];
    [_graphView setHidden:YES];
    _graphView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [self.navigationController.view addSubview:_graphView];
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
    _hud = [[MBProgressHUD alloc] initWithView:self.view];
    [_hud setLabelText:@"Parsing..."];
    [_hud show:YES];
    [self.view addSubview:_hud];
    
    [self performSelector:@selector(parse) withObject:nil afterDelay:0.2];
}

- (void)viewWillDisappear:(BOOL)animated
{
    [super viewWillDisappear:animated];
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    if (!self.completed) return NO;
    
    // Return YES for supported orientations
    if (!(interfaceOrientation == UIInterfaceOrientationPortrait)) {
        [[UIApplication sharedApplication] setStatusBarHidden:YES withAnimation:UIStatusBarAnimationSlide];
    } else {
        [[UIApplication sharedApplication] setStatusBarHidden:NO withAnimation:UIStatusBarAnimationSlide];
    }
    return !(interfaceOrientation == UIInterfaceOrientationPortraitUpsideDown);
}

- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
    if (fromInterfaceOrientation == UIInterfaceOrientationPortrait) {
        [_graphView setHidden:NO];
    } else {
        [_graphView setHidden:YES];
    }
}

#pragma mark - Table view data source

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView
{
    // Return the number of sections.
    return 3;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    // Return the number of rows in the section.
    return [_selectedLibraries count];
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    static NSString *CellIdentifier = @"Cell";
    
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:CellIdentifier];
    if (cell == nil) {
        cell = [[[UITableViewCell alloc] initWithStyle:UITableViewCellStyleValue1 reuseIdentifier:CellIdentifier] autorelease];
    }
    
    // Configure the cell...
    cell.textLabel.text = [_selectedLibraries objectAtIndex:indexPath.row];
    
    NSMutableArray *results = [_results objectForKey:[_selectedLibraries objectAtIndex:indexPath.row]];
    
    if (indexPath.section == 0) {
        cell.detailTextLabel.text = [NSString stringWithFormat:@"%.2f ms", [self average:results]];
    } else if (indexPath.section == 1) {
        cell.detailTextLabel.text = [NSString stringWithFormat:@"%.2f ms", [self min:results]];
    } else if (indexPath.section == 2) {
        cell.detailTextLabel.text = [NSString stringWithFormat:@"%.2f ms", [self max:results]];
    }
    
    return cell;
}

- (NSString *)tableView:(UITableView *)tableView titleForHeaderInSection:(NSInteger)section
{
    switch (section) {
        case 0:
            return [NSString stringWithFormat:@"Average (%i repeats)", self.repeats];
            break;
        case 1:
            return @"Min";
            break;
        case 2:
            return @"Max";
            break;
        default:
            break;
    }
    return nil;
}

/*
// Override to support conditional editing of the table view.
- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the specified item to be editable.
    return YES;
}
*/

/*
// Override to support editing the table view.
- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath
{
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        // Delete the row from the data source
        [tableView deleteRowsAtIndexPaths:[NSArray arrayWithObject:indexPath] withRowAnimation:UITableViewRowAnimationFade];
    }   
    else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view
    }   
}
*/

/*
// Override to support rearranging the table view.
- (void)tableView:(UITableView *)tableView moveRowAtIndexPath:(NSIndexPath *)fromIndexPath toIndexPath:(NSIndexPath *)toIndexPath
{
}
*/

/*
// Override to support conditional rearranging of the table view.
- (BOOL)tableView:(UITableView *)tableView canMoveRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Return NO if you do not want the item to be re-orderable.
    return YES;
}
*/

#pragma mark - Table view delegate

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    // Navigation logic may go here. Create and push another view controller.
    /*
     <#DetailViewController#> *detailViewController = [[<#DetailViewController#> alloc] initWithNibName:@"<#Nib name#>" bundle:nil];
     // ...
     // Pass the selected object to the new view controller.
     [self.navigationController pushViewController:detailViewController animated:YES];
     [detailViewController release];
     */
}

#pragma mark - HUD delegate

- (void)hudWasHidden:(MBProgressHUD *)hud
{
    [hud release];
}

#pragma mark - Calculation

- (float)average:(NSArray *)items
{
    if ([items count] == 0) return 0;
    float total = 0.0;
    for (NSNumber *n in items) {
        total+= [n floatValue];
    }
    return total / [items count];
}

- (float)min:(NSArray *)items
{
    if ([items count] == 0) return 0;
    float min = MAXFLOAT;
    for (NSNumber *n in items) {
        if ([n floatValue] < min)
            min = [n floatValue];
    }
    return min;
}

- (float)max:(NSArray *)items
{
    if ([items count] == 0) return 0;
    float max = 0.0;
    for (NSNumber *n in items) {
        if ([n floatValue] > max)
            max = [n floatValue];
    }
    return max;
}

#pragma merk - Build Chard

- (void)buildChart
{
    NSString *pathToTemplate = [[NSBundle mainBundle] pathForResource:@"graph" 
                                                               ofType:@"html" 
                                                          inDirectory:@"/htdocs" ];
    NSString *template = [NSString stringWithContentsOfFile:pathToTemplate 
                                                   encoding:NSUTF8StringEncoding 
                                                      error:nil];
    
    //categories: ['Africa', 'America', 'Asia', 'Europe', 'Oceania']
    NSMutableString *categories = [NSMutableString stringWithString:@"["];
    for (NSString *libname in self.selectedLibraries) {
        [categories appendFormat:@"'%@'", ([libname isEqualToString:@"NSJSONSerialization"]?@"Apple JSON":libname)];
        if (libname != [self.selectedLibraries lastObject])
            [categories appendString:@", "];
    }
    [categories appendString:@"]"];
    
    NSMutableString *series = [NSMutableString stringWithString:@"[{\n name: 'Average', \ndata: ["];
    for (NSString *libname in self.selectedLibraries) {        
        NSMutableArray *results = [_results objectForKey:libname];
        [series appendFormat:@"%.2f", [self average:results]];
        
        if (libname != [self.selectedLibraries lastObject])
            [series appendString:@", "];
    }
    [series appendString:@"]}, \n{name: 'Min', \ndata: ["];
    
    for (NSString *libname in self.selectedLibraries) {        
        NSMutableArray *results = [_results objectForKey:libname];
        [series appendFormat:@"%.2f", [self min:results]];
        
        if (libname != [self.selectedLibraries lastObject])
            [series appendString:@", "];
    }
    [series appendString:@"]}, \n{name: 'Max', \ndata: ["];
    
    for (NSString *libname in self.selectedLibraries) {        
        NSMutableArray *results = [_results objectForKey:libname];
        [series appendFormat:@"%.2f", [self max:results]];
        
        if (libname != [self.selectedLibraries lastObject])
            [series appendString:@", "];
    }
    [series appendString:@"]}\n]"];
    
    NSString *html = [NSString stringWithFormat:template, self.selectedFile, self.repeats, categories, series];
    
    [self.graphView loadHTMLString:html baseURL:[NSURL fileURLWithPath:
                                                 [NSString stringWithFormat:@"%@/htdocs/graph.html", 
                                                  [[NSBundle mainBundle] bundlePath]]]];    
}

#pragma mark - Parser

- (void)parse
{    
    [_hud setLabelText:@"Parsing..."];
    
    NSError *error = nil;
    NSString *pathToFile = [[NSBundle mainBundle] pathForResource:selectedFile ofType:@""];
    NSString *contentFile = [NSString stringWithContentsOfFile:pathToFile encoding:NSUTF8StringEncoding error:&error];
    if (error) {
        NSLog(@"Error: %@", [error description]);
        return;
    }
    NSData *contentData = [contentFile dataUsingEncoding:NSUTF8StringEncoding];
    
    //Libs JSONKit, TouchJSON, NextiveJson, SBJSON, NSJSONSerialization
    
    for (NSString *libName in _selectedLibraries) 
    {
        NSMutableArray *results = [[[NSMutableArray alloc] initWithCapacity:self.repeats] autorelease];
        SEL method = NSSelectorFromString([NSString stringWithFormat:@"parseWith%@:",libName]);
        
        for (int i = 0; i < self.repeats; i++) {
            [results addObject:[self performSelector:method withObject:([libName hasSuffix:@"Data"] ? contentData : contentFile)]];
        }
       
        [_results setObject:results forKey:libName];
    }
    
    [_hud hide:YES];
    [self.tableView reloadData];    
    [self buildChart];
    [self setCompleted:YES];
}

- (NSNumber *)parseWithJsonLite:(NSString *)content {
    NSDate *startTime = [NSDate date];
    NSData *utf8Data = [content dataUsingEncoding:NSUTF8StringEncoding];
    id result = [JsonLiteAccumulator objectFromData:utf8Data
                                       withMaxDepth:32];
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (result == nil)
        elapsedTime = -1.0;
    return [NSNumber numberWithFloat:elapsedTime];
}

- (NSNumber *)parseWithJsonLiteData:(NSData *)content {
    NSDate *startTime = [NSDate date];
    id result = [JsonLiteAccumulator objectFromData:content
                                       withMaxDepth:32];
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (result == nil)
        elapsedTime = -1.0;
    return [NSNumber numberWithFloat:elapsedTime];
}

- (NSNumber *)parseWithJSONKit:(NSString *)content
{
    NSDate *startTime = [NSDate date];
    id result = [content objectFromJSONString];  
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (result == nil)
        elapsedTime = -1.0;
    return [NSNumber numberWithFloat:elapsedTime];
}

- (NSNumber *)parseWithJSONKitData:(NSData *)content
{
    NSDate *startTime = [NSDate date];
    id result = [content objectFromJSONData];  
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (result == nil)
        elapsedTime = -1.0;
    return [NSNumber numberWithFloat:elapsedTime];
}

- (NSNumber *)parseWithJSONKitThread:(NSString *)content
{
    JSONDecoder *decoder = [[[NSThread currentThread] threadDictionary] objectForKey:@"JSONKit Thread Local Decoder"];
    if(decoder == NULL) {
        decoder = [JSONDecoder decoder];
        [[[NSThread currentThread] threadDictionary] setObject:decoder forKey:@"JSONKit Thread Local Decoder"];
    }
    NSDate *startTime = [NSDate date];
    NSData *utf8Data = [content dataUsingEncoding:NSUTF8StringEncoding];
    id result = [decoder objectWithUTF8String:[utf8Data bytes] length:[utf8Data length]];
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (result == nil)
        elapsedTime = -1.0;
    return [NSNumber numberWithFloat:elapsedTime];
}

- (NSNumber *)parseWithJSONKitThreadData:(NSData *)content
{
    JSONDecoder *decoder = [[[NSThread currentThread] threadDictionary] objectForKey:@"JSONKit Thread Local Decoder"];
    if(decoder == NULL) {
        decoder = [JSONDecoder decoder];
        [[[NSThread currentThread] threadDictionary] setObject:decoder forKey:@"JSONKit Thread Local Decoder"];
    }
    NSDate *startTime = [NSDate date];
    id result = [decoder objectWithData:content];
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (result == nil)
        elapsedTime = -1.0;
    return [NSNumber numberWithFloat:elapsedTime];
}

- (NSNumber *)parseWithSBJson:(NSString *)content
{
    NSDate *startTime = [NSDate date];
    // Create SBJSON object to parse JSON
    SBJsonParser *parser = [[SBJsonParser alloc] init];
    id result = [parser objectWithString:content error:nil];
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (result == nil)
        elapsedTime = -1.0;
     return [NSNumber numberWithFloat:elapsedTime];;
}

- (NSNumber *)parseWithSBJsonData:(NSData *)content
{
    NSDate *startTime = [NSDate date];
    // Create SBJSON object to parse JSON
    SBJsonParser *parser = [[SBJsonParser alloc] init];
    id result = [parser objectWithData:content];
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (result == nil)
        elapsedTime = -1.0;
    return [NSNumber numberWithFloat:elapsedTime];;
}


- (NSNumber *)parseWithTouchJSON:(NSString *)content
{
    NSData *jsonData = [content dataUsingEncoding:NSUTF8StringEncoding];
    NSError *error = nil;
    NSDate *startTime = [NSDate date];
    [[CJSONDeserializer deserializer] deserialize:jsonData error:&error];
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (error)
        elapsedTime = -1.0;;
     return [NSNumber numberWithFloat:elapsedTime];;
}

- (NSNumber *)parseWithNextiveJson:(NSString *)content
{
    NSError *error = nil;
	NSDate *startTime = [NSDate date];
	[NXJsonParser parseString:content error:&error ignoreNulls:NO];
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (error)
        elapsedTime = -1.0;;
     return [NSNumber numberWithFloat:elapsedTime];;
}

- (NSNumber *)parseWithNSJSONSerialization:(NSString *)content
{
    NSError *error = nil;
    NSData *data = [content dataUsingEncoding:NSUTF8StringEncoding];
    NSDate *startTime = [NSDate date];
	id result = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingMutableContainers error:&error];
    float elapsedTime = [startTime timeIntervalSinceNow] * -1000;
    if (result == nil)
        elapsedTime = -1.0;;
     return [NSNumber numberWithFloat:elapsedTime];;
}



@end
