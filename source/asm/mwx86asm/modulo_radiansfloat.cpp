/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	float __fastcall modulo_radians(float fInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	fInput = [esp+4]

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) float BURGER_API Burger::modulo_radians(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Load into the FPU
	fld		dword ptr[esp + 4]

; Load in 1/ 2Pi
	fld		dword ptr[g_fReciprocalPi2]

; Multiply (Really fInput/2Pi)
	fmul	st(0), st(1)

; Add half for rounding
	fadd	dword ptr[g_fHalf]

; Round to nearest in 24 bit to force consistent precision
	fst		dword ptr[esp + 4]

; Convert to integer
	frndint

; Compare the two and get rid of the pre-rounded
	fcom	dword ptr[esp + 4]
	fnstsw ax

; Branch if less or equal
	test	ah, 0x41

; Did it round up?
	jne		ModuloRadiansFExit

; Fixup
	fsub	dword ptr[g_fOne]
ModuloRadiansFExit:

; Mul by 2 pi
	fmul	dword ptr[g_fPi2]

; Subtract and clean up
	fsubp	st(1), st(0)
	ret		4
	}
	// clang-format on
}