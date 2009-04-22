/* Copyright (c) 2008 Janek Priimann */

#import <WebKit/npapi.h>
#import <WebKit/npfunctions.h>
#import <WebKit/npruntime.h>
#import "EstEIDWebObject.h"
#import "EstEIDWebPlugIn.h"

@interface EstEIDWebNPObject : EstEIDWebObject
{
	@private
	NPP m_npp;
	NPObject *m_object;
}

- (id)initWithObject:(NPObject *)object npp:(NPP)npp;

- (NPObject *)object;

@end

#pragma mark -

NPNetscapeFuncs *browser;

typedef struct _EstEIDWebObjectContext {
    NPClass *_class;
    uint32_t referenceCount;
    NPP npp;
    NPWindow *window;
    EstEIDWebObject *obj;
} EstEIDWebObjectContext, *EstEIDWebObjectContextRef;

static NPObject *EstEIDWebObjectContextCreate(NPP npp, EstEIDWebObject *obj);

static void EstEIDWebObjectContextVariantToObject(NPP npp, const NPVariant *variant, id *object)
{
	*object = nil;
	
	switch(variant->type) {
		case NPVariantType_Void:
		case NPVariantType_Null:
			*object = [NSNull null];
			break;
		case NPVariantType_Bool:
			*object = [NSNumber numberWithBool:variant->value.boolValue];
			break;
		case NPVariantType_Int32:
			*object = [NSNumber numberWithInt:variant->value.intValue];
			break;
		case NPVariantType_Double:
			*object = [NSNumber numberWithDouble:variant->value.doubleValue];
			break;
		case NPVariantType_String:
			*object = [[[NSString alloc] initWithBytes:variant->value.stringValue.UTF8Characters length:variant->value.stringValue.UTF8Length encoding:NSUTF8StringEncoding] autorelease];
			break;
		case NPVariantType_Object:
			*object = [[[EstEIDWebNPObject alloc] initWithObject:variant->value.objectValue npp:npp] autorelease];
			break;
	}
}

static void EstEIDWebObjectContextObjectToVariant(NPP npp, id object, NPVariant *variant)
{
	if(object == nil || [object isKindOfClass:[NSNull class]]) {
		variant->type = NPVariantType_Null;
	} else if([object isKindOfClass:[NSString class]]) {
		variant->type = NPVariantType_String;
		variant->value.stringValue.UTF8Length = [object lengthOfBytesUsingEncoding:NSUTF8StringEncoding];
		variant->value.stringValue.UTF8Characters = (const NPUTF8 *)memcpy(malloc(sizeof(UInt8) * variant->value.stringValue.UTF8Length), [object UTF8String], sizeof(UInt8) * variant->value.stringValue.UTF8Length);
	} else if([object isKindOfClass:[NSNumber class]]) {
		variant->type = NPVariantType_Int32;
		variant->value.intValue = [(NSNumber *)object intValue];
	} else if([object isKindOfClass:[EstEIDWebNPObject class]]) {
		variant->type = NPVariantType_Object;
		variant->value.objectValue = [(EstEIDWebNPObject *)object object];
		browser->retainobject(variant->value.objectValue);
	} else if([object isKindOfClass:[EstEIDWebObject class]]) {
		variant->type = NPVariantType_Object;
		variant->value.objectValue = EstEIDWebObjectContextCreate(npp, object);
	} else {
		variant->type = NPVariantType_Void;
	}
}

// Magic happens here.
static void EstEIDWebObjectContextMsgSend(EstEIDWebObjectContextRef ctx, SEL selector, const NPVariant *args, int argc, NPVariant *result)
{
	if(selector) {
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSMutableArray *args_objc = [[NSMutableArray alloc] init];
		id result_objc;
		int argi;
		
		for(argi = 0; argi < argc; argi++) {
			id object = nil;
			
			EstEIDWebObjectContextVariantToObject(ctx->npp, args + argi, &object);
			[args_objc addObject:(object) ? object : [NSNull null]];
		}
		
		result_objc = [ctx->obj	performSelector:selector withObject:args_objc];
		
		if(result) {
			EstEIDWebObjectContextObjectToVariant(ctx->npp, result_objc, result);
		}
		
		[args_objc release];
		[pool release];
	} else {
		result->type = NPVariantType_Void;
	}
}

