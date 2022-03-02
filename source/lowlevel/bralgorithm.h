/***************************************

	Templates to support "algorithm"

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

namespace type_traits {
typedef char yes_type;

struct no_type {
	/** Literally not used. */
	char m_NotUsed[8];
};

template<uintptr_t T>
struct size_type {
};
}

template<bool B, class T, class F>
struct conditional {
	/** Type for false condition */
	typedef F type;
};

template<class T, class F>
struct conditional<true, T, F> {
	/** Type for true condition */
	typedef T type;
};

template<bool B, class T = void>
struct enable_if {
	typedef T type;
};

template<class T>
struct enable_if<false, T> {
};

template<bool B, class T = void>
struct disable_if {
	typedef T type;
};

template<class T>
struct disable_if<true, T> {
};

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

template<bool _Value>
struct bool_constant: integral_constant<bool, _Value> {
};

typedef bool_constant<true> true_type;
typedef bool_constant<false> false_type;

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

template<typename T, typename U>
struct is_same {
	/** If this instantiated, the types are not the same */
	static const bool value = false;
};

#if defined(BURGER_HAS_SFINAE) && !defined(DOXYGEN)
template<typename T>
struct is_same<T, T> {
	/** If this instantiated, the types are the same */
	static const bool value = true;
};
#endif

template<class T>
struct remove_const {
	typedef T type; ///< Type without const
};

#if !defined(DOXYGEN)
template<class T>
struct remove_const<T const> {
	typedef T type; ///< Type without const
};

#if !(defined(BURGER_METROWERKS) && defined(BURGER_X86))
template<class T>
struct remove_const<T const[]> {
	typedef T type[]; ///< Type without const
};
#endif

template<class T, uintptr_t N>
struct remove_const<T const[N]> {
	typedef T type[N]; ///< Type without const
};
#endif

template<class T>
struct remove_volatile {
	typedef T type; ///< Type without volatile
};

#if !defined(DOXYGEN)
template<class T>
struct remove_volatile<T volatile> {
	typedef T type; ///< Type without volatile
};

#if !(defined(BURGER_METROWERKS) && defined(BURGER_X86))
template<class T>
struct remove_volatile<T volatile[]> {
	typedef T type[]; ///< Type without volatile
};
#endif

template<class T, uintptr_t N>
struct remove_volatile<T volatile[N]> {
	typedef T type[N]; ///< Type without volatile
};
#endif

template<class T>
struct remove_cv {
	/** Type without volatile or const */
	typedef typename remove_volatile<typename remove_const<T>::type>::type type;
};

template<class T>
struct add_const {
	typedef T const type; ///< Type with const
};

#if !defined(DOXYGEN)
template<class T>
struct add_const<T&> {
	typedef T& type; ///< Type with const
};
#endif

template<class T>
struct add_volatile {
	typedef volatile T type; ///< Type with volatile
};

#if !defined(DOXYGEN)
template<class T>
struct add_volatile<T&> {
	typedef T& type; ///< Type with volatile
};
#endif

template<class T>
struct add_cv {
	typedef const volatile T type; ///< Type with const and volatile
};

#if !defined(DOXYGEN)
template<class T>
struct add_cv<T&> {
	typedef T& type; ///< Type with const and volatile
};
#endif

template<class T>
struct remove_reference {
	typedef T type; ///< Type with reference removed
};

#if !defined(DOXYGEN)
template<class T>
struct remove_reference<T&> {
	typedef T type; ///< Type with reference removed
};

#if defined(BURGER_RVALUE_REFERENCES)
template<class T>
struct remove_reference<T&&> {
	typedef T type; ///< Type with reference removed
};
#endif
#endif

template<class T>
struct is_const: false_type {
};

#if !defined(DOXYGEN)
template<class T>
struct is_const<const T>: true_type {
};
#endif

template<class T>
struct is_volatile: false_type {
};

#if !defined(DOXYGEN)
template<class T>
struct is_volatile<volatile T>: true_type {
};
#endif

template<class T>
struct is_void: false_type {
};

#if !defined(DOXYGEN)
template<>
struct is_void<void>: true_type {
};

template<>
struct is_void<const void>: true_type {
};

