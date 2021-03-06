/***************************************

    Class to add perforce integration to Windows tools

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPERFORCE_H__
#define __BRPERFORCE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFILENAME_H__
#include "brfilename.h"
#endif

/* BEGIN */
#if (defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
namespace Burger {
class Perforce {
    BURGER_DISABLE_COPY(Perforce);
	Filename m_PerforceFilename;		///< Filename for p4 executable file
	uint_t m_bFilenameInitialized;		///< \ref TRUE if the perforce executable is found
public:
	Perforce();
	~Perforce();
	uint_t BURGER_API Init(void);
	uint_t BURGER_API Shutdown(void);
	uint_t BURGER_API Edit(const char *pFilename);
	uint_t BURGER_API RevertIfUnchanged(const char *pFilename);
};
}
#else
namespace Burger {
class Perforce {
    BURGER_DISABLE_COPY(Perforce);
public:
	BURGER_INLINE Perforce() {}
	BURGER_INLINE uint_t Init(void) const { return 20; }
	BURGER_INLINE uint_t Shutdown(void) const { return 20; }
	BURGER_INLINE uint_t Edit(const char * /* pFilename */) const { return 20; }
	BURGER_INLINE uint_t RevertIfUnchanged(const char * /* pFilename */) const { return 20; }
};
}
#endif
/* END */
#endif
