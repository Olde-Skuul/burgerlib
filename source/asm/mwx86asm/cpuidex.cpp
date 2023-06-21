/***************************************

	X86 assembly for implementing the intrinsic __cpuidex for
	Metrowerks Codewarrior

	Build with the C compiler

***************************************/

#include "brmetrowerks.h"

__declspec(naked) void __cdecl __cpuidex(int*, int, int)
{
	asm
	{
		// clang-format off
	push	esi
	push	ebx
; Save the pointer
	mov		esi, [esp+12]
; Command byte
	mov		eax, [esp+16]
; Get the sub command
	mov		ecx, [esp+20]

; Do it (Trashes ebx)
	cpuid

; Store the result in the same order as Visual C
    mov		[esi],eax
    mov		[esi+4],ebx
    mov		[esi+8],ecx
    mov		[esi+12],edx
    pop		ebx
    pop		esi
	ret

		// clang-format on
	}
}
