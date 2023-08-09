/***************************************

	iOS application manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brgameapp.h"
#if defined(BURGER_IOS) || defined(DOXYGEN)
#include "brfilemanager.h"
#include "brfilename.h"
#include "brfloatingpoint.h"
#include "brglobals.h"
#include "brstringfunctions.h"
#include "brtick.h"

#import <UIKit/UIKit.h>

// Static application instance

Burger::GameApp* Burger::GameApp::g_piOSApp;

#if !defined(DOXYGEN)

//
// Main app launch delegate
//

@interface BurgerUIKitDelegate: NSObject <UIApplicationDelegate> {
}
@end

//
// This view controller handles the main screen
// until the application begins.
//

@interface BurgerLaunchViewController: UIViewController {
	UIImageView* m_pSplashImageView; ///< View that will hold the image to
									 ///< display on launch
	UIImage* m_pPortraitImage;  ///< Image when the device is in portrait mode
	UIImage* m_pLandscapeImage; ///< Image when the device is in landscape mode
}

- (void)updateSplashImage:(UIInterfaceOrientation)interfaceOrientation;
@end

@implementation BurgerLaunchViewController

/***************************************

	Initialize with the launch screen

***************************************/

- (id)init
{
	self = [super init];
	// Boned?
	if (self == nil) {
		return nil;
	}

	//
	// Get the window to show the image
	UIImageView* pSplashImageView = [[UIImageView alloc] init];
	m_pSplashImageView = pSplashImageView;
	[self setView:pSplashImageView];

	// Get the size of the screen
	CGSize ScreenSize = [UIScreen mainScreen].bounds.size;
	// Use the larger to get the portrait height
	float fPortraitHeight = Burger::maximum(ScreenSize.width, ScreenSize.height);

	// Get the larger size portrait image if available
	UIImage* pPortraitImage =
		[UIImage imageNamed:[NSString stringWithFormat:@"Default-%dh.png",
									  (int)fPortraitHeight]];
	if (!pPortraitImage) {
		// Use the default image
		pPortraitImage = [UIImage imageNamed:@"Default.png"];
	}

	// Do the same for the landscape mode
	UIImage* pLandscapeImage = [UIImage imageNamed:@"Default-Landscape.png"];
	// No landscape?
	if (!pLandscapeImage && pPortraitImage) {
		// Create a landscape image from the portrait image
		pLandscapeImage =
			[[UIImage alloc] initWithCGImage:pPortraitImage.CGImage
									   scale:1.0f
								 orientation:UIImageOrientationRight];
	}
	// Store the pointers (Can be NULL)
	m_pPortraitImage = pPortraitImage;
	m_pLandscapeImage = pLandscapeImage;
	// Force them to stick around
	if (pPortraitImage) {
		[pPortraitImage retain];
	}
	if (pLandscapeImage) {
		[pLandscapeImage retain];
	}
	// Select the image to display
	[self updateSplashImage:[[UIApplication sharedApplication]
								statusBarOrientation]];
	return self;
}

/***************************************

	Release everything

***************************************/

- (void)dealloc
{
	// Release the view
	[self setView:nil];
	// Dispose of everything
	if (m_pSplashImageView) {
		m_pSplashImageView.image = NULL;
		[m_pSplashImageView release];
		m_pSplashImageView = NULL;
	}
	if (m_pPortraitImage) {
		[m_pPortraitImage release];
		m_pPortraitImage = NULL;
	}
	if (m_pLandscapeImage) {
		[m_pLandscapeImage release];
		m_pLandscapeImage = NULL;
	}
	// Finish the job
	[super dealloc];
}

/***************************************

	Get the orientations supported

***************************************/

- (NSUInteger)supportedInterfaceOrientations
{
#if defined(__IPHONE_6_0)
	NSUInteger uResult = UIInterfaceOrientationMaskAll;

	// Allow all directions on an iPad, but don't allow upside down
	// on iPhones due to issues with taking a call.

	if ([[UIDevice currentDevice] userInterfaceIdiom] ==
		UIUserInterfaceIdiomPhone) {
		uResult &= ~UIInterfaceOrientationMaskPortraitUpsideDown;
	}
#else
	NSUInteger uResult = 0;
#endif
	return uResult;
}

/***************************************

	Return TRUE if a suggested orientation is allowed

***************************************/

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)orient
{
	// Call the function only because it MIGHT be overridden
	NSUInteger uMark = [self supportedInterfaceOrientations];
	// Mask it
	return (uMark & (1U << orient)) != 0;
}

/***************************************

	Animate the screen to the new rotation

***************************************/

- (void)willAnimateRotationToInterfaceOrientation:
			(UIInterfaceOrientation)interfaceOrientation
										 duration:(NSTimeInterval)duration
{
	// No animation, just do it
#pragma unused(duration)
	[self updateSplashImage:interfaceOrientation];
}

/***************************************

	Select the image to show depending on the orientation of the iOS device

***************************************/

