/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

/**
 * This category adds methods to the Foundation framework’s NSString class.
 */
@interface NSString(Additions)

/**
 * @name Creating and Initializing Strings
 */

/**
 * Returns an initialized NSString object that contains string representation of the file size.
 * @param fileSize The file size in bytes.
 * @return An initialized NSString object that contains string representation of the file size. The returned object may be different from the original receiver.
 */
+ (id)stringWithFileSize:(long long)fileSize;

/**
 * Returns an initialized NSString object that contains string representation of the file size.
 * @param fileSize The file size in bytes.
 * @return An initialized NSString object that contains string representation of the file size. The returned object may be different from the original receiver.
 */
- (id)initWithFileSize:(long long)fileSize;

/**
 * @name Comparing Versions
 */

/**
 * Returns YES if the version passes the receiver; NO otherwise.
 *
 * The receiver may end with '>=', '<=', '<', '>' or '=' and '<' is used by default.
 * For example if the receiver is '1.0' and the version is '0.9' then '1.0<' is tested against '0.9' and it returns YES.
 *
 * @param version The version to use when testing against the receiver.
 * @return YES if the version passes the receiver; NO otherwise.
 */
- (BOOL)testVersionString:(NSString *)version;

/**
 * @name Encryption
 */

/**
 * Returns the receiver's text as a SHA-1 hash. All the characters are lower-case.
 * @return A NSString object with a SHA-1 hash.
 */
- (NSString *)sha1String;

/**
 * @name Helpers
 */

- (NSString *)trimmedString;

@end
