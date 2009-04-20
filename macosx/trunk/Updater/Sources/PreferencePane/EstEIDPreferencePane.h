/* Copyright (c) 2008 Janek Priimann */

#import <PreferencePanes/PreferencePanes.h>

@class EstEIDAgent, SFAuthorizationView;

/**
 * The EstEIDPreferencePane class is the entry-point for the standard Mac OS X System Preferences Pane.
 */
@interface EstEIDPreferencePane : NSPreferencePane
{
    @private
	EstEIDAgent *m_agent;
	IBOutlet SFAuthorizationView *m_authorizationView;
	IBOutlet NSButton *m_automaticUpdateButton;
	IBOutlet NSButton *m_manualUpdateButton;
	IBOutlet NSTextField *m_infoTextField;
	IBOutlet NSTextView *m_infoTextView;
}

/**
 * @name Accessing Attributes
 */

/**
 * Returns the agent of the receiver. Creating it if necessary.
 * @return An EstEIDAgent object.
 */
- (EstEIDAgent *)agent;

/**
 * @name Actions
 */

/**
 * Opens user's web-browser with a helpful link.
 * @param sender Typically the object that invoked this method.
 */
- (IBAction)help:(id)sender;

/**
 * Performs manual update or enables/disables automatic updates.
 * @param sender Typically the object that invoked this method.
 */
- (IBAction)update:(id)sender;

@end
