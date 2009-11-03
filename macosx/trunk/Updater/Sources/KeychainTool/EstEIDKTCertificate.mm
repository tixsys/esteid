/* Copyright (c) 2009 Janek Priimann */

#import <CommonCrypto/CommonDigest.h>
#import <Security/Security.h>
#import <dlfcn.h>
#import "common.h"
#import "PCSCManager.h"
#import "SmartCardManager.h"
#import "EstEidCard.h"
#import "EstEIDKTCertificate.h"
#import "EstEIDKTIdentityPreference.h"

static NSString *EstEIDKTCertificateGetCommonName(SecCertificateRef certificate)
{
	NSString *CN = nil;
	CSSM_CL_HANDLE CLhandle;
	CSSM_HANDLE FRHandle;
	CSSM_DATA *value;
	CSSM_DATA data;
	uint32 count;
	
	if(SecCertificateGetCLHandle(certificate, &CLhandle) == noErr &&
	   SecCertificateGetData(certificate, &data) == noErr &&
	   CSSM_CL_CertGetFirstFieldValue(CLhandle, &data, &CSSMOID_X509V1SubjectNameCStruct, &FRHandle, &count, &value) == CSSM_OK &&
	   count > 0 && value && value->Data) {
		CSSM_X509_NAME_PTR x509Name = (CSSM_X509_NAME_PTR)value->Data;
		CFStringBuiltInEncodings encoding;
		const CSSM_DATA	*cn = NULL;
		
		for(uint32 rdnDex = 0; rdnDex < x509Name->numberOfRDNs; rdnDex++) {
			const CSSM_X509_RDN *rdnPtr = &(x509Name->RelativeDistinguishedName[rdnDex]);
			
			for(uint32 tvpDex = 0; tvpDex < rdnPtr->numberOfPairs; tvpDex++) {
				const CSSM_X509_TYPE_VALUE_PAIR *tvpPtr = &rdnPtr->AttributeTypeAndValue[tvpDex];
				
				if(tvpPtr->type.Length == CSSMOID_CommonName.Length && memcmp(tvpPtr->type.Data, CSSMOID_CommonName.Data, CSSMOID_CommonName.Length) == 0) {
					switch(tvpPtr->valueType) {
						case BER_TAG_PRINTABLE_STRING:
						case BER_TAG_IA5_STRING:
						case BER_TAG_PKIX_UTF8_STRING:
							encoding = kCFStringEncodingUTF8;
							cn = &tvpPtr->value;
							break;
						case BER_TAG_T61_STRING:
							encoding = kCFStringEncodingISOLatin1;
							cn = &tvpPtr->value;
							break;
						case BER_TAG_PKIX_BMP_STRING:
							encoding = kCFStringEncodingUnicode;
							cn = &tvpPtr->value;
							break;
						default:
							break;
					}
				}
				
				if(cn) break;
			}
			
			if(cn) break;
		}
		
		if(cn) {
			CN = [(NSString *)CFStringCreateWithBytes(NULL, cn->Data, CFIndex(cn->Length), encoding, YES) autorelease];
		}
		
		CSSM_CL_FreeFieldValue(CLhandle, &CSSMOID_X509V1SubjectNameCStruct, value);		
	}
	
	return CN;
}

#pragma mark -

@implementation EstEIDKTCertificate

- (id)initWithData:(NSData *)data
{
	self = [super init];
	
	if(self) {
		SecCertificateRef internal;
		CSSM_DATA cssmd;
		
		cssmd.Data = (uint8 *)[data bytes];
		cssmd.Length = [data length];
		
		if(SecCertificateCreateFromData(&cssmd, CSSM_CERT_UNKNOWN, CSSM_CERT_ENCODING_UNKNOWN, &internal) == noErr) {
			SecIdentitySearchRef search;
			NSString *cn;
			
			self->m_internal = internal;
			cn = [self CN];
			
			if(SecIdentitySearchCreate(NULL, CSSM_KEYUSE_SIGN, &search) == noErr && [cn length] > 0) {
				SecIdentityRef identity;
				
				while(SecIdentitySearchCopyNext(search, &identity) == noErr) {
					SecCertificateRef certificate;
					
					if(SecIdentityCopyCertificate(identity, &certificate) == noErr) {
						if([EstEIDKTCertificateGetCommonName(certificate) isEqualToString:cn]) {
							CFRelease(self->m_internal);
							self->m_internal = certificate;
							break;
						} else {
							CFRelease(certificate);
						}
					}
					
					CFRelease(identity);
				}
				
				CFRelease(search);
			}
		} else {
			[self release];
			self = nil;
		}
	}
	
	return self;
}

