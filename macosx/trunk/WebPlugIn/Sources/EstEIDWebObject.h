/* Copyright (c) 2008 Janek Priimann */

#import <Foundation/Foundation.h>

@interface EstEIDWebObject : NSObject

+ (SEL)selectorForMethod:(const char *)name;
+ (SEL)selectorForGetProperty:(const char *)name;
+ (SEL)selectorForSetProperty:(const char *)name;

- (id)invokeMethod:(NSString *)name withArguments:(NSArray *)arguments;

@end
