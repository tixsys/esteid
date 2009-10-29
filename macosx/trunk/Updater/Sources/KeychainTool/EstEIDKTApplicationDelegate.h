/* Copyright (c) 2009 Janek Priimann */

#import <Cocoa/Cocoa.h>

typedef enum _EstEIDTKLanguage {
	EstEIDKTLanguageEnglish = 0,
	EstEIDKTLanguageEstonian = 1,
	EstEIDKTLanguageRussian = 2
} EstEIDKTLanguage;

@class EstEIDKTCertificate;

@interface EstEIDKTApplicationDelegate : NSObject
{
	@private
	EstEIDKTCertificate *m_certificate;
	NSMutableArray *m_websites;
	IBOutlet NSPopUpButton *m_languagePopUpButton;
	IBOutlet NSButton *m_prevButton;
	IBOutlet NSButton *m_nextButton;
	IBOutlet NSButton *m_saveButton;
	IBOutlet NSTextField *m_infoTextField;
	IBOutlet NSTableView *m_tableView;
	IBOutlet NSTabView *m_tabView;
	IBOutlet NSTextView *m_textView;
	IBOutlet NSWindow *m_window;
}

+ (EstEIDKTApplicationDelegate *)sharedApplicationDelegate;

- (IBAction)changeLanguage:(id)sender;
- (IBAction)prev:(id)sender;
- (IBAction)next:(id)sender;

- (NSString *)pathForResource:(NSString *)resource ofType:(NSString *)type;
- (NSString *)stringForKey:(NSString *)key;

@end
