/***************************************

	\brief Fetch a 32 bit unsigned value from memory with byte alignment.

	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 32 bit value in native endian.

	\param pInput Pointer to a 32 bit value.

	\return The 32 bit unsigned value.

	\sa _load_unaligned_swap(const uint32_t*)

***************************************/

#include "brendian.h"

asm uint32_t BURGER_API Burger::_load_unaligned(
	const uint32_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
{
	// Get the highest byte first
	move.b	(a0),d0

	// Shift it up and fetch byte 2
	lsl.l	#8,d0
	move.b	1(a0),d0

	// Shift it up and fetch byte 3
	lsl.l	#8,d0
	move.b	2(a0),d0

	// Shift it up and fetch byte 4
	lsl.l	#8,d0
	move.b	3(a0),d0
	rts
}