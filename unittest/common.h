/***************************************

	Unit tests for burgerlib

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_WINDOWS) || defined(BURGER_LINUX) || defined(BURGER_MACOSX)
#define ALLOWCOMMANDLINE
#endif

extern void BURGER_ANSIAPI ReportFailure(const char *pTemplate,Word uFailure,...);
extern void BURGER_ANSIAPI Message(const char *pMessage,...);
extern void BURGER_API BlastBuffer(void *pOutput,WordPtr uSize);
extern Word BURGER_API VerifyBuffer(const void *pBuffer,WordPtr uSize,const void *pInput,WordPtr uSkip);
extern int BURGER_ANSIAPI main(int argc,const char **argv);

#endif
