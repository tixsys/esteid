/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDWebPlugIn.h"

// TODO: Must decide what javascript functions should be available.

@implementation EstEIDWebPlugIn

+ (NSView *)plugInViewWithArguments:(NSDictionary *)arguments
{
    return [[[EstEIDWebPlugIn alloc] initWithFrame:NSMakeRect(0.0F, 0.0F, 10.0F, 10.0F)] autorelease];
}

+ (BOOL)isSelectorExcludedFromWebScript:(SEL)selector
{
    if(selector == @selector(sign)) return NO;
    if(selector == @selector(version)) return NO;
    if(selector == @selector(addEvent: forListener:)) return NO;
    
    return YES;
}

+ (NSString *)webScriptNameForSelector:(SEL)selector
{
    if(selector == @selector(sign)) return @"sign";
    if(selector == @selector(version)) return @"getVersion";
    if(selector == @selector(addEvent: forListener:)) return @"addEventListener";
	
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
	[self setFrame:NSMakeRect(0, 0, 10, 10)];
}

- (void)webPlugInDestroy
{
}

- (void)webPlugInSetIsSelected:(BOOL)selected
{
}

- (void)webPlugInStart
{
	[self setFrame:NSMakeRect(0, 0, 10, 10)];
}

- (void)webPlugInStop
{
}

- (NSString *)version
{
    return @"1.2";
}

- (id)sign
{
	return @"123...567";
	//return [NSString stringWithFormat:@"%@ - %@", [[self->m_listener invokeDefaultMethodWithArguments:[NSArray arrayWithObject:@"42"]] description], [self->m_listener stringRepresentation]];
}

- (NSString *)addEvent:(NSString *)event forListener:(WebScriptObject *)something
{
	return [NSString stringWithFormat:@"event=%@, listener=%@, desc=%@", event, NSStringFromClass([something class]), [something stringRepresentation]];
}

#pragma mark - NSObject

- (void)dealloc
{
	
	[super dealloc];
}

@end
