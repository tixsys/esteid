/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDReaderManager.h"
#import "EstEIDPINPanel.h"
#import "EstEIDWebCertificate.h"
#import "EstEIDWebService.h"
#import <Carbon/Carbon.h>
#import <AppKit/AppKit.h>

NSString *EstEIDWebServiceEventCardInserted = @"OnCardInserted";
NSString *EstEIDWebServiceEventCardRemoved = @"OnCardRemoved";
NSString *EstEIDWebServiceEventReadersChanged = @"OnReadersChanged";

NSString *EstEIDWebServiceExceptionInvalidArguments = @"InvalidArguments";
NSString *EstEIDWebServiceExceptionInvalidEvent = @"InvalidEvent";
NSString *EstEIDWebServiceExceptionInvalidHash = @"InvalidHash";

@implementation EstEIDWebService

- (void)handleEvent:(int)reader listeners:(NSArray *)listeners
{
	NSEnumerator *enumerator = [[[listeners copy] autorelease] objectEnumerator];
	NSArray *arguments = [NSArray arrayWithObject:[NSNumber numberWithInt:reader]];
	EstEIDWebObject *listener;
	
	while((listener = [enumerator nextObject]) != nil) {
		if(![listener invokeMethod:@"handleEvent" withArguments:arguments result:nil]) {
			[listener invokeMethod:nil withArguments:arguments result:nil];
		}
	}
}

- (EstEIDWebCertificate *)authCertificate
{
	NSData *data = [[self->m_readerManager selectedReader] authCertificate];
	
	return (data) ? [[[EstEIDWebCertificate alloc] initWithData:data] autorelease] : nil;
}

- (EstEIDWebCertificate *)signCertificate
{
	NSData *data = [[self->m_readerManager selectedReader] signCertificate];
	
	return (data) ? [[[EstEIDWebCertificate alloc] initWithData:data] autorelease] : nil;
}

- (NSString *)lastName
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataLastNameKey];
}

- (NSString *)firstName
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataFirstNameKey];
}

- (NSString *)middleName
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataMiddleNameKey];
}

- (NSString *)sex
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataSexKey];
}

- (NSString *)citizenship
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataCitizenKey];
}

- (NSString *)birthDate
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataBirthDateKey];
}

- (NSString *)personalID
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataIDKey];
}

- (NSString *)documentID
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataDocumentIDKey];
}

- (NSString *)expiryDate
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataExpiryKey];
}

- (NSString *)placeOfBirth
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataBirthPlaceKey];
}

- (NSString *)issuedDate
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataIssueDateKey];
}

- (NSString *)residencePermit
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataResidencePermitKey];
}

- (NSString *)comment1
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataComment1Key];
}

- (NSString *)comment2
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataComment2Key];
}

- (NSString *)comment3
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataComment3Key];
}

- (NSString *)comment4
{
	return [[[self->m_readerManager selectedReader] personData] objectForKey:EstEIDReaderPersonDataComment4Key];
}

