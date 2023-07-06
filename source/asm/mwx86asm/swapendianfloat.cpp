/***************************************

	X86 assembly for implementing the function
	Burger::_swapendianfloat(float) for Metrowerks Codewarrior

	Build with the C compiler

	EAX, ECX and EDX are scratch registers

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

***************************************/

#include "brendian.h"

__declspec(naked) float BURGER_API Burger::_swapendianfloat(
	float /* fInput */) BURGER_NOEXCEPT
{
	asm
	{
; Get the float
	mov		edx, dword ptr[esp + 4]

; Swap the float	
	bswap	edx

; Store the float
	mov		dword ptr[esp + 4], edx

; Load into the x87 FPU
	fld		dword ptr[esp + 4]

; Exit
	ret		4

	}
}