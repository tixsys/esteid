/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDAgent.h"
#import "EstEIDApplicationDelegate.h"
#import "EstEIDConfiguration.h"
#import "EstEIDInstaller.h"
#import "EstEIDManifest.h"
#import "EstEIDManifestComponent.h"
#import "EstEIDManifestDownload.h"
#import "EstEIDPackage.h"
#import "EstEIDPackageDownload.h"
#import "NSString+Additions.h"

BOOL EstEIDApplicationOptionSilent = NO;

static NSString *EstEIDApplicationDelegateImageIdentifier = @"image";
static NSString *EstEIDApplicationDelegateInstallIdentifier = @"install";
static NSString *EstEIDApplicationDelegateNameIdentifier = @"name";
static NSString *EstEIDApplicationDelegateVersionIdentifier = @"version";
static NSString *EstEIDApplicationDelegateSizeIdentifier = @"size";

@interface EstEIDApplicationDelegate(Private)

- (void)p_invalidateInstallButton;
- (void)p_terminateWithMessage:(NSString *)message title:(NSString *)title;

- (BOOL)p_startProgress:(NSString *)title limit:(double)limit timer:(BOOL)timer;
- (BOOL)p_stopProgress;

@end

@implementation EstEIDApplicationDelegate(Private)

- (void)p_invalidateInstallButton
{
	int count = [self->m_selectedComponents count];
	
	if(count == 0) {
		[self->m_installButton setTitle:[NSString stringWithFormat:NSLocalizedString(@"Window.Action.Install.Plural", nil), 0]];
		[self->m_installButton setEnabled:NO];
	} else {
		[self->m_installButton setTitle:[NSString stringWithFormat:(count > 1) ? NSLocalizedString(@"Window.Action.Install.Plural", nil) : NSLocalizedString(@"Window.Action.Install", nil), count]];
		[self->m_installButton setEnabled:YES];
	}
}

- (void)p_terminateWithMessage:(NSString *)message title:(NSString *)title
{
	NSAlert *alert = [NSAlert alertWithMessageText:title defaultButton:NSLocalizedString(@"Alert.Action.Quit", nil) alternateButton:nil otherButton:nil informativeTextWithFormat:message];
	
	[self p_stopProgress];
	[alert beginSheetModalForWindow:self->m_window modalDelegate:self didEndSelector:@selector(terminateAlertDidEnd: returnCode: contextInfo:) contextInfo:NULL];
}

- (BOOL)p_startProgress:(NSString *)title limit:(double)limit timer:(BOOL)timer
{
	NSWindow *sheet = [self->m_window attachedSheet];
	
	if(sheet != self->m_progressPanel) {
		if(sheet != nil) {
			[[NSApplication sharedApplication] endSheet:sheet];
			[sheet orderOut:nil];
		}
		
		[self->m_quitButton setHidden:YES];
		[self->m_progressTextField setStringValue:title];
		[self->m_progressIndicator setDoubleValue:0.0F];
		[self->m_progressIndicator setMaxValue:limit];
		
		[[NSApplication sharedApplication] beginSheet:self->m_progressPanel modalForWindow:self->m_window modalDelegate:self didEndSelector:@selector(progressSheetDidEnd: returnCode: contextInfo:) contextInfo:NULL];
		[self->m_progressPanel orderFront:nil];
		
		[self->m_timer invalidate];
		[self->m_timer release];
		
		self->m_timeInterval = [NSDate timeIntervalSinceReferenceDate];
		self->m_timer = (timer) ? [[NSTimer scheduledTimerWithTimeInterval:1.0 target:self selector:@selector(progress:) userInfo:nil repeats:YES] retain] : nil;
	}
	
	return YES;
}

- (BOOL)p_stopProgress
{
	if(self->m_timer) {
		[self->m_timer invalidate];
		[self->m_timer release];
		
		self->m_timer = nil;
	}
	
	[self->m_progressIndicator setIndeterminate:NO];
	[self->m_progressIndicator stopAnimation:nil];
	
	if(self->m_progressPanel == [self->m_window attachedSheet]) {
		[[NSApplication sharedApplication] endSheet:self->m_progressPanel];
		[self->m_progressPanel orderOut:nil];
		
		if([[self->m_manifestDownload components] count] > 0) {
			[self->m_quitButton setHidden:NO];
		}
	}
	
	return YES;
}

@end

@implementation EstEIDApplicationDelegate

