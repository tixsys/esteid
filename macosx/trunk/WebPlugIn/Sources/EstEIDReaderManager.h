/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

typedef enum _EstEIDReaderState {
	EstEIDReaderStateUnknown = 0x00,
	EstEIDReaderStateEmpty = 0x01,
	EstEIDReaderStatePresent = 0x02,
	EstEIDReaderStateInUse = 0x10
} EstEIDReaderState;

typedef enum _EstEIDReaderError {
	EstEIDReaderErrorUnknown = 0,
	EstEIDReaderErrorInvalidPIN,
	EstEIDReaderErrorInvalidHash,
	EstEIDReaderErrorLockedPIN,
	EstEIDReaderErrorState
} EstEIDReaderError;

extern NSString *EstEIDReaderErrorDomain;

extern NSString *EstEIDReaderPersonDataLastNameKey;
extern NSString *EstEIDReaderPersonDataFirstNameKey;
extern NSString *EstEIDReaderPersonDataMiddleNameKey;
extern NSString *EstEIDReaderPersonDataSexKey;
extern NSString *EstEIDReaderPersonDataCitizenKey;
extern NSString *EstEIDReaderPersonDataBirthDateKey;
extern NSString *EstEIDReaderPersonDataIDKey;
extern NSString *EstEIDReaderPersonDataDocumentIDKey;
extern NSString *EstEIDReaderPersonDataExpiryKey;
extern NSString *EstEIDReaderPersonDataBirthPlaceKey;
extern NSString *EstEIDReaderPersonDataIssueDateKey;
extern NSString *EstEIDReaderPersonDataResidencePermitKey;
extern NSString *EstEIDReaderPersonDataComment1Key;
extern NSString *EstEIDReaderPersonDataComment2Key;
extern NSString *EstEIDReaderPersonDataComment3Key;
extern NSString *EstEIDReaderPersonDataComment4Key;

extern NSString *EstEIDReaderRetryCounterPUK;
extern NSString *EstEIDReaderRetryCounterPIN1;
extern NSString *EstEIDReaderRetryCounterPIN2;

/**
 * The EstEIDReader protocol groups methods that are essential for card readers.
 */
@protocol EstEIDReader <NSObject>

/**
 * @name Accessing Reader Data
 */

/**
 * Returns YES if the reader is empty; NO otherwise.
 * @return YES if the reader is empty; NO otherwise.
 */
- (BOOL)isEmpty;

/**
 * Returns the index of the receiver in the reader manager.
 * @return The index of the receiver.
 */
- (unsigned int)index;

/**
 * Returns the name of the receiver eg "CardMan OmniKey 1020 00 00".
 * @return The name of the receiver.
 */
- (NSString *)name;

/**
 * Returns The state of the receiver. You can use this method for example to check if data is being read from the receiver.
 * @return The state of the receiver.
 */
- (EstEIDReaderState)state;

/**
 * Returns the retry counters from the card. All the fields are included.
 * @return The retry counters;
 */
- (NSDictionary *)retryCounters;

/**
 * Returns the personal information from the card. All the fields are included.
 * @return The personal information.
 */
- (NSDictionary *)personData;

/**
 * Returns fully capitalized name of the card holder eg "Mari-Liis Mannik".
 * @return The name of the card holder.
 */
- (NSString *)personName;

/**
 * Returns the raw data of the authentification certificate in x509 format.
 * @return The authentification certificate or nil.
 */
- (NSData *)authCertificate;

/**
 * Returns the raw data of the signature certificate in x509 format.
 * @return The signature certificate or nil.
 */
- (NSData *)signCertificate;

/**
 * Returns the signed hash or nil if there was an error.
 * @param hash The hash to be signed.
 * @param pin The pin to be used during signing.
 * @param error The error that is set and returned if the signing failed.
 * @return The signature or nil.
 */
- (NSString *)sign:(NSString *)hash pin:(NSString *)pin error:(NSError **)error;

@end

@class EstEIDReaderManager;

/**
 * The EstEIDReaderManagerDelegate protocol allows to get notified of EstEIDReaderManager changes as they happen.
 */
@protocol EstEIDReaderManagerDelegate <NSObject>

/**
 * @name Card Events
 */

/**
 * Informs the delegate that a new card was inserted into one of the readers.
 * @param readerManager The reader manager sending this information.
 * @param reader The reader where the card was inserted.
 */
- (void)readerManager:(EstEIDReaderManager *)readerManager didInsertCard:(id <EstEIDReader>)reader;

/**
 * Informs the delegate that a previously added card was removed from one of the readers.
 * @param readerManager The reader manager sending this information.
 * @param reader The reader from the card was removed.
 */
- (void)readerManager:(EstEIDReaderManager *)readerManager didRemoveCard:(id <EstEIDReader>)reader;

/**
 * Informs the delegate that the number of readers has changed.
 * @param readerManager The reader manager sending this information.
 */
- (void)readerManagerDidChange:(EstEIDReaderManager *)readerManager;

@end

/**
 * The EstEIDReaderManager class is monitors all the card readers that available in the system.
 */
@interface EstEIDReaderManager : NSObject
{
	@private
	id <EstEIDReaderManagerDelegate> m_delegate;
	struct _EstEIDReaderManagerState *m_state;
}

/**
 * @name Managing the Delegate
 */

/**
 * Returns the object that acts as the delegate of the receiver and implements the EstEIDReaderManagerDelegate protocol.
 * @return The delegate.
 */
- (id <EstEIDReaderManagerDelegate>)delegate;

/**
 * Set the object that acts as the delegate of the receiver and implements the EstEIDReaderManagerDelegate protocol.
 * @param delegate The delegate.
 */
- (void)setDelegate:(id <EstEIDReaderManagerDelegate>)delegate;

/**
 * @name Managing the Selected Reader
 */

/**
 * Returns the selected reader or nil if there are no readers available.
 * @return The selected reader.
 */
- (id <EstEIDReader>)selectedReader;

/**
 * Changes the selected reader. Can be nil.
 * @param selectedReader The selected reader.
 */
- (void)setSelectedReader:(id <EstEIDReader>)selectedReader;

/**
 * @name Managing Readers
 */

/**
 * Returns an array containing the receiver's readers.
 * @return A new array containing the receiver's readers, or nil if the receiver has no readers.
 */
- (NSArray *)readers;

/**
 * Returns the reader located at index.
 * @param index An index within the bounds of the receiver.
 * @return The reader located at index or nil if the index is out of bounds.
 */
- (id <EstEIDReader>)readerAtIndex:(unsigned int)index;

/**
 * Returns the number of readers currently in the receiver.
 * @return The number of readers currently in the receiver.
 */
- (unsigned int)count;

@end
