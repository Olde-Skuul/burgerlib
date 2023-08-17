;
; Espresso assembly for the WiiU
;
; Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; uint_t BURGER_API Burger::is_NaN(float fInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; Result in r3
; fInput = fp1
;

	.align 2

	.text

	.global is_NaN__6BurgerFf
is_NaN__6BurgerFf:
	.type is_NaN__6BurgerFf, @function

; Perform a NaN test
	fcmpu	cr0, f1, f1

; Move cr0 into r0
	mfcr	r0

; (Flags>>28)&1 Grab the "Unordered" flag
	extrwi	r3, r0, 1, 3
	blr

	.size	is_NaN__6BurgerFf,$-is_NaN__6BurgerFf
