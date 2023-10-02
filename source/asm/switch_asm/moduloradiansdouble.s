#
# Nintendo assembly for the Switch
#
# Copyright 2020-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with Nintedo Switch tool chain
#
# double BURGER_API Burger::modulo_radians(double dInput) BURGER_NOEXCEPT
#

/***************************************

	ARM 32 Version
	
	r0-r3, s0-s15 are volatile
	fInput = v0
	Result in v0

***************************************/

#if defined(__arm__)
	.section	.text._ZN6Burger14modulo_radiansEd,"ax",%progbits
	.globl		_ZN6Burger14modulo_radiansEd
	.p2align	2
	.type		_ZN6Burger14modulo_radiansEd,%function
	.code		32

_ZN6Burger14modulo_radiansEd:
	.fnstart
	.cfi_sections .debug_frame
	.cfi_startproc

# Save registers since calls are needed (Save 2 to maintain 64 bit alignment)
	.save			{r11, lr}
	push			{r11, lr}

# Fetch the pointers and constants
# d2 = g_dReciprocalPi2, d4 = 0.5, d6 = 1.0, d4 = g_dPi2

# Get g_fReciprocalPi2
	vldr			d2, .g_dReciprocalPi2
	vmov.f64		d1, #0.50000000

# (fInput * g_fReciprocalPi2 + 0.5f
# Do not use the fmadd instruction
# It fails unit tests due to sign being flipped
#	vfma.f64		d1, d0, d2
	vmul.f64		d2, d0, d2
	vadd.f64		d1, d2, d1

# Convert to integer and back again
# fVar is d2
	vmov			r0, r1, d1

# Save the values that can't be destroyed
	.vsave			{d0, d1}
	vpush			{d0, d1}
	bl				__aeabi_d2lz
	bl				__aeabi_l2d
	vmov			d2, r0, r1

# Restore the values
	vpop			{d0, d1}

# Subtract 1.0f for next integer
	vmov.f64		d3, #1.00000000
	vsub.f64		d3, d2, d3

# If (fVar > fTemp) fVar = f3
	vcmp.f64		d2, d1
	vmrs			APSR_nzcv, fpscr
	vselgt.f64		d2, d3, d2

# fInput - (fVar * g_fPi2)
	vldr			d3, .g_dPi2
	vfms.f64		d0, d2, d3

# Restore
	pop				{r11, pc}

	.p2align	3

# Constants, localized
.g_dReciprocalPi2:
	.long	0x6DC9C883
	.long	0x3FC45F30
.g_dPi2:
	.long	0x54442D18
	.long	0x401921FB

	.size	_ZN6Burger14modulo_radiansEd, .-_ZN6Burger14modulo_radiansEd
	.cfi_endproc
	.fnend

#endif

/***************************************

	ARM 64 Version
	
	x0-x15, v0-v7, v16-v31 are volatile
	fInput = v0
	Result in v0

***************************************/

#if defined(__aarch64__)
	.section	.text._ZN6Burger14modulo_radiansEd,"ax",@progbits
	.globl		_ZN6Burger14modulo_radiansEd
	.p2align	4
	.type		_ZN6Burger14modulo_radiansEd,@function
_ZN6Burger14modulo_radiansEd:
	.cfi_startproc

# Fetch the pointers and constants
# d1 = g_dReciprocalPi2, d2 = 0.5, d3 = 1.0, d4 = g_dPi2
	mov		x8, #0xC883
	mov		x9, #0x2D18
	fmov	d2, #0.50000000
	movk	x8, #0x6DC9, lsl #16
	movk	x9, #0x5444, lsl #16
	fmov	d3, #1.00000000
	movk	x8, #0x5F30, lsl #32
	movk	x9, #0x21FB, lsl #32
	movk	x8, #0x3FC4, lsl #48
	movk	x9, #0x4019, lsl #48

# Get g_dReciprocalPi2
	fmov	d1, x8

# (fInput * g_dReciprocalPi2 + 0.5f
# Do not use the fmadd instruction
# It fails unit tests due to sign being flipped
#	fmadd	d1, d0, d1, d2
	fmul	d1, d0, d1
	fadd	d1, d1, d2

# Convert to integer and back again
# dVar is d2
	fcvtzs	d2, d1

# Get g_dPi2
	fmov	d4, x9
	scvtf	d2, d2

# Subtract 1.0f for next integer
	fsub	d3, d2, d3

# If (dVar > dTemp) dVar = d3
	fcmp	d1, d2
	fcsel	d1, d3, d2, mi

# dInput - (dVar * g_dPi2)
	fmsub	d0, d1, d4, d0
	ret

	.size	_ZN6Burger14modulo_radiansEd,.-_ZN6Burger14modulo_radiansEd
	.cfi_endproc
#endif
