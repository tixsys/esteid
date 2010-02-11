/* Copyright (c) 2009 Janek Priimann */

#import <smartcardpp/common.h>
#import <smartcardpp/PCSCManager.h>
#import <smartcardpp/SmartCardManager.h>
#import <smartcardpp/esteid/EstEidCard.h>
#import "EstEIDReaderManager.h"

static inline NSData *CPlusDataToNSData(ByteVec bytes)
{
	return [NSData dataWithBytes:&bytes[0] length:bytes.size()];
}

static inline NSString *CPlusStringToNSString(std::string str)
{
	return [(NSString *)CFStringCreateWithBytes(kCFAllocatorDefault, (const UInt8 *)str.c_str(), str.length(), kCFStringEncodingISOLatin9, NO) autorelease];
}

typedef struct _EstEIDReaderManagerState {
	SmartCardManager *internal;
	struct {
		id <EstEIDReader> *data;
		unsigned int count;
	} readers;
	int selection;
	NSTimer *timer;
	id proxy;
} EstEIDReaderManagerState;

#define EstEIDReaderManagerStateIsValid(state) (state && state->internal)

#pragma mark -

std::string toHex(const std::vector<unsigned char> &byteVec) {
	std::ostringstream buf;
	for(ByteVec::const_iterator it = byteVec.begin();it!=byteVec.end();it++)
		buf << std::setfill('0') << std::setw(2) <<std::hex <<
		(short) *it;
	return buf.str();
}

bool getOneChar(ByteVec &hexStr,int &c) {
	if (hexStr.empty()) return false;
	int ch = hexStr.back();
	hexStr.pop_back();
	while (ch == ' ' || ch == '\r' || ch == '\t' || ch == '\n') {
		if (hexStr.empty()) return false;
		ch = hexStr.back();
		hexStr.pop_back();
	}
	if (ch >= '0' && ch <= '9')
		c = ch - '0';
	else if (ch >= 'A' && ch <= 'F')
		c = ch - 'A' + 10;
	else if (ch >= 'a' && ch <= 'f')
		c = ch - 'a' + 10;
	return true;
}

ByteVec fromHex(const std::string &hexStr) {
	ByteVec retVal,inpVal(hexStr.length(),'\0');
	copy(hexStr.begin(),hexStr.end(),inpVal.begin());
	reverse(inpVal.begin(),inpVal.end());
	int c1,c2;
	while(getOneChar(inpVal,c1) && getOneChar(inpVal,c2)) {
		retVal.push_back( 	(c1 << 4)  | c2 );
	}
	return retVal;
}

#pragma mark -

NSString *EstEIDReaderErrorDomain = @"EstEIDReaderError";

NSString *EstEIDReaderPersonDataLastNameKey = @"SURNAME";
NSString *EstEIDReaderPersonDataFirstNameKey = @"FIRSTNAME";
NSString *EstEIDReaderPersonDataMiddleNameKey = @"MIDDLENAME";
NSString *EstEIDReaderPersonDataSexKey = @"SEX";
NSString *EstEIDReaderPersonDataCitizenKey = @"CITIZEN";
NSString *EstEIDReaderPersonDataBirthDateKey = @"BIRTHDATE";
NSString *EstEIDReaderPersonDataIDKey = @"ID";
NSString *EstEIDReaderPersonDataDocumentIDKey = @"DOCUMENTID";
NSString *EstEIDReaderPersonDataExpiryKey = @"EXPIRY";
NSString *EstEIDReaderPersonDataBirthPlaceKey = @"BIRTHPLACE";
NSString *EstEIDReaderPersonDataIssueDateKey = @"ISSUEDATE";
NSString *EstEIDReaderPersonDataResidencePermitKey = @"RESIDENCEPERMIT";
NSString *EstEIDReaderPersonDataComment1Key = @"COMMENT1";
NSString *EstEIDReaderPersonDataComment2Key = @"COMMENT2";
NSString *EstEIDReaderPersonDataComment3Key = @"COMMENT3";
NSString *EstEIDReaderPersonDataComment4Key = @"COMMENT4";

NSString *EstEIDReaderRetryCounterPUK = @"puk";
NSString *EstEIDReaderRetryCounterPIN1 = @"pin1";
NSString *EstEIDReaderRetryCounterPIN2 = @"pin2";

