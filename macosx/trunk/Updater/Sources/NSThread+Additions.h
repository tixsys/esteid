/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

/**
 * This category adds methods to the Foundation framework’s NSString class.
 */
@interface NSThread(Additions)

/**
 * Sleeps the thread for a given time interval.
 * @param timeInterval The duration of the sleep in seconds.
 */
+ (void)sleepForTimeInterval:(NSTimeInterval)timeInterval;

@end