- (NSString *)CN
{
	return EstEIDKTCertificateGetCommonName((SecCertificateRef)self->m_internal);
}

- (NSDictionary *)identityPreferences
{
	NSMutableDictionary *identityPreferences = [NSMutableDictionary dictionary];
	SecKeychainAttribute attributes[1];
	SecKeychainSearchRef search;
	SecKeychainAttributeList list;
	FourCharCode code = 'iprf';
	
	attributes[0].tag = kSecTypeItemAttr;
	attributes[0].length = sizeof(FourCharCode);
	attributes[0].data = &code;
	
	list.attr = &(attributes[0]);
	list.count = sizeof(attributes) / sizeof(SecKeychainAttribute);
	
	if(SecKeychainSearchCreateFromAttributes(NULL, kSecGenericPasswordItemClass, &list, &search) == noErr) {
		SecKeychainItemRef item;
		
		while(SecKeychainSearchCopyNext(search, &item) == noErr) {
			UInt32 tag[2] = { kSecServiceItemAttr, kSecGenericItemAttr };
			UInt32 format[2] = { CSSM_DB_ATTRIBUTE_FORMAT_STRING, CSSM_DB_ATTRIBUTE_FORMAT_BLOB };
			SecKeychainAttributeInfo attrInfo;
			SecKeychainAttributeList *attributes;
			
			attrInfo.count = 2;
			attrInfo.tag = &(tag[0]);
			attrInfo.format = &(format[0]);
			
			if(SecKeychainItemCopyAttributesAndData(item, &attrInfo, NULL, &attributes, 0, NULL) == noErr) {
				for(int i = 0; i < attributes->count; i++) {
					SecKeychainAttribute attr = attributes->attr[i];
					NSString *key = nil;
					
					switch(attr.tag) {
						case kSecServiceItemAttr:
							key = [[[NSString alloc] initWithData:[NSData dataWithBytes:attr.data length:attr.length] encoding:NSUTF8StringEncoding] autorelease];
							break;
						case kSecGenericItemAttr:
							// TODO: Epic! How the fsk do I make it work with 10.4 too?!!?!?
							break;
					}
					
					if(key) {
						EstEIDKTIdentityPreference *identityPreference = [[EstEIDKTIdentityPreference alloc] initWithItem:item];
						
						[identityPreferences setValue:identityPreference forKey:key];
						[identityPreference release];
					}
				}
				
				SecKeychainItemFreeAttributesAndData(attributes, NULL);
			}
			
			CFRelease(item);
		}
		
		CFRelease(search);
	}
	
	return identityPreferences;
	
}

- (NSArray *)websites
{
	NSMutableArray *websites = [NSMutableArray array];
	
	[websites addObjectsFromArray:[[self identityPreferences] allKeys]];
	[websites sortUsingSelector:@selector(compare:)];
	
	return websites;
}

/*
 security set-identity-preference -h
 
 Usage: set-identity-preference [-c identity] [-s service] [-u keyUsage] [-Z hash] [keychain...]
	-c  Specify identity by common name of the certificate
	-s  Specify service (may be a URL, RFC822 email address, DNS host, or other name) for which this identity is to be preferred
	-u  Specify key usage (optional) - see man page for values
	-Z  Specify identity by SHA-1 hash of certificate (optional)
	
	Set the preferred identity to use for a service.
 */
