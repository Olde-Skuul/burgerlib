;
; Espresso assembly for the WiiU
;
; Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; extern "C" uint64_t _swapendian64(uint64_t uInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; Result in r3/r4
; uInput = r3/r4
;

	.align 2

	.text

	.global _swapendian64
_swapendian64:
	.type _swapendian64, @function

; Perform the endian swap on both 32 bit registers

; 4,1,2,3 <- 1,2,3,4
	rlwinm	r0,r3,24,0,31
	rlwinm	r12,r4,24,0,31

; 4,3,2,3 <- 1,2,3,4 (Only copy bits 8-15)
	rlwimi	r0,r3,8,8,15
	rlwimi	r12,r4,8,8,15

; 4,3,2,1 <- 1,2,3,4 (Only copy bits 24-31)
	rlwimi	r0,r3,8,24,31
	rlwimi	r12,r4,8,24,31

; Return them, word swapped!
	mr		r3,r12
	mr		r4,r0
	blr

	.size	_swapendian64,$-_swapendian64
