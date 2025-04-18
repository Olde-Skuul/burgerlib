#
# Sony AMD64 assembly for the Playstation 4 / 5
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS4 or PS5 tool chain
#
# float Burger::get_sign(float fInput) BURGER_NOEXCEPT
# rax, rcx, rdx, r8-11 are volatile
# fInput = xmm0
# Result in xmm0
#

	.text
	.balign 16
	.globl	_ZN6Burger8get_signEf
	.type	_ZN6Burger8get_signEf, @function

_ZN6Burger8get_signEf:
	.cfi_startproc

# Does not use a stack frame
#	.cfi_def_cfa_offset 16

# Move to an integer register
	vmovd	%xmm0, %eax

# Load 1.0, shifted up by one bit
	movl	$0x7F000000, %ecx

# Obtain the sign bit in carry
	shll	$1, %eax

# Convert 1.0 to 1.0 or -1.0 depending on sign
	rcrl	$1,%ecx

# Set carry if not zero
	negl	%eax

# Temp is mask, zero or 0xFFFFFFFF
	sbbl	%eax, %eax

# Mask 1.0/-1.0f to zero if zero
	andl	%eax, %ecx

# Move result to float
	vmovd	%ecx, %xmm0
	ret

	.size	_ZN6Burger8get_signEf, .-_ZN6Burger8get_signEf
	.cfi_endproc
