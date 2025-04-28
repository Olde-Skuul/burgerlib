/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the C compiler

	uint64_t BURGER_API Burger::bit_reverse(uint64_t, uint_t) BURGER_NOEXCEPT
	eax, ecx and edx are volatile
	Result in eax/edx
	uInput = ESP+4/8, ecx

***************************************/

#include "brpoweroftwo.h"

__declspec(naked) uint64_t BURGER_API Burger::bit_reverse(
	uint64_t /* uInput */, uint_t /* uBitLength */) BURGER_NOEXCEPT
{
	asm {
		// clang-format off

; Save registers
	push	esi
	push	edi

; Initialize the result
	xor		eax, eax
	xor		edx, edx

	mov		esi, [esp + 12]
	mov		edi, [esp + 16]

; Shift out a 64 bit
BitReverse64:
	shr		edi, 1
	rcr		esi, 1

; Shift in a 64 bit
	adc		eax, eax
	adc		edx, edx

; Count down the iterator
	sub		ecx, 1
	jne		BitReverse64

; Restore registers
	pop		edi
	pop		esi
	ret		8

		// clang-format on
	}
}
