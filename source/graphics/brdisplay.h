/***************************************

	Display base class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDISPLAY_H__
#define __BRDISPLAY_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRREZFILE_H__
#include "brrezfile.h"
#endif

#ifndef __BRRECT_H__
#include "brrect.h"
#endif

/* BEGIN */
namespace Burger {
struct RGBAWord8_t;
class Display : public Base {
	BURGER_DISABLECOPYCONSTRUCTORS(Display);
public:
	enum eAPI {
		OPENGL=0,				///< Underlying API is OpenGL
		DIRECTX9,				///< Underlying API is DirectX9
		DIRECTX11,				///< Underlying API is DirectX11
		DIB,					///< Underlying API is Windows DIB
		CUSTOM					///< Underlying API is custom to the hardware
	};
	enum {
		INWINDOW=0x0,				///< The display is in a desktop window, best for debugging
		FULLSCREEN=0x1,				///< Set if full screen
		ALLOWFULLSCREENTOGGLE=0x2,	///< Set if Alt-Enter is allowed to switch from full screen to windowed mode
		FULLPALETTEALLOWED=0x40,	///< Set if all 256 colors of the palette can be used
		STEREO=0x80,				///< Set if 3D Glasses support is enabled
#if defined(_DEBUG) || defined(DOXYGEN)
		DEFAULTFLAGS=INWINDOW		///< Default window flags (Debug is in a window, release is full screen)
#else
		DEFAULTFLAGS=FULLSCREEN	///< Default window flags (NDEBUG is full screen)
#endif
	};
	typedef Word (BURGER_API *FadeProc)(void *pThis,Word uStep);		///< Callback function prototype for palette fading functions

	Display(GameApp *pGameApp,eAPI API=CUSTOM);
	virtual ~Display();
	Word Init(Word uWidth,Word uHeight,Word uDepth=32,Word uFlags=DEFAULTFLAGS);
	void Shutdown(void);
	void BeginScene(void);
	void EndScene(void);
protected:
	virtual Word InitContext(void);
	virtual void PostShutdown(void);
	virtual void PreBeginScene(void);
	virtual void PostEndScene(void);
public:
	void SetRenderer(Renderer *pRenderer);
	void SetPalette(Word uStart,Word uCount,const Word8 *pPalette);
	void SetPalette(Word uStart,Word uCount,const RGBAWord8_t *pPalette);
	void SetBorderColor(Word uColor);
	void SetWindowTitle(const char *pTitle);
	void SetPaletteBlack(void);
	void SetPaletteWhite(void);
	void SetPalette(const Word8 *pPalette);
	void SetPalette(const RGBAWord8_t *pPalette);
	void SetPalette(void **pHandle);
	void SetPalette(RezFile *pRez,Word uResID);
	void SetPalette(Word uStart,Word uCount,RezFile *pRez,Word uResID);
	void FadeTo(const Word8 *pPalette,FadeProc pProc=NULL,void *pData=NULL);
	void FadeToBlack(FadeProc pProc=NULL,void *pData=NULL);
	void FadeToWhite(FadeProc pProc=NULL,void *pData=NULL);
	void FadeTo(RezFile *pRez,Word uResID,FadeProc pProc=NULL,void *pData=NULL);
	void FadeTo(void **pHandle,FadeProc pProc=NULL,void *pData=NULL);
	BURGER_INLINE Word GetWidth(void) const { return m_uWidth; }
	BURGER_INLINE Word GetHeight(void) const { return m_uHeight; }
	BURGER_INLINE Word GetDepth(void) const { return m_uDepth; }
	BURGER_INLINE Word GetFlags(void) const { return m_uFlags; }
	BURGER_INLINE eAPI GetAPI(void) const { return m_eAPI; }
	BURGER_INLINE const Word8 *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE Word GetBorderColor(void) const { return m_uBorderColor; }
	BURGER_INLINE Word GetFadeSpeed(void) const { return m_uPaletteFadeSpeed; }
	BURGER_INLINE void SetFadeSpeed(Word uPaletteFadeSpeed) { m_uPaletteFadeSpeed = uPaletteFadeSpeed; }
	BURGER_INLINE Word GetPaletteVSync(void) const { return m_bPaletteVSync; }
	BURGER_INLINE void SetPaletteVSync(Word bPaletteVSync) { m_bPaletteVSync=bPaletteVSync; }

protected:
	GameApp *m_pGameApp;		///< Pointer to the game application instance
	Renderer *m_pRenderer;		///< Pointer to a renderer
	Word m_bPaletteDirty;		///< \ref TRUE if the palette buffer was changed
	Word m_uWidth;				///< Width in pixels of the display buffer
	Word m_uHeight;				///< Height in pixels of the display buffer
	Word m_uDepth;				///< Depth in bits of the display buffer
	Word m_uFlags;				///< Flags for describing available features
	Word m_uBorderColor;		///< Hardware border color (MSDOS / Amiga only)
	Word m_bPaletteVSync;		///< \ref TRUE if palette updates sync to video
	Word m_uPaletteFadeSpeed;	///< Speed in 1/60ths of a second for a palette fade
	eAPI m_eAPI;				///< Underlying display API
	Word8 m_Palette[256*3];		///< Palette of 256 RGB values
};
}
/* END */

#endif
