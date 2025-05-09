/***************************************

	X86 assembly for Metrowerks Codewarrior

	Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com

	Build with the Metrowerks 9.0 Pro C compiler

	void __fastcall __cpuid(int* a, int b)
	eax, ecx and edx are volatile
	a = ecx
	b = edx

***************************************/

#include "brintrinsics.h"

__declspec(naked) void __fastcall __cpuid(int*, int)
{
	asm
	{

; These registers must be preseved
	push	esi
	push	ebx

; Save the output pointer
	mov		esi, ecx

; Command byte
	mov		eax, edx

; Set the sub command to zero
	xor		ecx, ecx

; Do it (Trashes ebx)
	cpuid

; Store the result in the same order as Visual C
	mov		[esi], eax
	mov		[esi+4], ebx
	mov		[esi+8], ecx
	mov		[esi+12], edx

; Restore the non-volatile registers
	pop 	ebx
	pop		esi
	ret

	}
}
