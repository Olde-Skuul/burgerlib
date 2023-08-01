;
; Load a 64 bit integer with unaligned operations
;
; Espresso assembly for the WiiU
;
; Build with WiiU tool chain
;
; uint64_t BURGER_API Burger::_load_unaligned(const uint64_t* pInput)
;

	.align 2

	.text

	.global _load_unaligned__6BurgerFPCUL
_load_unaligned__6BurgerFPCUL:
	.type _load_unaligned__6BurgerFPCUL, @function

; Load the lower 32 bits
	lbz		r7,0(r3)
	lbz		r6,1(r3)
	lbz		r5,2(r3)
	lbz		r8,3(r3)

; r8 has x,x,2,3
	rlwimi	r8,r5,8,16,23
; r6 has x,x,0,1
	rlwimi	r6,r7,8,16,23
; r8 has 0,1,2,3
	rlwimi	r8,r6,16,0,15

; Load the upper 32 bits
	lbz		r7,4(r3)
	lbz		r6,5(r3)
	lbz		r5,6(r3)
	lbz		r4,7(r3)

; r4 has x,x,2,3
	rlwimi	r4,r5,8,16,23
; r6 has x,x,0,1
	rlwimi	r6,r7,8,16,23
	mr		r3,r8

; r4 has 0,1,2,3
	rlwimi	r4,r6,16,0,15

; Return the value in r3,r4
	blr

	.size	_load_unaligned__6BurgerFPCUL,$-_load_unaligned__6BurgerFPCUL