static bool EstEIDWebObjectContextHasProperty(EstEIDWebObjectContextRef ctx, NPIdentifier identifier)
{	
    bool result = false;
	
	if(ctx->obj) {
		NPUTF8 *str = browser->utf8fromidentifier(identifier);
		
		result = [[ctx->obj class] selectorForGetProperty:(const char *)str];
		
		free(str);
	}
	
	return result;
}

static bool EstEIDWebObjectContextHasMethod(EstEIDWebObjectContextRef ctx, NPIdentifier identifier)
{
	bool result = false;
	
	if(ctx->obj) {
		NPUTF8 *str = browser->utf8fromidentifier(identifier);
		
		result = [[ctx->obj class] selectorForMethod:(const char *)str];
		
		free(str);
	}
	
	return result;
}

static void EstEIDWebObjectContextGetProperty(EstEIDWebObjectContextRef ctx, NPIdentifier identifier, NPVariant *result)
{
    if(ctx->obj) {
		NPUTF8 *str = browser->utf8fromidentifier(identifier);
		
		EstEIDWebObjectContextMsgSend(ctx, [[ctx->obj class] selectorForGetProperty:(const char *)str], nil, 0, result);
		
		free(str);
	} else {
		result->type = NPVariantType_Void;
	}
}

static void EstEIDWebObjectContextSetProperty(EstEIDWebObjectContextRef ctx, NPIdentifier identifier, const NPVariant *value)
{
    if(ctx->obj) {
		NPUTF8 *str = browser->utf8fromidentifier(identifier);
		
		EstEIDWebObjectContextMsgSend(ctx, [[ctx->obj class] selectorForSetProperty:(const char *)str], value, (value) ? 1 : 0, nil);
		
		free(str);
	}
}

static void EstEIDWebObjectContextInvoke(EstEIDWebObjectContextRef ctx, NPIdentifier identifier, NPVariant *args, unsigned argc, NPVariant *result)
{
	if(ctx->obj) {
		NPUTF8 *str = browser->utf8fromidentifier(identifier);
		
		EstEIDWebObjectContextMsgSend(ctx, [[ctx->obj class] selectorForMethod:(const char *)str], args, argc, result);
		
		free(str);
	} else {
		result->type = NPVariantType_Void;
	}
}

static void EstEIDWebObjectContextInvokeDefault(EstEIDWebObjectContextRef ctx, NPVariant *args, unsigned argCount, NPVariant *result)
{
    result->type = NPVariantType_Void;
}

static void EstEIDWebObjectContextInvalidate()
{
}

static NPObject *EstEIDWebObjectContextAllocate(NPP npp, NPClass *klass)
{
    EstEIDWebObjectContextRef ctx = malloc(sizeof(EstEIDWebObjectContext));
    
    ctx->obj = nil;
    ctx->npp = npp;
    
    return (NPObject *)ctx;
}

static void EstEIDWebObjectContextDeallocate(EstEIDWebObjectContextRef ctx) 
{
    if(ctx->obj != nil) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        [ctx->obj release];
        ctx->obj = nil;
        
        [pool release];
    }
    
    free((void *)ctx);
}

static NPClass *EstEIDWebObjectContextGetClass(void)
{
    static NPClass interface = { 
        NP_CLASS_STRUCT_VERSION,
        (NPAllocateFunctionPtr)EstEIDWebObjectContextAllocate, 
        (NPDeallocateFunctionPtr)EstEIDWebObjectContextDeallocate, 
        (NPInvalidateFunctionPtr)EstEIDWebObjectContextInvalidate,
        (NPHasMethodFunctionPtr)EstEIDWebObjectContextHasMethod,
        (NPInvokeFunctionPtr)EstEIDWebObjectContextInvoke,
        (NPInvokeDefaultFunctionPtr)EstEIDWebObjectContextInvokeDefault,
        (NPHasPropertyFunctionPtr)EstEIDWebObjectContextHasProperty,
        (NPGetPropertyFunctionPtr)EstEIDWebObjectContextGetProperty,
        (NPSetPropertyFunctionPtr)EstEIDWebObjectContextSetProperty,
    };
    
    return &interface;
}

