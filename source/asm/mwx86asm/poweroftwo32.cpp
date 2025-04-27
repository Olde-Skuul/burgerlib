/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the C compiler

	uint32_t BURGER_API Burger::power_of_two(uint32_t) BURGER_NOEXCEPT
	eax, ecx and edx are volatile
	Result in eax
	uInput = ecx

***************************************/

#include "brpoweroftwo.h"

__declspec(naked) uint32_t BURGER_API Burger::power_of_two(
	uint32_t /* uInput */) BURGER_NOEXCEPT
{
	asm {
// clang-format off

; Force 0x40000000 to be 0x3FFFFFFF so in the end
; it is changed back to 0x40000000
	dec		ecx

; Clear eax for the setnz
	xor		eax, eax

; Find the LEFTMOST bit, ecx will have 0x00 to 0x1F
	bsr		ecx, ecx

; If ecx == 0, clear eax, otherwise set it to 1
	setnz	al

; edx = 0 or 1
	mov		edx, eax

; eax has the power of 2, but it needs one more left shift
	shl		eax, cl

; edx is 1 only on the uinput==1 case, otherwise it is zero
	xor		edx, 1

; For the final result, eax has the power of two but it
; needs one more shift, so eax * 2 performs that. However
; in the 1 input case, eax is zero.
; edx has 0 in all cases but uInput == 1, where it will
; have 1. So...
; uInput was 1 (1-1) -> 0, so, 0^1 = 1+0 = 1
	lea		eax, [edx + eax * 2]
	ret

// clang-format on
	}
}
