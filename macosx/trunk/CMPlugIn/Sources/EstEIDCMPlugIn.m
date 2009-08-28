/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDCMPlugIn.h"
#import "NSMenu+Additions.h"

typedef enum _EstEIDCMPlugInCommand {
    EstEIDCMPlugInCommandSign = 1
} EstEIDCMPlugInCommand;

typedef enum _EstEIDCMPlugInFilter {
    EstEIDCMPlugInFilterNone = 0x00,
    EstEIDCMPlugInFilterInvalid = 0x01,
    EstEIDCMPlugInFilterMultiple = 0x02,
    EstEIDCMPlugInFilterFile = 0x04,
    EstEIDCMPlugInFilterFolder = 0x08,
    EstEIDCMPlugInFilterBDOC = 0x10 | EstEIDCMPlugInFilterFile,
	EstEIDCMPlugInFilterCDOC = 0x20 | EstEIDCMPlugInFilterFile,
	EstEIDCMPlugInFilterDDOC = 0x30 | EstEIDCMPlugInFilterFile,
    EstEIDCMPlugInFilterAny = 0xFF
} EstEIDCMPlugInFilter;

@implementation EstEIDCMPlugIn

- (EstEIDCMPlugInFilter)filterForSelection:(NSArray *)selection
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    EstEIDCMPlugInFilter filter = EstEIDCMPlugInFilterNone;
    NSEnumerator *enumerator = [selection objectEnumerator];
    int count = 0;
    NSURL *url;
    
    while((url = [enumerator nextObject]) != nil) {
        if([url isFileURL]) {
            NSString *path = [url path];
            BOOL isDirectory = NO;
            
            if([fileManager fileExistsAtPath:path isDirectory:&isDirectory]) {
                filter |= (isDirectory) ? EstEIDCMPlugInFilterFolder : EstEIDCMPlugInFilterFile;
                
				if([path hasSuffix:@".bdoc"]) filter |= EstEIDCMPlugInFilterBDOC;
                if([path hasSuffix:@".cdoc"]) filter |= EstEIDCMPlugInFilterCDOC;
                if([path hasSuffix:@".ddoc"]) filter |= EstEIDCMPlugInFilterDDOC;
				
                count++;
            } else {
                filter |= EstEIDCMPlugInFilterInvalid;
            }
        } else {
            filter |= EstEIDCMPlugInFilterInvalid;
        }
    }
    
    if(count > 1) {
        filter |= EstEIDCMPlugInFilterMultiple;
    }
    
    return filter;
}

- (NSArray *)selection:(NSArray *)selection usingFilter:(EstEIDCMPlugInFilter)filter
{
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSEnumerator *enumerator = [selection objectEnumerator];
    NSMutableArray *_selection = [NSMutableArray array];
    NSURL *url;
    
    while((url = [enumerator nextObject]) != nil) {
        if([url isFileURL]) {
            NSString *path = [url path];
            BOOL isDirectory = NO;
            
            if([fileManager fileExistsAtPath:path isDirectory:&isDirectory]) {
                filter |= (isDirectory) ? EstEIDCMPlugInFilterFolder : EstEIDCMPlugInFilterFile;
                
                if(((filter & EstEIDCMPlugInFilterBDOC) != 0 && [path hasSuffix:@".bdoc"]) ||
				   ((filter & EstEIDCMPlugInFilterCDOC) != 0 && [path hasSuffix:@".cdoc"]) ||
				   ((filter & EstEIDCMPlugInFilterDDOC) != 0 && [path hasSuffix:@".ddoc"]) ||
                   ((filter & EstEIDCMPlugInFilterFile) != 0 && !isDirectory) ||
                   ((filter & EstEIDCMPlugInFilterFolder) != 0 && isDirectory)) {
                    [_selection addObject:url];
                }
            } else if((filter & EstEIDCMPlugInFilterInvalid) != 0) {
               [_selection addObject:url];
            }
        } else if((filter & EstEIDCMPlugInFilterInvalid) != 0) {
            [_selection addObject:url];
        }
        
        if((filter & EstEIDCMPlugInFilterMultiple) == 0 && [_selection count] > 0) {
            break;
        }
    }
    
    return _selection;
}

- (NSMenu *)contextMenu:(NSArray *)selection
{
	EstEIDCMPlugInFilter filter = [self filterForSelection:selection];
	
	if((filter & EstEIDCMPlugInFilterFile) != 0 && (filter & EstEIDCMPlugInFilterInvalid) == 0 && (filter & EstEIDCMPlugInFilterFolder) == 0) {
		NSBundle *bundle = [NSBundle bundleForClass:[self class]];
		NSMenu *menu = [[NSMenu alloc] init];
		
		[menu addItemWithTitle:[bundle localizedStringForKey:@"Menu.Action.Sign" value:nil table:nil] tag:EstEIDCMPlugInCommandSign];
		
		return [menu autorelease];
	}
	
	return nil;
}

- (void)contextMenu:(NSArray *)selection handleCommand:(int)command
{
    switch(command) {
		case EstEIDCMPlugInCommandSign:
			if([selection count] > 0) {
				NSMutableArray *arguments = [NSMutableArray arrayWithObjects:@"-a", @"qdigidocclient", nil];
				NSEnumerator *enumerator = [selection objectEnumerator];
				NSURL *url;
				
				while((url = [enumerator nextObject]) != nil) {
					[arguments addObject:[url path]];
				}
				
				[NSTask launchedTaskWithLaunchPath:@"/usr/bin/open" arguments:arguments];
			} else {
				NSBeep();
			}
			break;
    }
}

@end
