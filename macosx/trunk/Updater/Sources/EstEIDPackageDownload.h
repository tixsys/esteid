/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDAsynchronousTask.h"

@class EstEIDPackage, EstEIDPackageDownload;

/**
 * The EstEIDPackageDownloadDelegate protocol allows to monitor the package download process.
 */
@protocol EstEIDPackageDownloadDelegate <NSObject>

/**
 * @name Monitoring the Download
 */

/**
 * Informs the delegate that all the packages were downloaded successfully by the sender.
 * @param download The download object sending this information.
 */
- (void)packageDownloadDidFinishLoading:(EstEIDPackageDownload *)download;

/**
 * Informs the delegate that a number of bytes were downloaded.
 * @param download The download object sending this information.
 * @param length Number of bytes downloaded.
 */
- (void)packageDownload:(EstEIDPackageDownload *)download progress:(long long)length;

/**
 * Called by the download object after unsuccessful download.
 * @param download The download object sending this information.
 * @param error A NSError object with the error.
 */
- (void)packageDownload:(EstEIDPackageDownload *)download failedToLoadWithError:(NSError *)error;

@end

/**
 * The EstEIDPackageDownload class allows to download packages asynchronously.
 */
@interface EstEIDPackageDownload : EstEIDAsynchronousTask
{
	@private
	NSURL *m_baseURL;
	id <EstEIDPackageDownloadDelegate> m_delegate;
	struct {
		int x;
		int y;
	} m_locator;
	NSURLDownload *m_download;
	NSArray *m_inPackages;
	NSArray *m_outPackages;
	long long m_length;
	long long m_progress;
	long long m_complete;
	NSString *m_path;
	NSString *m_tmp;
	BOOL m_result;
}

/**
 * @name Creating and Initializing Package Downloads
 */

/**
 * Returns an initialized EstEIDPackageDownload object with the specified manifest packages.
 * @param packages An array of EstEIDManifestPackage objects.
 * @return An initialized EstEIDPackageDownload object.
 */
- (id)initWithManifestPackages:(NSArray *)packages;

/**
 * Returns an initialized EstEIDPackageDownload object with the specified manifest packages.
 * @param packages An array of EstEIDManifestPackage objects.
 * @param location The base URL for packages that are referenced relatively.
 * @param prefix The prefix to be used for the temporary directory.
 * @return An initialized EstEIDPackageDownload object.
 */
- (id)initWithManifestPackages:(NSArray *)packages location:(NSString *)location prefix:(NSString *)prefix;

/**
 * @name Managing the Delegate
 */

/**
 * Returns the object that acts as the delegate of the receiver and implements the EstEIDPackageDownloadDelegate protocol.
 * @return The delegate.
 */
- (id <EstEIDPackageDownloadDelegate>)delegate;

/**
 * Set the object that acts as the delegate of the receiver and implements the EstEIDPackageDownloadDelegate protocol.
 * @param delegate The delegate.
 */
- (void)setDelegate:(id <EstEIDPackageDownloadDelegate>)delegate;

/**
 * @name Accessing Attributes
 */

/**
 * Returns the base URL that is used for packages that are referenced with relative urls.
 * @return The base URL.
 */
- (NSURL *)baseURL;

/**
 * Returns the length in bytes of all the packages to be installed.
 * @return The length in bytes.
 */
- (long long)length;

/**
 * Returns a list of packages to be installed.
 * @return A list of packages to be installed.
 */
- (NSArray *)packages;

/**
 * Returns the temporary directory path for the receiver.
 * @return The temporary directory path.
 */
- (NSString *)path;

@end
