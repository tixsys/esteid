/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDKTApplicationDelegate.h"
#import "EstEIDKTCertificate.h"
#import "NSString+Additions.h"

static NSString *EstEIDKTApplicationDelegateWebsiteColumn = @"website";
static NSString *EstEIDKTApplicationLanguageKey = @"language";

@implementation EstEIDKTApplicationDelegate

+ (EstEIDKTApplicationDelegate *)sharedApplicationDelegate
{
	return (EstEIDKTApplicationDelegate *)[[NSApplication sharedApplication] delegate];
}

- (NSString *)pathForResource:(NSString *)resource ofType:(NSString *)type
{
	switch([self->m_languagePopUpButton selectedTag]) {
		case EstEIDKTLanguageEstonian:
			return [[NSBundle mainBundle] pathForResource:resource ofType:type inDirectory:nil forLocalization:@"et"];
		case EstEIDKTLanguageRussian:
			return [[NSBundle mainBundle] pathForResource:resource ofType:type inDirectory:nil forLocalization:@"ru"];
		case EstEIDKTLanguageEnglish:
			return [[NSBundle mainBundle] pathForResource:resource ofType:type inDirectory:nil forLocalization:@"en"];
	}
			
	return [[NSBundle mainBundle] pathForResource:resource ofType:type];
}

- (NSString *)stringForKey:(NSString *)key
{
	NSString *value = nil;
	
	switch([self->m_languagePopUpButton selectedTag]) {
		case EstEIDKTLanguageEstonian: {
			static NSDictionary *et = nil;
			
			if(!et) {
				et = [[NSDictionary alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"KeychainTool" ofType:@"strings" inDirectory:nil forLocalization:@"et"]];
			}
			
			value = [et objectForKey:key];
			} break;
		case EstEIDKTLanguageRussian: {
			static NSDictionary *ru = nil;
			
			if(!ru) {
				ru = [[NSDictionary alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"KeychainTool" ofType:@"strings" inDirectory:nil forLocalization:@"ru"]];
			}
			
			value = [ru objectForKey:key];
			} break;
		case EstEIDKTLanguageEnglish:
			{
				static NSDictionary *en = nil;
				
				if(!en) {
					en = [[NSDictionary alloc] initWithContentsOfFile:[[NSBundle mainBundle] pathForResource:@"KeychainTool" ofType:@"strings" inDirectory:nil forLocalization:@"en"]];
				}
				
				value = [en objectForKey:key];
			} break;
	}
	
	return (value) ? value : NSLocalizedStringFromTable(key, @"KeychainTool", nil);
}

