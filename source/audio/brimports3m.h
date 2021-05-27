/***************************************

    Import an S3M file

    Documentation is found here
    http://www.shikadi.net/moddingwiki/S3M_Format

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRIMPORTS3M_H__
#define __BRIMPORTS3M_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSEQUENCER_H__
#include "brsequencer.h"
#endif

/* BEGIN */
namespace Burger {
extern void BURGER_API ImportS3MEffect(Sequencer::Command_t *pOutput,uint_t uS3MCommand,uint_t uS3MArgument);
extern uint_t BURGER_API ImportS3M(Sequencer::SongPackage *pOutput,const uint8_t *pInput,uintptr_t uInputLength);
}
/* END */

#endif
