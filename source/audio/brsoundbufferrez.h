/***************************************

    Sound manager buffer from resource

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSOUNDBUFFERREZ_H__
#define __BRSOUNDBUFFERREZ_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRREZFILE_H__
#include "brrezfile.h"
#endif

#ifndef __BRSOUND_H__
#include "brsound.h"
#endif

/* BEGIN */
namespace Burger {
class SoundBufferRez : public SoundManager::Buffer {
    BURGER_DISABLE_COPY(SoundBufferRez);
	BURGER_RTTI_IN_CLASS();
protected:
	RezFile *m_pRezFile;			///< Pointer to the resource class that the resource is attached to
	uint_t m_uRezNum;					///< Resource number of the sound file
	SoundBufferRez(void);
	SoundBufferRez(RezFile *pRezFile,uint_t uRezNum);
public:
    uint_t Upload(SoundManager *pSoundManager) BURGER_OVERRIDE;
	void BURGER_API Set(RezFile *pRezFile,uint_t uRezNum);
	static SoundBufferRez * BURGER_API new_object(void);
	static SoundBufferRez * BURGER_API new_object(RezFile *pRezFile,uint_t uRezNum);
};
}
/* END */

#endif
