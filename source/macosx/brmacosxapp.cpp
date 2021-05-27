/***************************************

    MacOSX application manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brgameapp.h"
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "brdisplay.h"
#include "brfilemanager.h"
#include "brfilename.h"
#include "brkeyboard.h"
#include "brmouse.h"
#include "broscursor.h"
#include "brstringfunctions.h"
#include "brtick.h"
#import <AppKit/NSApplication.h>
#include <AppKit/NSApplicationScripting.h>
#import <AppKit/NSEvent.h>
#include <AppKit/NSMenu.h>
#import <AppKit/NSScreen.h>
#import <AppKit/NSWindow.h>
#import <ApplicationServices/ApplicationServices.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSNotification.h>

#if !defined(DOXYGEN)

extern void FixNSRectOrigin(NSRect* pInput);

@class BurgerWindowListener;

/***************************************

	Extend the NSApplication so events can be intercepted

***************************************/

@interface BurgerApplication : NSApplication {
	Burger::GameApp* m_App; ///< Parent app
}
- (void)terminate:(id)sender;
@end

@implementation BurgerApplication

//
// Capture the exit code
//

- (void)terminate:(id)sender
{
#pragma unused(sender)
	// Alert burgerlib to shut down the app
	m_App->SetQuitCode();
}

//
// Initialize the application to have the game pointer
// Note: Since the app is initialized with sharedApplication
// initWithGameApp is not an option to set the value on startup
//

- (void)setGameApp:(Burger::GameApp*)pGameApp
{
	m_App = pGameApp;
}

@end

/***************************************

	Create an NSApplicationDelegate to capture window activate/deactivate events

***************************************/

@interface BurgerApplicationDelegate : NSObject {
	Burger::GameApp* m_App;
	uint_t m_bStarted;
}
- (id)initWithGameApp:(Burger::GameApp*)pGameApp;
@end

//
// Initialize
//

@implementation BurgerApplicationDelegate : NSObject

//
// Initialization function
// Track the activate window event.
// Further reading
// https://developer.apple.com/library/mac/documentation/Cocoa/Reference/ApplicationKit/Classes/NSApplication_Class/Reference/Reference.html#jumpTo_185
//

- (id)initWithGameApp:(Burger::GameApp*)pGameApp
{
	self = [super init];
	if (self) {
		m_bStarted = FALSE;
		m_App = pGameApp;
		[[NSNotificationCenter defaultCenter]
			addObserver:self
			   selector:@selector(focusSomeWindow:)
				   name:NSApplicationDidBecomeActiveNotification
				 object:nil];
	}
	return self;
}

//
// Destructor function
//

- (void)dealloc
{
	// Remove my observer
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	[super dealloc];
}

//
// When a NSApplicationDidBecomeActiveNotification is triggered, this private
// function will be called
//

- (void)focusSomeWindow:(NSNotification*)aNotification
{
#pragma unused(aNotification)

	// The moment a window is created, there's a race condition where if may be
	// visible before being made invisible. To avoid this, ignore the first time
	// this is called

	if (!m_bStarted) {
		m_bStarted = TRUE;
	} else {
		m_App->FocusWindow();
	}
}

@end

/***************************************

	Create an NSWindow which will be manipulated to suit the game's needs

***************************************/

@interface BurgerWindow : NSWindow
- (BOOL)canBecomeKeyWindow;
- (BOOL)canBecomeMainWindow;
- (void)sendEvent:(NSEvent*)event;
@end

@implementation BurgerWindow : NSWindow

//
// Burgerlib windows can always be a key window
//

- (BOOL)canBecomeKeyWindow
{
	return YES;
}

//
// Burgerlib windows can always be a main window
//
- (BOOL)canBecomeMainWindow
{
	return YES;
}

//
// Eat all events that are sent to the window
//

- (void)sendEvent:(NSEvent*)event
{
#if 0
    // Send up the food chain
    [super sendEvent:event];
    id pDelegate = [self delegate];
    if ([pDelegate isKindOfClass:[BurgerWindowListener class]]) {
        // Was the window moving?
        if ([pDelegate isMoving]) {
            // See if it's stopped
            [pDelegate windowDidFinishMoving];
        }
    }
#else
	BURGER_UNUSED(event);
#endif
}
@end

/***************************************

	Create an NSResponder which will track events that the window is interested
	in

***************************************/

enum ePendingOperation {
	PENDING_NULL,
	PENDING_GOFULLSCREEN,
	PENDING_EXITFULLSCREEN,
	PENDING_MINIMIZE,
	PENDING_MAXIMIZE
};

#if defined(MAC_OS_X_VERSION_10_6)

static uint_t s_moveHack;


@interface BurgerWindowListener : NSResponder <NSWindowDelegate> {
	Burger::GameApp* m_pParent;
	uint8_t observingVisible;
	uint8_t wasCtrlLeft;
	uint_t wasVisible;
	uint_t isFullscreenSpace;
	uint8_t inFullscreenTransition;
	ePendingOperation pendingWindowOperation;
	uint8_t isMoving;
	int pendingWindowWarpX;
	int pendingWindowWarpY;
}

