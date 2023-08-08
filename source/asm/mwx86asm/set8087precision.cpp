/***************************************

	X86 assembly for implementing the function for Metrowerks Codewarrior

	Build with the C compiler

	EAX, ECX and EDX are scratch registers

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) Burger::e8087Precision BURGER_API Burger::set_8087_precision(
	e8087Precision /* uInput */) BURGER_NOEXCEPT
{
	asm {

; Space for the status word
	sub		esp, 4

; Get the current status word
	fnstcw	[esp]

; Get the current value for return
	mov		eax, dword ptr[esp]

; Move to the Pentium bits area
	shl		ecx, 8

; Get the bits to retain for write
	mov		edx, eax

; Mask for return value
	and		eax, 0x0300

; Mask off unused bits
	and		edx, 0xFCFF

; Convert to enum
	shr		eax, 8

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
