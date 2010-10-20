/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDCMPlugIn.h"
#import "EstEIDCMPlugIn+Test.h"

@implementation EstEIDCMPlugInTest

- (void)testPlugIn
{
	EstEIDCMPlugIn *plugIn = [[EstEIDCMPlugIn alloc] init];
	
	STAssertNotNil([plugIn contextMenu:nil], nil, nil);
	STAssertNotNil([plugIn contextMenu:[NSArray array]], nil, nil);
	STAssertNotNil([plugIn contextMenu:[NSArray arrayWithObject:@"Invalid"]], nil, nil);
	
	[plugIn release];
}

@end
