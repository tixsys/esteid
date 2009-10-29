/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDKTIdentityPreference.h"

@implementation EstEIDKTIdentityPreference

- (id)initWithItem:(SecKeychainItemRef)item
{
	self = [super init];
	
	if(self) {
		self->m_item = (SecKeychainItemRef)CFRetain(item);
	}
	
	return self;
}

- (SecKeychainItemRef)item
{
	return self->m_item;
}

#pragma mark NSObject

- (void)dealloc
{
	CFRelease(self->m_item);
	
	[super dealloc];
}

@end
