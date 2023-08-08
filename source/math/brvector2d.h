/***************************************

    2D Floating point vector manager

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRVECTOR2D_H__
#define __BRVECTOR2D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFLOATINGPOINT_H__
#include "brfloatingpoint.h"
#endif

/* BEGIN */
namespace Burger {
struct FixedVector2D_t;

struct Vector2D_t {
    float x; ///< 32 bit floating point X value for the 2D Vector
    float y; ///< 32 bit floating point Y value for the 2D Vector

    BURGER_INLINE void Zero(void) BURGER_NOEXCEPT
    {
        x = 0.0f;
        y = 0.0f;
    }
    BURGER_INLINE void One(void) BURGER_NOEXCEPT
    {
        x = 1.0f;
        y = 1.0f;
    }
    BURGER_INLINE float GetX(void) const BURGER_NOEXCEPT
    {
        return x;
    }
    BURGER_INLINE float GetY(void) const BURGER_NOEXCEPT
    {
        return y;
    }
    BURGER_INLINE void SetX(float fX) BURGER_NOEXCEPT
    {
        x = fX;
    }
    BURGER_INLINE void SetY(float fY) BURGER_NOEXCEPT
    {
        y = fY;
    }
    BURGER_INLINE void Set(float fInput) BURGER_NOEXCEPT
    {
        x = fInput;
        y = fInput;
    };
    BURGER_INLINE void Set(float fX, float fY) BURGER_NOEXCEPT
    {
        x = fX;
        y = fY;
    };
    BURGER_INLINE void Set(const Vector2D_t* pInput) BURGER_NOEXCEPT
    {
        *this = *pInput;
    }
    void BURGER_API Set(const FixedVector2D_t* pInput) BURGER_NOEXCEPT;
    BURGER_INLINE void Negate(void) BURGER_NOEXCEPT
    {
        x = -x;
        y = -y;
    }
    BURGER_INLINE void Negate(float fX, float fY) BURGER_NOEXCEPT
    {
        x = -fX;
        y = -fY;
    }
    BURGER_INLINE void Negate(const Vector2D_t* pInput) BURGER_NOEXCEPT
    {
        x = -pInput->x;
        y = -pInput->y;
    }
    void BURGER_API Interpolate(const Vector2D_t* pFrom, const Vector2D_t* pTo,
        float fFactor) BURGER_NOEXCEPT;
    BURGER_INLINE float Dot(float fX, float fY) const BURGER_NOEXCEPT
    {
        return (x * fX) + (y * fY);
    }
    BURGER_INLINE float Dot(const Vector2D_t* pInput) const BURGER_NOEXCEPT
    {
        return (x * pInput->x) + (y * pInput->y);
    }
    BURGER_INLINE void Add(const Vector2D_t* pInput) BURGER_NOEXCEPT
    {
        x += pInput->x;
        y += pInput->y;
    }
    BURGER_INLINE void Add(float fInput) BURGER_NOEXCEPT
    {
        x += fInput;
        y += fInput;
    }
    BURGER_INLINE void Add(
        const Vector2D_t* pInput1, const Vector2D_t* pInput2) BURGER_NOEXCEPT
    {
        x = pInput1->x + pInput2->x;
        y = pInput1->y + pInput2->y;
    }
    BURGER_INLINE void Add(
        const Vector2D_t* pInput, float fInput) BURGER_NOEXCEPT
    {
        x = pInput->x + fInput;
        y = pInput->y + fInput;
    }
    BURGER_INLINE void Sub(const Vector2D_t* pInput) BURGER_NOEXCEPT
    {
        x -= pInput->x;
        y -= pInput->y;
    }
    BURGER_INLINE void Sub(float fInput) BURGER_NOEXCEPT
    {
        x -= fInput;
        y -= fInput;
    }
    BURGER_INLINE void Sub(
        const Vector2D_t* pInput1, const Vector2D_t* pInput2) BURGER_NOEXCEPT
    {
        x = pInput1->x - pInput2->x;
        y = pInput1->y - pInput2->y;
    }
    BURGER_INLINE void Sub(
        const Vector2D_t* pInput, float fInput) BURGER_NOEXCEPT
    {
        x = pInput->x - fInput;
        y = pInput->y - fInput;
    }
    BURGER_INLINE void Mul(const Vector2D_t* pInput) BURGER_NOEXCEPT
    {
        x *= pInput->x;
        y *= pInput->y;
    }
    BURGER_INLINE void Mul(float fInput) BURGER_NOEXCEPT
    {
        x *= fInput;
        y *= fInput;
    }
    BURGER_INLINE void Mul(
        const Vector2D_t* pInput1, const Vector2D_t* pInput2) BURGER_NOEXCEPT
    {
        x = pInput1->x * pInput2->x;
        y = pInput1->y * pInput2->y;
    }
    BURGER_INLINE void Mul(
        const Vector2D_t* pInput, float fInput) BURGER_NOEXCEPT
    {
        x = pInput->x * fInput;
        y = pInput->y * fInput;
    }
    BURGER_INLINE float Cross(float fX, float fY) const BURGER_NOEXCEPT
    {
        return (x * fY) - (y * fX);
    }
    BURGER_INLINE float Cross(const Vector2D_t* pInput) const BURGER_NOEXCEPT
    {
        return (x * pInput->y) - (y * pInput->x);
    }
    BURGER_INLINE float GetLengthSquared(void) const BURGER_NOEXCEPT
    {
        return (x * x) + (y * y);
    }
    BURGER_INLINE float GetLength(void) const BURGER_NOEXCEPT
    {
        return square_root((x * x) + (y * y));
    }
    BURGER_INLINE float GetLengthFast(void) const BURGER_NOEXCEPT
    {
        return square_root((x * x) + (y * y));
    }
    void BURGER_API SetLength(float fInput) BURGER_NOEXCEPT;
    void BURGER_API SetLengthFast(float fInput) BURGER_NOEXCEPT;
    float BURGER_API GetDistanceSquared(
        float fX, float fY) const BURGER_NOEXCEPT;
    float BURGER_API GetDistanceSquared(
        const Vector2D_t* pInput) const BURGER_NOEXCEPT;
    float BURGER_API GetDistance(float fX, float fY) const BURGER_NOEXCEPT;
    float BURGER_API GetDistance(
        const Vector2D_t* pInput) const BURGER_NOEXCEPT;
    float BURGER_API GetDistanceFast(float fX, float fY) const BURGER_NOEXCEPT;
    float BURGER_API GetDistanceFast(
        const Vector2D_t* pInput) const BURGER_NOEXCEPT;
    void BURGER_API Normalize(void) BURGER_NOEXCEPT;
    void BURGER_API Normalize(float fX, float fY) BURGER_NOEXCEPT;
    void BURGER_API Normalize(const Vector2D_t* pInput) BURGER_NOEXCEPT;
    void BURGER_API NormalizeFast(void) BURGER_NOEXCEPT;
    void BURGER_API NormalizeFast(float fX, float fY) BURGER_NOEXCEPT;
    void BURGER_API NormalizeFast(const Vector2D_t* pInput) BURGER_NOEXCEPT;
    uint_t BURGER_API BitwiseEqual(
        const Vector2D_t* pInput) const BURGER_NOEXCEPT;
    BURGER_INLINE float Determinant(const Vector2D_t* pInput1,
        const Vector2D_t* pInput2) const BURGER_NOEXCEPT
    {
        return ((pInput1->x - x) * (pInput2->y - y)) -
            ((pInput1->y - y) * (pInput2->x - x));
    }
    uint_t BURGER_API IsPointInTriangle(const Vector2D_t* pVertex1,
        const Vector2D_t* pVertex2,
        const Vector2D_t* pVertex3) const BURGER_NOEXCEPT;
    BURGER_INLINE float& operator[](uintptr_t uInput) BURGER_NOEXCEPT
    {
        return (&x)[uInput];
    }
    BURGER_INLINE const float& operator[](
        uintptr_t uInput) const BURGER_NOEXCEPT
    {
        return (&x)[uInput];
    }
    BURGER_INLINE uint_t operator==(
        const Vector2D_t& rInput) const BURGER_NOEXCEPT
    {
        return (x == rInput.x) && (y == rInput.y);
    }
    BURGER_INLINE uint_t operator!=(
        const Vector2D_t& rInput) const BURGER_NOEXCEPT
    {
        return (x != rInput.x) || (y != rInput.y);
    }
    BURGER_INLINE operator const float *() const BURGER_NOEXCEPT
    {
        return &x;
    }
};

union Word32ToVector2D_t {
    uint32_t x[2]; ///< Value as two 32 bit unsigned integers
    Vector2D_t v;  ///< Value as two 32 bit floats

    BURGER_INLINE operator const Vector2D_t &() const BURGER_NOEXCEPT
    {
        return v;
    }
};

extern const Vector2D_t g_Vector2DZero;
extern const Vector2D_t g_Vector2DOne;
extern const Vector2D_t g_Vector2DOneX;
extern const Vector2D_t g_Vector2DOneY;
extern const Word32ToVector2D_t g_Vector2DMax;
extern const Word32ToVector2D_t g_Vector2DInfinity;
}
/* END */

#endif
