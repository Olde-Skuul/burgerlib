/***************************************

	Simple memory functions

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	void* pOutput, const void* pInput, uintptr_t uCount) BURGER_NOEXCEPT;
extern void BURGER_API MemoryMove(
	void* pOutput, const void* pInput, uintptr_t uCount) BURGER_NOEXCEPT;
extern void BURGER_API MemoryClear(
	void* pOutput, uintptr_t uCount) BURGER_NOEXCEPT;
extern void BURGER_API MemoryFill(
	void* pOutput, uint8_t uFill, uintptr_t uCount) BURGER_NOEXCEPT;
extern void BURGER_API MemoryNot(
	void* pOutput, uintptr_t uCount) BURGER_NOEXCEPT;
extern void BURGER_API MemoryNot(
	void* pOutput, const void* pInput, uintptr_t uCount) BURGER_NOEXCEPT;
extern void BURGER_API MemoryXor(
	void* pOutput, uint8_t uXor, uintptr_t uCount) BURGER_NOEXCEPT;
extern void BURGER_API MemoryXor(void* pOutput, const void* pInput,
	uint8_t uXor, uintptr_t uCount) BURGER_NOEXCEPT;
extern void BURGER_API MemoryXor(
	void* pOutput, const void* pInput, uintptr_t uCount) BURGER_NOEXCEPT;
extern void BURGER_API MemoryXor(void* pOutput, const void* pInput1,
	const void* pInput2, uintptr_t uCount) BURGER_NOEXCEPT;
extern int BURGER_API MemoryCompare(
	const void* pInput1, const void* pInput2, uintptr_t uCount) BURGER_NOEXCEPT;
extern int BURGER_API MemoryCaseCompare(
	const void* pInput1, const void* pInput2, uintptr_t uCount) BURGER_NOEXCEPT;
extern char* BURGER_API MemoryCharacter(
	const char* pInput, uintptr_t uInputSize, int iChar) BURGER_NOEXCEPT;
extern uint16_t* BURGER_API MemoryCharacter(
	const uint16_t* pInput, uintptr_t uInputSize, uint_t uChar) BURGER_NOEXCEPT;
extern char* BURGER_API MemoryCharacterReverse(
	const char* pInput, uintptr_t uInputSize, int iChar) BURGER_NOEXCEPT;
extern uint16_t* BURGER_API MemoryCharacterReverse(
	const uint16_t* pInput, uintptr_t uInputSize, uint_t uChar) BURGER_NOEXCEPT;

}

/* END */

#endif
