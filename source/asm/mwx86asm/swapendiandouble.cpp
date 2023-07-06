/***************************************

	X86 assembly for implementing the function
	Burger::_swapendiandouble(double) for Metrowerks Codewarrior

	Build with the C compiler

	EAX, ECX and EDX are scratch registers

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

***************************************/

#include "brendian.h"

__declspec(naked) double BURGER_API Burger::_swapendiandouble(
	double /* dInput */)
{
	asm
	{

; Fetch the double
	mov		eax, dword ptr[esp + 4]
	mov		edx, dword ptr[esp + 8]

; Swap the 64 bits
	bswap	eax
	bswap	edx

; Store double, swapped
	mov		dword ptr[esp + 8], eax
	mov		dword ptr[esp + 4], edx

; Load the double into the FPU
	fld		qword ptr[esp + 4]

; Exit
	ret		8

	}
}