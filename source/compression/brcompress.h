/***************************************

	Generic compression manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCOMPRESS_H__
#define __BRCOMPRESS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BROUTPUTMEMORYSTREAM_H__
#include "broutputmemorystream.h"
#endif

/* BEGIN */
namespace Burger {
class Compress : public Base {
protected:
	OutputMemoryStream m_Output;		///< Main output buffer for compressed data
	char m_uSignature[4];				///< 4 character code to identify this compressor
public:
	enum eError {
		COMPRESS_OKAY,				///< No errors
		COMPRESS_OUTOFMEMORY		///< Error found in compressed data (Decompression was aborted)
	};
	Compress(void);
	virtual ~Compress();
	virtual eError Init(void) = 0;
	virtual eError Process(const void *pInput,WordPtr uInputLength) = 0;
	virtual eError Finalize(void) = 0;
	BURGER_INLINE OutputMemoryStream *GetOutput(void) { return &m_Output; }
	BURGER_INLINE WordPtr GetOutputSize(void) const { return m_Output.GetSize(); }
	BURGER_INLINE Word32 GetSignature(void) const { return reinterpret_cast<const Word32 *>(m_uSignature)[0]; }
};
}
/* END */

#endif

