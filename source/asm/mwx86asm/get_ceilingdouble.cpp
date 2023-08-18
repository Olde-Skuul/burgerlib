/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	double __fastcall get_ceiling(double dInput)
	eax, ecx and edx are volatile
	Result in 8087 FPU
	dInput = [esp+4]

***************************************/

#include "brfloatingpoint.h"

__declspec(naked) double BURGER_API Burger::get_ceiling(
	double /* dInput */) BURGER_NOEXCEPT
{
	// clang-format off
	asm {

; Load the value in an integer register
	mov		eax, [esp + 8]

; Load the same value in the FPU
	fld		qword ptr[esp + 4]

; Mask off the sign
	and		eax, 0x7FFFFFFF

; Compare to 4503599627370496.0
	cmp		eax, 0x43300000

; Out of range, return original value
	jae		CeilExit

; Convert the integer to float (It is in range)
	frndint

; Clear ax for flags
	xor		eax, eax

; Compare values for difference (Pop stack)
	fcom	qword ptr[esp + 4]
	fnstsw	ax

; Was the new less than the original?
	and		eax, 0x100
	jz		CeilExit

; Add 1.0f to round up
	fadd	dword ptr[g_fOne]
CeilExit:
	ret		8
	}
	// clang-format on
}