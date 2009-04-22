/* Copyright (c) 2008 Janek Priimann */

#import "EstEIDManager.h"
#import "EstEIDWebCertificate.h"
#import "EstEIDWebObjectProxy.h"
#import "EstEIDWebPlugIn.h"

NSString *EstEIDWebPlugInEventCardInsert = @"cardInsert";
NSString *EstEIDWebPlugInEventCardRemove = @"cardRemove";
NSString *EstEIDWebPlugInEventCardError = @"cardError";

@implementation EstEIDWebPlugIn

- (void)handleEvent:(int)event listeners:(NSArray *)listeners
{
	NSEnumerator *enumerator = [[[listeners copy] autorelease] objectEnumerator];
	NSArray *arguments = [NSArray arrayWithObject:[NSNumber numberWithInt:event]];
	EstEIDWebObject *listener;
	
	// Remove cached results
	[self->m_userInfo release];
	self->m_userInfo = nil;
	
	[self->m_authCertificate release];
	self->m_authCertificate = nil;
	
	[self->m_signCertificate release];
	self->m_signCertificate = nil;
	
	while((listener = [enumerator nextObject]) != nil) {
		[listener invokeMethod:@"handleEvent" withArguments:arguments];
	}
}

- (NSDictionary *)userInfo
{
	if(!self->m_userInfo) {
		self->m_userInfo = [[self->m_manager userInfo] retain];
	}
	
	return self->m_userInfo;
}

// TODO: Is this acceptable?
- (void)invalidate:(NSTimer *)timer
{
	NSString *state = [self->m_manager readerState];
	
	// Don't send an event if it is reading something at the moment.
	if(state && ![state hasSuffix:@"INUSE"] && ![self->m_state isEqualToString:state]) {
		BOOL notify = (self->m_state) ? YES : NO;
		
		[self->m_state release];
		self->m_state = [state retain];
		
		if(notify) {
			if([state isEqualToString:@"PRESENT"]) {
				[self handleEvent:0 listeners:[self->m_eventListeners objectForKey:EstEIDWebPlugInEventCardInsert]];
			} else if([state isEqualToString:@"EMPTY"]) {
				[self handleEvent:0 listeners:[self->m_eventListeners objectForKey:EstEIDWebPlugInEventCardRemove]];
			// TODO: Really?
			} else {
				[self handleEvent:0 listeners:[self->m_eventListeners objectForKey:EstEIDWebPlugInEventCardError]];
			}
		}
		
#if DEBUG
		NSLog(@"EstEID: State changed to %@", state);
#endif
	}
}

- (EstEIDWebCertificate *)authCertificate
{
	if(!self->m_authCertificate) {
		NSData *data = [self->m_manager authCertificate];
		
		if(data) {
			self->m_authCertificate = [[EstEIDWebCertificate alloc] initWithData:data];
		}
	}
	
	return self->m_authCertificate;
}

- (EstEIDWebCertificate *)signCertificate
{
	if(!self->m_signCertificate) {
		NSData *data = [self->m_manager signCertificate];
		
		if(data) {
			self->m_signCertificate = [[EstEIDWebCertificate alloc] initWithData:data];
		}
	}
	
	return self->m_signCertificate;
}

- (NSString *)lastName
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoLastNameKey];
}

- (NSString *)firstName
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoFirstNameKey];
}

- (NSString *)middleName
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoMiddleNameKey];
}

- (NSString *)sex
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoSexKey];
}

- (NSString *)citizenship
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoCitizenKey];
}

- (NSString *)birthDate
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoBirthDateKey];
}

- (NSString *)personalID
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoIDKey];
}

- (NSString *)documentID
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoDocumentIDKey];
}

- (NSString *)expiryDate
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoExpiryKey];
}

- (NSString *)placeOfBirth
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoBirthPlaceKey];
}

- (NSString *)issuedDate
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoIssueDateKey];
}

- (NSString *)residencePermit
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoResidencePermitKey];
}

- (NSString *)comment1
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoComment1Key];
}

- (NSString *)comment2
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoComment2Key];
}

- (NSString *)comment3
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoComment3Key];
}

- (NSString *)comment4
{
	return [[self userInfo] objectForKey:EstEIDManagerUserInfoComment4Key];
}

- (NSString *)version
{
	return [[[NSBundle bundleForClass:[self class]] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
}

- (NSString *)readerName:(NSArray *)arguments
{
	NSNumber *reader = ([arguments count] == 1) ? [arguments objectAtIndex:0] : nil;
	
	return [self->m_manager nameForReader:([reader isKindOfClass:[NSNumber class]]) ? [reader intValue] : 0];
}

- (NSString *)sign:(NSArray *)arguments
{
	// TODO: 
	return @"";
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
				self->m_proxy = [[EstEIDWebObjectProxy alloc] initWithObject:self];
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

+ (SEL)selectorForGetProperty:(const char *)name
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
	
	return [super selectorForGetProperty:name];;
}

+ (SEL)selectorForSetProperty:(const char *)name
{
	return [super selectorForSetProperty:name];
}

#pragma mark NSObject

- (id)init
{
	self = [super init];
	
	if(self) {
		self->m_manager = [[EstEIDManager alloc] init];
		self->m_eventListeners = [[NSMutableDictionary alloc] init];
	}
	
	return self;
}

- (void)dealloc
{
#if DEBUG
	NSLog(@"%@: dealloc", NSStringFromClass([self class]));
#endif
	
	[self->m_timer invalidate];
	[self->m_timer release];
	[self->m_state release];
	[self->m_proxy release];
	[self->m_manager release];
	[self->m_eventListeners release];
	[self->m_authCertificate release];
	[self->m_signCertificate release];
	[self->m_userInfo release];
	
	[super dealloc];
}

@end
