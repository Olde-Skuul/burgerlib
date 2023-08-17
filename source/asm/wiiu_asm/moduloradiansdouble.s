;
; Espresso assembly for the WiiU
;
; Copyright 1995-2023 by Rebecca Ann Heineman becky@burgerbecky.com
;
; Build with WiiU tool chain
;
; double BURGER_API Burger::ModuloRadians(double dInput) BURGER_NOEXCEPT
; r0, r3-12, fp0-fp13 are volatile
; Result in fp1
; dInput = fp1
;

	.weak	g_dReciprocalPi2__6Burger
	.weak	g_fHalf__6Burger
	.weak	g_dNegPi2__6Burger

	.align 2

	.text

	.global ModuloRadians__6BurgerFd
ModuloRadians__6BurgerFd:
	.type ModuloRadians__6BurgerFd, @function

; Fetch the pointers
	lis		r3, %hiadj(g_dReciprocalPi2__6Burger)

; Create 0x4330000080000000 for integer to float conversions
	lis		r6, 0x4330
	lis		r4, %hiadj(g_fHalf__6Burger)
	lis		r7, 0x8000
	lis		r5, %hiadj(g_dNegPi2__6Burger)

; Load in 1/ 2Pi
	lfd		fp3, %lo(g_dReciprocalPi2__6Burger)(r3)

; Load in 0.5f
	lfs		fp4, %lo(g_fHalf__6Burger)(r4)

; (dInput*g_dReciprocalPi2)
	fmul	f3, f1, f3

; Load in -Pi2
	lfd		fp5, %lo(g_dNegPi2__6Burger)(r5)

; (dInput*g_dReciprocalPi2)+g_fHalf
	fadd	f3, f3, f4

; fmadd doesn't handle sign properly so it failed the unit test
; Used explicit fmul and fadd to get the accuracy
; (fInput*g_fReciprocalPi2)+g_fHalf
;	fmadd	fp3, fp1, fp3, fp4

; Store 0x4330000080000000
	stw		r6, -16(SP)
	stw		r7, -12(SP)

; Load 0x4330000080000000 in FPU
	lfd		fp2, -16(SP)

; Convert to integer
	fctiwz	fp0, fp3

; Store the integer in memory (64 bit)
	stfd	fp0, -8(SP)

; Extract the low word
	lwz		r0, -4(SP)

; Flip the bit
	xor		r0, r0, r7

; Create a fake double
	stw		r6, -8(SP)

; Store the integer
	stw		r0, -4(SP)

; Load the rounded double
	lfd		fp0, -8(SP)

; Complete the int to float conversion
	fsub	fp2, fp0, fp2

; Compare the two and get rid of the prerounded
	fcmpu	cr0, fp2, fp3
	ble		FModule1

; Load 1.0f (0.5 + 0.5)
	fadd	fp0, fp4, fp4

; Fixup
	fsub	fp2, fp2, fp0

; Don't use fnmsubs, it doesn't handle the sign bit properly
; (fVar*-g_fPi2) + fInput
FModule1:
	fmadd	fp1, fp2, fp5, fp1
	blr

	.size	ModuloRadians__6BurgerFd,$-ModuloRadians__6BurgerFd
