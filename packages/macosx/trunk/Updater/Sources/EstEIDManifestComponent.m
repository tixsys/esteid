/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDConfiguration.h"
#import "EstEIDManifestComponent.h"
#import "EstEIDManifestPackage.h"
#import "NSObject+Additions.h"
#import "NSString+Additions.h"

@implementation EstEIDManifestComponent

+ (NSString *)language
{
	static NSString *language = nil;
	
	if(!language) {
		NSArray *languages = [NSLocale preferredLanguages];
		
		language = ([languages count] > 0) ? [languages objectAtIndex:0] : nil;
		
		if([language length] > 2) {
			language = [language substringToIndex:2];
		}
		
		language = [[language lowercaseString] retain];
	}
	
	return language;
}

- (NSString *)description
{
	NSString *language = [EstEIDManifestComponent language];
	id value = nil;
	
	// Try preferred language like 'et' or 'ru'
	if(language) {
		value = [self->m_description objectForKey:language];
	}
	
	// Try english
	if(!value) {
		value = [self->m_description objectForKey:@"en"];
	}
	
	// Try first
	if(!value) {
		value = [[self->m_description objectEnumerator] nextObject];
	}
	
	return (value) ? value : @"";
}

- (EstEIDManifestComponentPriority)priority
{
	return self->m_priority;
}

- (long long)length
{
	@synchronized(self) {
		if(self->m_length == -1) {
			NSEnumerator *enumerator = [[self packages] objectEnumerator];
			EstEIDManifestPackage *package;
			
			self->m_length = 0;
			
			while((package = [enumerator nextObject]) != nil) {
				self->m_length += [package length];
			}
		}
	}
	
	return self->m_length;
}

- (NSArray *)packages
{
	NSDictionary *installedPackages = [[EstEIDConfiguration sharedConfiguration] packages];
	NSMutableArray *packages = [NSMutableArray array];
	int i, c;
	
	[self writePackagesToArray:packages];
	
	// Remove any installed and newer packages
	for(i = 0, c = [packages count]; i < c; i++) {
		EstEIDManifestPackage *package = [packages objectAtIndex:i];
		NSString *version = [installedPackages objectForKey:[package identifier]];
		
		if(version && ![[package version] testVersionString:version]) {
			[packages removeObjectAtIndex:i];
			i--;
			c--;
		}
			
	}
	
	return packages;
}

- (BOOL)restart
{
	NSMutableArray *packages = [[NSMutableArray alloc] init];
	BOOL restart = NO;
	int i, c;
	
	[self writePackagesToArray:packages];
	
	for(i = 0, c = [packages count]; i < c; i++) {
		if([(EstEIDManifestPackage *)[packages objectAtIndex:i] restart]) {
			restart = YES;
			break;
		}
	}
	
	[packages release];
	
	return restart;
}

- (NSString *)title
{
	NSString *language = [EstEIDManifestComponent language];
	id value = nil;
	
	// Try preferred language like 'et' or 'ru'
	if(language) {
		value = [self->m_title objectForKey:language];
	}
	
	// Try english
	if(!value) {
		value = [self->m_title objectForKey:@"en"];
	}
	
	// Try first
	if(!value) {
		value = [[self->m_title objectEnumerator] nextObject];
	}
	
	return (value) ? value : @"";
}

- (NSString *)version
{
	@synchronized(self) {
		if(!self->m_version) {
			NSMutableArray *packages = [[NSMutableArray alloc] init];
			
			[self writePackagesToArray:packages];
			
			if([packages count] > 0) {
				self->m_version = [[(EstEIDManifestPackage *)[packages objectAtIndex:0] version] retain];
			}
			
			[packages release];
		}
	}
	
	return self->m_version;
}

#pragma mark EstEIDManifestNode

+ (NSString *)tag
{
	return @"component";
}

- (id)initWithTag:(NSString *)tag attributes:(NSDictionary *)attributes children:(id)children
{
	self = [super initWithTag:tag attributes:attributes children:children];
	
	if(self) {
		switch([[attributes objectForKey:@"priority"] intValue]) {
			case EstEIDManifestComponentPriorityCritical:
				self->m_priority = EstEIDManifestComponentPriorityCritical;
				break;
			case EstEIDManifestComponentPriorityHigh:
				self->m_priority = EstEIDManifestComponentPriorityHigh;
				break;
			case EstEIDManifestComponentPriorityLow:
			default:
				self->m_priority = EstEIDManifestComponentPriorityLow;
				break;
		}
		
		self->m_description = [[NSMutableDictionary alloc] init];
		self->m_length = -1;
		self->m_title = [[NSMutableDictionary alloc] init];
		self->m_version = [[attributes objectForKey:@"version"] retain];
		
		if([children isKindOfClass:[NSArray class]]) {
			NSEnumerator *enumerator = [(NSArray *)children objectEnumerator];
			EstEIDManifestNode *node;
			
			while((node = [enumerator nextObject]) != nil) {
				NSString *language = [[node attributes] objectForKey:@"xml:lang"];
				NSString *tag = [node tag];
				
				if([tag isEqualToString:@"title"]) {
					[(NSMutableDictionary *)self->m_title setValue:[node contents] forKey:(language) ? language : @"en"];
				} else if([tag isEqualToString:@"description"]) {
					[(NSMutableDictionary *)self->m_description setValue:[node contents] forKey:(language) ? language : @"en"];
				}
			}
		}
	}
	
	return self;
}

- (void)writeComponentsToArray:(NSMutableArray *)components
{
	if([[self packages] count] > 0) {
		[components addObject:self];
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

#pragma mark NSObject

- (void)dealloc
{
	[self->m_description release];
	[self->m_title release];
	[self->m_version release];
	
	[super dealloc];
}

@end
