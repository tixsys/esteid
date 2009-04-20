/* Copyright (c) 2009 Janek Priimann */

#import <CommonCrypto/CommonDigest.h>
#import "NSString+Additions.h"

@implementation NSString(Additions)

+ (id)stringWithFileSize:(long long)fileSize
{
	return [[[self alloc] initWithFileSize:fileSize] autorelease];
}

- (id)initWithFileSize:(long long)fileSize
{
	double displaySize = fileSize;
	NSString *displayUnits = nil; 
	
	if(displaySize >= 1000.0F) {
		displaySize /= 1000.0F;
		
		if(displaySize >= 1000.0F) {
			displaySize /= 1000.0F;
			displayUnits = NSLocalizedString(@"MB", nil); 
		} else {
			displayUnits = NSLocalizedString(@"KB", nil); 
		}
	} else {
		displayUnits = NSLocalizedString(@"B", nil); 
	}
	
	return [self initWithFormat:@"%d %@", (int)round(displaySize), displayUnits];
}

- (BOOL)testVersionString:(NSString *)version
{
	NSScanner *scanner1 = [[NSScanner alloc] initWithString:self];
	NSScanner *scanner2 = [[NSScanner alloc] initWithString:(version) ? version : @"0"];
	NSComparisonResult result = NSOrderedSame;
	int value1, value2;
	
	while([scanner1 scanInt:&value1]) {
		if([scanner2 scanInt:&value2]) {
			if(value1 < value2) {
				result = NSOrderedAscending;
				break;
			} else if(value1 > value2) {
				result = NSOrderedDescending;
				break;
			}
		} else if(value1) {
			result = NSOrderedDescending;
			break;
		}
		
		[scanner1 scanString:@"." intoString:nil];
		[scanner2 scanString:@"." intoString:nil];
	}
	
	if(result == NSOrderedSame && ![scanner2 isAtEnd]) {
		while([scanner2 scanInt:&value2]) {
			if(value2 > 0) {
				result = NSOrderedAscending;
				break;
			}
		}
	}
	
	[scanner1 release];
	[scanner2 release];
	
	if([self hasSuffix:@">="]) {
		return (result == NSOrderedSame || result == NSOrderedAscending) ? YES : NO;
	} else if([self hasSuffix:@">"]) {
		return (result == NSOrderedAscending) ? YES : NO;
	} else if([self hasSuffix:@"<="]) {
		return (result == NSOrderedSame || result == NSOrderedDescending) ? YES : NO;
	} else if([self hasSuffix:@"<"]) {
		return (result == NSOrderedDescending) ? YES : NO;
	} else if([self hasSuffix:@"="]) {
		return (result == NSOrderedSame) ? YES : NO;
	}
	
	// '1.0' = '1.0<'
	return (result == NSOrderedDescending) ? YES : NO;
}

// COMPATIBILITY: Only here for 10.4 compatibility
- (long long)longLongValue
{
	NSScanner *scanner = [[NSScanner alloc] initWithString:self];
	long long result = 0;
	
	[scanner scanLongLong:&result];
	[scanner release];
	
	return result;
}

- (NSString *)sha1String
{
	NSMutableString *result = [NSMutableString string];
	CC_SHA1_CTX sha1;
	UInt8 bytes[20];
	int i;
	
	CC_SHA1_Init(&sha1);
	CC_SHA1_Update(&sha1, [self UTF8String], [self lengthOfBytesUsingEncoding:NSUTF8StringEncoding]);
	CC_SHA1_Final(&(bytes[0]), &sha1);
	
	for(i = 0; i < 20; i++) {
		[result appendFormat:@"%02x", bytes[i]];
	}
	
	return result;
}

@end