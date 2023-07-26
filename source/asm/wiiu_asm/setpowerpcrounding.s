;
; Set the PowerPC floating point rounding value
;
; Espresso assembly for the WiiU
;
; Build with WiiU tool chain
;
; ePowerPCRounding BURGER_API Burger::SetPowerPCRounding(
;	ePowerPCRounding uInput) BURGER_NOEXCEPT;
;

	.align 2

	.text

	.global SetPowerPCRounding__6BurgerFQ2_6Burger16ePowerPCRounding
SetPowerPCRounding__6BurgerFQ2_6Burger16ePowerPCRounding:
	.type SetPowerPCRounding__6BurgerFQ2_6Burger16ePowerPCRounding, @function

; Get the floating point control register
	mffs	f0

; Store it in memory
	stfd	f0, -8(r1)    

; Load into an integer register
	lwz		r4, -4(r1)

; Get the return value, mask off the lowest 2 bits (AND & 3)
	clrlwi	r5, r4, 30

; Move the lowest 2 bits into the existing value
	insrwi	r4, r3, 2, 30

; Store it in memory
	stw		r4, -4(r1)

; Reload the fp register
	lfd		f0, -8(r1)

; Set the flags
	mtfsf	255, f0

; Return the previous value
	mr		r3, r5

; Exit
	blr

	.size	SetPowerPCRounding__6BurgerFQ2_6Burger16ePowerPCRounding,$-SetPowerPCRounding__6BurgerFQ2_6Burger16ePowerPCRounding
