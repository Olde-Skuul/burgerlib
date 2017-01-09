/***************************************

	Decompression manager version of RLE

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDECOMPRESSLBMRLE_H__
#define __BRDECOMPRESSLBMRLE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDECOMPRESS_H__
#include "brdecompress.h"
#endif

/* BEGIN */
namespace Burger {
class DecompressILBMRLE : public Decompress {
	BURGER_RTTI_IN_CLASS();
protected:
	enum eState {
		STATE_INIT,			///< Start of a compression token
		STATE_FILLTOKEN,	///< Obtained a fill token, awaiting fill byte
		STATE_FILL,			///< Data fill in progress
		STATE_RUN			///< Raw data copy in progress
	};
	WordPtr m_uRun;		///< Run in progress
	Word m_uFill;		///< Last fill value
	eState m_eState;	///< State of the decompression
public:
	static const Word32 Signature = 0x524C4420;		///< 'RLE '
	DecompressILBMRLE();
	virtual eError Reset(void);
	virtual eError Process(void *pOutput,WordPtr uOutputChunkLength,const void *pInput,WordPtr uInputChunkLength);
};
extern Decompress::eError BURGER_API SimpleDecompressILBMRLE(void *pOutput,WordPtr uOutputChunkLength,const void *pInput,WordPtr uInputChunkLength);
}
/* END */

#endif

