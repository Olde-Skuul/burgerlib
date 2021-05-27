/***************************************

    Dos extender functions

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDOSEXTENDER_H__
#define __BRDOSEXTENDER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_MSDOS) && !defined(__BRMSDOSTYPES_H__)
#include "brmsdostypes.h"
#endif

/* BEGIN */
#if defined(BURGER_MSDOS) || defined(DOXYGEN)

#ifdef __cplusplus
extern "C" {
#endif

// These exist in the X32 DOS extender, Burgerlib creates them
// for DOS4GW and others so code can assume these exist
// for all MSDOS targets.

extern void *_x32_zero_base_ptr;		///< Base Pointer to Real memory MSDOS Only
extern uint16_t _x32_zero_base_selector;	///< Segment to Real memory MSDOS Only

BURGER_INLINE uint8_t *ZeroBase(void) { return static_cast<uint8_t *>(_x32_zero_base_ptr); }

extern void BURGER_API CallInt10(uint_t EAX);
extern uint_t BURGER_API CallInt14(uint_t EAX,uint_t EDX);
extern uint_t BURGER_API CallInt17(uint_t EAX,uint_t EDX);
extern int32_t BURGER_API Int86x(uint32_t uInterrupt,const Burger::Regs16 *pInput,Burger::Regs16 *pOutput);
extern void BURGER_API SetBothInts(uint32_t uInterrupt,void far *pCode);
extern void BURGER_API SetProtInt(uint32_t uInterrupt,void far *pCode);
extern void BURGER_API SetRealInt(uint32_t uInterrupt,uint32_t pCode);
extern void far * BURGER_API GetProtInt(uint32_t uInterrupt);
extern uint32_t BURGER_API GetRealInt(uint32_t uInterrupt);
extern void * BURGER_API MapPhysicalAddress(void *pInput,uint32_t uLength);
extern int BURGER_API CallRealProcX32(uint32_t pAddress,const Burger::Regs16 *pInput,Burger::Regs16 *pOutput);
extern void * BURGER_API RealToProtectedPtr(uint32_t pReal);
extern void * BURGER_API GetRealBufferProtectedPtr(void);
extern uint32_t BURGER_API GetRealBufferPtr(void);
extern uint32_t BURGER_API AllocRealMemory(uint32_t uSize);
extern void BURGER_API DeallocRealMemory(uint32_t pReal);

#ifdef __cplusplus
}
#endif
#endif
/* END */

#endif
