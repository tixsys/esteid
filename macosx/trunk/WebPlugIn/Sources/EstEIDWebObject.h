/* Copyright (c) 2008 Janek Priimann */

#import <Foundation/Foundation.h>

@class NSWindow;

@protocol EstEIDWebService <NSObject>

- (NSURL *)URL;
- (void)setURL:(NSURL *)URL;
- (NSWindow *)window;
- (void)setWindow:(NSWindow *)window;

@end

@interface EstEIDWebObject : NSObject

+ (SEL)selectorForMethod:(const char *)name;
+ (SEL)selectorForProperty:(const char *)name;

- (BOOL)invokeMethod:(NSString *)name withArguments:(NSArray *)arguments result:(id *)result;

@end
