/***************************************

	Flash player data stream reader
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BRFLASHSTREAM_H__
#define __BRFLASHSTREAM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRINPUTMEMORYSTREAM_H__
#include "brinputmemorystream.h"
#endif

#ifndef __BRFIXEDPOINT_H__
#include "brfixedpoint.h"
#endif

#ifndef __BRSIMPLEARRAY_H__
#include "brsimplearray.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
class Manager;
class Stream {
	InputMemoryStream *m_pStream;		///< Pointer to the input stream
	const Manager *m_pFlashPlayer;		///< Pointer to the parent manager
	SimpleArray<WordPtr> m_TagStack;	///< Stack of sections marks
	Word m_uBitBucket;					///< Bit bucket for bit fields
	Word m_uBitsRemaining;				///< Number of bits in the bit bucket
public:
	Stream(const Manager *pFlashPlayer,InputMemoryStream *pInput);
	~Stream();
	BURGER_INLINE const Manager *GetManager(void) const { return m_pFlashPlayer; }
	BURGER_INLINE void ByteAlign(void) { m_uBitsRemaining = 0; }
	BURGER_INLINE InputMemoryStream* GetStream(void) const { return m_pStream; }
	BURGER_INLINE Word GetBoolean(void) { return GetWord(1); }
	Word32 BURGER_API GetEncodedU32(void);
	Word BURGER_API GetWord(Word uBitCount);
	Int BURGER_API GetInt(Word uBitCount);
	BURGER_INLINE float GetFixedAsFloat(void) { m_uBitsRemaining = 0; return FIXEDTOFLOAT(static_cast<Fixed32>(m_pStream->GetWord32())); }
	float BURGER_API GetFloat16(void);
	BURGER_INLINE float GetFloat(void) { m_uBitsRemaining = 0; return m_pStream->GetFloat(); }
	BURGER_INLINE double GetDouble(void) { m_uBitsRemaining = 0; return m_pStream->GetDouble(); }
	BURGER_INLINE Word8 GetByte(void) { m_uBitsRemaining = 0; return m_pStream->GetByte(); }
	BURGER_INLINE Int8 GetInt8(void) { m_uBitsRemaining = 0; return static_cast<Int8>(m_pStream->GetByte()); }
	BURGER_INLINE Word16 GetShort(void) { m_uBitsRemaining = 0; return m_pStream->GetShort(); }
	BURGER_INLINE Int16 GetInt16(void) { m_uBitsRemaining = 0; return static_cast<Int16>(m_pStream->GetShort()); }
	BURGER_INLINE Word32 GetWord32(void) { m_uBitsRemaining = 0; return m_pStream->GetWord32(); }
	BURGER_INLINE Int32 GetInt32(void) { m_uBitsRemaining = 0; return static_cast<Int32>(m_pStream->GetWord32()); }
	BURGER_INLINE Int32 GetEncodedInt32(void) { return static_cast<Int32>(GetEncodedU32()); }
	BURGER_INLINE Word32 GetEncodedU30(void) { return GetEncodedU32(); }
	Word BURGER_API GetVariableCount(void);
	void BURGER_API ReadString(String *pOutput);		
	void BURGER_API ReadPString(String *pOutput);		
	void BURGER_API ReadString(WordPtr uLength,String *pOutput);		
	BURGER_INLINE WordPtr GetMark(void) const { return m_pStream->GetMark(); }
	void BURGER_API SetMark(WordPtr uMark);
	WordPtr	BURGER_API GetTagEndPosition(void) const;
	Word BURGER_API StartTag(void);
	void BURGER_API CloseTag(void);
};
}}
/* END */

#endif
