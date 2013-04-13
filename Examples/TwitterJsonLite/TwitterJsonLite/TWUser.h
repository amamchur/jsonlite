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

#import <Foundation/Foundation.h>


@interface TWUser : NSObject {
    
}

@property (nonatomic, copy) NSNumber *notifications;
@property (nonatomic, copy) NSNumber *favourites_count;
@property (nonatomic, copy) NSString *description;
@property (nonatomic, copy) NSNumber *following;
@property (nonatomic, copy) NSNumber *statuses_count;
@property (nonatomic, copy) NSString *profile_text_color;
@property (nonatomic, copy) NSNumber *geo_enabled;
@property (nonatomic, copy) NSString *profile_background_image_url;
@property (nonatomic, copy) NSString *profile_image_url;
@property (nonatomic, copy) NSString *profile_link_color;
@property (nonatomic, copy) NSNumber *verified;
@property (nonatomic, copy) NSNumber *profile_background_tile;
@property (nonatomic, copy) NSString *url;
@property (nonatomic, copy) NSString *screen_name;
@property (nonatomic, copy) NSString *created_at;
@property (nonatomic, copy) NSString *profile_background_color;
@property (nonatomic, copy) NSString *profile_sidebar_fill_color;
@property (nonatomic, copy) NSNumber *followers_count;
@property (nonatomic, copy) NSNumber *protected;
@property (nonatomic, copy) NSString *location;
@property (nonatomic, copy) NSString *name;
@property (nonatomic, copy) NSString *time_zone;
@property (nonatomic, copy) NSNumber *friends_count;
@property (nonatomic, copy) NSString *profile_sidebar_border_color;
@property (nonatomic, copy) NSNumber *identifier;
@property (nonatomic, copy) NSNumber *utc_offset;

@end
