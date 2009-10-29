/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>
#import <Security/Security.h>

@interface EstEIDKTIdentityPreference : NSObject
{
	@private
	SecKeychainItemRef m_item;
}

- (id)initWithItem:(SecKeychainItemRef)item;

- (SecKeychainItemRef)item;

@end
