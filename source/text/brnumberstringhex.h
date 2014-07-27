/***************************************

	Number String Manager in hexadecimal

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRNUMBERSTRINGHEX_H__
#define __BRNUMBERSTRINGHEX_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRINGFUNCTIONS_H__
#include "brstringfunctions.h"
#endif

/* BEGIN */
namespace Burger {
class NumberStringHex {
private:
	char m_Data[32];		///< String buffer
public:
	NumberStringHex() { m_Data[0] = 0; }
	NumberStringHex(Word32 uInput);
	NumberStringHex(Word32 uInput,Word uFormat);
	NumberStringHex(Word64 uInput);
	NumberStringHex(Word64 uInput,Word uFormat);
	NumberStringHex(float fInput);
	NumberStringHex(double dInput);
	NumberStringHex & operator = (Word32 uInput);
	NumberStringHex & operator = (Word64 uInput);
	NumberStringHex & operator = (float fInput);
	NumberStringHex & operator = (double dInput);
	BURGER_INLINE operator const char *() const { return m_Data; }
	BURGER_INLINE const char *GetPtr(void) const { return m_Data; }
	BURGER_INLINE char operator[](WordPtr uIndex) const { return m_Data[uIndex]; }
	BURGER_INLINE const char &operator[](WordPtr uIndex) { return m_Data[uIndex]; }
	BURGER_INLINE void Clear(void) { m_Data[0] = 0; }
	BURGER_INLINE Word IsEmpty(void) const { return !m_Data[0]; }
	BURGER_INLINE WordPtr GetLength(void) const { return Burger::StringLength(m_Data); }
};
}
/* END */

#endif