static NPObject *EstEIDWebObjectContextCreate(NPP npp, EstEIDWebObject *obj)
{
    EstEIDWebObjectContextRef ctx = (EstEIDWebObjectContextRef)browser->createobject(npp, EstEIDWebObjectContextGetClass());
    
    if(ctx != NULL && obj != nil) {
        ctx->obj = [obj retain];
    }
    
    return (NPObject *)ctx;
}

#pragma mark -

@implementation EstEIDWebObject

+ (SEL)selectorForMethod:(const char *)name
{
	return NULL;
}

+ (SEL)selectorForGetProperty:(const char *)name
{
	return NULL;
}

+ (SEL)selectorForSetProperty:(const char *)name
{
	return NULL;
}

- (id)invokeMethod:(NSString *)name withArguments:(NSArray *)arguments
{
	SEL selector = [[self class] selectorForMethod:[name UTF8String]];
	
	return (selector) ? [self performSelector:selector withObject:arguments] : nil;
}

@end

#pragma mark -

@implementation EstEIDWebNPObject

- (id)initWithObject:(NPObject *)object npp:(NPP)npp
{
	self = [super init];
	
	if(self) {
		self->m_npp = npp;
		self->m_object = object;
		browser->retainobject(self->m_object);
	}
	
	return self;
}

- (NPObject *)object
{
	return self->m_object;
}

#pragma mark EstEIDWebObject

- (id)invokeMethod:(NSString *)name withArguments:(NSArray *)arguments
{
	uint32_t argi, argc = [arguments count];
	NPVariant *args = malloc(sizeof(NPVariant) * argc);
	NPVariant result;
	
	for(argi = 0; argi < argc; argi++) {
		EstEIDWebObjectContextObjectToVariant(self->m_npp, [arguments objectAtIndex:argi], args + argi);
	}
	
	// FIXME: Add a NSError parameter to method arguments?
	if((name && browser->invoke(self->m_npp, self->m_object, browser->getstringidentifier((NPUTF8 *)[name UTF8String]), args, argc, &result)) ||
	   browser->invokeDefault(self->m_npp, self->m_object, args, argc, &result)) {
		id object;
		
		EstEIDWebObjectContextVariantToObject(self->m_npp, &result, &object);
		browser->releasevariantvalue(&result);
		
		return object;
	}
	
	return nil;
}

#pragma mark NSObject

- (unsigned)hash
{
	return (unsigned)self->m_object;
}

- (BOOL)isEqual:(EstEIDWebNPObject *)object
{
	return (self == object || self->m_object == object->m_object) ? YES : NO;
}

- (void)dealloc
{
	browser->releaseobject(self->m_object);
	
	[super dealloc];
}

@end

#pragma mark -

typedef void (*FunctionPointer)(void);
typedef void (*TransitionVector)(void);

static FunctionPointer functionPointerForTVector(TransitionVector tvp)
{
    const uint32 temp[6] = { 0x3D800000, 0x618C0000, 0x800C0000, 0x804C0004, 0x7C0903A6, 0x4E800420 };
    uint32 *newGlue = NULL;
    
    if(tvp != NULL) {
        newGlue = (uint32 *)malloc(sizeof(temp));
        
        if(newGlue != NULL) {
            unsigned i;
            
            for(i = 0; i < 6; i++) newGlue[i] = temp[i];
            
            newGlue[0] |= ((UInt32)tvp >> 16);
            newGlue[1] |= ((UInt32)tvp & 0xFFFF);
            
            MakeDataExecutable(newGlue, sizeof(temp));
        }
    }
    
    return (FunctionPointer)newGlue;
}

static TransitionVector tVectorForFunctionPointer(FunctionPointer fp)
{
    FunctionPointer *newGlue = NULL;
    
    if(fp != NULL) {
        newGlue = (FunctionPointer *)malloc(2 * sizeof(FunctionPointer));
        
        if(newGlue != NULL) {
            newGlue[0] = fp;
            newGlue[1] = NULL;
        }
    }
    
    return (TransitionVector)newGlue;
}

NPNetscapeFuncs *browser;

#pragma export on
// Mach-o entry points
NPError NP_Initialize(NPNetscapeFuncs *browserFuncs);
NPError NP_GetEntryPoints(NPPluginFuncs *pluginFuncs);
void NP_Shutdown(void);
// For compatibility with CFM browsers.
int main(NPNetscapeFuncs *browserFuncs, NPPluginFuncs *pluginFuncs, NPP_ShutdownProcPtr *shutdown);
#pragma export off

