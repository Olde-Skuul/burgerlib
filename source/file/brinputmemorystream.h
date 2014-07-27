/***************************************

	Simple data stream class for input

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRINPUTMEMORYSTREAM_H__
#define __BRINPUTMEMORYSTREAM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class Filename;
class InputMemoryStream {
	const Word8 *m_pWork;			///< Pointer to the input
	const Word8 *m_pEndOfBuffer;	///< Pointer to the end of the buffer
	const Word8 *m_pData;			///< Pointer to the first data buffer
	WordPtr m_uBufferSize;			///< Size of the buffer
	Word m_bDontFree;				///< \ref TRUE if the memory isn't released on Clear()
	BURGER_DISABLECOPYCONSTRUCTORS(InputMemoryStream);
public:
	InputMemoryStream();
	InputMemoryStream(const char *pFilename);
	InputMemoryStream(const void *pBuffer,WordPtr uBufferSize,Word bDontFree=FALSE);
	~InputMemoryStream();
	Word BURGER_API Open(const char *pFilename);
	Word BURGER_API Open(Filename *pFilename);
	void BURGER_API Open(const void *pBuffer,WordPtr uBufferSize,Word bDontFree=FALSE);
	void BURGER_API Clear(void);
	void BURGER_API SkipForward(WordPtr uOffset);
	void BURGER_API SkipBack(WordPtr uOffset);
	void BURGER_API SetMark(WordPtr uOffset);
	BURGER_INLINE const Word8 * GetPtr(void) const { return m_pWork; }
	BURGER_INLINE WordPtr GetMark(void) const { return static_cast<WordPtr>(m_pWork-m_pData); }
	BURGER_INLINE WordPtr GetSize(void) const { return m_uBufferSize; }
	BURGER_INLINE Word IsEmpty(void) const { return m_pWork>=m_pEndOfBuffer; }
	BURGER_INLINE WordPtr BytesRemaining(void) const { return static_cast<WordPtr>(m_pEndOfBuffer-m_pWork); }
	void BURGER_API GetString(char *pOutput,WordPtr uOutputSize);
	void BURGER_API GetCString(char *pOutput,WordPtr uOutputSize);
	Word8 BURGER_API GetByte(void);
	Word16 BURGER_API GetShort(void);
	Word16 BURGER_API GetBigShort(void);
	Word32 BURGER_API GetWord32(void);
	Word32 BURGER_API GetBigWord32(void);
	WordPtr BURGER_API Get(void *pOutput,WordPtr uOutputSize);
	void BURGER_API ParseBeyondWhiteSpace(void);
	Word BURGER_API IsStringMatch(const char *pInput);
	Word BURGER_API IsStringMatchCase(const char *pInput);
	Word BURGER_API IsDataMatch(const Word8 *pInput,WordPtr uLength);
};
}
/* END */

#endif
