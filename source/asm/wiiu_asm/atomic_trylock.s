;
; PowerPC32 assembly for Metrowerks tool chain
;
; Copyright 1995-2025 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with Metrowerks Codewarrior 10 for MacOSX Carbon
;
; uint_t BURGER_API Burger::atomic_trylock(
;	volatile uint32_t* pInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; uInput = r3
; Result in r3
;

	.align 2

	.text

	.global atomic_trylock__6BurgerFPVUi
atomic_trylock__6BurgerFPVUi:
	.type atomic_trylock__6BurgerFPVUi, @function

; Perform an atomic read of pInput (r3)
retry:
	lwarx	r4,r0,r3

; Check if it is already locked
	cmpwi	r4,0
	
; In delay slot, load 1
	li		r0,1
	bne-	abort

; Make sure the cache is coherent
	dcbst	r0,r3

; Store 1 in the variable
	stwcx.	r0,r0,r3

; Did the store "take", that is, it was atomic?
	bne-	retry

; Return true if the lock was acquired
abort:
	cntlzw	r0,r4

; If zero, return 1, else 0 (32 = 1, all else = 0)
	srwi	r3,r0,5
	blr

	.size	atomic_trylock__6BurgerFPVUi,$-atomic_trylock__6BurgerFPVUi

