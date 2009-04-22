/* Copyright (c) 2009 Janek Priimann */

// FIXME: Okay... ?!?
#define __COREFOUNDATION_CFPLUGINCOM__

#import "common.h"
#import "PCSCManager.h"
#import "EstEidCard.h"

#import <Foundation/Foundation.h>

extern NSString *EstEIDManagerUserInfoLastNameKey;
extern NSString *EstEIDManagerUserInfoFirstNameKey;
extern NSString *EstEIDManagerUserInfoMiddleNameKey;
extern NSString *EstEIDManagerUserInfoSexKey;
extern NSString *EstEIDManagerUserInfoCitizenKey;
extern NSString *EstEIDManagerUserInfoBirthDateKey;
extern NSString *EstEIDManagerUserInfoIDKey;
extern NSString *EstEIDManagerUserInfoDocumentIDKey;
extern NSString *EstEIDManagerUserInfoExpiryKey;
extern NSString *EstEIDManagerUserInfoBirthPlaceKey;
extern NSString *EstEIDManagerUserInfoIssueDateKey;
extern NSString *EstEIDManagerUserInfoResidencePermitKey;
extern NSString *EstEIDManagerUserInfoComment1Key;
extern NSString *EstEIDManagerUserInfoComment2Key;
extern NSString *EstEIDManagerUserInfoComment3Key;
extern NSString *EstEIDManagerUserInfoComment4Key;

@interface EstEIDManager : NSObject
{
	@private
	PCSCManager *m_pcscManager;
}

- (NSData *)authCertificate;
- (NSData *)signCertificate;
- (NSString *)nameForReader:(int)reader;
- (NSDictionary *)userInfo;
- (NSString *)readerState;

@end
