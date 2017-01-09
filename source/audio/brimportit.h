/***************************************

	IT file importer

	Format is found at http://schismtracker.org/wiki/ITTECH.TXT

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRIMPORTIT_H__
#define __BRIMPORTIT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSEQUENCER_H__
#include "brsequencer.h"
#endif

/* BEGIN */
namespace Burger {
extern void BURGER_API ImportITEffect(Sequencer::Command_t *pOutput,Word uITCommand,Word uITArgument);
extern Word BURGER_API ImportIT(Sequencer::SongPackage *pOutput,const Word8 *pInput,WordPtr uInputLength);
}
/* END */

#endif
