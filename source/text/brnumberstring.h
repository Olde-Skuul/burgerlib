/***************************************

	Number String Manager

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRNUMBERSTRING_H__
#define __BRNUMBERSTRING_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRINGFUNCTIONS_H__
#include "brstringfunctions.h"
#endif

/* BEGIN */
namespace Burger {
class NumberString {
private:
	char m_Data[32];		///< String buffer
public:
	NumberString() { m_Data[0] = 0; }
	NumberString(Word32 uInput);
	NumberString(Word32 uInput,Word uFormat);
	NumberString(Int32 iInput);
	NumberString(Int32 iInput,Word uFormat);
	NumberString(Word64 uInput);
	NumberString(Word64 uInput,Word uFormat);
	NumberString(Int64 iInput);
	NumberString(Int64 iInput,Word uFormat);
	NumberString(float fInput);
	NumberString(double dInput);
	NumberString & operator = (Word32 uInput);
	NumberString & operator = (Int32 iInput);
	NumberString & operator = (Word64 uInput);
	NumberString & operator = (Int64 iInput);
	NumberString & operator = (float fInput);
	NumberString & operator = (double dInput);
	BURGER_INLINE operator const char *() const { return m_Data; }
	BURGER_INLINE const char *GetPtr(void) const { return m_Data; }
	BURGER_INLINE char operator[](WordPtr uIndex) const { return m_Data[uIndex]; }
	BURGER_INLINE const char &operator[](WordPtr uIndex) { return m_Data[uIndex]; }
	BURGER_INLINE void Clear(void) { m_Data[0] = 0; }
	BURGER_INLINE Word IsEmpty(void) const { return !m_Data[0]; }
	BURGER_INLINE WordPtr GetLength(void) const { return StringLength(m_Data); }
};
}
/* END */

#endif

