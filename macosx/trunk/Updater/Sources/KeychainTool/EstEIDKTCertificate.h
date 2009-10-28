/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

@interface EstEIDKTCertificate : NSObject
{
	@private
	NSString *m_CN;
	void *m_x509;
}

- (id)init;
- (id)initWithData:(NSData *)data;

- (NSString *)CN;

@end
