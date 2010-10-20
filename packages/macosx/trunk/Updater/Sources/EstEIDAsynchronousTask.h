/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

typedef enum _EstEIDAsynchronousTaskStatus {
	EstEIDAsynchronousTaskStopped = 0,
	EstEIDAsynchronousTaskStarted = 1,
	EstEIDAsynchronousTaskStopping = 2
} EstEIDAsynchronousTaskStatus;

/**
 * The EstEIDAsynchronousTask class is an abstract base class for asynchronous operations.
 * Unfortunately NSOperation and NSOperationQueue cannot be used in 10.4.
 */
@interface EstEIDAsynchronousTask : NSObject
{
	@private
	EstEIDAsynchronousTaskStatus m_status;
}

/**
 * @name Accessing and Modifying State
 */

/**
 * Returns YES if the receiver is running; NO otherwise.
 */
- (BOOL)isRunning;

/**
 * Starts the receiver by calling [self main] from another thread. Does nothing if the receiver is already running.
 */
- (void)start;

/**
 * Stops the receiver if it is running at the moment.
 */
- (void)stop;

/**
 * @name Abstract Methods for Subclasses
 */

/**
 * The main entry-point for the subclass. Called from another thread and must be implement by subclasses.
 */
- (void)main;

@end
