/***************************************

	Base class for command parameters

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCOMMANDPARAMETER_H__
#define __BRCOMMANDPARAMETER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

/* BEGIN */
namespace Burger {
class CommandParameter : public Base {
	const char *m_pHelp;				///< Pointer to the optional help string
	const char *const *m_ppParameterNames;		///< Array of "C" strings of the -parameter (Minus the -)
	WordPtr m_uParameterCount;			///< Number of possible strings to check
protected:
	CommandParameter(const char *pHelp,const char * const *ppParameterNames,WordPtr uParameterCount) :
		m_pHelp(pHelp),
		m_ppParameterNames(ppParameterNames),
		m_uParameterCount(uParameterCount) {}
public:
	BURGER_INLINE Word IsHelpAvailable(void) const { return m_pHelp!=NULL; }
	BURGER_INLINE const char *GetHelp(void) const { return m_pHelp; }
	virtual int Action(int argc,const char **argv) = 0;
	static int BURGER_API RemoveParms(int argc,const char **argv,int iIndexToArgv,int iRemoveCount);
	static int Process(int argc,const char **argv,const CommandParameter **ppParms,WordPtr uParmCount,const char *pUsage=NULL,int iMin=0,int iMax=0);
};
}
/* END */

#endif
