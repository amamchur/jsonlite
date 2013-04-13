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
#import "TWUser.h"

@interface TWTimeline : NSObject

@property (nonatomic, copy) NSString *geo;
@property (nonatomic, copy) NSNumber *in_reply_to_user_id;
@property (nonatomic, copy) NSNumber *in_reply_to_status_id;
@property (nonatomic, copy) NSNumber *truncated;
@property (nonatomic, copy) NSString *source;
@property (nonatomic, copy) NSNumber *favorited;
@property (nonatomic, copy) NSString *created_at;
@property (nonatomic, copy) NSString *in_reply_to_screen_name;
@property (nonatomic, retain) TWUser *user;
@property (nonatomic, copy) NSString *text;
@property (nonatomic, copy) NSNumber *identifier;

@end
