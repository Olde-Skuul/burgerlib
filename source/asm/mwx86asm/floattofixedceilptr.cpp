/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	void __fastcall float_to_fixed_ceil(Fixed32* pOutput, float fInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	ecx = pOutput
	fInput = [esp+4]

***************************************/

#include "brfixedpoint.h"
#include "brstructs.h"

__declspec(naked) void BURGER_API Burger::float_to_fixed_ceil(
	Fixed32* /* pOutput */, float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Get the input value
	fld		dword ptr[esp + 4]

; Convert to fixed point
	fmul	dword ptr[g_f65536]

; Load the same value in the FPU
	fld		st(0)

; Convert the float to integer
	frndint

; Store the integer (Leave a copy in the FPU)
	fist	dword ptr[esp + 4]

; Compare the integer to the input and pop
	fcomip	st(0), st(1)

; Clean up the FP stack
	fstp	st(0)

; Get the integer
	mov		eax, dword ptr[esp + 4]

; Need to update the rounding?
	jae		NoExtra

; ++1
	inc		eax
NoExtra:

; Store in the pointer
	mov		dword ptr[ecx], eax
	ret		4
	}
	// clang-format on
}