template<>
struct is_void<volatile void>: true_type {
};

template<>
struct is_void<const volatile void>: true_type {
};
#endif

template<class T>
struct is_floating_point: false_type {
};

#if !defined(DOXYGEN)
template<class T>
struct is_floating_point<volatile const T>: is_floating_point<T> {
};

template<class T>
struct is_floating_point<const T>: is_floating_point<T> {
};

template<class T>
struct is_floating_point<volatile T>: is_floating_point<T> {
};

template<>
struct is_floating_point<float>: true_type {
};

template<>
struct is_floating_point<double>: true_type {
};

template<>
struct is_floating_point<long double>: true_type {
};
#endif

template<class T>
struct is_integral: false_type {
};

#if !defined(DOXYGEN)
template<class T>
struct is_integral<const T>: is_integral<T> {
};

template<class T>
struct is_integral<volatile const T>: is_integral<T> {
};

template<class T>
struct is_integral<volatile T>: is_integral<T> {
};

template<>
struct is_integral<char>: true_type {
};

template<>
struct is_integral<signed char>: true_type {
};

template<>
struct is_integral<unsigned char>: true_type {
};

template<>
struct is_integral<short>: true_type {
};

template<>
struct is_integral<unsigned short>: true_type {
};

template<>
struct is_integral<int>: true_type {
};

template<>
struct is_integral<unsigned int>: true_type {
};

template<>
struct is_integral<long>: true_type {
};

template<>
struct is_integral<unsigned long>: true_type {
};

template<>
struct is_integral<long long>: true_type {
};

template<>
struct is_integral<unsigned long long>: true_type {
};

template<>
struct is_integral<bool>: true_type {
};

#if defined(BURGER_HAS_CHAR8_T)
template<>
struct is_integral<char8_t>: true_type {
};
#endif

#if defined(BURGER_HAS_WCHAR_T)
template<>
struct is_integral<wchar_t>: true_type {
};
#endif

#if defined(BURGER_HAS_CHAR16_T)
template<>
struct is_integral<char16_t>: true_type {
};

template<>
struct is_integral<char32_t>: true_type {
};
#endif
#endif

template<class T>
struct is_arithmetic
	: bool_constant<is_integral<T>::value || is_floating_point<T>::value> {
};

template<class T>
struct is_pointer: false_type {
};

#if !defined(DOXYGEN)
template<class T>
struct is_pointer<T*>: true_type {
};

template<class T>
struct is_pointer<T* const>: true_type {
};

template<class T>
struct is_pointer<T* volatile>: true_type {
};

template<class T>
struct is_pointer<T* const volatile>: true_type {
};

template<class T>
struct is_pointer<T const>: is_pointer<T> {
};

template<class T>
struct is_pointer<T volatile>: is_pointer<T> {
};

template<class T>
struct is_pointer<T const volatile>: is_pointer<T> {
};
#endif

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

// Workaround MSVC 2005-2013 bug on edge case for T (*)(X) where the types are
// different.
#if defined(BURGER_MSVC) && (BURGER_MSVC < 190000000)

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
	typedef T type;
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

template<class T>
struct is_lvalue_reference: false_type {
};

template<class T>
struct is_lvalue_reference<T&>: true_type {
};

template<class T>
struct is_rvalue_reference: false_type {
};

#if defined(BURGER_RVALUE_REFERENCES) && !defined(DOXYGEN)
template<class T>
struct is_rvalue_reference<T&&>: true_type {
};

#if (defined(BURGER_MSVC) && (BURGER_MSVC <= 170000000)) && !defined(DOXYGEN)
template<class R>
struct is_rvalue_reference<R(&&)()>: true_type {
};
template<class R, class T0>
struct is_rvalue_reference<R(&&)(T0)>: true_type {
};
template<class R, class T0, class T1>
struct is_rvalue_reference<R(&&)(T0, T1)>: true_type {
};
template<class R, class T0, class T1, class T2>
struct is_rvalue_reference<R(&&)(T0, T1, T2)>: true_type {
};
template<class R, class T0, class T1, class T2, class T3>
struct is_rvalue_reference<R(&&)(T0, T1, T2, T3)>: true_type {
};
template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_rvalue_reference<R(&&)(T0, T1, T2, T3, T4)>: true_type {
};

