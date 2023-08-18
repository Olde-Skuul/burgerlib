/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	double __fastcall modulo_radians(double dInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	dInput = [esp+4]

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) double BURGER_API Burger::modulo_radians(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Load into the FPU
	fld		qword ptr[esp + 4]

; Load in 1/2Pi
	fld		qword ptr[g_dReciprocalPi2]

; Multiply
	fmul	st(0), st(1)

; Add half for rounding
	fadd	qword ptr[g_dHalf]

; Store at double precision
	fst		qword ptr[esp + 4]

; Remove fraction
	frndint

; Compare the two and get rid of the pre-rounded
	fcom	qword ptr[esp + 4]
	fnstsw	ax
	test	ah, 0x41

; Did it round up?
	jnz		ModuloRadiansFExit

; Fixup
	fsub	qword ptr[g_dOne]
ModuloRadiansFExit:

; Mul by 2 pi
	fmul	qword ptr[g_dPi2]

; Subtract and clean up
	fsubp	st(1), st(0)
	ret		8
	}
	// clang-format on
}