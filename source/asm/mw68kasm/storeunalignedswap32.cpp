/***************************************

	\brief Store a 32 bit unsigned value with endian swapping to memory with
		byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 32 bit value in reversed endian.

	\param pOutput Pointer to a 32 bit value.

	\param uInput The 32 bit unsigned value.

	\sa _store_unaligned(uint32_t*, uint32_t)

***************************************/

#include "brendian.h"

asm void BURGER_API Burger::_store_unaligned_swap(
	uint32_t* pOutput BURGER_68K_A0, uint32_t uInput BURGER_68K_D0) BURGER_NOEXCEPT
{
	// A0 = pOutput
	// D0 = uInput

	// Store the low byte
	move.b	d0, (a0)
	lsr.l	#8,d0

	// Next byte
	move.b	d0, 1(a0)
	lsr.l	#8,d0

	move.b	d0, 2(a0)
	lsr.l	#8,d0

	// Store the final byte
	move.b	d0, 3(a0)
	rts
}
