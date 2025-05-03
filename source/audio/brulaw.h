/***************************************

    uLaw decompresser

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRULAW_H__
#define __BRULAW_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDECOMPRESS_H__
#include "brdecompress.h"
#endif

#ifndef __BRSOUNDDECOMPRESS_H__
#include "brsounddecompress.h"
#endif

/* BEGIN */
namespace Burger {
class DecompressULaw : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
protected:
	enum eState {
		STATE_INIT,				///< Basic state
		STATE_CACHEFULL			///< Cache is full, need to output before more input
	};
	uint_t m_uCacheSize;			///< Number of valid bytes in m_uCache
	eState m_eState;			///< State of the decompression
	int16_t m_iCache;				///< Temp uncompressed data buffer
public:
	static const int16_t g_Table[256];
#if defined(BURGER_BIGENDIAN)
	static const uint32_t Signature = 0x756C6177;		///< 'ulaw'
#else
	static const uint32_t Signature = 0x77616C75;		///< 'ulaw'
#endif
	DecompressULaw();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput, uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
	static DecompressULaw * BURGER_API new_object(void);
};
}
/* END */

#endif
