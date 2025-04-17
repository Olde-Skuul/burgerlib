/***************************************

	Simple String Manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSIMPLESTRING_H__
#define __BRSIMPLESTRING_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRINGFUNCTIONS_H__
#include "brstringfunctions.h"
#endif

/* BEGIN */
namespace Burger {
class SimpleString {
private:
	const char* m_pData; ///< Pointer to the actual string, never is \ref NULL
public:
	SimpleString(void): m_pData(Burger::g_EmptyString) {}
	SimpleString(char cInput);
	SimpleString(const char* pInput1);
	SimpleString(const char* pInput1, const char* pInput2);
	SimpleString(const char* pInput1, const char* pInput2, const char* pInput3);
	SimpleString(const char* pInput1, const char* pInput2, const char* pInput3,
		const char* pInput4);
	SimpleString(const char* pInput1, const char* pInput2, const char* pInput3,
		const char* pInput4, const char* pInput5);
	SimpleString(const Burger::SimpleString& rInput);
	~SimpleString();
	Burger::SimpleString& operator=(char cInput);
	Burger::SimpleString& operator=(const char* pInput);
	Burger::SimpleString& operator=(const Burger::SimpleString& rInput);
	BURGER_INLINE operator const char*() const
	{
		return m_pData;
	}
	BURGER_INLINE const char* GetPtr(void) const
	{
		return m_pData;
	}
	BURGER_INLINE char operator[](uintptr_t uIndex) const
	{
		return m_pData[uIndex];
	}
	BURGER_INLINE const char& operator[](uintptr_t uIndex)
	{
		return m_pData[uIndex];
	}
	void BURGER_API Clear(void);
	BURGER_INLINE uint_t IsEmpty(void) const
	{
		return !m_pData[0];
	}
	BURGER_INLINE uintptr_t GetLength(void) const
	{
		return Burger::string_length(m_pData);
	}
};
}

/* END */

#endif
