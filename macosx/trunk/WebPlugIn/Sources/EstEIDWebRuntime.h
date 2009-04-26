/* Copyright (c) 2009 Janek Priimann */

#import <WebKit/npapi.h>
#import <WebKit/npfunctions.h>
#import <WebKit/npruntime.h>

extern NPNetscapeFuncs *browser;

void EstEIDWebRuntimeObjectToVariant(NPP npp, id object, NPVariant *variant);
void EstEIDWebRuntimeVariantToObject(NPP npp, const NPVariant *variant, id *object);
bool EstEIDWebRuntimeMsgSend(NPP npp, id object, SEL selector, const NPVariant *args, int argc, NPVariant *result);
