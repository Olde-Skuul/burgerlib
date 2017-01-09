/***************************************

	Static class to be a shim to an operating system cursor

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "broscursor.h"

/*! ************************************

	\class Burger::OSCursorImage
	\brief Class to generate an operating system cursor

	For desktop operating systems like MacOS, Windows and Linux,
	a mouse cursor is drawn and updated by the operating system.
	This class allows the creation of cursors from monochrome or color
	bitmaps for run time creation of a hardware cursor.

***************************************/

/*! ************************************

	\brief Default constructor

	Power up defaults. No cursor is generated.

***************************************/

Burger::OSCursorImage::OSCursorImage() :
#if defined(BURGER_WINDOWS)
	m_pCursorImage(NULL),
#endif
	m_uWidth(0),
	m_uHeight(0),
	m_iHotX(0),
	m_iHotY(0)
{
}

/*! ************************************

	\brief Default destructor

	Release all resources allocated
	\sa Shutdown()

***************************************/

Burger::OSCursorImage::~OSCursorImage()
{
	Shutdown();
}

/*! ************************************

	\brief Generate a monochrome cursor

	Given a black and white image, create a cursor.

	\note MacOS has a 16x16 maximum and Windows has a 32x32 maximum. Care should be used with this function.
	\param pXor Pointer to the 1 bit per pixel bit map that is exclusive ored with the screen
	\param pAnd Pointer to the 1 bit per pixel bit map that is and'd with the screen before the exclusive or
	\param uWidth Width of the cursor image in pixels
	\param uHeight Height of the cursor image in pixels
	\param iHotX X coordinate of the hot spot in the cursor
	\param iHotY Y coordinate of the hot spot in the cursor
	\return Zero on success, non-zero on failure
	\sa Shutdown()

***************************************/

#if (!defined(BURGER_MAC) && !defined(BURGER_WINDOWS)) || defined(DOXYGEN)
Word BURGER_API Burger::OSCursorImage::CreateMonoChromeImage(const Word8 * /*pXor */,const Word8 * /* pAnd */,Word uWidth,Word uHeight,Int iHotX,Int iHotY)
{
	m_uWidth = uWidth;
	m_uHeight = uHeight;
	m_iHotX = iHotX;
	m_iHotY = iHotY;
	return 10;		// Always error out
}

/*! ************************************

	\brief Release memory

	Release all allocated resources in the class.

***************************************/

void BURGER_API Burger::OSCursorImage::Shutdown(void)
{
}
#endif

/*! ************************************

	\class Burger::OSCursor
	\brief Class to handle an operating system cursor

	For desktop operating systems like MacOS, Windows and Linux,
	a mouse cursor is drawn and updated by the operating system.
	This class standardizes the behavior of the cursor and
	allows the application to show, hide or if possible, change
	the shape of the cursor in a system neutral way.

***************************************/

/*! ************************************

	\brief Singleton instance of the Burger::OSCursor class

	Since this is a shim to a global system resource, this class
	exists as a singleton.

***************************************/

Burger::OSCursor Burger::OSCursor::g_Global;

/*! ************************************

	\brief Initialize the defaults

	Set up the variables to a default state, however, no actual
	calls to the operating system are performed. Call Burger::OSCursor::Init()
	to initialize.

	\sa OSCursor::Init()

***************************************/

Burger::OSCursor::OSCursor() :
#if defined(BURGER_MAC)
	m_pCursorImage(NULL),
#endif
	m_eIDNumber(CURSOR_NONE),
	m_bVisibleFlag(TRUE),
	m_bActiveFlag(FALSE)
{
}

/*! ************************************

	\brief Release all cursor resources

	On shutdown, release any held custom cursor resources

***************************************/

Burger::OSCursor::~OSCursor()
{
	Shutdown();
}

/*! ************************************

	\fn Word Burger::OSCursor::IsVisible(void);

	\brief Return the current state of the visible flag
	\return \ref TRUE if the cursor is currently visible, \ref FALSE if not.

***************************************/

