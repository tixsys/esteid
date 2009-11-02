/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDWhitelist.h"

static NSString *EstEIDWhitelistKey = @"EstEIDWhitelist";

@implementation EstEIDWhitelist

+ (EstEIDWhitelist *)sharedWhitelist
{
	static EstEIDWhitelist *sharedWhitelist = nil;
	
	if(!sharedWhitelist) {
		sharedWhitelist = [[EstEIDWhitelist alloc] init];
	}
	
	return sharedWhitelist;
}

- (EstEIDWebsiteStatus)statusForWebsite:(NSURL *)website
{
	return [[self->m_websites objectForKey:[website host]] intValue];
}

- (void)setStatus:(EstEIDWebsiteStatus)status forWebsite:(NSURL *)website
{
	[self->m_websites setObject:[NSNumber numberWithInt:status] forKey:[website host]];
	[[NSUserDefaults standardUserDefaults] setObject:[NSDictionary dictionaryWithDictionary:self->m_websites] forKey:EstEIDWhitelistKey];
	[[NSUserDefaults standardUserDefaults] synchronize];
}

#pragma mark NSObject

- (id)init
{
	self = [super init];
	
	if(self) {
		NSDictionary *websites = [[NSUserDefaults standardUserDefaults] objectForKey:EstEIDWhitelistKey];
		
		self->m_websites = [[NSMutableDictionary alloc] init];
		
		if([websites isKindOfClass:[NSDictionary class]]) {
			[self->m_websites addEntriesFromDictionary:websites];
		}
	}
	
	return self;
}

- (void)dealloc
{
	[self->m_websites release];
	
	[super dealloc];
}

@end
