/***************************************

    Decompression manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDECOMPRESS_H__
#define __BRDECOMPRESS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

/* BEGIN */
namespace Burger {
class Decompress : public Base {
	BURGER_RTTI_IN_CLASS();
protected:
    uintptr_t m_uTotalInput;		///< Total number of bytes processed for input
    uintptr_t m_uTotalOutput;		///< Total number of bytes processed for output
    uintptr_t m_uInputLength;		///< Number of input bytes processed from the last call to Process()
    uintptr_t m_uOutputLength;	///< Number of output bytes processed from the last call to Process()
	uint32_t m_uSignature;		///< 4 character code to identify this decompresser
public:
	enum eError {
		DECOMPRESS_OKAY,			///< No errors
		DECOMPRESS_OUTPUTUNDERRUN,	///< All input was exhausted before output was satisfied
		DECOMPRESS_OUTPUTOVERRUN,	///< Input data remained after output was satisfied
		DECOMPRESS_BADINPUT			///< Error found in compressed data (Decompression was aborted)
	};
	Decompress(void);
	virtual eError Reset(void) = 0;
	virtual eError Process(void *pOutput, uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) = 0;
	BURGER_INLINE uintptr_t GetTotalInputSize(void) const { return m_uTotalInput; }
	BURGER_INLINE uintptr_t GetTotalOutputSize(void) const { return m_uTotalOutput; }
	BURGER_INLINE uintptr_t GetProcessedInputSize(void) const { return m_uInputLength; }
	BURGER_INLINE uintptr_t GetProcessedOutputSize(void) const { return m_uOutputLength; }
	BURGER_INLINE uint32_t GetSignature(void) const { return m_uSignature; }
};
}
/* END */

#endif

