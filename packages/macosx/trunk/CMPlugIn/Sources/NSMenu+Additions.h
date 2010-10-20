/* Copyright (c) 2009 Janek Priimann */

#import <Cocoa/Cocoa.h>

/**
 * This category adds methods to the Foundation framework’s NSMenu class.
 */
@interface NSMenu(Additions)

/**
 * Creates a new menu item and adds it to the end of the receiver.
 * @param title A string to be made the title of the menu item.
 * @param tag An integer tag to associate with the receiver.
 */
- (void)addItemWithTitle:(NSString *)title tag:(int)tag;

@end
