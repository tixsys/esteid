/* Copyright (c) 2009 Janek Priimann */

#import "NSMenu+Additions.h"

@implementation NSMenu(Additions)

- (void)addItemWithTitle:(NSString *)title tag:(int)tag
{
	NSMenuItem *item = [[NSMenuItem alloc] init];
	
    [item setTitle:title];
    [item setTag:tag];
    [self addItem:item];
    [item release];
}

@end
