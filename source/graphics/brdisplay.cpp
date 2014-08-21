/***************************************

	Display base class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplay.h"
#include "brtick.h"
#include "brrenderer.h"
#include "brpalette.h"

/*! ************************************

	\class Burger::Display

	\brief Base class for instantiating a video display and the API to drive it.

	This class manages video displays and resources. It does not manage
	rendering functions, use the Renderer class for that.

	Since palettes are tied to a video display, this class will manage
	palettes for paletted display contexts. If the display is in true color,
	palette functions will only update internal buffers but will not affect
	actual rendering.

	Classes for specific rendering APIs derive from this base class.

	\sa Burger::Renderer, Burger::Display, Burger::DisplayOpenGL, Burger::DisplayDirectX9,
		Burger::DisplayDirectX11, Burger::DisplayOpenGLSoftware16 or Burger::DisplayOpenGLSoftware8

***************************************/

/*! ************************************

	\brief Default constructor.
	
	Initializes all of the shared variables and
	hooks up the Display and the Renderer to the GameApp.

	Variables are initialized, but the display is not activated.
	Call Init(Word,Word,Word,Word) to activate the display.
	\param pGameApp Pointer to the game application
	\param API Derived class API identifier

	\sa Init(Word,Word,Word,Word)

***************************************/

Burger::Display::Display(GameApp *pGameApp,eAPI API) :
	m_pGameApp(pGameApp),
	m_pRenderer(NULL),
	m_bPaletteDirty(TRUE),
	m_uBorderColor(0),
	m_bPaletteVSync(FALSE),
	m_uPaletteFadeSpeed(Tick::TICKSPERSEC/15),
	m_eAPI(API)
{
	MemoryClear(m_Palette,sizeof(m_Palette));
#if defined(BURGER_MACOS)
	m_Palette[0*3+0] = 255;
	m_Palette[0*3+1] = 255;
	m_Palette[0*3+2] = 255;
#else
	m_Palette[255*3+0] = 255;
	m_Palette[255*3+1] = 255;
	m_Palette[255*3+2] = 255;
#endif
	// Set the GameApp to point to the active renderer
	pGameApp->SetDisplay(this);
	pGameApp->SetRenderer(NULL);
}

/*! ************************************

	\brief Default destructor.
	
	Shuts down the video context and issues a shutdown to the renderer
	
	\sa Shutdown() and SetRenderer()

***************************************/

Burger::Display::~Display()
{
	// Shut down the attached renderer first!
	SetRenderer(NULL);
	// Make sure everything is released
	Shutdown();
}

/*! ************************************

	\fn Word Burger::Display::Init(Word uWidth,Word uHeight,Word uDepth,Word uFlags)
	\brief Initialize the display

	Set up the video display hardware to the specified mode and depth.
	This calls derived functions and it's the responsiblity of the underlying class to
	use the appropriate API such as OpenGL, DirectX or something else to perform
	the display operations and then call the base class to complete the setup

	\param uWidth Width of the requested display in pixels
	\param uHeight Height of the requested display in pixels
	\param uDepth Bit depth of the requested display in bits
	\param uFlags Helper flags for hinting at the preferred display
	\return Zero if no error, non-zero if an error has occured.

	\sa Shutdown() and InitContext()
	
***************************************/

