/* Copyright (c) 2009 Janek Priimann */

// FIXME: Okay... ?!?
#define __COREFOUNDATION_CFPLUGINCOM__

#import "common.h"
#import "PCSCManager.h"
#import "EstEidCard.h"

#import <Foundation/Foundation.h>

extern NSString *EstEIDReaderUserInfoLastNameKey;
extern NSString *EstEIDReaderUserInfoFirstNameKey;
extern NSString *EstEIDReaderUserInfoMiddleNameKey;
extern NSString *EstEIDReaderUserInfoSexKey;
extern NSString *EstEIDReaderUserInfoCitizenKey;
extern NSString *EstEIDReaderUserInfoBirthDateKey;
extern NSString *EstEIDReaderUserInfoIDKey;
extern NSString *EstEIDReaderUserInfoDocumentIDKey;
extern NSString *EstEIDReaderUserInfoExpiryKey;
extern NSString *EstEIDReaderUserInfoBirthPlaceKey;
extern NSString *EstEIDReaderUserInfoIssueDateKey;
extern NSString *EstEIDReaderUserInfoResidencePermitKey;
extern NSString *EstEIDReaderUserInfoComment1Key;
extern NSString *EstEIDReaderUserInfoComment2Key;
extern NSString *EstEIDReaderUserInfoComment3Key;
extern NSString *EstEIDReaderUserInfoComment4Key;

@interface EstEIDReaderManager : NSObject
{
	@private
	PCSCManager *m_pcscManager;
	unsigned int m_selectedReader;
}

/**
 * @name Managing the Selected Reader
 */

- (NSData *)authCertificate;
- (NSData *)signCertificate;
- (NSDictionary *)userInfo;
- (NSString *)readerName;
- (NSString *)readerState;

/**
 * @name Getting and Modifying the Selected Reader
 */

- (unsigned int)selectedReader;
- (void)setSelectedReader:(unsigned int)selectedReader;

/**
 * @name Querying Readers
 */

- (NSString *)readerName:(unsigned int)reader;
- (NSDictionary *)readerStates;
- (unsigned int)recommendedReader;

@end
