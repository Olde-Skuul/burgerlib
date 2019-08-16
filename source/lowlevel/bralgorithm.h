/***************************************

    Templates to support "algorithm"

    Copyright (c) 1995-2019 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRALGORITHM_H__
#define __BRALGORITHM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

#ifndef __BRXCODE_H__
#include "brxcode.h"
#endif

#if defined(BURGER_WIIU) && !defined(__BRWIIUTYPES_H__)
#include "brwiiutypes.h"
#endif

/* BEGIN */
namespace Burger {

// value is true if both are the same
template<typename T, typename U>
struct is_same {
    static const bool value = false;
};

template<typename T>
struct is_same<T, T> {
    static const bool value = true;
};

// Add const to type
template<typename T>
struct add_const {
    typedef const T type;
};

// Remove const from type
template<typename T>
struct remove_const {
    typedef T type;
};

template<typename T>
struct remove_const<const T> {
    typedef T type;
};

// Remove const and volatile from type
template<typename T>
struct remove_cv {
    typedef T type;
};

template<typename T>
struct remove_cv<const T> {
    typedef T type;
};

template<typename T>
struct remove_cv<const volatile T> {
    typedef T type;
};

template<typename T>
struct remove_cv<volatile T> {
    typedef T type;
};

// Remove reference from type
template<class T>
struct remove_reference {
    typedef T type;
};

template<class T>
struct remove_reference<T&> {
    typedef T type;
};

#if defined(BURGER_RVALUE_REFERENCES)
template<class T>
struct remove_reference<T&&> {
    typedef T type;
};
#endif

// Return the minimum value
template<typename T>
BURGER_INLINE T Min(T A, T B)
{
    return ((A < B) ? A : B);
}

// Return the maximum value
template<typename T>
BURGER_INLINE T Max(T A, T B)
{
    return ((A > B) ? A : B);
}

#if defined(BURGER_PPC)
template<>
BURGER_INLINE float Min(float fA, float fB)
{
    return static_cast<float>(__fsel((fA - fB), fB, fA));
}

template<>
BURGER_INLINE double Min(double dA, double dB)
{
    return __fsel((dA - dB), dB, dA);
}

template<>
BURGER_INLINE float Max(float fA, float fB)
{
    return static_cast<float>(__fsel((fA - fB), fA, fB));
}

template<>
BURGER_INLINE double Max(double dA, double dB)
{
    return __fsel((dA - dB), dA, dB);
}

#elif (defined(BURGER_MSVC) && defined(BURGER_AMD64)) || \
    defined(BURGER_PS4) || \
    ((defined(BURGER_MACOSX) || defined(BURGER_IOS)) && defined(BURGER_INTEL))
template<>
BURGER_INLINE float Min(float fA, float fB)
{
    return _mm_cvtss_f32(_mm_min_ss(_mm_set_ss(fA), _mm_set_ss(fB)));
}

template<>
BURGER_INLINE double Min(double dA, double dB)
{
    return _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(dA), _mm_set_sd(dB)));
}

template<>
BURGER_INLINE float Max(float fA, float fB)
{
    return _mm_cvtss_f32(_mm_max_ss(_mm_set_ss(fA), _mm_set_ss(fB)));
}

template<>
BURGER_INLINE double Max(double dA, double dB)
{
    return _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(dA), _mm_set_sd(dB)));
}
#endif

}
/* END */

#endif
