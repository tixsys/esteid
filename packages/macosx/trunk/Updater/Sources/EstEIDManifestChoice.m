/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDManifestChoice.h"
#import "EstEIDManifestPackage.h"

@implementation EstEIDManifestChoice

#pragma mark EstEIDManifestNode

+ (NSString *)tag
{
	return @"switch";
}

- (void)writeComponentsToArray:(NSMutableArray *)components
{
	NSEnumerator *enumerator = [[self nodes] objectEnumerator];
	EstEIDManifestNode *node;
	
	while((node = [enumerator nextObject]) != nil) {
		if([node test]) {
			[node writeComponentsToArray:components];
			return;
		}
	}
}

- (void)writePackagesToArray:(NSMutableArray *)packages
{
	NSEnumerator *enumerator = [[self nodes] objectEnumerator];
	Class class = [EstEIDManifestPackage class];
	EstEIDManifestNode *node;
	
	while((node = [enumerator nextObject]) != nil) {
		if([node test]) {
			if([node isKindOfClass:class]) {
				[packages addObject:node];
				return;
			} else {
				int count = [packages count];
				
				[node writePackagesToArray:packages];
				if(count < [packages count]) return;
			}
		}
	}
}

@end