+ (EstEIDApplicationDelegate *)sharedApplicationDelegate
{
	return (EstEIDApplicationDelegate *)[[NSApplication sharedApplication] delegate];
}

- (BOOL)isSilent
{
	return EstEIDApplicationOptionSilent;
}

- (IBAction)cancel:(id)sender
{
	[self->m_manifestDownload stop];
	[self->m_packageDownload stop];
	[self->m_installer stop];
	[self p_stopProgress];
	[[EstEIDAgent sharedAgent] deauthorize];
	[self->m_window orderOut:nil];
}

- (IBAction)install:(id)sender
{
	if(!self->m_packageDownload && [[EstEIDAgent sharedAgent] authorize]) {
		NSEnumerator *enumerator = [[self->m_manifestDownload components] objectEnumerator];
		NSMutableSet *packages = [NSMutableSet set];
		EstEIDManifestComponent *component;
		
		while((component = [enumerator nextObject]) != nil) {
			if([self->m_selectedComponents objectForKey:component]) {
				[packages addObjectsFromArray:[component packages]];
			}
		}
		
		if([packages count] > 0 && [self p_startProgress:NSLocalizedString(@"Progress.Label.Downloading", nil) limit:100.0F timer:NO]) {
			self->m_packageDownload = [[EstEIDPackageDownload alloc] initWithManifestPackages:[packages allObjects] location:[self->m_manifestDownload location] prefix:nil];
			[self->m_packageDownload setDelegate:(id <EstEIDPackageDownloadDelegate>)self];
			[self->m_packageDownload start];
		} else {
			NSBeep();
		}
	}
}

- (IBAction)quit:(id)sender
{
	[self->m_installer release];
	self->m_installer = nil;
	
	[self->m_packageDownload release];
	self->m_packageDownload = nil;
	
	[self->m_manifestDownload release];
	self->m_manifestDownload = nil;
	
	if([self->m_window isVisible]) {
		[self->m_window orderOut:nil];
	} else {
		[[NSApplication sharedApplication] terminate:nil];
	}
}

- (IBAction)showWindow:(id)sender
{
	// Localize GUI here, because localized IB files suck.
	[[[self->m_tableView tableColumnWithIdentifier:EstEIDApplicationDelegateInstallIdentifier] headerCell] setStringValue:NSLocalizedString(@"Window.Label.Install", nil)];
	[[[self->m_tableView tableColumnWithIdentifier:EstEIDApplicationDelegateNameIdentifier] headerCell] setStringValue:NSLocalizedString(@"Window.Label.Name", nil)];
	[[[self->m_tableView tableColumnWithIdentifier:EstEIDApplicationDelegateVersionIdentifier] headerCell] setStringValue:NSLocalizedString(@"Window.Label.Version", nil)];
	[[[self->m_tableView tableColumnWithIdentifier:EstEIDApplicationDelegateSizeIdentifier] headerCell] setStringValue:NSLocalizedString(@"Window.Label.Size", nil)];
	[self->m_quitButton setTitle:NSLocalizedString(@"Window.Action.Quit", nil)];
	[self->m_quitButton setHidden:YES];
	[self->m_hintTextField setStringValue:NSLocalizedString(@"Window.Label.RestartRequired", nil)];
	[self->m_window setTitle:NSLocalizedString(@"Window.Title", nil)];
	[self->m_hintImageView setHidden:YES];
	[self->m_hintTextField setHidden:YES];
	[self->m_progressButton setTitle:NSLocalizedString(@"Progress.Action.Cancel", nil)];
	[self->m_window setLevel:NSFloatingWindowLevel];
	[self p_invalidateInstallButton];
	
	[[NSApplication sharedApplication] activateIgnoringOtherApps:YES];
	[self->m_window orderFront:nil];
}

- (void)progress:(NSTimer *)timer
{
	if(self->m_manifestDownload) {
		NSTimeInterval delta = [NSDate timeIntervalSinceReferenceDate] - self->m_timeInterval;
		
		[self->m_progressIndicator setDoubleValue:delta];
	}
}

- (void)installAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	if([[alert buttons] count] > 1 && returnCode == NSAlertDefaultReturn) {
		NSAppleScript *script = [[NSAppleScript alloc] initWithSource:@"tell app \"Finder\"\nrestart\nend tell"];
		
		[script executeAndReturnError:nil];
		[script release];
	}
	
	[self quit:nil];
}

