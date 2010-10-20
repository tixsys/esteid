/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDConfiguration.h"
#import "EstEIDManifest.h"
#import "EstEIDManifestComponent.h"
#import "EstEIDManifestDownload.h"
#import "NSThread+Additions.h"

@implementation EstEIDManifestDownload

- (id)initWithLocations:(NSArray *)locations
{
	self = [super init];
	
	if(self) {
		self->m_delegate = nil;
		self->m_manifest = nil;
		self->m_signature = nil;
		self->m_locations = [locations retain];
	}
	
	return self;
}

- (NSArray *)components
{
	if(!self->m_components && self->m_manifest) {
		self->m_components = [[self->m_manifest components] retain];
	}
	
	return self->m_components;
}

- (NSString *)location
{
	int locator = self->m_locator - 1;
	
	return (locator >= 0 && locator < [self->m_locations count]) ? [self->m_locations objectAtIndex:locator] : nil;
}

- (EstEIDManifest *)manifest
{
	return self->m_manifest;
}

- (NSData *)signature
{
	return self->m_signature;
}

- (id <EstEIDManifestDownloadDelegate>)delegate
{
	return self->m_delegate;
}

- (void)setDelegate:(id <EstEIDManifestDownloadDelegate>)delegate
{
	self->m_delegate = delegate;
}

- (void)download:(NSString *)location
{
	if(!self->m_download) {
		NSURL *url = [[NSURL alloc] initWithString:location];
		NSURLRequest *request = [[NSMutableURLRequest alloc] initWithURL:url cachePolicy:NSURLRequestReloadIgnoringCacheData timeoutInterval:30.0F];
		
		self->m_download = [[NSURLDownload alloc] initWithRequest:request delegate:self];
		
		[request release];
		[url release];
	}
}

- (void)notifyResult:(NSError *)error
{
	if(!error) {
		[self->m_delegate manifestDownloadDidFinishLoading:self];
	} else {
		[self->m_delegate manifestDownload:self failedToLoadWithError:error];
	}
}

#pragma mark EstEIDAsynchronousTask

- (void)main
{
	NSString *location = nil;
	int code = 0;
	
	self->m_result = NO;
	
	while([self isRunning] && code == 0) {
		if(self->m_result) {
			NSURL *download = nil;
			
			if(self->m_tmp) {
				NSData *data = [NSData dataWithContentsOfFile:self->m_tmp];
				
				if(code == 0) {
					// Signature
					if(self->m_manifest) {
						self->m_signature = [data retain];
						
						if(![self->m_manifest verifySignature:self->m_signature usingPublicKey:[[EstEIDConfiguration sharedConfiguration] publicKey]]) {
							code = EstEIDManifestVerifyError;
						}
					// Manifest
					} else {
						self->m_manifest = [[EstEIDManifest manifestFromData:data] retain];
					
						if(self->m_manifest) {
#ifndef DEBUG
							NSString *signature = [self->m_manifest signature];
							
							if([signature length]) {
								download = [[[NSURL alloc] initWithString:signature relativeToURL:[NSURL URLWithString:[self location]]] autorelease];
							} else {
								code = EstEIDManifestVerifyError;
							}
#endif
						} else {
							code = EstEIDManifestParseError;
						}
					}
				}
				
				[[NSFileManager defaultManager] removeFileAtPath:self->m_tmp handler:nil];
				[self->m_tmp release];
				self->m_tmp = nil;
					
				if(!download) {
					break;
				}
			} else {
				NSLog(@"Failed to download manifest from '%@'", location);
			}
			
			[self->m_download release];
			self->m_download = nil;
			self->m_result = NO;
			
			if(download) {
				[self performSelectorOnMainThread:@selector(download:) withObject:[download absoluteString] waitUntilDone:YES];
			}
		}
		
		if(!self->m_download && code == 0) {
			if(self->m_manifest) {
				[self->m_manifest release];
				self->m_manifest = nil;
			}
			
			if(self->m_locator < [self->m_locations count]) {
				EstEIDConfiguration *configuration = [EstEIDConfiguration sharedConfiguration];
				
				location = [NSString stringWithFormat:@"%@?v=%@&o=%@&a=%@",
									   [self->m_locations objectAtIndex:self->m_locator++],
									   [[configuration version] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
									   [[configuration architecture] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding],
									   [[configuration operatingSystem] stringByAddingPercentEscapesUsingEncoding:NSUTF8StringEncoding]];
				
				[self performSelectorOnMainThread:@selector(download:) withObject:location waitUntilDone:YES];
			} else {
				NSLog(@"Failed to download manifest. Out of available mirrors.");
				code = EstEIDManifestFetchError;
				break;
			}
		}
		
		[NSThread sleepForTimeInterval:0.1];
	}
	
	if([self isRunning]) {
		NSError *error;
		
		if([self isRunning]) {
			error = (code != 0) ? [[NSError alloc] initWithDomain:EstEIDManifestErrorDomain code:code userInfo:nil] : nil;
			[self performSelectorOnMainThread:@selector(notifyResult:) withObject:error waitUntilDone:NO];
			[error release];
		}
	} else {
		[self->m_download cancel];
	}
	
	[self->m_download release];
	self->m_download = nil;
}

#pragma mark NSURLDownload+Delegate

- (void)download:(NSURLDownload *)download decideDestinationWithSuggestedFilename:(NSString *)filename
{
    [download setDestination:[NSTemporaryDirectory() stringByAppendingPathComponent:filename] allowOverwrite:NO];
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
	[self->m_components release];
	[self->m_manifest release];
	[self->m_locations release];
	[self->m_download release];
	[self->m_signature release];
	
	[super dealloc];
}

@end
