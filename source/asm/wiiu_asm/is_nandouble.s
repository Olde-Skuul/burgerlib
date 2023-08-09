;
; Test if a double is a NaN
;
; Espresso assembly for the WiiU
;
; Build with WiiU tool chain
;
; uint_t BURGER_API Burger::is_NaN(double dInput) BURGER_NOEXCEPT
;

	.align 2

	.text

	.global is_NaN__6BurgerFd
is_NaN__6BurgerFd:
	.type is_NaN__6BurgerFd, @function

; Perform a NaN test
	fcmpu	cr0, f1, f1

; Move cr0 into r0
	mfcr	r0

; (Flags>>28)&1 Grab the "Unordered" flag
	extrwi	r3, r0, 1, 3

; Exit
	blr

	.size	is_NaN__6BurgerFd,$-is_NaN__6BurgerFd
