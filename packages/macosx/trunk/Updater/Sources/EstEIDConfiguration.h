/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

/**
 * The EstEIDConfiguration class holds the system and application configuration.
 */
@interface EstEIDConfiguration : NSObject
{
	@private
	NSString *m_architecture;
	NSString *m_operatingSystem;
	NSDictionary *m_packages;
	NSString *m_publicKey;
	NSString *m_state;
	NSString *m_version;
	NSArray *m_webServices;
}

/**
 * @name Getting the Shared Configuration Instance
 */

/**
 * Returns the shared configuration object.
 * @return The shared configuration object.
 */
+ (EstEIDConfiguration *)sharedConfiguration;

/**
 * Returns the target volume specified in the Info.plist file. Used when installing new software.
 * @return The target volume specified in the Info.plist file. Used when installing new software.
 */
+ (NSString *)targetVolume;

/**
 * @name Creating and Initializing Configurations
 */

/**
 * Returns an initialized NSString object that contains string representation of the file size.
 * @param fileSize The file size in bytes.
 * @return An initialized NSString object that contains string representation of the file size. The returned object may be different from the original receiver.
 */
- (id)init;

/**
 * @name Accessing Attributes
 */

/**
 * Returns the architecture of this computer. Possible values are 'ppc', 'ppc32', 'ppc64', 'i386', 'x86_32' and 'x86_64'.
 * @return The architecture of this computer.
 */
- (NSString *)architecture;

/**
 * Returns the operating system that is running at the moment. Possible values are for example "10.4" or "10.5.1".
 * @return the operating system that is running at the moment.
 */
- (NSString *)operatingSystem;

/**
 * Returns the list of installed packages. CFBundleIdentifier values are used for keys and CFBundleShortVersionString values for values.
 * @return The list of installed packages.
 */
- (NSDictionary *)packages;

/**
 * Returns the public key for the private key that is used by the web-service.
 * @return The public key for the private key that is used by the web-service.
 */
- (NSString *)publicKey;

/**
 * Returns the version of this application.
 * @return The version of this application.
 */
- (NSString *)version;

/**
 * Returns an array with all the web-service locations.
 * @return An array with all the web-service locations.
 */
- (NSArray *)webServices;

/**
 * @name Accessing and Modifying Application State
 */

/**
 * Returns the current application state or nil if there is none.
 * @return The current application state or nil
 */
- (NSString *)state;

/**
 * Sets the new application state.
 * @param state The new state.
 */
- (void)setState:(NSString *)state;

@end
