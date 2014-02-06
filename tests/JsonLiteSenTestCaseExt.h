//
//  Copyright 2012-2014, Andrii Mamchur
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

#import <XCTest/XCTest.h>

@interface XCTestCase(JsonLiteSenTestCaseExt)

- (BOOL)compareNumber:(NSNumber *)n1 withNumber:(NSNumber *)n2;
- (BOOL)compareDictionary:(NSDictionary *)d1 withDictionary:(NSDictionary *)d2;
- (BOOL)compareArray:(NSArray *)a1 withArray:(NSArray *)a2;

- (id)parseObjectFromFile:(NSString *)file inDir:(NSString *)dir;
- (NSError *)parseErrorFromFile:(NSString *)file inDir:(NSString *)dir;
- (NSData *)dataFromFile:(NSString *)file inDir:(NSString *)dir;

@end
