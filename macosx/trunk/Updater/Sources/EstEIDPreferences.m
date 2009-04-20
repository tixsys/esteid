/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDPreferences.h"

@implementation EstEIDPreferences

+ (EstEIDPreferences *)sharedPreferences
{
	static EstEIDPreferences *sharedPreferences = nil;
	
	if(!sharedPreferences) {
		sharedPreferences = [[EstEIDPreferences alloc] init];
	}
	
	return sharedPreferences;
}

- (NSDate *)lastUpdate
{
	return self->m_lastUpdate;
}

- (void)setLastUpdate:(NSDate *)lastUpdate
{
	if(self->m_lastUpdate != lastUpdate) {
		[self->m_lastUpdate release];
		self->m_lastUpdate = [lastUpdate retain];
	}
}

- (EstEIDUpdateFrequency)updateFrequency
{
	return self->m_updateFrequency;
}

- (void)setUpdateFrequency:(EstEIDUpdateFrequency)updateFrequency
{
	if(self->m_updateFrequency != updateFrequency) {
		self->m_updateFrequency = updateFrequency;
	}
}

@end
