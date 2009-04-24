/* Copyright (c) 2009 Janek Priimann */

#import <Carbon/Carbon.h>
#import <Foundation/Foundation.h>

@interface EstEIDPINPanel : NSObject
{
	@private
	WindowRef m_window;
}

- (NSString *)hash;
- (void)setHash:(NSString *)hash;
- (NSString *)name;
- (void)setName:(NSString *)name;
- (NSString *)PIN;
- (void)setPIN:(NSString *)PIN;
- (NSString *)title;
- (void)setTitle:(NSString *)title;
- (NSString *)website;
- (void)setWebsite:(NSString *)website;

- (WindowRef)windowRef;
- (BOOL)isVisible;
- (void)runModal;

/**
 * @name Actions
 */

- (IBAction)orderOut:(id)sender;
- (IBAction)orderFront:(id)sender;

@end
