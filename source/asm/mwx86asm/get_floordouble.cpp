/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	double __fastcall get_floor(double dInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	dInput = [esp+4]

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) double BURGER_API Burger::get_floor(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Load the same value in the FPU
	fld		qword ptr[esp+4]

; Save the current 8087 control word
	fstcw	word ptr[esp+4]

; Force round to negative infinity
	fldcw	[g_u8087RoundToNegativeInfinity]

; Perform the rounding
	frndint

; Restore the old 8087 control word
	fldcw	word ptr[esp+4]
	ret		8
	}
	// clang-format on
}