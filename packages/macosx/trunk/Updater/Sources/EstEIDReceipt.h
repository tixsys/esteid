/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

/**
 * The EstEIDReceipt class holds information about one package that is actually installed in the system.
 */
@interface EstEIDReceipt : NSObject
{
	@private
	NSString *m_description;
	NSString *m_identifier;
	NSString *m_metadata;
	NSString *m_title;
	NSString *m_version;
}

/**
 * @name Creating and Initializing Receipts
 */

/**
 * Returns a list with the installed receipts in the system.
 * @return A list with the installed receipts in the system.
 */
+ (NSArray *)receipts;

/**
 * Returns a list with the installed receipts in the system.
 * @param prefix Only receipts that start with this prefix are returned. Use nil if you want all the receipts.
 * @return A list with the installed receipts in the system.
 */
+ (NSArray *)receiptsWithPrefix:(NSString *)prefix;

/**
 * Returns an initialized EstEIDReceipt object with the specified identifier or nil if not found in the system.
 * @param identifier The identifier for the receipt.
 * @return An initialized EstEIDReceipt object.
 */
- (id)initWithIdentifier:(NSString *)identifier;

/**
 * @name Accessing Attributes
 */

/**
 * Returns the localized description of the receiver or nil.
 * @return The localized description of the receiver or nil.
 */
- (NSString *)description;

/**
 * Returns the value of the CFBundleIdentifier key.
 * @return The value of the CFBundleIdentifier key.
 */
- (NSString *)identifier;

/**
 * Returns the localized title of the receiver or nil.
 * @return The localized title of the receiver or nil.
 */
- (NSString *)title;

/**
 * Returns the value of the CFBundleShortVersionString key.
 * @return The value of the CFBundleShortVersionString key.
 */
- (NSString *)version;

@end
