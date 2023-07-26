#
# Get the PowerPC floating point rounding value
#
# Sony Cell assembly for the Playstation 3
#
# Build with PS3 tool chain
#
# ePowerPCRounding BURGER_API Burger::GetPowerPCRounding(void) BURGER_NOEXCEPT;
#

	.align 2

	.text

	.type	._ZN6Burger18GetPowerPCRoundingEv,@function
	.globl	._ZN6Burger18GetPowerPCRoundingEv
._ZN6Burger18GetPowerPCRoundingEv:

# Get the floating point control register
	mffs	0

# Store it in memory
	stfd	0, -8(1)

# Load into an integer register
	lwz		3, -4(1)

# Mask off the lowest 2 bits (AND & 3)
	clrlwi	3, 3, 30

# Exit
	blr

	.size	._ZN6Burger18GetPowerPCRoundingEv,.-._ZN6Burger18GetPowerPCRoundingEv
