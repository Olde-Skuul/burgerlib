/***************************************

	X86 assembly for implementing the function for Metrowerks Codewarrior

	Build with the C compiler

	EAX, ECX and EDX are scratch registers

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) Burger::e8087Precision BURGER_API Burger::get_8087_precision(
	void) BURGER_NOEXCEPT
{
	asm
	{

; Space for the status word
	push	eax

; Get the current status word
	fnstcw	[esp]

; Get the current value for return
	pop		eax

; Shift the bits of interest down
	shr		eax, 8

; Mask to enumeration
	and		eax, 3
	ret
	}
}
