/***************************************

	\brief Fetch a 64 bit unsigned reverse endian integer from memory with byte
		alignment.

	Assuming the data is unaligned, it will grab data a byte at a time
	and reconstruct it into a 64 bit value in reverse endian. The pointer does
	not have to be 64-bit aligned. uint8_t alignment is acceptable.

	\param pInput Pointer to a 64 bit value.

	\return The value with the bytes swapped.

	\note \ref nullptr pointers are illegal and will page fault.

	\sa _load_unaligned(const uint64_t*)

***************************************/

#include "brendian.h"

asm uint64_t BURGER_API Burger::_load_unaligned_swap(
	const uint64_t* pInput BURGER_68K_A0) BURGER_NOEXCEPT
{
	// Get the destination pointer
	movea.l	4(a7), a1

	// Copy the data
	move.b	7(a0), (a1)
	move.b	6(a0), 1(a1)
	move.b	5(a0), 2(a1)
	move.b	4(a0), 3(a1)
	move.b	3(a0), 4(a1)
	move.b	2(a0), 5(a1)
	move.b	1(a0), 6(a1)
	move.b	(a0), 7(a1)

#if defined(BURGER_CFM)
	rtd		#4
#else
	rts
#endif
}