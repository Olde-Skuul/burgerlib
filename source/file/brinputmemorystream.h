/***************************************

    Simple data stream class for input

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRINPUTMEMORYSTREAM_H__
#define __BRINPUTMEMORYSTREAM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRVECTOR2D_H__
#include "brvector2d.h"
#endif

#ifndef __BRVECTOR3D_H__
#include "brvector3d.h"
#endif

#ifndef __BRVECTOR4D_H__
#include "brvector4d.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

/* BEGIN */
namespace Burger {
class Filename;
struct RGBWord8_t;
struct RGBAWord8_t;
struct RGBFloat_t;
struct RGBAFloat_t;

class InputMemoryStream {
protected:
    const uint8_t* m_pWork;        ///< Pointer to the input
    const uint8_t* m_pEndOfBuffer; ///< Pointer to the end of the buffer
    const uint8_t* m_pData;        ///< Pointer to the first data buffer
    uintptr_t m_uBufferSize;       ///< Size of the buffer
    uint_t m_bDontFree; ///< \ref TRUE if the memory isn't released on Clear()
    BURGER_DISABLE_COPY(InputMemoryStream);

public:
    InputMemoryStream() BURGER_NOEXCEPT;
    InputMemoryStream(const char* pFilename) BURGER_NOEXCEPT;
    InputMemoryStream(Filename* pFilename) BURGER_NOEXCEPT;
    InputMemoryStream(
        const void* pBuffer, uintptr_t uBufferSize, uint_t bDontFree = FALSE) BURGER_NOEXCEPT;
    ~InputMemoryStream();
    uint_t BURGER_API Open(const char* pFilename) BURGER_NOEXCEPT;
    uint_t BURGER_API Open(Filename* pFilename) BURGER_NOEXCEPT;
    void BURGER_API Open(
        const void* pBuffer, uintptr_t uBufferSize, uint_t bDontFree = FALSE) BURGER_NOEXCEPT;
    void BURGER_API Clear(void) BURGER_NOEXCEPT;
    void BURGER_API SkipForward(uintptr_t uOffset) BURGER_NOEXCEPT;
    void BURGER_API SkipBack(uintptr_t uOffset) BURGER_NOEXCEPT;
    void BURGER_API SetMark(uintptr_t uOffset) BURGER_NOEXCEPT;

    BURGER_INLINE const uint8_t* GetPtr(void) const BURGER_NOEXCEPT
    {
        return m_pWork;
    }
    BURGER_INLINE uintptr_t GetMark(void) const BURGER_NOEXCEPT
    {
        return static_cast<uintptr_t>(m_pWork - m_pData);
    }
    BURGER_INLINE uintptr_t GetSize(void) const BURGER_NOEXCEPT
    {
        return m_uBufferSize;
    }
    BURGER_INLINE uint_t IsEmpty(void) const BURGER_NOEXCEPT
    {
        return m_pWork >= m_pEndOfBuffer;
    }
    BURGER_INLINE uintptr_t BytesRemaining(void) const BURGER_NOEXCEPT
    {
        return static_cast<uintptr_t>(m_pEndOfBuffer - m_pWork);
    }
    void BURGER_API GetString(char* pOutput, uintptr_t uOutputSize) BURGER_NOEXCEPT;
    void BURGER_API GetString(String* pOutput) BURGER_NOEXCEPT;
	void BURGER_API GetCString(char *pOutput, uintptr_t uOutputSize) BURGER_NOEXCEPT;
    void BURGER_API GetPString(char* pOutput, uintptr_t uOutputSize) BURGER_NOEXCEPT;
    uint8_t BURGER_API GetByte(void) BURGER_NOEXCEPT;
    uint16_t BURGER_API GetShort(void) BURGER_NOEXCEPT;
    uint16_t BURGER_API GetBigShort(void) BURGER_NOEXCEPT;
    uint32_t BURGER_API GetWord32(void) BURGER_NOEXCEPT;
    uint32_t BURGER_API GetBigWord32(void) BURGER_NOEXCEPT;
    uint64_t BURGER_API GetWord64(void) BURGER_NOEXCEPT;
    uint64_t BURGER_API GetBigWord64(void) BURGER_NOEXCEPT;
    float BURGER_API GetFloat(void) BURGER_NOEXCEPT;
    float BURGER_API GetBigFloat(void) BURGER_NOEXCEPT;
    double BURGER_API GetDouble(void) BURGER_NOEXCEPT;
    double BURGER_API GetBigDouble(void) BURGER_NOEXCEPT;
    uintptr_t BURGER_API Get(void* pOutput, uintptr_t uOutputSize) BURGER_NOEXCEPT;
    uint_t BURGER_API Get(RGBWord8_t* pOutput) BURGER_NOEXCEPT;
    uint_t BURGER_API Get(RGBAWord8_t* pOutput) BURGER_NOEXCEPT;
    uint_t BURGER_API Get(Vector2D_t* pOutput) BURGER_NOEXCEPT;
    uint_t BURGER_API Get(Vector3D_t* pOutput) BURGER_NOEXCEPT;
    uint_t BURGER_API Get(Vector4D_t* pOutput) BURGER_NOEXCEPT;
    BURGER_INLINE uint_t Get(RGBFloat_t* pOutput) BURGER_NOEXCEPT
    {
        return Get(reinterpret_cast<Vector3D_t*>(pOutput));
    }
    BURGER_INLINE uint_t Get(RGBAFloat_t* pOutput) BURGER_NOEXCEPT
    {
        return Get(reinterpret_cast<Vector4D_t*>(pOutput));
    }
    void BURGER_API ParseBeyondWhiteSpace(void) BURGER_NOEXCEPT;
    uint_t BURGER_API IsStringMatch(const char* pInput) BURGER_NOEXCEPT;
    uint_t BURGER_API IsStringMatchCase(const char* pInput) BURGER_NOEXCEPT;
    uint_t BURGER_API IsDataMatch(const uint8_t* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
};
}
/* END */

#endif
