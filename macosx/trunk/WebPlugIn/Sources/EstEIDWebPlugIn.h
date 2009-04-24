/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDWebObject.h"

@class EstEIDReaderManager, EstEIDWebCertificate, EstEIDWebPlugInProxy;

extern NSString *EstEIDWebPlugInEventCardInsert;
extern NSString *EstEIDWebPlugInEventCardRemove;
extern NSString *EstEIDWebPlugInEventCardError;

@interface EstEIDWebPlugIn : EstEIDWebObject
{
    @private
	NSMutableDictionary *m_eventListeners;
	EstEIDWebCertificate *m_authCertificate;
	EstEIDWebCertificate *m_signCertificate;
	EstEIDReaderManager *m_readerManager;
	NSString *m_readerState;
	EstEIDWebPlugInProxy *m_proxy;
	NSTimer *m_timer;
	NSDictionary *m_userInfo;
}

- (void)invalidate:(NSTimer *)timer;

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
- (NSObject *)sign:(NSArray *)arguments;
- (id)addEventListener:(NSArray *)arguments;
- (id)removeEventListener:(NSArray *)arguments;

@end
