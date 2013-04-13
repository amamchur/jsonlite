//
//  TwitterJsonLiteAppDelegate.h
//  TwitterJsonLite
//
//  Created by Andriy Mamchur on 6/9/12.
//  Copyright 2012 soft serve. All rights reserved.
//

#import <UIKit/UIKit.h>

@class TwitterJsonLiteViewController;

@interface TwitterJsonLiteAppDelegate : NSObject <UIApplicationDelegate> {

}

@property (nonatomic, retain) IBOutlet UIWindow *window;

@property (nonatomic, retain) IBOutlet TwitterJsonLiteViewController *viewController;

@end
