/***************************************

	X86 assembly for implementing the function for Metrowerks Codewarrior

	Build with the C compiler

	EAX, ECX and EDX are scratch registers

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) Burger::e8087Rounding BURGER_API Burger::set_8087_rounding(
	e8087Rounding /* uInput */) BURGER_NOEXCEPT
{
	asm {

; Space for the status word
	sub		esp, 4

; Get the current status word
	fnstcw	[esp]

; Get the current value for return
	mov		eax, dword ptr[esp]

; Move to the Pentium bits area
	shl		ecx, 10

; Get the bits to retain for write
	mov		edx, eax

; Mask for return value
	and		eax, 0x0C00

; Mask off unused bits
	and		edx, 0xF3FF

; Convert to enum
	shr		eax, 10

; Blend in the bits
	or		edx, ecx

; Store in memory
	mov		dword ptr[esp], edx

; Save the new status register
	fldcw	[esp]

; Fix stack
	add		esp, 4
	ret
	}
}
