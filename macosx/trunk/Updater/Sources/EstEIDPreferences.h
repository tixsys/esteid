/* Copyright (c) 2009 Janek Priimann */

#import <Cocoa/Cocoa.h>

typedef enum _EstEIDUpdateFrequency {
	EstEIDUpdateFrequencyEveryTime = 0,
	EstEIDUpdateFrequencyEveryDay,
	EstEIDUpdateFrequencyEveryWeek,
	EstEIDUpdateFrequencyEveryMonth,
	EstEIDUpdateFrequencyNever
} EstEIDUpdateFrequency;

@interface EstEIDPreferences : NSObject
{
	@protected
	NSDate *m_lastUpdate;
	EstEIDUpdateFrequency m_updateFrequency;
}

+ (EstEIDPreferences *)sharedPreferences;

- (NSDate *)lastUpdate;
- (void)setLastUpdate:(NSDate *)lastUpdate;
- (EstEIDUpdateFrequency)updateFrequency;
- (void)setUpdateFrequency:(EstEIDUpdateFrequency)updateFrequency;

@end
