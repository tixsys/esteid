/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDPINPanel.h"

static NSString *EstEIDPINPanelShowsDetailsKey = @"EstEIDPINPanelShowsDetails";

@implementation EstEIDPINPanel

- (NSWindow *)window
{
	return self->m_window;
}

- (id <EstEIDPINPanelDelegate>)delegate
{
	return self->m_delegate;
}

- (void)setDelegate:(id <EstEIDPINPanelDelegate>)delegate
{
	self->m_delegate = delegate;
}

- (BOOL)showsDetails
{
	return ([self->m_detailsBox isHidden] == NO) ? YES : NO;
}

- (void)setShowsDetails:(BOOL)flag animate:(BOOL)animate
{
	if([self showsDetails] != flag) {
		float height = [self->m_detailsBox frame].size.height;
		NSRect frame = [self->m_window frame];
		
		if([self->m_detailsBox isHidden]) {
			frame.origin.y -= height;
			frame.size.height += height;
			
			[self->m_window setFrame:frame display:animate animate:animate];
			[self->m_detailsBox setHidden:NO];
			[self->m_detailsButton setState:NSOnState];
		} else {
			frame.origin.y += height;
			frame.size.height -= height;
			
			[self->m_detailsBox setHidden:YES];
			[self->m_detailsButton setState:NSOffState];
			[self->m_window setFrame:frame display:animate animate:animate];
		}
		
		[[NSUserDefaults standardUserDefaults] setBool:flag forKey:EstEIDPINPanelShowsDetailsKey];
	}
}

- (void)beginSheetForWindow:(NSWindow *)window modalDelegate:(id)delegate didEndSelector:(SEL)selector contextInfo:(void *)info
{
	[[NSApplication sharedApplication] beginSheet:[self window] modalForWindow:window modalDelegate:delegate didEndSelector:selector contextInfo:info];
}

- (NSString *)hash
{
	return [self->m_hashTextField stringValue];
}

- (void)setHash:(NSString *)hash
{
	[self->m_hashTextField setStringValue:(hash) ? hash : @""];
}

- (NSString *)name
{
	return [self->m_nameTextField stringValue];
}

- (void)setName:(NSString *)name
{
	[self->m_nameTextField setStringValue:(name) ? name : @""];
}

- (NSString *)PIN
{
	return [self->m_pinTextField stringValue];
}

- (void)setPIN:(NSString *)PIN
{
	[self->m_pinTextField setStringValue:(PIN) ? PIN : @""];
}

- (NSString *)URL
{
	return [self->m_urlTextField stringValue];
}

- (void)setURL:(NSString *)URL
{
	[self->m_urlTextField setStringValue:(URL) ? URL : @""];
}

- (IBAction)cancel:(id)sender
{
	[[self window] orderOut:sender];
	[[NSApplication sharedApplication] endSheet:[self window] returnCode:NSRunAbortedResponse];
}

- (IBAction)ok:(id)sender
{
	if(![self->m_delegate respondsToSelector:@selector(pinPanelShouldEnd:)] || [self->m_delegate pinPanelShouldEnd:self]) {
		[[self window] orderOut:sender];
		[[NSApplication sharedApplication] endSheet:[self window] returnCode:NSRunStoppedResponse];
	}
}

- (IBAction)showHelp:(id)sender
{
	NSBundle *bundle = [NSBundle bundleForClass:[self class]];
	id site = [[bundle localizedInfoDictionary] objectForKey:@"EstEIDHelpWebsite"];
	
	if(!site) {
		site = [[bundle infoDictionary] objectForKey:@"EstEIDHelpWebsite"];
	}
	
	if(!([site isKindOfClass:[NSString class]] && [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:(NSString *)site]])) {
		NSLog(@"%@: Couldn't open help site (%@)!", NSStringFromClass([self class]), site);
		NSBeep();
	}
}

- (IBAction)showOptions:(id)sender
{
	// TODO: Download the document? Display a contextual menu?
}

- (IBAction)toggleDetails:(id)sender
{
	[self setShowsDetails:![self showsDetails] animate:YES];
}

#pragma mark NSWindowController

- (void)awakeFromNib
{
	NSBundle *bundle = [NSBundle bundleForClass:[self class]];
	
	[self->m_urlLabel setStringValue:[bundle localizedStringForKey:@"PINPanel.Label.URL" value:nil table:nil]];
	[self->m_hashLabel setStringValue:[bundle localizedStringForKey:@"PINPanel.Label.Hash" value:nil table:nil]];
	[self->m_nameLabel setStringValue:[bundle localizedStringForKey:@"PINPanel.Label.Name" value:nil table:nil]];
	[self->m_pinLabel setStringValue:[bundle localizedStringForKey:@"PINPanel.Label.PIN2" value:nil table:nil]];
	[self->m_messageLabel setStringValue:[bundle localizedStringForKey:@"PINPanel.Message.PIN2" value:nil table:nil]];
	[self->m_detailsLabel setStringValue:[bundle localizedStringForKey:@"PINPanel.Label.Details" value:nil table:nil]];
	[self->m_cancelButton setStringValue:[bundle localizedStringForKey:@"PINPanel.Action.Cancel" value:nil table:nil]];
	[self->m_okButton setStringValue:[bundle localizedStringForKey:@"PINPanel.Action.OK" value:nil table:nil]];
	
	[self setShowsDetails:[[NSUserDefaults standardUserDefaults] boolForKey:EstEIDPINPanelShowsDetailsKey] animate:NO];
}

#pragma mark NSObject

- (id)init
{
	self = [super init];
	
	if(self) {
		[NSBundle loadNibNamed:@"PINPanel" owner:self];
	}
	
	return self;
}

- (void)dealloc
{
	[self->m_window release];
	
	[super dealloc];
}

@end
