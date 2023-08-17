#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# extern "C" uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# uInput = r3
# Result in r3
#

	.align 2

	.text

	.type ._swapendian64, @function
	.globl ._swapendian64
._swapendian64:

# Split to 2 32 bit registers
	srdi	4,3,32

# 4,1,2,3 <- 1,2,3,4
	rlwinm	0,3,24,0,31
	rlwinm	12,4,24,0,31

# 4,3,2,3 <- 1,2,3,4 (Only copy bits 8-15)
	rlwimi	0,3,8,8,15
	rlwimi	12,4,8,8,15

# 4,3,2,1 <- 1,2,3,4 (Only copy bits 24-31)
	rlwimi	0,3,8,24,31
	rlwimi	12,4,8,24,31

	sldi	3,0,32
	or		3,3,12
	blr

	.size	._swapendian64,.-._swapendian64
