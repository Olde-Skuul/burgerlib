/***************************************

	DIB display context class
	Windows only

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplaydibsoftware8.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brwindowsapp.h"
#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>
#include <WindowsX.h>

/*! ************************************

	\class Burger::DisplayDIBSoftware8
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

Burger::DisplayDIBSoftware8::DisplayDIBSoftware8(Burger::GameApp *pGameApp) :
	DisplayDIB(pGameApp),
	m_pBitMap(NULL),
	m_pHPalette(NULL),
	m_pHBitMap(NULL)
{
	SetRenderer(&m_Renderer);
}


/*! ************************************

	\brief Initialize a windowed video context
	
	Given a width, height and depth (8 or 16), create a DIB
	context and resize the application window to match.

	\return Zero if no error, non-zero error code if not successful.

	\sa Shutdown()
	
***************************************/

Word Burger::DisplayDIBSoftware8::InitContext()
{
	Word uResult = DisplayDIB::InitContext();
	if (!uResult) {
		m_uDepth = 8;
		struct {
			BITMAPINFO h;
			RGBQUAD extraColors[256];
		} MyBitMapInfo;

		// First 255 colors are black
		Word j = 256-1;
		RGBQUAD *pBitMapColors = MyBitMapInfo.h.bmiColors;
		do {
			pBitMapColors->rgbRed = 0;
			pBitMapColors->rgbGreen = 0;
			pBitMapColors->rgbBlue = 0;
			pBitMapColors->rgbReserved = 0;
			++pBitMapColors;
		} while (--j);
		// Make the last color white
		pBitMapColors->rgbRed = 255;
		pBitMapColors->rgbGreen = 255;
		pBitMapColors->rgbBlue = 255;
		pBitMapColors->rgbReserved = 0;

		MyBitMapInfo.h.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		MyBitMapInfo.h.bmiHeader.biWidth = static_cast<LONG>(m_uWidth);
		// Note: DIBs are upside down, using -uHeight flips it vertically
		MyBitMapInfo.h.bmiHeader.biHeight = -static_cast<LONG>(m_uHeight);
		MyBitMapInfo.h.bmiHeader.biPlanes = 1;			// Must be one, don't change!
		MyBitMapInfo.h.bmiHeader.biBitCount = static_cast<WORD>(m_uDepth);
		MyBitMapInfo.h.bmiHeader.biCompression = BI_RGB;
		MyBitMapInfo.h.bmiHeader.biSizeImage = m_uHeight*m_uWidth;
		MyBitMapInfo.h.bmiHeader.biXPelsPerMeter = 0;
		MyBitMapInfo.h.bmiHeader.biYPelsPerMeter = 0;
		MyBitMapInfo.h.bmiHeader.biClrUsed = 256;
		MyBitMapInfo.h.bmiHeader.biClrImportant = 0;
		m_pHBitMap = CreateDIBSection(m_pWindowDC,&MyBitMapInfo.h,DIB_RGB_COLORS,reinterpret_cast<void **>(&m_pBitMap),0,0);
		m_Renderer.SetFrameBuffer(m_pBitMap);
		// Calculate the bytes per scan line
		// DIBs require DWORD alignment
		m_Renderer.SetStride(((m_uWidth*(8>>3))+3)&(~3));

		// Create the palette
		struct {
			LOGPALETTE Header;
			PALETTEENTRY palPalEntry[256];
		} WorkPalette;
		WorkPalette.Header.palVersion = 0x0300;
		WorkPalette.Header.palNumEntries = 256;
		Word i = 256-1;
		PALETTEENTRY *pPalEntry = WorkPalette.Header.palPalEntry;
		do {
			pPalEntry->peRed = 0;
			pPalEntry->peGreen = 0;
			pPalEntry->peBlue = 0;
			pPalEntry->peFlags = 0;		// PC_EXPLICIT | PC_NOCOLLAPSE | PC_RESERVED
		} while (--i);
		pPalEntry->peRed = 255;
		pPalEntry->peGreen = 255;
		pPalEntry->peBlue = 255;
		pPalEntry->peFlags = 0;		// PC_EXPLICIT | PC_NOCOLLAPSE | PC_RESERVED
		m_pHPalette = CreatePalette(&WorkPalette.Header);
		SelectPalette(m_pWindowDC,m_pHPalette,FALSE);
	}
	return uResult;
}

/*! ************************************

	\brief Shut down a windowed video context
	
	If a video context was set up, release it.

	\sa Init()
	
***************************************/

void Burger::DisplayDIBSoftware8::PostShutdown(void)
{
	// Disable all future access to the frame buffer

	// If there was a bitmap, kill it.
	if (m_pHBitMap) {
		DeleteObject(m_pHBitMap);
		m_pHBitMap = NULL;

		// Because there was a video context, capture the location
		// of the window, so if the window was re-opened, use it's
		// old location.

		static_cast<WindowsApp *>(m_pGameApp)->RecordWindowLocation();
	}

	// Release everything else

	if (m_pHPalette) {
		DeleteObject(m_pHPalette);
		m_pHPalette = NULL;
	}
	DisplayDIB::PostShutdown();
}

/*! ************************************

	\brief Update the window assuming the entire buffer must be redrawn.

	\sa Update()
	
***************************************/

void Burger::DisplayDIBSoftware8::PostEndScene(void)
{
	if (m_bPaletteDirty) {
		if (m_pHPalette) {
			const Word8* pPalette = m_Palette;
			PALETTEENTRY WinPal[256];
			Word uTemp = 254;
			PALETTEENTRY *pWinPal = WinPal;
			pWinPal->peRed = 0;
			pWinPal->peGreen = 0;
			pWinPal->peBlue = 0;
			pWinPal->peFlags = 0;
			++pWinPal;
			pPalette+=3;
			do {
				pWinPal->peRed = pPalette[0];
				pWinPal->peGreen = pPalette[1];
				pWinPal->peBlue = pPalette[2];
				pWinPal->peFlags = 0;
				pPalette = pPalette+3;
				++pWinPal;
			} while (--uTemp);
			pWinPal->peRed = 255;
			pWinPal->peGreen = 255;
			pWinPal->peBlue = 255;
			pWinPal->peFlags = 0;

			// If GDI is present, set it too.

			SetPaletteEntries(m_pHPalette,0,256,WinPal);
			
			// Convert PALETTEENTRIES to RGBQUADS
			uTemp = 256;
			pWinPal = WinPal;
			do {
				Word8 Temp2 = pWinPal->peRed;
				pWinPal->peRed = pWinPal->peBlue;
				pWinPal->peBlue = Temp2;
				++pWinPal;
			} while (--uTemp);
			// Select the bitmap
			SelectObject(m_pBitMapDC,m_pHBitMap);
			// Set the palette
			SetDIBColorTable(m_pBitMapDC,0,256,reinterpret_cast<RGBQUAD *>(WinPal));
			m_bPaletteDirty = FALSE;
		}
	}
	RealizePalette(m_pWindowDC);			// Make sure the palette is set
	SelectObject(m_pBitMapDC,m_pHBitMap);	// Select my temp bitmap
	BitBlt(m_pWindowDC,0,0,static_cast<int>(m_uWidth),static_cast<int>(m_uHeight),m_pBitMapDC,0,0,SRCCOPY);
	DisplayDIB::PostEndScene();
}

#endif
