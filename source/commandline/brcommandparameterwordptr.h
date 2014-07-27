/***************************************

	Class for an integer number command parameter

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCOMMANDPARAMETERWORDPTR_H__
#define __BRCOMMANDPARAMETERWORDPTR_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRCOMMANDPARAMETER_H__
#include "brcommandparameter.h"
#endif

/* BEGIN */
namespace Burger {
class CommandParameterWordPtr : public CommandParameter {
	WordPtr m_uValue;	///< Parsed value
	WordPtr m_uMin;		///< Lowest acceptable value
	WordPtr m_uMax;		///< Highest acceptable value
public:
	CommandParameterWordPtr(const char *pHelp,const char **ppParameterName,WordPtr uParameterCount,WordPtr uDefault,WordPtr uMin=0,WordPtr uMax=BURGER_MAXWORDPTR) :
		CommandParameter(pHelp,ppParameterName,uParameterCount),
		m_uValue(uDefault),
		m_uMin(uMin),
		m_uMax(uMax) {}
	CommandParameterWordPtr(const char *pHelp,const char *pParameterName,WordPtr uDefault,WordPtr uMin=0,WordPtr uMax=BURGER_MAXWORDPTR) :
		CommandParameter(pHelp,reinterpret_cast<const char * const *>(pParameterName),0),
		m_uValue(uDefault),
		m_uMin(uMin),
		m_uMax(uMax) {}
	virtual int Action(int argc,const char **argv);
	BURGER_INLINE WordPtr GetValue(void) const { return m_uValue; }
};
}
/* END */

#endif
