//
//  TwitterJsonLiteViewController.h
//  TwitterJsonLite
//
//  Created by Andriy Mamchur on 6/9/12.
//  Copyright 2012 soft serve. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TwitterJsonLiteViewController : UIViewController<UITableViewDataSource, UITableViewDelegate> {    
    UITableView *table;
}

@property (nonatomic, retain) IBOutlet UITableView *table;
@property (retain, nonatomic) IBOutlet UILabel *label;
- (IBAction)onLoad:(id)sender;

@end
