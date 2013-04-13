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

#import "TWTimeline.h"
#import "JsonLiteMetaData.h"

@implementation TWTimeline

@synthesize geo;
@synthesize in_reply_to_user_id;
@synthesize in_reply_to_status_id;
@synthesize truncated;
@synthesize source;
@synthesize favorited;
@synthesize created_at;
@synthesize in_reply_to_screen_name;
@synthesize user;
@synthesize text;
@synthesize identifier;

+ (NSArray *)jsonLiteBindingRules {
    return [NSArray arrayWithObjects:[JsonLiteBindRule ruleForKey:@"id"
                                                           bindTo:@"identifier"],
            nil];
}

- (void)dealloc {
    self.geo = nil;
    self.in_reply_to_user_id = nil;
    self.in_reply_to_status_id = nil;
    self.truncated = nil;
    self.source = nil;
    self.favorited = nil;
    self.created_at = nil;
    self.in_reply_to_screen_name = nil;
    self.user = nil;
    self.text = nil;
    self.identifier = nil;
    [super dealloc];
}

@end
