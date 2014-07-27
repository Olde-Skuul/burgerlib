/***************************************

	Class for a string command parameter

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCOMMANDPARAMETERSTRING_H__
#define __BRCOMMANDPARAMETERSTRING_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRCOMMANDPARAMETER_H__
#include "brcommandparameter.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

/* BEGIN */
namespace Burger {
class CommandParameterString : public CommandParameter {
	String m_Value;		///< Parsed string
public:
	CommandParameterString(const char *pHelp,const char *const *ppParameterName,WordPtr uParameterCount,const char *pDefault=NULL) :
		CommandParameter(pHelp,ppParameterName,uParameterCount),
		m_Value(pDefault) {}
	CommandParameterString(const char *pHelp,const char *pParameterName,const char *pDefault=NULL) :
		CommandParameter(pHelp,reinterpret_cast<const char *const *>(pParameterName),0),
		m_Value(pDefault) {}
	virtual int Action(int argc,const char **argv);
	BURGER_INLINE const char *GetValue(void) const { return m_Value.GetPtr(); }
};
}
/* END */

#endif
