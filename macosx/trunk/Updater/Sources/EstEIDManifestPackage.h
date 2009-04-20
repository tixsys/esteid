/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDManifestNode.h"

/**
 * The EstEIDManifestPackage represents a package in the manifest.
 * 
 * <package ... />
 */
@interface EstEIDManifestPackage : EstEIDManifestNode
{
	@private
	NSArray *m_locations;
	NSString *m_identifier;
	long long m_length;
	BOOL m_restart;
	NSString *m_sha1;
	NSString *m_version;
}

/**
 * @name Accessing Attributes
 */

/**
 * Returns the identifier of the receiver.
 * @return The identifier of the receiver.
 */
- (NSString *)identifier;

/**
 * Returns the file-size in bytes of the receiver.
 * @return The file-size in bytes of the receiver.
 */
- (long long)length;

/**
 * Returns a NSArray with NSString objects with all the locations for the receiver.
 * @return A NSArray with NSString objects with all the locations for the receiver.
 */
- (NSArray *)locations;

/**
 * Returns YES if the package requires restarting the system after installation; NO otherwise.
 * @return YES if the package requires restarting the system after installation; NO otherwise.
 */
- (BOOL)restart;

/**
 * Returns the sha-1 checksum for the receiver.
 * @return The sha-1 checksum for the receiver.
 */
- (NSString *)sha1;

/**
 * Returns the version for the receiver.
 * @return The version for the receiver.
 */
- (NSString *)version;

@end
