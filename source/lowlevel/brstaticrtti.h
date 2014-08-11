/***************************************

	Run Time Type Information class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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
class StaticRTTI {
	protected:
		const char* m_pClassName;		///< Pointer to the name of the class
		const StaticRTTI* m_pParent;	///< Pointer to the parent in a derived class
	public:
		StaticRTTI(const char *pClassName,const StaticRTTI *pParent);
		BURGER_INLINE const char* GetClassName(void) const { return m_pClassName; }
		Word BURGER_API IsInList(const StaticRTTI *pInput) const;
	};
}
#define BURGER_RTTI_IN_CLASS() \
	public: \
	BURGER_INLINE const char* GetClassName(void) const { return GetStaticRTTI()->GetClassName(); } \
	virtual const Burger::StaticRTTI* GetStaticRTTI(void) const; \
	static const Burger::StaticRTTI m_StaticRTTI

#define BURGER_CREATE_STATICRTTI_BASE(__ClassName) \
	const Burger::StaticRTTI* __ClassName::GetStaticRTTI(void) const { return &m_StaticRTTI; } \
	const Burger::StaticRTTI __ClassName::m_StaticRTTI(#__ClassName,NULL)

#define BURGER_CREATE_STATICRTTI_PARENT(__ClassName,__ParentClass) \
	const Burger::StaticRTTI * __ClassName ::GetStaticRTTI(void) const { return &m_StaticRTTI; } \
	const Burger::StaticRTTI __ClassName::m_StaticRTTI(#__ClassName,&__ParentClass::m_StaticRTTI)

#define BURGER_STATICRTTI_ISTYPE(__ClassName,__Pointer) (__Pointer)->GetStaticRTTI()->IsInList(&__ClassName::m_StaticRTTI)
#define BURGER_RTTICAST(__ClassName,__Pointer) ((__Pointer)->GetStaticRTTI()->IsInList(&__ClassName::m_StaticRTTI) ? static_cast<__ClassName*>(__Pointer) : NULL)
#define BURGER_RTTICONSTCAST(__ClassName,__Pointer) ((__Pointer)->GetStaticRTTI()->IsInList(&__ClassName::m_StaticRTTI) ? static_cast<const __ClassName*>(__Pointer) : NULL)

/* END */

#endif
