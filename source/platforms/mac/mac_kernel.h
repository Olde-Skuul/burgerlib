/***************************************

	MacOS specific kernel functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MAC_KERNEL_H__
#define __MAC_KERNEL_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __MAC_TYPES_H__
#include "mac_types.h"
#endif

/* BEGIN */
namespace Burger { namespace MacOS {

enum ePowerMacType {
	/** Unknown machine */
	kUnknown = 0,
	/** Piltdown Man Power Mac */
	kPiltdownMan = 1,
	/** PCI slot Power Mac */
	kPCIMachine = 2,
	/** Communication slot Power Mac */
	kCommSlotMachine = 3,
	/** Communication slot 2 w/PCI Power Mac */
	kPCIComm2Machine = 4
};

extern uint_t BURGER_API is_trap_available(uint_t uTrapNum) BURGER_NOEXCEPT;
extern void BURGER_API kill_process(
	ProcessSerialNumber* pVictim) BURGER_NOEXCEPT;
extern void BURGER_API kill_other_processes(void) BURGER_NOEXCEPT;
extern ePowerMacType BURGER_API get_power_mac_type(void) BURGER_NOEXCEPT;

}}

/* END */

#endif
