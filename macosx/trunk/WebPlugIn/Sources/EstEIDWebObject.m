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
		NSMethodSignature *signature = [self methodSignatureForSelector:selector];
		NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:signature];
		
		[invocation setTarget:self];
		[invocation setSelector:selector];
		
		if([signature numberOfArguments] == 3) {
			[invocation setArgument:&arguments atIndex:2];
		}
		
		[invocation invoke];
		
		if(result) {
			if([signature methodReturnLength] == sizeof(id)) {
				[invocation getReturnValue:result];
			} else {
				*result = nil;
			}
		}
		
		return YES;
	}
	
	return NO;
}

@end
