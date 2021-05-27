/***************************************

    Compress using LZSS

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCOMPRESSLZSS_H__
#define __BRCOMPRESSLZSS_H__

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
class CompressLZSS : public Compress {
	BURGER_RTTI_IN_CLASS();
protected:
	static const uint_t RINGBUFFERSIZE=4096;		///< Size of the LZSS ring buffer
	static const uint_t MAXMATCHLENGTH=18;		///< Largest size of a string to match
	static const uint_t MINMATCHLENGTH=2;			///< Encode string into position and length
	static const uint_t NOTUSED=RINGBUFFERSIZE;	///< Index for root of binary search trees

	uintptr_t m_uBitMaskOffset;					///< Location in the output stream to store any bit masks
	uint_t m_uSourceIndex;						///< Index to insert nodes into
	uint_t m_uDestIndex;							///< Index to remove nodes from (is usually CompressLZSS::m_uSourceIndex-MAXMATCHLENGTH)
	uint_t m_uCachedLength;						///< Ring buffer cache size (Max CompressLZSS::MAXMATCHLENGTH)
	uint_t m_uMatchOffset;						///< Offset of string match
	uint_t m_uMatchSize;							///< Length of string match 0-18 of longest match. These are set by the InsertNode() procedure.
	uint_t m_uPreviousMatchSize;					///< Length of the last match
	uint_t m_uMatchIterator;						///< Match size iterator
	uint_t m_LeftBranch[RINGBUFFERSIZE+1];		///< Left child
	uint_t m_RightBranch[RINGBUFFERSIZE+1+256];	///< Right child / Hash table
	uint_t m_RootBranch[RINGBUFFERSIZE+1];		///< Roots for each binary tree left & right children & parents
	uint8_t m_bBitMask;							///< Bit field to store in the output stream
	uint8_t m_bOrMask;							///< Bit mask for which bit is currently being modified
	uint8_t m_RingBuffer[RINGBUFFERSIZE+MAXMATCHLENGTH-1];	///< Ring buffer of size RINGBUFFERSIZE, with extra MAXMATCHLENGTH-1 bytes to facilitate string comparison

	void DeleteNode(uintptr_t uNodeNumber);
	void InsertNode(uintptr_t uNodeNumber);
	void InitTrees(void);
public:
	static const uint32_t Signature = 0x4C5A5353;		///< 'LZSS'
	CompressLZSS(void);
	eError Init(void) BURGER_OVERRIDE;
	eError Process(const void *pInput, uintptr_t uInputLength) BURGER_OVERRIDE;
	eError Finalize(void) BURGER_OVERRIDE;
};
}
/* END */

#endif

