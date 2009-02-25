/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDWebPlugIn.h"

// TODO: Must decide what javascript functions should be available.

@implementation EstEIDWebPlugIn

+ (NSView *)plugInViewWithArguments:(NSDictionary *)arguments
{
    return [[[EstEIDWebPlugIn alloc] init] autorelease];
}

+ (BOOL)isSelectorExcludedFromWebScript:(SEL)selector
{
    if(selector == @selector(sign)) return NO;
    if(selector == @selector(version)) return NO;
    
    return YES;
}

+ (NSString *)webScriptNameForSelector:(SEL)selector
{
    if(selector == @selector(sign)) return @"sign";
    if(selector == @selector(version)) return @"getVersion";
    
    return nil;
}

+ (NSString *)webScriptNameForKey:(const char *)name
{
    return nil;
}

+ (BOOL)isKeyExcludedFromWebScript:(const char *)property
{
    return YES;
}

- (id)objectForWebScript
{
    return self;
}

- (void)webPlugInInitialize
{
}

- (void)webPlugInDestroy
{
}

- (void)webPlugInSetIsSelected:(BOOL)selected
{
}

- (void)webPlugInStart
{
}

- (void)webPlugInStop
{
}

- (NSString *)version
{
    return @"version 1";
}

- (id)sign
{
    return @"sign 2";
}

@end
