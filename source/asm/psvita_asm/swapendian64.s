#
# ARM32 assembly for implementing the intrinsic _swapendian64 for
# the Playstation Vita
#
# Build with PS Vita tool chain
#
# extern "C" uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT;
#

	.cpu	cortex-a9
	.fpu	neon
	.object_arch	armv7-a

	.balign 2
	.thumb_func
	.thumb
	.text

	.type _swapendian64, %function
	.global _swapendian64
_swapendian64:
	.fnstart

# Perform the endian swap on both 32 bit registers

	rev		r2,r0
	rev		r0,r1

# Finish swapping the two 32 bit values
	mov		r1,r2

# Return value in r0,r1
	bx		lr

	.size	_swapendian64,.-_swapendian64
