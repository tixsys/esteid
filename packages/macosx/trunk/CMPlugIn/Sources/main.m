/* Copyright (c) 2008 Janek Priimann */

#import <Carbon/Carbon.h>
#import <CoreFoundation/CFPlugInCOM.h>
#import "EstEIDCMPlugIn.h"

static CFTypeRef EstEIDBundleGetValue(CFStringRef key)
{
	NSBundle *bundle = [NSBundle bundleForClass:[EstEIDCMPlugIn class]];
    id value = [[bundle localizedInfoDictionary] objectForKey:(NSString *)key];
    
    if(!value) {
        value = [[bundle infoDictionary] objectForKey:(NSString *)key];
    }
    
	return (CFTypeRef)value;
}

static CFUUIDRef EstEIDBundleCopyUUID()
{
    CFTypeRef types = EstEIDBundleGetValue(CFSTR("CFPlugInTypes"));
    
    if(types != NULL && CFGetTypeID(types) == CFDictionaryGetTypeID()) {
        CFTypeRef cms = CFDictionaryGetValue((CFDictionaryRef)types, CFSTR("2F6522E9-3E66-11D5-80A7-003065B300BC"));
        
        if(cms != NULL && CFGetTypeID(cms) == CFArrayGetTypeID() && CFArrayGetCount((CFArrayRef)cms) > 0) {
            CFTypeRef uuidStr = CFArrayGetValueAtIndex((CFArrayRef)cms, 0);
            
            if(CFGetTypeID(uuidStr) == CFStringGetTypeID()) {
                return CFUUIDCreateFromString(kCFAllocatorDefault, (CFStringRef)uuidStr);
            }
        }
    }
    
    return NULL;
}

static unsigned int EstEIDBundleCopyOptions()
{
    CFTypeRef value = EstEIDBundleGetValue(CFSTR("IsFinderOnly"));
    unsigned int options = 0;
    
    if(value == NULL || (CFGetTypeID(value) == CFBooleanGetTypeID() && CFBooleanGetValue((CFBooleanRef)value))) {
        options = 1;
    }
    
    return options;
}

#pragma mark -

typedef struct _EstEIDCMPlugInContext {
    ContextualMenuInterfaceStruct *interface;
    CFUUIDRef factoryID;
    unsigned int options;
    EventHandlerUPP menuEventHandlerUPP;
    EventHandlerRef menuEventHandler;
    EstEIDCMPlugIn *plugin;
} EstEIDCMPlugInContext, *EstEIDCMPlugInContextRef;

static CFArrayRef EstEIDCMPlugInContextCopySelection(const AEDesc *inContext)
{
    CFMutableArrayRef selection = CFArrayCreateMutable(kCFAllocatorDefault, 0, &kCFTypeArrayCallBacks);
    OSStatus result;
    long i, c;
    
    if(inContext != NULL && (result = AECountItems(inContext, &c)) == noErr) {
        for(i = 1; i <= c; i++) {
            AEKeyword theAEKeyword;
            AEDesc theAEDesc;
            
            if((result = AEGetNthDesc(inContext, i, typeAlias, &theAEKeyword, &theAEDesc)) == noErr) {
                if(theAEDesc.descriptorType == typeAlias) {
                    Size dataSize = AEGetDescDataSize(&theAEDesc);
                    AliasHandle tAliasHdl = (AliasHandle)NewHandle(dataSize);
                    
                    if(tAliasHdl) {
                        Boolean wasChanged;
                        FSRef tFSRef;
                        
                        if((result = AEGetDescData(&theAEDesc, *tAliasHdl, dataSize)) == noErr &&
                           (result = FSResolveAlias(NULL, tAliasHdl, &tFSRef, &wasChanged)) == noErr) {
                            CFURLRef url = CFURLCreateFromFSRef(kCFAllocatorDefault, &tFSRef);
                            
                            if(url) {
                                CFArrayAppendValue(selection, url);
                                CFRelease(url);
                            }
                        }
                        
                        DisposeHandle((Handle)tAliasHdl);
                    }
                }
                
                AEDisposeDesc(&theAEDesc);
            }
        }
    }
    
    return selection;
}

