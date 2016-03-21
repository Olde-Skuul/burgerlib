/***************************************

	4 bit font class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	BURGER_DISABLECOPYCONSTRUCTORS(Font4Bit);
protected:
	union {
		Word8 Bytes[16];	///< Color of font for 8 bit rendering
		Word16 Shorts[16];	///< Color of font for 16 bit rendering
		Word32 Words[16];	///< Color of font for 32 bit rendering
	} m_ColorTable;			///< Storage for the color tables for rendering
	Renderer *m_pRenderer;	///< Software renderer context
	RezFile *m_pRezFile;	///< Resource file manager
	void **m_ppData;		///< Handle to the active font
	WordPtr m_uPixelOffset;	///< Offset to the pixel array
	Word m_uRezNum;			///< Resource ID of the last font loaded
	Word m_uInvisibleColor;	///< Color to ignore for drawing
public:
	struct State_t {
		Word32 m_ColorTable[16];	///< Colors to render with
		RezFile *m_pRezFile;		///< Resource file manager
		int m_iX;					///< X coord
		int m_iY;					///< Y coord
		Word m_uRezNum;				///< Resource ID of the last font loaded
		Word m_uInvisibleColor;		///< Color to ignore for drawing
	};
	struct RGBColorList_t {
		Word8 m_uCount;			///< Number of entries
		RGBWord8_t m_Colors[1];	///< Actual data to set
	};
	Font4Bit(Renderer *pRenderer=NULL);
	virtual ~Font4Bit();
	virtual Word GetPixelWidth(const char *pInput,WordPtr uLength);
	virtual void DrawChar(Word uLetter);
	void BURGER_API Init(RezFile *pRezFile,Word uRezNum,const Word8 *pPalette,Renderer *pRenderer=NULL);
	void BURGER_API Shutdown(void);
	void BURGER_API SaveState(State_t *pOutput);
	void BURGER_API RestoreState(const State_t *pInput);
	void BURGER_API SetColor(Word uColorIndex,Word uColor);
	BURGER_INLINE void UseZero(void) { m_uInvisibleColor = 0x7FFF; }
	BURGER_INLINE void UseMask(void) { m_uInvisibleColor = 0; }
	void BURGER_API InstallToPalette(RezFile *pRezFile,Word uRezNum,const Word8 *pPalette);
	void BURGER_API SetColorRGBListToPalette(const RGBColorList_t *pRGBList,const Word8 *pPalette);
	void BURGER_API SetToPalette(const Word8 *pPalette);
};
}
/* END */

#endif
