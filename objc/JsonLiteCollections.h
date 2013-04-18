//
//  JsonLiteCollections.h
//  JsonLiteObjC
//
//  Created by Andrii Mamchur on 12/10/12.
//  Copyright (c) 2012 Andrii Mamchur. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface JsonLiteDictionary : NSDictionary
@end

@interface JsonLiteArray : NSArray
@end

id JsonLiteCreateDictionary(const id *values, const id *keys, const CFHashCode *hashes, NSUInteger count);
id JsonLiteCreateArray(const id *objects, NSUInteger count);