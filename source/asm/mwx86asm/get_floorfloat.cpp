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

; Load the value in an integer register
	mov		eax, [esp + 4]

; Load the same value in the FPU
	fld		dword ptr[esp + 4]

; Mask off the sign
	and		eax, 0x7FFFFFFF

; Compare to 8388608.0f
	cmp		eax, 0x4B000000

; Out of range, return original value
	jae		FloorExit

; Convert the integer to float (It is in range)
	frndint

; Clear ax for flags
	xor		eax, eax

; Compare values for difference (Pop stack)
	fcom	dword ptr[esp + 4]

; Get the 8087 condition flags
	fnstsw	ax

; Was the new greater than or equal the original?
	and		ax, 0x4100
	jnz		FloorExit

; Subtract 1.0f to round down
	fsub	dword ptr[g_fOne]
FloorExit:
	ret		4
	}
	// clang-format on
}