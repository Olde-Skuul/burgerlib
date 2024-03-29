/***************************************

    4 bit font class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFONT4BIT_H__
#define __BRFONT4BIT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRRENDERER_H__
#include "brrenderer.h"
#endif

#ifndef __BRFONT_H__
#include "brfont.h"
#endif

#ifndef __BRREZFILE_H__
#include "brrezfile.h"
#endif

#ifndef __BRPALETTE_H__
#include "brpalette.h"
#endif

/* BEGIN */
namespace Burger {
class Font4Bit : public Font {
	BURGER_RTTI_IN_CLASS();		// Handle RTTI
    BURGER_DISABLE_COPY(Font4Bit);
protected:
	union {
		uint8_t Bytes[16];	///< Color of font for 8 bit rendering
		uint16_t Shorts[16];	///< Color of font for 16 bit rendering
		uint32_t Words[16];	///< Color of font for 32 bit rendering
	} m_ColorTable;			///< Storage for the color tables for rendering
	Renderer *m_pRenderer;	///< Software renderer context
	RezFile *m_pRezFile;	///< Resource file manager
	void **m_ppData;		///< Handle to the active font
	uintptr_t m_uPixelOffset;	///< Offset to the pixel array
	uint_t m_uRezNum;			///< Resource ID of the last font loaded
	uint_t m_uInvisibleColor;	///< Color to ignore for drawing
public:
	struct State_t {
		uint32_t m_ColorTable[16];	///< Colors to render with
		RezFile *m_pRezFile;		///< Resource file manager
		int m_iX;					///< X coord
		int m_iY;					///< Y coord
		uint_t m_uRezNum;				///< Resource ID of the last font loaded
		uint_t m_uInvisibleColor;		///< Color to ignore for drawing
	};
	struct RGBColorList_t {
		uint8_t m_uCount;			///< Number of entries
		RGBWord8_t m_Colors[1];	///< Actual data to set
	};
	Font4Bit(Renderer *pRenderer=NULL);
	virtual ~Font4Bit();
	uint_t GetPixelWidth(const char *pInput,uintptr_t uLength) BURGER_OVERRIDE;
	void DrawChar(uint_t uLetter) BURGER_OVERRIDE;
	void BURGER_API Init(RezFile *pRezFile,uint_t uRezNum,const uint8_t *pPalette,Renderer *pRenderer=NULL);
	void BURGER_API Shutdown(void);
	void BURGER_API SaveState(State_t *pOutput);
	void BURGER_API RestoreState(const State_t *pInput);
	void BURGER_API SetColor(uint_t uColorIndex,uint_t uColor);
	BURGER_INLINE void UseZero(void) { m_uInvisibleColor = 0x7FFF; }
	BURGER_INLINE void UseMask(void) { m_uInvisibleColor = 0; }
	void BURGER_API InstallToPalette(RezFile *pRezFile,uint_t uRezNum,const uint8_t *pPalette);
	void BURGER_API SetColorRGBListToPalette(const RGBColorList_t *pRGBList,const uint8_t *pPalette);
	void BURGER_API SetToPalette(const uint8_t *pPalette);
};
}
/* END */

#endif
