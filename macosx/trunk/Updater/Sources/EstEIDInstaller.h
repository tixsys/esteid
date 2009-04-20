/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDAsynchronousTask.h"

@class EstEIDInstaller, EstEIDPackage;

/**
 * The EstEIDInstallerDelegate protocol allows to monitor the installation process.
 */
@protocol EstEIDInstallerDelegate

/**
 * @name Monitoring Installation Process
 */

/**
 * Informs the delegate that all the packages were installed successfully by the installer.
 * @param installer The installer sending this information.
 */
- (void)installerDidFinishInstalling:(EstEIDInstaller *)installer;

/**
 * Informs the delegate that a package was installed successfully by the installer.
 * @param installer The installer sending this information.
 * @param package The package that was installed successfully.
 */
- (void)installer:(EstEIDInstaller *)installer progress:(EstEIDPackage *)package;

/**
 * Called by the installer after unsuccessful installation.
 * @param installer The installer sending this information.
 * @param error A NSError object with the error.
 */
- (void)installer:(EstEIDInstaller *)installer failedToInstallWithError:(NSError *)error;

@end

/**
 * The EstEIDInstaller class allows to perform installation of packages asynchronously.
 */
@interface EstEIDInstaller : EstEIDAsynchronousTask
{
	@private
	id <EstEIDInstallerDelegate> m_delegate;
	NSArray *m_packages;
}

/**
 * @name Creating and Initializing Installers
 */

/**
 * Returns an initialized EstEIDInstaller object with the specified packages.
 * @param packages The list of packages to be installed.
 * @return An initialized EstEIDInstaller object.
 */
- (id)initWithPackages:(NSArray *)packages;

/**
 * @name Accessing Attributes
 */

/**
 * Returns the list of packages used during initialization.
 * @param The list of packages used during initialization.
 */
- (NSArray *)packages;

/**
 * Returns YES if the computer should be restarted after the installation; NO otherwise.
 * @return YES if the computer should be restarted after the installation; NO otherwise.
 */
- (BOOL)restart;

/**
 * @name Managing the Delegate
 */

/**
 * Returns the object that acts as the delegate of the receiver and implements the EstEIDInstallerDelegate protocol.
 * @return The delegate.
 */
- (id <EstEIDInstallerDelegate>)delegate;

/**
 * Set the object that acts as the delegate of the receiver and implements the EstEIDInstallerDelegate protocol.
 * @param delegate The delegate.
 */
- (void)setDelegate:(id <EstEIDInstallerDelegate>)delegate;

@end
