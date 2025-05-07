/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the C compiler

	uint_t BURGER_API Burger::has_CPUID(void) BURGER_NOEXCEPT
	eax, ecx and edx are volatile
	Result in eax

***************************************/

#include "bratomic.h"

__declspec(naked) uint_t BURGER_API Burger::has_CPUID(void) BURGER_NOEXCEPT
{
	asm {
// clang-format off

; See of flags bit 21 can be changed.
; If it can, it is higher than 486, which
; implies that CPUID is available

; Save flags on the stack
	pushfd

; Get in register and leave on stack
	mov		eax, dword ptr[esp]

; Switch bit 21 for the test
	xor		eax, 0x00200000

; Set the flags with the new value
	push	eax
	popfd

; Read in the flags into a register
	pushfd
	pop		eax

; Did the change "take"
	xor		eax,dword ptr[esp]

; Move to the lowest bit
	shr		eax,21

; Set to TRUE or FALSE
	and		eax,1

; Restore the flags to the original state
	popfd
	ret

// clang-format on
	}
}