- (IBAction)changeLanguage:(id)sender
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	[self->m_prevButton setTitle:[self stringForKey:@"KeychainTool.Action.Cancel"]];
	[self->m_nextButton setTitle:[self stringForKey:@"KeychainTool.Action.Next"]];
	[self->m_saveButton setTitle:[self stringForKey:@"KeychainTool.Action.Save"]];
	[self->m_window setTitle:[self stringForKey:@"KeychainTool.Title"]];
	[self->m_textView readRTFDFromFile:[self pathForResource:@"KeychainTool" ofType:@"rtf"]];
	
	[defaults setInteger:[self->m_languagePopUpButton selectedTag] forKey:EstEIDKTApplicationLanguageKey];
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
					[self->m_websites removeAllObjects];
					[self->m_websites addObjectsFromArray:[self->m_certificate websites]];
					[self->m_infoTextField setStringValue:[self->m_certificate CN]];
					[self->m_tabView selectLastTabViewItem:nil];
				} else {
					NSAlert *alert = [NSAlert alertWithMessageText:[self stringForKey:@"KeychainTool.Alert.CardNotFound.Title"]
													 defaultButton:[self stringForKey:@"KeychainTool.Action.Retry"]
												   alternateButton:[self stringForKey:@"KeychainTool.Action.Cancel"]
													   otherButton:nil
										 informativeTextWithFormat:[self stringForKey:@"KeychainTool.Alert.CardNotFound.Message"], nil];
					
					NSBeep();
					
					if([alert runModal] != NSAlertAlternateReturn) {
						retry = YES;
					}
				}
				break;
			case 1: {
				NSEnumerator *enumerator = [self->m_websites objectEnumerator];
				NSString *website;
				
				while((website = [enumerator nextObject]) != nil) {
					if(![NSURL URLWithString:website] || ![[website lowercaseString] hasPrefix:@"https://"]) {
						NSAlert *alert = [NSAlert alertWithMessageText:[self stringForKey:@"KeychainTool.Alert.InvalidURL.Title"]
														 defaultButton:[self stringForKey:@"KeychainTool.Action.Close"]
													   alternateButton:nil
														   otherButton:nil
											 informativeTextWithFormat:[NSString stringWithFormat:[self stringForKey:@"KeychainTool.Alert.InvalidURL.Message"], website], nil];
						
						NSBeep();
						[alert runModal];
						return;
					}
				}
				
				if([self->m_certificate setWebsites:self->m_websites]) {
					NSAlert *alert = [NSAlert alertWithMessageText:[self stringForKey:@"KeychainTool.Alert.SaveSuccess.Title"]
													 defaultButton:[self stringForKey:@"KeychainTool.Action.Close"]
												   alternateButton:nil
													   otherButton:nil
										 informativeTextWithFormat:[self stringForKey:@"KeychainTool.Alert.SaveSuccess.Message"], nil];
					
					[alert runModal];
					
					//[[NSApplication sharedApplication] terminate:nil];
				} else {
					NSAlert *alert = [NSAlert alertWithMessageText:[self stringForKey:@"KeychainTool.Alert.SaveFailure.Title"]
													 defaultButton:[self stringForKey:@"KeychainTool.Action.Close"]
												   alternateButton:nil
													   otherButton:nil
										 informativeTextWithFormat:[self stringForKey:@"KeychainTool.Alert.SaveFailure.Message"], nil];
					
					NSBeep();
					[alert runModal];
				}
				} break;
		}
	} while(retry);
}

#pragma mark NSTableDataSource

- (int)numberOfRowsInTableView:(NSTableView *)tableView
{
	return [self->m_websites count] + 1;
}

- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	if([[tableColumn identifier] isEqualToString:EstEIDKTApplicationDelegateWebsiteColumn]) {
		[cell setPlaceholderString:[self stringForKey:@"KeychainTool.Placeholder.Website"]];
	}
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	if([[tableColumn identifier] isEqualToString:EstEIDKTApplicationDelegateWebsiteColumn]) {
		if(rowIndex < [self->m_websites count]) {
			return [self->m_websites objectAtIndex:rowIndex];
		}
	}
	
	return @"";
}

- (void)tableView:(NSTableView *)tableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	if([[tableColumn identifier] isEqualToString:EstEIDKTApplicationDelegateWebsiteColumn]) {
		NSString *value = [anObject trimmedString];
		
		if(rowIndex < [self->m_websites count]) {
			if([value length] == 0) {
				[self->m_websites removeObjectAtIndex:rowIndex];
				[self->m_tableView reloadData];
			} else if(![self->m_websites containsObject:value]) {
				[self->m_websites replaceObjectAtIndex:rowIndex withObject:value];
				[self->m_tableView reloadData];
			}
		} else {
			if([value length] > 0 && ![self->m_websites containsObject:value]) {
				[self->m_websites addObject:value];
				[self->m_tableView reloadData];
			}
		}
	}
}

#pragma mark NSTableView+Delegate

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
}

- (void)tableViewOnDelete:(NSTableView *)tableView row:(int)rowIndex
{
	if(rowIndex < [self->m_websites count]) {
		[self->m_websites removeObjectAtIndex:rowIndex];
		[self->m_tableView reloadData];
	}
}

#pragma mark NSApplication+Delegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
	NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
	
	if([defaults objectForKey:EstEIDKTApplicationLanguageKey]) {
		[self->m_languagePopUpButton selectItemWithTag:[defaults integerForKey:EstEIDKTApplicationLanguageKey]];
	}
	
	[self changeLanguage:nil];
	[self->m_window orderFront:nil];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)application
{
	return YES;
}

#pragma mark NSObject

- (id)init
{
	self = [super init];
	
	if(self) {
		self->m_websites = [[NSMutableArray alloc] init];
	}
	
	return self;
}

- (void)dealloc
{
	[self->m_certificate release];
	[self->m_websites release];
	
	[super dealloc];
}

@end
