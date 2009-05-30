/* Copyright (c) 2008 Janek Priimann */

#import <openssl/asn1.h>
#import <openssl/bio.h>
#import <openssl/pem.h>
#import <openssl/x509.h>
#import "EstEIDWebCertificate.h"

static NSString *X509NameEntryToNSString(X509_NAME *name, int nid)
{
	int index = (name) ? X509_NAME_get_index_by_NID(name, nid, -1) : -1;
	X509_NAME_ENTRY *entry = (index >= 0) ? X509_NAME_get_entry(name, index) : NULL;
	ASN1_STRING *asnstr = (entry) ? X509_NAME_ENTRY_get_data(entry) : NULL;
	NSString *result = nil;
	
	if(asnstr) {
		unsigned char *data = NULL;
		int length = ASN1_STRING_to_UTF8(&data, asnstr);
		
		if(length >= 0) {
			result = [[[NSString alloc] initWithBytes:data length:length encoding:NSUTF8StringEncoding] autorelease];
		}
		
		OPENSSL_free(data);
	}
	
	return result;
}

static NSString *X509IntegerToNSString(ASN1_INTEGER *integer)
{
	BIO *bio = BIO_new(BIO_s_mem());
	NSString *result = nil;
	char *data;
	int length;
	
	if(i2a_ASN1_INTEGER(bio, integer)) {
		length = BIO_get_mem_data(bio, &data);
		result = [[[NSString alloc] initWithBytes:data length:length encoding:NSUTF8StringEncoding] autorelease];
	}
	
	BIO_free(bio);
	
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

static NSDate *X509DateToNSDate(ASN1_TIME *date)
{
	NSDate *result = nil;
	
	// YYMMDDHHMMSSZ
	if(date != NULL && date->data != NULL) {
		int length = strlen((char *)date->data);
		
		if(length == 13 || length == 15) {
			NSString *str = [[NSString alloc] initWithBytes:date->data length:length encoding:NSASCIIStringEncoding];
			NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
			
			[dateFormatter setDateFormat:(length == 13) ? @"yyMMddHHmmss" : @"yyyyMMddHHmmss"];
			result = [dateFormatter dateFromString:str];
			[dateFormatter release];
			[str release];
		}
	}
	
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
				self->m_CN = [X509NameEntryToNSString(X509_get_subject_name(x509), NID_commonName) retain];
				self->m_keyUsage = [X509NameEntryToNSString(X509_get_subject_name(x509), NID_organizationalUnitName) retain];
				self->m_validFrom = [X509DateToNSDate(X509_get_notBefore(x509)) retain];
				self->m_validTo = [X509DateToNSDate(X509_get_notAfter(x509)) retain];
				self->m_issuerCN = [X509NameEntryToNSString(X509_get_issuer_name(x509), NID_commonName) retain];
				self->m_certificate = [X509ToNSString(x509) retain];
				self->m_serial = [X509IntegerToNSString(X509_get_serialNumber(x509)) retain];
				self->m_x509 = x509;
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

- (NSDate *)validFrom
{
	return self->m_validFrom;
}

- (NSDate *)validTo
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

- (NSString *)serial
{
	return self->m_serial;
}

#pragma mark EstEIDWebObject

+ (SEL)selectorForProperty:(const char *)name
{
	if(strcmp(name, "CN") == 0) return @selector(CN);
	if(strcmp(name, "validFrom") == 0) return @selector(validFrom);
	if(strcmp(name, "validTo") == 0) return @selector(validTo);
	if(strcmp(name, "issuerCN") == 0) return @selector(issuerCN);
	if(strcmp(name, "keyUsage") == 0) return @selector(keyUsage);
	if(strcmp(name, "cert") == 0) return @selector(certificate);
	if(strcmp(name, "serial") == 0) return @selector(serial);
	
	return [super selectorForProperty:name];
}

#pragma mark NSObject

- (void)dealloc
{
	X509_free((X509 *)self->m_x509);
	
	[self->m_CN release];
	[self->m_validFrom release];
	[self->m_validTo release];
	[self->m_issuerCN release];
	[self->m_keyUsage release];
	[self->m_certificate release];
	[self->m_serial release];
	
	[super dealloc];
}

@end
