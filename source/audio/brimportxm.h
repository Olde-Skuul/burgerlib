/***************************************

    XM file importer

    Format is found at http://www.fileformat.info/format/xm/corion.htm

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRIMPORTXM_H__
#define __BRIMPORTXM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSEQUENCER_H__
#include "brsequencer.h"
#endif

/* BEGIN */
namespace Burger {
extern const uint8_t * BURGER_API ImportXMEffect(Sequencer::Command_t *pOutput,const uint8_t *pInput);
extern uint_t BURGER_API ImportXM(Sequencer::SongPackage *pOutput,const uint8_t *pInput,uintptr_t uInputLength);
}
/* END */

#endif
