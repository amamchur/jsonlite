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

#import "TWUser.h"

@implementation TWUser

@synthesize notifications;
@synthesize favourites_count;
@synthesize description;
@synthesize following;
@synthesize statuses_count;
@synthesize profile_text_color;
@synthesize geo_enabled;
@synthesize profile_background_image_url;
@synthesize profile_image_url;
@synthesize profile_link_color;
@synthesize verified;
@synthesize profile_background_tile;
@synthesize url;
@synthesize screen_name;
@synthesize created_at;
@synthesize profile_background_color;
@synthesize profile_sidebar_fill_color;
@synthesize followers_count;
@synthesize protected;
@synthesize location;
@synthesize name;
@synthesize time_zone;
@synthesize friends_count;
@synthesize profile_sidebar_border_color;
@synthesize identifier;
@synthesize utc_offset;

+ (NSArray *)jsonLiteBindingRules {
    return [NSArray arrayWithObjects:[JsonLiteBindRule ruleForKey:@"id"
                                                           bindTo:@"identifier"],
            nil];
}

- (void)dealloc {
    self.notifications = nil;
    self.favourites_count = nil;
    self.description = nil;
    self.following = nil;
    self.statuses_count = nil;
    self.profile_text_color = nil;
    self.geo_enabled = nil;
    self.profile_background_image_url = nil;
    self.profile_image_url = nil;
    self.profile_link_color = nil;
    self.verified = nil;
    self.profile_background_tile = nil;
    self.url = nil;
    self.screen_name = nil;
    self.created_at = nil;
    self.profile_background_color = nil;
    self.profile_sidebar_fill_color = nil;
    self.followers_count = nil;
    self.protected = nil;
    self.location = nil;
    self.name = nil;
    self.time_zone = nil;
    self.friends_count = nil;
    self.profile_sidebar_border_color = nil;
    self.identifier = nil;
    self.utc_offset = nil;
    [super dealloc];
}

@end
