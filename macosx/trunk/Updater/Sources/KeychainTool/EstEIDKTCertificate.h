/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

@interface EstEIDKTCertificate : NSObject
{
	@private
	void *m_internal;
}

- (id)init;
- (id)initWithData:(NSData *)data;

- (NSString *)CN;
- (NSArray *)websites;
- (BOOL)setWebsites:(NSArray *)websites;

@end
