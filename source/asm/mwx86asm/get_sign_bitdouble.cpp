/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	uint_t __fastcall get_sign_bit(double dInput)
	eax, ecx and edx are volatile
	Result in eax
	dInput = [esp+4]

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) uint_t BURGER_API Burger::get_sign_bit(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {
; Get the sign (ip, low, high)
	mov		eax, [esp+8]

; Save the sign in the bottom bit
	shr		eax, 31
	ret		8
	}
	// clang-format on
}
