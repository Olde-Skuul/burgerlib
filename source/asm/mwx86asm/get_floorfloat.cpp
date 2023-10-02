/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	float __fastcall get_floor(float fInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	fInput = [esp+4]

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) float BURGER_API Burger::get_floor(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Load the same value in the FPU
	fld		dword ptr[esp+4]

; Save the current 8087 control word
	fstcw	word ptr[esp+4]

; Force round to negative infinity
	fldcw	[g_u8087RoundToNegativeInfinity]

; Perform the rounding
	frndint

; Restore the old 8087 control word
	fldcw	word ptr[esp+4]
	ret		4
	}
	// clang-format on
}