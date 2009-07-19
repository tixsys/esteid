/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDAgent.h"
#import "EstEIDConfiguration.h"
#import "EstEIDInstaller.h"
#import "EstEIDManifestPackage.h"
#import "EstEIDPackage.h"
#import "NSThread+Additions.h"

//#define NO_OP

@implementation EstEIDInstaller

- (id)initWithPackages:(NSArray *)packages
{
	self = [super init];
	
	if(self) {
		self->m_packages = [packages retain];
	}
	
	return self;
}

- (NSArray *)packages
{
	return self->m_packages;
}

- (BOOL)restart
{
	NSEnumerator *enumerator = [self->m_packages objectEnumerator];
	EstEIDPackage *package;
	
	while((package = [enumerator nextObject]) != nil) {
		if([[package metadata] restart]) {
			return YES;
		}
	}
	
	return NO;
}

- (id <EstEIDInstallerDelegate>)delegate
{
	return self->m_delegate;
}

- (void)setDelegate:(id <EstEIDInstallerDelegate>)delegate
{
	self->m_delegate = delegate;
}

- (void)notifyProgress:(EstEIDPackage *)package
{
	[self->m_delegate installer:self progress:package];
}

- (void)notifyResult:(NSError *)error
{
	if(!error) {
		[self->m_delegate installerDidFinishInstalling:self];
	} else {
		[self->m_delegate installer:self failedToInstallWithError:error];
	}
}

#pragma mark EstEIDAsynchronousTask

- (void)main
{
	NSEnumerator *enumerator = [self->m_packages objectEnumerator];
#ifndef NO_OP
	NSString *targetVolume = [EstEIDConfiguration targetVolume];
#endif
	
	EstEIDAgent *agent = [EstEIDAgent sharedAgent];
	EstEIDPackage *package;
	int count = 0;
	int code = 0;
	
NS_DURING
	[[agent launchWithArguments:@"--preflight", nil] waitUntilExit];
	
	while([self isRunning] && (package = [enumerator nextObject]) != nil) {
#ifndef NO_OP
		NSTask *task = [agent launchWithArguments:@"--install", [[package metadata] sha1], [package path], targetVolume, nil];
		
		[task waitUntilExit];
		
		if(task && [task terminationStatus] == 0) {
			NSLog(@"%@: %@ installation was successful!", NSStringFromClass([self class]), [[package metadata] identifier]);
			count += 1;
		} else if([task terminationStatus] == -10) {
			NSLog(@"%@: %@ uncompression error", [[package metadata] identifier]);
			code = EstEIDPackageUncompressionError;
			break;
		} else {
			NSLog(@"%@: %@ installation failed!", NSStringFromClass([self class]), [[package metadata] identifier]);
			code = EstEIDPackageInstallError;
			break;
		}
#else
		[NSThread sleepForTimeInterval:3.0F];
		NSLog(@"%@: %@ installation simulation was successful!", NSStringFromClass([self class]), [[package metadata] identifier]);
		count += 1;
#endif
	}
	
	[[agent launchWithArguments:@"--postflight", nil] waitUntilExit];
NS_HANDLER
	NSLog(@"%@: Installation failed because of a unexpected exception", NSStringFromClass([self class]));
NS_ENDHANDLER
	
	if([self isRunning]) {
		NSError *error = (count != [self->m_packages count]) ? [[NSError alloc] initWithDomain:EstEIDPackageErrorDomain code:code userInfo:nil] : nil;
		
		[self performSelectorOnMainThread:@selector(notifyResult:) withObject:error waitUntilDone:NO];
		[error release];
	}
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_packages release];
	
	[super dealloc];
}

@end
