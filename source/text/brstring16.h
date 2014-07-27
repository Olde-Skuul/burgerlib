/***************************************

	C++ ANSI "C" string class for UTF16
	This is optimized for performance

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSTRING16_H__
#define __BRSTRING16_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGLOBALMEMORYMANAGER_H__
#include "brglobalmemorymanager.h"
#endif

#ifndef __BRSTRINGFUNCTIONS_H__
#include "brstringfunctions.h"
#endif

/* BEGIN */
namespace Burger {
class String16 {
public:
	static const Word BUFFERSIZE = static_cast<Word>((128-(sizeof(Word16 *)+sizeof(WordPtr)))/sizeof(Word16));	///< Ensures the structure is 128 bytes in size on all platforms
private:
	Word16 *m_pData;		///< Pointer to the string
	WordPtr m_uLength;		///< Length of the string
	Word16 m_Raw[BUFFERSIZE];	///< Temp preallocated buffer for most strings
							// Structure size is 128 bytes
public:
	String16(void) { m_pData = m_Raw;m_uLength=0;m_Raw[0]=0;}
	String16(const String16 &rInput);
	String16(const String16 &rInput,WordPtr uStart,WordPtr uEnd = static_cast<WordPtr>(-1));
	String16(const char *pInput);
	String16(const char *pInput,WordPtr uPadding);
	String16(const Word16 *pInput);
	~String16() { if (m_pData != m_Raw) Burger::Free(m_pData); }

	BURGER_INLINE operator Word16 *() { return m_pData; }
	BURGER_INLINE operator const Word16 *() const { return m_pData; }
	BURGER_INLINE const Word16 *c_str() const { return m_pData; }
	BURGER_INLINE Word16 *c_str() { return m_pData; }
	BURGER_INLINE operator Word16 () const { return m_pData[0]; }
	BURGER_INLINE WordPtr length(void) const { return m_uLength; }
	BURGER_INLINE WordPtr GetLength(void) const { return m_uLength; }
	BURGER_INLINE Word16 *GetPtr() { return m_pData; }
	BURGER_INLINE const Word16 *GetPtr() const { return m_pData; }
	BURGER_INLINE String16 operator () (WordPtr uStart,WordPtr uEnd) const {return String16(*this,uStart,uEnd);}
	BURGER_INLINE Word16 & operator () (WordPtr uInput) { if (uInput>=m_uLength) { m_Raw[BUFFERSIZE-1] = 0; return m_Raw[BUFFERSIZE-1]; } return m_pData[uInput];}
	BURGER_INLINE Word16 const & operator ()(WordPtr uInput) const { if (uInput>=m_uLength) { return g_EmptyString16[0]; } return m_pData[uInput];}
	BURGER_INLINE Word16 & operator [] (WordPtr uInput) { if (uInput>=m_uLength) { m_Raw[BUFFERSIZE-1] = 0; return m_Raw[BUFFERSIZE-1]; } return m_pData[uInput];}
	BURGER_INLINE Word16 const & operator [](WordPtr uInput) const { if (uInput>=m_uLength) { return g_EmptyString16[0]; } return m_pData[uInput];}
	BURGER_INLINE Word operator ! () const { return (m_uLength==0); }
	BURGER_INLINE Word IsValid(void) const { return (m_uLength!=0); }
	void Clear(void);
};
}
/* END */

#endif
