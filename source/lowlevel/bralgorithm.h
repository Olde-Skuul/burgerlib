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

/** Type used for templates to return 1 */
typedef char yes_type;

/** Type used for templates to return 0 */
struct no_type {
    char m_NotUsed[8];
};

/** Template helper to detect a parameter exists */
template<class T>
struct wrap {
};

template<class T, T _Value>
struct integral_constant {
    /** Encapsulated value */
    static BURGER_CONSTEXPR const T value = _Value;
    /** Encapsulated type */
    typedef T value_type;
    /** Instanciated type of this template */
    typedef integral_constant<T, _Value> type;
    BURGER_INLINE BURGER_CONSTEXPR operator value_type() const BURGER_NOEXCEPT
    {
        return value;
    }

    BURGER_INLINE BURGER_CONSTEXPR value_type operator()() const BURGER_NOEXCEPT
    {
        return value;
    }
};

template<bool _Value>
struct bool_constant: Burger::integral_constant<bool, _Value> {
};

typedef bool_constant<true> true_type;
typedef bool_constant<false> false_type;

// Get alignment of an object
template<class T>
#if defined(BURGER_CPP11) || __has_feature(cxx_alignof) || \
    __has_extension(cxx_alignof) || defined(DOXYGEN)
struct alignment_of: integral_constant<uintptr_t, alignof(T)> {
};
#else
struct alignment_of {
    struct pad {
        T val;
        char byte;
    };
    enum { value = sizeof(pad) - sizeof(T) };
};
#endif

// Intrgral Constant Expression for and, or, equal, not
template<bool p1, bool p2, bool p3 = true, bool p4 = true, bool p5 = true,
    bool p6 = true, bool p7 = true>
struct ice_and;

template<bool p1, bool p2, bool p3, bool p4, bool p5, bool p6, bool p7>
struct ice_and {
    enum {
        value = false ///< true or false
    };
};

BURGER_EMPTY_TEMPLATE_DECLARATION
struct ice_and<true, true, true, true, true, true, true> {
    enum { value = true };
};

template<bool p1, bool p2, bool p3 = false, bool p4 = false, bool p5 = false,
    bool p6 = false, bool p7 = false>
struct ice_or;

template<bool p1, bool p2, bool p3, bool p4, bool p5, bool p6, bool p7>
struct ice_or {
    enum {
        value = true ///< true or false
    };
};

BURGER_EMPTY_TEMPLATE_DECLARATION
struct ice_or<false, false, false, false, false, false, false> {
    enum { value = false };
};

template<int p1, int p2>
struct ice_eq {
    enum {
        value = (p1 == p2) ///< true if equal
    };
};

template<int p1, int p2>
struct ice_ne {
    enum {
        value = (p1 != p2) ///< true if not equal
    };
};

template<bool p>
struct ice_not {
    enum {
        value = true ///< false becomes true
    };
};

BURGER_EMPTY_TEMPLATE_DECLARATION
struct ice_not<true> {
    enum {
        value = false ///< true becomes false
    };
};

// Value is true if both are the same
template<typename T1, typename T2>
struct is_same {
    static const bool value = false;
};

#if defined(BURGER_HAS_SFINAE)
template<typename T>
struct is_same<T, T> {
    static const bool value = true;
};
#endif

// Default deleter for std::delete
template<class T>
struct default_delete {
    /** Perform the delete with std::delete */
    BURGER_INLINE void operator()(T* pInput) const BURGER_NOEXCEPT
    {
        BURGER_STATIC_ASSERT(0 < sizeof(T));
        delete pInput;
    }
};

// Default deleter for std::delete[]
template<class T>
struct default_delete_array {
    /** Perform the delete with std::delete[] */
    BURGER_INLINE void operator()(T* pInput) const BURGER_NOEXCEPT
    {
        BURGER_STATIC_ASSERT(0 < sizeof(T));
        delete[] pInput;
    }
};

// Default deleter for Burger::Delete
template<class T>
struct default_delete_Base {
    /** Perform the delete with Burger::Delete() */
    BURGER_INLINE void operator()(T* pInput) const BURGER_NOEXCEPT
    {
        BURGER_STATIC_ASSERT(0 < sizeof(T));
        Delete(pInput);
    }
};

// Default deleter for Burger::Alloc and Free
template<class T>
struct default_delete_Free {
    /** Perform the delete with Burger::Free() */
    BURGER_INLINE void operator()(T* pInput) const BURGER_NOEXCEPT
    {
        BURGER_STATIC_ASSERT(0 < sizeof(T));
        Free(pInput);
    }
};

