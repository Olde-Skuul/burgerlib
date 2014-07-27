/***************************************

	File Manager ANSI helper functions

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
extern void BURGER_API WriteBig(FILE *fp,Word32 uInput);
extern void BURGER_API WriteBig(FILE *fp,Word16 uInput);
extern void BURGER_API WriteLittle(FILE *fp,Word32 uInput);
extern void BURGER_API WriteLittle(FILE *fp,Word16 uInput);
extern void BURGER_API WriteCString(FILE *fp,const char *pInput);
extern Word32 BURGER_API ReadBigWord32(FILE *fp);
extern Word16 BURGER_API ReadBigWord16(FILE *fp);
extern Word32 BURGER_API ReadLittleWord32(FILE *fp);
extern Word16 BURGER_API ReadLittleWord16(FILE *fp);
extern Word BURGER_API ReadCString(FILE *fp,char *pInput,WordPtr uLength);
extern WordPtr BURGER_API GetSize(FILE *fp);
extern Word BURGER_API SaveFile(FILE *fp,const void *pData,WordPtr uLength);
extern void * BURGER_API LoadFile(FILE *fp,WordPtr *uLength);
}
/* END */

#endif

