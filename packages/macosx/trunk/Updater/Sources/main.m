/* Copyright (c) 2009 Janek Priimann */

#import <Cocoa/Cocoa.h>

extern BOOL EstEIDApplicationOptionSilent;

int main(int argc, char *argv[])
{
	int i;
	
	for(i = 0; i < argc; i++) {
		if(strcmp(argv[i], "--silent") == 0) {
			EstEIDApplicationOptionSilent = YES;
		}
	}
	
    return NSApplicationMain(argc, (const char **)argv);
}
