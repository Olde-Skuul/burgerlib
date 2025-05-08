;
; Espresso assembly for the WiiU
;
; Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; uint_t atomic_compare_and_set(volatile uint32_t* pOutput, uint32_t uOld,
;	uint32_t uNew) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; uInput = r3/r4/r5
; Result in r3
;

	.align 2

	.text

	.global atomic_compare_and_set__6BurgerFPVUiUiT2
atomic_compare_and_set__6BurgerFPVUiUiT2:
	.type atomic_compare_and_set__6BurgerFPVUiUiT2, @function

; Perform an atomic read of pOutput (r3)
retry:
	lwarx	r0,r0,r3

; Compare with uOld
	cmplw	r0,r4

; Exit if the value is not what was expected
	bne-	abort

; Make sure the cache is coherent
	dcbst	r0,r3

; Store uNew in the variable
	stwcx.	r5,r0,r3

; Did the store "take", that is, it was atomic?
	bne-	retry

abort:
; Create uTemp == uOld

; Zero if equal
	subf		r0,r0,r4

; 32 if zero, less than 32 if non zero
	cntlzw		r0,r0

; Use the high bit, so 1 for zero, 0 for non zero
	srwi		r3,r0,5
	blr

	.size	atomic_compare_and_set__6BurgerFPVUiUiT2,$-atomic_compare_and_set__6BurgerFPVUiUiT2
