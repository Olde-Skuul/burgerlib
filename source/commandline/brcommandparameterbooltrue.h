/***************************************

    Class for a boolean command parameter

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRCOMMANDPARAMETERBOOLTRUE_H__
#define __BRCOMMANDPARAMETERBOOLTRUE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRCOMMANDPARAMETER_H__
#include "brcommandparameter.h"
#endif

/* BEGIN */
namespace Burger {
class CommandParameterBooleanTrue: public CommandParameter {
    uint_t m_bValue; ///< \ref TRUE if triggered, \ref FALSE if not
public:
    CommandParameterBooleanTrue(const char* pHelp,
        const char* const* ppParameterName,
        uintptr_t uParameterCount) BURGER_NOEXCEPT
        : CommandParameter(pHelp, ppParameterName, uParameterCount),
          m_bValue(FALSE)
    {
    }
    CommandParameterBooleanTrue(
        const char* pHelp, const char* pParameterName) BURGER_NOEXCEPT
        : CommandParameter(
              pHelp, reinterpret_cast<const char* const*>(pParameterName), 0),
          m_bValue(FALSE)
    {
    }
    int Action(int argc, const char** argv) BURGER_NOEXCEPT BURGER_OVERRIDE;
    BURGER_INLINE uint_t GetValue(void) const BURGER_NOEXCEPT
    {
        return m_bValue;
    }
};
}
/* END */

#endif
