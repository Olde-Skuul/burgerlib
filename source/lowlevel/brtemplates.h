/***************************************

	Templates

	Copyright (c) 2021-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRTEMPLATES_H__
#define __BRTEMPLATES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRVECTORTYPES_H__
#include "brvectortypes.h"
#endif

// For __fsel
#ifndef __BRXCODE_H__
#include "brxcode.h"
#endif

// For intrinsics
#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

/* BEGIN */
// Get rid of a spurious warning about the use of sizeof()
#if defined(BURGER_GHS)
#pragma ghs nowarning 1931
#endif

namespace Burger {

// Differing sized types for return value testing
namespace type_traits {

// Must be size of one
typedef char yes_type;

// Must NOT be size of one
struct no_type {
	/** Literally not used. */
	char m_NotUsed[8];
};

// Only instantiated when invoked by uintptr_t
template<uintptr_t T>
struct size_type {};
}

// conditional
template<bool B, class T, class F>
struct conditional {
	/** Type for false condition */
	typedef F type;
};

#if !defined(DOXYGEN)
template<class T, class F>
struct conditional<true, T, F> {
	/** Type for true condition */
	typedef T type;
};
#endif

// enable_if
template<bool B, class T = void>
struct enable_if {
	typedef T type;
};

#if !defined(DOXYGEN)
template<class T>
struct enable_if<false, T> {};
#endif

// disable_if
template<bool B, class T = void>
struct disable_if {
	typedef T type;
};

#if !defined(DOXYGEN)
template<class T>
struct disable_if<true, T> {};
#endif

// integral_constant
template<class T, T _Value>
struct integral_constant {

	/** Encapsulated value */
	static BURGER_CONSTEXPR const T value = _Value;

	/** Encapsulated type */
	typedef T value_type;

	/** Instantiated type of this template */
	typedef integral_constant<T, _Value> type;

	BURGER_INLINE BURGER_CONSTEXPR operator T() const BURGER_NOEXCEPT
	{
		return value;
	}