static pascal OSStatus EstEIDCMPlugInContextHandleMenuEvent(EventHandlerCallRef inEventHandlerCallRef, EventRef inEventRef, void *inUserData)
{
    return eventNotHandledErr;
}

static HRESULT EstEIDCMPlugInContextQueryInterface(EstEIDCMPlugInContextRef ctx, REFIID iid, LPVOID *ppv)
{
    CFUUIDRef interfaceID = CFUUIDCreateFromUUIDBytes(NULL, iid);
    HRESULT result = E_NOINTERFACE;
    
    *ppv = NULL;
    
    if(CFEqual(interfaceID, kContextualMenuInterfaceID) || CFEqual(interfaceID, IUnknownUUID)) {
        [ctx->plugin retain];
        *ppv = ctx;
        result = S_OK;
    }
    
    CFRelease(interfaceID);
    
    return result;
}

static ULONG EstEIDCMPlugInContextRetain(EstEIDCMPlugInContextRef ctx)
{
    return [[ctx->plugin retain] retainCount];
}

static ULONG EstEIDCMPlugInContextRelease(EstEIDCMPlugInContextRef ctx)
{
    int refc = [ctx->plugin retainCount];
    
    [ctx->plugin release];
    
    if(refc == 0) {
        CFRelease(ctx->factoryID);
        free(ctx);
        
        return 0;
    }
    
    return refc - 1;
}

static OSStatus EstEIDCMPlugInContextBuildMenuItem(CFStringRef title, SInt32 tag, MenuItemAttributes inAttributes, AEDesc *submenu, AEDescList *output)
{
    AERecord record = { typeNull, NULL };
    OSStatus result;
    
    if((result = AECreateList(NULL, 0, TRUE, &record)) == noErr) {
        CFRetain(title);
        result = AEPutKeyPtr(&record, keyContextualMenuName, typeCFStringRef, &title, sizeof(CFStringRef));
        result = (submenu != NULL) ? AEPutKeyDesc(&record, keyContextualMenuSubmenu, submenu) : AEPutKeyPtr(&record, keyContextualMenuCommandID, typeSInt32, &tag, sizeof(tag));
        result = AEPutDesc(output, 0, &record);
        AEDisposeDesc(&record);
    }
    
    return result;
}

static void EstEIDCMPlugInContextBuildMenu(EstEIDCMPlugInContextRef ctx, NSMenu *menu, AEDescList *output)
{
    int i, c;
    
    for(i = 0, c = [menu numberOfItems]; i < c; i++) {
        NSMenuItem *item = [menu itemAtIndex:i];
        
        if([item isSeparatorItem]) {
            EstEIDCMPlugInContextBuildMenuItem(CFSTR("-"), 0, kMenuItemAttrSeparator, NULL, output);
        } else {
            NSMenu *submenu = [item submenu];
            
            if(submenu) {
                AEDesc desc = { typeNull, NULL };
                OSStatus result;
                
                if((result = AECreateList(NULL, 0, FALSE, &desc)) == noErr) {
                    EstEIDCMPlugInContextBuildMenu(ctx, submenu, &desc);
                    EstEIDCMPlugInContextBuildMenuItem((CFStringRef)[item title], 0, kMenuItemAttrDynamic, &desc, output);
                    AEDisposeDesc(&desc);
                }
            } else {
                EstEIDCMPlugInContextBuildMenuItem((CFStringRef)[item title], [item tag], ([item isEnabled]) ? kMenuItemAttrDynamic : kMenuItemAttrDisabled, NULL, output);
            }
        }
    }
}

