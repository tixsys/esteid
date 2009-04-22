/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDManager.h"

NSString *EstEIDManagerUserInfoLastNameKey = @"SURNAME";
NSString *EstEIDManagerUserInfoFirstNameKey = @"FIRSTNAME";
NSString *EstEIDManagerUserInfoMiddleNameKey = @"MIDDLENAME";
NSString *EstEIDManagerUserInfoSexKey = @"SEX";
NSString *EstEIDManagerUserInfoCitizenKey = @"CITIZEN";
NSString *EstEIDManagerUserInfoBirthDateKey = @"BIRTHDATE";
NSString *EstEIDManagerUserInfoIDKey = @"ID";
NSString *EstEIDManagerUserInfoDocumentIDKey = @"DOCUMENTID";
NSString *EstEIDManagerUserInfoExpiryKey = @"EXPIRY";
NSString *EstEIDManagerUserInfoBirthPlaceKey = @"BIRTHPLACE";
NSString *EstEIDManagerUserInfoIssueDateKey = @"ISSUEDATE";
NSString *EstEIDManagerUserInfoResidencePermitKey = @"RESIDENCEPERMIT";
NSString *EstEIDManagerUserInfoComment1Key = @"COMMENT1";
NSString *EstEIDManagerUserInfoComment2Key = @"COMMENT2";
NSString *EstEIDManagerUserInfoComment3Key = @"COMMENT3";
NSString *EstEIDManagerUserInfoComment4Key = @"COMMENT4";

static inline NSData *CPlusDataToNSData(ByteVec bytes)
{
	return [NSData dataWithBytes:&bytes length:bytes.size()];
}

static inline NSString *CPlusStringToNSString(std::string str)
{
	return [[[NSString alloc] initWithBytes:str.c_str() length:str.length() encoding:NSISOLatin1StringEncoding] autorelease];
}

#pragma mark -

@implementation EstEIDManager

- (NSData *)authCertificate
{
	try {
		EstEidCard card(*self->m_pcscManager, (unsigned int)0);
		
		return CPlusDataToNSData(card.getAuthCert());
	} catch(std::runtime_error err) {
		NSLog(@"EstEID: Couldn't read auth certificate");
	}
	
	return nil;
}

- (NSData *)signCertificate
{
	try {
		EstEidCard card(*self->m_pcscManager, (unsigned int)0);
		
		return CPlusDataToNSData(card.getSignCert());
	} catch(std::runtime_error err) {
		NSLog(@"EstEID: Couldn't read sign certificate");
	}
	
	return nil;
}

- (NSString *)nameForReader:(int)reader
{
	try {
		return CPlusStringToNSString(self->m_pcscManager->getReaderName(reader));
	} catch(std::runtime_error err) {
		NSLog(@"EstEID: Couldn't read reader name");
	}
	
	return nil;
}

- (NSDictionary *)userInfo
{
	try {
		EstEidCard card(*self->m_pcscManager, (unsigned int)0);
		vector <std::string> data;
		
		if(card.readPersonalData(data, EstEidCard::SURNAME, EstEidCard::COMMENT4)) {
			NSMutableDictionary *userInfo = [NSMutableDictionary dictionary];
			
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::SURNAME]) forKey:EstEIDManagerUserInfoLastNameKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::FIRSTNAME]) forKey:EstEIDManagerUserInfoFirstNameKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::MIDDLENAME]) forKey:EstEIDManagerUserInfoMiddleNameKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::SEX]) forKey:EstEIDManagerUserInfoSexKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::CITIZEN]) forKey:EstEIDManagerUserInfoCitizenKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::BIRTHDATE]) forKey:EstEIDManagerUserInfoBirthDateKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::ID]) forKey:EstEIDManagerUserInfoIDKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::DOCUMENTID]) forKey:EstEIDManagerUserInfoDocumentIDKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::EXPIRY]) forKey:EstEIDManagerUserInfoExpiryKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::BIRTHPLACE]) forKey:EstEIDManagerUserInfoBirthPlaceKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::ISSUEDATE]) forKey:EstEIDManagerUserInfoIssueDateKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::RESIDENCEPERMIT]) forKey:EstEIDManagerUserInfoResidencePermitKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::COMMENT1]) forKey:EstEIDManagerUserInfoComment1Key];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::COMMENT2]) forKey:EstEIDManagerUserInfoComment2Key];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::COMMENT3]) forKey:EstEIDManagerUserInfoComment3Key];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::COMMENT4]) forKey:EstEIDManagerUserInfoComment4Key];
			
			return userInfo;
		}
	}
	catch(std::runtime_error err) {
		NSLog(@"EstEID: Couldn't read userInfo");
	}
	
	return nil;
}

#pragma mark NSObject

- (id)init
{
	self = [super init];
	
	if(self) {
		self->m_pcscManager = new PCSCManager();
	}
	
	return self;
}

- (void)dealloc
{
	delete self->m_pcscManager;
	
	[super dealloc];
}

@end