- (NSString *)version
{
	return [[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
}

- (NSString *)readerName:(NSArray *)arguments
{
	NSNumber *reader = ([arguments count] == 1) ? [arguments objectAtIndex:0] : nil;
	
	if([reader isKindOfClass:[NSNumber class]]) {
		return [[self->m_readerManager readerAtIndex:[reader intValue]] name];
	}
	
	@throw EstEIDWebServiceExceptionInvalidArguments;
}

- (NSString *)sign:(NSArray *)arguments
{
	if([arguments count] == 2) {
		NSString *hash = [arguments objectAtIndex:0];
		NSString *url = [arguments objectAtIndex:1];
		
		if([hash isKindOfClass:[NSString class]] && [url isKindOfClass:[NSString class]]) {
			if([hash length] != 40) {
				@throw EstEIDWebServiceExceptionInvalidHash;
			}
			
			@synchronized(self) {
				EstEIDWebCertificate *certificate = [self signCertificate];
				
				if(certificate) {
					EstEIDPINPanel *panel = [[EstEIDPINPanel alloc] init];
					NSString *result;
					
					[panel setDelegate:(id <EstEIDPINPanelDelegate>)self];
					[panel setHash:[hash uppercaseString]];
					[panel setURL:url];
					[panel setName:[[self->m_readerManager selectedReader] personName]];
					[panel beginSheetForWindow:[self window]];
					[panel runModal];
					result = [panel userInfo];
					[panel release];
					
					return result;
				} else {
					return nil;
				}
			}
		}
	}
	
	@throw EstEIDWebServiceExceptionInvalidArguments;
}

- (void)addEventListener:(NSArray *)arguments
{
	if([arguments count] == 2) {
		NSString *type = [arguments objectAtIndex:0];
		EstEIDWebObject *listener = [arguments objectAtIndex:1];
		
		if([type isKindOfClass:[NSString class]] && [listener isKindOfClass:[EstEIDWebObject class]]) {
			NSMutableArray *listeners;
			
			if(![type isEqualToString:EstEIDWebServiceEventCardInserted] && ![type isEqualToString:EstEIDWebServiceEventCardRemoved] && ![type isEqualToString:EstEIDWebServiceEventReadersChanged]) {
				@throw EstEIDWebServiceExceptionInvalidEvent;
			}
			
			if(!(listeners = [self->m_eventListeners objectForKey:type])) {
				listeners = [NSMutableArray array];
				[self->m_eventListeners setObject:listeners forKey:type];
			}
			
			if(![listeners containsObject:listener]) {
				[listeners addObject:listener];
			}
			
			if([self->m_eventListeners count] > 0) {
				[self->m_readerManager setDelegate:(id <EstEIDReaderManagerDelegate>)self];
			}
		} else {
			@throw EstEIDWebServiceExceptionInvalidArguments;
		}
	} else {
		@throw EstEIDWebServiceExceptionInvalidArguments;
	}
}

- (void)removeEventListener:(NSArray *)arguments
{
	if([arguments count] == 2) {
		NSString *type = [arguments objectAtIndex:0];
		EstEIDWebObject *listener = [arguments objectAtIndex:1];
		
		if([type isKindOfClass:[NSString class]] && [listener isKindOfClass:[EstEIDWebObject class]]) {
			NSMutableArray *listeners;
			
			if(![type isEqualToString:EstEIDWebServiceEventCardInserted] && ![type isEqualToString:EstEIDWebServiceEventCardRemoved] && ![type isEqualToString:EstEIDWebServiceEventReadersChanged]) {
				@throw EstEIDWebServiceExceptionInvalidEvent;
			}
			
			if((listeners = [self->m_eventListeners objectForKey:type]) != nil) {
				if([listeners containsObject:listener]) {
					[listeners removeObject:listener];
				}
				
				if([listeners count] == 0) {
					[self->m_eventListeners removeObjectForKey:type];
				}
			}
			
			if(![self->m_eventListeners count] == 0) {
				[self->m_readerManager setDelegate:nil];
			}
		} else {
			@throw EstEIDWebServiceExceptionInvalidArguments;
		}
	} else {
		@throw EstEIDWebServiceExceptionInvalidArguments;
	}
}

#pragma mark EstEIDReaderManagerDelegate

- (void)readerManager:(EstEIDReaderManager *)readerManager didInsertCard:(id <EstEIDReader>)reader
{
	[self handleEvent:[reader index] listeners:[self->m_eventListeners objectForKey:EstEIDWebServiceEventCardInserted]];
}

- (void)readerManager:(EstEIDReaderManager *)readerManager didRemoveCard:(id <EstEIDReader>)reader
{
	[self handleEvent:[reader index] listeners:[self->m_eventListeners objectForKey:EstEIDWebServiceEventCardRemoved]];
}

- (void)readerManagerDidChange:(EstEIDReaderManager *)readerManager
{
	[self handleEvent:-1 listeners:[self->m_eventListeners objectForKey:EstEIDWebServiceEventReadersChanged]];
}

#pragma mark EstEIDPINPanelDelegate

- (BOOL)pinPanelShouldEnd:(EstEIDPINPanel *)pinPanel
{
	NSError *error;
	
	[pinPanel setUserInfo:[[[self->m_readerManager selectedReader] sign:[pinPanel hash] pin:[pinPanel PIN] error:&error] uppercaseString]]; 
	
	return ([pinPanel userInfo]) ? YES : NO;
}

#pragma mark EstEIDWebService

- (NSURL *)URL
{
	return self->m_url;
}

- (void)setURL:(NSURL *)URL
{
	if(self->m_url != URL) {
		[self->m_url release];
		self->m_url = [URL retain];
	}
}

- (NSWindow *)window
{
	return self->m_window;
}

- (void)setWindow:(NSWindow *)window
{
	if(self->m_window != window) {
		[self->m_window release];
		self->m_window = [window retain];
	}
}

#pragma mark EstEIDWebObject

+ (SEL)selectorForMethod:(const char *)name
{
	if(strcmp(name, "getReaderName") == 0) return @selector(readerName:);
	if(strcmp(name, "getVersion") == 0) return @selector(version);
	if(strcmp(name, "sign") == 0) return @selector(sign:);
	if(strcmp(name, "addEventListener") == 0) return @selector(addEventListener:);
	if(strcmp(name, "removeEventListener") == 0) return @selector(removeEventListener:);
	
	return [super selectorForMethod:name];
}

+ (SEL)selectorForProperty:(const char *)name
{
	if(strcmp(name, "authCert") == 0) return @selector(authCertificate);
	if(strcmp(name, "signCert") == 0) return @selector(signCertificate);
	if(strcmp(name, "lastName") == 0) return @selector(lastName);
	if(strcmp(name, "firstName") == 0) return @selector(firstName);
	if(strcmp(name, "middleName") == 0) return @selector(middleName);
	if(strcmp(name, "sex") == 0) return @selector(sex);
	if(strcmp(name, "citizenship") == 0) return @selector(citizenship);
	if(strcmp(name, "birthDate") == 0) return @selector(birthDate);
	if(strcmp(name, "personalID") == 0) return @selector(personalID);
	if(strcmp(name, "documentID") == 0) return @selector(documentID);
	if(strcmp(name, "expiryDate") == 0) return @selector(expiryDate);
	if(strcmp(name, "placeOfBirth") == 0) return @selector(placeOfBirth);
	if(strcmp(name, "issuedDate") == 0) return @selector(issuedDate);
	if(strcmp(name, "residencePermit") == 0) return @selector(residencePermit);
	if(strcmp(name, "comment1") == 0) return @selector(comment1);
	if(strcmp(name, "comment2") == 0) return @selector(comment2);
	if(strcmp(name, "comment3") == 0) return @selector(comment3);
	if(strcmp(name, "comment4") == 0) return @selector(comment4);
	
	return [super selectorForProperty:name];
}

#pragma mark NSObject

- (id)init
{
	self = [super init];
	
	EstEIDLog(@"%@: Alloc! (address=0x%X)", NSStringFromClass([self class]), self);
	
	if(self) {
		self->m_readerManager = [[EstEIDReaderManager alloc] init];
		self->m_eventListeners = [[NSMutableDictionary alloc] init];
		self->m_window = nil;
		self->m_url = nil;
	}
	
	return self;
}

- (void)dealloc
{
	EstEIDLog(@"%@: Dealloc! (address=0x%X)", NSStringFromClass([self class]), self);
	
	[self->m_readerManager release];
	[self->m_eventListeners release];
	[self->m_window release];
	[self->m_url release];
	
	[super dealloc];
}

@end
