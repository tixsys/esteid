/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

@class EstEIDWebPlugIn;

/**
 * The EstEIDWebPlugInProxy class is used to prevent circular references caused by NSTimer objects.
 */
@interface EstEIDWebPlugInProxy : NSObject
{
	@private
	EstEIDWebPlugIn *m_plugin;
}

- (id)initWithPlugIn:(EstEIDWebPlugIn *)plugin;

@end
