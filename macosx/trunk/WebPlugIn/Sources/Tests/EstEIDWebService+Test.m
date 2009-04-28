/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDWebCertificate.h"
#import "EstEIDWebService.h"
#import "EstEIDWebService+Test.h"

@implementation EstEIDWebServiceTest

- (void)testUserInfo
{
	EstEIDWebService *service = [[EstEIDWebService alloc] init];
	
	STAssertTrue([[service firstName] length] > 0, nil, nil);
	STAssertTrue([[service lastName] length] > 0, nil, nil);
	STAssertNotNil([service middleName], nil, nil);
	STAssertTrue([[service sex] isEqualToString:@"M"] || [[service sex] isEqualToString:@"N"], nil, nil);
	STAssertTrue([[service citizenship] length] == 3, nil, nil);
	STAssertTrue([[service birthDate] length] == 10, nil, nil);
	STAssertTrue([[service personalID] length] > 0, nil, nil);
	STAssertTrue([[service documentID] length] > 0, nil, nil);
	STAssertTrue([[service expiryDate] length] == 10, nil, nil);
	STAssertTrue([[service placeOfBirth] length] > 0, nil, nil);
	STAssertTrue([[service issuedDate] length] == 10, nil, nil);
	STAssertNotNil([service residencePermit], nil, nil);
	STAssertNotNil([service comment1], nil, nil);
	STAssertNotNil([service comment2], nil, nil);
	STAssertNotNil([service comment3], nil, nil);
	STAssertNotNil([service comment4], nil, nil);
	
	[service release];
}

- (void)testAuthCertificate
{
	EstEIDWebService *service = [[EstEIDWebService alloc] init];
	
	STAssertTrue([[[service authCertificate] keyUsage] isEqualToString:@"authentication"], nil, nil);
	STAssertNotNil([[service authCertificate] certificate], nil, nil);
	
	[service release];
}

- (void)testSignCertificate
{
	EstEIDWebService *service = [[EstEIDWebService alloc] init];
	
	STAssertTrue([[[service signCertificate] keyUsage] isEqualToString:@"digital signature"], nil, nil);
	STAssertNotNil([[service signCertificate] certificate], nil, nil);
	
	[service release];
}

@end
