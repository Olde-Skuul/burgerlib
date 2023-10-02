/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	Fixed32 __fastcall float_to_fixed_round(float fInput)
	eax, ecx and edx are volatile
	Result in eax
	fInput = [esp+4]

***************************************/

#include "brfixedpoint.h"
#include "brfloatingpoint.h"

__declspec(naked) Fixed32 BURGER_API Burger::float_to_fixed_round(
	float /* fInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Get the sign bit
	mov		eax, dword ptr[esp + 4]

; Load into the FPU
	fld		dword ptr[esp + 4]

; Convert to fixed point
	fmul	dword ptr[g_f65536]

; 1 for negative, 0 for positive
	shr		eax, 31

; Save state
	fstcw	[esp + 4]

; Set round down
	fldcw	word ptr[g_u8087RoundToZero]

; Add or subtract 0.5f
	fadd	dword ptr[g_X86HalfAndNegHalf + eax * 4]

; Round
	frndint

; Restore rounding
	fldcw	[esp + 4]

; Store integer
	fistp	dword ptr[esp + 4]

; Get the result
	mov		eax, dword ptr[esp + 4]
	ret		4
	}
	// clang-format on
}