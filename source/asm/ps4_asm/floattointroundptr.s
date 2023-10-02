#
# X64 assembly for the Clang and GNU tool chain
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with as
#
# void Burger::float_to_int_round(int32_t* pOutput, float fInput) BURGER_NOEXCEPT
# rax, rcx, rdx, r8-11 are volatile
# pOutput = rdi
# fInput = xmm0
#

	.align 8
	.text
	.globl	_ZN6Burger18float_to_int_roundEPif
	.type	_ZN6Burger18float_to_int_roundEPif, @function

_ZN6Burger18float_to_int_roundEPif:
	.cfi_startproc

# Does not use a stack frame
#	.cfi_def_cfa_offset 16

# Convert to integer with round to zero
	cvttps2dq	%xmm0, %xmm2

# Save the sign for later
	movd		%xmm0, %ecx

# Convert back to float
	cvtdq2ps	%xmm2, %xmm1

# Move sign to bit 0x01
	shrl		$31, %ecx

# Get the difference
	subss		%xmm1, %xmm0

# 0/1 -> 0/2
	shll		$1, %ecx

# Get the absolute value mask
	movd		LC0(%rip), %xmm3

# 0/2 -> -1, 1
	decl		%ecx

# Get the absolute value
	pand		%xmm3, %xmm0

# Move the difference to integer register
	cmpnltss	LC1(%rip), %xmm0
	
# Get the mask
	movd		%xmm0, %eax
	
# Mask off the adjustment
	andl		%eax, %ecx
	
# Return the result
	movd		%xmm2, %eax
	subl		%ecx, %eax
	movl		%eax, (%rdi)
	ret

	.align	2
LC0:
	.long	0x7FFFFFFF
	
LC1:
	.long	0x3F000000		# 0.5f

	.size	_ZN6Burger18float_to_int_roundEPif, .-_ZN6Burger18float_to_int_roundEPif
	.cfi_endproc
