;
; Espresso assembly for the WiiU
;
; Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; ePowerPCRounding BURGER_API Burger::get_PowerPC_rounding(void) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; Result in r3
;

	.align 2

	.text

	.global get_PowerPC_rounding__6BurgerFv
get_PowerPC_rounding__6BurgerFv:
	.type get_PowerPC_rounding__6BurgerFv, @function

; WiiU stack frame
	stwu	sp, -24(sp)

; Get the floating point control register
	mffs	f0

; Store it in memory
	stfd	f0, 8(sp)

; Load into an integer register
	lwz		r3, 12(sp)

; Mask off the lowest 2 bits (AND & 3)
	clrlwi	r3, r3, 30
	addi	sp, sp, 24
	blr

	.size	get_PowerPC_rounding__6BurgerFv,$-get_PowerPC_rounding__6BurgerFv