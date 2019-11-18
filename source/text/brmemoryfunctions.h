/***************************************

	Simple memory functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMEMORYFUNCTIONS_H__
#define __BRMEMORYFUNCTIONS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

extern void BURGER_API MemoryCopy(
	void* pOutput, const void* pInput, WordPtr uCount);
extern void BURGER_API MemoryMove(
	void* pOutput, const void* pInput, WordPtr uCount);
extern void BURGER_API MemoryClear(void* pOutput, WordPtr uCount);
extern void BURGER_API MemoryFill(void* pOutput, Word8 uFill, WordPtr uCount);
extern void BURGER_API MemoryNot(void* pOutput, WordPtr uCount);
extern void BURGER_API MemoryNot(
	void* pOutput, const void* pInput, WordPtr uCount);
extern void BURGER_API MemoryXor(void* pOutput, Word8 uXor, WordPtr uCount);
extern void BURGER_API MemoryXor(
	void* pOutput, const void* pInput, Word8 uXor, WordPtr uCount);
extern void BURGER_API MemoryXor(
	void* pOutput, const void* pInput, WordPtr uCount);
extern void BURGER_API MemoryXor(
	void* pOutput, const void* pInput1, const void* pInput2, WordPtr uCount);
extern int BURGER_API MemoryCompare(
	const void* pInput1, const void* pInput2, WordPtr uCount);
extern int BURGER_API MemoryCaseCompare(
	const void* pInput1, const void* pInput2, WordPtr uCount) BURGER_NOEXCEPT;
extern char* BURGER_API MemoryCharacter(
	const char* pInput, WordPtr uInputSize, int iChar);
extern Word16* BURGER_API MemoryCharacter(
	const Word16* pInput, WordPtr uInputSize, Word uChar);
extern char* BURGER_API MemoryCharacterReverse(
	const char* pInput, WordPtr uInputSize, int iChar);
extern Word16* BURGER_API MemoryCharacterReverse(
	const Word16* pInput, WordPtr uInputSize, Word uChar);

}

/* END */

#endif
