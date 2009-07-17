/* Copyright (c) 2009 Janek Priimann */

#import <Security/Authorization.h>
#import <SecurityFoundation/SFAuthorization.h>
#import <sys/stat.h>
#import "EstEIDAgent.h"

#define EstEIDAgentIdentifier "org.esteid.updater"

@implementation EstEIDAgent

+ (EstEIDAgent *)sharedAgent
{
	static EstEIDAgent *sharedAgent = nil;
	
	if(!sharedAgent) {
		@synchronized(self) {
			if(!sharedAgent) {
				sharedAgent = [[EstEIDAgent alloc] initWithPath:[[NSBundle bundleForClass:self] pathForResource:@"agent" ofType:nil] state:nil];
			}
		}
	}
	
	return sharedAgent;
}

+ (const char *)identifier
{
	return EstEIDAgentIdentifier;
}

- (id)initWithPath:(NSString *)path state:(void *)state
{
	self = [super init];
	
	if(self) {
		self->m_ext = (state) ? YES : NO;
		self->m_path = [path retain];
		self->m_state = state;
	}
	
	return self;
}

- (BOOL)isOwnedBySystem
{
	struct stat st;
	
	return (stat([self->m_path UTF8String], &st) == 0 && st.st_uid == 0) ? YES : NO;
}

- (NSString *)path
{
	return self->m_path;
}

- (NSTask *)launchWithArguments:(NSString *)argument, ...
{
	NSTask *task = nil;
	
	@synchronized(self) {
		if((self->m_ext || self->m_level > 0) && self->m_state) {
			AuthorizationExternalForm authorizationExternalForm;
			OSStatus err;
			
			if((err = AuthorizationMakeExternalForm((AuthorizationRef)self->m_state, &authorizationExternalForm)) == errAuthorizationSuccess) {
				NSMutableArray *arguments = [NSMutableArray array];
				NSFileHandle *handle = nil;
				va_list args;
				
				va_start(args, argument);
				
				if(argument) {
					[arguments addObject:argument];
				}
				
				while((argument = va_arg(args, NSString *)) != nil) {
					[arguments addObject:argument];
				}
				
				va_end(args);
				
NS_DURING
				NSPipe *pipe = [NSPipe pipe];
				
				handle = [pipe fileHandleForWriting];
				task = [[[NSTask alloc] init] autorelease];
				
				[task setLaunchPath:self->m_path];
				[task setArguments:arguments];
				[task setStandardInput:pipe];
				[task launch];
				
				[handle writeData:[NSData dataWithBytes:&authorizationExternalForm length:sizeof(AuthorizationExternalForm)]];
NS_HANDLER
				[handle closeFile];
				task = nil;
NS_ENDHANDLER
			}
		}
	}
	
	return task;
}

- (BOOL)authorize
{
	@synchronized(self) {
		if(!self->m_ext && self->m_level == 0) {
			AuthorizationRights rights;
			AuthorizationItem items[1];
			OSStatus err;
			
			if(!self->m_state) {
				if((err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, (AuthorizationRef *)&(self->m_state))) != errAuthorizationSuccess) {
					return NO;
				}
			}
			
			items[0].name = EstEIDAgentIdentifier;
			items[0].value = 0;
			items[0].valueLength = 0;
			items[0].flags = 0;
			rights.count = 1;
			rights.items = items;
			
			if((err = AuthorizationCopyRights((AuthorizationRef)self->m_state, &rights, kAuthorizationEmptyEnvironment, kAuthorizationFlagExtendRights| kAuthorizationFlagInteractionAllowed, NULL)) != errAuthorizationSuccess) {
				return NO;
			}
		}
		
		self->m_level++;
	}
	
	return YES;
}

- (void)deauthorize
{
	@synchronized(self) {
		if(self->m_level > 0) {
			self->m_level--;
			
			if(!self->m_ext && self->m_level == 0 && self->m_state) {
				AuthorizationFree((AuthorizationRef)self->m_state, kAuthorizationFlagDestroyRights);
				self->m_state = NULL;
			}
		}
	}
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_path release];
	
	[super dealloc];
}

@end
