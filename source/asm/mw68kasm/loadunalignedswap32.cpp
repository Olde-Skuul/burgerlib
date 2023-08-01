/***************************************

	\brief Fetch a 32 bit unsigned reverse endian integer from memory with byte
		alignment.

	Given a pointer to a 32 bit value, load it and swap the bytes
	so that 0x12345678 becomes 0x78563412. The pointer does not have
	to be 32-bit aligned. uint8_t alignment is acceptable.

	\param pInput Pointer to value to endian convert

	\return The value with the bytes swapped.

	\note \ref nullptr pointers are illegal and will page fault.

	\sa _load_unaligned(const uint32_t *)

***************************************/

#include "brendian.h"

asm uint32_t BURGER_API Burger::_load_unaligned_swap(
	const uint32_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
{
	// Get the highest byte first
	move.b	3(a0),d0

	// Shift it up and fetch byte 2
	lsl.l	#8,d0
	move.b	2(a0),d0

	// Shift it up and fetch byte 3
	lsl.l	#8,d0
	move.b	1(a0),d0

	// Shift it up and fetch byte 4
	lsl.l	#8,d0
	move.b	(a0),d0
	rts
}