/***************************************

    aLaw decompresser

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRALAW_H__
#define __BRALAW_H__

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
class DecompressALaw : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
protected:
	enum eState {
		STATE_INIT,				///< Basic state
		STATE_CACHEFULL			///< Cache is full, need to output before more input
	};
	Word m_uCacheSize;			///< Number of valid bytes in m_uCache
	eState m_eState;			///< State of the decompression
	Int16 m_iCache;				///< Temp uncompressed data buffer
public:
	static const Int16 g_Table[256];
	static const Word32 Signature = 0x616C6177;		///< 'alaw'
	DecompressALaw();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput,uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
	static DecompressALaw * BURGER_API New(void);
};
}
/* END */

#endif
