/* Copyright (c) 2008 Janek Priimann */

#import <openssl/bio.h>
#import <openssl/pem.h>
#import <openssl/x509.h>
#import "EstEIDWebCertificate.h"

static NSString *X509NameToNSString(X509_NAME *name)
{
	NSString *result = nil;
	
	if(name) {
		BIO *bio = BIO_new(BIO_s_mem());
		char *data;
		int length;
		
		X509_NAME_print_ex(bio, name, 0, 0);
		length = BIO_get_mem_data(bio, &data);
		result = [[[NSString alloc] initWithBytes:data length:length encoding:NSUTF8StringEncoding] autorelease];
				
		BIO_free(bio);
	}
	
	return result;
}

static NSString *X509ToNSString(X509 *x509)
{
	BIO *bio = BIO_new(BIO_s_mem());
	NSString *result = nil;
	char *data;
	int length;
	
	if(PEM_write_bio_X509(bio, x509)) {
		length = BIO_get_mem_data(bio, &data);
		result = [[[NSString alloc] initWithBytes:data length:length encoding:NSUTF8StringEncoding] autorelease];
	}
	
	BIO_free(bio);
	
	return result;
}

#pragma mark -

@implementation EstEIDWebCertificate

- (id)initWithData:(NSData *)data
{
	self = [super init];
	
	if(self) {
		BIO *bio;
		
		if((bio = BIO_new_mem_buf((char *)[data bytes], [data length])) != NULL) {
			X509 *x509 = NULL;
			
			if(d2i_X509_bio(bio, &x509) != NULL) {
				// TODO: Figure this out
				self->m_CN = [X509NameToNSString(X509_get_subject_name(x509)) retain];
				self->m_validFrom = nil;
				self->m_validTo = nil;
				self->m_issuerCN = [X509NameToNSString(X509_get_issuer_name(x509)) retain];
				self->m_keyUsage = nil;
				self->m_certificate = [X509ToNSString(x509) retain];
				self->m_identifier = nil;
				
				X509_free(x509);
			}
			
			BIO_free(bio);
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
