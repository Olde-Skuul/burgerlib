/***************************************

	Simple data stream class for output

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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
	enum {
		CHUNKSIZE = 0x40000-static_cast<int>(sizeof(Word8 *))	///< Size of each data chunk
	};
	Word8 *m_pData;		///< Pointer to the first data buffer
	Word8 *m_pWork;		///< Pointer to the current buffer
	WordPtr m_uIndex;	///< Current file mark
	BURGER_DISABLECOPYCONSTRUCTORS(OutputMemoryStream);
public:
	OutputMemoryStream();
	~OutputMemoryStream();
	void BURGER_API Clear(void);
	WordPtr BURGER_API GetSize(void) const;
	Word BURGER_API IsEmpty(void) const;
	Word BURGER_API SaveFile(const char *pFilename) const;
	Word BURGER_API SaveFile(Filename *pFilename) const;
	Word BURGER_API Save(String *pOutput) const;
	Word BURGER_API Flatten(void *pOutput,WordPtr uLength) const;
	void *BURGER_API Flatten(WordPtr *pLength) const;
	Word BURGER_API Append(char iChar);
	Word BURGER_API Append(const char *pString);
	Word BURGER_API AppendPString(const char *pString);
	Word BURGER_API Append(Word8 uByte);
	Word BURGER_API Append(Word16 uShort);
	Word BURGER_API Append(Word32 uWord);
	Word BURGER_API Append(Word64 uWord);
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
