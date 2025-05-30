/***************************************

    Generic compression manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
	BURGER_RTTI_IN_CLASS();
protected:
	OutputMemoryStream m_Output;		///< Main output buffer for compressed data
	uint32_t m_uSignature;				///< 4 character code to identify this compressor
public:
	Compress(void);
	virtual ~Compress();
	virtual eError Init(void) = 0;
	virtual eError Process(const void *pInput, uintptr_t uInputLength) = 0;
	virtual eError Finalize(void) = 0;
	BURGER_INLINE OutputMemoryStream *GetOutput(void) BURGER_NOEXCEPT { return &m_Output; }
	BURGER_INLINE uintptr_t GetOutputSize(void) const BURGER_NOEXCEPT { return m_Output.GetSize(); }
	BURGER_INLINE uint32_t GetSignature(void) const BURGER_NOEXCEPT { return m_uSignature; }
};
}
/* END */

#endif

