/* Copyright (c) 2009 Janek Priimann */

#import "NSThread+Additions.h"

@implementation NSThread(Additions)

// COMPATIBILITY: Only here for 10.4 compatibility
+ (void)sleepForTimeInterval:(NSTimeInterval)timeInterval
{
	NSDate *date = [[NSDate alloc] initWithTimeIntervalSinceNow:timeInterval];
	
	[NSThread sleepUntilDate:date];
	[date release];
}

@end
