/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	double __fastcall _swapendiandouble(const double* pInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	pInput = ecx

***************************************/

#include "brendian.h"

__declspec(naked) double BURGER_API Burger::_swapendiandouble(
	const double* /* pInput */)
{
	asm
	{

; Fetch the double
	mov		eax, dword ptr[ecx]
	mov		edx, dword ptr[ecx + 4]

; Swap the 64 bits
	bswap	eax
	bswap	edx

; Store double, swapped
	push	eax
	push	edx

; Load the double into the FPU
	fld		qword ptr[esp]

; Fix the stack
	add		esp, 8
	ret

	}
}