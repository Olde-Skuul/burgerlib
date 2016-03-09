/***************************************

	Compress using ILBM RLE

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCOMPRESSLBMRLE_H__
#define __BRCOMPRESSLBMRLE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BROUTPUTMEMORYSTREAM_H__
#include "broutputmemorystream.h"
#endif

#ifndef __BRCOMPRESS_H__
#include "brcompress.h"
#endif

/* BEGIN */
namespace Burger {
class CompressILBMRLE : public Compress {
	BURGER_RTTI_IN_CLASS();
protected:
	WordPtr m_uCacheUsed;		///< Number of bytes in the cache
	WordPtr m_uRemaining;		///< Number of bytes unprocessed from the last call to Compact()
	Word8 m_Cache[128+8];		///< Data cache for resuming compression
	eError Compact(const Word8 *pInput,WordPtr uInputLength);
public:
	static const Word32 Signature = 0x524C4420;		///< 'RLE '
	CompressILBMRLE(void);
	virtual eError Init(void);
	virtual eError Process(const void *pInput,WordPtr uInputLength);
	virtual eError Finalize(void);
};
}
/* END */

#endif

