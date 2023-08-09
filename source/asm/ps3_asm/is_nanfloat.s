#
# Test if a float is a NaN
#
# Sony Cell assembly for the Playstation 3
#
# Build with PS3 tool chain
#
# uint_t BURGER_API Burger::is_NaN(float fInput) BURGER_NOEXCEPT
#

	.align 2

	.text

	.type	._ZN6Burger6is_NaNEf,@function
	.globl	._ZN6Burger6is_NaNEf
._ZN6Burger6is_NaNEf:

# Perform a NaN test
	fcmpu	0, 1, 1

# Move cr0 into r0
	mfcr	0

# (Flags>>28)&1 Grab the "Unordered" flag
	extrwi	3, 0, 1, 3

# Exit
	blr

	.size	._ZN6Burger6is_NaNEf,.-._ZN6Burger6is_NaNEf