template<class R>
struct is_rvalue_reference<R(&&)(...)>: true_type {
};
template<class R, class T0>
struct is_rvalue_reference<R(&&)(T0, ...)>: true_type {
};
template<class R, class T0, class T1>
struct is_rvalue_reference<R(&&)(T0, T1, ...)>: true_type {
};
template<class R, class T0, class T1, class T2>
struct is_rvalue_reference<R(&&)(T0, T1, T2, ...)>: true_type {
};
template<class R, class T0, class T1, class T2, class T3>
struct is_rvalue_reference<R(&&)(T0, T1, T2, T3, ...)>: true_type {
};
template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_rvalue_reference<R(&&)(T0, T1, T2, T3, T4, ...)>: true_type {
};

template<class R>
struct is_rvalue_reference<R (&)()>: false_type {
};
template<class R, class T0>
struct is_rvalue_reference<R (&)(T0)>: false_type {
};
template<class R, class T0, class T1>
struct is_rvalue_reference<R (&)(T0, T1)>: false_type {
};
template<class R, class T0, class T1, class T2>
struct is_rvalue_reference<R (&)(T0, T1, T2)>: false_type {
};
template<class R, class T0, class T1, class T2, class T3>
struct is_rvalue_reference<R (&)(T0, T1, T2, T3, ...)>: false_type {
};
template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_rvalue_reference<R (&)(T0, T1, T2, T3, T4)>: false_type {
};

template<class R>
struct is_rvalue_reference<R (&)(...)>: false_type {
};
template<class R, class T0>
struct is_rvalue_reference<R (&)(T0, ...)>: false_type {
};
template<class R, class T0, class T1>
struct is_rvalue_reference<R (&)(T0, T1, ...)>: false_type {
};
template<class R, class T0, class T1, class T2>
struct is_rvalue_reference<R (&)(T0, T1, T2, ...)>: false_type {
};
template<class R, class T0, class T1, class T2, class T3>
struct is_rvalue_reference<R (&)(T0, T1, T2, T3)>: false_type {
};
template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_rvalue_reference<R (&)(T0, T1, T2, T3, T4, ...)>: false_type {
};

#endif

#endif

template<class T>
struct is_reference: bool_constant<is_lvalue_reference<T>::value ||
						 is_rvalue_reference<T>::value> {
};

#if !defined(DOXYGEN)
template<class T>
struct _signed_val_test: bool_constant<((typename remove_cv<T>::type) - 1) <
							 ((typename remove_cv<T>::type)0)> {
};
#endif

template<class T>
struct is_signed
	: bool_constant<is_floating_point<T>::value ||
		  (is_integral<T>::value &&
			  _signed_val_test<typename conditional<is_integral<T>::value, T,
				  int>::type>::value)> {
};

template<class T>
struct is_unsigned
	: bool_constant<is_integral<T>::value &&
		  !_signed_val_test<typename conditional<is_integral<T>::value, T,
			  int>::type>::value> {
};

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

template<typename T>
struct is_function_impl {
	static T* t;
	static const bool value = (sizeof(type_traits::is_function_ptr_tester(t)) ==
		sizeof(type_traits::yes_type));
};

template<typename T>
struct is_function_impl<T&>: false_type {
};

#if defined(BURGER_RVALUE_REFERENCES)
template<typename T>
struct is_function_impl<T&&>: false_type {
};
#endif
#endif

template<class T>
struct is_function: integral_constant<bool, is_function_impl<T>::value> {
};

#if (!defined(BURGER_MSVC) || (BURGER_MSVC >= 170000000)) && !defined(DOXYGEN)
template<class T>
struct is_function<T&>: public false_type {
};
#endif

#if defined(BURGER_RVALUE_REFERENCES) && !defined(DOXYGEN)
template<class T>
struct is_function<T&&>: public false_type {
};

