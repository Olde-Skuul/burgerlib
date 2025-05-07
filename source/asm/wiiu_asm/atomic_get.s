;
; PowerPC32 assembly for Metrowerks tool chain
;
; Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with Metrowerks Codewarrior 10 for MacOSX Carbon
;
; uint32_t BURGER_API Burger::atomic_get(
;	volatile uint32_t* pInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; uInput = r3
; Result in r3
;

	.align 2

	.text

	.global atomic_get__6BurgerFPVUi
atomic_get__6BurgerFPVUi:
	.type atomic_get__6BurgerFPVUi, @function

; Perform an atomic read of pInput (r3)
retry:
	lwarx	r0,r0,r3

; Make sure the cache is coherent
	dcbst	r0,r3

; Store it back into the variable
	stwcx.	r0,r0,r3

; Did the store "take", that is, it was atomic?
	bne-	retry

; Return the loaded value
	mr		r3,r0
	blr

	.size	atomic_get__6BurgerFPVUi,$-atomic_get__6BurgerFPVUi