/*! ************************************

	\fn Word Burger::OSCursor::IsActive(void);

	\brief Return \ref TRUE if a cursor is visible while the mouse is in the client area
	\return \ref TRUE if a cursor should be visible in the client area of the window, \ref FALSE if not.

***************************************/

/*! ************************************

	\fn Word Burger::OSCursor::GetImageIDNumber(void);

	\brief Return the current cursor shape number
	\return The operating system resource ID of the current cursor.

***************************************/

/*! ************************************

	\fn Word Burger::OSCursor::IsThisADesktop(void);

	\brief Is there a desktop cursor?

	If the application wishes to detect if this is a desktop or mobile/console
	application, call this function to detect if there is a desktop cursor
	available.

	\return \ref TRUE if there is a desktop cursor, \ref FALSE if not.

***************************************/

#if (!defined(BURGER_MAC) && !defined(BURGER_WINDOWS)) || defined(DOXYGEN)

/*! ************************************

	\brief Load an OS resource and set the cursor to it

	Given a resource number specific to the operating system, set the cursor to it.

	\note This function will pull the cursor from the Windows Resource data or the MacOS
	Resource fork using this value as the index. Great care must be taken if this
	call is issued in platform neutral code.

***************************************/

void BURGER_API Burger::OSCursor::SetImageFromIDNumber(eCursor eCursorNumber)
{
	g_Global.m_eIDNumber = eCursorNumber;
	g_Global.m_bActiveFlag = eCursorNumber!=CURSOR_NONE;
}

/*! ************************************

	\brief Set the cursor to a generated cursor

	Given a custom cursor, set the cursor to it.
	\param pImage \ref NULL to hide the cursor, or a pointer to a generated cursor

***************************************/

void BURGER_API Burger::OSCursor::SetImage(const OSCursorImage *pImage)
{
	g_Global.m_eIDNumber = CURSOR_CUSTOM;
	g_Global.m_bActiveFlag = pImage!=0;
}

/*! ************************************

	\brief Make an OS cursor visible
	\return \ref TRUE if the cursor was previously visible, \ref FALSE if not.

***************************************/

Word BURGER_API Burger::OSCursor::Show(void)
{
	Word uResult = g_Global.m_bVisibleFlag;
	// It's visible
	g_Global.m_bVisibleFlag = TRUE;
	return uResult;
}

/*! ************************************

	\brief Make an OS cursor visible if non-zero

	If the input is a non-zero number, make the cursor visible. Otherwise,
	make it invisible.

	\param bVisible \ref TRUE to show the cursor, \ref FALSE to hide it.
	\return \ref TRUE if the cursor was previously visible, \ref FALSE if not.

***************************************/

Word BURGER_API Burger::OSCursor::Show(Word bVisible)
{
	Word uResult = g_Global.m_bVisibleFlag;
	// Set the visibility
	g_Global.m_bVisibleFlag = bVisible;
	return uResult;
}

/*! ************************************

	\brief Make an OS cursor disappear
	\return \ref TRUE if the cursor was previously visible, \ref FALSE if not.

***************************************/

Word BURGER_API Burger::OSCursor::Hide(void)
{
	Word uResult = g_Global.m_bVisibleFlag;
	// It's hidden
	g_Global.m_bVisibleFlag = FALSE;
	return uResult;
}

/*! ************************************

	\brief Reset an OS cursor to an arrow and make it visible

***************************************/

void BURGER_API Burger::OSCursor::Init(void)
{
	g_Global.m_bVisibleFlag = TRUE;
	g_Global.m_eIDNumber = CURSOR_ARROW;
	g_Global.m_bActiveFlag = TRUE;
}

/*! ************************************

	\brief Release all resources allocated by the cursor

***************************************/

void BURGER_API Burger::OSCursor::Shutdown(void)
{
}

#endif