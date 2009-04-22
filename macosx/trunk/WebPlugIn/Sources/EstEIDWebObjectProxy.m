/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDWebObjectProxy.h"

@implementation EstEIDWebObjectProxy

- (id)initWithObject:(EstEIDWebObject *)object
{
	self = [super init];
	
	if(self) {
		self->m_object = object;
	}
	
	return self;
}

- (void)invalidate:(NSTimer *)timer
{
	[self->m_object invalidate:timer];
}

@end
