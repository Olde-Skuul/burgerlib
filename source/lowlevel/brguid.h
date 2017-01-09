/***************************************

	Create and work with Windows style GUID structures

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGUID_H__
#define __BRGUID_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
#ifdef __cplusplus
extern "C" {
#endif
#if !defined(GUID_DEFINED) || defined(DOXYGEN)
#if !defined(DOXYGEN)
#define GUID_DEFINED		// Define used by the windows headers
#endif
typedef struct _GUID {
	Word32 Data1;			///< Initial 32 bits as a little endian Word32
	Word16 Data2;			///< Next 16 bits as a little endian Word16
	Word16 Data3;			///< Third value as a 16 bit little endian Word16
	Word8 Data4[8];			///< Remaining 64 bits as 8 bytes
} GUID;
#endif
#ifdef __cplusplus
}
#endif
namespace Burger {
extern const GUID g_GUIDZero;		///< Empty GUID
extern void BURGER_API GUIDInit(GUID *pOutput);
extern void BURGER_API GUIDToString(char *pOutput,const GUID *pInput);
extern Word BURGER_API GUIDFromString(GUID *pOutput,const char *pInput);
extern Word BURGER_API GUIDHash(const GUID *pInput);
extern Word BURGER_API GUIDIsEqual(const GUID *pInput1,const GUID *pInput2);
extern int BURGER_API GUIDCompare(const GUID *pInput1,const GUID *pInput2);
}
/* END */

#endif
