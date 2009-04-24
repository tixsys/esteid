/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDReaderManager.h"

NSString *EstEIDReaderUserInfoLastNameKey = @"SURNAME";
NSString *EstEIDReaderUserInfoFirstNameKey = @"FIRSTNAME";
NSString *EstEIDReaderUserInfoMiddleNameKey = @"MIDDLENAME";
NSString *EstEIDReaderUserInfoSexKey = @"SEX";
NSString *EstEIDReaderUserInfoCitizenKey = @"CITIZEN";
NSString *EstEIDReaderUserInfoBirthDateKey = @"BIRTHDATE";
NSString *EstEIDReaderUserInfoIDKey = @"ID";
NSString *EstEIDReaderUserInfoDocumentIDKey = @"DOCUMENTID";
NSString *EstEIDReaderUserInfoExpiryKey = @"EXPIRY";
NSString *EstEIDReaderUserInfoBirthPlaceKey = @"BIRTHPLACE";
NSString *EstEIDReaderUserInfoIssueDateKey = @"ISSUEDATE";
NSString *EstEIDReaderUserInfoResidencePermitKey = @"RESIDENCEPERMIT";
NSString *EstEIDReaderUserInfoComment1Key = @"COMMENT1";
NSString *EstEIDReaderUserInfoComment2Key = @"COMMENT2";
NSString *EstEIDReaderUserInfoComment3Key = @"COMMENT3";
NSString *EstEIDReaderUserInfoComment4Key = @"COMMENT4";

static inline NSData *CPlusDataToNSData(ByteVec bytes)
{
	return [NSData dataWithBytes:&bytes[0] length:bytes.size()];
}

static inline NSString *CPlusStringToNSString(std::string str)
{
	return [[[NSString alloc] initWithBytes:str.c_str() length:str.length() encoding:NSISOLatin1StringEncoding] autorelease];
}

#pragma mark -

@implementation EstEIDReaderManager

- (unsigned int)selectedReader
{
	return self->m_selectedReader;
}

- (void)setSelectedReader:(unsigned int)selectedReader
{
	if(self->m_selectedReader != selectedReader) {
		self->m_selectedReader = selectedReader;
	}
}

- (NSData *)authCertificate
{
	try {
		EstEidCard card(*self->m_pcscManager, self->m_selectedReader);
		
		return CPlusDataToNSData(card.getAuthCert());
	}
	catch(std::runtime_error err) {
		NSLog(@"EstEID: Couldn't read auth certificate");
	}
	
	return nil;
}

- (NSData *)signCertificate
{
	try {
		EstEidCard card(*self->m_pcscManager, self->m_selectedReader);
		
		return CPlusDataToNSData(card.getSignCert());
	}
	catch(std::runtime_error err) {
		NSLog(@"EstEID: Couldn't read sign certificate");
	}
	
	return nil;
}

- (NSString *)readerName
{
	return [self readerName:self->m_selectedReader];
}

- (NSString *)readerState
{
	try {
		return CPlusStringToNSString(self->m_pcscManager->getReaderState(self->m_selectedReader));
	}
	catch(std::runtime_error err) {
#if DEBUG
		NSLog(@"EstEID: Couldn't read reader state");
#endif
	}
	
	return nil;
}

- (NSDictionary *)userInfo
{
	try {
		EstEidCard card(*self->m_pcscManager, self->m_selectedReader);
		vector <std::string> data;
		
		if(card.readPersonalData(data, EstEidCard::SURNAME, EstEidCard::COMMENT4)) {
			NSMutableDictionary *userInfo = [NSMutableDictionary dictionary];
			
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::SURNAME]) forKey:EstEIDReaderUserInfoLastNameKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::FIRSTNAME]) forKey:EstEIDReaderUserInfoFirstNameKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::MIDDLENAME]) forKey:EstEIDReaderUserInfoMiddleNameKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::SEX]) forKey:EstEIDReaderUserInfoSexKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::CITIZEN]) forKey:EstEIDReaderUserInfoCitizenKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::BIRTHDATE]) forKey:EstEIDReaderUserInfoBirthDateKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::ID]) forKey:EstEIDReaderUserInfoIDKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::DOCUMENTID]) forKey:EstEIDReaderUserInfoDocumentIDKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::EXPIRY]) forKey:EstEIDReaderUserInfoExpiryKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::BIRTHPLACE]) forKey:EstEIDReaderUserInfoBirthPlaceKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::ISSUEDATE]) forKey:EstEIDReaderUserInfoIssueDateKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::RESIDENCEPERMIT]) forKey:EstEIDReaderUserInfoResidencePermitKey];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::COMMENT1]) forKey:EstEIDReaderUserInfoComment1Key];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::COMMENT2]) forKey:EstEIDReaderUserInfoComment2Key];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::COMMENT3]) forKey:EstEIDReaderUserInfoComment3Key];
			[userInfo setValue:CPlusStringToNSString(data[EstEidCard::COMMENT4]) forKey:EstEIDReaderUserInfoComment4Key];
			
			return userInfo;
		}
	}
	catch(std::runtime_error err) {
		NSLog(@"EstEID: Couldn't read userInfo");
	}
	
	return nil;
}

- (NSString *)readerName:(unsigned int)reader
{
	try {
		return CPlusStringToNSString(self->m_pcscManager->getReaderName(reader));
	}
	catch(std::runtime_error err) {
		NSLog(@"EstEID: Couldn't read reader name (reader=%d)", reader);
	}
	
	return nil;
}

- (NSDictionary *)readerStates
{
	NSMutableDictionary *readerStates = [NSMutableDictionary dictionary];
	
	try {
		int i, c;
		
		for(i = 0, c = self->m_pcscManager->getReaderCount(false); i < c; i++) {
			[readerStates setObject:CPlusStringToNSString(self->m_pcscManager->getReaderState(i)) forKey:[NSNumber numberWithInt:i]];
		}
	}
	catch(std::runtime_error err) {
		NSLog(@"EstEID: Couldn't get reader states");
	}
	
	return readerStates;
}

- (unsigned int)recommendedReader
{
	try {
		int i, c;
		
		for(i = 0, c = self->m_pcscManager->getReaderCount(true); i < c; i++) {
			if(self->m_pcscManager->getReaderState(i).compare("PRESENT") == 0) {
				return i;
			}
		}
	}
	catch(std::runtime_error err) {
		NSLog(@"EstEID: Couldn't get the recommended reader");
	}
	
	return 0;
}

#pragma mark NSObject

- (id)init
{
	self = [super init];
	
	if(self) {
		self->m_pcscManager = new PCSCManager();
		self->m_selectedReader = [self recommendedReader];
	}
	
	return self;
}

- (void)dealloc
{
	delete self->m_pcscManager;
	
	[super dealloc];
}

@end
