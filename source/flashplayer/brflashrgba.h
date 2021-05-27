/***************************************

    Flash player data stream reader

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
	RGBAWord8(uint8_t bRed,uint8_t bGreen,uint8_t bBlue,uint8_t bAlpha) {
		m_uRed = bRed;
		m_uGreen = bGreen;
		m_uBlue = bBlue;
		m_uAlpha = bAlpha;
	}
	RGBAWord8(double dColor) { Set(dColor); }
	void BURGER_API Read(Stream *pStream,uint_t uTagType);
	void BURGER_API ReadRGBA(Stream *pStream);
	void BURGER_API ReadRGB(Stream *pStream);
	BURGER_INLINE void Set(uint8_t bRed,uint8_t bGreen,uint8_t bBlue,uint8_t bAlpha) {
		m_uRed = bRed;
		m_uGreen = bGreen;
		m_uBlue = bBlue;
		m_uAlpha = bAlpha;
	}
	void BURGER_API Set(uint32_t uColor);
	BURGER_INLINE void BURGER_API Set(double dColor) { Set(static_cast<uint32_t>(static_cast<int>(dColor))); }
	void BURGER_API Print(void) const;
};
}}
/* END */

#endif
