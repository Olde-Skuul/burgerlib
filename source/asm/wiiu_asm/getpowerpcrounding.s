;
; Get the PowerPC floating point rounding value
;
; Espresso assembly for the WiiU
;
; Build with WiiU tool chain
;
; ePowerPCRounding BURGER_API Burger::GetPowerPCRounding(void) BURGER_NOEXCEPT;
;

	.align 2

	.text

	.global GetPowerPCRounding__6BurgerFv
GetPowerPCRounding__6BurgerFv:
	.type GetPowerPCRounding__6BurgerFv, @function

; Get the floating point control register
	mffs	f0

; Store it in memory
	stfd	f0, -8(r1)

; Load into an integer register
	lwz		r3, -4(r1)

; Mask off the lowest 2 bits (AND & 3)
	clrlwi	r3, r3, 30
	
; Exit
	blr

	.size	GetPowerPCRounding__6BurgerFv,$-GetPowerPCRounding__6BurgerFv