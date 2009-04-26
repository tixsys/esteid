/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDWebCertificate.h"
#import "EstEIDWebPlugIn.h"
#import "EstEIDWebPlugIn+Test.h"

@implementation EstEIDWebPlugInTest

- (void)testUserInfo
{
	EstEIDWebPlugIn *plugin = [[EstEIDWebPlugIn alloc] init];
	
	STAssertTrue([[plugin firstName] length] > 0, nil, nil);
	STAssertTrue([[plugin lastName] length] > 0, nil, nil);
	STAssertNotNil([plugin middleName], nil, nil);
	STAssertTrue([[plugin sex] isEqualToString:@"M"] || [[plugin sex] isEqualToString:@"N"], nil, nil);
	STAssertTrue([[plugin citizenship] length] == 3, nil, nil);
	STAssertTrue([[plugin birthDate] length] == 10, nil, nil);
	STAssertTrue([[plugin personalID] length] > 0, nil, nil);
	STAssertTrue([[plugin documentID] length] > 0, nil, nil);
	STAssertTrue([[plugin expiryDate] length] == 10, nil, nil);
	STAssertTrue([[plugin placeOfBirth] length] > 0, nil, nil);
	STAssertTrue([[plugin issuedDate] length] == 10, nil, nil);
	STAssertNotNil([plugin residencePermit], nil, nil);
	STAssertNotNil([plugin comment1], nil, nil);
	STAssertNotNil([plugin comment2], nil, nil);
	STAssertNotNil([plugin comment3], nil, nil);
	STAssertNotNil([plugin comment4], nil, nil);
	
	[plugin release];
}

- (void)testAuthCertificate
{
	EstEIDWebPlugIn *plugin = [[EstEIDWebPlugIn alloc] init];
	
	STAssertTrue([[[plugin authCertificate] keyUsage] isEqualToString:@"authentication"], nil, nil);
	STAssertNotNil([[plugin authCertificate] certificate], nil, nil);
	
	[plugin release];
}

- (void)testSignCertificate
{
	EstEIDWebPlugIn *plugin = [[EstEIDWebPlugIn alloc] init];
	
	STAssertTrue([[[plugin signCertificate] keyUsage] isEqualToString:@"digital signature"], nil, nil);
	STAssertNotNil([[plugin signCertificate] certificate], nil, nil);
	
	[plugin release];
}

@end