- (void)listen:(Burger::GameApp*)data;
- (void)pauseVisibleObservation;
- (void)resumeVisibleObservation;
- (BOOL)setFullscreenSpace:(BOOL)state;
- (BOOL)isInFullscreenSpace;
- (BOOL)isInFullscreenSpaceTransition;
- (void)addPendingWindowOperation:(ePendingOperation)operation;
- (void)close;

- (BOOL)isMoving;
- (void)setPendingMoveX:(int)x Y:(int)y;
- (void)windowDidFinishMoving;

/* Window delegate functionality */
- (BOOL)windowShouldClose:(id)sender;
- (void)windowDidExpose:(NSNotification*)aNotification;
- (void)windowDidMove:(NSNotification*)aNotification;
- (void)windowDidResize:(NSNotification*)aNotification;
- (void)windowDidMiniaturize:(NSNotification*)aNotification;
- (void)windowDidDeminiaturize:(NSNotification*)aNotification;
- (void)windowDidBecomeKey:(NSNotification*)aNotification;
- (void)windowDidResignKey:(NSNotification*)aNotification;
- (void)windowWillEnterFullScreen:(NSNotification*)aNotification;
- (void)windowDidEnterFullScreen:(NSNotification*)aNotification;
- (void)windowWillExitFullScreen:(NSNotification*)aNotification;
- (void)windowDidExitFullScreen:(NSNotification*)aNotification;
- (NSApplicationPresentationOptions)window:(NSWindow*)window
	  willUseFullScreenPresentationOptions:
		  (NSApplicationPresentationOptions)proposedOptions;

/* Window event handling */
- (void)mouseDown:(NSEvent*)theEvent;
- (void)rightMouseDown:(NSEvent*)theEvent;
- (void)otherMouseDown:(NSEvent*)theEvent;
- (void)mouseUp:(NSEvent*)theEvent;
- (void)rightMouseUp:(NSEvent*)theEvent;
- (void)otherMouseUp:(NSEvent*)theEvent;
- (void)mouseMoved:(NSEvent*)theEvent;
- (void)mouseDragged:(NSEvent*)theEvent;
- (void)rightMouseDragged:(NSEvent*)theEvent;
- (void)otherMouseDragged:(NSEvent*)theEvent;
- (void)scrollWheel:(NSEvent*)theEvent;
- (void)touchesBeganWithEvent:(NSEvent*)theEvent;
- (void)touchesMovedWithEvent:(NSEvent*)theEvent;
- (void)touchesEndedWithEvent:(NSEvent*)theEvent;
- (void)touchesCancelledWithEvent:(NSEvent*)theEvent;

/* Touch event handling */
typedef enum {
	COCOA_TOUCH_DOWN,
	COCOA_TOUCH_UP,
	COCOA_TOUCH_MOVE,
	COCOA_TOUCH_CANCELLED
} cocoaTouchType;
- (void)handleTouches:(cocoaTouchType)type withEvent:(NSEvent*)event;

@end

@implementation BurgerWindowListener

- (void)listen:(Burger::GameApp*)data
{
	m_pParent = data;
	NSNotificationCenter* center;
	NSWindow* window = data->GetWindow();
	NSView* view = [window contentView];

	observingVisible = YES;
	wasCtrlLeft = NO;
	wasVisible = [window isVisible];
	isFullscreenSpace = NO;
	inFullscreenTransition = NO;
	pendingWindowOperation = PENDING_NULL;
	isMoving = NO;

	center = [NSNotificationCenter defaultCenter];

	if ([window delegate] != nil) {
		[center addObserver:self
				   selector:@selector(windowDidExpose:)
					   name:NSWindowDidExposeNotification
					 object:window];
		[center addObserver:self
				   selector:@selector(windowDidMove:)
					   name:NSWindowDidMoveNotification
					 object:window];
		[center addObserver:self
				   selector:@selector(windowDidResize:)
					   name:NSWindowDidResizeNotification
					 object:window];
		[center addObserver:self
				   selector:@selector(windowDidMiniaturize:)
					   name:NSWindowDidMiniaturizeNotification
					 object:window];
		[center addObserver:self
				   selector:@selector(windowDidDeminiaturize:)
					   name:NSWindowDidDeminiaturizeNotification
					 object:window];
		[center addObserver:self
				   selector:@selector(windowDidBecomeKey:)
					   name:NSWindowDidBecomeKeyNotification
					 object:window];
		[center addObserver:self
				   selector:@selector(windowDidResignKey:)
					   name:NSWindowDidResignKeyNotification
					 object:window];
		[center addObserver:self
				   selector:@selector(windowWillEnterFullScreen:)
					   name:NSWindowWillEnterFullScreenNotification
					 object:window];
		[center addObserver:self
				   selector:@selector(windowDidEnterFullScreen:)
					   name:NSWindowDidEnterFullScreenNotification
					 object:window];
		[center addObserver:self
				   selector:@selector(windowWillExitFullScreen:)
					   name:NSWindowWillExitFullScreenNotification
					 object:window];
		[center addObserver:self
				   selector:@selector(windowDidExitFullScreen:)
					   name:NSWindowDidExitFullScreenNotification
					 object:window];
	} else {
		[window setDelegate:self];
	}

	/* Haven't found a delegate / notification that triggers when the window is
	 * ordered out (is not visible any more). You can be ordered out without
	 * minimizing, so DidMiniaturize doesn't work. (e.g. -[NSWindow orderOut:])
	 */
	[window addObserver:self
			 forKeyPath:@"visible"
				options:NSKeyValueObservingOptionNew
				context:NULL];

	[window setNextResponder:self];
	[window setAcceptsMouseMovedEvents:YES];

	[view setNextResponder:self];

	if ([view respondsToSelector:@selector(setAcceptsTouchEvents:)]) {
		[view setAcceptsTouchEvents:YES];
	}
}

