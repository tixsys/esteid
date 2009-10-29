/* Copyright (c) 2009 Janek Priimann */

#import <CommonCrypto/CommonDigest.h>
#import <Security/Security.h>
#import "common.h"
#import "PCSCManager.h"
#import "SmartCardManager.h"
#import "EstEidCard.h"
#import "EstEIDKTCertificate.h"

OSStatus SecKeychainItemCreatePersistentReference(SecKeychainItemRef itemRef, CFDataRef *persistentItemRef);

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
			self->m_internal = internal;
		} else {
			[self release];
			self = nil;
		}
	}
	
	return self;
}

- (NSString *)CN
{
	NSString *CN = nil;
	CSSM_CL_HANDLE CLhandle;
	CSSM_HANDLE FRHandle;
	CSSM_DATA *value;
	CSSM_DATA data;
	UInt32 count;
	
	if(SecCertificateGetCLHandle((SecCertificateRef)self->m_internal, &CLhandle) == noErr &&
	   SecCertificateGetData((SecCertificateRef)self->m_internal, &data) == noErr &&
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
						case BER_TAG_T61_STRING:
							encoding = kCFStringEncodingISOLatin1;
							cn = &tvpPtr->value;
						case BER_TAG_PKIX_BMP_STRING:
							encoding = kCFStringEncodingUnicode;
							cn = &tvpPtr->value;
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

- (NSArray *)websites
{
	NSMutableArray *websites = [NSMutableArray array];
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
					NSString *website = nil;
					
					switch(attr.tag) {
						case kSecServiceItemAttr:
							website = [[[NSString alloc] initWithData:[NSData dataWithBytes:attr.data length:attr.length] encoding:NSUTF8StringEncoding] autorelease];
							break;
						case kSecGenericItemAttr:
							// TODO: Epic! How the fsk do I make it work with 10.4 too?!!?!?
							break;
					}
					
					if(website) {
						[websites addObject:website];
					}
				}
				
				SecKeychainItemFreeAttributesAndData(attributes, NULL);
			}
			
			CFRelease(item);
		}
		
		CFRelease(search);
	}
	
	[websites sortUsingSelector:@selector(compare:)];
	
	return websites;
}

- (BOOL)setWebsites:(NSArray *)websites
{
	// SecCertificateAddToKeychain(SecCertificateRef certificate, SecKeychainRef keychain);
	
	return NO;
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
