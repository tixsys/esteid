/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDManifestNode.h"

#define EstEIDManifestFetchError -1
#define EstEIDManifestParseError -2
#define EstEIDManifestVerifyError -3

extern NSString *EstEIDManifestErrorDomain;

/**
 * The EstEIDManifest class contains information about all the packages and it also supports digital signatures.
 */
@interface EstEIDManifest : EstEIDManifestTreeNode
{
	@private
	NSData *m_data;
	NSString *m_signature;
}

/**
 * @name Creating and Initializing Manifests
 */

/**
 * Returns an initialized EstEIDManifest object from the specified data.
 * @param data The contents of the manifest.
 * @return An initialized EstEIDManifest object.
 */
+ (id)manifestFromData:(NSData *)data;

/**
 * Returns an initialized EstEIDManifest object from the specified string.
 * @param string The contents of the manifest as a string.
 * @return An initialized EstEIDManifest object.
 */
+ (id)manifestFromString:(NSString *)string;

/**
 * @name Accessing Attributes
 */

/**
 * Returns all the components inside the manifest that are relevant for the current configuration.
 * @return All the components.
 */
- (NSArray *)components;

/**
 * Returns the original data that is suitable for verification.
 * @return Returns the original data that is suitable for verification.
 */
- (NSData *)data;

/**
 * Returns the url for the signature or nil.
 * @return The url for the signature or nil.
 */
- (NSString *)signature;

/**
 * Returns the state for the receiver. Useful for monitoring system configuration and/or manifest changes.
 * @return The state for the receiver.
 */
- (NSString *)state;

/**
 * @name Verifying the Signature
 */

/**
 * Returns YES if the public-key, message and signature match; NO otherwise.
 * @param signature The signature to be used during verification.
 * @param publicKey The public key to be used during verification.
 * @return YES if the public-key, message and signature match; NO otherwise.
 */
- (BOOL)verifySignature:(NSData *)signature usingPublicKey:(NSString *)publicKey;

@end
