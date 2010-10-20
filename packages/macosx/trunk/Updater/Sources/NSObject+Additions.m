/* Copyright (c) 2009 Janek Priimann */

#import "NSObject+Additions.h"

@implementation NSObject(Additions)

// COMPATIBILITY: Only here for 10.4 compatibility
+ (NSArray *)preferredLanguages
{
	return [NSArray arrayWithObjects:[[NSLocale currentLocale] objectForKey:NSLocaleLanguageCode], nil];
}

@end
