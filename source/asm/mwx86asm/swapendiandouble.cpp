/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	double __fastcall _swapendiandouble(double dInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	dInput = [esp+4]

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
	ret		8

	}
}