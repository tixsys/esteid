/* Copyright (c) 2009 Janek Priimann */

#import <CommonCrypto/CommonDigest.h>
#import <openssl/sha.h>
#import <sys/stat.h>
#import <unistd.h>
#import "EstEIDAgent.h"
#import "EstEIDConfiguration.h"
#import "EstEIDManifestPackage.h"
#import "EstEIDPackage.h"

NSString *EstEIDPackageErrorDomain = @"EstEIDPackageErrorDomain";

@implementation EstEIDPackage

- (id)initWithContentsOfFile:(NSString *)path metadata:(EstEIDManifestPackage *)metadata
{
	self = [super init];
	
	if(self) {
		self->m_metadata = [metadata retain];
		self->m_path = [path retain];
	}
	
	return self;
}

- (long long)length
{
	int file = open([self->m_path UTF8String], O_RDONLY);
	long long length = 0;
	
	if(file != -1) {
		struct stat buffer;
		
		fstat(file, &buffer);
		length = buffer.st_size;
		
		close(file);
	}
	
	return length;
}

- (EstEIDManifestPackage *)metadata
{
	return self->m_metadata;
}

- (NSString *)path
{
	return self->m_path;
}

- (NSString *)sha1
{
	int file = open([self->m_path UTF8String], O_RDONLY);
	NSMutableString *result = nil;
	
	if(file != -1) {
		UInt8 bytes[SHA_DIGEST_LENGTH];
		UInt8 buffer[8192];
		CC_SHA1_CTX sha1;
		int i, length;
		
		CC_SHA1_Init(&sha1);
		
		while((length = read(file, &(buffer[0]), 8192)) > 0) {
			CC_SHA1_Update(&sha1, &(buffer[0]), length);
		}
		
		CC_SHA1_Final(&(bytes[0]), &sha1);
		
		result = [NSMutableString string];
		
		for(i = 0; i < 20; i++) {
			[result appendFormat:@"%02x", bytes[i]];
		}
		
		close(file);
	}
	
	return result;
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_metadata release];
	[self->m_path release];
	
	[super dealloc];
}

@end
