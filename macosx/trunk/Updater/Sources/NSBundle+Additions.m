/* Copyright (c) 2008 Janek Priimann */

#import "NSBundle+Additions.h"

@implementation NSBundle(Additions)

- (NSArray *)arrayForKey:(NSString *)key
{
	id value = [[self localizedInfoDictionary] objectForKey:key];
    
    if(!value) {
        value = [[self infoDictionary] objectForKey:key];
    }
    
	return ([value isKindOfClass:[NSArray class]]) ? (NSArray *)value : nil;
}

- (id)objectForKey:(NSString *)key
{
	id value = [[self localizedInfoDictionary] objectForKey:key];
    
    if(!value) {
        value = [[self infoDictionary] objectForKey:key];
    }
    
	return value;
}

- (NSString *)stringForKey:(NSString *)key
{
	id value = [[self localizedInfoDictionary] objectForKey:key];
    
    if(!value) {
        value = [[self infoDictionary] objectForKey:key];
    }
    
	return ([value isKindOfClass:[NSString class]]) ? (NSString *)value : nil;
}

@end
