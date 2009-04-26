/* Copyright (c) 2008 Janek Priimann */

#import <Foundation/Foundation.h>

@class NSWindow;

@interface EstEIDWebObject : NSObject

+ (SEL)selectorForMethod:(const char *)name;
+ (SEL)selectorForProperty:(const char *)name;

- (NSWindow *)window;
- (void)setWindow:(NSWindow *)window;

- (BOOL)invokeMethod:(NSString *)name withArguments:(NSArray *)arguments result:(id *)result;

@end
