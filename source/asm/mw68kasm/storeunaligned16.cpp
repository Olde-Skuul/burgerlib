/***************************************

	\brief Store a 16 bit unsigned value to memory with byte alignment.

	Assuming the output pointer is unaligned, it will store data a byte at a
	time into a 16 bit value in native endian.

	\param pOutput Pointer to a 16 bit value.

	\param uInput The 16 bit unsigned value.

	\sa _store_unaligned_swap(uint16_t*,uint16_t)

***************************************/

#include "brendian.h"

asm void BURGER_API Burger::_store_unaligned(
	uint16_t* pOutput BURGER_68K_A0, uint16_t uInput BURGER_68K_D0) BURGER_NOEXCEPT
{
	// A0 = pOutput
	// D0 = uInput

	// Store the high byte
	move.b	d0, 1(a0)

	// Shift it down
	lsr.w	#8,d0

	// Store the final byte
	move.b	d0, (a0)
	rts
}
