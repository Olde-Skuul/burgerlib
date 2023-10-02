;
; Espresso assembly for the WiiU
;
; Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; int32_t BURGER_API Burger::float_to_int_floor(float fInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; Result in r3
; fInput = fp1
;

	.weak	g_fMinNoInteger__6Burger

	.align 2

	.text

	.global float_to_int_floor__6BurgerFf
float_to_int_floor__6BurgerFf:
	.type float_to_int_floor__6BurgerFf, @function

; WiiU stack frame
	stwu	sp, -24(sp)

; Get the address of g_fMinNoInteger
	lis		r3, %hiadj(g_fMinNoInteger__6Burger)

; Save the rounding register
	mffs	fp4
	
; Load 8388608.0f
	lfs		fp2, %lo(g_fMinNoInteger__6Burger)(r3)
	
; Set bits 30 and 31 to Round toward -infinity
	mtfsfi	7, 0x03

; Push the positive number to highest value without fraction
	fadds	fp3, fp1, fp2

; Push the negative number to the lowest value without fraction
	fsubs	fp0, fp1, fp2

; Undo the push (Fraction is gone)
	fsubs	fp3, fp3, fp2
	
; Undo the push (Fraction is gone)
	fadds	fp0, fp0, fp2

; Which one to use? Positive or negative?
	fsel	fp1, fp1, fp3, fp0

; Restore rounding
	mtfsf	255, fp4

; Convert to integer
	fctiwz	fp1, fp1

; Pointer to temp on the stack
	addi	r3, sp, 8

; Store the integer
	stfiwx	fp1, 0, r3

; Return the integer
	lwz		r3, 0(r3)
	addi	sp, sp, 24
	blr

	.size	float_to_int_floor__6BurgerFf,$-float_to_int_floor__6BurgerFf
