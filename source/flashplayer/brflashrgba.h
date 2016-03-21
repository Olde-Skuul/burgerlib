/***************************************

	Flash player data stream reader
	
	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BRFLASHRGBA_H__
#define __BRFLASHRGBA_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFLASHSTREAM_H__
#include "brflashstream.h"
#endif

#ifndef __BRPALETTE_H__
#include "brpalette.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
class RGBAWord8 : public RGBAWord8_t {
public:
	RGBAWord8() {
		m_uRed = 255;
		m_uGreen = 255;
		m_uBlue = 255;
		m_uAlpha = 255;
	}
	RGBAWord8(Word8 bRed,Word8 bGreen,Word8 bBlue,Word8 bAlpha) {
		m_uRed = bRed;
		m_uGreen = bGreen;
		m_uBlue = bBlue;
		m_uAlpha = bAlpha;
	}
	RGBAWord8(double dColor) { Set(dColor); }
	void BURGER_API Read(Stream *pStream,Word uTagType);
	void BURGER_API ReadRGBA(Stream *pStream);
	void BURGER_API ReadRGB(Stream *pStream);
	BURGER_INLINE void Set(Word8 bRed,Word8 bGreen,Word8 bBlue,Word8 bAlpha) {
		m_uRed = bRed;
		m_uGreen = bGreen;
		m_uBlue = bBlue;
		m_uAlpha = bAlpha;
	}
	void BURGER_API Set(Word32 uColor);
	BURGER_INLINE void BURGER_API Set(double dColor) { Set(static_cast<Word32>(static_cast<int>(dColor))); }
	void BURGER_API Print(void) const;
};
}}
/* END */

#endif
