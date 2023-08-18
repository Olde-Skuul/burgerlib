;
; Espresso assembly for the WiiU
;
; Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; fInput BURGER_API Burger::modulo_radians(float fInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; Result in fp1
; fInput = fp1
;

	.weak	g_fReciprocalPi2__6Burger
	.weak	g_fHalf__6Burger
	.weak	g_fNegPi2__6Burger

	.align 2

	.text

	.global modulo_radians__6BurgerFf
modulo_radians__6BurgerFf:
	.type modulo_radians__6BurgerFf, @function

; Fetch the pointers
	lis		r3, %hiadj(g_fReciprocalPi2__6Burger)

; Create 0x4330000080000000 for integer to float conversions
	lis		r6, 0x4330
	lis		r4, %hiadj(g_fHalf__6Burger)
	lis		r7, 0x8000
	lis		r5, %hiadj(g_fNegPi2__6Burger)

; Load in 1/ 2Pi
	lfs		fp3, %lo(g_fReciprocalPi2__6Burger)(r3)

; Load in 0.5f
	lfs		fp4, %lo(g_fHalf__6Burger)(r4)

; Load in -Pi2
	lfs		fp5, %lo(g_fNegPi2__6Burger)(r5)

; (fInput*g_fReciprocalPi2)+g_fHalf
	fmadds	fp3, fp1, fp3, fp4

; Store 0x4330000080000000
	stw		r6, -16(SP)
	stw		r7, -12(SP)

; Load 0x4330000080000000 in FPU
	lfd		fp2, -16(SP)

; Convert to integer
	fctiwz	fp0, fp3

; Store the integer in memory (64 bit)
	stfd	fp0, -8(SP)

; Extract the low word
	lwz		r0, -4(SP)

; Flip the bit
	xor		r0, r0, r7

; Create a fake double
	stw		r6, -8(SP)

; Store the integer
	stw		r0, -4(SP)

; Load the rounded double
	lfd		fp0, -8(SP)

; Complete the int to float conversion
	fsubs	fp2, fp0, fp2

; Compare the two and get rid of the prerounded
	fcmpu	cr0, fp2, fp3
	ble		FModule1

; Load 1.0f (0.5 + 0.5)
	fadds	fp0, fp4, fp4

; Fixup
	fsubs	fp2, fp2, fp0

; Don't use fnmsubs, it doesn't handle the sign bit properly
; (fVar*-g_fPi2) + fInput
FModule1:
	fmadds	fp1, fp2, fp5, fp1
	blr

	.size	modulo_radians__6BurgerFf,$-modulo_radians__6BurgerFf