- (void)observeValueForKeyPath:(NSString*)keyPath
					  ofObject:(id)object
						change:(NSDictionary*)change
					   context:(void*)context
{
	if (!observingVisible) {
		return;
	}

	if (object == m_pParent->GetWindow() &&
		[keyPath isEqualToString:@"visible"]) {
		int newVisibility = [[change objectForKey:@"new"] intValue];
		if (newVisibility) {
			// SDL_SendWindowEvent(m_pParent->GetWindow(),
			// SDL_WINDOWEVENT_SHOWN, 0, 0);
		} else {
			// SDL_SendWindowEvent(m_pParent->GetWindow(),
			// SDL_WINDOWEVENT_HIDDEN, 0, 0);
		}
	}
}

- (void)pauseVisibleObservation
{
	observingVisible = NO;
	wasVisible = [m_pParent->GetWindow() isVisible];
}

- (void)resumeVisibleObservation
{
	BOOL isVisible = [m_pParent->GetWindow() isVisible];
	observingVisible = YES;
	if (wasVisible != isVisible) {
		if (isVisible) {
			// SDL_SendWindowEvent(m_pParent->GetWindow(),
			// SDL_WINDOWEVENT_SHOWN, 0, 0);
		} else {
			// SDL_SendWindowEvent(m_pParent->GetWindow(),
			// SDL_WINDOWEVENT_HIDDEN, 0, 0);
		}

		wasVisible = isVisible;
	}
}

- (BOOL)setFullscreenSpace:(BOOL)state
{
	NSWindow* nswindow = m_pParent->GetWindow();
	Burger::Display* videodata = m_pParent->GetDisplay();

	if (state && (videodata->GetFlags() & Burger::Display::FULLSCREEN)) {
		return NO; /* we only allow you to make a Space on FULLSCREEN_DESKTOP
					  windows. */
	} else if (state == isFullscreenSpace) {
		return YES; /* already there. */
	}

	if (inFullscreenTransition) {
		if (state) {
			[self addPendingWindowOperation:PENDING_GOFULLSCREEN];
		} else {
			[self addPendingWindowOperation:PENDING_EXITFULLSCREEN];
		}
		return YES;
	}
	inFullscreenTransition = YES;

	/* you need to be FullScreenPrimary, or toggleFullScreen doesn't work. Unset
	 * it again in windowDidExitFullScreen. */
	[nswindow
		setCollectionBehavior:NSWindowCollectionBehaviorFullScreenPrimary];
	[nswindow performSelectorOnMainThread:@selector(toggleFullScreen:)
							   withObject:nswindow
							waitUntilDone:NO];
	return YES;
}

- (BOOL)isInFullscreenSpace
{
	return isFullscreenSpace;
}

- (BOOL)isInFullscreenSpaceTransition
{
	return inFullscreenTransition;
}

- (void)addPendingWindowOperation:(ePendingOperation)operation
{
	pendingWindowOperation = operation;
}

- (void)close
{
	NSNotificationCenter* center;
	NSWindow* window = m_pParent->GetWindow();
	NSView* view = [window contentView];
	NSArray* windows = nil;

	center = [NSNotificationCenter defaultCenter];

	if ([window delegate] != self) {
		[center removeObserver:self
						  name:NSWindowDidExposeNotification
						object:window];
		[center removeObserver:self
						  name:NSWindowDidMoveNotification
						object:window];
		[center removeObserver:self
						  name:NSWindowDidResizeNotification
						object:window];
		[center removeObserver:self
						  name:NSWindowDidMiniaturizeNotification
						object:window];
		[center removeObserver:self
						  name:NSWindowDidDeminiaturizeNotification
						object:window];
		[center removeObserver:self
						  name:NSWindowDidBecomeKeyNotification
						object:window];
		[center removeObserver:self
						  name:NSWindowDidResignKeyNotification
						object:window];
		[center removeObserver:self
						  name:NSWindowWillEnterFullScreenNotification
						object:window];
		[center removeObserver:self
						  name:NSWindowDidEnterFullScreenNotification
						object:window];
		[center removeObserver:self
						  name:NSWindowWillExitFullScreenNotification
						object:window];
		[center removeObserver:self
						  name:NSWindowDidExitFullScreenNotification
						object:window];
	} else {
		[window setDelegate:nil];
	}

	[window removeObserver:self forKeyPath:@"visible"];

	if ([window nextResponder] == self) {
		[window setNextResponder:nil];
	}
	if ([view nextResponder] == self) {
		[view setNextResponder:nil];
	}

	/* Make the next window in the z-order Key. If we weren't the foreground
	 when closed, this is a no-op.
	 !!! FIXME: Note that this is a hack, and there are corner cases where
	 !!! FIXME:  this fails (such as the About box). The typical nib+RunLoop
	 !!! FIXME:  handles this for Cocoa apps, but we bypass all that in SDL.
	 !!! FIXME:  We should remove this code when we find a better way to
	 !!! FIXME:  have the system do this for us. See discussion in
	 !!! FIXME:   http://bugzilla.libsdl.org/show_bug.cgi?id=1825
	 */
	windows = [NSApp orderedWindows];
	for (NSWindow* win in windows) {
		if (win == window) {
			continue;
		}

		[win makeKeyAndOrderFront:self];
		break;
	}
}

