/***************************************

    Flash player data stream reader

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
	SimpleArray<uintptr_t> m_TagStack;	///< Stack of sections marks
	uint_t m_uBitBucket;					///< Bit bucket for bit fields
	uint_t m_uBitsRemaining;				///< Number of bits in the bit bucket
public:
	Stream(const Manager *pFlashPlayer,InputMemoryStream *pInput);
	~Stream();
	BURGER_INLINE const Manager *GetManager(void) const { return m_pFlashPlayer; }
	BURGER_INLINE void ByteAlign(void) { m_uBitsRemaining = 0; }
	BURGER_INLINE InputMemoryStream* GetStream(void) const { return m_pStream; }
	BURGER_INLINE uint_t GetBoolean(void) { return GetWord(1); }
	uint32_t BURGER_API GetEncodedU32(void);
	uint_t BURGER_API GetWord(uint_t uBitCount);
	int_t BURGER_API GetInt(uint_t uBitCount);
	BURGER_INLINE float GetFixedAsFloat(void) { m_uBitsRemaining = 0; return FIXEDTOFLOAT(static_cast<Fixed32>(m_pStream->GetWord32())); }
	float BURGER_API GetFloat16(void);
	BURGER_INLINE float GetFloat(void) { m_uBitsRemaining = 0; return m_pStream->GetFloat(); }
	BURGER_INLINE double GetDouble(void) { m_uBitsRemaining = 0; return m_pStream->GetDouble(); }
	BURGER_INLINE uint8_t GetByte(void) { m_uBitsRemaining = 0; return m_pStream->GetByte(); }
	BURGER_INLINE int8_t GetInt8(void) { m_uBitsRemaining = 0; return static_cast<int8_t>(m_pStream->GetByte()); }
	BURGER_INLINE uint16_t GetShort(void) { m_uBitsRemaining = 0; return m_pStream->GetShort(); }
	BURGER_INLINE int16_t GetInt16(void) { m_uBitsRemaining = 0; return static_cast<int16_t>(m_pStream->GetShort()); }
	BURGER_INLINE uint32_t GetWord32(void) { m_uBitsRemaining = 0; return m_pStream->GetWord32(); }
	BURGER_INLINE int32_t GetInt32(void) { m_uBitsRemaining = 0; return static_cast<int32_t>(m_pStream->GetWord32()); }
	BURGER_INLINE int32_t GetEncodedInt32(void) { return static_cast<int32_t>(GetEncodedU32()); }
	BURGER_INLINE uint32_t GetEncodedU30(void) { return GetEncodedU32(); }
	uint_t BURGER_API GetVariableCount(void);
	void BURGER_API ReadString(String *pOutput);		
	void BURGER_API ReadPString(String *pOutput);		
	void BURGER_API ReadString(uintptr_t uLength,String *pOutput);		
	BURGER_INLINE uintptr_t GetMark(void) const { return m_pStream->GetMark(); }
	void BURGER_API SetMark(uintptr_t uMark);
	uintptr_t	BURGER_API GetTagEndPosition(void) const;
	uint_t BURGER_API StartTag(void);
	void BURGER_API CloseTag(void);
};
}}
/* END */

#endif