- (void)progressSheetDidEnd:(NSWindow *)sheet returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
}

- (void)terminateAlertDidEnd:(NSAlert *)alert returnCode:(int)returnCode contextInfo:(void *)contextInfo
{
	[self quit:nil];
}

#pragma mark EstEIDManifestDownloadDelegate

- (void)manifestDownloadDidFinishLoading:(EstEIDManifestDownload *)download
{
	EstEIDConfiguration *configuration = [EstEIDConfiguration sharedConfiguration];
	EstEIDManifest *manifest = [download manifest];
	BOOL fresh = (![[configuration state] isEqualToString:[manifest state]]) ? YES : NO;
	EstEIDManifestComponent *component;
	NSEnumerator *enumerator;
	
	self->m_selectedComponents = [[NSMutableDictionary alloc] init];
	[configuration setState:[manifest state]];
	[self->m_tableView reloadData];
	
	enumerator = [[self->m_manifestDownload components] objectEnumerator];
	
	while((component = [enumerator nextObject]) != nil) {
		if([component priority] != EstEIDManifestComponentPriorityLow) {
			[self->m_selectedComponents setObject:@"1" forKey:component];
		}
	}
	
	[self p_invalidateInstallButton];
	
	// --silent option, only show GUI if there are fresh components available.
	if([self isSilent]) {
		if(fresh && [[self->m_manifestDownload components] count] > 0) {
			[self showWindow:nil];
		} else {
			[self quit:nil];
		}
	} else {
		[self p_stopProgress];
		
		// No updates available!
		if([[self->m_manifestDownload components] count] == 0) {
			NSAlert *alert = [NSAlert alertWithMessageText:NSLocalizedString(@"Alert.Installer.SoftwareUpToDate.Title", nil) defaultButton:NSLocalizedString(@"Alert.Action.Quit", nil) alternateButton:nil otherButton:nil informativeTextWithFormat:NSLocalizedString(@"Alert.Installer.SoftwareUpToDate.Message", nil)];
			
			[alert beginSheetModalForWindow:self->m_window modalDelegate:self didEndSelector:@selector(terminateAlertDidEnd: returnCode: contextInfo:) contextInfo:NULL];
		}
	}
}

- (void)manifestDownload:(EstEIDManifestDownload *)download failedToLoadWithError:(NSError *)error
{
	if([self isSilent]) {
		NSLog(@"Failed to download manifest with error '%@'", [error description]);
		[self quit:nil];
	} else {
		if([[error domain] isEqualToString:EstEIDManifestErrorDomain]) {
			switch([error code]) {
				case EstEIDManifestFetchError:
					[self p_terminateWithMessage:NSLocalizedString(@"Alert.Manifest.FetchError.Message", nil) title:NSLocalizedString(@"Alert.Manifest.FetchError.Title", nil)];
					break;
				case EstEIDManifestParseError:
					[self p_terminateWithMessage:NSLocalizedString(@"Alert.Manifest.ParseError.Message", nil) title:NSLocalizedString(@"Alert.Manifest.ParseError.Title", nil)];
					break;
				case EstEIDManifestVerifyError:
					[self p_terminateWithMessage:NSLocalizedString(@"Alert.Manifest.VerifyError.Message", nil) title:NSLocalizedString(@"Alert.Manifest.VerifyError.Title", nil)];
					break;
				default:
					[self p_terminateWithMessage:NSLocalizedString(@"Alert.Manifest.GenericError.Message", nil) title:NSLocalizedString(@"Alert.Manifest.GenericError.Title", nil)];
					break;
			}
		} else {
			[self p_terminateWithMessage:NSLocalizedString(@"Alert.Manifest.GenericError.Message", nil) title:NSLocalizedString(@"Alert.Manifest.GenericError.Title", nil)];
		}
	}
}

#pragma mark EstEIDPackageDownloadDelegate

- (void)packageDownloadDidFinishLoading:(EstEIDPackageDownload *)download
{
	NSArray *packages = [download packages];
	
	[self->m_progressButton setEnabled:NO];
	[self->m_progressIndicator setDoubleValue:0.0F];
	[self->m_progressIndicator setMaxValue:[packages count]];
	[self->m_progressTextField setStringValue:NSLocalizedString(@"Progress.Label.Installing", nil)];
	
	self->m_installer = [[EstEIDInstaller alloc] initWithPackages:packages];
	[self->m_installer setDelegate:(id <EstEIDInstallerDelegate>)self];
	[self->m_installer start];
	
	[self->m_progressIndicator setIndeterminate:YES];
	[self->m_progressIndicator startAnimation:nil];
}