Word Burger::Display::Init(Word uWidth,Word uHeight,Word uDepth,Word uFlags)
{
	// Initialize my desired context
	m_uWidth = uWidth;
	m_uHeight = uHeight;
	m_uDepth = uDepth;
	m_uFlags = uFlags;

	// Set up the hardware
	Word uResult = InitContext();
	if (uResult) {
		// If there's a logged renderer, set it up.
		Renderer *pRenderer = m_pRenderer;
		if (pRenderer) {
			uResult = pRenderer->Init(m_uWidth,m_uHeight,m_uDepth,m_uFlags);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Shut down the current video display context

	Shut down any attached renderer and then shut down the video display.

	\sa Init(Word,Word,Word,Word)
	
***************************************/

void Burger::Display::Shutdown(void)
{
	// First, release the renderer, since
	// it may depend on things in the Display
	// class
	Renderer *pRenderer = m_pRenderer;
	if (pRenderer) {
		pRenderer->Shutdown();
	}
	// Clear out the display class
	PostShutdown();
}

/*! ************************************

	\brief Prepare the display for rendering.
	
	This function will call the Renderer::BeginScene() function
	after any operating system calls are issued so the renderer can prepare for drawing.

	This must be paired with a subsequent call to Burger::Display::EndScene()

	\sa Burger::Display::EndScene()
	
***************************************/

void Burger::Display::BeginScene(void)
{
	// Call the derived class for all set up functions
	PreBeginScene();
	// Call the renderer to set up rendering
	Renderer *pRenderer = m_pRenderer;
	if (pRenderer) {
		pRenderer->BeginScene();
	}
}

/*! ************************************

	\brief Render the scene to the display

	Alert the operating system that all rendering calls are complete
	and the hardware is permitted to begin rendering the scene.

	The renderer is issued a call to Renderer::EndScene() before
	the operating system so all rendering calls can be completed before
	the scene is sent to the GPU or other rendering sub-system.
	
	\sa Burger::Display::BeginScene()
	
***************************************/

void Burger::Display::EndScene(void)
{
	// Wrap up the rendering engine
	Renderer *pRenderer = m_pRenderer;
	if (pRenderer) {
		pRenderer->EndScene();
	}
	// Present the scene to the display
	PostEndScene();
}

/*! ************************************

	\brief Initialize the video display hardware

	Set up all of the video display hardware since the state has changed

	This function should never be called. It's here to force an error
	when bringing up a new version of Burgerlib

	\return Zero if no error, non-zero if an error has occured.
	\sa Burger::Display::Init(Word,Word,Word,Word)

***************************************/

Word Burger::Display::InitContext(void)
{
	return 10;
}

/*! ************************************

	\brief Shut down the current video display context

	Release all resources and restore the video display to the 
	system defaults. This is an internal function and is meant
	to be called as part of a call to Display::Shutdown()
	so the renderer is shut down first before the display

	This code does nothing. It's a placeholder for classes that have no
	need for a shutdown call
	
	\sa Burger::Display::Shutdown()

***************************************/

void Burger::Display::PostShutdown(void)
{
}

/*! ************************************

	\brief Prepare the display for rendering

	This is called before the renderer is called 
	to begin a scene so the video display can perform any
	hardware initialization before rendering is to begin
	
	This code does nothing. It's a placeholder for classes that have no
	need for this part of the rendering pipeline

	\sa Burger::Display::PostEndScene()

***************************************/

void Burger::Display::PreBeginScene(void)
{
}

/*! ************************************

	\brief Rendering is completed so update the display to the new scene

	This is called after the renderer is called 
	to end a scene so the video display can perform any
	hardware initialization after rendering is completed

	This code does nothing. It's a placeholder for classes that have no
	need for this part of the rendering pipeline

	\sa Burger::Display::PreBeginScene()

***************************************/

void Burger::Display::PostEndScene(void)
{
}

/*! ************************************

	\brief Sets a new rendering context

	Shuts down any previous rendering context and attaches
	to the new context. If the context is \ref NULL, a
	null driver is installed that performs no actions.
	
	\param pRenderer Pointer to a valid Renderer for enabling the video display
	
	\sa Burger::Renderer::Init(Word,Word,Word,Word) or Burger::GameApp::SetRenderer()

***************************************/

void Burger::Display::SetRenderer(Renderer *pRenderer)
{
	// Release the old context if there was a difference
	Renderer *pOldRenderer = m_pRenderer;
	if (pOldRenderer != pRenderer) {
		if (pOldRenderer) {
			pOldRenderer->Shutdown();
			pOldRenderer->SetDisplay(NULL);
		}
	}
	// Set the new context, and if one is present, connect to it.
	m_pRenderer = pRenderer;
	m_pGameApp->SetRenderer(pRenderer);
	if (pRenderer) {
		pRenderer->SetDisplay(this);
		pRenderer->Init(m_uWidth,m_uHeight,m_uDepth,m_uFlags);
	}
	// Assume the palette is NOT updated
	m_bPaletteDirty = TRUE;
}

/*! ************************************

	\brief Update the color palette

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of 3 byte triplettes of Red,Green, and Blue.

	\param uStart First color entry to update (0-255)
	\param uCount Number of colors to update (256-uStart)
	\param pPalette Base pointer to the colors to use in the update in the size of uCount*3
	
	\sa SetPalette(const Word8 *) or SetPalette(void **)

***************************************/

void Burger::Display::SetPalette(Word uStart,Word uCount,const Word8 *pPalette)
{
	// Bad?
	if (pPalette && uStart<256) {
		// Out of range?
		if ((uStart+uCount)>=257) {
			// Set the limit on the count
			uCount = 256-uStart;
		}
		// Are colors 0 and 255 reserved?
		if (!(m_uFlags & FULLPALETTEALLOWED)) {

			// Hard code the first and last colors to black and white
			// Note: MacOS has black and white reversed!
#if defined(BURGER_MACOS)
			m_Palette[0*3+0] = 255;
			m_Palette[0*3+1] = 255;
			m_Palette[0*3+2] = 255;
			m_Palette[255*3+0] = 0;
			m_Palette[255*3+1] = 0;
			m_Palette[255*3+2] = 0;
#else
			m_Palette[0*3+0] = 0;
			m_Palette[0*3+1] = 0;
			m_Palette[0*3+2] = 0;
			m_Palette[255*3+0] = 255;
			m_Palette[255*3+1] = 255;
			m_Palette[255*3+2] = 255;
#endif
			if (uCount) {
				// Starting at color #0?
				if (!uStart) {
					// Remove it from the update list
					++uStart;
					--uCount;
					pPalette+=3;
				}
			}

			// Only updating the last color?
			if (uStart>=255) {
				uCount = 0;		// Kill it
			}
			// Is the last color part of the range?
			if (uStart+uCount==255) {
				--uCount;		// Remove it from the end (254 is the highest allowed)
			}
		}
		// If any colors survived the pruning, update them
		if (uCount && (m_bPaletteDirty || MemoryCompare(m_Palette+(uStart*3),pPalette,uCount*3)) ) {
			MemoryCopy(m_Palette+(uStart*3),pPalette,uCount*3);	// Update the palette
			m_bPaletteDirty = TRUE;
		}
	}
}

/*! ************************************

	\brief Update the color palette

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of RGBAWord8_t entries.

	\param uStart First color entry to update (0-255)
	\param uCount Number of colors to update (256-uStart)
	\param pPalette Base pointer to an array of RGBAWord8_t colors to use in the update in the size of uCount
	
	\sa SetPalette(const Word8 *) or SetPalette(void **)

***************************************/

void Burger::Display::SetPalette(Word uStart,Word uCount,const RGBAWord8_t *pPalette)
{
	// Bad?
	if (pPalette && uStart<256) {
		// Out of range?
		if ((uStart+uCount)>=257) {
			// Set the limit on the count
			uCount = 256-uStart;
		}
		// Are colors 0 and 255 reserved?
		if (!(m_uFlags & FULLPALETTEALLOWED)) {

			// Hard code the first and last colors to black and white
			// Note: MacOS has black and white reversed!
#if defined(BURGER_MACOS)
			m_Palette[0*3+0] = 255;
			m_Palette[0*3+1] = 255;
			m_Palette[0*3+2] = 255;
			m_Palette[255*3+0] = 0;
			m_Palette[255*3+1] = 0;
			m_Palette[255*3+2] = 0;
#else
			m_Palette[0*3+0] = 0;
			m_Palette[0*3+1] = 0;
			m_Palette[0*3+2] = 0;
			m_Palette[255*3+0] = 255;
			m_Palette[255*3+1] = 255;
			m_Palette[255*3+2] = 255;
#endif
			if (uCount) {
				// Starting at color #0?
				if (!uStart) {
					// Remove it from the update list
					++uStart;
					--uCount;
					++pPalette;
				}
			}

			// Only updating the last color?
			if (uStart>=255) {
				uCount = 0;		// Kill it
			}
			// Is the last color part of the range?
			if (uStart+uCount==255) {
				--uCount;		// Remove it from the end (254 is the highest allowed)
			}
		}
		// If any colors survived the pruning, update them
		if (uCount) {
			Word8 *pDest = m_Palette+(uStart*3);
			// Copy up the colors, ignoring the alpha
			do {
				pDest[0] = pPalette->m_uRed;
				pDest[1] = pPalette->m_uGreen;
				pDest[2] = pPalette->m_uBlue;
				++pPalette;
				pDest+=3;
			} while (--uCount);
			m_bPaletteDirty = TRUE;
		}
	}
}

/*! ************************************

	\brief Update the display border color

	On some platforms, such as MSDOS VGA/MCGA or the Commodore Amiga,
	the display's border color can be set. This function will set
	that color. On all other platforms, the function only logs the
	color change, otherwise it does nothing.

	\note On MSDOS, the EGA value of the border color is 4 bits (0-15).

	\param uColor Border color that is specific to the hardware being updated.
	
	\sa GetBorderColor()

***************************************/

#if (!defined(BURGER_MSDOS)) || defined(DOXYGEN)
void Burger::Display::SetBorderColor(Word uColor)
{
	m_uBorderColor = uColor;
}
#endif

/*! ************************************

	\brief Set the display window title
	
	On desktop platforms such as Windows or MacOS,
	the game could be running in a desktop window.
	The window can have a title string, and it can be 
	set using this function.

	On consoles and handhelds, this function does nothing.
	\param pTitle UTF-8 string to display for the title bar
	
***************************************/

#if (!defined(BURGER_WINDOWS)) || defined(DOXYGEN)
void Burger::Display::SetWindowTitle(const char * /* pTitle */)
{
}
#endif


/*! ************************************

	\brief Set the screen palette to all black

	Set all 8 bit palette color entries to zero (Black)

	\sa SetPalette(const Word8 *), SetPalette(Word,Word,const Word8 *) or SetPaletteWhite()

***************************************/

void Burger::Display::SetPaletteBlack(void)
{
	// Perform a compare to the palette to force an update only when changed
	Word8 TempPalette[sizeof(m_Palette)];
	MemoryClear(m_Palette,sizeof(m_Palette));
	SetPalette(TempPalette);
}

/*! ************************************

	\brief Set the screen palette to all white

	Set all 8 bit palette color entries to 255 (White)

	\sa SetPalette(const Word8 *), SetPalette(Word,Word,const Word8 *) or SetPaletteBlack()

***************************************/

void Burger::Display::SetPaletteWhite(void)
{
	Word8 TempPalette[sizeof(m_Palette)];
	MemoryFill(TempPalette,255,sizeof(TempPalette));
	SetPalette(TempPalette);
}

/*! ************************************

	\brief Update the color palette

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of 3 byte triplettes of Red,Green, and Blue.

	\param pPalette Base pointer to the colors to use in the update in the size of 256*3 (768)
	\sa SetPalette(void **)

***************************************/

void Burger::Display::SetPalette(const Word8 *pPalette)
{
	SetPalette(0,256,pPalette);
}

/*! ************************************

	\brief Update the color palette

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of RGBAWord8_t.

	\param pPalette Base pointer to the array of 256 RGBAWord8_t colors to use in the update
	\sa SetPalette(void **)

***************************************/

void Burger::Display::SetPalette(const RGBAWord8_t *pPalette)
{
	SetPalette(0,256,pPalette);
}

/*! ************************************

	\brief Update the color palette using a memory handle

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of 3 byte triplettes of Red,Green, and Blue.

	\param pHandle Base handle to the colors to use in the update in the size of 256*3 (768)
	\sa SetPalette(const Word8 *)

***************************************/

void Burger::Display::SetPalette(void **pHandle)
{
	const Word8 *pPalette = static_cast<const Word8 *>(MemoryManagerHandle::Lock(pHandle));
	if (pPalette) {
		SetPalette(0,256,pPalette);
		MemoryManagerHandle::Unlock(pHandle);
	}
}

/*! ************************************

	\brief Update the color palette using a resource

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of 3 byte triplettes of Red,Green, and Blue.

	\param pRez Reference to the resource file
	\param uResID Resource entry the contains the 768 byte palette
	\sa SetPalette(const Word8 *)

***************************************/

void Burger::Display::SetPalette(RezFile *pRez,Word uResID)
{
	const Word8 *pPalette = static_cast<const Word8 *>(pRez->Load(uResID));
	if (pPalette) {
		SetPalette(0,256,pPalette);
		pRez->Release(uResID);
	}
}

/*! ************************************

	\brief Update a partial color palette using a resource

	On 8 bit rendering surfaces, update the color palette for the current
	hardware or rendering context. On all other rendering modes, it will
	update the internal color buffer.

	The palette is an array of 3 byte triplettes of Red,Green, and Blue.

	\param uStart First color entry to update (0-255)
	\param uCount Number of colors to update (256-uStart)
	\param pRez Reference to the resource file
	\param uResID Resource entry the contains the palette that's uCount*3 bytes in length
	\sa SetPalette(const Word8 *)

***************************************/

void Burger::Display::SetPalette(Word uStart,Word uCount,RezFile *pRez,Word uResID)
{
	const Word8 *pPalette = static_cast<const Word8 *>(pRez->Load(uResID));
	if (pPalette) {
		SetPalette(uStart,uCount,pPalette);
		pRez->Release(uResID);
	}
}

/*! ************************************

	\brief Fade the hardware palette to a palette

	Given an arbitrary palette, and a preset palette speed variable,
	fade from the currently shown palette to the new arbitrary palette
	using smooth steps.

	If the requested palette is the same as Display::m_Palette, this routine
	does nothing.

	Every time the hardware palette is written to, the function pProc()
	is called if the user has set it to a valid function pointer. It is passed a 0 on startup
	and the value increments until it reaches 16 when the palette
	is finished. Only parameter 0 and 16 are guaranteed to be called. All
	other values will only be passed once or skipped due to low machine
	speed.

	\note Running under Windows or BeOS, the thread that calls this routine will
	be put to sleep during time delays between Vertical Blank (VBL) palette updates.
	Keyboard::GetKeyEvent() will only be called no more than the VBL rate.
	All other platforms will wait between VBL's with CPU polling.

	\param pPalette Pointer to the 768 byte palette to fade to.
	\param pProc Function pointer to callback or \ref NULL if callbacks are not desired
	\param pData Data pointer for the callback. Can be \ref NULL.

	\sa FadeTo(RezFile *,Word,,FadeProc,void *), FadeToWhite(), FadeToBlack(), GameApp::Poll(),
		m_uPaletteFadeSpeed, m_bPaletteVSync, m_Palette, Tick::Read()

***************************************/

void Burger::Display::FadeTo(const Word8 *pPalette,FadeProc pProc,void *pData)
{
	int DeltaPalette[768];		// Must be SIGNED!
	Word8 WorkPalette[768];		// Temp palette

	// Same palette?
	if (MemoryCompare(pPalette,m_Palette,768)) {
		// Save the palette VSync flag
		// Since I am fading, I can wait for VSync
		Word OldVSync = m_bPaletteVSync;		
		m_bPaletteVSync = TRUE;		

		// Need to first get the deltas for each color component
		// Since a char is -127 to 128 and the differences are -255 to 255
		// I need the difference table to be an array of int's

		{
			const Word8 *pOriginal = m_Palette;	// Pointer to palette
			int *pDelta = DeltaPalette;			// Pointer to delta
			do {
				Word a1 = pOriginal[0];			// Get the values
				Word a2 = pPalette[0];
				pDelta[0] = static_cast<int>(a1-a2);		// Calculate the delta
				++pOriginal;
				++pPalette;
				++pDelta;
			} while (pOriginal<&m_Palette[768]);
		}
		// Restore the palette pointer
		pPalette = pPalette-768;

		{
			// Palette scale temp 0.0-1.0
			Fixed32 fScale;
			// Initialize the callback constant
			Word LastCall = 0;

			// Get the time base
			Word32 uMark = Tick::Read();
			// Number of ticks to elapse for 16 steps
			Word uTotalTicks = 16*m_uPaletteFadeSpeed;
			do {
				// Yield CPU time if needed
				m_pGameApp->Poll();

				// Get elapsed time
				fScale = static_cast<Fixed32>(Tick::Read()-uMark);
				fScale = fScale*(0x10000/static_cast<Fixed32>(uTotalTicks));
				if (fScale>0x10000) {		// Overflow?
					fScale = 0x10000;		// Cap at 1.0f fixed
				}
				fScale=0x10000-fScale;		// Reverse the scale
				Word8 *pNewPalette = WorkPalette;	// Init palette pointers
				int *pDelta = DeltaPalette;
				do {
					// Get a delta
					Fixed32 Foo = pDelta[0];
					// Scale the delta by 0.0 to 1.0
					Foo = Foo*fScale;
					// Div by 16 (Signed) (Result is -255 to 255)
					Foo = Foo>>16;
					pNewPalette[0] = static_cast<Word8>(pPalette[0]+Foo);
					++pNewPalette;
					++pPalette;
					++pDelta;
				} while (pNewPalette<&WorkPalette[768]);
				// Restore the pointer
				pPalette = pPalette-768;
				// Set the new palette
				SetPalette(0,256,WorkPalette);
				PostEndScene();

				// Is there a callback?
				if (pProc) {
					// 0-16
					Word uCount = static_cast<Word>(16-(fScale>>12));
					// New value?
					if (uCount>LastCall) {
						LastCall = uCount;
						pProc(pData,uCount);	// Call the callback routine
					}
				}
				// All done?
			} while (fScale);
		}
		// Restore the sync value
		m_bPaletteVSync = OldVSync;
	} else {
		// On occassions where there is no palette change, alert any callback that the
		// stepping concluded
		if (pProc) {
			pProc(pData,16);
		}
	}
}

/*! ************************************

	\brief Fade the hardware palette to black

	All color entries in the hardware palette are
	set to black slowly over time.

	\param pProc Function pointer to callback or \ref NULL if callbacks are not desired
	\param pData Data pointer for the callback. Can be \ref NULL.
	\sa FadeTo(const Word8 *,FadeProc,void *), SetPaletteBlack(), FadeToWhite()

***************************************/

void Burger::Display::FadeToBlack(FadeProc pProc,void *pData)
{
	Word8 TempPalette[sizeof(m_Palette)];
	MemoryClear(TempPalette,sizeof(TempPalette));
	FadeTo(TempPalette,pProc,pData);
}

/*! ************************************

	\brief Fade the hardware palette to white

	All color entries in the hardware palette are
	set to white slowly over time.

	\param pProc Function pointer to callback or \ref NULL if callbacks are not desired
	\param pData Data pointer for the callback. Can be \ref NULL.

	\sa FadeTo(const Word8 *,FadeProc,void *), SetPaletteWhite(), FadeToBlack()

***************************************/

void Burger::Display::FadeToWhite(FadeProc pProc,void *pData)
{
	Word8 TempPalette[sizeof(m_Palette)];
	MemoryFill(TempPalette,255,sizeof(TempPalette));
	FadeTo(TempPalette,pProc,pData);
}

/*! ************************************

	\brief Fade the hardware palette to a palette

	All color entries in the hardware palette are
	set to the new palette slowly over time.

	\param pRez Reference to the resource file that has the palette to fade to.
	\param uResID The resource number of the palette
	\param pProc Function pointer to callback or \ref NULL if callbacks are not desired
	\param pData Data pointer for the callback. Can be \ref NULL.

	\sa FadeTo(const Word8 *,FadeProc,void *), FadeToWhite(), FadeToBlack()

***************************************/

void Burger::Display::FadeTo(RezFile *pRez,Word uResID,FadeProc pProc,void *pData)
{
	// Load in the resource file
	const Word8 *pPalette = static_cast<const Word8 *>(pRez->Load(uResID));
	if (pPalette) {
		FadeTo(pPalette,pProc,pData);
		// Release the resource data
		pRez->Release(uResID);
	}
}

/*! ************************************

	\brief Fade the hardware palette to a palette
	
	All color entries in the hardware palette are
	set to the new palette slowly over time.

	The handle is returned unlocked. Nothing is performed if the
	handle is invalid or purged.

	\param pHandle Handle to the palette to fade to.
	\param pProc Function pointer to callback or \ref NULL if callbacks are not desired
	\param pData Data pointer for the callback. Can be \ref NULL.

	\sa FadeTo(const Word8 *,FadeProc,void *), FadeToWhite(), FadeToBlack()

***************************************/

void Burger::Display::FadeTo(void **pHandle,FadeProc pProc,void *pData)
{
	// Lock it down
	const Word8 *pPalette = static_cast<const Word8 *>(MemoryManagerHandle::Lock(pHandle));
	if (pPalette) {
		// Perform the fade
		FadeTo(pPalette,pProc,pData);
		// Release the memory
		MemoryManagerHandle::Unlock(pHandle);
	}
}

/*! ************************************

	\fn Word Burger::Display::GetWidth(void) const
	\brief Get the width in pixels of the display buffer
	
	\return Width of the display buffer in pixels
	\sa GetHeight() const or GetDepth() const
	
***************************************/

/*! ************************************

	\fn Word Burger::Display::GetHeight(void) const
	\brief Get the height in pixels of the display buffer
	
	\return Height of the display buffer in pixels
	\sa GetWidth() const or GetDepth() const
	
***************************************/

/*! ************************************

	\fn Word Burger::Display::GetDepth(void) const
	\brief Get the depth in bits of the display buffer
	
	The display buffer could be 8 for 8 bit paletted, 15 for 5:5:5 RGB, 16
	for 5:6:5 RGB or 24 or 32 for hardware rendering

	\return Depth of the display buffer in bits
	\sa GetWidth() const or GetHeight() const
	
***************************************/
	
/*! ************************************

	\fn Word Burger::Display::GetFlags(void) const
	\brief Get the flags associated with this Display class instance
	
	\return Flags containing the current state of the display system
	\sa GetWidth() const or GetHeight() const
	
***************************************/

/*! ************************************

	\fn const Word8 * Burger::Display::GetPalette(void) const
	\brief Get the current palette

	When video is set to an 8 bit mode, a 256 entry color palette is needed
	for the video display to properly show the graphics. A copy of
	the palette is maintained in the class that matches what
	the hardware is currently displaying. For some displays, some
	colors are considered read only. When calling SetPalette(Word,Word,const Word8 *)
	it's not guaranteed that all colors will be updated due to system
	reserved colors (For windowed modes). The palette will have
	the reserved colors in it if this is the case.
	
	\return Pointer to a 768 byte array of Red,Green,Blue color components
	\sa SetPalette()

***************************************/

/*! ************************************

	\fn Burger::Display::GetBorderColor(void) const
	\brief Get the hardware border color

	On some platforms, such as MSDOS VGA/MCGA or the Commodore Amiga,
	the display's border color can be set. This function will return
	that color.

	\note On MSDOS, the EGA value of the border color is 4 bits (0-15).

	\return Value previously set by SetBorderColor(Word) or zero if uninitialized.

	\sa SetBorderColor(Word)

***************************************/

/*! ************************************

	\fn Display::GetAPI(void) const
	\brief Return the underlying API for the Display

	\return Burger::Display::OPENGL, Burger::Display::DIRECTX9, Burger::Display::DIRECTX11 or Burger::Display::CUSTOM
	\sa Display::GetFlags() const

***************************************/

/*! ************************************

	\fn Display::GetFadeSpeed(void) const
	\brief Return the timer constant in Burger::Tick

	When calling the palette fade functions, it will perform the fade evenly until
	this amount of time has elapsed. This is not a "per frame" time. It's
	a total time. If the desired time is one second from start to finish, set this value
	to Tick::TICKSPERSEC

	\return Return the current tick value for a palette fade
	\sa SetFadeSpeed(Word)

***************************************/

/*! ************************************

	\fn Display::SetFadeSpeed(Word uPaletteFadeSpeed) const
	\brief Set the timer constant in Burger::Tick

	When calling the palette fade functions, it will perform the fade evenly until
	this amount of time has elapsed. This is not a "per frame" time. It's
	a total time. If the desired time is one second from start to finish, set this value
	to Tick::TICKSPERSEC. Setting this value to zero will disable the fade feature
	and will have palettes update immediately

	\param uPaletteFadeSpeed New time delay in ticks.
	\sa GetFadeSpeed()

***************************************/

/*! ************************************

	\fn Display::GetPaletteVSync(void) const
	\brief Return non-zero if palette updates are synced to vertical blank.

	\return Return non-zero if palette updates are synced to vertical blank.
	\sa SetPaletteVSync(Word)

***************************************/

/*! ************************************

	\fn Display::SetPaletteVSync(Word bPaletteVSync)
	\brief Set the flag to enable palette updates 

	\param bPaletteVSync \ref TRUE to enable vertical blank syncing, \ref FALSE to disable it
	\sa GetPaletteVSync() const

***************************************/


