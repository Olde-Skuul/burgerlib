/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the C compiler

	uint64_t BURGER_API Burger::power_of_two(uint64_t) BURGER_NOEXCEPT
	eax, ecx and edx are volatile
	Result in eax/edx
	uInput = [esp+4] / [esp+8]

***************************************/

#include "brpoweroftwo.h"

__declspec(naked) uint64_t BURGER_API Burger::power_of_two(
	uint64_t /* uInput */) BURGER_NOEXCEPT
{
	asm {
// clang-format off

; Force 0x40000000 to be 0x3FFFFFFF so in the end
; it is changed back to 0x40000000 (64 bit)

	mov		ecx, [esp+4]
	mov		edx, [esp+8]
	sub		ecx, 1
	sbb		edx, 0

; Check if the upper bits are clear
	bsr		edx, edx
	jz		low32

; Shift has to use ecx
	mov		ecx, edx

; Prepare for shift
	mov		edx, 1

; edx has the power of 2, but it needs one more left shift
	shl		edx, cl

; Do the final shift, if overflow, return zero
	add		edx, edx

; Low 32 bits are always clear
	xor		eax, eax
	ret		8

; Input value resolves to low 32 bits.
; Answer is 0x1 to 0x100000000
low32:

; Prepare for shift
	mov		eax, 1

; Clear high word
	xor		edx, edx

; Find the LEFTMOST bit, ecx will have 0x00 to 0x1F
	bsr		ecx, ecx

; Smallest number
	jz		one

; eax has the power of 2, but it needs one more left shift
	shl		eax, cl

; Do the final shift, if overflow, set edx to 1
	add		eax, eax
	adc		edx, 0
one:
	ret		8
// clang-format on
	}
}
