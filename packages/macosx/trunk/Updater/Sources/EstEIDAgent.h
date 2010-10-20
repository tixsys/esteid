/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

NSString *EstEIDAgentDidInstallNotification;
NSString *EstEIDAgentIdentifier;

/**
 * Using the EstEIDAgent class, your program can run various operations as an elevated subprocess and can monitor that program's execution.
 * An EstEIDAgent object creates a separate executable entity; it does not share memory space with the process that creates it.
 *
 * Instances of the EstEIDAgent class are thread-safe.
 */
@interface EstEIDAgent : NSObject
{
	@private
	BOOL m_ext;
	BOOL m_level;
	NSString *m_path;
	void *m_state;
}

/**
 * @name Getting the Shared EstEIDAgent Instance
 */

/**
 * Returns the shared agent object.
 * @return The shared agent object.
 */
+ (EstEIDAgent *)sharedAgent;

/**
 * @name Creating and Initializing Agents
 */

/**
 * Creates and initializes an EstEIDAgent object.
 * @param path The path to the executable.
 * @param state An opaque pointer to the external authorization state or NULL.
 * @return An EstEIDAgent object.
 */
- (id)initWithPath:(NSString *)path state:(void *)state;

/**
 * @name Accessing Attributes
 */

/**
 * Returns the identifier for the EstEIDAgent class.
 */
+ (const char *)identifier;

/**
 * Returns the path of the receiver's executable.
 * @return The path of the receiver's executable.
 */
- (NSString *)path;

/**
 * @name Authorization
 */

/**
 * Returns YES if the agent is owned by system; otherwise NO.
 * @return YES if the agent is owned by system; otherwise NO.
 */
- (BOOL)isOwnedBySystem;

/**
 * Authorizes the receiver. Returns YES if the agent was authorized successfully; NO otherwise.
 * @return YES if the agent was authorized successfully; NO otherwise.
 */
- (BOOL)authorize;

/**
 * Deauthorizes the receiver.
 */
- (void)deauthorize;

/**
 * @name Launching Agents
 */

/**
 * Launches a new instance of the agent manually with the specified command line arguments.
 * The receiver must be authorized before calling this method.
 * @param arguments The first argument to be sent. Usually the name of the command.
 * @param ... Optional additional arguments that must be terminated with nil.
 * @return A NSTask object if successful; otherwise nil.
 */
- (NSTask *)launchWithArguments:(NSString *)arguments, ...;

@end
