/* Copyright (c) 2009 Janek Priimann */

#import <Cocoa/Cocoa.h>

typedef enum _EstEIDWebsiteStatus {
	EstEIDWebsiteUnknown = 0,
	EstEIDWebsiteAllowed,
	EstEIDWebsiteBlocked
} EstEIDWebsiteStatus;

@interface EstEIDWhitelist : NSObject
{
	@private
	NSMutableDictionary *m_websites;
}

+ (EstEIDWhitelist *)sharedWhitelist;

- (EstEIDWebsiteStatus)statusForWebsite:(NSURL *)website;
- (void)setStatus:(EstEIDWebsiteStatus)status forWebsite:(NSURL *)website;

@end
