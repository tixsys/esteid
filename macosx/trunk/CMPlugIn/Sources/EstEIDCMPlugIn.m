/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDCMPlugIn.h"
#import "NSMenu+Additions.h"

// TODO: Must decide what commands should be available.

typedef enum _EstEIDCMPlugInCommand {
    EstEIDCMPlugInCommandOpenDigiDoc = 1
} EstEIDCMPlugInCommand;

typedef enum _EstEIDCMPlugInFilter {
    EstEIDCMPlugInFilterNone = 0x00,
    EstEIDCMPlugInFilterInvalid = 0x01,
    EstEIDCMPlugInFilterMultiple = 0x02,
    EstEIDCMPlugInFilterFile = 0x04,
    EstEIDCMPlugInFilterFolder = 0x08,
    EstEIDCMPlugInFilterDDOC = 0x10 | EstEIDCMPlugInFilterFile,
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
                
                if([path hasSuffix:@".ddoc"]) {
                    filter |= EstEIDCMPlugInFilterDDOC;
                }
                
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
                
                if(((filter & EstEIDCMPlugInFilterDDOC) != 0 && [path hasSuffix:@".ddoc"]) ||
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
    NSMenu *menu = [[NSMenu alloc] init];
    
	[menu addItemWithTitle:NSLocalizedString(@"Open DigiDoc...", nil) tag:EstEIDCMPlugInCommandOpenDigiDoc];
    [menu addItem:[NSMenuItem separatorItem]];
    
    return [menu autorelease];
}

- (void)contextMenu:(NSArray *)selection handleCommand:(int)command
{
    switch(command) {
        case EstEIDCMPlugInCommandOpenDigiDoc:
			// TODO: Just to test that it is working properly
            [[NSWorkspace sharedWorkspace] openFile:nil withApplication:@"TextEdit"];
            break;
    }
}

@end
