/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDWebObject.h"

@interface EstEIDWebCertificate : EstEIDWebObject
{
	@private
	NSString *m_CN;
	NSString *m_validFrom;
	NSString *m_validTo;
	NSString *m_issuerCN;
	NSString *m_keyUsage;
	NSString *m_certificate;
	NSString *m_identifier;
}

- (id)initWithData:(NSData *)data;

/**
 * @name JavaScript Properties
 */

- (NSString *)CN;
- (NSString *)validFrom;
- (NSString *)validTo;
- (NSString *)issuerCN;
- (NSString *)keyUsage;
- (NSString *)certificate;
- (NSString *)identifier;

@end
