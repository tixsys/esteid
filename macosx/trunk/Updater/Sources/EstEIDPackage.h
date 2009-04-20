/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

#define EstEIDPackageGenericError -1
#define EstEIDPackageFetchError -2
#define EstEIDPackageLengthError -3
#define EstEIDPackageVerifyError -4
#define EstEIDPackageUncompressionError -5
#define EstEIDPackageInstallError -6

extern NSString *EstEIDPackageErrorDomain;

@class EstEIDManifestPackage;

/**
 * The EstEIDPackage represents an actual package in the system.
 */
@interface EstEIDPackage : NSObject
{
	@private
	EstEIDManifestPackage *m_metadata;
	NSString *m_path;
}

/**
 * @name Creating and Initializing Packages
 */

/**
 * Returns an initialized EstEIDPackage object with the specified path and metadata.
 * @param path A NSString with the path where the package is located.
 * @param metadata The manifest package.
 * @return An initialized EstEIDPackage object.
 */
- (id)initWithContentsOfFile:(NSString *)path metadata:(EstEIDManifestPackage *)metadata;

/**
 * @name Accessing Attributes
 */

/**
 * Returns the actual size in bytes of the package.
 * @return The actual size in byts of the package.
 */
- (long long)length;

/**
 * Returns the manifest package associated with the receiver. Useful for comparing metadata against actual data.
 * @return The manifest package.
 */
- (EstEIDManifestPackage *)metadata;

/**
 * Returns the path of the receiver.
 * @return The path of the receiver.
 */
- (NSString *)path;

/**
 * Returns the sha1 checksum of the receiver.
 * @return The sha1 checksum of the receiver.
 */
- (NSString *)sha1;

@end
