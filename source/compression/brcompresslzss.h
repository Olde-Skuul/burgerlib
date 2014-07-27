/***************************************

	Compress using LZSS

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	static const Word RINGBUFFERSIZE=4096;		///< Size of the LZSS ring buffer
	static const Word MAXMATCHLENGTH=18;		///< Largest size of a string to match
	static const Word MINMATCHLENGTH=2;			///< Encode string into position and length
	static const Word NOTUSED=RINGBUFFERSIZE;	///< Index for root of binary search trees

	WordPtr m_uBitMaskOffset;					///< Location in the output stream to store any bit masks
	Word m_uSourceIndex;						///< Index to insert nodes into
	Word m_uDestIndex;							///< Index to remove nodes from (is usually CompressLZSS::m_uSourceIndex-MAXMATCHLENGTH)
	Word m_uCachedLength;						///< Ring buffer cache size (Max CompressLZSS::MAXMATCHLENGTH)
	Word m_uMatchOffset;						///< Offset of string match
	Word m_uMatchSize;							///< Length of string match 0-18 of longest match. These are set by the InsertNode() procedure.
	Word m_uPreviousMatchSize;					///< Length of the last match
	Word m_uMatchIterator;						///< Match size iterator
	Word m_LeftBranch[RINGBUFFERSIZE+1];		///< Left child
	Word m_RightBranch[RINGBUFFERSIZE+1+256];	///< Right child / Hash table
	Word m_RootBranch[RINGBUFFERSIZE+1];		///< Roots for each binary tree left & right children & parents
	Word8 m_bBitMask;							///< Bit field to store in the output stream
	Word8 m_bOrMask;							///< Bit mask for which bit is currently being modified
	Word8 m_RingBuffer[RINGBUFFERSIZE+MAXMATCHLENGTH-1];	///< Ring buffer of size RINGBUFFERSIZE, with extra MAXMATCHLENGTH-1 bytes to facilitate string comparison

	void DeleteNode(WordPtr uNodeNumber);
	void InsertNode(WordPtr uNodeNumber);
	void InitTrees(void);
public:
#if defined(BURGER_BIGENDIAN)
	static const Word32 Signature = 0x4C5A5353;		///< 'LZSS'
#else
	static const Word32 Signature = 0x53535A4C;		///< 'LZSS'
#endif
	CompressLZSS(void);
	virtual eError Init(void);
	virtual eError Process(const void *pInput,WordPtr uInputLength);
	virtual eError Finalize(void);
};
}
/* END */

#endif

