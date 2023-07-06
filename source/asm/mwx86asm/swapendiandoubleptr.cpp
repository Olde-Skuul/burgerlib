/***************************************

	X86 assembly for implementing the function
	Burger::_swapendiandouble(const double*) for Metrowerks Codewarrior

	Build with the C compiler

	EAX, ECX and EDX are scratch registers
	pInput = ECX

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

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

; Exit
	ret

	}
}