- (BOOL)isMoving
{
	return isMoving;
}

- (void)setPendingMoveX:(int)x Y:(int)y
{
	pendingWindowWarpX = x;
	pendingWindowWarpY = y;
}

- (void)windowDidFinishMoving
{
	if ([self isMoving]) {
		isMoving = NO;

		// SDL_Mouse *mouse = SDL_GetMouse();
		if (pendingWindowWarpX >= 0 && pendingWindowWarpY >= 0) {
			// mouse->WarpMouse(_data->window, pendingWindowWarpX,
			// pendingWindowWarpY);
			pendingWindowWarpX = pendingWindowWarpY = -1;
		}
		// if (mouse->relative_mode && SDL_GetMouseFocus() ==
		// _data->window) {
		//	mouse->SetRelativeMouseMode(SDL_TRUE);
		// }
	}
}

- (BOOL)windowShouldClose:(id)sender
{
	// SDL_SendWindowEvent(_data->window, SDL_WINDOWEVENT_CLOSE, 0, 0);
	return NO;
}

- (void)windowDidExpose:(NSNotification*)aNotification
{
	// SDL_SendWindowEvent(_data->window, SDL_WINDOWEVENT_EXPOSED, 0, 0);
}

- (void)windowWillMove:(NSNotification*)aNotification
{
	if ([m_pParent->GetWindow() isKindOfClass:[BurgerWindow class]]) {
		pendingWindowWarpX = pendingWindowWarpY = -1;
		isMoving = YES;
	}
}

- (void)windowDidMove:(NSNotification*)aNotification
{
	int x, y;
	NSWindow* nswindow = m_pParent->GetWindow();
	NSRect rect = [nswindow contentRectForFrameRect:[nswindow frame]];
	FixNSRectOrigin(&rect);

	if (s_moveHack) {
		uint_t blockMove = ((Burger::Tick::Read() - s_moveHack) < 500);

		s_moveHack = 0;

		if (blockMove) {
			/* Cocoa is adjusting the window in response to a mode change */
			rect.origin.x = 0; // window->x;
			rect.origin.y = 0; // window->y;
			FixNSRectOrigin(&rect);
			[nswindow setFrameOrigin:rect.origin];
			return;
		}
	}

	x = (int)rect.origin.x;
	y = (int)rect.origin.y;

	//    ScheduleContextUpdates(_data);

	//    SDL_SendWindowEvent(window, SDL_WINDOWEVENT_MOVED, x, y);
}

- (void)windowDidResize:(NSNotification*)aNotification
{
	if (inFullscreenTransition) {
		/* We'll take care of this at the end of the transition */
		return;
	}

	//    SDL_Window *window = _data->window;
	NSWindow* nswindow = m_pParent->GetWindow();
	int x, y, w, h;
	NSRect rect = [nswindow contentRectForFrameRect:[nswindow frame]];
	FixNSRectOrigin(&rect);
	x = (int)rect.origin.x;
	y = (int)rect.origin.y;
	w = (int)rect.size.width;
	h = (int)rect.size.height;

	//    if (SDL_IsShapedWindow(window)) {
	//        Cocoa_ResizeWindowShape(window);
	//    }

	//    ScheduleContextUpdates(_data);

	/* The window can move during a resize event, such as when maximizing
	 or resizing from a corner */
	// SDL_SendWindowEvent(window, SDL_WINDOWEVENT_MOVED, x, y);
	// SDL_SendWindowEvent(window, SDL_WINDOWEVENT_RESIZED, w, h);

	const BOOL zoomed = [nswindow isZoomed];
	if (!zoomed) {
		// SDL_SendWindowEvent(window, SDL_WINDOWEVENT_RESTORED, 0, 0);
	} else if (zoomed) {
		// SDL_SendWindowEvent(window, SDL_WINDOWEVENT_MAXIMIZED, 0, 0);
	}
}

- (void)windowDidMiniaturize:(NSNotification*)aNotification
{
	// SDL_SendWindowEvent(_data->window, SDL_WINDOWEVENT_MINIMIZED, 0, 0);
}

- (void)windowDidDeminiaturize:(NSNotification*)aNotification
{
	// SDL_SendWindowEvent(_data->window, SDL_WINDOWEVENT_RESTORED, 0, 0);
}

