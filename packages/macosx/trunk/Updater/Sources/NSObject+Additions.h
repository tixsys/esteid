/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

/**
 * This category adds methods to the Foundation framework’s NSObject class.
 */
@interface NSObject(Additions)

/**
 * @name Getting Preferred Languages
 */

/**
 * Returns the user's language preference order as an array of strings.
 * @return The user's language preference order as an array of NSString objects, each of which is a canonicalized IETF BCP 47 language identifier.
 */
+ (NSArray *)preferredLanguages;

@end