static OSStatus EstEIDCMPlugInContextExamineContext(EstEIDCMPlugInContextRef ctx, const AEDesc *inContext, AEDescList *outCommandPairs)
{
    const EventTypeSpec menuEvents[] = { { kEventClassMenu, kEventMenuPopulate }, };
    OSStatus result = noErr;
    
    if(ctx->options) {
        ProcessSerialNumber tPSN = { 0, kCurrentProcess };
        ProcessInfoRec tPIR;
        
        tPIR.processInfoLength = sizeof(ProcessInfoRec);
        tPIR.processName = nil;
        tPIR.processAppSpec = nil;
        
        if((result = GetProcessInformation(&tPSN, &tPIR)) == noErr) {
            if(tPIR.processSignature != 'MACS' || tPIR.processType != 'FNDR') {
                return noErr;
            }
        } else {
            fprintf(stderr, "EstEIDCMPlugInContextExamineContext(): GetProcessInformation returned %ld.", result);
            fflush(stderr);
            result = noErr;
        }
    }
    
    if(!ctx->menuEventHandlerUPP) {
        ctx->menuEventHandlerUPP = NewEventHandlerUPP(EstEIDCMPlugInContextHandleMenuEvent);
    }
    
    InstallApplicationEventHandler(ctx->menuEventHandlerUPP, GetEventTypeCount(menuEvents), menuEvents, NULL, &(ctx->menuEventHandler));
    
    if(Gestalt(gestaltContextualMenuAttr, &result) == noErr) {
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        CFArrayRef selection = EstEIDCMPlugInContextCopySelection(inContext);
        
        EstEIDCMPlugInContextBuildMenu(ctx, [ctx->plugin contextMenu:(NSMutableArray *)selection], outCommandPairs);
        CFRelease(selection);
        
        [pool release];
    }
    
    return noErr;
}

static OSStatus EstEIDCMPlugInContextHandleSelection(EstEIDCMPlugInContextRef ctx, AEDesc *inContext, SInt32 inCommandID)
{
    NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
    CFArrayRef selection = EstEIDCMPlugInContextCopySelection(inContext);
    
    [ctx->plugin contextMenu:(NSArray *)selection handleCommand:inCommandID];
    
    CFRelease(selection);
    [pool release];
    
    return noErr;
}

static void EstEIDCMPlugInContextPostMenuCleanup(EstEIDCMPlugInContextRef ctx)
{
    if(ctx->menuEventHandler) {
        RemoveEventHandler(ctx->menuEventHandler);
        ctx->menuEventHandler = NULL;
    }
    
    if(ctx->menuEventHandlerUPP) {
        DisposeEventHandlerUPP(ctx->menuEventHandlerUPP);
        ctx->menuEventHandlerUPP = NULL;
    }
}

static ContextualMenuInterfaceStruct gEstEIDCMPlugInContextInterface =
{
    NULL, 
    
    (SInt32(*)(void *, CFUUIDBytes, void **))&EstEIDCMPlugInContextQueryInterface, 
    (UInt32(*)(void *))&EstEIDCMPlugInContextRetain, 
    (UInt32(*)(void *))&EstEIDCMPlugInContextRelease, 
    
    (OSStatus(*)(void *, const AEDesc *, AEDescList *))&EstEIDCMPlugInContextExamineContext, 
    (OSStatus(*)(void *, AEDesc *, SInt32))&EstEIDCMPlugInContextHandleSelection, 
    (void(*)(void *))&EstEIDCMPlugInContextPostMenuCleanup
};
            
void *EstEIDCMPlugInContextFactory(CFAllocatorRef allocator, CFUUIDRef typeID)
{
    if(CFEqual(typeID, kContextualMenuTypeID)) {
        EstEIDCMPlugInContextRef ctx = malloc(sizeof(EstEIDCMPlugInContext));
        NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
        
        ctx->interface = &gEstEIDCMPlugInContextInterface;
        ctx->factoryID = EstEIDBundleCopyUUID();
        ctx->options = EstEIDBundleCopyOptions();
        ctx->menuEventHandler = NULL;
        ctx->menuEventHandlerUPP = NULL;
        ctx->plugin = [[EstEIDCMPlugIn alloc] init];
        
        CFPlugInAddInstanceForFactory(ctx->factoryID);
        
        [pool release];
        
        return ctx;
    }
    
    return NULL;
}
