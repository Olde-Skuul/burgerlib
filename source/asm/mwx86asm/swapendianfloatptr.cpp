/***************************************

	X86 assembly for implementing the function
	Burger::_swapendianfloat(const float*) for Metrowerks Codewarrior

	Build with the C compiler

	EAX, ECX and EDX are scratch registers
	pInput = ECX

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

***************************************/

#include "brendian.h"

__declspec(naked) float BURGER_API Burger::_swapendianfloat(
	const float* /* pInput */) BURGER_NOEXCEPT
{
	asm
	{
; Get the float
	mov		edx, dword ptr[ecx]

; Swap the float	
	bswap	edx

; Store the float into a temp variable
	push	edx

; Load into the x87 FPU
	fld		dword ptr[esp]

; Fix the stack
	add		esp, 4

; Exit
	ret

	}
}