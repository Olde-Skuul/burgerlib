/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	float __fastcall _swapendianfloat(float fInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	fInput = [esp+4]

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
	ret		4

	}
}