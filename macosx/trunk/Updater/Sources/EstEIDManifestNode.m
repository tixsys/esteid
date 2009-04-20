/* Copyright (c) 2009 Janek Priimann */

#import <objc/objc-runtime.h>
#import "EstEIDConfiguration.h"
#import "EstEIDManifestNode.h"
#import "NSString+Additions.h"

@interface EstEIDManifestGenericNode : EstEIDManifestTreeNode
{
	@private
	NSDictionary *m_attributes;
	NSString *m_contents;
	NSString *m_tag;
}

@end

@implementation EstEIDManifestGenericNode

#pragma mark EstEIDManifestNode

- (id)initWithTag:(NSString *)tag attributes:(NSDictionary *)attributes children:(id)children
{
	self = [super initWithTag:tag attributes:attributes children:children];
	
	if(self) {
		self->m_attributes = [attributes retain];
		self->m_contents = ([children isKindOfClass:[NSString class]]) ? [children retain] : nil;
		self->m_tag = [tag retain];
	}
	
	return self;
}

- (NSDictionary *)attributes
{
	return self->m_attributes;
}

- (NSString *)contents
{
	return self->m_contents;
}

- (NSString *)tag
{
	return self->m_tag;
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_attributes release];
	[self->m_contents release];
	[self->m_tag release];
	
	[super dealloc];
}

@end

#pragma mark -

@implementation EstEIDManifestNode

+ (Class)classForTag:(NSString *)tag
{
	static NSMutableDictionary *cache = nil;
	Class class = NULL;
	
	if(!cache) {
		@synchronized(self) {
			if(!cache) {
				// HACK: Just to avoid loading one deprecated cocoa class. Nothing serious.
				Class class_deprecated = NSClassFromString(@"NSATSGlyphGenerator");
				int i, c = objc_getClassList(NULL, 0);
				
				cache = [[NSMutableDictionary alloc] init];
				
				if(c > 0) {
					Class *classes = malloc(sizeof(Class) * c);
					SEL sel = @selector(classForTag:);
					
					for(i = 0, c = objc_getClassList(classes, c); i < c; i++) {
						Class class_1 = classes[i];
						
						if(class_1 != class_deprecated && class_getClassMethod(class_1, sel)) {
							NSString *tag_1 = (NSString *)[class_1 tag];
							
							if(tag_1 != nil && [tag_1 isKindOfClass:[NSString class]]) {
								[cache setObject:(id)class_1 forKey:tag_1];
							}
						}
					}
					
					free(classes);
				}
			}
		}
	}
	
	if(tag) {
		class = (Class)[cache objectForKey:tag];
	}
	
	return (class) ? class : [EstEIDManifestGenericNode class];
}

+ (NSString *)tag
{
	return nil;
}

- (id)copyWithZone:(NSZone *)zone
{
	return [self retain];
}

- (id)initWithTag:(NSString *)tag attributes:(NSDictionary *)attributes children:(id)children
{
	self = [super init];
	
	if(self) {
		self->m_architecture = [[[attributes objectForKey:@"arch"] componentsSeparatedByString:@" "] retain];
		self->m_installations = [[[attributes objectForKey:@"has"] componentsSeparatedByString:@" "] retain];
		self->m_operatingSystem = [[[attributes objectForKey:@"macos"] componentsSeparatedByString:@" "] retain];
	}
	
	return self;
}

- (NSDictionary *)attributes
{
	return nil;
}

- (NSString *)contents
{
	return nil;
}

- (NSString *)tag
{
	return (NSString *)[[self class] tag];
}

- (BOOL)test
{
	EstEIDConfiguration *configuration = [EstEIDConfiguration sharedConfiguration];
	NSEnumerator *enumerator;
	NSString *key;
	
	if([self->m_architecture count] > 0) {
		NSString *architecture = [configuration architecture];
		BOOL found = NO;
		
		enumerator = [self->m_architecture objectEnumerator];
		
		while((key = [enumerator nextObject]) != nil) {
			if([architecture rangeOfString:key].length > 0) {
				found = YES;
				break;
			}
		}
		
		if(!found) {
			return NO;
		}
	}
	
	if([self->m_installations count] > 0) {
		NSDictionary *packages = [configuration packages];
		BOOL found = NO;
		
		enumerator = [self->m_installations objectEnumerator];
		
		while((key = [enumerator nextObject]) != nil) {
			NSArray *components = [key componentsSeparatedByString:@"$"];
			NSString *version = [packages objectForKey:key];
			
			if(version && [components count] > 1 && [[components objectAtIndex:1] testVersionString:version]) {
				found = YES;
				break;
			}
		}
		
		if(!found) {
			return NO;
		}
	}
	
	if([self->m_operatingSystem count] > 0) {
		NSString *operatingSystem = [configuration operatingSystem];
		BOOL found = NO;
		
		enumerator = [self->m_operatingSystem objectEnumerator];
		
		while((key = [enumerator nextObject]) != nil) {
			if([key testVersionString:operatingSystem]) {
				found = YES;
				break;
			}
		}
		
		if(!found) {
			return NO;
		}
	}
	
	return YES;
}

- (void)writeComponentsToArray:(NSMutableArray *)components
{
}

- (void)writePackagesToArray:(NSMutableArray *)packages
{
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_architecture release];
	[self->m_installations release];
	[self->m_operatingSystem release];
	
	[super dealloc];
}

@end

#pragma mark -

@implementation EstEIDManifestTreeNode

- (NSArray *)nodes
{
	return self->m_nodes;
}

#pragma mark EstEIDManifestNode

- (id)initWithTag:(NSString *)tag attributes:(NSDictionary *)attributes children:(id)children
{
	self = [super initWithTag:tag attributes:attributes children:children];
	
	if(self) {
		self->m_nodes = ([children isKindOfClass:[NSArray class]]) ? [children retain] : nil;
	}
	
	return self;
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_nodes release];
	
	[super dealloc];
}

@end
