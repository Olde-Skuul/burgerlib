#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# double BURGER_API Burger::ModuloRadians(double dInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# dInput = fp1
# Result in fp1
#

	.align 2

	.text

	.type	._ZN6Burger13ModuloRadiansEd,@function
	.globl	._ZN6Burger13ModuloRadiansEd
._ZN6Burger13ModuloRadiansEd:

# Fetch the pointers
	lis		3, _ZN6Burger16g_dReciprocalPi2E@ha

# Create 0x4330000080000000 for integer to float conversions
	lis		6, 0x4330
	lis		4, _ZN6Burger7g_dHalfE@ha
	lis		7, 0x8000
	lis		5, _ZN6Burger9g_dNegPi2E@ha

# Load in 1/ 2Pi
	lfd		3, _ZN6Burger16g_dReciprocalPi2E@l(3)

# Load in 0.5f
	lfd		4, _ZN6Burger7g_dHalfE@l(4)

# (dInput*g_dReciprocalPi2)
	fmul	3, 1, 3

# Load in Pi2
	lfd		5, _ZN6Burger9g_dNegPi2E@l(5)

# (dInput*g_dReciprocalPi2)+g_fHalf
	fadd	3, 3, 4

# fmadd doesn't handle sign properly so it failed the unit test
# Used explicit fmul and fadd to get the accuracy
# (dInput*g_dReciprocalPi2)+g_dHalf
#	fmadd	3, 1, 3, 4

# Store 0x4330000080000000
	stw		6, -16(1)
	stw		7, -12(1)

# Load 0x4330000080000000 in FPU
	lfd		2, -16(1)

# Convert to integer
	fctiwz	0, 3

# Store the integer in memory (64 bit)
	stfd	0, -8(1)

# Extract the low word
	lwz		0, -4(1)

# Flip the bit
	xor		0,0,7

# Create a fake double
	stw		6,-8(1)

# Store the integer
	stw		0,-4(1)

# Load the rounded double
	lfd		0,-8(1)

# Complete the int to float conversion
	fsub 	2,0,2

# Compare the two and get rid of the pre-rounded
	fcmpu 	cr0,2,3
	ble		1f

# Load 1.0f
	fadd	0, 4, 4

# Fixup
	fsub	2, 2, 0

# Don't use fnmsubs, it doesn't handle the sign bit properly
# (dVar*-g_dPi2) + dInput
1:	fmadd	1, 2, 5, 1
	blr

	.size	._ZN6Burger13ModuloRadiansEd,.-._ZN6Burger13ModuloRadiansEd