@interface EstEIDReader : NSObject <EstEIDReader>
{
	@public
	BOOL m_connected;
	EstEIDReaderManagerState *m_state;
	
	@private
	NSData *m_authCertificate;
	NSData *m_signCertificate;
	NSDictionary *m_personData;
	NSString *m_personName;
}

- (id)initWithState:(EstEIDReaderManagerState *)state;

- (void)clearCache;

@end

@implementation EstEIDReader

- (id)initWithState:(EstEIDReaderManagerState *)state
{
	self = [super init];
	
	if(self) {
		self->m_state = state;
		self->m_connected = NO;
	}
	
	return self;
}

- (void)clearCache
{
	[self->m_authCertificate release];
	self->m_authCertificate = nil;
	[self->m_signCertificate release];
	self->m_signCertificate = nil;
	[self->m_personData release];
	self->m_personData = nil;
	[self->m_personName release];
	self->m_personName = nil;
}

- (BOOL)allowsSecureEntry
{
	if(EstEIDReaderManagerStateIsValid(self->m_state)) {
		try {
			EstEidCard card(*self->m_state->internal, [self index]);
			
			return card.hasSecurePinEntry();
		}
		catch(std::runtime_error err) {
#if DEBUG
			NSLog(@"%@: hasSecurePinEntry failure", NSStringFromClass([self class]));
#endif
		}
	}
	
	return NO;
}

- (BOOL)isEmpty
{
	return (self->m_connected) ? NO : YES;
}

- (unsigned int)index
{
	unsigned int index;
	
	for(index = 0; index < self->m_state->readers.count; index++) {
		if(self->m_state->readers.data[index] == self) {
			return index;
		}
	}
	
	return -1;
}

- (EstEIDReaderState)state
{
	EstEIDReaderState state = EstEIDReaderStateUnknown;
	
	if(EstEIDReaderManagerStateIsValid(self->m_state)) {
		try {
			const char *str = self->m_state->internal->getReaderState([self index]).c_str();
			
			if(strstr(str, "EMPTY")) {
				state = EstEIDReaderStateEmpty;
			} else if(strstr(str, "PRESENT")) {
				state = EstEIDReaderStatePresent;
			}
			
			if(strstr(str, "INUSE")) {
				state = (EstEIDReaderState)((int)state | EstEIDReaderStateInUse);
			}
		}
		catch(std::runtime_error err) {
#if DEBUG
			NSLog(@"%@: Couldn't read reader state", NSStringFromClass([self class]));
#endif
		}
	}
	
	return state;
}

- (NSString *)name
{
	if(EstEIDReaderManagerStateIsValid(self->m_state)) {
		try {
			return CPlusStringToNSString(self->m_state->internal->getReaderName([self index]));
		}
		catch(std::runtime_error err) {
			NSLog(@"%@: Couldn't read reader name (reader=%d)", NSStringFromClass([self class]), [self index]);
		}
	}
	
	return nil;
}

- (NSDictionary *)retryCounters
{
	if(EstEIDReaderManagerStateIsValid(self->m_state)) {
		try {
			EstEidCard card(*self->m_state->internal, [self index]);
			byte puk, pin1, pin2;
			
			if(card.getRetryCounts(puk, pin1, pin2)) {
				return [NSDictionary dictionaryWithObjectsAndKeys:
						[NSNumber numberWithUnsignedChar:puk], EstEIDReaderRetryCounterPUK,
						[NSNumber numberWithUnsignedChar:pin1], EstEIDReaderRetryCounterPIN1, 
						[NSNumber numberWithUnsignedChar:pin2], EstEIDReaderRetryCounterPIN2, nil];
			}
		}
		catch(std::runtime_error err) {
			NSLog(@"%@: Couldn't get retry counters (reader=%d)", NSStringFromClass([self class]), [self index]);
		}
	}
	
	return nil;
}

