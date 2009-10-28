/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDKTApplicationDelegate.h"
#import "EstEIDKTCertificate.h"

@implementation EstEIDKTApplicationDelegate

+ (EstEIDKTApplicationDelegate *)sharedApplicationDelegate
{
	return (EstEIDKTApplicationDelegate *)[[NSApplication sharedApplication] delegate];
}

- (IBAction)changeLanguage:(id)sender
{
	switch([self->m_languagePopUpButton selectedTag]) {
		case EstEIDKTLanguageEstonian:
			
			break;
		case EstEIDKTLanguageRussian:
			
			break;
		//case EstEIDKTLanguageEnglish:
		//	break;
		default:
			[self->m_prevButton setTitle:NSLocalizedStringFromTable(@"KeychainTool.Action.Cancel", @"KeychainTool", nil)];
			[self->m_nextButton setTitle:NSLocalizedStringFromTable(@"KeychainTool.Action.Next", @"KeychainTool", nil)];
			[self->m_saveButton setTitle:NSLocalizedStringFromTable(@"KeychainTool.Action.Save", @"KeychainTool", nil)];
			[self->m_window setTitle:NSLocalizedStringFromTable(@"KeychainTool.Title", @"KeychainTool", nil)];
			[self->m_textView readRTFDFromFile:[[NSBundle mainBundle] pathForResource:@"KeychainTool" ofType:@"rtf"]];
			break;
	}
}

- (IBAction)prev:(id)sender
{
	if([self->m_tabView indexOfTabViewItem:[self->m_tabView selectedTabViewItem]] == 1) {
		if(self->m_certificate) {
			[self->m_certificate release];
			self->m_certificate = nil;
		}
		
		[self->m_tabView selectFirstTabViewItem:nil];
	}
}

- (IBAction)next:(id)sender
{
	BOOL retry = NO;
	
	do {
		retry = NO;
		
		switch([self->m_tabView indexOfTabViewItem:[self->m_tabView selectedTabViewItem]]) {
			case 0:
				self->m_certificate = [[EstEIDKTCertificate alloc] init];
				
				if(self->m_certificate) {
					[self->m_infoTextField setStringValue:[self->m_certificate CN]];
					[self->m_tabView selectLastTabViewItem:nil];
				} else {
					NSAlert *alert = [NSAlert alertWithMessageText:NSLocalizedStringFromTable(@"KeychainTool.Alert.CardNotFound.Title", @"KeychainTool", nil)
													 defaultButton:NSLocalizedStringFromTable(@"KeychainTool.Action.Retry", @"KeychainTool", nil)
												   alternateButton:NSLocalizedStringFromTable(@"KeychainTool.Action.Cancel", @"KeychainTool", nil)
													   otherButton:nil
										 informativeTextWithFormat:NSLocalizedStringFromTable(@"KeychainTool.Alert.CardNotFound.Message", @"KeychainTool", nil), nil];
					
					if([alert runModal] != NSAlertAlternateReturn) {
						retry = YES;
					}
				}
				break;
			case 1:
				// Launch agent tool
				break;
		}
	} while(retry);
}

#pragma mark NSTableDataSource

- (int)numberOfRowsInTableView:(NSTableView *)tableView
{
	return 0;
}

- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	return nil;
}

- (void)tableView:(NSTableView *)tableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	
}

#pragma mark NSTableView+Delegate

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
}

#pragma mark NSApplication+Delegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
	[self changeLanguage:nil];
	[self->m_window orderFront:nil];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)application
{
	return YES;
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_certificate release];
	
	[super dealloc];
}

@end
