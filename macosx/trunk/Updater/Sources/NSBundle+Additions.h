/* Copyright (c) 2008 Janek Priimann */

#import <Foundation/Foundation.h>

/**
 * This category adds methods to the Foundation framework’s NSBundle class.
 */
@interface NSBundle(Additions)

/**
 * @name Accessing Info.plist Entries
 */

/**
 * Returns a NSArray object if the key exists in the receiver and is an instance of NSArray or nil.
 * @param key The key for which to return the corresponding value.
 * @return A NSArray object if the key exists in the receiver and is an instance of NSArray or nil.
 */
- (NSArray *)arrayForKey:(NSString *)key;

/**
 * Returns an object if the key exists in the receiver or nil.
 * @param key The key for which to return the corresponding value.
 * @return An object if the key exists in the receiver or nil.
 */
- (id)objectForKey:(NSString *)key;

/**
 * Returns a NSString object if the key exists in the receiver and is an instance of NSString or nil.
 * @param key The key for which to return the corresponding value.
 * @return A NSString object if the key exists in the receiver and is an instance of NSString or nil.
 */
- (NSString *)stringForKey:(NSString *)key;

@end
