/* Copyright (c) 2009 Janek Priimann */

#import <Foundation/Foundation.h>

@interface NSTask(Additions)

+ (NSString *)execute:(NSString *)command withArguments:(NSArray *)arguments;

@end
