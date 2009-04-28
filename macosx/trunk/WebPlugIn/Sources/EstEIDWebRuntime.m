/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDWebObject.h"
#import "EstEIDWebRuntime.h"
#import <AppKit/AppKit.h>
#import <Carbon/Carbon.h>
#import <WebKit/WebScriptObject.h>

NPNetscapeFuncs *browser = NULL;

@interface EstEIDWebNPObject : EstEIDWebObject
{
	@private
	NPP m_npp;
	NPObject *m_object;
}

- (id)initWithObject:(NPObject *)object npp:(NPP)npp;

- (NPObject *)object;

@end

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

- (BOOL)invokeMethod:(NSString *)name withArguments:(NSArray *)arguments result:(id *)result
{
	uint32_t argi, argc = [arguments count];
	NPVariant *args = malloc(sizeof(NPVariant) * argc);
	NPVariant variant;
	
	for(argi = 0; argi < argc; argi++) {
		EstEIDWebRuntimeObjectToVariant(self->m_npp, [arguments objectAtIndex:argi], args + argi);
	}
	
	if((name) ? browser->invoke(self->m_npp, self->m_object, browser->getstringidentifier((NPUTF8 *)[name UTF8String]), args, argc, &variant) : browser->invokeDefault(self->m_npp, self->m_object, args, argc, &variant)) {
		if(result) {
			EstEIDWebRuntimeVariantToObject(self->m_npp, &variant, result);
		}
		
		browser->releasevariantvalue(&variant);
		
		return YES;
	}
	
	return NO;
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

typedef struct _EstEIDWebObjectContext {
    NPClass *_class;
    uint32_t referenceCount;
    NPP npp;
    NPWindow *window;
    EstEIDWebObject *obj;
} EstEIDWebObjectContext, *EstEIDWebObjectContextRef;

static bool EstEIDWebObjectContextHasProperty(EstEIDWebObjectContextRef ctx, NPIdentifier identifier)
{	
    bool result = false;
	
	if(ctx->obj) {
		NPUTF8 *str = browser->utf8fromidentifier(identifier);
		
		result = [[ctx->obj class] selectorForProperty:(const char *)str];
		
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

static bool EstEIDWebObjectContextGetProperty(EstEIDWebObjectContextRef ctx, NPIdentifier identifier, NPVariant *result)
{
    if(ctx->obj) {
		NPUTF8 *str = browser->utf8fromidentifier(identifier);
		
		return EstEIDWebRuntimeMsgSend(ctx->npp, ctx->obj, [[ctx->obj class] selectorForProperty:(const char *)str], nil, 0, result);
		
		free(str);
		
		return true;
	} else {
		result->type = NPVariantType_Void;
	}
	
	return false;
}

static bool EstEIDWebObjectContextSetProperty(EstEIDWebObjectContextRef ctx, NPIdentifier identifier, const NPVariant *value)
{
    if(ctx->obj) {
		NPUTF8 *str = browser->utf8fromidentifier(identifier);
		unsigned int length = strlen(str);
		char buffer[length + 2];
		
		strcpy(buffer, (const char *)str);
		buffer[length + 0] = '=';
		buffer[length + 1] = 0x00;
		
		EstEIDWebRuntimeMsgSend(ctx->npp, ctx->obj, [[ctx->obj class] selectorForProperty:buffer], value, (value) ? 1 : 0, nil);
		
		free(str);
		
		return true;
	}
	
	return false;
}

static bool EstEIDWebObjectContextInvoke(EstEIDWebObjectContextRef ctx, NPIdentifier identifier, NPVariant *args, unsigned argc, NPVariant *result)
{
	if(ctx->obj) {
		NPUTF8 *str = browser->utf8fromidentifier(identifier);
		
		EstEIDWebRuntimeMsgSend(ctx->npp, ctx->obj, [[ctx->obj class] selectorForMethod:(const char *)str], args, argc, result);
		
		free(str);
		
		return true;
	} else {
		result->type = NPVariantType_Void;
	}
	
	return false;
}

static bool EstEIDWebObjectContextInvokeDefault(EstEIDWebObjectContextRef ctx, NPVariant *args, unsigned argCount, NPVariant *result)
{
    result->type = NPVariantType_Void;
	
	return false;
}

static void EstEIDWebObjectContextInvalidate()
{
}

static NPObject *EstEIDWebObjectContextAllocate(NPP npp, NPClass *klass)
{
    EstEIDWebObjectContextRef ctx = malloc(sizeof(EstEIDWebObjectContext));
	
    ctx->obj = nil;
    ctx->npp = npp;
    
#if DEBUG
	NSLog(@"EstEIDWebObjectContextAllocate (address=%X)", ctx);
#endif
	
    return (NPObject *)ctx;
}

static void EstEIDWebObjectContextDeallocate(EstEIDWebObjectContextRef ctx) 
{
#if DEBUG
	NSLog(@"EstEIDWebObjectContextDeallocate (address=%X)", ctx);
#endif
	
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
        (NPSetPropertyFunctionPtr)EstEIDWebObjectContextSetProperty
    };
    
    return &interface;
}

static NPObject *EstEIDWebObjectContextCreate(NPP plugin, EstEIDWebObject *obj)
{
    EstEIDWebObjectContextRef context = (EstEIDWebObjectContextRef)browser->createobject((NPP)plugin, EstEIDWebObjectContextGetClass());
    
    if(context != NULL && obj != nil) {
        context->obj = [obj retain];
    }
    
    return (NPObject *)context;
}

#pragma mark -

void EstEIDWebRuntimeVariantToObject(NPP npp, const NPVariant *variant, id *object)
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

void EstEIDWebRuntimeObjectToVariant(NPP npp, id object, NPVariant *variant)
{
	if(object == nil) {
		variant->type = NPVariantType_Void;
	} else if([object isKindOfClass:[NSNull class]]) {
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
	} else if([object isKindOfClass:[NSDate class]]) {
		NSDateFormatter *dateFormatter = [[NSDateFormatter alloc] init];
		
		[dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
		EstEIDWebRuntimeObjectToVariant(npp, [dateFormatter stringFromDate:(NSDate *)object], variant);
		[dateFormatter release];
	} else {
		NSLog(@"EstEID: The %@ class instance couldn't be converted.", [object class]);
		variant->type = NPVariantType_Void;
	}
}

// Magic happens here.
bool EstEIDWebRuntimeMsgSend(NPP npp, id self, SEL selector, const NPVariant *args, int argc, NPVariant *result)
{
	if(selector) {
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		NSMutableArray *arguments = [[NSMutableArray alloc] init];
		id object;
		int argi;
		
		for(argi = 0; argi < argc; argi++) {
			EstEIDWebRuntimeVariantToObject(npp, args + argi, &object);
			[arguments addObject:(object) ? object : [NSNull null]];
		}
		
		@try {
			NSMethodSignature *signature = [self methodSignatureForSelector:selector];
			NSInvocation *invocation = [NSInvocation invocationWithMethodSignature:signature];
			
			[invocation setTarget:self];
			[invocation setSelector:selector];
			
			if([signature numberOfArguments] == 3) {
				[invocation setArgument:&arguments atIndex:2];
			}
			
			[invocation invoke];
			
			if([signature methodReturnLength] == sizeof(id)) {
				[invocation getReturnValue:&object];
			} else {
				object = nil;
			}
		}
		@catch(NSException *e) {
			object = e;
		}
		@catch(NSString *e) {
			object = [NSException exceptionWithName:e reason:nil userInfo:nil];
		}
		
		if(object && [object isKindOfClass:[NSException class]]) {
			if(result) {
				result->type = NPVariantType_Void;
			}
			
			// Weird...?
			[WebScriptObject throwException:[(NSException *)object name]];
			//browser->setexception((NPObject *)npp->pdata, [(NSException *)object name]);
		} else {
			if(result) {
				EstEIDWebRuntimeObjectToVariant(npp, object, result);
			}
		}
		
		[arguments release];
		[pool release];
		
		return true;
	} else {
		if(result) {
			result->type = NPVariantType_Void;
		}
	}
	
	return false;
}

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

#pragma mark -

#pragma export on
// Mach-o entry points
NPError NP_Initialize(NPNetscapeFuncs *browserFuncs);
NPError NP_GetEntryPoints(NPPluginFuncs *pluginFuncs);
void NP_Shutdown(void);
// For compatibility with CFM browsers.
int main(NPNetscapeFuncs *browserFuncs, NPPluginFuncs *pluginFuncs, NPP_ShutdownProcPtr *shutdown);
#pragma export off

NPError NPP_New(NPMIMEType pluginType, NPP plugin, uint16 mode, int16 argc, char *argn[], char *argv[], NPSavedData *saved)
{
    if(browser->version >= 14) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		Class class = NSClassFromString([[[NSBundle bundleForClass:[EstEIDWebObject class]] infoDictionary] objectForKey:@"NSPrincipalClass"]);
		EstEIDWebObject *object = ([(NSObject *)class conformsToProtocol:@protocol(EstEIDWebService)]) ? [[(id)class alloc] init] : nil;
		NPObject *value = NULL;
		
        plugin->pdata = EstEIDWebObjectContextCreate(plugin, object);
		
		// The style attribute is ignored by Safari, so manually set width and height to 0 in order to make it invisible.
		if(browser->getvalue(plugin, NPNVPluginElementNPObject, (void *)&value) == NPERR_NO_ERROR) {
			NPVariant variant;
			
			variant.type = NPVariantType_Int32;
			variant.value.intValue = 0;
			
			browser->setproperty(plugin, (NPObject *)value, browser->getstringidentifier((NPUTF8 *)"width"), &variant);
			browser->setproperty(plugin, (NPObject *)value, browser->getstringidentifier((NPUTF8 *)"height"), &variant);
			browser->releaseobject((NPObject *)value);
		}
		
		if(browser->getvalue(plugin, NPNVWindowNPObject, (void *)&value) == NPERR_NO_ERROR) {
			NPVariant variant;
			
			if(browser->getproperty(plugin, value, browser->getstringidentifier("location"), &variant)) {
				NPVariant location;
				
				if(browser->getproperty(plugin, variant.value.objectValue, browser->getstringidentifier("href"), &location)) {
					if(location.type == NPVariantType_String) {
						NSString *result = nil;
						
						EstEIDWebRuntimeVariantToObject(plugin, &location, &result);
						[(id <EstEIDWebService>)object setURL:(result) ? [NSURL URLWithString:result] : nil];
					}
					
					browser->releasevariantvalue(&location);
				}
				
				browser->releasevariantvalue(&variant);
			}
		}
		
        [object release];
        [pool release];
    }
    
    return NPERR_NO_ERROR;
}

NPError NPP_Destroy(NPP plugin, NPSavedData **save)
{
    EstEIDWebObjectContextRef obj = plugin->pdata;
	
    if(obj != NULL) {
		browser->releaseobject((NPObject *)plugin->pdata);
    }
    
    return NPERR_NO_ERROR;
}

NPError NPP_SetWindow(NPP plugin, NPWindow *window)
{
    EstEIDWebObjectContextRef obj = plugin->pdata;
    
    if(obj != NULL) {
        obj->window = window;
		
		if(window && window->type == NPWindowTypeWindow) {
			NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
			
			[(id <EstEIDWebService>)obj->obj setWindow:[[[NSWindow alloc] initWithWindowRef:GetWindowFromPort(((NP_Port *)window->window)->port)] autorelease]];
			
			[pool release];
		}
    }
    
    return NPERR_NO_ERROR;
}


NPError NPP_NewStream(NPP plugin, NPMIMEType type, NPStream *stream, NPBool seekable, uint16 *stype)
{
    *stype = NP_ASFILEONLY;
    
    return NPERR_NO_ERROR;
}

NPError NPP_DestroyStream(NPP plugin, NPStream *stream, NPReason reason)
{
    return NPERR_NO_ERROR;
}

int32 NPP_WriteReady(NPP plugin, NPStream *stream)
{
    return 0;
}

int32 NPP_Write(NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer)
{
    return 0;
}

void NPP_StreamAsFile(NPP instance, NPStream *stream, const char *fname)
{
}

void NPP_Print(NPP instance, NPPrint *platformPrint)
{
	
}

int16 NPP_HandleEvent(NPP instance, void *event)
{
    EstEIDWebObjectContextRef obj = instance->pdata;
    
    if(obj != NULL) {
        return 0;
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
				browser->retainobject((NPObject *)instance->pdata);
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
