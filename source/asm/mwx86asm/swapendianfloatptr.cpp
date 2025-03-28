/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	float __fastcall _swapendianfloat(const float* pInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	pInput = ecx

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
	ret

	}
}