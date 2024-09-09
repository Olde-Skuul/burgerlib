/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the C compiler

	uint32_t BURGER_API Burger::bit_reverse(uint32_t, uint_t) BURGER_NOEXCEPT
	eax, ecx and edx are volatile
	Result in eax
	uInput = ecx, edx

***************************************/

#include "brpoweroftwo.h"

__declspec(naked) uint32_t BURGER_API Burger::bit_reverse(
	uint32_t /* uInput */, uint_t /* uBitLength */) BURGER_NOEXCEPT
{
	asm {
		// clang-format off

; Initialize the result
	xor		eax, eax

; Shift out a bit
BitReverse32:
	shr		ecx, 1

; Shift into the result
	adc		eax, eax

; Count down the iterator
	sub		edx, 1
	jne		BitReverse32
	ret

		// clang-format on
	}
}
