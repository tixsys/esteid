/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

@class EstEIDWebObject;

/**
 * The EstEIDWebObjectProxy class is used to prevent circular references caused by NSTimer objects.
 */
@interface EstEIDWebObjectProxy : NSObject
{
	@private
	EstEIDWebObject *m_object;
}

- (id)initWithObject:(EstEIDWebObject *)object;

- (void)invalidate:(NSTimer *)timer;

@end
