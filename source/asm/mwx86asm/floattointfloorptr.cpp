/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	void __fastcall float_to_int_floor(int32_t* pOutput, float fInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	ecx = pOutput
	fInput = [esp+4]

***************************************/

#include "brfixedpoint.h"

__declspec(naked) void BURGER_API Burger::float_to_int_floor(
	int32_t* /* pOutput */, float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Get the input value
	fld		dword ptr[esp + 4]

; Load the same value in the FPU
	fld		st(0)

; Convert the float to integer
	frndint

; Store the integer (Leave a copy in the FPU)
	fist	dword ptr[esp + 4]

; Compare the integer and pop
	fcomip	st(0), st(1)

; Clean up the FP stack
	fstp	st(0)

; Get the integer
	mov		eax, dword ptr[esp + 4]

; Overflow? (Negative)
	jbe		NoExtra

; Round to zero
	dec		eax

; Store in the pointer
NoExtra:
	mov		dword ptr[ecx], eax
	ret		4
	}
	// clang-format on
}