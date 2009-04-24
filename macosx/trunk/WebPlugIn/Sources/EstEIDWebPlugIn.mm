/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDReaderManager.h"
#import "EstEIDPINPanel.h"
#import "EstEIDWebCertificate.h"
#import "EstEIDWebPlugIn.h"
#import "EstEIDWebPlugInProxy.h"
#import <Carbon/Carbon.h>

NSString *EstEIDWebPlugInEventCardInsert = @"cardInsert";
NSString *EstEIDWebPlugInEventCardRemove = @"cardRemove";
NSString *EstEIDWebPlugInEventCardError = @"cardError";

@implementation EstEIDWebPlugIn

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

- (NSDictionary *)userInfo
{
	if(!self->m_userInfo) {
		self->m_userInfo = [[self->m_readerManager userInfo] retain];
	}
	
	return self->m_userInfo;
}

// TODO: ok?
- (void)invalidate:(NSTimer *)timer
{
	NSString *readerState = [self->m_readerManager readerState];
	
	// Don't send an event if it is reading something at the moment.
	if(![readerState hasSuffix:@"INUSE"] && self->m_readerState != readerState && (!readerState || ![self->m_readerState isEqualToString:readerState])) {
		BOOL notify = (self->m_readerState) ? YES : NO;
		
		// Update reader state
		[self->m_readerState release];
		self->m_readerState = [readerState retain];
		
		// Remove cached results
		[self->m_userInfo release];
		self->m_userInfo = nil;
		[self->m_authCertificate release];
		self->m_authCertificate = nil;
		[self->m_signCertificate release];
		self->m_signCertificate = nil;
		
		if(notify) {
			if([readerState isEqualToString:@"PRESENT"]) {
				[self handleEvent:[self->m_readerManager selectedReader] listeners:[self->m_eventListeners objectForKey:EstEIDWebPlugInEventCardInsert]];
			} else if([readerState isEqualToString:@"EMPTY"] || !readerState) {
				[self handleEvent:[self->m_readerManager selectedReader] listeners:[self->m_eventListeners objectForKey:EstEIDWebPlugInEventCardRemove]];
			} else {
				// TODO: ok?
				[self handleEvent:[self->m_readerManager selectedReader] listeners:[self->m_eventListeners objectForKey:EstEIDWebPlugInEventCardError]];
			}
		}
		
#if DEBUG
		NSLog(@"EstEID: Reader state changed to %@", readerState);
#endif
	}
}

- (EstEIDWebCertificate *)authCertificate
{
	if(!self->m_authCertificate) {
		NSData *data = [self->m_readerManager authCertificate];
		
		if(data) {
			self->m_authCertificate = [[EstEIDWebCertificate alloc] initWithData:data];
		}
	}
	
	return self->m_authCertificate;
}

- (EstEIDWebCertificate *)signCertificate
{
	if(!self->m_signCertificate) {
		NSData *data = [self->m_readerManager signCertificate];
		
		if(data) {
			self->m_signCertificate = [[EstEIDWebCertificate alloc] initWithData:data];
		}
	}
	
	return self->m_signCertificate;
}

- (NSString *)lastName
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoLastNameKey];
}

- (NSString *)firstName
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoFirstNameKey];
}

- (NSString *)middleName
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoMiddleNameKey];
}

- (NSString *)sex
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoSexKey];
}

- (NSString *)citizenship
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoCitizenKey];
}

- (NSString *)birthDate
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoBirthDateKey];
}

- (NSString *)personalID
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoIDKey];
}

- (NSString *)documentID
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoDocumentIDKey];
}

- (NSString *)expiryDate
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoExpiryKey];
}

- (NSString *)placeOfBirth
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoBirthPlaceKey];
}

- (NSString *)issuedDate
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoIssueDateKey];
}

- (NSString *)residencePermit
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoResidencePermitKey];
}

- (NSString *)comment1
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoComment1Key];
}

- (NSString *)comment2
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoComment2Key];
}

- (NSString *)comment3
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoComment3Key];
}

- (NSString *)comment4
{
	return [[self userInfo] objectForKey:EstEIDReaderUserInfoComment4Key];
}

