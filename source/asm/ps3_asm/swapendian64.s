#
# Espresso assembly for implementing the intrinsic _swapendian64 for
# the Playstation 3
#
# Build with PS3 tool chain
#
# extern "C" uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT;
#

	.align 2

	.text

	.global _swapendian64
_swapendian64:
	.type _swapendian64, @function

# Perform the endian swap on both 32 bit registers

# 4,1,2,3 <- 1,2,3,4
	rlwinm	0,3,24,0,31
	rlwinm	12,4,24,0,31

# 4,3,2,3 <- 1,2,3,4 (Only copy bits 8-15)
	rlwimi	0,3,8,8,15
	rlwimi	12,4,8,8,15

# 4,3,2,1 <- 1,2,3,4 (Only copy bits 24-31)
	rlwimi	0,3,8,24,31
	rlwimi	12,4,8,24,31

# Return them, word swapped!
	mr		3,12
	mr		4,0		
	blr

	.size	_swapendian64,.-_swapendian64
        