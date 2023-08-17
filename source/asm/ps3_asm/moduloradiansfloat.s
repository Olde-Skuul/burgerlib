#
# Sony Cell assembly for the Playstation 3
#
# Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with PS3 tool chain
#
# float BURGER_API Burger::ModuloRadians(float fInput) BURGER_NOEXCEPT
# r0, r3-12, fp0-fp13 are volatile
# fInput = fp1
# Result in fp1
#

	.align 2

	.text

	.type	._ZN6Burger13ModuloRadiansEf,@function
	.globl	._ZN6Burger13ModuloRadiansEf
._ZN6Burger13ModuloRadiansEf:

# Fetch the pointers
	lis		3, _ZN6Burger16g_fReciprocalPi2E@ha

# Create 0x4330000080000000 for integer to float conversions
	lis		6, 0x4330
	lis		4, _ZN6Burger7g_fHalfE@ha
	lis		7, 0x8000
	lis		5, _ZN6Burger9g_fNegPi2E@ha

# Load in 1/ 2Pi
	lfs		3, _ZN6Burger16g_fReciprocalPi2E@l(3)

# Load in 0.5f
	lfs		4, _ZN6Burger7g_fHalfE@l(4)

# Load in Pi2
	lfs		5, _ZN6Burger9g_fNegPi2E@l(5)

# (fInput*g_fReciprocalPi2)+g_fHalf
	fmadds	3, 1, 3, 4

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
	fsubs 	2,0,2

# Compare the two and get rid of the pre-rounded
	fcmpu 	cr0,2,3
	ble		1f

# Load 1.0f
	fadds	0, 4, 4

# Fixup
	fsubs	2, 2, 0

# Don't use fnmsubs, it doesn't handle the sign bit properly
# (fVar*-g_fPi2) + fInput
1:	fmadds	1, 2, 5, 1
	blr

	.size	._ZN6Burger13ModuloRadiansEf,.-._ZN6Burger13ModuloRadiansEf
