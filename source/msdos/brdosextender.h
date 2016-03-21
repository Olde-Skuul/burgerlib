/***************************************

	Dos extender functions

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
extern void *_x32_zero_base_ptr;		///< Base Pointer to Real memory MSDOS Only
extern Word16 _x32_zero_base_selector;	///< Segment to Real memory MSDOS Only
BURGER_INLINE Word8 *ZeroBase(void) { return static_cast<Word8 *>(_x32_zero_base_ptr); }
extern void BURGER_API CallInt10(Word EAX);
extern Word BURGER_API CallInt14(Word EAX,Word EDX);
extern Word BURGER_API CallInt17(Word EAX,Word EDX);
extern Int32 BURGER_API Int86x(Word32 uInterrupt,const Burger::Regs16 *pInput,Burger::Regs16 *pOutput);
extern void BURGER_API SetBothInts(Word32 uInterrupt,void far *pCode);
extern void BURGER_API SetProtInt(Word32 uInterrupt,void far *pCode);
extern void BURGER_API SetRealInt(Word32 uInterrupt,Word32 pCode);
extern void far * BURGER_API GetProtInt(Word32 uInterrupt);
extern Word32 BURGER_API GetRealInt(Word32 uInterrupt);
extern void * BURGER_API MapPhysicalAddress(void *pInput,Word32 uLength);
extern int BURGER_API CallRealProcX32(Word32 pAddress,const Burger::Regs16 *pInput,Burger::Regs16 *pOutput);
extern void * BURGER_API RealToProtectedPtr(Word32 pReal);
extern void * BURGER_API GetRealBufferProtectedPtr(void);
extern Word32 BURGER_API GetRealBufferPtr(void);
extern Word32 BURGER_API AllocRealMemory(Word32 uSize);
extern void BURGER_API DeallocRealMemory(Word32 pReal);
#ifdef __cplusplus
}
#endif
#endif
/* END */

#endif
