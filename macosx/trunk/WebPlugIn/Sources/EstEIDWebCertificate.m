/* Copyright (c) 2008 Janek Priimann */

#import <Security/SecCertificate.h>
#import "EstEIDWebCertificate.h"

@implementation EstEIDWebCertificate

- (id)initWithData:(NSData *)data
{
	self = [super init];
	
	if(self) {
		SecCertificateRef certificate;
		CSSM_DATA cssm;
		
		cssm.Data = (UInt8 *)[data bytes];
		cssm.Length = [data length];
		
		if(SecCertificateCreateFromData(&cssm, CSSM_CERT_X_509v3, CSSM_CERT_ENCODING_DER, &certificate) == noErr) {
			// Only 10.5
			//SecCertificateCopyCommonName(certificate, (CFStringRef *)&self->m_CN);
			self->m_CN = [@"CN!" retain];
			// TODO: 
			//SecCertificateGetIssuer(SecCertificateRef certificate, const CSSM_X509_NAME **issuer);
			CFRelease(certificate);
		} else {
			[self release];
			self = nil;
		}
	}
	
	return self;
}

- (NSString *)CN
{
	return self->m_CN;
}

- (NSString *)validFrom
{
	return self->m_validFrom;
}

- (NSString *)validTo
{
	return self->m_validTo;
}

- (NSString *)issuerCN
{
	return self->m_issuerCN;
}

- (NSString *)keyUsage
{
	return self->m_keyUsage;
}

- (NSString *)certificate
{
	return self->m_certificate;
}

- (NSString *)identifier
{
	return self->m_identifier;
}

#pragma mark EstEIDWebObject

+ (SEL)selectorForGetProperty:(const char *)name
{
	if(strcmp(name, "CN") == 0) return @selector(CN);
	if(strcmp(name, "validFrom") == 0) return @selector(validFrom);
	if(strcmp(name, "validTo") == 0) return @selector(validTo);
	if(strcmp(name, "issuerCN") == 0) return @selector(issuerCN);
	if(strcmp(name, "keyUsage") == 0) return @selector(keyUsage);
	if(strcmp(name, "cert") == 0) return @selector(certificate);
	if(strcmp(name, "id") == 0) return @selector(identifier);
	
	return [super selectorForGetProperty:name];
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_CN release];
	[self->m_validFrom release];
	[self->m_validTo release];
	[self->m_issuerCN release];
	[self->m_keyUsage release];
	[self->m_certificate release];
	[self->m_identifier release];
	
	[super dealloc];
}

@end