- (NSDictionary *)personData
{
	if(!self->m_personData && EstEIDReaderManagerStateIsValid(self->m_state)) {
		try {
			EstEidCard card(*self->m_state->internal, [self index]);
			vector <std::string> data;
			
			if(card.readPersonalData(data, EstEidCard::SURNAME, EstEidCard::COMMENT4)) {
				NSMutableDictionary *personData = [NSMutableDictionary dictionary];
				
				[personData setValue:CPlusStringToNSString(data[EstEidCard::SURNAME]) forKey:EstEIDReaderPersonDataLastNameKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::FIRSTNAME]) forKey:EstEIDReaderPersonDataFirstNameKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::MIDDLENAME]) forKey:EstEIDReaderPersonDataMiddleNameKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::SEX]) forKey:EstEIDReaderPersonDataSexKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::CITIZEN]) forKey:EstEIDReaderPersonDataCitizenKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::BIRTHDATE]) forKey:EstEIDReaderPersonDataBirthDateKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::ID]) forKey:EstEIDReaderPersonDataIDKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::DOCUMENTID]) forKey:EstEIDReaderPersonDataDocumentIDKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::EXPIRY]) forKey:EstEIDReaderPersonDataExpiryKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::BIRTHPLACE]) forKey:EstEIDReaderPersonDataBirthPlaceKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::ISSUEDATE]) forKey:EstEIDReaderPersonDataIssueDateKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::RESIDENCEPERMIT]) forKey:EstEIDReaderPersonDataResidencePermitKey];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::COMMENT1]) forKey:EstEIDReaderPersonDataComment1Key];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::COMMENT2]) forKey:EstEIDReaderPersonDataComment2Key];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::COMMENT3]) forKey:EstEIDReaderPersonDataComment3Key];
				[personData setValue:CPlusStringToNSString(data[EstEidCard::COMMENT4]) forKey:EstEIDReaderPersonDataComment4Key];
				
				self->m_personData = [personData retain];
			}
		}
		catch(std::runtime_error err) {
			NSLog(@"%@: Couldn't read person data", NSStringFromClass([self class]));
		}
	}
	
	return self->m_personData;
}

- (NSString *)personName
{
	if(!self->m_personName && EstEIDReaderManagerStateIsValid(self->m_state)) {
		NSString *firstName = [self->m_personData objectForKey:EstEIDReaderPersonDataFirstNameKey];
		NSString *lastName = [self->m_personData objectForKey:EstEIDReaderPersonDataLastNameKey];
		
		try {
			if(!firstName || !lastName) {
				EstEidCard card(*(self->m_state->internal), [self index]);
				vector <std::string> data;
				
				if(card.readPersonalData(data, EstEidCard::SURNAME, EstEidCard::FIRSTNAME)) {
					firstName = CPlusStringToNSString(data[EstEidCard::FIRSTNAME]);
					lastName = CPlusStringToNSString(data[EstEidCard::SURNAME]);
				}
			}
			
			if(firstName && lastName) {
				self->m_personName = [[NSString stringWithFormat:@"%@ %@", [firstName capitalizedString], [lastName capitalizedString]] retain];
			}
		}
		catch(std::runtime_error err) {
			NSLog(@"%@: Couldn't read person name", NSStringFromClass([self class]));
		}
		
		
	}
	
	return self->m_personName;
}

- (NSData *)authCertificate
{
	if(!self->m_authCertificate && EstEIDReaderManagerStateIsValid(self->m_state)) {
		try {
			EstEidCard card(*(self->m_state->internal), [self index]);
			
			self->m_authCertificate = [CPlusDataToNSData(card.getAuthCert()) retain];
		}
		catch(std::runtime_error err) {
			NSLog(@"%@: Couldn't read auth certificate", NSStringFromClass([self class]));
		}
	}
	
	return self->m_authCertificate;
}

- (NSData *)signCertificate
{
	if(!self->m_signCertificate && EstEIDReaderManagerStateIsValid(self->m_state)) {
		try {
			EstEidCard card(*(self->m_state->internal), [self index]);
			
			self->m_signCertificate = [CPlusDataToNSData(card.getSignCert()) retain];
		}
		catch(std::runtime_error err) {
			NSLog(@"%@: Couldn't read sign certificate", NSStringFromClass([self class]));
		}
	}
	
	return self->m_signCertificate;
}