- (void)windowDidBecomeKey:(NSNotification*)aNotification
{
	// SDL_Window *window = _data->window;
	// SDL_Mouse *mouse = SDL_GetMouse();
	//    if (mouse->relative_mode && ![self isMoving]) {
	//        mouse->SetRelativeMouseMode(SDL_TRUE);
	//    }

	/* We're going to get keyboard events, since we're key. */
	//    SDL_SetKeyboardFocus(window);

	/* If we just gained focus we need the updated mouse position */
#if 0
    if (!mouse->relative_mode) {
        NSPoint point;
        int x, y;

        point = [_data->nswindow mouseLocationOutsideOfEventStream];
        x = (int)point.x;
        y = (int)(window->h - point.y);

        if (x >= 0 && x < window->w && y >= 0 && y < window->h) {
            SDL_SendMouseMotion(window, 0, 0, x, y);
        }
    }
#endif

	/* Check to see if someone updated the clipboard */
	//    Cocoa_CheckClipboardUpdate(_data->videodata);

	if ((isFullscreenSpace) &&
		(m_pParent->GetDisplay()->GetFlags() & Burger::Display::FULLSCREEN)) {
		[NSMenu setMenuBarVisible:NO];
	}
}

- (void)windowDidResignKey:(NSNotification*)aNotification
{
#if 0
    SDL_Mouse *mouse = SDL_GetMouse();
    if (mouse->relative_mode) {
        mouse->SetRelativeMouseMode(SDL_FALSE);
    }

    /* Some other window will get mouse events, since we're not key. */
    if (SDL_GetMouseFocus() == _data->window) {
        SDL_SetMouseFocus(NULL);
    }

    /* Some other window will get keyboard events, since we're not key. */
    if (SDL_GetKeyboardFocus() == _data->window) {
        SDL_SetKeyboardFocus(NULL);
    }
#endif
	if (isFullscreenSpace) {
		[NSMenu setMenuBarVisible:YES];
	}
}

- (void)windowWillEnterFullScreen:(NSNotification*)aNotification
{
	//   SDL_Window *window = _data->window;

	//   SetWindowStyle(window,
	//   (NSTitledWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask|NSResizableWindowMask));

	isFullscreenSpace = YES;
	inFullscreenTransition = YES;
}

- (void)windowDidEnterFullScreen:(NSNotification*)aNotification
{
	//    SDL_Window *window = _data->window;

	inFullscreenTransition = NO;

	if (pendingWindowOperation == PENDING_EXITFULLSCREEN) {
		pendingWindowOperation = PENDING_NULL;
		[self setFullscreenSpace:NO];
	} else {
		if (m_pParent->GetDisplay()->GetFlags() & Burger::Display::FULLSCREEN) {
			[NSMenu setMenuBarVisible:NO];
		}

		pendingWindowOperation = PENDING_NULL;
		/* Force the size change event in case it was delivered earlier
		 while the window was still animating into place.
		 */
		//        window->w = 0;
		//        window->h = 0;
		[self windowDidResize:aNotification];
	}
}

- (void)windowWillExitFullScreen:(NSNotification*)aNotification
{
	//   SDL_Window *window = _data->window;

	//   SetWindowStyle(window, GetWindowStyle(window));

	isFullscreenSpace = NO;
	inFullscreenTransition = YES;
}

- (void)windowDidExitFullScreen:(NSNotification*)aNotification
{
	//    SDL_Window *window = _data->window;
	NSWindow* nswindow = m_pParent->GetWindow();

	inFullscreenTransition = NO;

	[nswindow setLevel:kCGNormalWindowLevel];

	if (pendingWindowOperation == PENDING_GOFULLSCREEN) {
		pendingWindowOperation = PENDING_NULL;
		[self setFullscreenSpace:YES];
	} else if (pendingWindowOperation == PENDING_MINIMIZE) {
		pendingWindowOperation = PENDING_NULL;
		[nswindow miniaturize:nil];
	} else {
		/* Adjust the fullscreen toggle button and readd menu now that we're
		 * here. */
		if (m_pParent->GetDisplay()->GetFlags() &
			Burger::Display::ALLOWRESIZING) {
			/* resizable windows are Spaces-friendly: they get the "go
			 * fullscreen" toggle button on their titlebar. */
			[nswindow setCollectionBehavior:
						  NSWindowCollectionBehaviorFullScreenPrimary];
		} else {
			[nswindow setCollectionBehavior:NSWindowCollectionBehaviorManaged];
		}
		[NSMenu setMenuBarVisible:YES];

		pendingWindowOperation = PENDING_NULL;
		/* Force the size change event in case it was delivered earlier
		 while the window was still animating into place.
		 */
		//        window->w = 0;
		//        window->h = 0;
		[self windowDidResize:aNotification];

		/* FIXME: Why does the window get hidden? */
		//       if (window->flags & SDL_WINDOW_SHOWN) {
		//           Cocoa_ShowWindow(SDL_GetVideoDevice(), window);
		//       }
	}
}

- (NSApplicationPresentationOptions)window:(NSWindow*)window
	  willUseFullScreenPresentationOptions:
		  (NSApplicationPresentationOptions)proposedOptions
{
	if (m_pParent->GetDisplay()->GetFlags() & Burger::Display::FULLSCREEN) {
		return NSApplicationPresentationFullScreen |
			NSApplicationPresentationHideDock |
			NSApplicationPresentationHideMenuBar;
	} else {
		return proposedOptions;
	}
}

/* We'll respond to key events by doing nothing so we don't beep.
 * We could handle key messages here, but we lose some in the NSApp dispatch,
 * where they get converted to action messages, etc.
 */
