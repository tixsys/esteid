/* Copyright (c) 2008 Janek Priimann */

#import "NSString+Additions+Test.h"
#import "NSString+Additions.h"

@implementation NSStringAdditionsTest

- (void)testSha1String
{
	// SHA1("Hello world") = 7b502c3a1f48c8609ae212cdfb639dee39673f5e
	STAssertTrue([[@"Hello world" sha1String] isEqualToString:@"7b502c3a1f48c8609ae212cdfb639dee39673f5e"], nil, nil);
}

- (void)testVersionCheck
{
	STAssertFalse([@"1.0" testVersionString:@"1.0"], nil, nil);
	STAssertTrue([@"1.0.1" testVersionString:@"1.0"], nil, nil);
	STAssertTrue([@"1.0<" testVersionString:@"0.9"], nil, nil);
	STAssertTrue([@"1.0.4<" testVersionString:@"0.9"], nil, nil);
	STAssertFalse([@"1.0<" testVersionString:@"1.1"], nil, nil);
	STAssertFalse([@"1.0.9<" testVersionString:@"1.1"], nil, nil);
	STAssertTrue([@"1.0=" testVersionString:@"1.0"], nil, nil);
	STAssertFalse([@"1.0=" testVersionString:@"1.0.1"], nil, nil);
	STAssertTrue([@"1.0=" testVersionString:@"1.0.0"], nil, nil);
	STAssertTrue([@"1.0=" testVersionString:@"1.0.0.0.0"], nil, nil);
	STAssertFalse([@"1.0.1=" testVersionString:@"1.0"], nil, nil);
	STAssertTrue([@"1.0.7.4<=" testVersionString:@"1.0.7.4"], nil, nil);
	STAssertTrue([@"1.0.7.4<=" testVersionString:@"1.0.7.1"], nil, nil);
	STAssertFalse([@"1.0.7.4<=" testVersionString:@"1.0.7.4.1"], nil, nil);
	STAssertTrue([@"1.0.7.4<=" testVersionString:@"1.0.7.4.0"], nil, nil);
	STAssertTrue([@"1.0.7.4>=" testVersionString:@"1.0.7.4"], nil, nil);
	STAssertFalse([@"1.0.7.4>=" testVersionString:@"1.0.7.1"], nil, nil);
	STAssertTrue([@"1.0.7.4>=" testVersionString:@"1.0.7.4.1"], nil, nil);	
}

@end
