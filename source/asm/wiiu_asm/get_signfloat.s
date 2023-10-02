;
; Espresso assembly for the WiiU
;
; Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; fInput BURGER_API Burger::get_sign(float fInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; Result in fp1
; fInput = fp1
;

	.weak	g_fHalf__6Burger

	.align 2

	.text

	.global get_sign__6BurgerFf
get_sign__6BurgerFf:
	.type get_sign__6BurgerFf, @function

; Test for NaN first, to avoid stalls later
	fcmpu	cr1, fp1, fp1

; Get 0.5
	lis		r3, %hiadj(g_fHalf__6Burger)

; -fInput
	fneg	fp2, fp1
	lfs		fp3, %lo(g_fHalf__6Burger)(r3)

; -0.5
	fneg	fp4, fp3

; Test twice, to also find 0.0f
	fsel	fp0, fp1, fp3, fp4
	fsel	fp4, fp2, fp4, fp3

; Yields 0.0f, 1.0f, or -1.0f
	fadds	fp1, fp4, fp0

; If not NaN, return now
	beqlr	cr1

; WiiU stack frame
	stwu	sp, -24(sp)

; NaN detected, you are boned
; Store the -NaN
	stfs	fp2, 8(sp)

; Assume result is 1.0f (0.5*2)
	fadds	fp1, fp3, fp3

; Test for negative with integer registers
	lwz		r3, 8(sp)
	andis.	r3, r3, 0x8000
	addi	sp, sp, 24

; Note: Testing on -NaN so not equal is positive
	bnelr

; Return negative
	fneg	fp1, fp1
	blr

	.size	get_sign__6BurgerFf,$-get_sign__6BurgerFf
