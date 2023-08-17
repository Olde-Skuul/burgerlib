/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	uint_t __fastcall get_sign_bit(float fInput)
	eax, ecx and edx are volatile
	Result in eax
	fInput = [esp+4]

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) uint_t BURGER_API Burger::get_sign_bit(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Get the sign (ip, fInput)
	mov		eax, [esp+4]

; Save the sign in the bottom bit
	shr		eax, 31
	ret		4
	}
	// clang-format on
}
