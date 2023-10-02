;
; Espresso assembly for the WiiU
;
; Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; ePowerPCRounding BURGER_API Burger::set_PowerPC_rounding(ePowerPCRounding uInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; Result in r3
; uInput = r3
;

	.align 2

	.text

	.global set_PowerPC_rounding__6BurgerFQ2_6Burger16ePowerPCRounding
set_PowerPC_rounding__6BurgerFQ2_6Burger16ePowerPCRounding:
	.type set_PowerPC_rounding__6BurgerFQ2_6Burger16ePowerPCRounding, @function

; WiiU stack frame
	stwu	sp, -24(sp)

; Get the floating point control register
	mffs	f0

; Store it in memory
	stfd	f0, 8(sp)

; Load into an integer register
	lwz		r4, 12(sp)

; Get the return value, mask off the lowest 2 bits (AND & 3)
	clrlwi	r5, r4, 30

; Move the lowest 2 bits into the existing value
	insrwi	r4, r3, 2, 30

; Store it in memory
	stw		r4, 12(sp)

; Reload the fp register
	lfd		f0, 8(sp)

; Set the flags
	mtfsf	255, f0

; Return the previous value
	mr		r3, r5
	addi	sp, sp, 24
	blr

	.size	set_PowerPC_rounding__6BurgerFQ2_6Burger16ePowerPCRounding,$-set_PowerPC_rounding__6BurgerFQ2_6Burger16ePowerPCRounding
