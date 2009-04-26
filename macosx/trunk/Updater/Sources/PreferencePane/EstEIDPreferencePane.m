/* Copyright (c) 2008 Janek Priimann */

#import <Carbon/Carbon.h>
#import <SecurityInterface/SFAuthorizationView.h>
#import "EstEIDAgent.h"
#import "EstEIDPreferencePane.h"
#import "EstEIDReceipt.h"

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

- (IBAction)update:(id)sender
{
	if(sender == self->m_automaticUpdateButton) {
		EstEIDAgent *agent = [self agent];
		
		if(agent) {
			if([agent isOwnedBySystem]) {
				[agent setAutoUpdate:([self->m_automaticUpdateButton state] == NSOnState) ? YES : NO];
			} else {
				[self->m_automaticUpdateButton setState:([agent autoUpdate]) ? NSOnState : NSOffState];
				NSLog(@"%@: Agent is not owned by system! Are permissions correct?", NSStringFromClass([self class]));
				NSBeep();
			}
		} else {
			NSLog(@"%@: Couldn't find agent!", NSStringFromClass([self class]));
			NSBeep();
		}
	} else {
		[[NSWorkspace sharedWorkspace] launchApplication:@"EstEIDSU"];
	}
}

#pragma mark NSPreferencePane

- (void)willSelect
{
	[self->m_automaticUpdateButton setState:([[self agent] autoUpdate]) ? NSOnState : NSOffState];
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
	
	// Preference panes are narrower in Tiger and Leopard is too stupid to figure this out itself.
	if(Gestalt(gestaltSystemVersion, &MacOSXVersionNumber) == noErr && MacOSXVersionNumber >= 0x1040 && MacOSXVersionNumber < 0x1050) {
		NSView *mainView = [self mainView];
		
		[mainView setFrameSize:NSMakeSize(594.0, [mainView frame].size.height)];
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
}

- (void)authorizationViewDidDeauthorize:(SFAuthorizationView *)view
{
	[self->m_automaticUpdateButton setEnabled:NO];
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_agent release];
	
    [super dealloc];
}

@end
