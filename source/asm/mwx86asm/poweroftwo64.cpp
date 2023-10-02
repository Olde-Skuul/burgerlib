/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

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

; Save registers
	push	ebx

; Force 0x40000000 to be 0x3FFFFFFF so in the end
; it is changed back to 0x40000000 (64 bit)

	mov		ecx, [esp+4+4]
	mov		edx, [esp+8+4]
	sub		ecx, 1
	sbb		edx, 0

; Implement the C code, but use shrd
; uInput |= uInput>>1
	mov		eax, ecx
	mov		ebx, edx
	shr		edx, 1
	rcr		eax, 1
	or		ecx, eax
	or		ebx, edx

; uInput |= uInput>>2
	mov		eax, ecx
	mov		edx, ebx
	shrd	eax, ebx, 2
	shr		edx, 2
	or		ecx, eax
	or		ebx, edx

; uInput |= uInput>>4
	mov		eax, ecx
	mov		edx, ebx
	shrd	eax, ebx, 4
	shr		edx, 4
	or		ecx, eax
	or		ebx, edx

; uInput |= uInput>>8
	mov		eax, ecx
	mov		edx, ebx
	shrd	eax, ebx, 8
	shr		edx, 8
	or		ecx, eax
	or		ebx, edx

; uInput |= uInput>>16
	mov		eax, ecx
	mov		edx, ebx
	shrd	eax, ebx, 16
	shr		edx, 16
	or		eax, ecx
	or		edx, ebx

; Perform the final or in one instruction
; uInput |= uInput>>32
	or		eax, edx

; Increment to get the final power of 2
	add		eax, 1
	adc		edx, 0

; Restore registers
	pop		ebx
	ret		8

// clang-format on
	}
}
