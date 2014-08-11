/***************************************

	DIB display context class
	Windows only

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplaydib.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brwindowsapp.h"
#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <WindowsX.h>

/*! ************************************

	\class Burger::VideoContextDIB
	\brief DIB startup and teardown class

	For software renderers in a window, a DIB context is created
	and the software buffer exists in the DIB which is then
	updated to the window using the BitBlt() function.

	The window will have a Minimize and a Close box.

	\note This class exists only on Microsoft Windows targets
	
***************************************/

/*! ************************************

	\brief Default constructor.
	
	Initializes all of the internal variables
	
***************************************/

Burger::DisplayDIB::DisplayDIB(Burger::GameApp *pGameApp) :
	Display(pGameApp,DIB),
	m_pWindowDC(NULL),
	m_pBitMapDC(NULL)
{
	SetRenderer(NULL);
}

/*! ************************************

	\brief Initialize a windowed video context
	
	Given a width, height and depth (8 or 16), create a DIB
	context and resize the application window to match.

	\return Zero if no error, non-zero error code if not successful.

	\sa Shutdown()
	
***************************************/

Word Burger::DisplayDIB::InitContext()
{
	// Set the new size of the screen
	Word uWidth = m_uWidth;
	Word uHeight = m_uHeight;

	static_cast<WindowsApp *>(m_pGameApp)->SetWindowSize(uWidth,uHeight);

	HWND pWindow = static_cast<WindowsApp *>(m_pGameApp)->GetWindow();
	// Get the video contexts so drawing can commence

	m_pWindowDC = GetDC(pWindow);
	m_pBitMapDC = CreateCompatibleDC(m_pWindowDC);
	return 0;
}

/*! ************************************

	\brief Shut down a windowed video context
	
	If a video context was set up, release it.

	\sa Init()
	
***************************************/

void Burger::DisplayDIB::PostShutdown(void)
{
	if (m_pWindowDC) {
		ReleaseDC(static_cast<WindowsApp *>(m_pGameApp)->GetWindow(),m_pWindowDC);
		m_pWindowDC = NULL;
	}
	if (m_pBitMapDC) {
		ReleaseDC(static_cast<WindowsApp *>(m_pGameApp)->GetWindow(),m_pBitMapDC);
		m_pBitMapDC = NULL;
	}
}

/*! ************************************

	\brief Update the window assuming the entire buffer must be redrawn.

	\sa Update()
	
***************************************/

void Burger::DisplayDIB::PostEndScene(void)
{
	ValidateRect(static_cast<WindowsApp *>(m_pGameApp)->GetWindow(),NULL);
}

#endif
