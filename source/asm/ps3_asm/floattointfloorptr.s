#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# void BURGER_API Burger::float_to_int_floor(int32_t* pOutput, float fInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# pOutput = r3
# fInput = fp1
#

	.align 2

	.text

	.type	._ZN6Burger18float_to_int_floorEPif,@function
	.globl	._ZN6Burger18float_to_int_floorEPif
._ZN6Burger18float_to_int_floorEPif:

# Convert the input into an integer
	fctidz	2, 1

# Load 1.0
	lis		4, _ZN6Burger6g_fOneE@ha

# Convert back to a double
	fcfid	2, 2

# f0 = 1.0
	lfs		0,	_ZN6Burger6g_fOneE@l(4)

# Check if there was a fraction
	fsub	1, 1, 2

# Round down by 1.0
	fsub	0, 2, 0
	
# If there was a fraction, use the round down version
	fsel	1, 1, 2, 0

# Convert to integer again
	fctiw	1, 1

# Store at sp-16 (Temp)
	stfiwx	1, 0, 3
	blr

	.size	._ZN6Burger18float_to_int_floorEPif,.-._ZN6Burger18float_to_int_floorEPif