- (NSString *)sign:(NSString *)hash pin:(NSString *)pin error:(NSError **)error
{
	if(error) {
		*error = nil;
	}
	
	if(EstEIDReaderManagerStateIsValid(self->m_state)) {
		try {
			EstEidCard card(*(self->m_state->internal), [self index]);
			ByteVec bytes = fromHex([hash UTF8String]);
			std::string cpin = [pin UTF8String];
			
			if(bytes.size() != 20) {
				if(error) {
					*error = [NSError errorWithDomain:EstEIDReaderErrorDomain code:EstEIDReaderErrorInvalidHash userInfo:nil];
				}
				
				return nil;
			}
			
			// FIXME: Remove this stuff
			return CPlusStringToNSString(toHex(card.calcSignSHA1(bytes, EstEidCard::SIGN, PinString(cpin.c_str()))));
		}
		catch(AuthError err) {
			NSLog(@"%@: Couldn't sign hash %@ because of '%s'.", NSStringFromClass([self class]), hash, err.what());
			
			if(err.m_blocked) {
				if(error) {
					*error = [NSError errorWithDomain:EstEIDReaderErrorDomain code:EstEIDReaderErrorLockedPIN userInfo:nil];
				}
			} else {
				if(error) {
					*error = [NSError errorWithDomain:EstEIDReaderErrorDomain code:EstEIDReaderErrorInvalidPIN userInfo:nil];
				}
			}
			
			return nil;
		}
		catch(std::runtime_error err) {
			NSLog(@"%@: Couldn't sign hash %@ because of '%s'", NSStringFromClass([self class]), hash, err.what());
		}
		
		if(error) {
			*error = [NSError errorWithDomain:EstEIDReaderErrorDomain code:EstEIDReaderErrorUnknown userInfo:nil];
		}
	} else {
		if(error) {
			*error = [NSError errorWithDomain:EstEIDReaderErrorDomain code:EstEIDReaderErrorState userInfo:nil];
		}
	}
	
	return nil;
}

#pragma mark NSObject

- (void)dealloc
{
	[self->m_authCertificate release];
	[self->m_signCertificate release];
	[self->m_personData release];
	[self->m_personName release];
	
	[super dealloc];
}

@end

#pragma mark -

@interface EstEIDReaderManagerProxy : NSObject
{
	@private
	EstEIDReaderManager *m_readerManager;
}

- (id)initWithReaderManager:(EstEIDReaderManager *)readerManager;

@end

@implementation EstEIDReaderManagerProxy

- (id)initWithReaderManager:(EstEIDReaderManager *)readerManager
{
	self = [super init];
	
	if(self) {
		self->m_readerManager = readerManager;
	}
	
	return self;
}

- (void)poll:(NSTimer *)timer
{
	[self->m_readerManager performSelector:@selector(poll)];
}

@end

#pragma mark -

@implementation EstEIDReaderManager

- (void)poll
{
	if(EstEIDReaderManagerStateIsValid(self->m_state)) {
		try {
			EstEidCard card = EstEidCard(*(self->m_state->internal));
			unsigned int count = 0;
			unsigned int index;
			BOOL changed;
			
			try {
				count = self->m_state->internal->getReaderCount();
			}
			catch(std::runtime_error &err) {
				// FIXME: Really?
				if(strcmp(err.what(), "No smart card readers") != 0) {
					throw err;
				}
			}
			
			changed = (self->m_state->readers.count != count);
			
			// Added readers
			if(self->m_state->readers.count < count) {
				self->m_state->readers.data = (id <EstEIDReader> *)realloc(self->m_state->readers.data, sizeof(id <EstEIDReader>) * count);
				
				for(index = self->m_state->readers.count; index < count; index++) {
					self->m_state->readers.data[index] = [[EstEIDReader alloc] initWithState:self->m_state];
				}
				
				self->m_state->readers.count = count;
			}
			
			// Check for changes
			for(index = 0; index < self->m_state->readers.count; index++) {
				EstEIDReader *reader = (EstEIDReader *)self->m_state->readers.data[index];
				BOOL connected = (index < count) ? card.isInReader(index) : NO;
				
				if(reader->m_connected != connected) {
					reader->m_connected = connected;
					[reader clearCache];
					
					if(connected) {
						[self->m_delegate readerManager:self didInsertCard:reader];
					} else {
						[self->m_delegate readerManager:self didRemoveCard:reader];
					}
				}
			}
			
			// Removed readers
			if(self->m_state->readers.count > count) {
				for(index = count; index < self->m_state->readers.count; index++) {
					((EstEIDReader *)self->m_state->readers.data[index])->m_state = NULL;
					[self->m_state->readers.data[index] autorelease];
				}
				
				self->m_state->readers.count = count;
				
				if(self->m_state->selection >= self->m_state->readers.count) {
					self->m_state->selection = NSNotFound;
				}
			}
			
			if(changed) {
				[self->m_delegate readerManagerDidChange:self];
			}
		} catch(std::runtime_error &err) {
#if DEBUG
			NSLog(@"%@: %s", NSStringFromClass([self class]), err.what());
#endif
			// Notify delegate here?
		}
	}
}

