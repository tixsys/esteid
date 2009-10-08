/* Copyright (c) 2009 Janek Priimann */

#import <AppKit/AppKit.h>
#import <Foundation/Foundation.h>

@class EstEIDPINPanel;

@protocol EstEIDPINPanelDelegate <NSObject>

- (BOOL)pinPanelShouldEnd:(EstEIDPINPanel *)pinPanel;

@end

@interface EstEIDPINPanel : NSObject
{
	@private
	id <EstEIDPINPanelDelegate> m_delegate;
	id m_userInfo;
	IBOutlet NSWindow *m_window;
	IBOutlet NSButton *m_cancelButton;
	IBOutlet NSButton *m_detailsButton;
	IBOutlet NSBox *m_detailsBox;
	IBOutlet NSTextField *m_detailsLabel;
	IBOutlet NSTextField *m_errorLabel;
	IBOutlet NSTextField *m_hashLabel;
	IBOutlet NSTextField *m_hashTextField;
	IBOutlet NSTextField *m_messageLabel;
	IBOutlet NSTextField *m_nameLabel;
	IBOutlet NSTextField *m_nameTextField;
	IBOutlet NSButton *m_okButton;
	IBOutlet NSTextField *m_pinLabel;
	IBOutlet NSSecureTextField *m_pinTextField;
	IBOutlet NSTextField *m_urlLabel;
	IBOutlet NSTextField *m_urlTextField;
}

- (NSWindow *)window;

- (id <EstEIDPINPanelDelegate>)delegate;
- (void)setDelegate:(id <EstEIDPINPanelDelegate>)delegate;
- (id)userInfo;
- (void)setUserInfo:(id)userInfo;

- (BOOL)showsDetails;
- (void)setShowsDetails:(BOOL)flag animate:(BOOL)animate;

- (NSString *)error;
- (void)setError:(NSString *)error fatal:(BOOL)fatal;
- (NSString *)hash;
- (void)setHash:(NSString *)hash;
- (NSString *)name;
- (void)setName:(NSString *)name;
- (NSString *)PIN;
- (void)setPIN:(NSString *)PIN;
- (NSString *)URL;
- (void)setURL:(NSString *)url;

- (void)beginSheetForWindow:(NSWindow *)window;
- (void)beginSheetForWindow:(NSWindow *)window modalDelegate:(id)delegate didEndSelector:(SEL)selector contextInfo:(void *)info;
- (void)runModal;

/**
 * @name Actions
 */

- (IBAction)cancel:(id)sender;
- (IBAction)ok:(id)sender;
- (IBAction)showHelp:(id)sender;
- (IBAction)showOptions:(id)sender;
- (IBAction)toggleDetails:(id)sender;

@end
