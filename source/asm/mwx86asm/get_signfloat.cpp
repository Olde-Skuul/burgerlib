/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	float __fastcall get_sign(float fInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	fInput = [esp+4]

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) float BURGER_API Burger::get_sign(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Get the sign (ip, fInput)
	mov		eax, [esp+4]

; Save the sign in the carry flag
	add		eax, eax

; If zero edge case, branch
	jz		itszero

; Load 1.0f
	fld1
	jnc		exit

; Convert to -1.0f
	fchs
exit:
	ret		4

; It is the zero case
itszero:
	fldz
	ret		4
	}
	// clang-format on
}