NPError NPP_New(NPMIMEType pluginType, NPP instance, uint16 mode, int16 argc, char *argn[], char *argv[], NPSavedData *saved)
{
    if(browser->version >= 14) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        EstEIDWebPlugIn *plugin = [[EstEIDWebPlugIn alloc] init];
        
        instance->pdata = EstEIDWebObjectContextCreate(instance, plugin);
        [plugin release];
        
        [pool release];
    }
    
    return NPERR_NO_ERROR;
}

NPError NPP_Destroy(NPP instance, NPSavedData **save)
{
    EstEIDWebObjectContextRef obj = instance->pdata;
    
    // Do nothing if browser didn't support NPN_CreateObject which would have created the EstEIDWebObjectContext.
    if(obj != NULL) {
        // release?
    }
    
    return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(NPP instance, NPWindow *window)
{
    EstEIDWebObjectContextRef obj = instance->pdata;
    
    // Do nothing if browser didn't support NPN_CreateObject which would have created the EstEIDWebObjectContext.
    if(obj != NULL) {
        obj->window = window;
    }
    
    return NPERR_NO_ERROR;
}


NPError NPP_NewStream(NPP instance, NPMIMEType type, NPStream *stream, NPBool seekable, uint16 *stype)
{
    *stype = NP_ASFILEONLY;
    
    return NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(NPP instance, NPStream *stream, NPReason reason)
{
    return NPERR_NO_ERROR;
}

int32 NPP_WriteReady(NPP instance, NPStream *stream)
{
    return 0;
}

int32 NPP_Write(NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer)
{
    return 0;
}

void NPP_StreamAsFile(NPP instance, NPStream *stream, const char *fname)
{
    EstEIDWebObjectContextRef obj = instance->pdata;
    
    // Do nothing if browser didn't support NPN_CreateObject which would have created the EstEIDWebObjectContextRef.
    if(obj != NULL) {
		/*
		 MovieObject *movieObject = obj->movieObject;
		 
		 DestroyMovie(movieObject);
		 
		 if(!LoadMovieFromFile(&movieObject->movie, fname)) {
		 return;
		 }
		 
		 if(!CreateMovieController(movieObject, obj->window)) {
		 return;
		 }
		 
		 PlayMovie(movieObject);*/
    }
}

void NPP_Print(NPP instance, NPPrint *platformPrint)
{
	
}

int16 NPP_HandleEvent(NPP instance, void *event)
{
    EstEIDWebObjectContextRef obj = instance->pdata;
    
    // Do nothing if browser didn't support NPN_CreateObject which would have created the EstEIDWebObjectContext.
    if(obj != NULL) {
        return 0;//HandleMovieEvent(obj->movieObject, (EventRecord *)event);
    }
    
    return 0;
}

void NPP_URLNotify(NPP instance, const char *url, NPReason reason, void *notifyData)
{
}

NPError NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
    if(variable == NPPVpluginScriptableNPObject) {
        void **v = (void **)value;
        
		if(instance->pdata) {
			const char *uagent = browser->uagent(instance);
			
			// Without this check we'll be leaking memory before 10.5
			if(!(uagent != NULL && (uagent = strstr(uagent, " AppleWebKit/")) != NULL && atoi(uagent + 13) < 420)) {
				browser->retainobject((NPObject*)instance->pdata);
			}
		}
		
        *v = instance->pdata;
        
        return NPERR_NO_ERROR;
    }
    
    return NPERR_GENERIC_ERROR;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
    return NPERR_GENERIC_ERROR;
}

// Mach-o entry points
NPError NP_Initialize(NPNetscapeFuncs *browserFuncs)
{
    browser = browserFuncs;
    
    return NPERR_NO_ERROR;
}

NPError NP_GetEntryPoints(NPPluginFuncs *pluginFuncs)
{
    pluginFuncs->version = 11;
    pluginFuncs->size = sizeof(pluginFuncs);
    pluginFuncs->newp = NPP_New;
    pluginFuncs->destroy = NPP_Destroy;
    pluginFuncs->setwindow = NPP_SetWindow;
    pluginFuncs->newstream = NPP_NewStream;
    pluginFuncs->destroystream = NPP_DestroyStream;
    pluginFuncs->asfile = NPP_StreamAsFile;
    pluginFuncs->writeready = NPP_WriteReady;
    pluginFuncs->write = (NPP_WriteProcPtr)NPP_Write;
    pluginFuncs->print = NPP_Print;
    pluginFuncs->event = NPP_HandleEvent;
    pluginFuncs->urlnotify = NPP_URLNotify;
    pluginFuncs->getvalue = NPP_GetValue;
    pluginFuncs->setvalue = NPP_SetValue;
    
    return NPERR_NO_ERROR;
}

void NP_Shutdown(void)
{
}

// For compatibility with CFM browsers.
int main(NPNetscapeFuncs *browserFuncs, NPPluginFuncs *pluginFuncs, NPP_ShutdownProcPtr *shutdown)
{
    browser = memset(malloc(sizeof(NPNetscapeFuncs)), 0x00, sizeof(NPNetscapeFuncs));
    
    browser->size = browserFuncs->size;
    browser->version = browserFuncs->version;
    
    browser->geturl = (NPN_GetURLProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->geturl);
    browser->posturl = (NPN_PostURLProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->posturl);
    browser->requestread = (NPN_RequestReadProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->requestread);
    browser->newstream = (NPN_NewStreamProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->newstream);
    browser->write = (NPN_WriteProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->write);
    browser->destroystream = (NPN_DestroyStreamProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->destroystream);
    browser->status = (NPN_StatusProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->status);
    browser->uagent = (NPN_UserAgentProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->uagent);
    browser->memalloc = (NPN_MemAllocProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->memalloc);
    browser->memfree = (NPN_MemFreeProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->memfree);
    browser->memflush = (NPN_MemFlushProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->memflush);
    browser->reloadplugins = (NPN_ReloadPluginsProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->reloadplugins);
    browser->geturlnotify = (NPN_GetURLNotifyProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->geturlnotify);
    browser->posturlnotify = (NPN_PostURLNotifyProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->posturlnotify);
    browser->getvalue = (NPN_GetValueProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->getvalue);
    browser->setvalue = (NPN_SetValueProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->setvalue);
    browser->invalidaterect = (NPN_InvalidateRectProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->invalidaterect);
    browser->invalidateregion = (NPN_InvalidateRegionProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->invalidateregion);
    browser->forceredraw = (NPN_ForceRedrawProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->forceredraw);
    browser->getJavaEnv = (NPN_GetJavaEnvProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->getJavaEnv);
    browser->getJavaPeer = (NPN_GetJavaPeerProcPtr)functionPointerForTVector((TransitionVector)browserFuncs->getJavaPeer);
    
    pluginFuncs->version = 11;
    pluginFuncs->size = sizeof(pluginFuncs);
    pluginFuncs->newp = (NPP_NewProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_New);
    pluginFuncs->destroy = (NPP_DestroyProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_Destroy);
    pluginFuncs->setwindow = (NPP_SetWindowProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_SetWindow);
    pluginFuncs->newstream = (NPP_NewStreamProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_NewStream);
    pluginFuncs->destroystream = (NPP_DestroyStreamProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_DestroyStream);
    pluginFuncs->asfile = (NPP_StreamAsFileProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_StreamAsFile);
    pluginFuncs->writeready = (NPP_WriteReadyProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_WriteReady);
    pluginFuncs->write = (NPP_WriteProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_Write);
    pluginFuncs->print = (NPP_PrintProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_Print);
    pluginFuncs->event = (NPP_HandleEventProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_HandleEvent);
    pluginFuncs->urlnotify = (NPP_URLNotifyProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_URLNotify);
    pluginFuncs->getvalue = (NPP_GetValueProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_GetValue);
    pluginFuncs->setvalue = (NPP_SetValueProcPtr)tVectorForFunctionPointer((FunctionPointer)NPP_SetValue);
    
    *shutdown = (NPP_ShutdownProcPtr)tVectorForFunctionPointer((FunctionPointer)NP_Shutdown);
    
    return NPERR_NO_ERROR;
}
