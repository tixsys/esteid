/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDManifestPackage.h"
#import "EstEIDPackage.h"
#import "EstEIDPackageDownload.h"
#import "NSThread+Additions.h"

@implementation EstEIDPackageDownload

- (id)initWithManifestPackages:(NSArray *)packages
{
	return [self initWithManifestPackages:packages location:nil prefix:nil];
}

- (id)initWithManifestPackages:(NSArray *)packages location:(NSString *)location prefix:(NSString *)prefix
{
	self = [super init];
	
	if(self) {
		NSEnumerator *enumerator = [packages objectEnumerator];
		EstEIDManifestPackage *package;
		
		self->m_baseURL = (location) ? [[NSURL alloc] initWithString:location] : nil;
		self->m_length = 0;
		self->m_locator.x = 0;
		self->m_locator.y = 0;
		self->m_inPackages = [packages retain];
		self->m_outPackages = nil;
		self->m_path = [[NSTemporaryDirectory() stringByAppendingPathComponent:(prefix) ? prefix : @"esteidsu"] retain];
		
		while((package = [enumerator nextObject]) != nil) {
			self->m_length += [package length];
		}
		
		[[NSFileManager defaultManager] createDirectoryAtPath:self->m_path attributes:nil];
	}
	
	return self;
}

- (NSURL *)baseURL
{
	return self->m_baseURL;
}

- (id <EstEIDPackageDownloadDelegate>)delegate
{
	return self->m_delegate;
}

- (void)setDelegate:(id <EstEIDPackageDownloadDelegate>)delegate
{
	self->m_delegate = delegate;
}

- (long long)length
{
	return self->m_length;
}

- (NSArray *)packages
{
	return self->m_outPackages;
}

- (NSString *)path
{
	return self->m_path;
}

- (void)download:(NSString *)location
{
	if(!self->m_download) {
		NSURL *url = [[NSURL alloc] initWithString:location relativeToURL:self->m_baseURL];
		NSURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:30.0F];
		
		self->m_download = [[NSURLDownload alloc] initWithRequest:request delegate:self];
		
		[request release];
		[url release];
	}
}

- (void)notifyProgress:(EstEIDPackage *)package
{
	[self->m_delegate packageDownload:self progress:self->m_complete];
}

- (void)notifyResult:(NSError *)error
{
	if(!error) {
		[self->m_delegate packageDownloadDidFinishLoading:self];
	} else {
		[self->m_delegate packageDownload:self failedToLoadWithError:error];
	}
}

#pragma mark EstEIDAsynchronousTask

- (void)main
{
	NSMutableDictionary *packages = [[NSMutableDictionary alloc] init];
	EstEIDManifestPackage *metadata = nil;
	NSString *location = nil;
	BOOL success = NO;
	int code = 0;
	
	self->m_result = NO;
	
	while([self isRunning] && code == 0) {
		if(self->m_result) {
			if(self->m_tmp && location && metadata) {
				EstEIDPackage *package = [[EstEIDPackage alloc] initWithContentsOfFile:self->m_tmp metadata:metadata];
				
				[packages setObject:package forKey:location];
				
#ifdef DEBUG
				NSLog(@"%@: Downloaded %@ package to %@", NSStringFromClass([self class]), [metadata identifier], self->m_tmp);
#endif
				self->m_locator.x += 1;
				
				[package release];
				[self->m_tmp release];
				self->m_tmp = nil;
			} else {
				NSLog(@"%@ fetch error. Failed to download from '%@'", [metadata identifier], location);
			}
			
			[self->m_download release];
			self->m_download = nil;
			self->m_result = NO;
		}
		
		if(!self->m_download && code == 0) {
			if(self->m_locator.x < [self->m_inPackages count]) {
				metadata = [self->m_inPackages objectAtIndex:self->m_locator.x];
				
				if(self->m_locator.y < [[metadata locations] count]) {
					location = [[metadata locations] objectAtIndex:self->m_locator.y++];
					
					if(![packages objectForKey:location]) {
						[self performSelectorOnMainThread:@selector(download:) withObject:location waitUntilDone:YES];
					} else {
						self->m_complete += [metadata length];
					}
				} else {
					self->m_locator.x += 1;
					self->m_locator.y = 0;
					NSLog(@"%@ fetch error. Out of available mirrors.", [metadata identifier]);
					code = EstEIDPackageFetchError;
				}
			} else {
				success = YES;
				break;
			}
		}
		
		[NSThread sleepForTimeInterval:0.1];
	}
	
	if([self isRunning]) {
		NSError *error;
		
		if(success) {
			NSArray *pkg = [packages allValues];
			NSEnumerator *enumerator = [pkg objectEnumerator];
			EstEIDPackage *package;
			
			while([self isRunning] && (package = [enumerator nextObject]) != nil && code == 0) {
				metadata = [package metadata];
				
				if([metadata length] != [package length]) {
					NSLog(@"%@ size mismatch. Was %lld, but expected %ldd", [metadata identifier], [package length], [metadata length]);
					code = EstEIDPackageLengthError;
				} else if(![[metadata sha1] isEqualToString:[package sha1]]) {
					NSLog(@"%@ checksum mismatch. Was %@, but expected %@", [metadata identifier], [package sha1], [metadata sha1]);
					code = EstEIDPackageVerifyError;
				}
			}
			
			if(code == 0) {
				[self->m_outPackages autorelease];
				self->m_outPackages = [pkg retain];
			}
		}
		
		if([self isRunning]) {
			error = (code != 0) ? [[NSError alloc] initWithDomain:EstEIDPackageErrorDomain code:code userInfo:nil] : nil;
			[self performSelectorOnMainThread:@selector(notifyResult:) withObject:error waitUntilDone:NO];
			[error release];
		}
	} else {
		[self->m_download cancel];
	}
	
	[packages release];
	[self->m_download release];
	self->m_download = nil;
}

#pragma mark NSURLDownload+Delegate

- (void)download:(NSURLDownload *)download decideDestinationWithSuggestedFilename:(NSString *)filename
{
    [download setDestination:[self->m_path stringByAppendingPathComponent:filename] allowOverwrite:NO];
}

- (void)download:(NSURLDownload *)download didCreateDestination:(NSString *)path
{
	[self->m_tmp release];
    self->m_tmp = [path retain];
}

- (void)downloadDidFinish:(NSURLDownload *)download
{
	self->m_result = YES;
}

- (void)download:(NSURLDownload *)download didReceiveDataOfLength:(int)length
{
	self->m_complete += length;
	
	if(self->m_delegate && self->m_complete - self->m_progress >= 4092) {
		self->m_progress = self->m_complete;
		
		[self performSelectorOnMainThread:@selector(notifyProgress:) withObject:nil waitUntilDone:YES];
	}
}

- (void)download:(NSURLDownload *)download didFailWithError:(NSError *)error
{
	[self->m_tmp release];
	self->m_tmp = nil;
	self->m_result = YES;
}

- (NSURLRequest *)download:(NSURLDownload *)download willSendRequest:(NSURLRequest *)request redirectResponse:(NSURLResponse *)redirectResponse
{
	return request;
}

#pragma mark NSObject

- (void)dealloc
{
	[self stop];
	
	[self->m_baseURL release];
	[self->m_inPackages release];
	[self->m_outPackages release];
	[self->m_download release];
	[[NSFileManager defaultManager] removeFileAtPath:self->m_path handler:nil];
	[self->m_path release];
	
	[super dealloc];
}

@end
