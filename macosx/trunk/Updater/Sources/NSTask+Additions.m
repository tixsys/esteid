/* Copyright (c) 2009 Janek Priimann */

#import "NSTask+Additions.h"

@implementation NSTask(Additions)

+ (NSString *)execute:(NSString *)command withArguments:(NSArray *)arguments
{
	NSTask *task = [[[NSTask alloc] init] autorelease];
	NSFileHandle *handle;
	NSData *data;
	
	[task setLaunchPath:command];
	[task setArguments:(arguments) ? arguments : [NSArray array]];
	
	@try {
		NSPipe *pipe = [NSPipe pipe];
		
		[task setStandardOutput:pipe];
		handle = [pipe fileHandleForReading];
		[task launch];
		
		if((data = [handle readDataToEndOfFile]) != nil) {
			return [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease];
		}
	}
	@catch(id exception) {
		
	}
	
	return nil;
}

@end
