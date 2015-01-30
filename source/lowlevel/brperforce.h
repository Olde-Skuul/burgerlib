/***************************************

	Class to add perforce integration to Windows tools

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	BURGER_DISABLECOPYCONSTRUCTORS(Perforce);
	Filename m_PerforceFilename;		///< Filename for p4 executable file
	Word m_bFilenameInitialized;		///< \ref TRUE if the perforce executable is found
public:
	Perforce();
	~Perforce();
	Word BURGER_API Init(void);
	Word BURGER_API Shutdown(void);
	Word BURGER_API Edit(const char *pFilename);
	Word BURGER_API RevertIfUnchanged(const char *pFilename);
};
}
#else
namespace Burger {
class Perforce {
	BURGER_DISABLECOPYCONSTRUCTORS(Perforce);
public:
	BURGER_INLINE Word Init(void) const { return 20; }
	BURGER_INLINE Word Shutdown(void) const { return 20; }
	BURGER_INLINE Word Edit(const char * /* pFilename */) const { return 20; }
	BURGER_INLINE Word RevertIfUnchanged(const char * /* pFilename */) const { return 20; }
};
}
#endif
/* END */
#endif
