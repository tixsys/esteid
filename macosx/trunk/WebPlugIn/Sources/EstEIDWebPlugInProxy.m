/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDWebPlugIn.h"
#import "EstEIDWebPlugInProxy.h"

@implementation EstEIDWebPlugInProxy

- (id)initWithPlugIn:(EstEIDWebPlugIn *)plugin
{
	self = [super init];
	
	if(self) {
		self->m_plugin = plugin;
	}
	
	return self;
}

- (void)invalidate:(NSTimer *)timer
{
	[self->m_plugin invalidate:timer];
}

@end
