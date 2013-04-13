//
//  JsonLiteCollections.h
//  JsonLiteObjC
//
//  Created by Andrii Mamchur on 12/10/12.
//  Copyright (c) 2012 Andrii Mamchur. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface JsonLiteDictionary : NSDictionary

+ (id)allocDictionaryWithValue:(const id *)values
                          keys:(const id *)keys
                        hashes:(const CFHashCode *)hashes
                         count:(NSUInteger)cnt;

@end

@interface JsonLiteArray : NSArray 
@end
