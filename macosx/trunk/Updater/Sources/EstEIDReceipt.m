/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDReceipt.h"
#import "NSBundle+Additions.h"
#import "NSObject+Additions.h"
#import "NSString+Additions.h"

@interface EstEIDReceipt(Private)

- (id)p_initWithIdentifier:(NSString *)identifier version:(NSString *)version metadata:(NSString *)metadata;

- (void)p_loadMetadata;

@end

@implementation EstEIDReceipt(Private)

- (id)p_initWithIdentifier:(NSString *)identifier version:(NSString *)version metadata:(NSString *)metadata
{
	self = [super init];
	
	if(self) {
		self->m_identifier = [identifier retain];
		self->m_version = [version retain];
		self->m_metadata = [metadata retain];
	}
	
	return self;
}

- (void)p_loadMetadata
{
	if(self->m_metadata) {
		NSDictionary *infoPlist = [[NSDictionary alloc] initWithContentsOfFile:self->m_metadata];
		
		if(infoPlist) {
			id title = [infoPlist objectForKey:@"IFPkgDescriptionTitle"];
			id description = [infoPlist objectForKey:@"IFPkgDescriptionDescription"];
			
			if([title isKindOfClass:[NSString class]]) {
				self->m_title = [(NSString *)title retain];
			}
			
			if([description isKindOfClass:[NSString class]]) {
				self->m_description = [(NSString *)description retain];
			}
			
			[infoPlist release];
		}
		
		[self->m_metadata release];
		self->m_metadata = nil;
	}
}

@end

@implementation EstEIDReceipt

+ (NSArray *)receipts
{
	return [self receiptsWithPrefix:nil];
}

// Q: Use the built-in pkgutil tool instead of scanning all the packages manually? Nevertheless there is no escape from localization hell. -- JP
+ (NSArray *)receiptsWithPrefix:(NSString *)prefix
{
	NSString *receiptsPath = [(NSString *)[NSSearchPathForDirectoriesInDomains(NSLibraryDirectory, NSLocalDomainMask, YES) objectAtIndex:0] stringByAppendingPathComponent:@"Receipts"];
	NSFileManager *fileManager = [NSFileManager defaultManager];
	NSEnumerator *enumerator = [[fileManager directoryContentsAtPath:receiptsPath] objectEnumerator];
	NSArray *languages = [NSLocale preferredLanguages];
	NSMutableDictionary *receipts = [NSMutableDictionary dictionary];
	NSBundle *bundle = [NSBundle bundleForClass:[self class]];
	NSMutableDictionary *dates;
	NSString *locale = nil;
	NSString *file;
	
	if([languages count] > 0) {
		locale = [[languages objectAtIndex:0] lowercaseString];
		if([locale length] > 2) locale = [locale substringToIndex:2];
		if([locale isEqualToString:@"en"]) locale = nil;
	}
	
	dates = ([[bundle objectForKey:@"EstEIDCheckPackageInstallationDate"] boolValue]) ? [NSMutableDictionary dictionary] : nil;
	
	while((file = [enumerator nextObject]) != nil) {
		if([file hasSuffix:@".pkg"]) {
			NSString *receiptPath = [receiptsPath stringByAppendingPathComponent:file];
			NSDictionary *infoPlist = [[NSDictionary alloc] initWithContentsOfFile:[receiptPath stringByAppendingPathComponent:@"Contents/Info.plist"]];
			
			if(infoPlist) {
				NSString *key = [infoPlist objectForKey:@"CFBundleIdentifier"];
				
				if(!prefix || [key hasPrefix:prefix]) {
					NSString *value = [infoPlist objectForKey:@"CFBundleShortVersionString"];
					
					// TODO: checkDates
					
					if([key isKindOfClass:[NSString class]] && [value isKindOfClass:[NSString class]]) {
						EstEIDReceipt *receipt = [receipts objectForKey:key];
						
						if(receipt == nil || [value testVersionString:[receipt version]]) {
							NSString *metadataPath = (locale) ? [receiptPath stringByAppendingPathComponent:[NSString stringWithFormat:@"Contents/Resources/%@.lproj/Description.plist", locale]] : nil;
							
							if(![fileManager fileExistsAtPath:metadataPath]) metadataPath = [receiptPath stringByAppendingPathComponent:@"Contents/Resources/en.lproj/Description.plist"];
							if(![fileManager fileExistsAtPath:metadataPath]) metadataPath = [receiptPath stringByAppendingPathComponent:@"Contents/Resources/English.lproj/Description.plist"];
							if(![fileManager fileExistsAtPath:metadataPath]) metadataPath = [receiptPath stringByAppendingPathComponent:@"Contents/Resources/en-GB.lproj/Description.plist"];
							if(![fileManager fileExistsAtPath:metadataPath]) metadataPath = [receiptPath stringByAppendingPathComponent:@"Contents/Resources/Description.plist"];
							
							receipt = [[EstEIDReceipt alloc] p_initWithIdentifier:key version:value metadata:metadataPath];
							[receipts setObject:receipt forKey:key];
							[receipt release];
						} else {
#ifdef DEBUG
							NSLog(@"%@: Skipped '%@' (new %@ vs old %@)", NSStringFromClass((Class)self), key, value, [receipt version]);
#endif
						}
					}
					
					[infoPlist release];
				}
			}
		}
	}
	
	return [receipts allValues];
}

- (id)initWithIdentifier:(NSString *)identifier
{
	NSArray *receipts = [[self class] receiptsWithPrefix:identifier];
	
	[self release];
	self = nil;
	
	return ([receipts count] == 1) ? [[receipts objectAtIndex:0] retain] : nil;
}

- (NSString *)description
{
	if(!self->m_description && self->m_metadata) {
		[self p_loadMetadata];
	}
	
	return self->m_description;
}

- (NSString *)identifier
{
	return self->m_identifier;
}

- (NSString *)title
{
	if(!self->m_title && self->m_metadata) {
		[self p_loadMetadata];
	}
	
	return self->m_title;
}

- (NSString *)version
{
	return self->m_version;
}

#pragma mark NSObject

- (unsigned int)hash
{
	return [self->m_identifier hash];
}

- (BOOL)isEqual:(EstEIDReceipt *)receipt
{
	return [self->m_identifier isEqualToString:[receipt identifier]];
}

- (void)dealloc
{
	[self->m_description release];
	[self->m_identifier release];
	[self->m_metadata release];
	[self->m_title release];
	[self->m_version release];
	
	[super dealloc];
}

@end
