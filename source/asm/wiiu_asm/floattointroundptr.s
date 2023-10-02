;
; Espresso assembly for the WiiU
;
; Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; void BURGER_API Burger::float_to_int_round(int32_t* pOutput, float fInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; pOutput = r3
; fInput = fp1
;

	.weak	g_fMinNoInteger__6Burger

	.align 2

	.text

	.global float_to_int_round__6BurgerFPif
float_to_int_round__6BurgerFPif:
	.type float_to_int_round__6BurgerFPif, @function

; Get the address of g_fMinNoInteger
	lis		r4, %hiadj(g_fMinNoInteger__6Burger)

; Get the abs value to test
	fabs	fp0, fp1

; Load 8388608.0f
	lfs		fp2, %lo(g_fMinNoInteger__6Burger)(r4)

; Pointer to 0.5f
	lis		r4, %hiadj(g_fHalf__6Burger)

; Push the positive number to highest value without fraction
; (Removes fraction)
	fadds	fp5, fp0, fp2

; Load 0.5f
	lfs		fp3, %lo(g_fHalf__6Burger)(r4)

; Undo the push (Fraction is gone)
	fsubs	fp5, fp5, fp2

; Subtract original from rounded to get the fraction
	fsubs	fp6, fp0, fp5

; Test against 0.5
	fsubs	fp6, fp6, fp3

; Set to one
	fadds	fp3, fp3, fp3

; Set to zero
	fsubs	fp4, fp5, fp5

; Set to zero or one depending on the test
	fsel	fp0, fp6, fp3, fp4

; Add 0 for no rounding, 1 for round up
	fadds	fp5, fp5, fp0

; Get the negative value
	fnabs	fp2, fp5

; Which one to use? Positive or negative?
	fsel	fp1, fp1, fp5, fp2

; Convert to integer
	fctiwz	fp1, fp1

; Store the integer
	stfiwx	fp1, 0, r3
	blr

	.size	float_to_int_round__6BurgerFPif,$-float_to_int_round__6BurgerFPif
