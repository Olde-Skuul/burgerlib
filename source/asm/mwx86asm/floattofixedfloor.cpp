/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	fixed16_16_t __fastcall float_to_fixed_floor(float fInput)
	eax, ecx and edx are volatile
	Result in eax
	fInput = [esp+4]

***************************************/

#include "brfixedpoint.h"
#include "brstructs.h"

__declspec(naked) fixed16_16_t BURGER_API Burger::float_to_fixed_floor(
	float /* fInput */) BURGER_NOEXCEPT
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

; Get the integer
	mov		eax, dword ptr[esp + 4]

; Need to update the rounding?
	jbe		NoExtra

; --1
	dec		eax
NoExtra:

; Clean up the FP stack
	fstp	st(0)
	ret		4
	}
	// clang-format on
}