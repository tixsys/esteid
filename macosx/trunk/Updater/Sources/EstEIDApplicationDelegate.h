/* Copyright (c) 2009 Janek Priimann */

#import <Cocoa/Cocoa.h>

@class EstEIDInstaller, EstEIDManifestDownload, EstEIDPackageDownload;

/**
 * The EstEIDApplicationDelegate class is the main entry-point for the Updater Panel.
 */
@interface EstEIDApplicationDelegate : NSObject
{
	@private
	EstEIDInstaller *m_installer;
	EstEIDManifestDownload *m_manifestDownload;
	EstEIDPackageDownload *m_packageDownload;
	NSMutableDictionary *m_selectedComponents;
	NSTimer *m_timer;
	NSTimeInterval m_timeInterval;
	IBOutlet NSButton *m_progressButton;
	IBOutlet NSProgressIndicator *m_progressIndicator;
	IBOutlet NSPanel *m_progressPanel;
	IBOutlet NSTextField *m_progressTextField;
	IBOutlet NSWindow *m_window;
	IBOutlet NSTableView *m_tableView;
	IBOutlet NSTextView *m_textView;
	IBOutlet NSImageView *m_hintImageView;
	IBOutlet NSTextField *m_hintTextField;
	IBOutlet NSButton *m_installButton;
	IBOutlet NSButton *m_quitButton;
}

/**
 * @name Getting the Shared Application Delegate Instance
 */

/**
 * Returns the shared application delegate object.
 * @return The shared application delegate object.
 */
+ (EstEIDApplicationDelegate *)sharedApplicationDelegate;

/**
 * Returns YES if the application was launched with the --silent option; NO otherwise.
 * @return YES if the application was launched with the --silent option; NO otherwise.
 */
- (BOOL)isSilent;

/**
 * @name Actions
 */

/**
 * Cancels the process sheet if applicable.
 * @param sender Typically the object that invoked this method.
 */
- (IBAction)cancel:(id)sender;

/**
 * Starts the installation process if possible.
 * @param sender Typically the object that invoked this method.
 */
- (IBAction)install:(id)sender;

/**
 * Terminated the application gracefully. Deletes any temporary files.
 * @param sender Typically the object that invoked this method.
 */
- (IBAction)quit:(id)sender;

/**
 * Shows the panel.
 * @param sender Typically the object that invoked this method.
 */
- (IBAction)showWindow:(id)sender;

@end
