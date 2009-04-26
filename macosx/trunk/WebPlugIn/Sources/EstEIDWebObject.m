/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDWebObject.h"

@implementation EstEIDWebObject

+ (SEL)selectorForMethod:(const char *)name
{
	return NULL;
}

+ (SEL)selectorForProperty:(const char *)name
{
	return NULL;
}

- (NSWindow *)window
{
	return nil;
}

- (void)setWindow:(NSWindow *)window
{
}

- (BOOL)invokeMethod:(NSString *)name withArguments:(NSArray *)arguments result:(id *)result
{
	SEL selector = [[self class] selectorForMethod:[name UTF8String]];
	
	if(selector) {
		id result_1 = [self performSelector:selector withObject:arguments];
		
		if(result) {
			*result = result_1;
		}
		
		return YES;
	}
	
	return NO;
}

@end
