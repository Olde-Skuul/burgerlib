/***************************************

	MacOSX application manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmacosxapp.h"
#if defined(BURGER_MACOSX) || defined(DOXYGEN)
#include "broscursor.h"
#include "brfilemanager.h"
#include "brfilename.h"
#include "brstringfunctions.h"
#include "brkeyboard.h"
#include "brmouse.h"
#import <ApplicationServices/ApplicationServices.h>
#import <AppKit/NSApplication.h>
#import <Foundation/NSAutoreleasePool.h>
#import <Foundation/NSNotification.h>
#import <AppKit/NSWindow.h>
#import <AppKit/NSScreen.h>
#import <AppKit/NSEvent.h>

/*! ************************************

	\class Burger::MacOSXApp
	\brief Base class for MacOSX game applications.

	This class contains everything needed to create a
	cross-platform environment for writing console based
	applications. Functions are included to allow for
	the application to gain <i>drag and drop</i> functionality
	on platforms that support it.

***************************************/

#if !defined(DOXYGEN)

/***************************************
 
	Extend the NSApplication so events
	can be intercepted

***************************************/

@interface BurgerApplication : NSApplication {
	Burger::MacOSXApp *m_App;		///< Parent app
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

- (void)setGameApp:(Burger::MacOSXApp *)pGameApp
{
	m_App = pGameApp;
}

@end

/***************************************
 
	Create an NSApplicationDelegate to capture
	window activate/deactivate events

***************************************/

@interface BurgerApplicationDelegate : NSObject {
	Burger::MacOSXApp *m_App;
	Word m_bStarted;
}
- (id)initWithGameApp:(Burger::MacOSXApp *)pGameApp;
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

- (id)initWithGameApp:(Burger::MacOSXApp *)pGameApp
{
	self = [super init];
	if (self) {
		m_bStarted = FALSE;
		m_App = pGameApp;
		[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(focusSomeWindow:)
			name:NSApplicationDidBecomeActiveNotification object:nil];
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
// When a NSApplicationDidBecomeActiveNotification is triggered,
// this private function will be called
//

- (void)focusSomeWindow:(NSNotification *)aNotification
{
#pragma unused(aNotification)
	
	// The moment a window is created, there's a race condition
	// where if may be visible before being made invisible
	// To avoid this, ignore the first time this is called
	
	if (!m_bStarted) {
		m_bStarted = TRUE;
	} else {
		m_App->FocusWindow();
	}
}

@end

/***************************************

	Create an NSWindow which will be
	manipulated to suit the game's needs
 
***************************************/

@interface BurgerWindow : NSWindow {
}
- (BOOL)canBecomeKeyWindow;
- (BOOL)canBecomeMainWindow;
- (void)sendEvent:(NSEvent *)event;
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

- (void)sendEvent:(NSEvent *)event
{
#pragma unused(event)
}
@end

#endif

/*! ************************************

	\brief Base constructor.

	Start up a MacOSX app

***************************************/

Burger::MacOSXApp::MacOSXApp(WordPtr uDefaultMemorySize,Word uDefaultHandleCount,WordPtr uMinReserveSize) :
	GameApp(uDefaultMemorySize,uDefaultHandleCount,uMinReserveSize),
	m_pApplication(NULL),
	m_pApplicationDelegate(NULL),
	m_pWindow(NULL),
	m_bCenterWindow(TRUE)
{
	//
	// Ensure our app is the foreground app
	//
	
	ProcessSerialNumber MyProcessNumber;
	if (!GetCurrentProcess(&MyProcessNumber)) {
		TransformProcessType(&MyProcessNumber,kProcessTransformToForegroundApplication);
		SetFrontProcess(&MyProcessNumber);
	}
	
	// Desktop applications require a NSApplication context to be created
	// Create it here
	
	NSAutoreleasePool *pPool = [[NSAutoreleasePool alloc] init];
	NSApplication *pApplication = [BurgerApplication sharedApplication];
	m_pApplication = pApplication;
	[static_cast<BurgerApplication *>(pApplication) setGameApp:this];
	
	//
	// Create the default application delegate to trap window screen
	// changes
	//
	
	BurgerApplicationDelegate *pDelegate = [[BurgerApplicationDelegate alloc] initWithGameApp:this];
	m_pApplicationDelegate = static_cast<NSApplicationDelegate *>(pDelegate);
	[pApplication setDelegate:static_cast<id>(pDelegate)];
	
	//
	// Create a master window for the application
	//
	
	// Start by creating the window in the center of the main screen
	
	NSRect MainScreenRect = [[NSScreen mainScreen] frame];
	NSRect MyFrame = NSMakeRect((MainScreenRect.size.width-640.0f)*0.5f,(MainScreenRect.size.height-480.0f)*0.5f, 640, 480);
	BurgerWindow* pWindow = [[BurgerWindow alloc] initWithContentRect:MyFrame
		styleMask:NSTitledWindowMask|NSClosableWindowMask|NSMiniaturizableWindowMask|NSResizableWindowMask
		backing:NSBackingStoreBuffered defer:NO];
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
	FileManager::GetPrefix(&Name,9);
	const char *pFileName = Name.GetPtr();
	WordPtr uLength = StringLength(pFileName);
	if ((uLength>7) && !StringCompare((pFileName+uLength)-7,":MacOS:")) {
		
		// Secondary test. Does the PkgInfo file exist?
		if (FileManager::DoesFileExist("9:..:PkgInfo")) {
			
			// The application is in a bundle
			// If there is a folder called "Resources", assume that's where the data resides,
			// otherwise, pop three folders to get to the sam folder as the app and
			// assume the data is next to the application
			
			if (FileManager::DoesFileExist("9:..:Resources")) {
				FileManager::SetPrefix(9,"9:..:Resources");
			} else {
				// Pop prefixes so the data is external
				FileManager::PopPrefix(9);
				FileManager::PopPrefix(9);
				FileManager::PopPrefix(9);
			}
		}
	}
	AddRoutine(EventPoll,this);
	// Garbage collect
	[pPool release];
}

/*! ************************************

	\brief Dispose of any allocated resources

	When a OSX app is shut down, dispose of 
	everything here.

***************************************/

Burger::MacOSXApp::~MacOSXApp()
{
	RemoveRoutine(EventPoll,this);
	// Release the file system
	FileManager::Shutdown();

	// Release the cursor
	OSCursor::Shutdown();
}

/*! ************************************
 
	\brief Handle NSApplicationDidBecomeActiveNotification
 
	When a NSApplicationDidBecomeActiveNotification is passed
	to this application, this function is called to hide or
	show the game window

***************************************/

void BURGER_API Burger::MacOSXApp::FocusWindow(void)
{
}

/*! ************************************
 
	\brief Change the size of the application window
 
	When initializing a display, the window needs to
	be adjusted to be able to accommodate the new size.
	This function will make the window visible and
	resize it to the requested dimensions.
 
	If this is the first time executing, the window
	will be placed in the center of the screen,
	otherwise it will be place in at the last
	recorded location.
 
	\param uWidth Width of the display rectangle in pixels
	\param uHeight Height of the display rectangle in pixels
	\return Zero if no error, non-zero windows error code
 
***************************************/

Word BURGER_API Burger::MacOSXApp::SetWindowSize(Word uWidth,Word uHeight)
{
	NSWindow *pWindow = m_pWindow;
	Word uResult = 1;
	if (pWindow) {
		CGFloat fWidth = static_cast<CGFloat>(static_cast<int>(uWidth));
		CGFloat fHeight = static_cast<CGFloat>(static_cast<int>(uHeight));
		// Should the window be placed in the center of the screen?
		if (m_bCenterWindow) {
			NSRect MainScreenRect = [[NSScreen mainScreen] frame];
			// Create the content rectangle centered in the screen
			NSRect NewFrame = NSMakeRect((MainScreenRect.size.width-fWidth)*0.5f,(MainScreenRect.size.height-fHeight)*0.5f,fWidth,fHeight);
			// Convert the contect rect into a window rect (Factor in the title bar)
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
 
	MacOSX uses cooperative multi-threading for operating
	system events. This function, which is called every
	time Poll() is called, will process all pending MacOSX
	system events.
 
	\param pData A pointer to the current MacOSXApp is expected
	\return RunQueue::OKAY
 
***************************************/

Burger::RunQueue::eReturnCode BURGER_API Burger::MacOSXApp::EventPoll(void *pData)
{
	MacOSXApp *pApp = static_cast<MacOSXApp *>(pData);
	NSAutoreleasePool *pReleasePool = [[NSAutoreleasePool alloc] init];
	for (;;) {
		NSEvent *pEvent = [NSApp nextEventMatchingMask:NSAnyEventMask untilDate:[NSDate distantPast] inMode:NSDefaultRunLoopMode dequeue:YES ];
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