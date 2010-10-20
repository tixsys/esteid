/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDManifestPackage.h"

@implementation EstEIDManifestPackage

- (NSString *)identifier
{
	return self->m_identifier;
}

- (long long)length
{
	return self->m_length;
}

- (NSArray *)locations
{
	return self->m_locations;
}

- (BOOL)restart
{
	return self->m_restart;
}

- (NSString *)sha1
{
	return self->m_sha1;
}

- (NSString *)version
{
	return self->m_version;
}

#pragma mark EstEIDManifestNode

+ (NSString *)tag
{
	return @"package";
}

- (id)initWithTag:(NSString *)tag attributes:(NSDictionary *)attributes children:(id)children
{
	self = [super initWithTag:tag attributes:attributes children:children];
	
	if(self) {
		self->m_identifier = [[attributes objectForKey:@"id"] retain];
		self->m_length = [[attributes objectForKey:@"length"] longLongValue];
		self->m_sha1 = [[[attributes objectForKey:@"sha1"] lowercaseString] retain];
		self->m_restart = ([[attributes objectForKey:@"restart"] intValue]) ? YES : NO;
		self->m_version = [[attributes objectForKey:@"version"] retain];
		
		if([children isKindOfClass:[NSString class]]) {
			NSCharacterSet *whitespaceCharacterSet = [NSCharacterSet whitespaceAndNewlineCharacterSet];
			NSScanner *scanner = [[NSScanner alloc] initWithString:(NSString *)children];
			NSString *location;
			
			self->m_locations = [[NSMutableArray alloc] init];
			
			[scanner setCharactersToBeSkipped:whitespaceCharacterSet];
			
			while([scanner scanUpToCharactersFromSet:whitespaceCharacterSet intoString:&location]) {
				[(NSMutableArray *)self->m_locations addObject:location];
			}
			
			[scanner release];
		}
	}
	
	return self;
}

- (void)writePackagesToArray:(NSMutableArray *)packages
{
	[packages addObject:self];
}

#pragma mark NSObject

- (unsigned int)hash
{
	return [self->m_locations hash];
}

- (BOOL)isEqual:(EstEIDManifestPackage *)package
{
	return [self->m_locations isEqual:[package locations]];
}

- (NSComparisonResult)compare:(EstEIDManifestPackage *)package
{
	return [self->m_identifier compare:[package identifier]];
}

- (void)dealloc
{
	[self->m_identifier release];
	[self->m_locations release];
	[self->m_sha1 release];
	[self->m_version release];
	
	[super dealloc];
}

@end