- (void)flagsChanged:(NSEvent*)theEvent
{
	/*Cocoa_HandleKeyEvent(SDL_GetVideoDevice(), theEvent);*/
}
- (void)keyDown:(NSEvent*)theEvent
{
	/*Cocoa_HandleKeyEvent(SDL_GetVideoDevice(), theEvent);*/
}
- (void)keyUp:(NSEvent*)theEvent
{
	/*Cocoa_HandleKeyEvent(SDL_GetVideoDevice(), theEvent);*/
}

/* We'll respond to selectors by doing nothing so we don't beep.
 * The escape key gets converted to a "cancel" selector, etc.
 */
- (void)doCommandBySelector:(SEL)aSelector
{
	/*NSLog(@"doCommandBySelector: %@\n", NSStringFromSelector(aSelector));*/
}

- (void)mouseDown:(NSEvent*)theEvent
{
	int button;

	switch ([theEvent buttonNumber]) {
	case 0:
		if (([theEvent modifierFlags] & NSControlKeyMask) /* &&
                GetHintCtrlClickEmulateRightClick() */ ) {
			wasCtrlLeft = YES;
			//                button = SDL_BUTTON_RIGHT;
		} else {
			wasCtrlLeft = NO;
			//                button = SDL_BUTTON_LEFT;
		}
		break;
	case 1:
		//            button = SDL_BUTTON_RIGHT;
		break;
	case 2:
		//            button = SDL_BUTTON_MIDDLE;
		break;
	default:
		//            button = [theEvent buttonNumber] + 1;
		break;
	}
	//    SDL_SendMouseButton(_data->window, 0, SDL_PRESSED, button);
}

- (void)rightMouseDown:(NSEvent*)theEvent
{
	[self mouseDown:theEvent];
}

- (void)otherMouseDown:(NSEvent*)theEvent
{
	[self mouseDown:theEvent];
}

- (void)mouseUp:(NSEvent*)theEvent
{
	NSInteger button;

	switch ([theEvent buttonNumber]) {
	case 0:
		if (wasCtrlLeft) {
			//                button = SDL_BUTTON_RIGHT;
			wasCtrlLeft = NO;
		} else {
			//                button = SDL_BUTTON_LEFT;
		}
		break;
	case 1:
		//            button = SDL_BUTTON_RIGHT;
		break;
	case 2:
		//            button = SDL_BUTTON_MIDDLE;
		break;
	default:
		button = [theEvent buttonNumber] + 1;
		break;
	}
	//    SDL_SendMouseButton(_data->window, 0, SDL_RELEASED, button);
}

- (void)rightMouseUp:(NSEvent*)theEvent
{
	[self mouseUp:theEvent];
}

- (void)otherMouseUp:(NSEvent*)theEvent
{
	[self mouseUp:theEvent];
}

- (void)mouseMoved:(NSEvent*)theEvent
{
	//    SDL_Mouse *mouse = SDL_GetMouse();
	//    SDL_Window *window = _data->window;
	NSPoint point;
	int x, y;

#if 0
    if (mouse->relative_mode) {
        return;
    }

    point = [theEvent locationInWindow];
    x = (int)point.x;
    y = (int)(window->h - point.y);

    if (x < 0 || x >= window->w || y < 0 || y >= window->h) {
        if (window->flags & SDL_WINDOW_INPUT_GRABBED) {
            if (x < 0) {
                x = 0;
            } else if (x >= window->w) {
                x = window->w - 1;
            }
            if (y < 0) {
                y = 0;
            } else if (y >= window->h) {
                y = window->h - 1;
            }

#if !SDL_MAC_NO_SANDBOX
            CGPoint cgpoint;

            /* When SDL_MAC_NO_SANDBOX is set, this is handled by
             * SDL_cocoamousetap.m.
             */

            cgpoint.x = window->x + x;
            cgpoint.y = window->y + y;

            /* According to the docs, this was deprecated in 10.6, but it's still
             * around. The substitute requires a CGEventSource, but I'm not entirely
             * sure how we'd procure the right one for this event.
             */
            CGSetLocalEventsSuppressionInterval(0.0);
            CGDisplayMoveCursorToPoint(kCGDirectMainDisplay, cgpoint);
            CGSetLocalEventsSuppressionInterval(0.25);

            Cocoa_HandleMouseWarp(cgpoint.x, cgpoint.y);
#endif
        }
    }
    SDL_SendMouseMotion(window, 0, 0, x, y);
#endif
}

- (void)mouseDragged:(NSEvent*)theEvent
{
	[self mouseMoved:theEvent];
}

- (void)rightMouseDragged:(NSEvent*)theEvent
{
	[self mouseMoved:theEvent];
}

- (void)otherMouseDragged:(NSEvent*)theEvent
{
	[self mouseMoved:theEvent];
}

- (void)scrollWheel:(NSEvent*)theEvent
{
	//    Cocoa_HandleMouseWheel(_data->window, theEvent);
}

- (void)touchesBeganWithEvent:(NSEvent*)theEvent
{
	[self handleTouches:COCOA_TOUCH_DOWN withEvent:theEvent];
}

- (void)touchesMovedWithEvent:(NSEvent*)theEvent
{
	[self handleTouches:COCOA_TOUCH_MOVE withEvent:theEvent];
}