	BURGER_INLINE BURGER_CONSTEXPR T operator()() const BURGER_NOEXCEPT
	{
		return value;
	}
};

// bool_constant, true_type, false_type
template<bool _Value>
struct bool_constant: integral_constant<bool, _Value> {};

typedef bool_constant<true> true_type;
typedef bool_constant<false> false_type;

// alignment_of

// C++ 11 version
template<class T>
#if defined(BURGER_CPP11) || __has_feature(cxx_alignof) || \
	__has_extension(cxx_alignof) || defined(DOXYGEN)
struct alignment_of: integral_constant<uintptr_t, alignof(T)> {
};

#else
// Older compiler version of alignment_of
struct alignment_of {
	struct pad {
		T val;
		uint8_t byte;
	};
	enum { value = uint_t(sizeof(pad) - sizeof(T)) };
};
#endif

// Integral Constant Expression for and, or, equal, not
#if !defined(DOXYGEN)
template<bool p1, bool p2, bool p3 = true, bool p4 = true, bool p5 = true,
	bool p6 = true, bool p7 = true>
struct ice_and;
#endif

template<bool p1, bool p2, bool p3, bool p4, bool p5, bool p6, bool p7>
struct ice_and {
	enum {
		value = false ///< true or false
	};
};

#if !defined(DOXYGEN)
template<>
struct ice_and<true, true, true, true, true, true, true> {
	enum { value = true };
};

template<bool p1, bool p2, bool p3 = false, bool p4 = false, bool p5 = false,
	bool p6 = false, bool p7 = false>
struct ice_or;
#endif

template<bool p1, bool p2, bool p3, bool p4, bool p5, bool p6, bool p7>
struct ice_or {
	enum {
		value = true ///< true or false
	};
};

#if !defined(DOXYGEN)
template<>
struct ice_or<false, false, false, false, false, false, false> {
	enum { value = false };
};
#endif

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

#if !defined(DOXYGEN)
template<>
struct ice_not<true> {
	enum {
		value = false ///< true becomes false
	};
};
#endif

// Implementation of is_same
template<class T, class U>
struct is_same {
	static BURGER_CONSTEXPR const bool value = false;
};

#if !defined(DOXYGEN)
template<class T>
struct is_same<T, T> {
	static BURGER_CONSTEXPR const bool value = true;
};
#endif

// Implementation of remove_const
template<class T>
struct remove_const {
	typedef T type; ///< Type without const
};

#if !defined(DOXYGEN)
template<class T>
struct remove_const<T const> {
	typedef T type;
};

#if !(defined(BURGER_METROWERKS) && defined(BURGER_X86))
template<class T>
struct remove_const<T const[]> {
	typedef T type[];
};
#endif

template<class T, uintptr_t N>
struct remove_const<T const[N]> {
	typedef T type[N];
};
#endif

// Implementation of remove_volatile
template<class T>
struct remove_volatile {
	typedef T type; ///< Type without volatile
};

#if !defined(DOXYGEN)
template<class T>
struct remove_volatile<T volatile> {
	typedef T type;
};

#if !(defined(BURGER_METROWERKS) && defined(BURGER_X86))
template<class T>
struct remove_volatile<T volatile[]> {
	typedef T type[];
};
#endif

template<class T, uintptr_t N>
struct remove_volatile<T volatile[N]> {
	typedef T type[N];
};
#endif

// Implementation of remove_cv
template<class T>
struct remove_cv {
	/** Type without volatile or const */
	typedef typename remove_volatile<typename remove_const<T>::type>::type type;
};

// Implementation of add_const
template<class T>
struct add_const {
	typedef T const type; ///< Type with const
};

#if !defined(DOXYGEN)
template<class T>
struct add_const<T&> {
	typedef T& type;
};
#endif

// Implementation of add_volatile
template<class T>
struct add_volatile {
	typedef volatile T type; ///< Type with volatile
};

#if !defined(DOXYGEN)
template<class T>
struct add_volatile<T&> {
	typedef T& type;
};
#endif

// Implementation of add_cv
template<class T>
struct add_cv {
	typedef const volatile T type; ///< Type with const and volatile
};

#if !defined(DOXYGEN)
template<class T>
struct add_cv<T&> {
	typedef T& type;
};
#endif

// Implementation of remove_reference
template<class T>
struct remove_reference {
	typedef T type; ///< Type with reference removed
};

#if !defined(DOXYGEN)
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
#endif

// Implementation of is_const

template<class T>
struct is_const {
	static BURGER_CONSTEXPR const bool value = false;
};

#if !defined(DOXYGEN)
template<class T>
struct is_const<const T> {
	static BURGER_CONSTEXPR const bool value = true;
};
#endif

// Implementation of is_volatile
template<class T>
struct is_volatile {
	static BURGER_CONSTEXPR const bool value = false;
};

#if !defined(DOXYGEN)
template<class T>
struct is_volatile<volatile T> {
	static BURGER_CONSTEXPR const bool value = true;
};
#endif

// Implementation of is_void
#if !defined(DOXYGEN)
namespace details {
template<class T>
struct is_void_imp {
	static BURGER_CONSTEXPR const bool value = false;
};

template<>
struct is_void_imp<void> {
	static BURGER_CONSTEXPR const bool value = true;
};
}
#endif

template<class T>
struct is_void {
	static BURGER_CONSTEXPR const bool value =
		details::is_void_imp<typename remove_cv<T>::type>::value;
};

// Implementation of is_integral
#if !defined(DOXYGEN)
namespace details {
template<class T>
struct is_integral_imp {
	static BURGER_CONSTEXPR const bool value = false;
};

template<>
struct is_integral_imp<bool> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<char> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<signed char> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<unsigned char> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<short> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<unsigned short> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<int> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<unsigned int> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<long> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<unsigned long> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<long long> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<unsigned long long> {
	static BURGER_CONSTEXPR const bool value = true;
};

#if defined(BURGER_HAS_WCHAR_T)
template<>
struct is_integral_imp<wchar_t> {
	static BURGER_CONSTEXPR const bool value = true;
};
#endif

#if defined(BURGER_HAS_CHAR8_T)
template<>
struct is_integral_imp<char8_t> {
	static BURGER_CONSTEXPR const bool value = true;
};
#endif

#if defined(BURGER_HAS_CHAR16_T)
template<>
struct is_integral_imp<char16_t> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_integral_imp<char32_t> {
	static BURGER_CONSTEXPR const bool value = true;
};
#endif
}
#endif

template<class T>
struct is_integral {
	static BURGER_CONSTEXPR const bool value =
		details::is_integral_imp<typename remove_cv<T>::type>::value;
};

// Implementation of is_floating_point
#if !defined(DOXYGEN)
namespace details {
template<class T>
struct is_floating_imp {
	static BURGER_CONSTEXPR const bool value = false;
};

template<>
struct is_floating_imp<float> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_floating_imp<double> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<>
struct is_floating_imp<long double> {
	static BURGER_CONSTEXPR const bool value = true;
};
}
#endif

template<class T>
struct is_floating_point {
	static BURGER_CONSTEXPR const bool value =
		details::is_floating_imp<typename remove_cv<T>::type>::value;
};

// Implementation of is_arithmetic
template<class T>
struct is_arithmetic {
	static BURGER_CONSTEXPR const bool value =
		is_integral<T>::value || is_floating_point<T>::value;
};

// Implementation of is_fundamental
template<class T>
struct is_fundamental {
	static BURGER_CONSTEXPR const bool value =
		is_arithmetic<T>::value || is_void<T>::value;
};

// Implementation of is_pointer
#if !defined(DOXYGEN)
namespace details {
template<class T>
struct is_pointer_imp {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class T>
struct is_pointer_imp<T*> {
	static BURGER_CONSTEXPR const bool value = true;
};
}
#endif

template<class T>
struct is_pointer {
	static BURGER_CONSTEXPR const bool value =
		details::is_pointer_imp<typename remove_cv<T>::type>::value;
};

// Implementation of remove_pointer
#if defined(BURGER_MSVC) && (BURGER_MSVC < 190000000)
// Workaround MSVC 2005-2013 bug on edge case for T (*)(X) where the types are
// different.
template<class T>
struct remove_pointer_4 {
	typedef T type;
};

template<class T>
struct remove_pointer_4<T*> {
	typedef T type;
};

template<class T, bool b>
struct remove_pointer_3 {
	typedef typename remove_pointer_4<typename remove_cv<T>::type>::type type;
};

template<class T>
struct remove_pointer_3<T, false> {
	typedef T type;
};

template<class T>
struct remove_pointer_2 {
	typedef typename remove_pointer_3<T, is_pointer<T>::value>::type type;
};

template<class T>
struct remove_pointer {
	typedef typename remove_pointer_2<T>::type type;
};

#else

template<class T>
struct remove_pointer {
	typedef T type; ///< Type with pointer removed
};

#if !defined(DOXYGEN)
template<class T>
struct remove_pointer<T*> {
	typedef T type;
};

template<class T>
struct remove_pointer<T* const> {
	typedef T type;
};

template<class T>
struct remove_pointer<T* volatile> {
	typedef T type;
};

template<class T>
struct remove_pointer<T* const volatile> {
	typedef T type;
};
#endif
#endif

// Implementation of move
#if !defined(BURGER_RVALUE_REFERENCES)
template<class T>
BURGER_CONSTEXPR BURGER_INLINE typename remove_reference<T>::type& move(
	T& t) BURGER_NOEXCEPT
{
	return static_cast<typename remove_reference<T>::type&>(t);
}
#else
template<class T>
BURGER_CONSTEXPR BURGER_INLINE typename remove_reference<T>::type&& move(
	T&& t) BURGER_NOEXCEPT
{
	return static_cast<typename remove_reference<T>::type&&>(t);
}
#endif

// Implementation of is_lvalue_reference
template<class T>
struct is_lvalue_reference {
	static BURGER_CONSTEXPR const bool value = false;
};

#if !defined(DOXYGEN)
template<class T>
struct is_lvalue_reference<T&> {
	static BURGER_CONSTEXPR const bool value = true;
};
#endif

// Implementation of is_rvalue_reference
template<class T>
struct is_rvalue_reference {
	static BURGER_CONSTEXPR const bool value = false;
};

#if defined(BURGER_RVALUE_REFERENCES) && !defined(DOXYGEN)
template<class T>
struct is_rvalue_reference<T&&> {
	static BURGER_CONSTEXPR const bool value = true;
};

// Visual Studio 2012 or lower needs these extras
#if (defined(BURGER_MSVC) && (BURGER_MSVC <= 170000000)) && !defined(DOXYGEN)
template<class R>
struct is_rvalue_reference<R (&&)()> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R, class T0>
struct is_rvalue_reference<R (&&)(T0)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R, class T0, class T1>
struct is_rvalue_reference<R (&&)(T0, T1)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R, class T0, class T1, class T2>
struct is_rvalue_reference<R (&&)(T0, T1, T2)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R, class T0, class T1, class T2, class T3>
struct is_rvalue_reference<R (&&)(T0, T1, T2, T3)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_rvalue_reference<R (&&)(T0, T1, T2, T3, T4)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R>
struct is_rvalue_reference<R (&&)(...)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R, class T0>
struct is_rvalue_reference<R (&&)(T0, ...)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R, class T0, class T1>
struct is_rvalue_reference<R (&&)(T0, T1, ...)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R, class T0, class T1, class T2>
struct is_rvalue_reference<R (&&)(T0, T1, T2, ...)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R, class T0, class T1, class T2, class T3>
struct is_rvalue_reference<R (&&)(T0, T1, T2, T3, ...)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_rvalue_reference<R (&&)(T0, T1, T2, T3, T4, ...)> {
	static BURGER_CONSTEXPR const bool value = true;
};

template<class R>
struct is_rvalue_reference<R (&)()> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0>
struct is_rvalue_reference<R (&)(T0)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1>
struct is_rvalue_reference<R (&)(T0, T1)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2>
struct is_rvalue_reference<R (&)(T0, T1, T2)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2, class T3>
struct is_rvalue_reference<R (&)(T0, T1, T2, T3, ...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_rvalue_reference<R (&)(T0, T1, T2, T3, T4)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R>
struct is_rvalue_reference<R (&)(...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0>
struct is_rvalue_reference<R (&)(T0, ...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1>
struct is_rvalue_reference<R (&)(T0, T1, ...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2>
struct is_rvalue_reference<R (&)(T0, T1, T2, ...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2, class T3>
struct is_rvalue_reference<R (&)(T0, T1, T2, T3)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_rvalue_reference<R (&)(T0, T1, T2, T3, T4, ...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

#endif
#endif

// Implementation of is_reference
template<class T>
struct is_reference {
	static BURGER_CONSTEXPR const bool value =
		is_lvalue_reference<T>::value || is_rvalue_reference<T>::value;
};

// Implementation of is_signed and is_unsigned
#if !defined(DOXYGEN)
namespace details {
// Use C cast, Metrowerks fails with static_cast<>
template<class T>
struct is_signed_imp {
	static BURGER_CONSTEXPR const bool value =
		((typename remove_cv<T>::type) - 1) < ((typename remove_cv<T>::type)0);
};
}
#endif

template<class T>
struct is_signed {
	static BURGER_CONSTEXPR const bool value = is_floating_point<T>::value ||
		(is_integral<T>::value &&
			details::is_signed_imp<typename conditional<is_integral<T>::value,
				T, int>::type>::value);
};

template<class T>
struct is_unsigned {
	static BURGER_CONSTEXPR const bool value = is_integral<T>::value &&
		!details::is_signed_imp<
			typename conditional<is_integral<T>::value, T, int>::type>::value;
};

// Implementation of is_function

// PPC and X86 metrowerks has this test built in
#if (BURGER_METROWERKS >= 0x2500) && !defined(DOXYGEN)
namespace details {
template<class T>
struct is_function_impl {
	static BURGER_CONSTEXPR const bool value = __builtin_type(T) == 0x8000;
};
}

template<class T>
struct is_function {
	static BURGER_CONSTEXPR const bool value =
		details::is_function_impl<typename remove_cv<T>::type>::value;
};

// Clang/GNUC feature
#elif __has_builtin(__is_function) && !defined(DOXYGEN)
template<class T>
struct is_function {
	static BURGER_CONSTEXPR const bool value = __is_function(T);
};

// Clang/GNUC makes functions const references
#elif defined(BURGER_GNUC) || defined(BURGER_CLANG) && !defined(DOXYGEN)
template<class T>
struct is_function {
	static BURGER_CONSTEXPR const bool value =
		!(is_reference<T>::value || is_const<const T>::value);
};

#else

#if !defined(DOXYGEN)
// If more than 24 parameters are needed, reevaluate your life choices.
namespace type_traits {
no_type BURGER_ANSIAPI is_function_ptr_tester(...);

template<class R>
yes_type is_function_ptr_tester(R (*)());
template<class R, class T0>
yes_type is_function_ptr_tester(R (*)(T0));
template<class R, class T0, class T1>
yes_type is_function_ptr_tester(R (*)(T0, T1));
template<class R, class T0, class T1, class T2>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2));
template<class R, class T0, class T1, class T2, class T3>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3));
template<class R, class T0, class T1, class T2, class T3, class T4>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15>
yes_type is_function_ptr_tester(R (*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16>
yes_type is_function_ptr_tester(R (*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23, class T24>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24));

// These will confuse the Open Watcom 1.9 compiler
#if !defined(BURGER_WATCOM)
template<class R>
yes_type is_function_ptr_tester(R (*)(...));
template<class R, class T0>
yes_type is_function_ptr_tester(R (*)(T0...));
template<class R, class T0, class T1>
yes_type is_function_ptr_tester(R (*)(T0, T1...));
template<class R, class T0, class T1, class T2>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2...));
template<class R, class T0, class T1, class T2, class T3>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3...));
template<class R, class T0, class T1, class T2, class T3, class T4>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15>
yes_type is_function_ptr_tester(R (*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23>
yes_type is_function_ptr_tester(R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9,
	T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23...));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23, class T24>
yes_type is_function_ptr_tester(
	R (*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15,
		T16, T17, T18, T19, T20, T21, T22, T23, T24...));
#endif

// Test for Visual Studio calling conventions
#if defined(_MSC_EXTENSIONS)
template<class R>
yes_type is_function_ptr_tester(R(__cdecl*)());
template<class R, class T0>
yes_type is_function_ptr_tester(R(__cdecl*)(T0));
template<class R, class T0, class T1>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1));
template<class R, class T0, class T1, class T2>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2));
template<class R, class T0, class T1, class T2, class T3>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3));
template<class R, class T0, class T1, class T2, class T3, class T4>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4, T5));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8>
yes_type is_function_ptr_tester(
	R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9>
yes_type is_function_ptr_tester(
	R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
yes_type is_function_ptr_tester(
	R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11>
yes_type is_function_ptr_tester(
	R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12>
yes_type is_function_ptr_tester(
	R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13>
yes_type is_function_ptr_tester(
	R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14>
yes_type is_function_ptr_tester(R(__cdecl*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15>
yes_type is_function_ptr_tester(R(__cdecl*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16>
yes_type is_function_ptr_tester(R(__cdecl*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8,
	T9, T10, T11, T12, T13, T14, T15, T16, T17));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8,
	T9, T10, T11, T12, T13, T14, T15, T16, T17, T18));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8,
	T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8,
	T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8,
	T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8,
	T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23>
yes_type is_function_ptr_tester(R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8,
	T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23, class T24>
yes_type is_function_ptr_tester(
	R(__cdecl*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14,
		T15, T16, T17, T18, T19, T20, T21, T22, T23, T24));

template<class R>
yes_type is_function_ptr_tester(R(__stdcall*)());
template<class R, class T0>
yes_type is_function_ptr_tester(R(__stdcall*)(T0));
template<class R, class T0, class T1>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1));
template<class R, class T0, class T1, class T2>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2));
template<class R, class T0, class T1, class T2, class T3>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3));
template<class R, class T0, class T1, class T2, class T3, class T4>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3, T4));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3, T4, T5));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8>
yes_type is_function_ptr_tester(
	R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9>
yes_type is_function_ptr_tester(
	R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
yes_type is_function_ptr_tester(
	R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11>
yes_type is_function_ptr_tester(
	R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12>
yes_type is_function_ptr_tester(
	R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13>
yes_type is_function_ptr_tester(
	R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14>
yes_type is_function_ptr_tester(R(__stdcall*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15>
yes_type is_function_ptr_tester(R(__stdcall*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16>
yes_type is_function_ptr_tester(R(__stdcall*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22>
yes_type is_function_ptr_tester(R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23>
yes_type is_function_ptr_tester(
	R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13,
		T14, T15, T16, T17, T18, T19, T20, T21, T22, T23));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23, class T24>
yes_type is_function_ptr_tester(
	R(__stdcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13,
		T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24));

#if !defined(_MANAGED)
template<class R>
yes_type is_function_ptr_tester(R(__fastcall*)());
template<class R, class T0>
yes_type is_function_ptr_tester(R(__fastcall*)(T0));
template<class R, class T0, class T1>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1));
template<class R, class T0, class T1, class T2>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2));
template<class R, class T0, class T1, class T2, class T3>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3));
template<class R, class T0, class T1, class T2, class T3, class T4>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3, T4));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3, T4, T5));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8>
yes_type is_function_ptr_tester(
	R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9>
yes_type is_function_ptr_tester(
	R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
yes_type is_function_ptr_tester(
	R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11>
yes_type is_function_ptr_tester(
	R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12>
yes_type is_function_ptr_tester(
	R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13>
yes_type is_function_ptr_tester(
	R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14>
yes_type is_function_ptr_tester(R(__fastcall*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15>
yes_type is_function_ptr_tester(R(__fastcall*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16>
yes_type is_function_ptr_tester(R(__fastcall*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22>
yes_type is_function_ptr_tester(R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23>
yes_type is_function_ptr_tester(
	R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13,
		T14, T15, T16, T17, T18, T19, T20, T21, T22, T23));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23, class T24>
yes_type is_function_ptr_tester(
	R(__fastcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13,
		T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24));
#endif

#if defined(BURGER_SSE2) && (BURGER_MSVC >= 180000000)
template<class R>
yes_type is_function_ptr_tester(R(__vectorcall*)());
template<class R, class T0>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0));
template<class R, class T0, class T1>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1));
template<class R, class T0, class T1, class T2>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2));
template<class R, class T0, class T1, class T2, class T3>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2, T3));
template<class R, class T0, class T1, class T2, class T3, class T4>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2, T3, T4));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2, T3, T4, T5));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7>
yes_type is_function_ptr_tester(
	R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8>
yes_type is_function_ptr_tester(
	R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9>
yes_type is_function_ptr_tester(
	R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10>
yes_type is_function_ptr_tester(
	R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11>
yes_type is_function_ptr_tester(
	R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12>
yes_type is_function_ptr_tester(
	R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13>
yes_type is_function_ptr_tester(R(__vectorcall*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14>
yes_type is_function_ptr_tester(R(__vectorcall*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15>
yes_type is_function_ptr_tester(R(__vectorcall*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16>
yes_type is_function_ptr_tester(R(__vectorcall*)(
	T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15, T16));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22>
yes_type is_function_ptr_tester(R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7,
	T8, T9, T10, T11, T12, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23>
yes_type is_function_ptr_tester(
	R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13,
		T14, T15, T16, T17, T18, T19, T20, T21, T22, T23));
template<class R, class T0, class T1, class T2, class T3, class T4, class T5,
	class T6, class T7, class T8, class T9, class T10, class T11, class T12,
	class T13, class T14, class T15, class T16, class T17, class T18, class T19,
	class T20, class T21, class T22, class T23, class T24>
yes_type is_function_ptr_tester(
	R(__vectorcall*)(T0, T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13,
		T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24));
#endif

#endif
}

namespace details {
template<typename T>
struct is_function_impl {
	static T* t;
	static BURGER_CONSTEXPR const bool value =
		(sizeof(type_traits::is_function_ptr_tester(t)) ==
			sizeof(type_traits::yes_type));
};

template<typename T>
struct is_function_impl<T&> {
	static BURGER_CONSTEXPR const bool value = false;
};

#if defined(BURGER_RVALUE_REFERENCES)
template<typename T>
struct is_function_impl<T&&> {
	static BURGER_CONSTEXPR const bool value = false;
};
#endif
#endif
}

template<class T>
struct is_function {
	static BURGER_CONSTEXPR const bool value =
		details::is_function_impl<typename remove_cv<T>::type>::value;
};
#endif

#if (!defined(BURGER_MSVC) || (BURGER_MSVC >= 170000000)) && !defined(DOXYGEN)
template<class T>
struct is_function<T&> {
	static BURGER_CONSTEXPR const bool value = false;
};
#endif

#if defined(BURGER_RVALUE_REFERENCES) && !defined(DOXYGEN)
template<class T>
struct is_function<T&&> {
	static BURGER_CONSTEXPR const bool value = false;
};

#if defined(BURGER_MSVC) && (BURGER_MSVC <= 170000000)
template<class R>
struct is_function<R (&&)()> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0>
struct is_function<R (&&)(T0)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1>
struct is_function<R (&&)(T0, T1)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2>
struct is_function<R (&&)(T0, T1, T2)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2, class T3>
struct is_function<R (&&)(T0, T1, T2, T3)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_function<R (&&)(T0, T1, T2, T3, T4)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R>
struct is_function<R (&&)(...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0>
struct is_function<R (&&)(T0, ...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1>
struct is_function<R (&&)(T0, T1, ...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2>
struct is_function<R (&&)(T0, T1, T2, ...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2, class T3>
struct is_function<R (&&)(T0, T1, T2, T3, ...)> {
	static BURGER_CONSTEXPR const bool value = false;
};

template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_function<R (&&)(T0, T1, T2, T3, T4, ...)> {
	static BURGER_CONSTEXPR const bool value = false;
};
#endif
#endif

// Implementation of round_up_pointer
template<typename T>
T* round_up_pointer(
	T* pInput, uintptr_t uSize = alignment_of<T>::value) BURGER_NOEXCEPT
{
	return reinterpret_cast<T*>(
		(reinterpret_cast<uintptr_t>(pInput) + (uSize - 1)) & (~(uSize - 1)));
}

// Implementation of select_value
template<bool B, class T, T _True, T _False>
struct select_value {
	/** Encapsulated value */
	static BURGER_CONSTEXPR const T value = (B) ? _True : _False;
};

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

// Swap the contents of two memory variables
template<class T>
BURGER_INLINE void swap_variables(T* pA, T* pB)
{
	T tTemp(*pA);
	*pA = *pB;
	*pB = tTemp;
}

template<class T>
BURGER_INLINE void exchange(T& __A, T& __B)
{
	T tTemp = __A;
	__A = __B;
	__B = tTemp;
}

// Return the minimum value
template<typename T>
BURGER_INLINE BURGER_CONSTEXPR T minimum(T A, T B) BURGER_NOEXCEPT
{
	return ((A < B) ? A : B);
}

// Return the maximum value
template<typename T>
BURGER_INLINE BURGER_CONSTEXPR T maximum(T A, T B) BURGER_NOEXCEPT
{
	return ((A > B) ? A : B);
}

#if !defined(DOXYGEN)
#if defined(BURGER_PPC)
template<>
BURGER_INLINE float minimum(float fA, float fB)
{
	return static_cast<float>(__fsel((fA - fB), fB, fA));
}

template<>
BURGER_INLINE double minimum(double dA, double dB)
{
	return __fsel((dA - dB), dB, dA);
}

template<>
BURGER_INLINE float maximum(float fA, float fB)
{
	return static_cast<float>(__fsel((fA - fB), fA, fB));
}

template<>
BURGER_INLINE double maximum(double dA, double dB)
{
	return __fsel((dA - dB), dA, dB);
}

#elif defined(BURGER_SSE2)
template<>
BURGER_INLINE float minimum(float fA, float fB) BURGER_NOEXCEPT
{
	return _mm_cvtss_f32(_mm_min_ss(_mm_set_ss(fA), _mm_set_ss(fB)));
}

template<>
BURGER_INLINE double minimum(double dA, double dB) BURGER_NOEXCEPT
{
	return _mm_cvtsd_f64(_mm_min_sd(_mm_set_sd(dA), _mm_set_sd(dB)));
}

template<>
BURGER_INLINE float maximum(float fA, float fB) BURGER_NOEXCEPT
{
	return _mm_cvtss_f32(_mm_max_ss(_mm_set_ss(fA), _mm_set_ss(fB)));
}

template<>
BURGER_INLINE double maximum(double dA, double dB) BURGER_NOEXCEPT
{
	return _mm_cvtsd_f64(_mm_max_sd(_mm_set_sd(dA), _mm_set_sd(dB)));
}
#endif
#endif

}
#if defined(BURGER_GHS)
#pragma ghs endnowarning
#endif
/* END */

#endif
