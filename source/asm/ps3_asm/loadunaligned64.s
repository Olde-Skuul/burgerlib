#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# uint64_t BURGER_API Burger::_load_unaligned(const uint64_t* pInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# uInput = r3
# Result in r3
#

	.align 2

	.text

	.type	._ZN6Burger15_load_unalignedEPKy,@function
	.globl	._ZN6Burger15_load_unalignedEPKy
._ZN6Burger15_load_unalignedEPKy:

# r7-r10 will get the upper 32 bits
	lbz		10,0(3)
	lbz		9,1(3)
	lbz		8,2(3)
	lbz		7,3(3)

# r6-r3 will get the lower 32 bits
	lbz		6,4(3)
	rlwimi	7,8,8,16,23
	lbz		5,5(3)
	rlwimi	7,9,16,8,15
	lbz		4,6(3)
	rlwimi	7,10,24,0,7
	lbz		3,7(3)

# r7 will have the upper 32 bits
	sldi	7,7,32

# Merge the lower 32 bits
	rlwimi	3,4,8,16,23
	rlwimi	3,5,16,8,15
	rlwimi	3,6,24,0,7

# Merge everything
	or		3,3,7
	blr

	.size	._ZN6Burger15_load_unalignedEPKy,.-._ZN6Burger15_load_unalignedEPKy