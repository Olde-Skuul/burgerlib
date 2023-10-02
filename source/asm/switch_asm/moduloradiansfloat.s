#
# Nintendo assembly for the Switch
#
# Copyright 2020-2023 by Rebecca Ann Heineman becky@burgerbecky.com
#
# Build with Nintedo Switch tool chain
#
# float BURGER_API Burger::modulo_radians(float fInput) BURGER_NOEXCEPT
#

/***************************************

	ARM 32 Version
	
	r0-r3, s0-s15 are volatile
	fInput = v0
	Result in v0

***************************************/

#if defined(__arm__)
	.section	.text._ZN6Burger14modulo_radiansEf,"ax",%progbits
	.globl		_ZN6Burger14modulo_radiansEf
	.p2align	2
	.type		_ZN6Burger14modulo_radiansEf,%function
	.code		32

_ZN6Burger14modulo_radiansEf:
	.fnstart
	.cfi_sections .debug_frame
	.cfi_startproc

# Fetch the pointers and constants
# s2 = g_fReciprocalPi2, s4 = 0.5, s6 = 1.0, s4 = g_fPi2

# Get g_fReciprocalPi2
	vldr			s2, .g_fReciprocalPi2
	vmov.f32		s4, #0.50000000
	vmov.f32		s6, #1.00000000

# (fInput * g_fReciprocalPi2 + 0.5f
	vfma.f32		s4, s0, s2

# Convert to integer and back again
# fVar is s2
	vcvt.s32.f32	s2, s4
	vcvt.f32.s32	s2, s2

# Subtract 1.0f for next integer
	vsub.f32		s6, s2, s6

# If (fVar > fTemp) fVar = f3
	vcmp.f32		s2, s4

# Get g_fPi2
	vldr			s4, .g_fPi2
	vmrs			APSR_nzcv, fpscr
	vselgt.f32		s2, s6, s2

# fInput - (fVar * g_fPi2)
	vfms.f32		s0, s2, s4
	bx	lr

	.p2align	2

# Constants, localized
.g_fReciprocalPi2:
	.long	0x3E22F983
.g_fPi2:
	.long	0x40C90FDB

	.size	_ZN6Burger14modulo_radiansEf, .-_ZN6Burger14modulo_radiansEf
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
	.section	.text._ZN6Burger14modulo_radiansEf,"ax",@progbits
	.globl		_ZN6Burger14modulo_radiansEf
	.p2align	4
	.type		_ZN6Burger14modulo_radiansEf,@function
_ZN6Burger14modulo_radiansEf:
	.cfi_startproc

# Fetch the pointers and constants
# s1 = g_fReciprocalPi2, s2 = 0.5, s3 = 1.0, s4 = g_fPi2
	mov		w8, #0xF983
	mov		w9, #0x0FDB
	fmov	s2, #0.50000000
	movk	w8, #0x3E22, lsl #16
	fmov	s3, #1.00000000

# Get g_fReciprocalPi2
	fmov	s1, w8
	movk	w9, #0x40C9, lsl #16

# (fInput * g_fReciprocalPi2 + 0.5f
	fmadd	s1, s0, s1, s2

# Convert to integer and back again
# fVar is s2
	fcvtzs	s2, s1

# Get g_fPi2
	fmov	s4, w9
	scvtf	s2, s2

# Subtract 1.0f for next integer
	fsub	s3, s2, s3

# If (fVar > fTemp) fVar = f3
	fcmp	s1, s2
	fcsel	s1, s3, s2, mi

# fInput - (fVar * g_fPi2)
	fmsub	s0, s1, s4, s0
	ret

	.size	_ZN6Burger14modulo_radiansEf,.-_ZN6Burger14modulo_radiansEf
	.cfi_endproc
#endif