#if defined(BURGER_MSVC) && (BURGER_MSVC <= 170000000) && !defined(DOXYGEN)
template<class R>
struct is_function<R(&&)()>: false_type {
};
template<class R, class T0>
struct is_function<R(&&)(T0)>: false_type {
};
template<class R, class T0, class T1>
struct is_function<R(&&)(T0, T1)>: false_type {
};
template<class R, class T0, class T1, class T2>
struct is_function<R(&&)(T0, T1, T2)>: false_type {
};
template<class R, class T0, class T1, class T2, class T3>
struct is_function<R(&&)(T0, T1, T2, T3)>: false_type {
};
template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_function<R(&&)(T0, T1, T2, T3, T4)>: false_type {
};
template<class R>
struct is_function<R(&&)(...)>: false_type {
};
template<class R, class T0>
struct is_function<R(&&)(T0, ...)>: false_type {
};
template<class R, class T0, class T1>
struct is_function<R(&&)(T0, T1, ...)>: false_type {
};
template<class R, class T0, class T1, class T2>
struct is_function<R(&&)(T0, T1, T2, ...)>: false_type {
};
template<class R, class T0, class T1, class T2, class T3>
struct is_function<R(&&)(T0, T1, T2, T3, ...)>: false_type {
};
template<class R, class T0, class T1, class T2, class T3, class T4>
struct is_function<R(&&)(T0, T1, T2, T3, T4, ...)>: false_type {
};
#endif
#endif

#if 0
template<class T>
struct make_signed {
private:
    // The template argument to make_signed must be an integer or enum type.
    BURGER_STATIC_ASSERT(is_integral<T>::value /* || is_enum<T>::value */);
    // The template argument to make_signed must not be the type bool.
    BURGER_STATIC_ASSERT(!(is_same<typename remove_cv<T>::type, bool>::value));

    typedef typename remove_cv<T>::type t_no_cv;
    typedef typename conditional<(is_signed<T>::value &&
                                     is_integral<T>::value &&
                                     !is_same<t_no_cv, char>::value &&
                                     !is_same<t_no_cv, wchar_t>::value &&
                                     !is_same<t_no_cv, bool>::value),
        T,
        typename conditional<(is_integral<T>::value &&
                                 !is_same<t_no_cv, char>::value &&
                                 !is_same<t_no_cv, wchar_t>::value &&
                                 !is_same<t_no_cv, bool>::value),
            typename conditional<is_same<t_no_cv, unsigned char>::value,
                signed char,
                typename conditional<is_same<t_no_cv, unsigned short>::value,
                    signed short,
                    typename conditional<is_same<t_no_cv, unsigned int>::value,
                        int,
                        typename conditional<
                            is_same<t_no_cv, unsigned long>::value, long,
                            int64_t>::type>::type>::type>::type,
            // Not a regular integer type:
            typename conditional<sizeof(t_no_cv) == sizeof(unsigned char),
                signed char,
                typename conditional<sizeof(t_no_cv) == sizeof(unsigned short),
                    signed short,
                    typename conditional<
                        sizeof(t_no_cv) == sizeof(unsigned int), int,
                        typename conditional<
                            sizeof(t_no_cv) == sizeof(unsigned long), long,
                            int64_t>::type>::type>::type>::type>::type>::type
        base_integer_type;

    // Add back any const qualifier:
    typedef typename conditional<is_const<T>::value,
        typename add_const<base_integer_type>::type, base_integer_type>::type
        const_base_integer_type;

public:
    // Add back any volatile qualifier:
    typedef typename conditional<is_volatile<T>::value,
        typename add_volatile<const_base_integer_type>::type,
        const_base_integer_type>::type type;
};
#endif

template<typename T>
T* round_up_pointer(
	T* pInput, uintptr_t uSize = alignment_of<T>::value) BURGER_NOEXCEPT
{
	return reinterpret_cast<T*>(
		(reinterpret_cast<uintptr_t>(pInput) + (uSize - 1)) & (~(uSize - 1)));
}

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
BURGER_INLINE BURGER_CONSTEXPR T Min(T A, T B) BURGER_NOEXCEPT
{
	return ((A < B) ? A : B);
}

// Return the maximum value
template<typename T>
BURGER_INLINE BURGER_CONSTEXPR T Max(T A, T B) BURGER_NOEXCEPT
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
