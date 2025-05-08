;
; Espresso assembly for the WiiU
;
; Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; uint32_t BURGER_API Burger::atomic_add(volatile uint32_t* pOutput,
;	uint32_t uInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; uInput = r3/r4
; Result in r3
;

	.align 2

	.text

	.global atomic_add__6BurgerFPVUiUi
atomic_add__6BurgerFPVUiUi:
	.type atomic_add__6BurgerFPVUiUi, @function

; Perform an atomic read of pOutput (r3)
retry:
	lwarx	r0,r0,r3

; Perform the add with uInput
	add		r0,r0,r4

; Make sure the cache is coherent
	dcbst	r0,r3

; Store result in the variable
	stwcx.	r0,r0,r3

; Did the store "take", that is, it was atomic?
	bne-	retry

; Return the final value
	mr		r3,r0
	blr

	.size	atomic_add__6BurgerFPVUiUi,$-atomic_add__6BurgerFPVUiUi
