/***************************************

	Class to add perforce integration to Windows tools

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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

/* BEGIN */
#if (defined(BURGER_WINDOWS) && defined(BURGER_MSVC)) || defined(DOXYGEN)
class ClientApi;
namespace Burger {
class Perforce {
	BURGER_DISABLECOPYCONSTRUCTORS(Perforce);
	ClientApi *m_pClientApi;				///< Pointer to the open perforce reference (Windows only)
public:
	Perforce() : m_pClientApi(NULL) {}
	~Perforce();
	int Init(void);
	int Shutdown(void);
	int Edit(const char *pFilename);
	int RevertIfUnchanged(const char *pFilename);
};
}
#else
namespace Burger {
class Perforce {
	BURGER_DISABLECOPYCONSTRUCTORS(Perforce);
public:
	Perforce() {}
	BURGER_INLINE int Init(void) const { return 20; }
	BURGER_INLINE int Shutdown(void) const { return 20; }
	BURGER_INLINE int Edit(const char * /* pFilename */) const { return 20; }
	BURGER_INLINE int RevertIfUnchanged(const char * /* pFilename */) const { return 20; }
};
}
#endif
/* END */
#endif
