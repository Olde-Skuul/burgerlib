/***************************************

	Simple data stream class for output

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BROUTPUTMEMORYSTREAM_H__
#define __BROUTPUTMEMORYSTREAM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

#ifndef __BRINPUTMEMORYSTREAM_H__
#include "brinputmemorystream.h"
#endif

/* BEGIN */
namespace Burger {
class OutputMemoryStream {
	static const WordPtr CHUNKSIZE = 0x40000U-(sizeof(Word8 *)+sizeof(WordPtr));	///< Size of each data chunk

	struct Chunk_t {
		Chunk_t *m_pNext;			///< Pointer to the next chunk
		WordPtr m_uMark;			///< Base file mark for this chunk
		Word8 m_Buffer[CHUNKSIZE];	///< Chunk data
	};

	Chunk_t *m_pRoot;		///< Pointer to the first chunk
	Chunk_t *m_pCurrent;	///< Pointer to the current chunk
	WordPtr m_uIndex;		///< Number of bytes used the current chunk
	WordPtr m_uFileSize;	///< Number of bytes stored in the stream
	Word m_uError;			///< Did an error occur?
    BURGER_DISABLE_COPY(OutputMemoryStream);
public:
	OutputMemoryStream();
	~OutputMemoryStream();
	void BURGER_API Clear(void);
	BURGER_INLINE WordPtr GetSize(void) const { return m_uFileSize; }
	BURGER_INLINE Word IsEmpty(void) const { return m_uFileSize==0; }
	BURGER_INLINE Word GetError(void) const { return m_uError; }
	Word BURGER_API SetMark(WordPtr uMark);
	WordPtr BURGER_API GetMark(void) const;
	Word BURGER_API SaveFile(const char *pFilename) const;
	Word BURGER_API SaveFile(Filename *pFilename) const;
	Word BURGER_API Save(String *pOutput) const;
	Word BURGER_API Flatten(void *pOutput,WordPtr uLength) const;
	void *BURGER_API Flatten(WordPtr *pLength) const;
	Word BURGER_API Append(char iChar);
	Word BURGER_API Append(const char *pString);
	Word BURGER_API AppendCString(const char *pString);
	BURGER_INLINE Word Append(const String *pString) { return AppendCString(pString->GetPtr()); }
	Word BURGER_API AppendPString(const char *pString);
	Word BURGER_API Append(Word8 uByte);
	Word BURGER_API Append(Word16 uShort);
	Word BURGER_API Append(Word32 uWord);
	Word BURGER_API Append(Word64 uWord);
	Word BURGER_API Append(float fInput);
	Word BURGER_API Append(double dInput);
	Word BURGER_API Append(const RGBWord8_t *pInput);
	Word BURGER_API Append(const RGBAWord8_t *pInput);
	Word BURGER_API Append(const Vector3D_t *pInput);
	Word BURGER_API Append(const Vector4D_t *pInput);
	BURGER_INLINE Word Append(const RGBFloat_t *pInput) { return Append(reinterpret_cast<const Vector3D_t *>(pInput)); }
	BURGER_INLINE Word Append(const RGBAFloat_t *pInput) { return Append(reinterpret_cast<const Vector4D_t *>(pInput)); }
	Word BURGER_API Append(const void *pData,WordPtr uSize);
	Word BURGER_API AppendAscii(Word32 uInput);
	Word BURGER_API AppendAscii(Word64 uInput);
	Word BURGER_API AppendAscii(Int32 iInput);
	Word BURGER_API AppendAscii(Int64 iInput);
	Word BURGER_API AppendAscii(float fInput);
	Word BURGER_API AppendAscii(double dInput);
	Word BURGER_API AppendTabs(Word uTabCount);
	Word BURGER_API Compare(const void *pInput,WordPtr uLength) const;
	Word BURGER_API Overwrite(const void *pInput,WordPtr uLength,WordPtr uOffset);
};
}
/* END */

#endif
