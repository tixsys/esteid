/* Copyright (c) 2008 Janek Priimann */

#import <Cocoa/Cocoa.h>

@interface EstEIDCMPlugIn : NSObject

- (NSMenu *)contextMenu:(NSArray *)selection;
- (void)contextMenu:(NSArray *)selection handleCommand:(int)command;

@end
