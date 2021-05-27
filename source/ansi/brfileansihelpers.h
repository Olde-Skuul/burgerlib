/***************************************

    File Manager ANSI helper functions

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEANSIHELPERS_H__
#define __BRFILEANSIHELPERS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef _INC_STDIO
#include <stdio.h>
#endif

/* BEGIN */
namespace Burger {
extern uint_t BURGER_API WriteBig(FILE* fp, uint16_t uInput);
extern uint_t BURGER_API WriteBig(FILE* fp, uint32_t uInput);
extern uint_t BURGER_API WriteBig(FILE* fp, uint64_t uInput);
extern uint_t BURGER_API WriteBig(FILE* fp, float fInput);
extern uint_t BURGER_API WriteBig(FILE* fp, double dInput);
extern uint_t BURGER_API WriteLittle(FILE* fp, uint16_t uInput);
extern uint_t BURGER_API WriteLittle(FILE* fp, uint32_t uInput);
extern uint_t BURGER_API WriteLittle(FILE* fp, uint64_t uInput);
extern uint_t BURGER_API WriteLittle(FILE* fp, float fInput);
extern uint_t BURGER_API WriteLittle(FILE* fp, double dInput);
extern uint_t BURGER_API WriteCString(FILE* fp, const char* pInput);
extern uint16_t BURGER_API ReadBigWord16(FILE* fp);
extern uint32_t BURGER_API ReadBigWord32(FILE* fp);
extern uint64_t BURGER_API ReadBigWord64(FILE* fp);
extern float BURGER_API ReadBigFloat(FILE* fp);
extern double BURGER_API ReadBigDouble(FILE* fp);
extern uint16_t BURGER_API ReadLittleWord16(FILE* fp);
extern uint32_t BURGER_API ReadLittleWord32(FILE* fp);
extern uint64_t BURGER_API ReadLittleWord64(FILE* fp);
extern float BURGER_API ReadLittleFloat(FILE* fp);
extern double BURGER_API ReadLittleDouble(FILE* fp);
extern uint_t BURGER_API ReadCString(FILE* fp, char* pInput, uintptr_t uLength);
extern uintptr_t BURGER_API GetSize(FILE* fp);
extern uint64_t BURGER_API GetSize64(FILE* fp);
extern uint_t BURGER_API SaveFile(FILE* fp, const void* pData, uintptr_t uLength);
extern void* BURGER_API LoadFile(FILE* fp, uintptr_t* uLength);
extern void BURGER_API SetBinaryMode(FILE* fp);
}
/* END */

#endif