- (void)updateSplashImage:(UIInterfaceOrientation)interfaceOrientation
{
	UIImage* pImage;

	if (UIInterfaceOrientationIsLandscape(interfaceOrientation)) {
		pImage = m_pLandscapeImage;
	} else {
		pImage = m_pPortraitImage;
	}
	if (pImage) {
		m_pSplashImageView.image = pImage;
	}
}

@end

/***************************************

	This class handles the opening of the launch screen
	and starting the application via a call to CodeEntry()

***************************************/

@implementation BurgerUIKitDelegate

/***************************************

	Initialize the delegate

***************************************/

- (id)init
{
	self = [super init];
	return self;
}

/***************************************

	Startup has completed, call the application

***************************************/

- (void)postFinishLaunch
{
	// Run the application and capture the exit code
	Burger::Globals::SetErrorCode(static_cast<Burger::eError>(CodeEntry(Burger::GameApp::GetApp())));
	// Dispose of the window
	Burger::GameApp::GetApp()->ReleaseWindow();
}

/***************************************

	UIKit has completed the magic, show the start screen if needed

***************************************/

- (BOOL)application:(UIApplication*)application
	didFinishLaunchingWithOptions:(NSDictionary*)launchOptions
{
#pragma unused(application)
#pragma unused(launchOptions)

	Burger::GameApp* pApp = Burger::GameApp::GetApp();

	// Retain the launch image
	UIWindow* pWindow = pApp->CreateWindow();
	UIViewController* pLaunchViewController =
		[[BurgerLaunchViewController alloc] init];
	pApp->SetViewController(pLaunchViewController, pLaunchViewController.view);
	[pWindow makeKeyAndVisible];

	// Ensure the filemanager is pointing to the bundle folder
	[[NSFileManager defaultManager]
		changeCurrentDirectoryPath:[[NSBundle mainBundle] resourcePath]];

	[self performSelector:@selector(postFinishLaunch)
			   withObject:nil
			   afterDelay:0.0];
	return YES;
}

/***************************************

	Called when the app is shut down

***************************************/

- (void)applicationWillTerminate:(UIApplication*)application
{
#pragma unused(application)
	// Tell the application to exit
	Burger::GameApp::GetApp()->SetQuitCode();
}

/***************************************

	Called when memory is low

***************************************/

- (void)applicationDidReceiveMemoryWarning:(UIApplication*)application
{
#pragma unused(application)
}

/***************************************

	Called when it's being put to sleep

***************************************/

- (void)applicationWillResignActive:(UIApplication*)application
{
#pragma unused(application)
}

/***************************************

	Called when backgrounded

***************************************/

- (void)applicationDidEnterBackground:(UIApplication*)application
{
#pragma unused(application)
}

/***************************************

	Called when activated

***************************************/

- (void)applicationWillEnterForeground:(UIApplication*)application
{
#pragma unused(application)
}

/***************************************

	Called after activation was complete

***************************************/

- (void)applicationDidBecomeActive:(UIApplication*)application
{
#pragma unused(application)
}

/***************************************

	Called when another app directs this one to respond to opening a file

***************************************/

- (BOOL)application:(UIApplication*)application
			  openURL:(NSURL*)url
	sourceApplication:(NSString*)sourceApplication
		   annotation:(id)annotation
{
#pragma unused(application)
#pragma unused(url)
#pragma unused(sourceApplication)
#pragma unused(annotation)
	return YES;
}

@end
#endif

/***************************************

	\brief Base constructor.

	Start up a iOS app

***************************************/

Burger::GameApp::GameApp(uintptr_t uDefaultMemorySize,
	uint_t uDefaultHandleCount, uintptr_t uMinReserveSize) BURGER_NOEXCEPT
	: m_MemoryManagerHandle(
		  uDefaultMemorySize, uDefaultHandleCount, uMinReserveSize),
	  m_pWindow(nullptr),
	  m_pViewController(nullptr),
	  m_pView(nullptr)
{
	Tick::init();
	InitDefaults();
	g_piOSApp = this;

	// Add the iOS callback function
	m_RunQueue.Add(Poll, nullptr, this, RunQueue::PRIORITY_FIRST);

	// Init the file system
	FileManager::initialize();
}

/***************************************

	\brief Dispose of any allocated resources

	When a iOS app is shut down, dispose of
	everything here.

***************************************/

Burger::GameApp::~GameApp()
{
	RemoveRoutine(Poll, this);
	ReleaseWindow();
	// Release the file system
	FileManager::shut_down();
	ShutdownDefaults();
	Tick::shutdown();
}

/*! ************************************

	\brief Set up iOS and call CodeEntry()

	Sets up an iOS application shell and calls CodeEntry()
	\return Result obtained from the call to CodeEntry()

***************************************/

int BURGER_API Burger::GameApp::Run(void) BURGER_NOEXCEPT
{
	// Prevent leaks by making a global autorelease pool
	NSAutoreleasePool* pPool = [[NSAutoreleasePool alloc] init];
	// Use the delegate above
	int iResult = UIApplicationMain(
		GetArgc(), const_cast<char**>(GetArgv()), nil, @"BurgerUIKitDelegate");
	// Application is done, release all memory
	[pPool release];
	return iResult;
}

