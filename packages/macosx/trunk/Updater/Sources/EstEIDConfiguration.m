/* Copyright (c) 2009 Janek Priimann */

#import <sys/sysctl.h>
#import "EstEIDConfiguration.h"
#import "EstEIDManifestComponent.h"
#import "EstEIDReceipt.h"
#import "NSBundle+Additions.h"
#import "NSString+Additions.h"

static NSString *EstEIDConfigurationStateKey = @"State";

@implementation EstEIDConfiguration

+ (EstEIDConfiguration *)sharedConfiguration
{
	static EstEIDConfiguration *sharedConfiguration = nil;
	
	if(!sharedConfiguration) {
		@synchronized(self) {
			if(!sharedConfiguration) {
				sharedConfiguration = [[EstEIDConfiguration alloc] init];
			}
		}
	}
	
	return sharedConfiguration;
}

+ (NSString *)targetVolume
{
	NSString *target = [[NSBundle bundleForClass:self] stringForKey:@"EstEIDTargetVolume"];
	
	return ([target length] > 0) ? target : @"/";
}

- (NSString *)architecture
{
	return self->m_architecture;
}

- (NSString *)operatingSystem
{
	return self->m_operatingSystem;
}

- (NSDictionary *)packages
{
	return self->m_packages;
}

- (NSString *)publicKey
{
	if(!self->m_publicKey) {
		NSString *path = [[NSBundle bundleForClass:[self class]] pathForResource:@"Manifest" ofType:@"pem"];
		
		if(path) {
			@synchronized(self) {
				if(!self->m_publicKey) {
					self->m_publicKey = [[NSString alloc] initWithContentsOfFile:path encoding:NSUTF8StringEncoding error:nil];
				}
			}
		}
	}
	
	return self->m_publicKey;
}

- (NSString *)version
{
	return self->m_version;
}

- (NSArray *)webServices
{
	return self->m_webServices;
}

- (NSString *)state
{
	return self->m_state;
}

- (void)setState:(NSString *)state
{
	if(self->m_state != state) {
		[self->m_state release];
		self->m_state = [state retain];
		
		[[NSUserDefaults standardUserDefaults] setObject:self->m_state forKey:EstEIDConfigurationStateKey];
	}
}

#pragma mark NSObject

- (id)init
{
	self = [super init];
	
	if(self) {
		NSEnumerator *enumerator = [[EstEIDReceipt receipts] objectEnumerator];
		NSBundle *bundle = [NSBundle bundleForClass:[self class]];
		unsigned long length = sizeof(int);
		SInt32 major, minor, bugfix;
		EstEIDReceipt *receipt;
		BOOL ppc = NO;
		BOOL b64 = NO;
		int value;
		
		self->m_packages = [[NSMutableDictionary alloc] init];
		
		while((receipt = [enumerator nextObject]) != nil) {
			[(NSMutableDictionary *)self->m_packages setObject:[receipt version] forKey:[receipt identifier]];
		}
		
		self->m_version = [[bundle stringForKey:@"CFBundleVersion"] retain];
		self->m_operatingSystem = (Gestalt(gestaltSystemVersionMajor, &major) == noErr && Gestalt(gestaltSystemVersionMinor, &minor) == noErr && Gestalt(gestaltSystemVersionBugFix, &bugfix) == noErr) ? [[NSString alloc] initWithFormat:@"%d.%d.%d", major, minor, bugfix] : [@"10.3" retain];
		self->m_webServices = [[bundle arrayForKey:@"EstEIDWebService"] retain];
		
		// Values are defined in <sys/mach/machine.h>
		if(sysctlbyname("hw.cputype", &value, &length, NULL, 0) == 0 && value == 18) {
			ppc = YES;
		}
		
		if(sysctlbyname("hw.cpu64bit_capable", &value, &length, NULL, 0) == 0 ||
		   sysctlbyname("hw.optional.x86_64", &value, &length, NULL, 0) == 0 ||
		   sysctlbyname("hw.optional.64bitops", &value, &length, NULL, 0) == 0 && value == 1) {
			b64 = YES;
		}
		
		if(ppc) {
			self->m_architecture = [(b64) ? @"ppc ppc64" : @"ppc ppc32" retain];
		} else {
			self->m_architecture = [(b64) ? @"i386 x86_64" : @"i386 x86_32" retain];
		}
		
		self->m_state = [[[NSUserDefaults standardUserDefaults] stringForKey:EstEIDConfigurationStateKey] retain];
	}
	
	return self;
}

- (void)dealloc
{
	[self->m_architecture release];
	[self->m_operatingSystem release];
	[self->m_packages release];
	[self->m_publicKey release];
	[self->m_version release];
	[self->m_webServices release];
	[self->m_state release];
	
	[super dealloc];
}

@end
