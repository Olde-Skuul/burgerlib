;
; Espresso assembly for the WiiU
;
; Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; uint32_t BURGER_API Burger::atomic_set(volatile uint32_t* pOutput,
;	uint32_t uInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; uInput = r3/r4
; Result in r3
;

	.align 2

	.text

	.global atomic_set__6BurgerFPVUiUi
atomic_set__6BurgerFPVUiUi:
	.type atomic_set__6BurgerFPVUiUi, @function

; Perform an atomic read of pOutput (r3)
retry:
	lwarx	r0,r0,r3

; Make sure the cache is coherent
	dcbst	r0,r3

; Store uInput in the variable
	stwcx.	r4,r0,r3

; Did the store "take", that is, it was atomic?
	bne-	retry

; Return the old value
	mr		r3,r0
	blr

	.size	atomic_set__6BurgerFPVUiUi,$-atomic_set__6BurgerFPVUiUi
