/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDWebObject.h"

@interface EstEIDWebCertificate : EstEIDWebObject
{
	@private
	NSString *m_CN;
	NSDate *m_validFrom;
	NSDate *m_validTo;
	NSString *m_issuerCN;
	NSString *m_keyUsage;
	NSString *m_certificate;
	NSString *m_serial;
	void *m_x509;
}

- (id)initWithData:(NSData *)data;

/**
 * @name JavaScript Properties
 */

- (NSString *)CN;
- (NSDate *)validFrom;
- (NSDate *)validTo;
- (NSString *)issuerCN;
- (NSString *)keyUsage;
- (NSString *)certificate;
- (NSString *)serial;

@end