// Implementation of C++20 std::unique_ptr
template<class T, class Deleter = Burger::default_delete<T> >
class unique_ptr {
public:
    /** Object type for this unique_ptr */
    typedef T element_type;
    /** Deleter type for this unique_ptr */
    typedef Deleter deleter_type;
    /** Pointer type of object in this unique_ptr */
    typedef T* pointer;

protected:
    pointer m_pObject;      ///< Pointer to the object to track
    deleter_type m_Deleter; ///< Instance of the deleter object

public:
    unique_ptr() BURGER_NOEXCEPT: m_pObject(nullptr) {}
    unique_ptr(pointer pObject) BURGER_NOEXCEPT: m_pObject(pObject) {}

private:
    unique_ptr(const unique_ptr&) BURGER_EQUALS_DELETE;
    unique_ptr& operator=(const unique_ptr&) BURGER_EQUALS_DELETE;

public:
#if defined(BURGER_RVALUE_REFERENCES) || defined(DOXYGEN)
    unique_ptr(unique_ptr&& rInput) BURGER_NOEXCEPT
        : m_pObject(rInput.m_pObject),
          m_Deleter(rInput.m_Deleter)
    {
        rInput.m_pObject = nullptr;
    }
    unique_ptr& operator=(unique_ptr&& rInput) BURGER_NOEXCEPT
    {
        if (this != &rInput) {
            m_Deleter(m_pObject);
            m_pObject = rInput.m_pObject;
            rInput.m_pObject = nullptr;
            m_Deleter = static_cast<deleter_type&&>(rInput.m_Deleter);
        }
        return *this;
    }
#endif
    /** Dispose of the object */
    ~unique_ptr()
    {
        m_Deleter(m_pObject);
    }

    BURGER_INLINE unique_ptr& operator=(pointer pObject) BURGER_NOEXCEPT
    {
        pointer pTemp = m_pObject;
        if (pTemp != pObject) {
            m_Deleter(pTemp);
            m_pObject = pObject;
        }
        return *this;
    }

    BURGER_INLINE pointer release() BURGER_NOEXCEPT
    {
        pointer pTemp = m_pObject;
        m_pObject = nullptr;
        return pTemp;
    }

    BURGER_INLINE void reset(pointer pObject) BURGER_NOEXCEPT
    {
        pointer pTemp = m_pObject;
        if (pTemp != pObject) {
            m_Deleter(pTemp);
            m_pObject = pObject;
        }
    }

    BURGER_INLINE void reset(void) BURGER_NOEXCEPT
    {
        m_Deleter(m_pObject);
        m_pObject = nullptr;
    }

    BURGER_NODISCARD BURGER_INLINE pointer get() const BURGER_NOEXCEPT
    {
        return m_pObject;
    }

    BURGER_NODISCARD BURGER_INLINE deleter_type& get_deleter() BURGER_NOEXCEPT
    {
        return m_Deleter;
    }

    BURGER_NODISCARD BURGER_INLINE const deleter_type&
    get_deleter() const BURGER_NOEXCEPT
    {
        return m_Deleter;
    }

    BURGER_NODISCARD BURGER_INLINE operator bool() const BURGER_NOEXCEPT
    {
        return m_pObject != nullptr;
    }

    BURGER_NODISCARD BURGER_INLINE element_type&
    operator*() const BURGER_NOEXCEPT
    {
        return *m_pObject;
    }

    BURGER_NODISCARD BURGER_INLINE pointer operator->() const BURGER_NOEXCEPT
    {
        return m_pObject;
    }
};

template<class T>
struct unique_ptr_array
    : Burger::unique_ptr<T, Burger::default_delete_array<T> > {
};

template<class T>
struct unique_ptr_Base: Burger::unique_ptr<T, Burger::default_delete_Base<T> > {
};

template<class T>
struct unique_ptr_Free: Burger::unique_ptr<T, Burger::default_delete_Free<T> > {
};

// Swap the contents of two memory variables
template<class T>
BURGER_INLINE void SwapVariables(T* pA, T* pB)
{
    T tTemp(*pA);
    *pA = *pB;
    *pB = tTemp;
}

// Return the minimum value
template<typename T>
BURGER_INLINE T Min(T A, T B) BURGER_NOEXCEPT
{
    return ((A < B) ? A : B);
}

// Return the maximum value
template<typename T>
BURGER_INLINE T Max(T A, T B) BURGER_NOEXCEPT
{
    return ((A > B) ? A : B);
}

#if defined(BURGER_PPC) && !defined(BURGER_MRC)
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
BURGER_INLINE float Min(float fA, float fB) BURGER_NOEXCEPT
{
    return _mm_cvtss_f32(_mm_min_ss(_mm_set_ss(fA), _mm_set_ss(fB)));
}

template<>
BURGER_INLINE double Min(double dA, double dB) BURGER_NOEXCEPT
{
    return _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(dA), _mm_set_sd(dB)));
}

template<>
BURGER_INLINE float Max(float fA, float fB) BURGER_NOEXCEPT
{
    return _mm_cvtss_f32(_mm_max_ss(_mm_set_ss(fA), _mm_set_ss(fB)));
}

template<>
BURGER_INLINE double Max(double dA, double dB) BURGER_NOEXCEPT
{
    return _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(dA), _mm_set_sd(dB)));
}
#endif

}
/* END */

#endif
