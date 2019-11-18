/***************************************

    Run Time Type Information class

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE
    for license details. Yes, you can use it in a
    commercial title without paying anything, just give me a credit.
    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTATICRTTI_H__
#define __BRSTATICRTTI_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
struct StaticRTTI {
    const char* m_pClassName;    ///< Pointer to the name of the class
    const StaticRTTI* m_pParent; ///< Pointer to the parent in a derived class
    BURGER_INLINE const char* GetClassName(void) const BURGER_NOEXCEPT
    {
        return m_pClassName;
    }
    uint_t BURGER_API IsInList(const StaticRTTI* pInput) const BURGER_NOEXCEPT;
};
}

#define BURGER_RTTI_IN_CLASS() \
public: \
    const Burger::StaticRTTI* GetStaticRTTI(void) \
        const BURGER_NOEXCEPT BURGER_OVERRIDE; \
    static const Burger::StaticRTTI g_StaticRTTI

#define BURGER_RTTI_IN_BASE_CLASS() \
public: \
    BURGER_INLINE const char* GetClassName(void) const BURGER_NOEXCEPT \
    { \
        return GetStaticRTTI()->GetClassName(); \
    } \
    virtual const Burger::StaticRTTI* GetStaticRTTI(void) \
        const BURGER_NOEXCEPT; \
    static const Burger::StaticRTTI g_StaticRTTI

#define BURGER_CREATE_STATICRTTI_BASE(__ClassName) \
    const Burger::StaticRTTI* __ClassName::GetStaticRTTI(void) \
        const BURGER_NOEXCEPT \
    { \
        return &g_StaticRTTI; \
    } \
    const Burger::StaticRTTI __ClassName::g_StaticRTTI = {#__ClassName, nullptr}

#define BURGER_CREATE_STATICRTTI_PARENT(__ClassName, __ParentClass) \
    const Burger::StaticRTTI* __ClassName ::GetStaticRTTI(void) \
        const BURGER_NOEXCEPT \
    { \
        return &g_StaticRTTI; \
    } \
    const Burger::StaticRTTI __ClassName::g_StaticRTTI = { \
        #__ClassName, &__ParentClass::g_StaticRTTI}

#define BURGER_STATICRTTI_ISTYPE(__ClassName, __Pointer) \
    (__Pointer)->GetStaticRTTI()->IsInList(&__ClassName::g_StaticRTTI)

#define BURGER_RTTICAST(__ClassName, __Pointer) \
    ((__Pointer) && \
                ((__Pointer)->GetStaticRTTI()->IsInList( \
                    &__ClassName::g_StaticRTTI)) ? \
            static_cast<__ClassName*>(__Pointer) : \
            nullptr)

#define BURGER_RTTICONSTCAST(__ClassName, __Pointer) \
    ((__Pointer) && \
                ((__Pointer)->GetStaticRTTI()->IsInList( \
                    &__ClassName::g_StaticRTTI)) ? \
            static_cast<const __ClassName*>(__Pointer) : \
            nullptr)

/* END */

#endif
