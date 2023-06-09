/***************************************

	Memory functions for MSDos

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "msdos_memory.h"

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
#include "brdebug.h"
#include "msdos_structs.h"

#include <stdlib.h>

#if defined(BURGER_X32)
#include <X32.h>
#pragma library("x32.lib");
#endif

/*! ************************************

	\brief Convert a real mode address to a flat address

	Convert an encoded real mode address (16:16) (Segment:Offset) into a
	protected mode pointer. The protected pointer allows the 32 bit flat code to
	access the memory.

	\msdosonly

	\param pReal Real memory pointer (Segment/Offset)
	\return Protected mode pointer that points to the same memory

	\sa get_temp_protected_buffer() or get_temp_real_buffer()

***************************************/

void* BURGER_API Burger::MSDos::real_to_protected(
	uint32_t pReal) BURGER_NOEXCEPT
{
	// Get the flattened address
	uint32_t uFlattened =
		((pReal >> 12) & 0xFFFFFFF0U) + static_cast<uint16_t>(pReal);

#if defined(BURGER_X32)
	return zero_base() + uFlattened;
#else
	// Dos4GW uses the address as is
	return reinterpret_cast<void*>(uFlattened);
#endif
}

/*! ************************************

	\brief Allocate a chunk of real mode memory.

	Memory is extremely limited in real mode, keep allocations to a minimum and
	in small sizes

	\msdosonly

	\param uSize Number of bytes to allocate
	\return Segment:Offset of real mode memory or 0 on failure
	\sa free_real(uint32_t)

***************************************/

uint32_t BURGER_API Burger::MSDos::alloc_real(uint32_t uSize) BURGER_NOEXCEPT
{
	// X32 version
#if defined(BURGER_X32)
	// Call X32 to allocate memory
	return _x32_real_alloc(uSize);
#else

	// DPMI version

	Regs16_t Regs;

	// Round to the nearest 16 bytes
	uSize = uSize + 15;

	// Number of paragraphs to allocate
	uSize = uSize >> 4;

	// DOS allocate memory command
	Regs.ax = 0x4800;

	// Number of paragraphs to allocate
	Regs.bx = static_cast<uint16_t>(uSize);

	// Allocate
	real_mode_interrupt(0x21, &Regs, &Regs);

	// Error??
	if (Regs.flags & 1U) {
#if defined(_DEBUG)
		Debug::Warning("Can't allocate real memory\n");
#endif
		return 0;
	}

	// Get the segment
	uSize = Regs.ax;

	// Return as a real mode pointer with offset 0
	return uSize << 16U;
#endif
}

/*! ************************************

	\brief Release real mode memory back to DOS

	\msdosonly

	\param pReal Segment:Offset of allocated memory. Zero performs no action
	\sa alloc_real(uint32_t)

***************************************/

void BURGER_API Burger::MSDos::free_real(uint32_t pReal) BURGER_NOEXCEPT
{
	// Only both if there's an actual pointer
	if (pReal) {

		// X32 version
#if defined(BURGER_X32)
		// Call X32 to release memory
		_x32_real_free(pReal);
#else

		Regs16_t Regs;

		// Isolate the segment
		pReal = pReal >> 16U;

		// DOS release memory command
		Regs.ax = 0x4900;

		// Pass the segment in
		Regs.es = static_cast<uint16_t>(pReal);

		// Release the memory
		real_mode_interrupt(0x21, &Regs, &Regs);

		// Serious error, so log it
#if defined(_DEBUG)
		if (Regs.flags & 1) {
			Burger::Debug::Warning("Can't release real memory\n");
		}
#endif

#endif
	}
}

/*! ************************************

	\brief Return pointer to shared real buffer

	Allocate an 8K buffer in real memory for use in DOS calls.
	Return the pointer in SEGMENT:OFFSET format

	\msdosonly

	\return 0 if buffer allocation failed, or SEGMENT:OFFSET of the buffer
	\sa get_temp_protected_buffer()

***************************************/

#if !defined(DOXYGEN)

// Cached pointer to real memory
static uint32_t g_pRealBuffer;

static void ReleaseBuff(void)
{
	// Release the memory
	Burger::MSDos::free_real(g_pRealBuffer);
	g_pRealBuffer = 0;
}
#endif

uint32_t BURGER_API Burger::MSDos::get_temp_real_buffer(void) BURGER_NOEXCEPT
{
	// Is the buffer already allocated?
	uint32_t pReal = g_pRealBuffer;
	if (!pReal) {

		// Get some REAL memory
		pReal = alloc_real(8192);

		if (pReal) {
			// Save in global
			g_pRealBuffer = pReal;
			// Allow release on exit
			atexit(ReleaseBuff);
		}
	}

	// Return value
	return pReal;
}

/*! ************************************

	\brief Get the pointer to the shared protected memory buffer

	Allocate an 8K buffer in real memory for use in DOS calls.
	Return the pointer in protected memory.

	If the memory allocation fails, the function will return \ref nullptr

	\msdosonly

	\return Protected mode pointer to the 8K shared real mode buffer
	\sa get_temp_real_buffer() and real_to_protected()

***************************************/

void* BURGER_API Burger::MSDos::get_temp_protected_buffer(void) BURGER_NOEXCEPT
{
	// Get the buffer pointer
	uint32_t pReal = get_temp_real_buffer();

	// Did it allocate?
	if (pReal) {
		// Convert to real pointer
		return real_to_protected(pReal);
	}
	// Error!
	return nullptr;
}

#endif
