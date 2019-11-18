/***************************************

    Base class for command parameters

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
class CommandParameter: public Base {
    BURGER_RTTI_IN_CLASS();

protected:
    /** Pointer to the optional help string */
    const char* m_pHelp;

    /** Array of "C" strings of the -parameter (Minus the -) */
    const char* const* m_ppParameterNames;

    /** Number of possible strings to check */
    uintptr_t m_uParameterCount;

    CommandParameter(const char* pHelp, const char* const* ppParameterNames,
        uintptr_t uParameterCount) BURGER_NOEXCEPT
        : m_pHelp(pHelp),
          m_ppParameterNames(ppParameterNames),
          m_uParameterCount(uParameterCount)
    {
    }

public:
    BURGER_INLINE uint_t IsHelpAvailable(void) const BURGER_NOEXCEPT
    {
        return m_pHelp != nullptr;
    }
    BURGER_INLINE const char* GetHelp(void) const BURGER_NOEXCEPT
    {
        return m_pHelp;
    }
    virtual int Action(int argc, const char** argv) = 0;
    static int BURGER_API RemoveParms(
        int argc, const char** argv, int iIndexToArgv, int iRemoveCount) BURGER_NOEXCEPT;
    static int BURGER_API Process(int argc, const char** argv,
        const CommandParameter** ppParms, uintptr_t uParmCount,
        const char* pUsage = nullptr, int iMin = 0, int iMax = 0) BURGER_NOEXCEPT;
};
}
/* END */

#endif
