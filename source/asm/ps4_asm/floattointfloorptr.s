#
# X64 assembly for the Clang and GNU tool chain
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with as
#
# void Burger::float_to_int_floor(int32_t* pOutput, float fInput) BURGER_NOEXCEPT
# rax, rcx, rdx, r8-11 are volatile
# pOutput = rdi
# fInput = xmm0
#

	.align 8
	.text
	.globl	_ZN6Burger18float_to_int_floorEPif
	.type	_ZN6Burger18float_to_int_floorEPif, @function

_ZN6Burger18float_to_int_floorEPif:
	.cfi_startproc

# Does not use a stack frame
#	.cfi_def_cfa_offset 16

# Convert to integer with round to zero
	cvttps2dq	%xmm0, %xmm2

# Convert back to float
	cvtdq2ps	%xmm2, %xmm1

# Should it round down?
	cmpltss		%xmm1, %xmm0

# Add either 0 or -1
	paddd		%xmm0, %xmm2

# Move to integer register
	movd		%xmm2, (%rdi)
	ret

	.size	_ZN6Burger18float_to_int_floorEPif, .-_ZN6Burger18float_to_int_floorEPif
	.cfi_endproc
