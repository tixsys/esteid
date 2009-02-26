/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDPreferencePane.h"

@implementation EstEIDPreferencePane

- (id)initWithBundle:(NSBundle *)bundle
{
    self = [super initWithBundle:bundle];
    
	if(self) {
		// do more initialization here
	}
	
	return self;
}

- (void)willSelect
{
}

- (void)didSelect
{
}

- (void)willUnselect
{
}

- (void)didUnselect
{
}

- (NSPreferencePaneUnselectReply)shouldUnselect
{
	return NSUnselectNow;
}

- (void)dealloc
{
    [super dealloc];
}

@end
