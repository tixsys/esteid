/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDManifestNode.h"

typedef enum _EstEIDManifestItemPriority {
	EstEIDManifestComponentPriorityLow = 0, /**< Optional, unselected by default. */
	EstEIDManifestComponentPriorityHigh = 1, /**< Optional, selected by default. */
	EstEIDManifestComponentPriorityCritical = 2 /**< Required, selected by default. */
} EstEIDManifestComponentPriority;

/**
 * The EstEIDManifestComponent represents a component in the manifest.
 * 
 * <component ... />
 */
@interface EstEIDManifestComponent : EstEIDManifestTreeNode
{
	@private
	NSDictionary *m_description;
	long long m_length;
	EstEIDManifestComponentPriority m_priority;
	NSDictionary *m_title;
	NSString *m_version;
}

/**
 * @name Accessing Attributes
 */

/**
 * Returns the localized description of the receiver.
 * @return The localized description.
 */
- (NSString *)description;

/**
 * Returns the total file-size in bytes of all the packages.
 * @return The total file-size in bytes of all the packages.
 */
- (long long)length;

/**
 * Returns a list of packages that match the configuration.
 * @return A list of packages.
 */
- (NSArray *)packages;

/**
 * Returns the priority of the receiver. Components that are marked as EstEIDManifestComponentPriorityCritical should be always installed.
 * @return The priority of the receiver.
 */
- (EstEIDManifestComponentPriority)priority;

/**
 * Returns YES if the component requires restarting the system after installation; NO otherwise.
 * @return YES if the component requires restarting the system after installation; NO otherwise.
 */
- (BOOL)restart;

/**
 * Returns the localized title of the receiver.
 * @return The localized title.
 */
- (NSString *)title;

/**
 * Returns the version of the receiver.
 * @return The version of the receiver.
 */
- (NSString *)version;

@end
