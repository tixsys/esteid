/* Copyright (c) 2008 Janek Priimann */

#import <Carbon/Carbon.h>
#import <SecurityInterface/SFAuthorizationView.h>
#import "NSTask+Additions.h"
#import "EstEIDAgent.h"
#import "EstEIDPreferencePane.h"
#import "EstEIDReceipt.h"

@interface EstEIDAgent(Additions)

- (BOOL)autoUpdate;
- (void)setAutoUpdate:(BOOL)autoUpdate;
- (BOOL)idLogin;
- (void)setIdLogin:(BOOL)idLogin;

@end

@implementation EstEIDAgent(Additions)

- (NSString *)username
{
	char *user = getenv("USER");
	
	return (user) ? [NSString stringWithUTF8String:user] : nil;
}

- (NSString *)launchdAgentPath
{
	return [[[NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSLocalDomainMask, YES) objectAtIndex:0] stringByAppendingPathComponent:@"LaunchAgents"] stringByAppendingPathComponent:[NSString stringWithFormat:@"%s.plist", [EstEIDAgent identifier]]];
}

- (BOOL)autoUpdate
{
	NSString *path = [self launchdAgentPath];
	BOOL result = NO;
	
	if([[NSFileManager defaultManager] fileExistsAtPath:path]) {
		NSDictionary *plist = [[NSDictionary alloc] initWithContentsOfFile:path];
		
		if(plist) {
			id obj = [plist objectForKey:@"Disabled"];
			
			if([obj isKindOfClass:[NSNumber class]]) {
				result = ![(NSNumber *)obj boolValue];
			}
			
			[plist release];
		}
	}
	
	return result;
}

- (void)setAutoUpdate:(BOOL)autoUpdate
{
	[[self launchWithArguments:@"--launchd", (autoUpdate) ? @"enable" : @"disable", [self launchdAgentPath], nil] waitUntilExit];
}

- (BOOL)idLogin
{
	return ([[NSTask execute:@"/usr/sbin/sc_auth" withArguments:[NSArray arrayWithObjects:@"list", @"-u", [self username], nil]] length] > 0) ? YES : NO;
}

- (void)setIdLogin:(BOOL)idLogin
{
	NSMutableArray *hashes = [NSMutableArray array];
	NSEnumerator *enumerator;
	NSString *hash;
	
	if(idLogin) {
		while(YES) {
			enumerator = [[[NSTask execute:@"/usr/sbin/sc_auth" withArguments:[NSArray arrayWithObjects:@"hash", nil]] componentsSeparatedByString:@"\n"] objectEnumerator];
			
			while((hash = [enumerator nextObject]) != nil) {
				if([hash hasSuffix:@"Isikutuvastus"] && [hash length] > 53) {
					[hashes addObject:[NSString stringWithFormat:@";pubkeyhash;%@", [hash substringToIndex:40]]];
					break;
				}
			}
			
			if([hashes count] > 0) {
				break;
			} else {
				NSBundle *bundle = [NSBundle bundleForClass:[self class]];
				
				NSAlert *alert = [NSAlert alertWithMessageText:[bundle localizedStringForKey:@"PreferencePane.Alert.IdLogin.CardNotFound.Title" value:nil table:@"PreferencePane"]
												 defaultButton:[bundle localizedStringForKey:@"PreferencePane.Action.Retry" value:nil table:@"PreferencePane"]
											   alternateButton:[bundle localizedStringForKey:@"PreferencePane.Action.Cancel" value:nil table:@"PreferencePane"]
												   otherButton:nil
									 informativeTextWithFormat:[bundle localizedStringForKey:@"PreferencePane.Alert.IdLogin.CardNotFound.Message" value:nil table:@"PreferencePane"], nil];
				
				if([alert runModal] == NSAlertAlternateReturn) {
					return;
				}
			}
		}
	} else {
		enumerator = [[[NSTask execute:@"/usr/bin/dscl" withArguments:[NSArray arrayWithObjects:@".", @"-read", [NSString stringWithFormat:@"/Users/%@", [self username]], @"AuthenticationAuthority", nil]] componentsSeparatedByString:@" "] objectEnumerator];
		
		while((hash = [enumerator nextObject]) != nil) {
			if([hash hasPrefix:@";pubkeyhash;"]) {
				[hashes addObject:[hash stringByTrimmingCharactersInSet:[NSCharacterSet whitespaceAndNewlineCharacterSet]]];
			}
		}
	}
	
	enumerator = [hashes objectEnumerator];
	
	while((hash = [enumerator nextObject]) != nil) {
		[[self launchWithArguments:@"--idlogin", (idLogin) ? @"enable" : @"disable", [NSString stringWithFormat:@"/Users/%@", [self username]], hash, nil] waitUntilExit];
	}
}

@end

@implementation EstEIDPreferencePane

- (EstEIDAgent *)agent
{
	if(!self->m_agent) {
		self->m_agent = [[EstEIDAgent alloc] initWithPath:@"/Applications/Utilities/EstEIDSU.app/Contents/Resources/agent" state:(void *)[[self->m_authorizationView authorization] authorizationRef]];
	}
	
	return self->m_agent;
}

- (IBAction)help:(id)sender
{
	NSBundle *bundle = [NSBundle bundleForClass:[self class]];
	id site = [[bundle localizedInfoDictionary] objectForKey:@"EstEIDHelpWebsite"];
	
	if(!site) {
		site = [[bundle infoDictionary] objectForKey:@"EstEIDHelpWebsite"];
	}
	
	if(!([site isKindOfClass:[NSString class]] && [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:(NSString *)site]])) {
		NSLog(@"%@: Couldn't open help site!", NSStringFromClass([self class]));
		NSBeep();
	}
}

