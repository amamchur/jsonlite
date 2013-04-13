//
//  MCMasterViewController.h
//  JSONlibs
//
//  Created by Junior Bontognali on 6.12.11.
//  Copyright (c) 2011 Mocha Code. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "MCTestViewController.h"

@interface MCMasterViewController : UITableViewController

@property (strong, nonatomic) NSArray *libraries;
@property (strong, nonatomic) NSArray *files;
@property (strong, nonatomic) NSMutableArray *selectedLibraries;
@property (strong, nonatomic) NSString *selectedFile;
@property (assign, nonatomic) NSInteger repeats;

@end