- (void)touchesEndedWithEvent:(NSEvent*)theEvent
{
	[self handleTouches:COCOA_TOUCH_UP withEvent:theEvent];
}

- (void)touchesCancelledWithEvent:(NSEvent*)theEvent
{
	[self handleTouches:COCOA_TOUCH_CANCELLED withEvent:theEvent];
}

- (void)handleTouches:(cocoaTouchType)type withEvent:(NSEvent*)event
{
	NSSet* touches = 0;
	NSEnumerator* enumerator;
	NSTouch* touch;

	switch (type) {
	case COCOA_TOUCH_DOWN:
		touches = [event touchesMatchingPhase:NSTouchPhaseBegan inView:nil];
		break;
	case COCOA_TOUCH_UP:
		touches = [event touchesMatchingPhase:NSTouchPhaseEnded inView:nil];
		break;
	case COCOA_TOUCH_CANCELLED:
		touches = [event touchesMatchingPhase:NSTouchPhaseCancelled inView:nil];
		break;
	case COCOA_TOUCH_MOVE:
		touches = [event touchesMatchingPhase:NSTouchPhaseMoved inView:nil];
		break;
	}

	enumerator = [touches objectEnumerator];
	touch = (NSTouch*)[enumerator nextObject];
	while (touch) {
#if 0
        const SDL_TouchID touchId = (SDL_TouchID)(intptr_t)[touch device];
        if (!SDL_GetTouch(touchId)) {
            if (SDL_AddTouch(touchId, "") < 0) {
                return;
            }
        }

        const SDL_FingerID fingerId = (SDL_FingerID)(intptr_t)[touch identity];
        float x = [touch normalizedPosition].x;
        float y = [touch normalizedPosition].y;
        /* Make the origin the upper left instead of the lower left */
        y = 1.0f - y;

        switch (type) {
            case COCOA_TOUCH_DOWN:
                SDL_SendTouch(touchId, fingerId, SDL_TRUE, x, y, 1.0f);
                break;
            case COCOA_TOUCH_UP:
            case COCOA_TOUCH_CANCELLED:
                SDL_SendTouch(touchId, fingerId, SDL_FALSE, x, y, 1.0f);
                break;
            case COCOA_TOUCH_MOVE:
                SDL_SendTouchMotion(touchId, fingerId, x, y, 1.0f);
                break;
        }
#endif
		touch = (NSTouch*)[enumerator nextObject];
	}
}

@end
#endif

#endif

/*! ************************************

	\brief Base constructor.

	Start up a MacOSX app

***************************************/

Burger::GameApp::GameApp(uintptr_t uDefaultMemorySize, uint_t uDefaultHandleCount,
	uintptr_t uMinReserveSize) :
	m_MemoryManagerHandle(
		uDefaultMemorySize, uDefaultHandleCount, uMinReserveSize),
	m_pApplication(NULL), m_pApplicationDelegate(NULL), m_pWindow(NULL),
	m_pListener(NULL), m_bCenterWindow(TRUE)
{
	InitDefaults();
	//
	// Ensure our app is the foreground app
	//

	ProcessSerialNumber MyProcessNumber;
	if (!GetCurrentProcess(&MyProcessNumber)) {
		TransformProcessType(
			&MyProcessNumber, kProcessTransformToForegroundApplication);
		SetFrontProcess(&MyProcessNumber);
	}

	// Desktop applications require a NSApplication context to be created
	// Create it here

	NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
	NSApplication* pApplication = [BurgerApplication sharedApplication];
	m_pApplication = pApplication;
	[static_cast<BurgerApplication*>(pApplication) setGameApp:this];

	//
	// Create the default application delegate to trap window screen
	// changes
	//

	BurgerApplicationDelegate* pDelegate =
		[[BurgerApplicationDelegate alloc] initWithGameApp:this];
	m_pApplicationDelegate = static_cast<NSApplicationDelegate*>(pDelegate);
	[pApplication setDelegate:static_cast<id>(pDelegate)];

	//
	// Create a master window for the application
	//

	// Start by creating the window in the center of the main screen

	NSRect MainScreenRect = [[NSScreen mainScreen] frame];
	NSRect MyFrame = NSMakeRect((MainScreenRect.size.width - 640.0f) * 0.5f,
		(MainScreenRect.size.height - 480.0f) * 0.5f, 640, 480);
	BurgerWindow* pWindow = [[BurgerWindow alloc]
		initWithContentRect:MyFrame
				  styleMask:NSTitledWindowMask | NSClosableWindowMask |
				  NSMiniaturizableWindowMask | NSResizableWindowMask
					backing:NSBackingStoreBuffered
					  defer:NO];
	m_pWindow = pWindow;
	[pWindow setAllowsToolTipsWhenApplicationIsInactive:NO];
	[pWindow setAutorecalculatesKeyViewLoop:NO];
	[pWindow setReleasedWhenClosed:NO];

	// Init the global cursor
	OSCursor::Init();

	// Init the file system
	FileManager::Init();

	// Since MacOSX Apps can be standalone filenames
	// or they can be in a ".app" bundle, let's figure it out

	// If it's not a bundle, directory 9 is fine.

	// First test if in a bundle. Is the code in a folder called "MacOS"?

	Filename Name;
	FileManager::GetPrefix(&Name, 9);
	const char* pFileName = Name.GetPtr();
	uintptr_t uLength = StringLength(pFileName);
	if ((uLength > 7) && !StringCompare((pFileName + uLength) - 7, ":MacOS:")) {

		// Secondary test. Does the PkgInfo file exist?
		if (FileManager::DoesFileExist("9:..:PkgInfo")) {

			// The application is in a bundle
			// If there is a folder called "Resources", assume that's where the
			// data resides, otherwise, pop three folders to get to the sam
			// folder as the app and assume the data is next to the application

			if (FileManager::DoesFileExist("9:..:Resources")) {
				FileManager::SetPrefix(9, "9:..:Resources");
			} else {
				// Pop prefixes so the data is external
				FileManager::PopPrefix(9);
				FileManager::PopPrefix(9);
				FileManager::PopPrefix(9);
			}
		}
	}
	AddRoutine(EventPoll, NULL, this, RunQueue::PRIORITY_FIRST);
	// Garbage collect
	[pPool release];
}

