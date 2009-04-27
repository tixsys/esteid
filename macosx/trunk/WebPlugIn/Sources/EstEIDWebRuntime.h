/* Copyright (c) 2009 Janek Priimann */

#import <WebKit/npapi.h>
#import <WebKit/npfunctions.h>
#import <WebKit/npruntime.h>

extern NPNetscapeFuncs *browser;

void EstEIDWebRuntimeObjectToVariant(NPP plugin, id object, NPVariant *variant);
void EstEIDWebRuntimeVariantToObject(NPP plugin, const NPVariant *variant, id *object);
bool EstEIDWebRuntimeMsgSend(NPP plugin, id object, SEL selector, const NPVariant *args, int argc, NPVariant *result);
