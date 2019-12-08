/***************************************

    Simple data stream class for output

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BROUTPUTMEMORYSTREAM_H__
#define __BROUTPUTMEMORYSTREAM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
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
    static const uintptr_t CHUNKSIZE = 0x40000U -
        (sizeof(uint8_t*) + sizeof(uintptr_t)); ///< Size of each data chunk

    struct Chunk_t {
        Chunk_t* m_pNext;            ///< Pointer to the next chunk
        uintptr_t m_uMark;           ///< Base file mark for this chunk
        uint8_t m_Buffer[CHUNKSIZE]; ///< Chunk data
    };

    Chunk_t* m_pRoot;      ///< Pointer to the first chunk
    Chunk_t* m_pCurrent;   ///< Pointer to the current chunk
    uintptr_t m_uIndex;    ///< Number of bytes used the current chunk
    uintptr_t m_uFileSize; ///< Number of bytes stored in the stream
    eError m_uError;       ///< Did an error occur?

    BURGER_DISABLE_COPY(OutputMemoryStream);

public:
    OutputMemoryStream() BURGER_NOEXCEPT;
    ~OutputMemoryStream();
    void BURGER_API Clear(void) BURGER_NOEXCEPT;
    BURGER_INLINE uintptr_t GetSize(void) const BURGER_NOEXCEPT
    {
        return m_uFileSize;
    }
    BURGER_INLINE uint_t IsEmpty(void) const BURGER_NOEXCEPT
    {
        return !m_uFileSize;
    }
    BURGER_INLINE eError GetError(void) const BURGER_NOEXCEPT
    {
        return m_uError;
    }
    eError BURGER_API SetMark(uintptr_t uMark) BURGER_NOEXCEPT;
    uintptr_t BURGER_API GetMark(void) const BURGER_NOEXCEPT;
    eError BURGER_API SaveFile(const char* pFilename) const BURGER_NOEXCEPT;
    eError BURGER_API SaveFile(Filename* pFilename) const BURGER_NOEXCEPT;
    eError BURGER_API Save(String* pOutput) const BURGER_NOEXCEPT;
    eError BURGER_API Flatten(
        void* pOutput, uintptr_t uLength) const BURGER_NOEXCEPT;
    void* BURGER_API Flatten(uintptr_t* pLength) const BURGER_NOEXCEPT;
    eError BURGER_API Append(char iChar) BURGER_NOEXCEPT;
    eError BURGER_API Append(const char* pString) BURGER_NOEXCEPT;
    eError BURGER_API AppendCString(const char* pString) BURGER_NOEXCEPT;
    BURGER_INLINE eError Append(const String* pString) BURGER_NOEXCEPT
    {
        return AppendCString(pString->GetPtr());
    }
    eError BURGER_API AppendPString(const char* pString) BURGER_NOEXCEPT;
    eError BURGER_API Append(uint8_t uInput) BURGER_NOEXCEPT;
    eError BURGER_API Append(uint16_t uInput) BURGER_NOEXCEPT;
    eError BURGER_API Append(uint32_t uInput) BURGER_NOEXCEPT;
    eError BURGER_API Append(uint64_t uInput) BURGER_NOEXCEPT;
    eError BURGER_API Append(float fInput) BURGER_NOEXCEPT;
    eError BURGER_API Append(double dInput) BURGER_NOEXCEPT;
    eError BURGER_API Append(const RGBWord8_t* pInput) BURGER_NOEXCEPT;
    eError BURGER_API Append(const RGBAWord8_t* pInput) BURGER_NOEXCEPT;
    eError BURGER_API Append(const Vector3D_t* pInput) BURGER_NOEXCEPT;
    eError BURGER_API Append(const Vector4D_t* pInput) BURGER_NOEXCEPT;
    BURGER_INLINE eError Append(const RGBFloat_t* pInput) BURGER_NOEXCEPT
    {
        return Append(reinterpret_cast<const Vector3D_t*>(pInput));
    }
    BURGER_INLINE eError Append(const RGBAFloat_t* pInput) BURGER_NOEXCEPT
    {
        return Append(reinterpret_cast<const Vector4D_t*>(pInput));
    }
    eError BURGER_API Append(
        const void* pData, uintptr_t uSize) BURGER_NOEXCEPT;
    eError BURGER_API BigEndianAppend(uint16_t uInput) BURGER_NOEXCEPT;
    eError BURGER_API BigEndianAppend(uint32_t uInput) BURGER_NOEXCEPT;
    eError BURGER_API BigEndianAppend(uint64_t uInput) BURGER_NOEXCEPT;
    eError BURGER_API BigEndianAppend(float fInput) BURGER_NOEXCEPT;
    eError BURGER_API BigEndianAppend(double dInput) BURGER_NOEXCEPT;
    eError BURGER_API AppendAscii(uint16_t uInput) BURGER_NOEXCEPT;
    eError BURGER_API AppendAscii(uint32_t uInput) BURGER_NOEXCEPT;
    eError BURGER_API AppendAscii(uint64_t uInput) BURGER_NOEXCEPT;
    eError BURGER_API AppendAscii(int16_t iInput) BURGER_NOEXCEPT;
    eError BURGER_API AppendAscii(int32_t iInput) BURGER_NOEXCEPT;
    eError BURGER_API AppendAscii(int64_t iInput) BURGER_NOEXCEPT;
    eError BURGER_API AppendAscii(float fInput) BURGER_NOEXCEPT;
    eError BURGER_API AppendAscii(double dInput) BURGER_NOEXCEPT;
    eError BURGER_API AppendTabs(uint_t uTabCount) BURGER_NOEXCEPT;
    uint_t BURGER_API Compare(
        const void* pInput, uintptr_t uLength) const BURGER_NOEXCEPT;
    eError BURGER_API Overwrite(const void* pInput, uintptr_t uLength,
        uintptr_t uOffset) BURGER_NOEXCEPT;
};
}
/* END */

#endif