- (void)packageDownload:(EstEIDPackageDownload *)download progress:(long long)length
{
	if(!EstEIDApplicationOptionSilent) {
		int value = round((double)length / (double)[download length] * 100.0F);
		
		if(value > 100) {
			value = 100;
		}
		
		[self->m_progressIndicator setDoubleValue:(double)value];
		[self->m_progressTextField setStringValue:[NSString stringWithFormat:@"%@ (%d%%)", NSLocalizedString(@"Progress.Label.Downloading", nil), value]];
	}
}

- (void)packageDownload:(EstEIDPackageDownload *)download failedToLoadWithError:(NSError *)error
{
	if([[error domain] isEqualToString:EstEIDPackageErrorDomain]) {
		switch([error code]) {
			case EstEIDPackageFetchError:
				[self p_terminateWithMessage:NSLocalizedString(@"Alert.Package.FetchError.Message", nil) title:NSLocalizedString(@"Alert.Package.FetchError.Title", nil)];
				break;
			case EstEIDPackageLengthError:
				[self p_terminateWithMessage:NSLocalizedString(@"Alert.Package.LengthError.Message", nil) title:NSLocalizedString(@"Alert.Package.LengthError.Title", nil)];
				break;
			case EstEIDPackageVerifyError:
				[self p_terminateWithMessage:NSLocalizedString(@"Alert.Package.VerifyError.Message", nil) title:NSLocalizedString(@"Alert.Package.VerifyError.Title", nil)];
				break;
			case EstEIDPackageUncompressionError:
				[self p_terminateWithMessage:NSLocalizedString(@"Alert.Package.UncompressError.Message", nil) title:NSLocalizedString(@"Alert.Package.UncompressError.Title", nil)];
				break;
			case EstEIDPackageInstallError:
				[self p_terminateWithMessage:NSLocalizedString(@"Alert.Package.InstallError.Message", nil) title:NSLocalizedString(@"Alert.Package.InstallError.Title", nil)];
				break;
			default:
				[self p_terminateWithMessage:NSLocalizedString(@"Alert.Package.GenericError.Message", nil) title:NSLocalizedString(@"Alert.Package.GenericError.Title", nil)];
				break;
		}
	} else {
		[self p_terminateWithMessage:NSLocalizedString(@"Alert.Package.GenericError.Message", nil) title:NSLocalizedString(@"Alert.Package.GenericError.Title", nil)];
	}
}
					  
#pragma mark EstEIDInstallerDelegate

- (void)installerDidFinishInstalling:(EstEIDInstaller *)installer
{
	if([self p_stopProgress]) {
		BOOL restart = [installer restart];
		NSAlert *alert = [NSAlert alertWithMessageText:NSLocalizedString(@"Alert.Installer.Successful.Title", nil) defaultButton:NSLocalizedString((restart) ? @"Alert.Action.Restart" : @"Alert.Action.Quit", nil) alternateButton:(restart) ? NSLocalizedString(@"Alert.Action.RestartLater", nil) : nil otherButton:nil informativeTextWithFormat:NSLocalizedString((restart) ? @"Alert.Installer.SuccessfulAndRestart.Message" : @"Alert.Installer.Successful.Message", nil)];
		
		[self->m_packageDownload release];
		self->m_packageDownload = nil;
		
		[alert beginSheetModalForWindow:self->m_window modalDelegate:self didEndSelector:@selector(installAlertDidEnd: returnCode: contextInfo:) contextInfo:NULL];
	}
}

- (void)installer:(EstEIDInstaller *)installer progress:(EstEIDPackage *)package
{
	NSLog(@"Installed '%@' v", [[package metadata] identifier], [[package metadata] version]);
}

- (void)installer:(EstEIDInstaller *)installer failedToInstallWithError:(NSError *)error
{
	[self packageDownload:self->m_packageDownload failedToLoadWithError:error];
}

#pragma mark NSApplication+Delegate

