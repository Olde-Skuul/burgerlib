/***************************************

	Simple structures for MSDOS

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MSDOS_STRUCTS_H__
#define __MSDOS_STRUCTS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
extern "C" void* _x32_zero_base_ptr;
extern "C" uint16_t _x32_zero_base_selector;

namespace Burger { namespace MSDos {

// Used by 16 bit intel code

struct Regs16_t {
	/** 80x86 ax register */
	uint16_t ax;

	/** 80x86 bx register */
	uint16_t bx;

	/** 80x86 cx register */
	uint16_t cx;

	/** 80x86 dx register */
	uint16_t dx;

	/** 80x86 si register */
	uint16_t si;

	/** 80x86 di register */
	uint16_t di;

	/** 80x86 bp register */
	uint16_t bp;

	/** 80x86 ds segment register */
	uint16_t ds;

	/** 80x86 es segment register */
	uint16_t es;

	/** 80x86 flags register */
	uint16_t flags;
};

BURGER_INLINE uint8_t* zero_base(void) BURGER_NOEXCEPT
{
	return static_cast<uint8_t*>(_x32_zero_base_ptr);
}

extern uint32_t BURGER_API set_video_mode_int10h(
	uint32_t uMode) BURGER_NOEXCEPT;
extern uint32_t BURGER_API init_serial_port_int14h(
	uint32_t uPortFlags, uint32_t uPortNumber) BURGER_NOEXCEPT;
extern uint32_t BURGER_API printer_port_int17h(
	uint32_t uPrinterCommand, uint32_t uPrinterNumber = 0) BURGER_NOEXCEPT;
extern int32_t BURGER_API real_mode_interrupt(uint32_t uInterrupt,
	const Regs16_t* pInput, Regs16_t* pOutput) BURGER_NOEXCEPT;

#if defined(BURGER_X32) || defined(DOXYGEN)
extern int BURGER_API x32_call_real_proc(uint32_t pAddress,
	const Regs16_t* pInput, Regs16_t* pOutput) BURGER_NOEXCEPT;
#endif
}}

/* END */

#endif
