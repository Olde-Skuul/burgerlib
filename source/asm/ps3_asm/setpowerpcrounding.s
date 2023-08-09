#
# Set the PowerPC floating point rounding value
#
# Sony Cell assembly for the Playstation 3
#
# Build with PS3 tool chain
#
# ePowerPCRounding BURGER_API Burger::set_PowerPC_rounding(
#	ePowerPCRounding uInput) BURGER_NOEXCEPT;
#

	.align 2

	.text

	.type	._ZN6Burger20set_PowerPC_roundingENS_16ePowerPCRoundingE,@function
	.globl	._ZN6Burger20set_PowerPC_roundingENS_16ePowerPCRoundingE
._ZN6Burger20set_PowerPC_roundingENS_16ePowerPCRoundingE:

# Get the floating point control register
	mffs	0

# Store it in memory
	stfd	0, -8(1)

# Load into an integer register
	lwz		4, -4(1)

# Get the return value, mask off the lowest 2 bits (AND & 3)
	clrlwi	5, 4, 30

# Move the lowest 2 bits into the existing value
	insrwi	4, 3, 2, 30

# Store it in memory
	stw		4, -4(1)

# Reload the fp register
	lfd		0, -8(1)

# Set the flags
	mtfsf	255, 0

# Return the previous value
	mr		3, 5

# Exit
	blr

	.size	._ZN6Burger20set_PowerPC_roundingENS_16ePowerPCRoundingE,.-._ZN6Burger20set_PowerPC_roundingENS_16ePowerPCRoundingE
