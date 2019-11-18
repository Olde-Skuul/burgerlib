/***************************************

    Class for an integer number command parameter

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
class CommandParameterWordPtr: public CommandParameter {
    uintptr_t m_uValue; ///< Parsed value
    uintptr_t m_uMin;   ///< Lowest acceptable value
    uintptr_t m_uMax;   ///< Highest acceptable value
public:
    CommandParameterWordPtr(const char* pHelp, const char** ppParameterName,
        uintptr_t uParameterCount, uintptr_t uDefault, uintptr_t uMin = 0,
        uintptr_t uMax = UINTPTR_MAX) BURGER_NOEXCEPT
        : CommandParameter(pHelp, ppParameterName, uParameterCount),
          m_uValue(uDefault),
          m_uMin(uMin),
          m_uMax(uMax)
    {
    }
    CommandParameterWordPtr(const char* pHelp, const char* pParameterName,
        uintptr_t uDefault, uintptr_t uMin = 0,
        uintptr_t uMax = UINTPTR_MAX) BURGER_NOEXCEPT
        : CommandParameter(
              pHelp, reinterpret_cast<const char* const*>(pParameterName), 0),
          m_uValue(uDefault),
          m_uMin(uMin),
          m_uMax(uMax)
    {
    }
    int Action(int argc, const char** argv) BURGER_OVERRIDE;
    BURGER_INLINE uintptr_t GetValue(void) const BURGER_NOEXCEPT
    {
        return m_uValue;
    }
};
}
/* END */

#endif
