/* Copyright (c) 2009 Janek Priimann */

#import "NSTableView+Additions.h"

@interface NSObject(NSTableViewDelegateAdditions)

- (void)tableViewOnDelete:(NSTableView *)tableView row:(int)rowIndex;

@end

#pragma mark -

@implementation NSTableView(Additions)

- (void)keyDown:(NSEvent *)event
{
	id delegate = [self delegate];
	
	if([[event characters] length] > 0) {
		unichar ch = [[event characters] characterAtIndex:0];
		
		if((ch == NSDeleteFunctionKey || ch == NSDeleteCharFunctionKey || ch == NSDeleteCharacter || ch == NSBackspaceCharacter) &&
		   [delegate respondsToSelector:@selector(tableViewOnDelete: row:)]) {
			[delegate tableViewOnDelete:self row:[self selectedRow]];
			return;
		}
	}
	
	[super keyDown:event];
}

@end
