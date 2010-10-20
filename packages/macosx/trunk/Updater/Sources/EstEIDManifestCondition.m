/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDManifestCondition.h"
#import "EstEIDManifestPackage.h"

@implementation EstEIDManifestCondition

#pragma mark EstEIDManifestNode

+ (NSString *)tag
{
	return @"if";
}

- (void)writeComponentsToArray:(NSMutableArray *)components
{
	NSEnumerator *enumerator = [[self nodes] objectEnumerator];
	EstEIDManifestNode *node;
	
	while((node = [enumerator nextObject]) != nil) {
		if([node test]) {
			[node writeComponentsToArray:components];
		}
	}
}

- (void)writePackagesToArray:(NSMutableArray *)packages
{
	NSEnumerator *enumerator = [[self nodes] objectEnumerator];
	EstEIDManifestNode *node;
	
	while((node = [enumerator nextObject]) != nil) {
		if([node test]) {
			[node writePackagesToArray:packages];
		}
	}
}

@end
