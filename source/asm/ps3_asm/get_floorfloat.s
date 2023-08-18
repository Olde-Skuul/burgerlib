#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# float BURGER_API Burger::get_floor(float fInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# fInput = fp1
# Result in fp1
#

	.align 2

	.text

	.type	._ZN6Burger9get_floorEf,@function
	.globl	._ZN6Burger9get_floorEf
._ZN6Burger9get_floorEf:

# Convert the float to an integer, round to zero
	fctiwz	2, 1

# Prep for loading 1.0f
	lis		3, _ZN6Burger6g_fOneE@ha

# Get abs fInput
	fabs	6, 1

# Convert back to float without fraction, possible final answer
	fcfid	2, 2

# Prep for loading 8388608.0f
	lis		4, _ZN6Burger15g_fMinNoIntegerE@ha

# Fetch 1.0f in fp3
	lfs		3, _ZN6Burger6g_fOneE@l(3)

# Fetch 8388608.0f in fp4
	lfs		4, _ZN6Burger15g_fMinNoIntegerE@l(4)

# Take the answer and subtract 1.0f
	fsubs	3, 2, 3

# fp6 has abs(fInput) - 8388608.0f
	fsubs	6, 6, 4

# fp5 has fInput - answer, to test if it overflowed
	fsubs	5, 1, 2

# answer = ((fInput-answer)>=0) ? answer : answer - 1.0f
	fsel	2, 5, 2, 3

# answer = (fabs(fInput) - 8388608)>=0 ? fInput, answer
	fsel	1, 6, 1, 2
	blr

	.size	._ZN6Burger9get_floorEf,.-._ZN6Burger9get_floorEf