/*! ************************************

	\brief Dispose of any allocated resources

	When a OSX app is shut down, dispose of everything here.

***************************************/

Burger::GameApp::~GameApp()
{
	RemoveRoutine(EventPoll, this);
	// Release the file system
	FileManager::Shutdown();

	// Release the cursor
	OSCursor::Shutdown();
	ShutdownDefaults();
}

/*! ************************************

	\brief Handle NSApplicationDidBecomeActiveNotification

	When a NSApplicationDidBecomeActiveNotification is passed to this
	application, this function is called to hide or show the game window

***************************************/

void BURGER_API Burger::GameApp::FocusWindow(void) {}

/***************************************

	\brief Change the size of the application window

	When initializing a display, the window needs to be adjusted to be able to
	accommodate the new size. This function will make the window visible and
	resize it to the requested dimensions.

	If this is the first time executing, the window will be placed in the center
	of the screen, otherwise it will be place in at the last recorded location.

	\param uWidth Width of the display rectangle in pixels
	\param uHeight Height of the display rectangle in pixels
	\return Zero if no error, non-zero windows error code

***************************************/

uint_t BURGER_API Burger::GameApp::SetWindowSize(uint_t uWidth, uint_t uHeight)
{
	NSWindow* pWindow = m_pWindow;
	uint_t uResult = 1;
	if (pWindow) {
		CGFloat fWidth = static_cast<CGFloat>(static_cast<int>(uWidth));
		CGFloat fHeight = static_cast<CGFloat>(static_cast<int>(uHeight));
		// Should the window be placed in the center of the screen?
		if (m_bCenterWindow) {
			NSRect MainScreenRect = [[NSScreen mainScreen] frame];
			// Create the content rectangle centered in the screen
			NSRect NewFrame = NSMakeRect(
				(MainScreenRect.size.width - fWidth) * 0.5f,
				(MainScreenRect.size.height - fHeight) * 0.5f, fWidth, fHeight);
			// Convert the contect rect into a window rect (Factor in the title
			// bar)
			NewFrame = [pWindow frameRectForContentRect:NewFrame];
			// Set the new window frame
			[pWindow setFrame:NewFrame display:YES animate:NO];
		} else {
			NSSize NewSize;
			NewSize.width = fWidth;
			NewSize.height = fHeight;
			// Just adjust the new window size
			[pWindow setContentSize:NewSize];
		}
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief MacOSX event handler

	MacOSX uses cooperative multi-threading for operating system events. This
	function, which is called every time Poll() is called, will process all
	pending MacOSX system events.

	\param pData A pointer to the current GameApp is expected
	\return RunQueue::OKAY

***************************************/

Burger::RunQueue::eReturnCode BURGER_API Burger::GameApp::EventPoll(void* pData)
{
	GameApp* pApp = static_cast<GameApp*>(pData);
	NSAutoreleasePool* pReleasePool = [[NSAutoreleasePool alloc] init];
	for (;;) {
		NSEvent* pEvent = [NSApp nextEventMatchingMask:NSAnyEventMask
											 untilDate:[NSDate distantPast]
												inMode:NSDefaultRunLoopMode
											   dequeue:YES];
		if (!pEvent) {
			break;
		}

		switch ([pEvent type]) {
		case NSLeftMouseDown:
		case NSOtherMouseDown:
		case NSRightMouseDown:
		case NSLeftMouseUp:
		case NSOtherMouseUp:
		case NSRightMouseUp:
		case NSLeftMouseDragged:
		case NSRightMouseDragged:
		case NSOtherMouseDragged: /* usually middle mouse dragged */
		case NSMouseMoved:
		case NSScrollWheel:
			if (pApp->m_pMouse) {
				//	Cocoa_HandleMouseEvent(_this, event);
			}
			break;
		case NSKeyDown:
		case NSKeyUp:
		case NSFlagsChanged:
			if (pApp->m_pKeyboard) {
				pApp->m_pKeyboard->ProcessEvent(pEvent);
			}
			break;
		default:
			break;
		}
		// Send the event to the operating system
		[NSApp sendEvent:pEvent];
	}
	[pReleasePool release];
	return RunQueue::OKAY;
}

#endif