- (BOOL)setWebsites:(NSArray *)websites
{
	SecKeychainRef keychain;
	BOOL result = YES;
	
	if(SecKeychainCopyDefault(&keychain) == noErr) {
		OSStatus (*secIdentityCreateWithCertificate)(CFTypeRef keychainOrArray, SecCertificateRef certificateRef, SecIdentityRef *identityRef) = (OSStatus (*)(CFTypeRef, SecCertificateRef, SecIdentityRef *))dlsym(RTLD_DEFAULT, "SecIdentityCreateWithCertificate");
		OSStatus (*secIdentitySetPreference)(SecIdentityRef identity, CFStringRef name, CSSM_KEYUSE keyUsage) = (OSStatus (*)(SecIdentityRef, CFStringRef, CSSM_KEYUSE))dlsym(RTLD_DEFAULT, "SecIdentitySetPreference");
		NSDictionary *identityPreferences = [self identityPreferences];
		NSEnumerator *enumerator = [identityPreferences keyEnumerator];
		SecIdentityRef identity = NULL;
		NSString *cn = [self CN];
		NSString *website;
		
		while((website = [enumerator nextObject]) != nil) {
			if(![websites containsObject:website]) {
				EstEIDKTIdentityPreference *identityPreference = [identityPreferences objectForKey:website];
				
				if(SecKeychainItemDelete([identityPreference item]) != noErr) {
					result = NO;
				}
			}
		}
		
		enumerator = [websites objectEnumerator];
		
		if(secIdentityCreateWithCertificate != NULL) {
			OSStatus error;
			
			if((error = secIdentityCreateWithCertificate(NULL, (SecCertificateRef)self->m_internal, &identity)) != noErr) {
				NSLog(@"%@: Creating identity for '%@' failed with error %d", NSStringFromClass([self class]), cn, error);
			}
		}
		
		while((website = [enumerator nextObject]) != nil) {
			if(secIdentitySetPreference != NULL && identity != NULL) {
				OSStatus error;
				
				if((error = secIdentitySetPreference(identity, (CFStringRef)website, CSSM_KEYUSE_SIGN)) != noErr && error != errSecDuplicateItem) {
					NSLog(@"%@: Setting identity preference for '%@' failed with error %d", NSStringFromClass([self class]), website, error);
					result = NO;
				}
			} else {
				// TODO: Figure out 10.4
				@try {
					NSTask *task = [[[NSTask alloc] init] autorelease];
					
					[task setLaunchPath:@"/usr/bin/security"];
					[task setArguments:[NSArray arrayWithObjects:@"set-identity-preference", @"-c", cn, @"-s", website, nil]];
					[task launch];
					[task waitUntilExit];
					
					if([task terminationStatus] != 0) {
						NSLog(@"%@: Saving '%@' failed", NSStringFromClass([self class]), website);
						result = NO;
					}
				}
				@catch(NSException *e) {
					NSLog(@"%@: %@, %@", NSStringFromClass([self class]), website, e);
					result = NO;
				}
			}
		}
		
		if(identity) {
			CFRelease(identity);
		}
	} else {
		result = NO;
	}
	
	return result;
}

#pragma mark NSObject

- (id)init
{
	try {
		SmartCardManager *manager = new SmartCardManager();
		
		try {
			EstEidCard card(*manager, (unsigned int)0);
			ByteVec bytes = card.getAuthCert();
			
			self = [self initWithData:[NSData dataWithBytes:&bytes[0] length:bytes.size()]];
		}
		catch(std::runtime_error err) {
			NSLog(@"%@: Couldn't read auth certificate", NSStringFromClass([self class]));
			
			[self release];
			self = nil;
		}
		
		delete manager;
	}
	catch(std::runtime_error err) {
		[self release];
		self = nil;
	}
	
	return self;
}

- (void)dealloc
{
	if(self->m_internal) CFRelease((SecCertificateRef)self->m_internal);
	
	[super dealloc];
}

@end
