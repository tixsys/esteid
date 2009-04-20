/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDAsynchronousTask.h"

@class EstEIDManifest, EstEIDManifestDownload;

/**
 * The EstEIDManifestDownloadDelegate protocol allows to monitor the manifest download process.
 */
@protocol EstEIDManifestDownloadDelegate <NSObject>

/**
 * @name Monitoring the Download
 */

/**
 * Informs the delegate that the manifest was downloaded successfully.
 * @param download The download object sending this information.
 */
- (void)manifestDownloadDidFinishLoading:(EstEIDManifestDownload *)download;

/**
 * Informs the delegate that the manifest was not downloaded successfully.
 * @param download The download object sending this information.
 * @param error A NSError object with the error.
 */
- (void)manifestDownload:(EstEIDManifestDownload *)download failedToLoadWithError:(NSError *)error;

@end

/**
 * The EstEIDManifestDownload class allows to download the manifest asynchronously.
 */
@interface EstEIDManifestDownload : EstEIDAsynchronousTask
{
	@private
	NSArray *m_components;
	id <EstEIDManifestDownloadDelegate> m_delegate;
	NSURLDownload *m_download;
	EstEIDManifest *m_manifest;
	NSData *m_signature;
	NSArray *m_locations;
	int m_locator;
	BOOL m_result;
	NSString *m_tmp;
}

/**
 * @name Creating and Initializing Manifest Downloads
 */

/**
 * Returns an initialized EstEIDManifestDownload object with the specified locations.
 * @param locations An array of NSString objects with all the urls for the manifest file.
 * @return An initialized EstEIDManifestDownload object.
 */
- (id)initWithLocations:(NSArray *)locations;

/**
 * @name Accessing Attributes
 */

/**
 * Returns all the components inside the manifest that are relevant for the current configuration.
 * @return All the components.
 */
- (NSArray *)components;

/**
 * Returns the url for the current web-service. Can be used to identify where the manifest was exactly downloaded.
 * @return The url for the current web-service.
 */
- (NSString *)location;

/**
 * Returns the manifest or nil if there is none.
 * @return The manifest or nil if there is none.
 */
- (EstEIDManifest *)manifest;

/**
 * Returns the signature or nil if there is none.
 * @reutnr The signature or nil.
 */
- (NSData *)signature;

/**
 * @name Managing the Delegate
 */

/**
 * Returns the object that acts as the delegate of the receiver and implements the EstEIDManifestDownloadDelegate protocol.
 * @return The delegate.
 */
- (id <EstEIDManifestDownloadDelegate>)delegate;

/**
 * Set the object that acts as the delegate of the receiver and implements the EstEIDManifestDownloadDelegate protocol.
 * @param delegate The delegate.
 */
- (void)setDelegate:(id <EstEIDManifestDownloadDelegate>)delegate;

@end
