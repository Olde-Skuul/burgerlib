/***************************************

	\brief Fetch a 16 bit unsigned value from memory with byte alignment.

	Assuming the data is unaligned, it will grab data a byte at a time and
	reconstruct it into a 16 bit value in native endian.

	\param pInput Pointer to a 16 bit value.

	\return The 16 bit unsigned value.

	\sa _load_unaligned_swap(const uint16_t*)

***************************************/

#include "brendian.h"

asm uint16_t BURGER_API Burger::_load_unaligned(
	const uint16_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
{
	// A0 = pInput
	// Get the highest byte first
	moveq	#0, d0
	move.b	(a0),d0

	// Shift it up
	lsl.w	#8,d0

	// Fetch the final byte
	move.b	1(a0),d0
	rts
}
