/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	e8087Precision __fastcall get_8087_precision(void)
	eax, ecx and edx are volatile
	Result in eax

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