/*! ************************************

	\fn UIWindow *Burger::GameApp::GetWindow(void) const
	\brief Return the main application window

	iOS applications have a single master window that covers
	the entire screen. This function will return the
	pointer to that window

	\return Pointer to the current UIWindow.
	\sa CreateWindow(void) or ReleaseWindow(void)

***************************************/

/*! ************************************

	\brief Create the main application window

	iOS applications have a single master window that covers
	the entire screen. This function will create the window

	\return Pointer to the newly created UIWindow.
	\sa GetWindow(void) const or ReleaseWindow(void)

***************************************/

UIWindow* BURGER_API Burger::GameApp::CreateWindow(void) BURGER_NOEXCEPT
{
	ReleaseWindow();
	UIWindow* pWindow =
		[[UIWindow alloc] initWithFrame:[[UIScreen mainScreen] bounds]];
	m_pWindow = pWindow;
	return pWindow;
}

/*! ************************************

	\brief Dispose of the window

	Release the UIWindow used by the iOS application
	\sa GetWindow(void) const or CreateWindow(void)

***************************************/

void BURGER_API Burger::GameApp::ReleaseWindow(void) BURGER_NOEXCEPT
{
	SetViewController(NULL, NULL);
	// Clean up, we're leaving
	[m_pWindow release];
	m_pWindow = NULL;
}

/*! ************************************

	\fn UIViewController *Burger::GameApp::GetViewController(void) const
	\brief Return the main application window controller

	iOS application windows can have an attached window controller,
	this is the currently active controller pointer

	\return Pointer to the current UIViewController.
	\sa GetView(void) const or SetViewController(UIViewController *,UIView*)

***************************************/

/*! ************************************

	\fn UIView *Burger::GameApp::GetView(void) const
	\brief Return the main application window view

	iOS application windows can have an attached window view,
	this is the currently active view pointer

	\return Pointer to the current UIView.
	\sa GetViewController(void) const or SetViewController(
		UIViewController *, UIView*)

***************************************/

/*! ************************************

	\brief Set the main window's UIView and UIViewController

	iOS applications have a single master window that covers
	the entire screen. This function will attach (If different)
	a UIViewController and / or UIView to it.

	\param pViewController Pointer to a UIViewController or \ref NULL to detach
		any previous controller
	\param pView Pointer to a UIView or \ref NULL to detach any previous view

	\sa GetViewController(void) const or GetView(void) const

***************************************/

void BURGER_API Burger::GameApp::SetViewController(
	UIViewController* pViewController, UIView* pView) BURGER_NOEXCEPT
{
	UIViewController* pOldViewController = m_pViewController;
	UIView* pOldView = m_pView;
	m_pView = pView;
	m_pViewController = pViewController;

	// Is the controller the same?

	if (pViewController == pOldViewController) {
		// Did the view change?
		if (pView != pOldView) {
			// Update the view
			[pOldViewController setView:pView];
			[pView retain];
			// Release the old view
			[pOldView release];
		}
	} else {
		// Release the controller if it's the one attached to the
		// main window
		UIWindow* pWindow = m_pWindow;
		if (pWindow) {
			pWindow.rootViewController = nil;
		}
		// Dispose (Release) the old controller

		[pOldViewController setView:nil];
		[pOldViewController release];
		[pOldView removeFromSuperview];
		[pOldView release];

		[pViewController retain];
		[pView retain];
		[pViewController setView:pView];
		if ([pView respondsToSelector:@selector(setCurrentContext:)]) {
			[pView performSelector:@selector(setCurrentContext:)];
		}
		[pWindow addSubview:pView];
		if (pWindow) {
			pWindow.rootViewController = pViewController;
		}
	}
}

/*! ************************************

	\fn GameApp *Burger::GameApp::GetApp(void)
	\brief Return the pointer to the global application instance

	iOS applications only exist in a single instance, to simplify internal code,
	the instance is reflected in a singleton and the pointer to it is stored in
	a global which is accessed through this function.

	\return Pointer to the singleton GameApp instance

***************************************/

/***************************************

	\brief Process iOS events

	Internal function that will call CFRunLoopRunInMode(kCFRunLoopDefaultMode)
	and CFRunLoopRunInMode(UITrackingRunLoopMode) to yield time to other apps

	\param pSelf "this" pointer to the GameApp class
	\return RunQueue::OKAY since this function never shuts down on its own

***************************************/

Burger::RunQueue::eReturnCode BURGER_API Burger::GameApp::Poll(
	void* /* pSelf */) BURGER_NOEXCEPT
{
	const CFTimeInterval fInterval = 0.000002;

	// Process input events

	SInt32 iResult;
	do {
		iResult = CFRunLoopRunInMode(kCFRunLoopDefaultMode, fInterval, TRUE);
	} while (iResult == kCFRunLoopRunHandledSource);

	// Needed to allow UIScrollView to work

	do {
		iResult = CFRunLoopRunInMode(
			(CFStringRef)UITrackingRunLoopMode, fInterval, TRUE);
	} while (iResult == kCFRunLoopRunHandledSource);

	return RunQueue::OKAY;
}

#endif
