/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	double __fastcall get_sign(double dInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	dInput = [esp+4]

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) double BURGER_API Burger::get_sign(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Get the sign (High word) (eax, ip, low, high)
	mov		eax, [esp+8]

; Save the sign in the carry flag
	add		eax, eax

; Save the carry flag
	pushfd

; Check if actually zero (64 bit) (flags, eax, ip, low)
	or		eax, [esp+8]
	jz		itszero

; Restore the carry to return 1 or -1
	popfd

; Load 1.0
	fld1
	jnc		exit

; Convert to -1.0
	fchs
exit:
	ret		8

; It is the zero case
itszero:
; Pop the flags and restore eax
	popfd
	fldz
	ret		8
	}
	// clang-format on
}