- (id <EstEIDReaderManagerDelegate>)delegate
{
	return self->m_delegate;
}

- (void)setDelegate:(id <EstEIDReaderManagerDelegate>)delegate
{
	self->m_delegate = delegate;
}

- (id <EstEIDReader>)selectedReader
{
	// Automatic reader selection?
	if(self->m_state->selection == NSNotFound) {
		unsigned int index;
		
		// Try to return the first reader with a card.
		for(index = 0; index < self->m_state->readers.count; index++) {
			if(((EstEIDReader *)self->m_state->readers.data[index])->m_connected) {
				return self->m_state->readers.data[index];
			}
		}
		
		// Just return the first reader. Most users have only one card reader anyway.
		return (self->m_state->readers.count > 0) ? self->m_state->readers.data[0] : nil;
	}
	
	return self->m_state->readers.data[self->m_state->selection];
}

- (void)setSelectedReader:(id <EstEIDReader>)selectedReader
{
	unsigned int index;
	
	for(index = 0; index < self->m_state->readers.count; index++) {
		if(self->m_state->readers.data[index] == selectedReader) {
			self->m_state->selection = index;
			return;
		}
	}
	
	self->m_state->selection = NSNotFound;
}

- (NSArray *)readers
{
	if(self->m_state->readers.count > 0) {
		NSMutableArray *readers = [NSMutableArray array];
		unsigned int index;
		
		for(index = 0; index < self->m_state->readers.count; index++) {
			[readers addObject:self->m_state->readers.data[index]];
		}
		
		return readers;
	}
	
	return nil;
}

- (id <EstEIDReader>)readerAtIndex:(unsigned int)index
{
	return (index >= 0 && index < self->m_state->readers.count) ? self->m_state->readers.data[index] : nil;
}

- (unsigned int)count
{
	return self->m_state->readers.count;
}

#pragma mark NSObject

- (id)init
{
	self = [super init];
	
	if(self) {
		self->m_state = (EstEIDReaderManagerState *)memset(malloc(sizeof(EstEIDReaderManagerState)), 0x00, sizeof(EstEIDReaderManagerState));
		self->m_state->selection = NSNotFound;
		
		try {
			self->m_state->internal = new SmartCardManager();
			self->m_state->proxy = [[EstEIDReaderManagerProxy alloc] initWithReaderManager:self];
			self->m_state->timer = [[NSTimer alloc] initWithFireDate:[NSDate dateWithTimeIntervalSinceNow:0.1F] interval:1.0F target:self->m_state->proxy selector:@selector(poll:) userInfo:nil repeats:YES];
			
			[self poll];
			[[NSRunLoop currentRunLoop] addTimer:self->m_state->timer forMode:NSDefaultRunLoopMode];
		}
		catch(std::runtime_error err) {
			NSLog(@"%@: Couldn't initialize SmartCardManager", NSStringFromClass([self class]));
		}
	}
	
	return self;
}

- (void)dealloc
{
	unsigned int index;
	
	[self->m_state->timer invalidate];
	[self->m_state->timer release];
	[self->m_state->proxy release];
	
	for(index = 0; index < self->m_state->readers.count; index++) {
		((EstEIDReader *)self->m_state->readers.data[index])->m_state = NULL;
		[self->m_state->readers.data[index] release];
	}
	
	delete self->m_state->internal;
	free(self->m_state->readers.data);
	free(self->m_state);
	
	[super dealloc];
}

@end