- (NSString *)version
{
	return [[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
}

- (NSString *)readerName:(NSArray *)arguments
{
	NSNumber *reader = ([arguments count] == 1) ? [arguments objectAtIndex:0] : nil;
	
	return [self->m_readerManager readerName:([reader isKindOfClass:[NSNumber class]]) ? [reader intValue] : [self->m_readerManager selectedReader]];
}

- (NSObject *)sign:(NSArray *)arguments
{
	if([arguments count] == 2) {
		NSString *hash = [arguments objectAtIndex:0];
		NSString *url = [arguments objectAtIndex:1];
		
		if([hash isKindOfClass:[NSString class]] && [url isKindOfClass:[NSString class]]) {
			EstEIDWebCertificate *certificate = [self signCertificate];
			
			// TODO: Cleanup
			
			if(certificate) {
				EstEIDPINPanel *panel = [[EstEIDPINPanel alloc] init];
				
				[panel setName:[NSString stringWithFormat:@"%@ %@", [[self firstName] capitalizedString], [[self lastName] capitalizedString]]];
				[panel setHash:[hash uppercaseString]];
				//[panel setURL:url];
				[panel orderFront:nil];
				[panel runModal];
				[panel release];
			} else {
				return nil;
			}
		} else {
			
			return nil;
		}
	}
	
	return [NSException exceptionWithName:@"SigningError!" reason:nil userInfo:nil];
}

- (id)addEventListener:(NSArray *)arguments
{
	if([arguments count] == 2) {
		id type = [arguments objectAtIndex:0];
		id listener = [arguments objectAtIndex:1];
		
		if([type isKindOfClass:[NSString class]] && [listener isKindOfClass:[EstEIDWebObject class]]) {
			NSMutableArray *listeners = [self->m_eventListeners objectForKey:(NSString *)type];
			
			if(!listeners) {
				listeners = [NSMutableArray array];
				[self->m_eventListeners setObject:listeners forKey:type];
			}
			
			if(![listeners containsObject:listener]) {
				[listeners addObject:listener];
			}
			
			if(!self->m_timer) {
				self->m_timer = [[NSTimer alloc] initWithFireDate:[NSDate dateWithTimeIntervalSinceNow:0.1F] interval:0.5F target:self->m_proxy selector:@selector(invalidate:) userInfo:nil repeats:YES];
				[[NSRunLoop currentRunLoop] addTimer:self->m_timer forMode:NSDefaultRunLoopMode];
			}
		}
	}
	
	return nil;
}

- (id)removeEventListener:(NSArray *)arguments
{
	if([arguments count] == 2) {
		id type = [arguments objectAtIndex:0];
		id listener = [arguments objectAtIndex:1];
		
		if([type isKindOfClass:[NSString class]] && [listener isKindOfClass:[EstEIDWebObject class]]) {
			NSMutableArray *listeners = [self->m_eventListeners objectForKey:(NSString *)type];
			
			if(listeners) {
				if([listeners containsObject:listener]) {
					[listeners removeObject:listener];
				}
				
				if([listeners count] == 0) {
					[self->m_eventListeners removeObjectForKey:(NSString *)type];
				}
			}
		}
		
		// No listeners left!
		if([self->m_eventListeners count] == 0) {
			[self->m_timer invalidate];
			[self->m_timer release];
			self->m_timer = nil;
		}
	}
	
	return nil;
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
#if DEBUG
	NSLog(@"EstEID: Plug-in allocated (address=0x%X)", self);
#endif
	
	self = [super init];
	
	if(self) {
		self->m_readerManager = [[EstEIDReaderManager alloc] init];
		self->m_eventListeners = [[NSMutableDictionary alloc] init];
		self->m_proxy = [[EstEIDWebPlugInProxy alloc] initWithPlugIn:self];
	}
	
	return self;
}

- (void)dealloc
{
#if DEBUG
	NSLog(@"EstEID: Plug-in deallocated (address=0x%X)", self);
#endif
	
	[self->m_timer invalidate];
	[self->m_timer release];
	[self->m_proxy release];
	[self->m_readerState release];
	[self->m_readerManager release];
	[self->m_eventListeners release];
	[self->m_authCertificate release];
	[self->m_signCertificate release];
	[self->m_userInfo release];
	
	[super dealloc];
}

@end
