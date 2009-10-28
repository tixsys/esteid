/* Copyright (c) 2009 Janek Priimann */

#import <openssl/asn1.h>
#import <openssl/bio.h>
#import <openssl/pem.h>
#import <openssl/x509.h>
#import "common.h"
#import "PCSCManager.h"
#import "SmartCardManager.h"
#import "EstEidCard.h"
#import "EstEIDKTCertificate.h"

// TODO: Cleanup

static inline NSData *CPlusDataToNSData(ByteVec bytes)
{
	return [NSData dataWithBytes:&bytes[0] length:bytes.size()];
}

static inline NSString *CPlusStringToNSString(std::string str)
{
	return [(NSString *)CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)str.c_str(), str.length(), kCFStringEncodingISOLatin9, NO) autorelease];
}

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

@implementation EstEIDKTCertificate

- (id)initWithData:(NSData *)data
{
	self = [super init];
	
	if(self) {
		BIO *bio;
		
		if((bio = BIO_new_mem_buf((char *)[data bytes], [data length])) != NULL) {
			X509 *x509 = NULL;
			
			if(d2i_X509_bio(bio, &x509) != NULL) {
				self->m_CN = [X509NameEntryToNSString(X509_get_subject_name(x509), NID_commonName) retain];
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

#pragma mark NSObject

- (id)init
{
	try {
		SmartCardManager *manager = new SmartCardManager();
		
		try {
			EstEidCard card(*manager, (unsigned int)0);
			NSData *data = CPlusDataToNSData(card.getAuthCert());
			
			self = [self initWithData:data];
		}
		catch(std::runtime_error err) {
			NSLog(@"%@: Couldn't read auth certificate", NSStringFromClass([self class]));
			
			[self release];
			self = nil;
		}
		
		delete manager;
	}
	// SCError?
	catch(std::runtime_error err) {
		[self release];
		self = nil;
	}
	
	return self;
}

- (void)dealloc
{
	if(self->m_x509) X509_free((X509 *)self->m_x509);
	[self->m_CN release];
	
	[super dealloc];
}

@end