- (IBAction)idLogin:(id)sender
{
	if(sender == self->m_idLoginButton) {
		EstEIDAgent *agent = [self agent];
		
		if(agent) {
			if([agent isOwnedBySystem]) {
				[agent setIdLogin:([self->m_idLoginButton state] == NSOnState) ? YES : NO];
			} else {
				NSLog(@"%@: Agent is not owned by the system!", NSStringFromClass([self class]));
				NSBeep();
			}
			
			[self->m_idLoginButton setState:([agent idLogin]) ? NSOnState : NSOffState];
		} else {
			NSLog(@"%@: Couldn't find agent!", NSStringFromClass([self class]));
			NSBeep();
		}
	}
}

- (IBAction)update:(id)sender
{
	if(sender == self->m_automaticUpdateButton) {
		EstEIDAgent *agent = [self agent];
		
		if(agent) {
			if([agent isOwnedBySystem]) {
				[agent setAutoUpdate:([self->m_automaticUpdateButton state] == NSOnState) ? YES : NO];
			} else {
				NSLog(@"%@: Agent is not owned by the system!", NSStringFromClass([self class]));
				NSBeep();
			}
			
			[self->m_automaticUpdateButton setState:([agent autoUpdate]) ? NSOnState : NSOffState];
		} else {
			NSLog(@"%@: Couldn't find agent!", NSStringFromClass([self class]));
			NSBeep();
		}
	} else {
		system("/usr/bin/open -a /Applications/Utilities/EstEIDSU.app");
	}
}

#pragma mark NSPreferencePane

- (void)willSelect
{
	EstEIDAgent *agent = [self agent];
	
	[self->m_automaticUpdateButton setState:([agent autoUpdate]) ? NSOnState : NSOffState];
	
	if(![self->m_idLoginButton isHidden]) {
		[self->m_idLoginButton setState:([agent idLogin]) ? NSOnState : NSOffState];
	}
}

- (NSPreferencePaneUnselectReply)shouldUnselect
{
	return NSUnselectNow;
}

- (void)mainViewDidLoad
{
	NSEnumerator *enumerator = [[EstEIDReceipt receiptsWithPrefix:@"org.esteid"] objectEnumerator];
	NSBundle *bundle = [NSBundle bundleForClass:[self class]];
	NSMutableString *info = [NSMutableString string];
	SInt32 MacOSXVersionNumber;
	EstEIDReceipt *receipt;
	
	if(Gestalt(gestaltSystemVersion, &MacOSXVersionNumber) == noErr) {
		// Preference panes are narrower in Tiger and Leopard is too stupid to figure this out itself.
		if(MacOSXVersionNumber >= 0x1040 && MacOSXVersionNumber < 0x1050) {
			NSView *mainView = [self mainView];
			
			[mainView setFrameSize:NSMakeSize(594.0, [mainView frame].size.height)];
		}
		
		// Enable the id-card login option if the operating system 10.5.6 or later.
		if(MacOSXVersionNumber >= 0x1056) {
			[self->m_idLoginButton setHidden:NO];
		}
	}
	
	while((receipt = [enumerator nextObject]) != nil) {
		if(![[receipt identifier] hasPrefix:@"org.esteid.installer"]) {
			NSString *description = [receipt description];
			
			if(![description length]) description = [receipt title];
			if(![description length]) description = [receipt identifier];
			
			[info appendFormat:[bundle localizedStringForKey:@"PreferencePane.Label.Version" value:nil table:@"PreferencePane"], description, [receipt version]];
			[info appendString:@"\n"];
		}
	}
	
	[self->m_authorizationView setDelegate:self];
	[self->m_authorizationView setString:[EstEIDAgent identifier]];
	[self->m_authorizationView updateStatus:nil];
	[self->m_authorizationView setAutoupdate:YES];
	[self->m_manualUpdateButton setTitle:[bundle localizedStringForKey:@"PreferencePane.Action.CheckNow" value:nil table:@"PreferencePane"]];
	[self->m_automaticUpdateButton setTitle:[bundle localizedStringForKey:@"PreferencePane.Label.CheckForUpdates" value:nil table:@"PreferencePane"]];
	[self->m_automaticUpdateButton setEnabled:([self->m_authorizationView authorizationState] == SFAuthorizationViewUnlockedState) ? YES : NO];
	[self->m_idLoginButton setEnabled:([self->m_authorizationView authorizationState] == SFAuthorizationViewUnlockedState) ? YES : NO];
	[self->m_infoTextView setString:info];
	[self->m_infoTextField setStringValue:[bundle localizedStringForKey:([info length] > 0) ? @"PreferencePane.Label.InstalledSoftware" : @"PreferencePane.Label.NoInstalledSoftware" value:nil table:@"PreferencePane"]];
}

#pragma mark SFAuthorizationViewDelegate

- (void)authorizationViewCreatedAuthorization:(SFAuthorizationView *)view
{
	[self->m_agent release];
	self->m_agent = nil;
}

- (void)authorizationViewReleasedAuthorization:(SFAuthorizationView *)view
{
	[self->m_agent release];
	self->m_agent = nil;
}

- (void)authorizationViewDidAuthorize:(SFAuthorizationView *)view
{
	[self->m_automaticUpdateButton setEnabled:YES];
	[self->m_idLoginButton setEnabled:YES];
}

- (void)authorizationViewDidDeauthorize:(SFAuthorizationView *)view
{
	[self->m_automaticUpdateButton setEnabled:NO];
	[self->m_idLoginButton setEnabled:NO];
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_agent release];
	
    [super dealloc];
}

@end
