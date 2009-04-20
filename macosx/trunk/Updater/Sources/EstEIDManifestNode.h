/* Copyright (c) 2009 Janek Priimann */

#import <Cocoa/Cocoa.h>

@class EstEIDConfiguration;

/**
 * The EstEIDManifestNode class represents XML nodes.
 */
@interface EstEIDManifestNode : NSObject
{
	@private
	NSArray *m_architecture;
	NSArray *m_installations;
	NSArray *m_operatingSystem;
}

/**
 * @name Node Tags
 */

/**
 * Returns the Class for the specified tag or NULL if it cannot be found.
 * @param tag The requested tag.
 * @return The class or NULL if no match.
 */
+ (Class)classForTag:(NSString *)tag;

/**
 * @name Creating and Initializing Nodes
 */

/**
 * Returns an initialized EstEIDManifestNode object.
 * @param tag The element name.
 * @param attributes The attributes.
 * @param children A NSString object with contents or a NSArray object with child nodes.
 * @return An initialized EstEIDManifestNode object.
 */
- (id)initWithTag:(NSString *)tag attributes:(NSDictionary *)attributes children:(id)children;

/**
 * @name Accessing Attributes
 */

/**
 * Returns the attributes of the receiver or nil.
 * @return The attributes of the receiver or nil.
 */
- (NSDictionary *)attributes;

/**
 * Returns the contents of the receiver or nil.
 * @return The contents of the receiver or nil.
 */
- (NSString *)contents;

/**
 * Returns the tag of the receiver.
 * @return The tag of the receiver.
 */
- (NSString *)tag;

/**
 * @name Testing
 */

/**
 * Returns YES if the receiver passes the test against the current configuration; NO otherwise.
 * @return YES if the receiver passes the test against the current configuration; NO otherwise.
 */
- (BOOL)test;

/**
 * @name Quering Common Nodes
 */

/**
 * Writes all the components in the receiver into the specified array.
 * @param components An NSMutableArray object where the components are written.
 */
- (void)writeComponentsToArray:(NSMutableArray *)components;

/**
 * Writes all the packages in the receiver into the specified array.
 * @param packages An NSMutableArray object where the packages are written.
 */
- (void)writePackagesToArray:(NSMutableArray *)packages;

@end

/**
 * The EstEIDManifestTreeNode class represents XML nodes that may contain child nodes.
 */
@interface EstEIDManifestTreeNode : EstEIDManifestNode
{
	@private
	NSArray *m_nodes;
}

/**
 * @name Accessing Attributes
 */

/**
 * Returns all the child nodes of the receiver.
 * @return The child nodes of the receiver.
 */
- (NSArray *)nodes;

@end
