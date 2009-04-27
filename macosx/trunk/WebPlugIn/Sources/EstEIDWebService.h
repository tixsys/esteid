/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDWebObject.h"

@class EstEIDReaderManager, EstEIDWebCertificate;

extern NSString *EstEIDWebServiceEventCardInsert;
extern NSString *EstEIDWebServiceEventCardRemove;
extern NSString *EstEIDWebServiceEventCardError;

@interface EstEIDWebService : EstEIDWebObject <EstEIDWebService>
{
	@private
	NSURL *m_url;
	NSWindow *m_window;
	NSMutableDictionary *m_eventListeners;
	EstEIDReaderManager *m_readerManager;
}

/**
 * @name JavaScript Properties
 */

- (EstEIDWebCertificate *)authCertificate;
- (EstEIDWebCertificate *)signCertificate;
- (NSString *)lastName;
- (NSString *)firstName;
- (NSString *)middleName;
- (NSString *)sex;
- (NSString *)citizenship;
- (NSString *)birthDate;
- (NSString *)personalID;
- (NSString *)documentID;
- (NSString *)expiryDate;
- (NSString *)placeOfBirth;
- (NSString *)issuedDate;
- (NSString *)residencePermit;
- (NSString *)comment1;
- (NSString *)comment2;
- (NSString *)comment3;
- (NSString *)comment4;

/**
 * @name JavaScript Methods
 */

- (NSString *)version;
- (NSString *)readerName:(NSArray *)arguments;
- (NSString *)sign:(NSArray *)arguments;
- (void)addEventListener:(NSArray *)arguments;
- (void)removeEventListener:(NSArray *)arguments;

@end
