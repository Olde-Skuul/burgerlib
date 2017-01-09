/***************************************

	Decompression manager version of LZSS

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRDECOMPRESSLZSS_H__
#define __BRDECOMPRESSLZSS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDECOMPRESS_H__
#include "brdecompress.h"
#endif

/* BEGIN */
namespace Burger {
class DecompressLZSS : public Decompress {
	BURGER_RTTI_IN_CLASS();
protected:
	enum eState {
		STATE_INIT,			///< Start of a compression token	
		STATE_16BIT2,		///< Grab the 2nd half of a 16 bit run token
		STATE_RUN			///< Memory copy in progress
	};
	WordPtr m_uRunCount;	///< Previous 16 bit token (Half)
	WordPtr m_uOffset;		///< Previous destination pointer
	Word m_uBitBucket;		///< Previous bit bucket
	eState m_eState;		///< State of the decompression
public:
	static const Word32 Signature = 0x4C5A5353;		///< 'LZSS'
	DecompressLZSS();
	virtual eError Reset(void);
	virtual eError Process(void *pOutput,WordPtr uOutputChunkLength,const void *pInput,WordPtr uInputChunkLength);
};
extern Decompress::eError BURGER_API SimpleDecompressLZSS(void *pOutput,WordPtr uOutputChunkLength,const void *pInput,WordPtr uInputChunkLength);
}
/* END */

#endif