- (void)applicationDidFinishLaunching:(NSNotification *)notification
{
	if(![self isSilent]) {
		[self showWindow:nil];
		[self p_startProgress:NSLocalizedString(@"Progress.Label.CheckingForUpdates", nil) limit:30.0F * [[[EstEIDConfiguration sharedConfiguration] webServices] count] + 1.0F timer:YES];
	}
	
	self->m_manifestDownload = [[EstEIDManifestDownload alloc] initWithLocations:[[EstEIDConfiguration sharedConfiguration] webServices]];
	[self->m_manifestDownload setDelegate:(id <EstEIDManifestDownloadDelegate>)self];
	[self->m_manifestDownload start];
}

- (BOOL)applicationShouldTerminateAfterLastWindowClosed:(NSApplication *)application
{
	return YES;
}

#pragma mark NSSplitView+Delegate

- (float)splitView:(NSSplitView *)splitView constrainSplitPosition:(float)proposedPosition ofSubviewAt:(int)offset
{
	return (proposedPosition < 76.0F) ? 76.0F : proposedPosition;
}

#pragma mark NSTableDataSource

- (int)numberOfRowsInTableView:(NSTableView *)tableView
{
	return [[self->m_manifestDownload components] count];
}

- (void)tableView:(NSTableView *)tableView willDisplayCell:(id)cell forTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	if([[tableColumn identifier] isEqualToString:EstEIDApplicationDelegateInstallIdentifier]) {
		EstEIDManifestComponent *component = [[self->m_manifestDownload components] objectAtIndex:rowIndex];
		
		[cell setEnabled:([component priority] != EstEIDManifestComponentPriorityCritical) ? YES : NO];
	}
}

- (id)tableView:(NSTableView *)tableView objectValueForTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	EstEIDManifestComponent *component = [[self->m_manifestDownload components] objectAtIndex:rowIndex];
	NSString *identifier = [tableColumn identifier];
	
	if([identifier isEqualToString:EstEIDApplicationDelegateImageIdentifier]) {
		return ([component restart]) ? [NSImage imageNamed:@"Restart.png"] : nil;
	} else if([identifier isEqualToString:EstEIDApplicationDelegateInstallIdentifier]) {
		return [NSNumber numberWithBool:([self->m_selectedComponents objectForKey:component]) ? YES : NO];
	} else if([identifier isEqualToString:EstEIDApplicationDelegateNameIdentifier]) {
		return [component title];
	} else if([identifier isEqualToString:EstEIDApplicationDelegateVersionIdentifier]) {
		return [component version];
	} else if([identifier isEqualToString:EstEIDApplicationDelegateSizeIdentifier]) {
		return [NSString stringWithFileSize:[component length]];
	}
	
	return nil;
}

- (void)tableView:(NSTableView *)tableView setObjectValue:(id)anObject forTableColumn:(NSTableColumn *)tableColumn row:(int)rowIndex
{
	if([[tableColumn identifier] isEqualToString:EstEIDApplicationDelegateInstallIdentifier]) {
		EstEIDManifestComponent *component = [[self->m_manifestDownload components] objectAtIndex:rowIndex];
		
		if([component priority] != EstEIDManifestComponentPriorityCritical) {
			if([anObject boolValue]) {
				[self->m_selectedComponents setObject:@"1" forKey:component];
			} else {
				[self->m_selectedComponents removeObjectForKey:component];
			}
			
			[self p_invalidateInstallButton];
		}
	}
}

#pragma mark NSTableView+Delegate

- (void)tableViewSelectionDidChange:(NSNotification *)notification
{
	int index = [self->m_tableView selectedRow];
	BOOL hideHint = YES;
	
	if(index != NSNotFound) {
		EstEIDManifestComponent *component = [[self->m_manifestDownload components] objectAtIndex:index];
		NSMutableString *str = [NSMutableString string];
		
		if([component restart]) {
			hideHint = NO;
		}
		
		[str appendString:@"<html><body style=\"font-family: 'Lucida Grande'; font-size: smaller;\">"];
		[str appendString:[component description]];
		[str appendString:@"</body></html>"];
		
		[[self->m_textView textStorage] setAttributedString:[[[NSAttributedString alloc] initWithHTML:[str dataUsingEncoding:NSUTF8StringEncoding] documentAttributes:nil] autorelease]];
	} else {
		[self->m_textView setString:@""];
	}
	
	[self->m_hintImageView setHidden:hideHint];
	[self->m_hintTextField setHidden:hideHint];
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_installer release];
	[self->m_manifestDownload release];
	[self->m_packageDownload release];
	[self->m_selectedComponents release];
	[self->m_timer invalidate];
	[self->m_timer release];
	
	[super dealloc];
}

@end
