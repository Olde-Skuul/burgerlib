;
; Espresso assembly for the WiiU
;
; Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; void BURGER_API Burger::float_to_int_ceil(int32_t* pOutput, float fInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; pOutput = r3
; fInput = fp1
;

	.weak	g_fMinNoInteger__6Burger

	.align 2

	.text

	.global float_to_int_ceil__6BurgerFPif
float_to_int_ceil__6BurgerFPif:
	.type float_to_int_ceil__6BurgerFPif, @function

; Get the address of g_fMinNoInteger
	lis		r4, %hiadj(g_fMinNoInteger__6Burger)

; Save the rounding register
	mffs	fp4

; Load 8388608.0f
	lfs		fp2, %lo(g_fMinNoInteger__6Burger)(r4)

; Set bits 30 and 31 to Round toward +infinity
	mtfsfi	7, 0x02

; Push the positive number to highest value without fraction
; (Removes fraction)
	fadds	fp3, fp1, fp2

; Push the negative number to the lowest value without fraction
; (Removes fraction)
	fsubs	fp0, fp1, fp2

; Undo the push (Fraction is gone)
	fsubs	fp3, fp3, fp2

; Undo the push (Fraction is gone)
	fadds	fp0, fp0, fp2

; Which one to use? Positive or negative?
	fsel	fp1, fp1, fp3, fp0

; Restore rounding
	mtfsf	255, fp4

; Conver to integer
	fctiwz	fp1, fp1

; Store the integer
	stfiwx	fp1, 0, r3
	blr

	.size	float_to_int_ceil__6BurgerFPif,$-float_to_int_ceil__6BurgerFPif
