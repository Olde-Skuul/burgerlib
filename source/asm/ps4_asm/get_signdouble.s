#
# Sony AMD64 assembly for the Playstation 4 / 5
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS4 or PS5 tool chain
#
# double Burger::get_sign(double dInput) BURGER_NOEXCEPT
# rax, rcx, rdx, r8-11 are volatile
# dInput = xmm0
# Result in xmm0
#

	.text
	.balign 16
	.globl	_ZN6Burger8get_signEd
	.type	_ZN6Burger8get_signEd, @function

_ZN6Burger8get_signEd:
	.cfi_startproc

# Does not use a stack frame
#	.cfi_def_cfa_offset 16

# Move to an integer register
	vmovd %xmm0, %rax

# Load 1.0, shifted up by one bit
	movq $0x7FE0000000000000, %rcx

# Obtain the sign bit in carry
	shlq $1, %rax

# Convert 1.0 to 1.0 or -1.0 depending on sign
	rcrq $1,%rcx

# Set carry if not zero
	negq %rax

# Temp is mask, zero or 0xFFFFFFFF
	sbbq %rax, %rax

# Mask 1.0/-1.0f to zero if zero
	andq %rax, %rcx

# Move result to float
	vmovd %rcx, %xmm0
	ret

	.size	_ZN6Burger8get_signEd, .-_ZN6Burger8get_signEd
	.cfi_endproc
