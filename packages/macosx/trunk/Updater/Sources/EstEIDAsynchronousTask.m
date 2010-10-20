/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDAsynchronousTask.h"
#import "NSThread+Additions.h"

@implementation EstEIDAsynchronousTask

- (void)main
{
	[self doesNotRecognizeSelector:_cmd];
}

- (void)run:(id)argument
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
#ifdef DEBUG
	NSLog(@"%@: started", NSStringFromClass([self class]));
#endif
	
	[self main];
	self->m_status = EstEIDAsynchronousTaskStopped;
	
#ifdef DEBUG
	NSLog(@"%@: stopped", NSStringFromClass([self class]));
#endif
	
	[pool release];
}

- (BOOL)isRunning
{
	return (self->m_status == EstEIDAsynchronousTaskStarted) ? YES : NO;
}

- (void)start
{
	@synchronized(self) {
		if(self->m_status == EstEIDAsynchronousTaskStopped) {
			self->m_status = EstEIDAsynchronousTaskStarted;
			[NSThread detachNewThreadSelector:@selector(run:) toTarget:self withObject:nil];
		}
	}
}

- (void)stop
{
	@synchronized(self) {
		if(self->m_status == EstEIDAsynchronousTaskStarted) {
			self->m_status = EstEIDAsynchronousTaskStopping;
			
			while(self->m_status != EstEIDAsynchronousTaskStopped) {
				[NSThread sleepForTimeInterval:0.01];
			}
		}
	}
}

#pragma mark NSObject

- (void)dealloc
{
	[self stop];
	
	[super dealloc];
}

@end
