/***************************************

	\brief Fetch a 16 bit unsigned reverse endian integer from memory with byte
		alignment.

	Given a pointer to a 16 bit value, load it and swap the bytes
	so that 0x1234 becomes 0x3412. The pointer does not have
	to be 16-bit aligned. uint8_t alignment is acceptable.

	\param pInput Pointer to a value to endian convert

	\return The value with the bytes swapped.

	\note \ref nullptr pointers are illegal and will page fault.

	\sa _load_unaligned(const uint16_t *)

***************************************/

#include "brendian.h"

asm uint16_t BURGER_API Burger::_load_unaligned_swap(
	const uint16_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
{
	// A0 = pInput
	// Get the highest byte first
	moveq	#0, d0
	move.b	1(a0),d0

	// Shift it up
	lsl.w	#8,d0

	// Fetch the final byte
	move.b	(a0),d0
	rts
}
