/* Copyright (c) 2009 Janek Priimann */

#import "EstEIDPINPanel.h"

#define kEstEIDPINPanelCommandGetDocument 'Evdo'
#define kEstEIDPINPanelControlIDNameKey 'enak'

const ControlID kEstEIDPINPanelControlIDImageView = { 'eimg', 0 };
//const ControlID kEstEIDPINPanelControlIDNameKey = { 'enak', 0 };
const ControlID kEstEIDPINPanelControlIDNameValue = { 'enav', 0 };
//const ControlID kEstEIDPINPanelControlIDHashKey = { 'ehak', 0 };
const ControlID kEstEIDPINPanelControlIDHashValue = { 'ehav', 0 };
//const ControlID kEstEIDPINPanelControlIDPINKey = { 'epik', 0 };
const ControlID kEstEIDPINPanelControlIDPINValue = { 'epiv', 0 };
//const ControlID kEstEIDPINPanelControlIDWebsiteKey = { 'ewsk', 0 };
const ControlID kEstEIDPINPanelControlIDWebsiteValue = { 'ewsv', 0 };

static OSStatus EstEIDPINPanelEventHandler(EventHandlerCallRef inHandlerCallRef, EventRef inEvent, void *userData)
{
	EstEIDPINPanel *self = (EstEIDPINPanel *)userData;
	UInt32 eventClass = GetEventClass(inEvent);
	UInt32 eventKind = GetEventKind(inEvent);
	OSStatus err = noErr;
	
	if(eventClass == kEventClassCommand && eventKind == kEventProcessCommand) {
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		HICommand hiCommand;
		
		err = GetEventParameter(inEvent, kEventParamDirectObject, typeHICommand, NULL, sizeof(HICommand), NULL, &hiCommand);
		
		switch(hiCommand.commandID) {
			case kHICommandOK:
				NSLog(@"OK!");
				
				break;
			case kHICommandCancel:
				[self orderOut:nil];
				break;
			case kEstEIDPINPanelCommandGetDocument:
				NSLog(@"Get document");
				break;
			default:
				err = eventNotHandledErr;
				break;
		}
		
		[pool release];
	} else if(eventClass == kEventClassWindow && eventKind == kEventWindowBoundsChanged) {
		// Do nothing at the moment
		NSLog(@"Bounds changed!");
	}
	
	return err;
}

DEFINE_ONE_SHOT_HANDLER_GETTER(EstEIDPINPanelEventHandler);

#pragma mark -

@implementation EstEIDPINPanel

- (NSString *)hash
{
	return nil;
}

- (void)setHash:(NSString *)hash
{
	ControlRef control;
	
	if(GetControlByID(self->m_window, &kEstEIDPINPanelControlIDHashValue, &control) == noErr) {
		HIViewSetText(control, (hash) ? (CFStringRef)hash : CFSTR(""));
	}
}

- (NSString *)name
{
	return nil;
}

- (void)setName:(NSString *)name
{
	ControlRef control;
	
	if(GetControlByID(self->m_window, &kEstEIDPINPanelControlIDNameValue, &control) == noErr) {
		HIViewSetText(control, (name) ? (CFStringRef)name : CFSTR(""));
	}
}

- (NSString *)PIN
{
	return nil;
}

- (void)setPIN:(NSString *)PIN
{
	ControlRef control;
	
	if(GetControlByID(self->m_window, &kEstEIDPINPanelControlIDPINValue, &control) == noErr) {
		HIViewSetText(control, (PIN) ? (CFStringRef)PIN : CFSTR(""));
	}
}

- (NSString *)title
{
	return nil;
}

- (void)setTitle:(NSString *)title
{
	SetWindowTitleWithCFString(self->m_window, (title) ? (CFStringRef)title : CFSTR(""));
}

- (NSString *)website
{
	return nil;
}

- (void)setWebsite:(NSString *)website
{
	ControlRef control;
	
	if(GetControlByID(self->m_window, &kEstEIDPINPanelControlIDWebsiteValue, &control) == noErr) {
		HIViewSetText(control, (website) ? (CFStringRef)website : CFSTR(""));
	}
}

- (WindowRef)windowRef
{
	return self->m_window;
}

- (BOOL)isVisible
{
	return (IsWindowVisible(self->m_window)) ? YES : NO;
}

- (void)runModal
{
	EventTargetRef theTarget = GetEventDispatcherTarget();
	EventRef theEvent;
	
	while(IsWindowVisible(self->m_window) && ReceiveNextEvent(0, NULL, kEventDurationForever, true, &theEvent) == noErr) {
		SendEventToEventTarget(theEvent, theTarget);
		ReleaseEvent(theEvent);
	}
}

- (IBAction)orderOut:(id)sender
{
	HideWindow(self->m_window);
}

- (IBAction)orderFront:(id)sender
{
	ShowWindow(self->m_window);
}

#pragma mark NSObject

- (id)init
{
	WindowRef window = NULL;
	IBNibRef nib = NULL;
	
	if(CreateNibReferenceWithCFBundle(CFBundleGetBundleWithIdentifier(CFSTR("org.esteid.webplugin")), CFSTR("PINPanel"), &nib) == noErr) {
		CreateWindowFromNib(nib, CFSTR("Panel"), &window);		
		DisposeNibReference(nib);
	}
	
	if(!window) {
		[self release];
		self = nil;
	}
	
	if(self) {//, { kEventClassWindow, kEventWindowBoundsChanged }
		EventTypeSpec windowEventList[] = { { kEventClassCommand, kEventProcessCommand } };
		NSBundle *bundle = [NSBundle bundleForClass:[self class]];
		ControlRef control;
		
		self->m_window = window;
		
		if(GetControlByID(self->m_window, &kEstEIDPINPanelControlIDImageView, &control) == noErr) {
			NSString *path = [bundle pathForResource:@"Icon" ofType:@"png"];
			
			if(path) {
				CGDataProviderRef dataProvider = CGDataProviderCreateWithFilename([path fileSystemRepresentation]);
				
				if(dataProvider) {
					CGImageRef image = CGImageCreateWithPNGDataProvider(dataProvider, NULL, NO, kCGRenderingIntentDefault);
					
					if(image) {
						HIImageViewSetImage(control, image);
						HIViewSetNeedsDisplay(control, TRUE);
						CGImageRelease(image);
					}
					
					CGDataProviderRelease(dataProvider);
				}
			}
		}
		
		// TODO: Localize UI here
		
		if(InstallWindowEventHandler(window, GetEstEIDPINPanelEventHandlerUPP(), GetEventTypeCount(windowEventList), windowEventList, self, NULL) != noErr) {
			[self release];
			self = nil;
		}
	}
	
	return self;
}

- (void)dealloc
{
	if(self->m_window) {
		ReleaseWindow(self->m_window);
	}
	
	[super dealloc];
}

@end
