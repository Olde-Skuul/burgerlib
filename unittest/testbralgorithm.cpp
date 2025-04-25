/***************************************

	Unit tests for the algorithm template library

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbralgorithm.h"
#include "bralgorithm.h"
#include "brfloatingpoint.h"
#include "brnumberstringhex.h"
#include "brstring.h"
#include "brstringfunctions.h"
#include "common.h"

// Get rid of a spurious warning about the use of sizeof()
#if defined(BURGER_GHS)
#pragma ghs nowarning 1931
#endif

#if defined(BURGER_WATCOM)
// Unreachable code
#pragma warning 13 9
// No reference to symbol in structure
#pragma warning 14 9
#endif

struct undefined_struct;

typedef void call_type1();
typedef void call_type2(int);
typedef void call_type3(int&, double);
typedef void call_type4(bool&, short, int, int);
typedef void call_type5(int, bool, int*, int[], int, int, int, int, int);
typedef void (*call_ptr1)(void);
typedef int (*call_ptr2)(int);
typedef float (*call_ptr3)(int, float);

#if defined(_MSC_EXTENSIONS)
typedef void __stdcall stdcall_function0_t();
typedef void __stdcall stdcall_function1_t(int);
typedef void __stdcall stdcall_function2_t(int&, double);
typedef void __stdcall stdcall_function3_t(int&, bool, int, int);
typedef void __stdcall stdcall_function4_t(
	int, bool, int*, int[], int, int, int, int, int);

typedef void __cdecl cdecl_function0_t();
typedef void __cdecl cdecl_function1_t(int);
typedef void __cdecl cdecl_function2_t(int&, double);
typedef void __cdecl cdecl_function3_t(int&, bool, int, int);
typedef void __cdecl cdecl_function4_t(
	int, bool, int*, int[], int, int, int, int, int);

#if !defined(_MANAGED)
typedef void __fastcall fastcall_function0_t();
typedef void __fastcall fastcall_function1_t(int);
typedef void __fastcall fastcall_function2_t(int&, double);
typedef void __fastcall fastcall_function3_t(int&, bool, int, int);
typedef void __fastcall fastcall_function4_t(
	int, bool, int*, int[], int, int, int, int, int);
#endif

#if defined(BURGER_SSE2) && (BURGER_MSVC >= 180000000)
typedef void __vectorcall vectorcall_function0_t();
typedef void __vectorcall vectorcall_function1_t(int);
typedef void __vectorcall vectorcall_function2_t(int&, double);
typedef void __vectorcall vectorcall_function3_t(int&, bool, int, int);
typedef void __vectorcall vectorcall_function4_t(
	int, bool, int*, int[], int, int, int, int, int);
#endif

#endif

struct fake_pointer_operator {
	operator void*() BURGER_NOEXCEPT
	{
		return this;
	}
};

struct IntTest32x32_t {
	int32_t m_iInput1; // Source value 1
	int32_t m_iInput2; // Source value 2
	int32_t m_iOutput; // Expected output
};

struct UIntTest32x32_t {
	uint32_t m_uInput1; // Source value 1
	uint32_t m_uInput2; // Source value 2
	uint32_t m_uOutput; // Expected output
};

struct IntTest64x64_t {
	int64_t m_iInput1; // Source value 1
	int64_t m_iInput2; // Source value 2
	int64_t m_iOutput; // Expected output
};

struct UIntTest64x64_t {
	uint64_t m_uInput1; // Source value 1
	uint64_t m_uInput2; // Source value 2
	uint64_t m_uOutput; // Expected output
};

struct FloatTest_t {
	Burger::uint32_float_t m_Input1; // Source value 1
	Burger::uint32_float_t m_Input2; // Source value 2
	Burger::uint32_float_t m_Output; // Expected output
};

struct DoubleTest_t {
	Burger::uint64_double_t m_Input1; // Source value 1
	Burger::uint64_double_t m_Input2; // Source value 2
	Burger::uint64_double_t m_Output; // Expected output
};

//
// Test minimum(int32_t)
//

static const IntTest32x32_t MinTestTableInt32[] = {
	{0x00000000, 0x00000001, 0x00000000}, {0x00000001, 0x00000000, 0x00000000},
	{0x00000000, ~0x7FFFFFFF, ~0x7FFFFFFF},
	{~0x7FFFFFFF, 0x00000000, ~0x7FFFFFFF},
	{~0x7FFFFFFF, ~0x7FFFFFFF, ~0x7FFFFFFF},
	{0x00000000, 0x7FFFFFFF, 0x00000000}, {0x7FFFFFFF, 0x00000000, 0x00000000},
	{0x7FFFFFFF, ~0x7FFFFFFF, ~0x7FFFFFFF},
	{0x40000000, -0x40000000, -0x40000000},
	{-0x7FFFFFFF, 0x7FFFFFFF, -0x7FFFFFFF},
	{0x3FFFFFFF, -0x41000000, -0x41000000},
	{-0x00000001, 0x00000001, -0x00000001}};

static uint_t BURGER_API TestMinInt32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32x32_t* pWork = MinTestTableInt32;
	uintptr_t uCount = BURGER_ARRAYSIZE(MinTestTableInt32);
	do {
		const int32_t iReturn = Burger::minimum(pWork->m_iInput1, pWork->m_iInput2);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::minimumMin((int32_t)0x%08X,(int32_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput1, pWork->m_iInput2, iReturn,
			pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test minimum(int64_t)
//

static const IntTest64x64_t MinTestTableInt64[] = {
	{0x0000000000000000LL, 0x0000000000000001LL, 0x0000000000000000LL},
	{0x0000000000000001LL, 0x0000000000000000LL, 0x0000000000000000LL},
	{0x0000000000000000LL, ~0x7FFFFFFFFFFFFFFFLL, ~0x7FFFFFFFFFFFFFFFLL},
	{~0x7FFFFFFFFFFFFFFFLL, 0x0000000000000000LL, ~0x7FFFFFFFFFFFFFFFLL},
	{~0x7FFFFFFFFFFFFFFFLL, ~0x7FFFFFFFFFFFFFFFLL, ~0x7FFFFFFFFFFFFFFFLL},
	{0x0000000000000000LL, 0x7FFFFFFFFFFFFFFFLL, 0x0000000000000000LL},
	{0x7FFFFFFFFFFFFFFFLL, 0x0000000000000000LL, 0x0000000000000000LL},
	{0x7FFFFFFFFFFFFFFFLL, ~0x7FFFFFFFFFFFFFFFLL, ~0x7FFFFFFFFFFFFFFFLL},
	{0x4000000000000000LL, -0x4000000000000000LL, -0x4000000000000000LL},
	{-0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFFFFFFFFFFLL, -0x7FFFFFFFFFFFFFFFLL},
	{0x3FFFFFFFFFFFFFFFLL, -0x4100000000000000LL, -0x4100000000000000LL},
	{-0x0000000000000001LL, 0x0000000000000001LL, -0x0000000000000001LL}};

static uint_t BURGER_API TestMinInt64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest64x64_t* pWork = MinTestTableInt64;
	uintptr_t uCount = BURGER_ARRAYSIZE(MinTestTableInt64);
	do {
		const int64_t iReturn = Burger::minimum(pWork->m_iInput1, pWork->m_iInput2);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		if (uTest) {
			Burger::String Text(
				"Burger::minimum((int64_t)0x%016llX,(int64_t)0x%016llX) = 0x%016llX, expected 0x%016llX",
				pWork->m_iInput1, pWork->m_iInput2, iReturn, pWork->m_iOutput);
			ReportFailure(Text.c_str(), uTest);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test minimum(uint32_t)
//

static const UIntTest32x32_t MinTestTableUInt32[] = {
	{0x00000000U, 0x00000001U, 0x00000000U},
	{0x00000001U, 0x00000000U, 0x00000000U},
	{0x00000000U, 0x80000000U, 0x00000000U},
	{0x80000000U, 0x00000000U, 0x00000000U},
	{0x80000000U, 0x80000000U, 0x80000000U},
	{0x00000000U, 0x7FFFFFFFU, 0x00000000U},
	{0x7FFFFFFFU, 0x00000000U, 0x00000000U},
	{0x7FFFFFFFU, 0x80000000U, 0x7FFFFFFFU},
	{0x40000000U, 0xC0000000U, 0x40000000U},
	{0x80000001U, 0x7FFFFFFFU, 0x7FFFFFFFU},
	{0x3FFFFFFFU, 0xBF000000U, 0x3FFFFFFFU},
	{0xFFFFFFFFU, 0x00000001U, 0x00000001U}};

static uint_t BURGER_API TestMinUInt32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const UIntTest32x32_t* pWork = MinTestTableUInt32;
	uintptr_t uCount = BURGER_ARRAYSIZE(MinTestTableUInt32);
	do {
		const uint32_t uReturn =
			Burger::minimum(pWork->m_uInput1, pWork->m_uInput2);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::minimum((uint32_t)0x%08X,(uint32_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_uInput1, pWork->m_uInput2, uReturn,
			pWork->m_uOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test minimum(uint64_t)
//

static const UIntTest64x64_t MinTestTableUInt64[] = {
	{0x0000000000000000ULL, 0x0000000000000001ULL, 0x0000000000000000ULL},
	{0x0000000000000001ULL, 0x0000000000000000ULL, 0x0000000000000000ULL},
	{0x0000000000000000ULL, 0x8000000000000000ULL, 0x0000000000000000ULL},
	{0x8000000000000000ULL, 0x0000000000000000ULL, 0x0000000000000000ULL},
	{0x8000000000000000ULL, 0x8000000000000000ULL, 0x8000000000000000ULL},
	{0x0000000000000000ULL, 0x7FFFFFFFFFFFFFFFULL, 0x0000000000000000ULL},
	{0x7FFFFFFFFFFFFFFFULL, 0x0000000000000000ULL, 0x0000000000000000ULL},
	{0x7FFFFFFFFFFFFFFFULL, 0x8000000000000000ULL, 0x7FFFFFFFFFFFFFFFULL},
	{0x4000000000000000ULL, 0xC000000000000000ULL, 0x4000000000000000ULL},
	{0x8000000000000001ULL, 0x7FFFFFFFFFFFFFFFULL, 0x7FFFFFFFFFFFFFFFULL},
	{0x3FFFFFFFFFFFFFFFULL, 0xBF00000000000000ULL, 0x3FFFFFFFFFFFFFFFULL},
	{0xFFFFFFFFFFFFFFFFULL, 0x0000000000000001ULL, 0x0000000000000001ULL}};

static uint_t BURGER_API TestMinUInt64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const UIntTest64x64_t* pWork = MinTestTableUInt64;
	uintptr_t uCount = BURGER_ARRAYSIZE(MinTestTableUInt64);
	do {
		const uint64_t uReturn =
			Burger::minimum(pWork->m_uInput1, pWork->m_uInput2);
		const uint_t uTest = uReturn != pWork->m_uOutput;
		uFailure |= uTest;
		if (uTest) {
			Burger::String Text(
				"Burger::minimum((int64_t)0x%016llX,(int64_t)0x%016llX) = 0x%016llX, expected 0x%016llX",
				pWork->m_uInput1, pWork->m_uInput2, uReturn, pWork->m_uOutput);
			ReportFailure(Text.c_str(), uTest);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test minimum(float)
//

static const FloatTest_t MinTestTableFloat[] = {
	{0x00000000U, 0x3F800000U, 0x00000000U}, // 0,1,0
	{0x3F800000U, 0x00000000U, 0x00000000U}, // 1,0,0
	{0x7F800000U, 0x3F800000U, 0x3F800000U}, // Inf,1,1
	{0x3F800000U, 0x7F800000U, 0x3F800000U}, // 1,Inf,1
	{0xFF800000U, 0x3F800000U, 0xFF800000U}, // -Inf,1,-Inf
	{0x3F800000U, 0xFF800000U, 0xFF800000U}, // 1,-Inf,-Inf
	//  {0x7FBFFFFFU, 0x00000000U, 0x00000000U}, // Nan,0,0
	//  {0x00000000U, 0x7FBFFFFFU, 0x7FBFFFFFU}, // 0,Nan,Nan
	{0xFF800000U, 0x3F800000U, 0xFF800000U}, // -Inf,Inf,-Inf
	{0x3F800000U, 0xFF800000U, 0xFF800000U}, // Inf,-Inf,-Inf
	{0x3F800000U, 0xBF800000U, 0xBF800000U}, // 1,-1,-1
	{0xBF800000U, 0x3F800000U, 0xBF800000U}  // -1,1,-1
};

static uint_t BURGER_API TestMinFloat(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const FloatTest_t* pWork = MinTestTableFloat;
	uintptr_t uCount = BURGER_ARRAYSIZE(MinTestTableFloat);
	do {
		Burger::uint32_float_t Return;
		Return.f = Burger::minimum(pWork->m_Input1.f, pWork->m_Input2.f);
		const uint_t uTest = Return.w != pWork->m_Output.w;
		uFailure |= uTest;
		ReportFailure(
			"Burger::minimum((float)0x%08X,(float)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_Input1.w, pWork->m_Input2.w, Return.w,
			pWork->m_Output.w);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test maximum(int32_t)
//

static const IntTest32x32_t MaxTestTableInt32[] = {
	{0x00000000, 0x00000001, 0x00000001}, {0x00000001, 0x00000000, 0x00000001},
	{0x00000000, ~0x7FFFFFFF, 0x00000000},
	{~0x7FFFFFFF, 0x00000000, 0x00000000},
	{~0x7FFFFFFF, ~0x7FFFFFFF, ~0x7FFFFFFF},
	{0x00000000, 0x7FFFFFFF, 0x7FFFFFFF}, {0x7FFFFFFF, 0x00000000, 0x7FFFFFFF},
	{0x7FFFFFFF, ~0x7FFFFFFF, 0x7FFFFFFF},
	{0x40000000, -0x40000000, 0x40000000},
	{-0x7FFFFFFF, 0x7FFFFFFF, 0x7FFFFFFF},
	{0x3FFFFFFF, -0x41000000, 0x3FFFFFFF},
	{-0x00000001, 0x00000001, 0x00000001}};

static uint_t BURGER_API TestMaxInt32(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest32x32_t* pWork = MaxTestTableInt32;
	uintptr_t uCount = BURGER_ARRAYSIZE(MaxTestTableInt32);
	do {
		const int32_t iReturn = Burger::maximum(pWork->m_iInput1, pWork->m_iInput2);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		ReportFailure(
			"Burger::maximum((int32_t)0x%08X,(int32_t)0x%08X) = 0x%08X, expected 0x%08X",
			uTest, pWork->m_iInput1, pWork->m_iInput2, iReturn,
			pWork->m_iOutput);
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test maximum(int64_t)
//

static const IntTest64x64_t MaxTestTableInt64[] = {
	{0x0000000000000000LL, 0x0000000000000001LL, 0x0000000000000001LL},
	{0x0000000000000001LL, 0x0000000000000000LL, 0x0000000000000001LL},
	{0x0000000000000000LL, ~0x7FFFFFFFFFFFFFFFLL, 0x0000000000000000LL},
	{~0x7FFFFFFFFFFFFFFFLL, 0x0000000000000000LL, 0x0000000000000000LL},
	{~0x7FFFFFFFFFFFFFFFLL, ~0x7FFFFFFFFFFFFFFFLL, ~0x7FFFFFFFFFFFFFFFLL},
	{0x0000000000000000LL, 0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFFFFFFFFFFLL},
	{0x7FFFFFFFFFFFFFFFLL, 0x0000000000000000LL, 0x7FFFFFFFFFFFFFFFLL},
	{0x7FFFFFFFFFFFFFFFLL, ~0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFFFFFFFFFFLL},
	{0x4000000000000000LL, -0x4000000000000000LL, 0x4000000000000000LL},
	{-0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFFFFFFFFFFLL, 0x7FFFFFFFFFFFFFFFLL},
	{0x3FFFFFFFFFFFFFFFLL, -0x4100000000000000LL, 0x3FFFFFFFFFFFFFFFLL},
	{-0x0000000000000001LL, 0x0000000000000001LL, 0x0000000000000001LL}};

static uint_t BURGER_API TestMaxInt64(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const IntTest64x64_t* pWork = MaxTestTableInt64;
	uintptr_t uCount = BURGER_ARRAYSIZE(MaxTestTableInt64);
	do {
		const int64_t iReturn = Burger::maximum(pWork->m_iInput1, pWork->m_iInput2);
		const uint_t uTest = iReturn != pWork->m_iOutput;
		uFailure |= uTest;
		if (uTest) {
			Burger::String Text(
				"Burger::maximum((int64_t)0x%016llX,(int64_t)0x%016llX) = 0x%016llX, expected 0x%016llX",
				pWork->m_iInput1, pWork->m_iInput2, iReturn, pWork->m_iOutput);
			ReportFailure(Text.c_str(), uTest);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test enable_if<> and disable_if<>

***************************************/

// BECKY TODO, fix watcom for enable_if, Open Watcom 1.9 crashes
// when using enable_if<>

#if !defined(BURGER_WATCOM)
template<typename T>
typename Burger::enable_if<Burger::is_floating_point<T>::value,
	Burger::type_traits::yes_type>::type is_it_fp(T);

template<typename T>
typename Burger::disable_if<Burger::is_floating_point<T>::value,
	Burger::type_traits::no_type>::type is_it_fp(T);

#endif

static uint_t BURGER_API Test_enable_if(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

#if !defined(BURGER_68K) && !defined(BURGER_WATCOM)
	// Test if float not detected
	uint_t uTest =
		sizeof(is_it_fp(1.0f)) == sizeof(Burger::type_traits::no_type);
	uFailure |= uTest;
	ReportFailure("sizeof(is_it_fp(1.0f)) = %u", uTest,
		sizeof(Burger::type_traits::no_type));

	const int64_t* pLong = nullptr;
	uTest = sizeof(is_it_fp(*pLong)) == sizeof(Burger::type_traits::yes_type);
	uFailure |= uTest;
	ReportFailure("sizeof(is_it_fp(*pLong)) = %u", uTest,
		sizeof(Burger::type_traits::yes_type));

	// Test if float detected
	uTest = sizeof(is_it_fp(static_cast<uint32_t>(1))) ==
		sizeof(Burger::type_traits::yes_type);
	uFailure |= uTest;
	ReportFailure("sizeof(is_it_fp(static_cast<uint32_t>(1))) = %u", uTest,
		sizeof(Burger::type_traits::yes_type));

	uTest = sizeof(is_it_fp(1.0)) == sizeof(Burger::type_traits::no_type);
	uFailure |= uTest;
	ReportFailure("sizeof(is_it_fp(1.0)) = %u", uTest,
		sizeof(Burger::type_traits::no_type));

	const uint8_t* pU8 = nullptr;
	uTest = sizeof(is_it_fp(*pU8)) == sizeof(Burger::type_traits::yes_type);
	uFailure |= uTest;
	ReportFailure("sizeof(is_it_fp(*pU8)) = %u", uTest,
		sizeof(Burger::type_traits::yes_type));
#endif
	return uFailure;
}

/***************************************

	Test integral_constant<>

***************************************/

static uint_t BURGER_API Test_integral_constant(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	uint_t uTest = Burger::true_type::value != TRUE;
	uFailure |= uTest;
	ReportFailure(
		"Burger::true_type::value = %d", uTest, int(Burger::true_type::value));

	uTest = Burger::false_type::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::false_type::value = %d", uTest,
		int(Burger::false_type::value));

	typedef Burger::integral_constant<int, 2> two_t;
	typedef Burger::integral_constant<int, 4> four_t;

	uTest = two_t::value != 2;
	uFailure |= uTest;
	ReportFailure("Burger::integral_constant<int, 2>::value = %d", uTest,
		int(two_t::value));

	uTest = four_t::value != 4;
	uFailure |= uTest;
	ReportFailure("Burger::integral_constant<int, 4>::value = %d", uTest,
		int(four_t::value));

	return uFailure;
}

/***************************************

	Test is_same<>

***************************************/

// conditional expression is constant
BURGER_MSVC_PUSH_DISABLE_WARNING(4127)

#define TEST_IS_SAME(input1, input2, result) \
	if (Burger::is_same<input1, input2>::value != result) { \
		uFailure |= 1; \
		ReportFailure("Burger::is_same<" #input1 ", " #input2 \
					  ">::value is not %d", \
			1, static_cast<int>(result)); \
	}

static uint_t BURGER_API Test_is_same(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS_SAME(int, int, true)
	TEST_IS_SAME(int&, int&, true)
	TEST_IS_SAME(void*, void*, true)

	TEST_IS_SAME(int, int[2], false)
	TEST_IS_SAME(int, double, false)
	TEST_IS_SAME(double, double, true)
	TEST_IS_SAME(IntTest64x64_t, IntTest64x64_t, true)
	TEST_IS_SAME(IntTest64x64_t, char, false)

	// Test for const and volatile
	TEST_IS_SAME(int, const int, false)
	TEST_IS_SAME(int, volatile int, false)
	TEST_IS_SAME(int, const volatile int, false)
	TEST_IS_SAME(const int*, const int*, true)

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS_SAME(int, int&&, false)
	TEST_IS_SAME(int&, int&&, false)
	TEST_IS_SAME(int&&, int&&, true)
#endif

	return uFailure;
}

/***************************************

	Test conditional<>

***************************************/

#define TEST_IS_CONDITIONAL(input1, input2, input3, input4, result) \
	if (Burger::is_same<Burger::conditional<input1, input2, input3>::type, \
			input4>::value != result) { \
		uFailure |= 1; \
		ReportFailure("Burger::conditional<" #input1 ", " #input2 ", " #input3 \
					  ">::type is not " #input4, \
			1, static_cast<int>(result)); \
	}
static uint_t BURGER_API Test_conditional(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS_CONDITIONAL(true, char, long, char, true)
	TEST_IS_CONDITIONAL(false, char, long, long, true)
	TEST_IS_CONDITIONAL(true, char, long, long, false)
	TEST_IS_CONDITIONAL(false, char, long, char, false)
	return uFailure;
}

/***************************************

	Test remove_const<>

***************************************/

#define CHECK_IS_SAME_TYPE(function, input, result) \
	if (!Burger::is_same<Burger::function<input>::type, result>::value) { \
		uFailure |= 1; \
		ReportFailure("Burger::" #function "<" #input "> != %s", 1, #result); \
	}

#define CHECK_ADD_REMOVE(function, input, result) \
	CHECK_IS_SAME_TYPE(function, bool input, bool result) \
	CHECK_IS_SAME_TYPE(function, char input, char result) \
	CHECK_IS_SAME_TYPE(function, signed char input, signed char result) \
	CHECK_IS_SAME_TYPE(function, unsigned char input, unsigned char result) \
	CHECK_IS_SAME_TYPE(function, short input, short result) \
	CHECK_IS_SAME_TYPE(function, int input, int result) \
	CHECK_IS_SAME_TYPE(function, long input, long result) \
	CHECK_IS_SAME_TYPE(function, long long input, long long result) \
	CHECK_IS_SAME_TYPE(function, float input, float result) \
	CHECK_IS_SAME_TYPE(function, double input, double result) \
	CHECK_IS_SAME_TYPE(function, IntTest32x32_t input, IntTest32x32_t result) \
	CHECK_IS_SAME_TYPE(function, Burger::eError input, Burger::eError result)

static uint_t BURGER_API Test_remove_const(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	CHECK_ADD_REMOVE(
		remove_const, BURGER_NULL_MACRO_PARAM, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_const, const, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_const, volatile, volatile)
	CHECK_ADD_REMOVE(remove_const, const volatile, volatile)

	CHECK_ADD_REMOVE(remove_const, *, *)
	CHECK_ADD_REMOVE(remove_const, * const, *)
	CHECK_ADD_REMOVE(remove_const, * volatile, * volatile)
	CHECK_ADD_REMOVE(remove_const, * const volatile, * volatile)
	CHECK_ADD_REMOVE(remove_const, const*, const*)
	CHECK_ADD_REMOVE(remove_const, volatile*, volatile*)
	CHECK_ADD_REMOVE(remove_const, const* const, const*)

#if !defined(BURGER_WATCOM) && \
	!(defined(BURGER_METROWERKS) && defined(BURGER_68K))
	CHECK_ADD_REMOVE(remove_const, [2], [2])
	CHECK_ADD_REMOVE(remove_const, const[2], [2])
	CHECK_ADD_REMOVE(remove_const, volatile[2], volatile[2])
	CHECK_ADD_REMOVE(remove_const, const volatile[2], volatile[2])
#endif

	return uFailure;
}

/***************************************

	Test remove_volatile<>

***************************************/

static uint_t BURGER_API Test_remove_volatile(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	CHECK_ADD_REMOVE(
		remove_volatile, BURGER_NULL_MACRO_PARAM, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_volatile, const, const)
	CHECK_ADD_REMOVE(remove_volatile, volatile, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_volatile, const volatile, const)

	CHECK_ADD_REMOVE(remove_volatile, *, *)
	CHECK_ADD_REMOVE(remove_volatile, * const, * const)
	CHECK_ADD_REMOVE(remove_volatile, * volatile, *)
	CHECK_ADD_REMOVE(remove_volatile, * const volatile, * const)
	CHECK_ADD_REMOVE(remove_volatile, const*, const*)
	CHECK_ADD_REMOVE(remove_volatile, volatile*, volatile*)
	CHECK_ADD_REMOVE(remove_volatile, const* volatile, const*)

#if !defined(BURGER_WATCOM) && \
	!(defined(BURGER_METROWERKS) && defined(BURGER_68K))
	CHECK_ADD_REMOVE(remove_volatile, [2], [2])
	CHECK_ADD_REMOVE(remove_volatile, const[2], const[2])
	CHECK_ADD_REMOVE(remove_volatile, volatile[2], [2])
	CHECK_ADD_REMOVE(remove_volatile, const volatile[2], const[2])
#endif

	return uFailure;
}

/***************************************

	Test remove_cv<>

***************************************/

static uint_t BURGER_API Test_remove_cv(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	CHECK_ADD_REMOVE(
		remove_cv, BURGER_NULL_MACRO_PARAM, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_cv, const, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_cv, volatile, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_cv, const volatile, BURGER_NULL_MACRO_PARAM)

	CHECK_ADD_REMOVE(remove_cv, *, *)
	CHECK_ADD_REMOVE(remove_cv, * const, *)
	CHECK_ADD_REMOVE(remove_cv, * volatile, *)
	CHECK_ADD_REMOVE(remove_cv, * const volatile, *)
	CHECK_ADD_REMOVE(remove_cv, const*, const*)
	CHECK_ADD_REMOVE(remove_cv, volatile*, volatile*)
	CHECK_ADD_REMOVE(remove_cv, const* volatile, const*)

#if !defined(BURGER_WATCOM) && \
	!(defined(BURGER_METROWERKS) && defined(BURGER_68K))
	CHECK_ADD_REMOVE(remove_cv, [2], [2])
	CHECK_ADD_REMOVE(remove_cv, const[2], [2])
	CHECK_ADD_REMOVE(remove_cv, volatile[2], [2])
	CHECK_ADD_REMOVE(remove_cv, const volatile[2], [2])
#endif

	return uFailure;
}

/***************************************

	Test add_const<>

***************************************/

static uint_t BURGER_API Test_add_const(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	CHECK_ADD_REMOVE(add_const, BURGER_NULL_MACRO_PARAM, const)
	CHECK_ADD_REMOVE(add_const, const, const)
	CHECK_ADD_REMOVE(add_const, volatile, volatile const)
	CHECK_ADD_REMOVE(add_const, &, &)
	CHECK_ADD_REMOVE(add_const, const&, const&)
	CHECK_ADD_REMOVE(add_const, volatile&, volatile&)
	CHECK_ADD_REMOVE(add_const, *, * const)
	CHECK_ADD_REMOVE(add_const, const*, const* const)
	CHECK_ADD_REMOVE(add_const, volatile*, volatile* const)

#if !defined(BURGER_WATCOM)
	CHECK_ADD_REMOVE(add_const, [2], const[2])
	CHECK_ADD_REMOVE(add_const, const[2], const[2])
	CHECK_ADD_REMOVE(add_const, volatile[2], volatile const[2])
	CHECK_ADD_REMOVE(add_const, volatile const[2], volatile const[2])
#endif

#if defined(BURGER_RVALUE_REFERENCES)
	CHECK_ADD_REMOVE(add_const, &&, &&)
	CHECK_ADD_REMOVE(add_const, const&&, const&&)
	CHECK_ADD_REMOVE(add_const, volatile&&, volatile &&)
	CHECK_ADD_REMOVE(add_const, volatile const&&, volatile const&&)
#endif
	return uFailure;
}

/***************************************

	Test add_volatile<>

***************************************/

static uint_t BURGER_API Test_add_volatile(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	CHECK_ADD_REMOVE(add_volatile, BURGER_NULL_MACRO_PARAM, volatile)
	CHECK_ADD_REMOVE(add_volatile, const, volatile const)
	CHECK_ADD_REMOVE(add_volatile, volatile, volatile)
	CHECK_ADD_REMOVE(add_volatile, &, &)
	CHECK_ADD_REMOVE(add_volatile, const&, const&)
	CHECK_ADD_REMOVE(add_volatile, volatile&, volatile&)
	CHECK_ADD_REMOVE(add_volatile, *, * volatile)
	CHECK_ADD_REMOVE(add_volatile, const*, const* volatile)
	CHECK_ADD_REMOVE(add_volatile, volatile*, volatile* volatile)

#if !defined(BURGER_WATCOM)
	CHECK_ADD_REMOVE(add_volatile, [2], volatile[2])
	CHECK_ADD_REMOVE(add_volatile, const[2], volatile const[2])
	CHECK_ADD_REMOVE(add_volatile, volatile[2], volatile[2])
	CHECK_ADD_REMOVE(add_volatile, volatile const[2], volatile const[2])
#endif

#if defined(BURGER_RVALUE_REFERENCES)
	CHECK_ADD_REMOVE(add_volatile, &&, &&)
	CHECK_ADD_REMOVE(add_volatile, const&&, const&&)
	CHECK_ADD_REMOVE(add_volatile, volatile&&, volatile &&)
	CHECK_ADD_REMOVE(add_volatile, volatile const&&, volatile const&&)
#endif

	return uFailure;
}

/***************************************

	Test add_cv<>

***************************************/

static uint_t BURGER_API Test_add_cv(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	CHECK_ADD_REMOVE(add_cv, BURGER_NULL_MACRO_PARAM, volatile const)
	CHECK_ADD_REMOVE(add_cv, const, volatile const)
	CHECK_ADD_REMOVE(add_cv, volatile, volatile const)
	CHECK_ADD_REMOVE(add_cv, &, &)
	CHECK_ADD_REMOVE(add_cv, const&, const&)
	CHECK_ADD_REMOVE(add_cv, volatile&, volatile&)
	CHECK_ADD_REMOVE(add_cv, *, * volatile const)
	CHECK_ADD_REMOVE(add_cv, const*, const* volatile const)
	CHECK_ADD_REMOVE(add_cv, volatile*, volatile* volatile const)

#if !defined(BURGER_WATCOM)
	CHECK_ADD_REMOVE(add_cv, [2], volatile const[2])
	CHECK_ADD_REMOVE(add_cv, const[2], volatile const[2])
	CHECK_ADD_REMOVE(add_cv, volatile[2], volatile const[2])
	CHECK_ADD_REMOVE(add_cv, volatile const[2], volatile const[2])
#endif

#if defined(BURGER_RVALUE_REFERENCES)
	CHECK_ADD_REMOVE(add_cv, &&, &&)
	CHECK_ADD_REMOVE(add_cv, const&&, const&&)
	CHECK_ADD_REMOVE(add_cv, volatile&&, volatile &&)
	CHECK_ADD_REMOVE(add_cv, volatile const&&, volatile const&&)
#endif

	return uFailure;
}

/***************************************

	Test remove_reference<>

***************************************/

static uint_t BURGER_API Test_remove_reference(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	CHECK_ADD_REMOVE(
		remove_reference, BURGER_NULL_MACRO_PARAM, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_reference, const, const)
	CHECK_ADD_REMOVE(remove_reference, volatile, volatile)
	CHECK_ADD_REMOVE(remove_reference, &, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_reference, const&, const)
	CHECK_ADD_REMOVE(remove_reference, volatile&, volatile)
	CHECK_ADD_REMOVE(remove_reference, *, *)
	CHECK_ADD_REMOVE(remove_reference, const*, const*)
	CHECK_ADD_REMOVE(remove_reference, volatile*, volatile*)
	CHECK_ADD_REMOVE(remove_reference, [2], [2])
	CHECK_ADD_REMOVE(remove_reference, const[2], const[2])
	CHECK_ADD_REMOVE(remove_reference, volatile[2], volatile[2])
	CHECK_ADD_REMOVE(remove_reference, volatile const[2], volatile const[2])

#if defined(BURGER_RVALUE_REFERENCES)
	CHECK_ADD_REMOVE(remove_reference, &&, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_reference, const&&, const)
	CHECK_ADD_REMOVE(remove_reference, volatile&&, volatile)
	CHECK_ADD_REMOVE(remove_reference, volatile const&&, volatile const)
#endif

	return uFailure;
}

/***************************************

	Test remove_pointer<>

***************************************/

static uint_t BURGER_API Test_remove_pointer(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	CHECK_ADD_REMOVE(
		remove_pointer, BURGER_NULL_MACRO_PARAM, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_pointer, const, const)
	CHECK_ADD_REMOVE(remove_pointer, volatile, volatile)
	CHECK_ADD_REMOVE(remove_pointer, &, &)
	CHECK_ADD_REMOVE(remove_pointer, const&, const&)
	CHECK_ADD_REMOVE(remove_pointer, volatile&, volatile&)
	CHECK_ADD_REMOVE(remove_pointer, *, BURGER_NULL_MACRO_PARAM)
	CHECK_ADD_REMOVE(remove_pointer, const*, const)
	CHECK_ADD_REMOVE(remove_pointer, volatile*, volatile)
	CHECK_ADD_REMOVE(remove_pointer, [2], [2])
	CHECK_ADD_REMOVE(remove_pointer, const[2], const[2])
	CHECK_ADD_REMOVE(remove_pointer, volatile[2], volatile[2])
	CHECK_ADD_REMOVE(remove_pointer, volatile const[2], volatile const[2])
	CHECK_ADD_REMOVE(remove_pointer, (&)[2], (&)[2])

#if defined(BURGER_RVALUE_REFERENCES)
	CHECK_ADD_REMOVE(remove_pointer, &&, &&)
	CHECK_ADD_REMOVE(remove_pointer, const&&, const&&)
	CHECK_ADD_REMOVE(remove_pointer, volatile&&, volatile &&)
	CHECK_ADD_REMOVE(remove_pointer, volatile const&&, volatile const&&)
#endif
	// Test for edge cases
	CHECK_ADD_REMOVE(remove_pointer, (*)(int), (int))
	CHECK_ADD_REMOVE(remove_pointer, (*const)(short), (short))
	return uFailure;
}

/***************************************

	Test is_const<>

***************************************/

#define TEST_IS(function, input, result) \
	if (Burger::function<input>::value != result) { \
		uFailure |= 1; \
		ReportFailure("Burger::" #function "<" #input ">::value is not %d", 1, \
			static_cast<int>(result)); \
	}

static uint_t BURGER_API Test_is_const(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS(is_const, void, false)
	TEST_IS(is_const, const void, true)
	TEST_IS(is_const, int, false)
	TEST_IS(is_const, const int, true)
	TEST_IS(is_const, volatile int, false)
	TEST_IS(is_const, volatile const int, true)
	TEST_IS(is_const, int&, false)
	TEST_IS(is_const, const int&, false)
	TEST_IS(is_const, int*, false)
	TEST_IS(is_const, const int*, false)
	TEST_IS(is_const, volatile int*, false)
	TEST_IS(is_const, const int* const, true)
	TEST_IS(is_const, volatile int* const, true)
	TEST_IS(is_const, const int* volatile, false)
	TEST_IS(is_const, volatile int* volatile, false)
	TEST_IS(is_const, IntTest64x64_t, false)
	TEST_IS(is_const, IntTest64x64_t*, false)
	TEST_IS(is_const, const IntTest64x64_t, true)
	TEST_IS(is_const, const IntTest64x64_t*, false)
	TEST_IS(is_const, const IntTest64x64_t* const, true)

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_const, int&&, false)
	TEST_IS(is_const, const int&&, false)
	TEST_IS(is_const, volatile int&&, false)
	TEST_IS(is_const, volatile const int&&, false)
#endif
	return uFailure;
}

/***************************************

	Test is_volatile<>

***************************************/

static uint_t BURGER_API Test_is_volatile(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	TEST_IS(is_volatile, void, false)
	TEST_IS(is_volatile, const void, false)
	TEST_IS(is_volatile, int, false)
	TEST_IS(is_volatile, const int, false)
	TEST_IS(is_volatile, volatile int, true)
	TEST_IS(is_volatile, volatile const int, true)
	TEST_IS(is_volatile, int&, false)
	TEST_IS(is_volatile, const int&, false)
	TEST_IS(is_volatile, int*, false)
	TEST_IS(is_volatile, const int*, false)
	TEST_IS(is_volatile, volatile int*, false)
	TEST_IS(is_volatile, const int* const, false)
	TEST_IS(is_volatile, volatile int* const, false)
	TEST_IS(is_volatile, const int* volatile, true)
	TEST_IS(is_volatile, volatile int* volatile, true)
	TEST_IS(is_volatile, IntTest64x64_t, false)
	TEST_IS(is_volatile, IntTest64x64_t*, false)
	TEST_IS(is_volatile, volatile IntTest64x64_t, true)
	TEST_IS(is_volatile, volatile IntTest64x64_t*, false)
	TEST_IS(is_volatile, volatile IntTest64x64_t* volatile, true)

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_volatile, int&&, false)
	TEST_IS(is_volatile, const int&&, false)
	TEST_IS(is_volatile, volatile int&&, false)
	TEST_IS(is_volatile, volatile const int&&, false)
#endif
	return uFailure;
}

/***************************************

	Test is_void<>

***************************************/

static uint_t BURGER_API Test_is_void(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS(is_void, void, true)
	TEST_IS(is_void, const void, true)
	TEST_IS(is_void, volatile void, true)
	TEST_IS(is_void, const volatile void, true)
	TEST_IS(is_void, void*, false)
	// Note void& is illegal
	TEST_IS(is_void, int&, false)

	TEST_IS(is_void, int, false)
	TEST_IS(is_void, float, false)
	TEST_IS(is_void, double, false)
	TEST_IS(is_void, long double, false)
	TEST_IS(is_void, long, false)
	TEST_IS(is_void, double*, false)
	TEST_IS(is_void, long double*, false)
	TEST_IS(is_void, float&, false)
	TEST_IS(is_void, double&, false)
	TEST_IS(is_void, long double&, false)
	TEST_IS(is_void, double* volatile, false)
	TEST_IS(is_void, double* const, false)
	TEST_IS(is_void, const float, false)
	TEST_IS(is_void, volatile float, false)
	TEST_IS(is_void, const volatile float, false)
	TEST_IS(is_void, volatile long double&, false)
	TEST_IS(is_void, call_type1, false)
	TEST_IS(is_void, call_type2, false)
	TEST_IS(is_void, call_type3, false)
	TEST_IS(is_void, call_type4, false)
	TEST_IS(is_void, call_type5, false)
	TEST_IS(is_void, undefined_struct, false)

#if defined(BURGER_RVALUE_REFERENCES)
	// Note void && is illegal
	TEST_IS(is_void, int&&, false)
	TEST_IS(is_void, const int&&, false)
	TEST_IS(is_void, volatile int&&, false)
	TEST_IS(is_void, volatile const int&&, false)
#endif
	return uFailure;
}

/***************************************

	Test is_floating_point<>

***************************************/

static uint_t BURGER_API Test_is_floating_point(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS(is_floating_point, int, false)
	TEST_IS(is_floating_point, float, true)
	TEST_IS(is_floating_point, double, true)
	TEST_IS(is_floating_point, long double, true)
	TEST_IS(is_floating_point, long, false)
	TEST_IS(is_floating_point, float*, false)
	TEST_IS(is_floating_point, double*, false)
	TEST_IS(is_floating_point, long double*, false)
	TEST_IS(is_floating_point, float&, false)
	TEST_IS(is_floating_point, double&, false)
	TEST_IS(is_floating_point, long double&, false)
	TEST_IS(is_floating_point, double* volatile, false)
	TEST_IS(is_floating_point, double* const, false)
	TEST_IS(is_floating_point, const float, true)
	TEST_IS(is_floating_point, volatile float, true)
	TEST_IS(is_floating_point, const volatile float, true)
	TEST_IS(is_floating_point, volatile long double&, false)

	return uFailure;
}

/***************************************

	Test is_integral<>

***************************************/

static uint_t BURGER_API Test_is_integral(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS(is_integral, char, true)
	TEST_IS(is_integral, const char, true)
	TEST_IS(is_integral, volatile char, true)
	TEST_IS(is_integral, const volatile char, true)

	TEST_IS(is_integral, bool, true)
	TEST_IS(is_integral, short, true)
	TEST_IS(is_integral, int, true)
	TEST_IS(is_integral, long, true)
	TEST_IS(is_integral, long long, true)

	TEST_IS(is_integral, float, false)
	TEST_IS(is_integral, double, false)
	TEST_IS(is_integral, long double, false)

	TEST_IS(is_integral, int*, false)
	TEST_IS(is_integral, float*, false)
	TEST_IS(is_integral, double*, false)
	TEST_IS(is_integral, long double*, false)
	TEST_IS(is_integral, float&, false)
	TEST_IS(is_integral, double&, false)
	TEST_IS(is_integral, long double&, false)
	TEST_IS(is_integral, double* volatile, false)
	TEST_IS(is_integral, double* const, false)
	TEST_IS(is_integral, const float, false)
	TEST_IS(is_integral, volatile float, false)
	TEST_IS(is_integral, const volatile float, false)
	TEST_IS(is_integral, volatile long double&, false)

	TEST_IS(is_integral, int[2], false)
	TEST_IS(is_integral, call_type1, false)
	TEST_IS(is_integral, call_type2, false)
	TEST_IS(is_integral, call_type3, false)
	TEST_IS(is_integral, call_type4, false)
	TEST_IS(is_integral, call_type5, false)
	TEST_IS(is_integral, undefined_struct, false)

#if defined(BURGER_MSVC)
	TEST_IS(is_integral, __int8, true)
	TEST_IS(is_integral, __int16, true)
	TEST_IS(is_integral, __int32, true)
	TEST_IS(is_integral, __int64, true)
	TEST_IS(is_integral, unsigned __int8, true)
	TEST_IS(is_integral, unsigned __int16, true)
	TEST_IS(is_integral, unsigned __int32, true)
	TEST_IS(is_integral, unsigned __int64, true)
#endif

#if defined(BURGER_HAS_WCHAR_T)
	TEST_IS(is_integral, wchar_t, true)
#endif

#if defined(BURGER_HAS_CHAR8_T)
	TEST_IS(is_integral, char8_t, true)
#endif

#if defined(BURGER_HAS_CHAR16_T)
	TEST_IS(is_integral, char16_t, true)
	TEST_IS(is_integral, char32_t, true)
#endif

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_integral, int&&, false)
	TEST_IS(is_integral, const int&&, false)
	TEST_IS(is_integral, volatile int&&, false)
	TEST_IS(is_integral, volatile const int&&, false)
#endif

	return uFailure;
}

/***************************************

	Test is_arithmetic<>

***************************************/

static uint_t BURGER_API Test_is_arithmetic(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS(is_arithmetic, char, true)
	TEST_IS(is_arithmetic, const char, true)
	TEST_IS(is_arithmetic, volatile char, true)
	TEST_IS(is_arithmetic, const volatile char, true)

	TEST_IS(is_arithmetic, bool, true)
	TEST_IS(is_arithmetic, short, true)
	TEST_IS(is_arithmetic, int, true)
	TEST_IS(is_arithmetic, long, true)
	TEST_IS(is_arithmetic, long long, true)

	TEST_IS(is_arithmetic, float, true)
	TEST_IS(is_arithmetic, double, true)
	TEST_IS(is_arithmetic, long double, true)

	TEST_IS(is_arithmetic, int*, false)
	TEST_IS(is_arithmetic, float*, false)
	TEST_IS(is_arithmetic, double*, false)
	TEST_IS(is_arithmetic, long double*, false)
	TEST_IS(is_arithmetic, float&, false)
	TEST_IS(is_arithmetic, double&, false)
	TEST_IS(is_arithmetic, long double&, false)
	TEST_IS(is_arithmetic, double* volatile, false)
	TEST_IS(is_arithmetic, double* const, false)
	TEST_IS(is_arithmetic, const float, true)
	TEST_IS(is_arithmetic, volatile float, true)
	TEST_IS(is_arithmetic, const volatile float, true)
	TEST_IS(is_arithmetic, volatile long double&, false)

	TEST_IS(is_arithmetic, int[2], false)
	TEST_IS(is_arithmetic, call_type1, false)
	TEST_IS(is_arithmetic, call_type2, false)
	TEST_IS(is_arithmetic, call_type3, false)
	TEST_IS(is_arithmetic, call_type4, false)
	TEST_IS(is_arithmetic, call_type5, false)
	TEST_IS(is_arithmetic, undefined_struct, false)

#if defined(BURGER_MSVC)
	TEST_IS(is_arithmetic, __int8, true)
	TEST_IS(is_arithmetic, __int16, true)
	TEST_IS(is_arithmetic, __int32, true)
	TEST_IS(is_arithmetic, __int64, true)
	TEST_IS(is_arithmetic, unsigned __int8, true)
	TEST_IS(is_arithmetic, unsigned __int16, true)
	TEST_IS(is_arithmetic, unsigned __int32, true)
	TEST_IS(is_arithmetic, unsigned __int64, true)
#endif

#if defined(BURGER_HAS_WCHAR_T)
	TEST_IS(is_arithmetic, wchar_t, true)
#endif

#if defined(BURGER_HAS_CHAR8_T)
	TEST_IS(is_arithmetic, char8_t, true)
#endif

#if defined(BURGER_HAS_CHAR16_T)
	TEST_IS(is_arithmetic, char16_t, true)
	TEST_IS(is_arithmetic, char32_t, true)
#endif

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_arithmetic, int&&, false)
	TEST_IS(is_arithmetic, const int&&, false)
	TEST_IS(is_arithmetic, volatile int&&, false)
	TEST_IS(is_arithmetic, volatile const int&&, false)
#endif

	return uFailure;
}

/***************************************

	Test is_signed>

***************************************/

static uint_t BURGER_API Test_is_signed(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	bool bSignedResult = false;
	if (static_cast<char>(-1) < 0) {
		bSignedResult = true;
	}
	TEST_IS(is_signed, char, bSignedResult)
	TEST_IS(is_signed, const char, bSignedResult)
	TEST_IS(is_signed, volatile char, bSignedResult)
	TEST_IS(is_signed, const volatile char, bSignedResult)

	TEST_IS(is_signed, bool, false)
	TEST_IS(is_signed, short, true)
	TEST_IS(is_signed, int, true)
	TEST_IS(is_signed, long, true)
	TEST_IS(is_signed, long long, true)

	TEST_IS(is_signed, unsigned short, false)
	TEST_IS(is_signed, unsigned int, false)
	TEST_IS(is_signed, unsigned long, false)
	TEST_IS(is_signed, unsigned long long, false)

	TEST_IS(is_signed, float, true)
	TEST_IS(is_signed, double, true)
	TEST_IS(is_signed, long double, true)

	TEST_IS(is_signed, int*, false)
	TEST_IS(is_signed, float*, false)
	TEST_IS(is_signed, double*, false)
	TEST_IS(is_signed, long double*, false)
	TEST_IS(is_signed, float&, false)
	TEST_IS(is_signed, double&, false)
	TEST_IS(is_signed, long double&, false)
	TEST_IS(is_signed, double* volatile, false)
	TEST_IS(is_signed, double* const, false)
	TEST_IS(is_signed, const float, true)
	TEST_IS(is_signed, volatile float, true)
	TEST_IS(is_signed, const volatile float, true)
	TEST_IS(is_signed, volatile long double&, false)

	TEST_IS(is_signed, int[2], false)
	TEST_IS(is_signed, call_type1, false)
	TEST_IS(is_signed, call_type2, false)
	TEST_IS(is_signed, call_type3, false)
	TEST_IS(is_signed, call_type4, false)
	TEST_IS(is_signed, call_type5, false)
	TEST_IS(is_signed, undefined_struct, false)

#if defined(BURGER_MSVC)
	TEST_IS(is_signed, __int8, true)
	TEST_IS(is_signed, __int16, true)
	TEST_IS(is_signed, __int32, true)
	TEST_IS(is_signed, __int64, true)
	TEST_IS(is_signed, unsigned __int8, false)
	TEST_IS(is_signed, unsigned __int16, false)
	TEST_IS(is_signed, unsigned __int32, false)
	TEST_IS(is_signed, unsigned __int64, false)
#endif

#if defined(BURGER_HAS_WCHAR_T)
	bSignedResult = false;
	if (static_cast<wchar_t>(-1) < 0) {
		bSignedResult = true;
	}
	TEST_IS(is_signed, wchar_t, bSignedResult)
#endif

#if defined(BURGER_HAS_CHAR8_T)
	bSignedResult = false;
	if (static_cast<char8_t>(-1) < 0) {
		bSignedResult = true;
	}
	TEST_IS(is_signed, char8_t, bSignedResult)
#endif

#if defined(BURGER_HAS_CHAR16_T)
	bSignedResult = false;
	if (static_cast<char16_t>(-1) < 0) {
		bSignedResult = true;
	}
	TEST_IS(is_signed, char16_t, bSignedResult)
	TEST_IS(is_signed, char32_t, bSignedResult)
#endif

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_signed, int&&, false)
	TEST_IS(is_signed, const int&&, false)
	TEST_IS(is_signed, volatile int&&, false)
	TEST_IS(is_signed, volatile const int&&, false)
#endif
	return uFailure;
}

/***************************************

	Test is_unsigned>

***************************************/

static uint_t BURGER_API Test_is_unsigned(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	bool bUnsignedResult = true;
	if (static_cast<char>(-1) < 0) {
		bUnsignedResult = false;
	}
	TEST_IS(is_unsigned, char, bUnsignedResult)
	TEST_IS(is_unsigned, const char, bUnsignedResult)
	TEST_IS(is_unsigned, volatile char, bUnsignedResult)
	TEST_IS(is_unsigned, const volatile char, bUnsignedResult)

	TEST_IS(is_unsigned, bool, true)
	TEST_IS(is_unsigned, short, false)
	TEST_IS(is_unsigned, int, false)
	TEST_IS(is_unsigned, long, false)
	TEST_IS(is_unsigned, long long, false)

	TEST_IS(is_unsigned, unsigned short, true)
	TEST_IS(is_unsigned, unsigned int, true)
	TEST_IS(is_unsigned, unsigned long, true)
	TEST_IS(is_unsigned, unsigned long long, true)

	TEST_IS(is_unsigned, float, false)
	TEST_IS(is_unsigned, double, false)
	TEST_IS(is_unsigned, long double, false)

	TEST_IS(is_unsigned, int*, false)
	TEST_IS(is_unsigned, float*, false)
	TEST_IS(is_unsigned, double*, false)
	TEST_IS(is_unsigned, long double*, false)
	TEST_IS(is_unsigned, float&, false)
	TEST_IS(is_unsigned, double&, false)
	TEST_IS(is_unsigned, long double&, false)
	TEST_IS(is_unsigned, double* volatile, false)
	TEST_IS(is_unsigned, double* const, false)
	TEST_IS(is_unsigned, const float, false)
	TEST_IS(is_unsigned, volatile float, false)
	TEST_IS(is_unsigned, const volatile float, false)
	TEST_IS(is_unsigned, volatile long double&, false)

	TEST_IS(is_unsigned, int[2], false)
	TEST_IS(is_unsigned, call_type1, false)
	TEST_IS(is_unsigned, call_type2, false)
	TEST_IS(is_unsigned, call_type3, false)
	TEST_IS(is_unsigned, call_type4, false)
	TEST_IS(is_unsigned, call_type5, false)
	TEST_IS(is_unsigned, undefined_struct, false)

#if defined(BURGER_MSVC)
	TEST_IS(is_unsigned, __int8, false)
	TEST_IS(is_unsigned, __int16, false)
	TEST_IS(is_unsigned, __int32, false)
	TEST_IS(is_unsigned, __int64, false)
	TEST_IS(is_unsigned, unsigned __int8, true)
	TEST_IS(is_unsigned, unsigned __int16, true)
	TEST_IS(is_unsigned, unsigned __int32, true)
	TEST_IS(is_unsigned, unsigned __int64, true)
#endif

#if defined(BURGER_HAS_WCHAR_T)
	bUnsignedResult = true;
	if (static_cast<wchar_t>(-1) < 0) {
		bUnsignedResult = false;
	}
	TEST_IS(is_unsigned, wchar_t, bUnsignedResult)
#endif

#if defined(BURGER_HAS_CHAR8_T)
	bUnsignedResult = true;
	if (static_cast<char8_t>(-1) < 0) {
		bUnsignedResult = false;
	}
	TEST_IS(is_unsigned, char8_t, bUnsignedResult)
#endif

#if defined(BURGER_HAS_CHAR16_T)
	bUnsignedResult = true;
	if (static_cast<char16_t>(-1) < 0) {
		bUnsignedResult = false;
	}
	TEST_IS(is_unsigned, char16_t, bUnsignedResult)
	TEST_IS(is_unsigned, char32_t, bUnsignedResult)
#endif

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_unsigned, int&&, false)
	TEST_IS(is_unsigned, const int&&, false)
	TEST_IS(is_unsigned, volatile int&&, false)
	TEST_IS(is_unsigned, volatile const int&&, false)
#endif

	return uFailure;
}

/***************************************

	Test is_pointer<>

***************************************/

static uint_t BURGER_API Test_is_pointer(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS(is_pointer, char, false)
	TEST_IS(is_pointer, const char, false)
	TEST_IS(is_pointer, volatile char, false)
	TEST_IS(is_pointer, const volatile char, false)

	TEST_IS(is_pointer, bool, false)
	TEST_IS(is_pointer, short, false)
	TEST_IS(is_pointer, int, false)
	TEST_IS(is_pointer, long, false)
	TEST_IS(is_pointer, long long, false)

	TEST_IS(is_pointer, float, false)
	TEST_IS(is_pointer, double, false)
	TEST_IS(is_pointer, long double, false)

	TEST_IS(is_pointer, int*, true)
	TEST_IS(is_pointer, float*, true)
	TEST_IS(is_pointer, double*, true)
	TEST_IS(is_pointer, long double*, true)
	TEST_IS(is_pointer, float&, false)
	TEST_IS(is_pointer, double&, false)
	TEST_IS(is_pointer, long double&, false)
	TEST_IS(is_pointer, double* volatile, true)
	TEST_IS(is_pointer, double* const, true)
	TEST_IS(is_pointer, double* const volatile, true)
	TEST_IS(is_pointer, const float, false)
	TEST_IS(is_pointer, volatile float, false)
	TEST_IS(is_pointer, const volatile float, false)
	TEST_IS(is_pointer, volatile long double&, false)

	TEST_IS(is_pointer, int[2], false)
	TEST_IS(is_pointer, call_type1, false)
	TEST_IS(is_pointer, call_type2, false)
	TEST_IS(is_pointer, call_type3, false)
	TEST_IS(is_pointer, call_type4, false)
	TEST_IS(is_pointer, call_type5, false)
	TEST_IS(is_pointer, undefined_struct, false)

	TEST_IS(is_pointer, fake_pointer_operator, false)
	TEST_IS(is_pointer, fake_pointer_operator*, true)
	TEST_IS(is_pointer, fake_pointer_operator&, false)
	TEST_IS(is_pointer, int (*const)(void), true)
	TEST_IS(is_pointer, call_ptr1, true)
	TEST_IS(is_pointer, call_ptr2, true)
	TEST_IS(is_pointer, call_ptr3, true)

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_pointer, int&&, false)
	TEST_IS(is_pointer, const int&&, false)
	TEST_IS(is_pointer, volatile int&&, false)
	TEST_IS(is_pointer, volatile const int&&, false)
#endif

	return uFailure;
}

/***************************************

	Test is_lvalue_reference<>

***************************************/

static uint_t BURGER_API Test_is_lvalue_reference(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS(is_lvalue_reference, char, false)
	TEST_IS(is_lvalue_reference, const char, false)
	TEST_IS(is_lvalue_reference, volatile char, false)
	TEST_IS(is_lvalue_reference, const volatile char, false)

	TEST_IS(is_lvalue_reference, bool, false)
	TEST_IS(is_lvalue_reference, short, false)
	TEST_IS(is_lvalue_reference, int, false)
	TEST_IS(is_lvalue_reference, long, false)
	TEST_IS(is_lvalue_reference, long long, false)

	TEST_IS(is_lvalue_reference, float, false)
	TEST_IS(is_lvalue_reference, double, false)
	TEST_IS(is_lvalue_reference, long double, false)

	TEST_IS(is_lvalue_reference, int*, false)
	TEST_IS(is_lvalue_reference, float*, false)
	TEST_IS(is_lvalue_reference, double*, false)
	TEST_IS(is_lvalue_reference, long double*, false)
	TEST_IS(is_lvalue_reference, float&, true)
	TEST_IS(is_lvalue_reference, double&, true)
	TEST_IS(is_lvalue_reference, long double&, true)
	TEST_IS(is_lvalue_reference, double* volatile, false)
	TEST_IS(is_lvalue_reference, double* const, false)
	TEST_IS(is_lvalue_reference, double* const volatile, false)
	TEST_IS(is_lvalue_reference, const float, false)
	TEST_IS(is_lvalue_reference, volatile float, false)
	TEST_IS(is_lvalue_reference, const volatile float, false)
	TEST_IS(is_lvalue_reference, volatile long double&, true)

	TEST_IS(is_lvalue_reference, int[2], false)
	TEST_IS(is_lvalue_reference, int(&)[2], true)
	TEST_IS(is_lvalue_reference, short (&)(short), true)
	TEST_IS(is_lvalue_reference, call_type1, false)
	TEST_IS(is_lvalue_reference, call_type2, false)
	TEST_IS(is_lvalue_reference, call_type3, false)
	TEST_IS(is_lvalue_reference, call_type4, false)
	TEST_IS(is_lvalue_reference, call_type5, false)
	TEST_IS(is_lvalue_reference, undefined_struct, false)

	TEST_IS(is_lvalue_reference, fake_pointer_operator, false)
	TEST_IS(is_lvalue_reference, fake_pointer_operator*, false)
	TEST_IS(is_lvalue_reference, fake_pointer_operator&, true)
	TEST_IS(is_lvalue_reference, int (*const)(void), false)
	TEST_IS(is_lvalue_reference, call_ptr1, false)
	TEST_IS(is_lvalue_reference, call_ptr2, false)
	TEST_IS(is_lvalue_reference, call_ptr3, false)

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_lvalue_reference, int&&, false)
	TEST_IS(is_lvalue_reference, const int&&, false)
	TEST_IS(is_lvalue_reference, volatile int&&, false)
	TEST_IS(is_lvalue_reference, volatile const int&&, false)
#endif

	return uFailure;
}

/***************************************

	Test is_rvalue_reference<>

***************************************/

static uint_t BURGER_API Test_is_rvalue_reference(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS(is_rvalue_reference, char, false)
	TEST_IS(is_rvalue_reference, const char, false)
	TEST_IS(is_rvalue_reference, volatile char, false)
	TEST_IS(is_rvalue_reference, const volatile char, false)

	TEST_IS(is_rvalue_reference, bool, false)
	TEST_IS(is_rvalue_reference, short, false)
	TEST_IS(is_rvalue_reference, int, false)
	TEST_IS(is_rvalue_reference, long, false)
	TEST_IS(is_rvalue_reference, long long, false)

	TEST_IS(is_rvalue_reference, float, false)
	TEST_IS(is_rvalue_reference, double, false)
	TEST_IS(is_rvalue_reference, long double, false)

	TEST_IS(is_rvalue_reference, int*, false)
	TEST_IS(is_rvalue_reference, float*, false)
	TEST_IS(is_rvalue_reference, double*, false)
	TEST_IS(is_rvalue_reference, long double*, false)
	TEST_IS(is_rvalue_reference, float&, false)
	TEST_IS(is_rvalue_reference, double&, false)
	TEST_IS(is_rvalue_reference, long double&, false)
	TEST_IS(is_rvalue_reference, double* volatile, false)
	TEST_IS(is_rvalue_reference, double* const, false)
	TEST_IS(is_rvalue_reference, double* const volatile, false)
	TEST_IS(is_rvalue_reference, const float, false)
	TEST_IS(is_rvalue_reference, volatile float, false)
	TEST_IS(is_rvalue_reference, const volatile float, false)
	TEST_IS(is_rvalue_reference, volatile long double&, false)

	TEST_IS(is_rvalue_reference, int[2], false)
	TEST_IS(is_rvalue_reference, int(&)[2], false)

// The xbox 360 compiler has a bug.
#if !defined(BURGER_XBOX360)
	TEST_IS(is_rvalue_reference, short (&)(short), false)
#endif
	TEST_IS(is_rvalue_reference, call_type1, false)
	TEST_IS(is_rvalue_reference, call_type2, false)
	TEST_IS(is_rvalue_reference, call_type3, false)
	TEST_IS(is_rvalue_reference, call_type4, false)
	TEST_IS(is_rvalue_reference, call_type5, false)
	TEST_IS(is_rvalue_reference, undefined_struct, false)

	TEST_IS(is_rvalue_reference, fake_pointer_operator, false)
	TEST_IS(is_rvalue_reference, fake_pointer_operator*, false)
	TEST_IS(is_rvalue_reference, fake_pointer_operator&, false)
	TEST_IS(is_rvalue_reference, int (*const)(void), false)
	TEST_IS(is_rvalue_reference, call_ptr1, false)
	TEST_IS(is_rvalue_reference, call_ptr2, false)
	TEST_IS(is_rvalue_reference, call_ptr3, false)

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_rvalue_reference, int&&, true)
	TEST_IS(is_rvalue_reference, const int&&, true)
	TEST_IS(is_rvalue_reference, volatile int&&, true)
	TEST_IS(is_rvalue_reference, volatile const int&&, true)
	TEST_IS(is_rvalue_reference, fake_pointer_operator&&, true)
#endif

	return uFailure;
}

/***************************************

	Test is_reference<>

***************************************/

static uint_t BURGER_API Test_is_reference(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	TEST_IS(is_reference, char, false)
	TEST_IS(is_reference, const char, false)
	TEST_IS(is_reference, volatile char, false)
	TEST_IS(is_reference, const volatile char, false)

	TEST_IS(is_reference, bool, false)
	TEST_IS(is_reference, short, false)
	TEST_IS(is_reference, int, false)
	TEST_IS(is_reference, long, false)
	TEST_IS(is_reference, long long, false)

	TEST_IS(is_reference, float, false)
	TEST_IS(is_reference, double, false)
	TEST_IS(is_reference, long double, false)

	TEST_IS(is_reference, int*, false)
	TEST_IS(is_reference, float*, false)
	TEST_IS(is_reference, double*, false)
	TEST_IS(is_reference, long double*, false)
	TEST_IS(is_reference, float&, true)
	TEST_IS(is_reference, double&, true)
	TEST_IS(is_reference, long double&, true)
	TEST_IS(is_reference, double* volatile, false)
	TEST_IS(is_reference, double* const, false)
	TEST_IS(is_reference, double* const volatile, false)
	TEST_IS(is_reference, const float, false)
	TEST_IS(is_reference, volatile float, false)
	TEST_IS(is_reference, const volatile float, false)
	TEST_IS(is_reference, volatile long double&, true)

	TEST_IS(is_reference, int[2], false)
	TEST_IS(is_reference, int(&)[2], true)
	TEST_IS(is_reference, short (&)(short), true)
	TEST_IS(is_reference, call_type1, false)
	TEST_IS(is_reference, call_type2, false)
	TEST_IS(is_reference, call_type3, false)
	TEST_IS(is_reference, call_type4, false)
	TEST_IS(is_reference, call_type5, false)
	TEST_IS(is_reference, undefined_struct, false)

	TEST_IS(is_reference, fake_pointer_operator, false)
	TEST_IS(is_reference, fake_pointer_operator*, false)
	TEST_IS(is_reference, fake_pointer_operator&, true)
	TEST_IS(is_reference, int (*const)(void), false)
	TEST_IS(is_reference, call_ptr1, false)
	TEST_IS(is_reference, call_ptr2, false)
	TEST_IS(is_reference, call_ptr3, false)

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_reference, int&&, true)
	TEST_IS(is_reference, const int&&, true)
	TEST_IS(is_reference, volatile int&&, true)
	TEST_IS(is_reference, volatile const int&&, true)
	TEST_IS(is_reference, fake_pointer_operator&&, true)
#endif

	return uFailure;
}

/***************************************

	Test is_function<>

***************************************/

typedef void function0_t();
typedef void function1_t(int);
typedef void function2_t(int&, double);
typedef void function3_t(int&, bool, int, int);
typedef void function4_t(int, bool, int*, int[], int, int, int, int, int);
typedef double function5_t(double...);
#if __cpp_noexcept_function_type
typedef int function6_t(void) noexcept;
typedef int function7_t(double) noexcept(false);
typedef int function8_t(int, double) noexcept(true);
#endif

static uint_t BURGER_API Test_is_function(void) BURGER_NOEXCEPT
{
	// Metrowerks earlier than 7.0 cannot handle is_function<>
#if defined(BURGER_METROWERKS) && (BURGER_METROWERKS < 0x2500)
	Message("Metrowerks version %08X doesn't support is_function<>",
		BURGER_METROWERKS);
	return FALSE;
#else

	uint_t uFailure = FALSE;

	TEST_IS(is_function, char, false)
	TEST_IS(is_function, const char, false)
	TEST_IS(is_function, volatile char, false)
	TEST_IS(is_function, const volatile char, false)

	TEST_IS(is_function, bool, false)
	TEST_IS(is_function, short, false)
	TEST_IS(is_function, int, false)
	TEST_IS(is_function, long, false)
	TEST_IS(is_function, long long, false)

	TEST_IS(is_function, float, false)
	TEST_IS(is_function, double, false)
	TEST_IS(is_function, long double, false)

	TEST_IS(is_function, int*, false)
	TEST_IS(is_function, float*, false)
	TEST_IS(is_function, double*, false)
	TEST_IS(is_function, long double*, false)
	TEST_IS(is_function, float&, false)
	TEST_IS(is_function, double&, false)
	TEST_IS(is_function, long double&, false)
	TEST_IS(is_function, double* volatile, false)
	TEST_IS(is_function, double* const, false)
	TEST_IS(is_function, double* const volatile, false)
	TEST_IS(is_function, const float, false)
	TEST_IS(is_function, volatile float, false)
	TEST_IS(is_function, const volatile float, false)
	TEST_IS(is_function, volatile long double&, false)

	TEST_IS(is_function, int[2], false)
	TEST_IS(is_function, int(&)[2], false)
	TEST_IS(is_function, short (&)(short), false)
	TEST_IS(is_function, call_type1, true)
	TEST_IS(is_function, call_type2, true)
	TEST_IS(is_function, call_type3, true)
	TEST_IS(is_function, call_type4, true)
	TEST_IS(is_function, call_type5, true)
	TEST_IS(is_function, undefined_struct, false)

	TEST_IS(is_function, fake_pointer_operator, false)
	TEST_IS(is_function, fake_pointer_operator*, false)
	TEST_IS(is_function, fake_pointer_operator&, false)
	TEST_IS(is_function, int (*const)(void), false)
	TEST_IS(is_function, call_ptr1, false)
	TEST_IS(is_function, call_ptr2, false)
	TEST_IS(is_function, call_ptr3, false)

#if defined(BURGER_RVALUE_REFERENCES)
	TEST_IS(is_function, int&&, false)
	TEST_IS(is_function, const int&&, false)
	TEST_IS(is_function, volatile int&&, false)
	TEST_IS(is_function, volatile const int&&, false)
	TEST_IS(is_function, fake_pointer_operator&&, false)
#endif
	TEST_IS(is_function, function0_t, true)
	TEST_IS(is_function, function1_t, true)
	TEST_IS(is_function, function2_t, true)
	TEST_IS(is_function, function3_t, true)
	TEST_IS(is_function, function4_t, true)
	TEST_IS(is_function, function5_t, true)
#if __cpp_noexcept_function_type
	TEST_IS(is_function, function6_t, true)
	TEST_IS(is_function, function7_t, true)
	TEST_IS(is_function, function8_t, true)
#endif

#if defined(_MSC_EXTENSIONS)
	TEST_IS(is_function, stdcall_function0_t, true)
	TEST_IS(is_function, stdcall_function1_t, true)
	TEST_IS(is_function, stdcall_function2_t, true)
	TEST_IS(is_function, stdcall_function3_t, true)
	TEST_IS(is_function, stdcall_function4_t, true)

	TEST_IS(is_function, cdecl_function0_t, true)
	TEST_IS(is_function, cdecl_function1_t, true)
	TEST_IS(is_function, cdecl_function2_t, true)
	TEST_IS(is_function, cdecl_function3_t, true)
	TEST_IS(is_function, cdecl_function4_t, true)

#if !defined(_MANAGED)
	TEST_IS(is_function, fastcall_function0_t, true)
	TEST_IS(is_function, fastcall_function1_t, true)
	TEST_IS(is_function, fastcall_function2_t, true)
	TEST_IS(is_function, fastcall_function3_t, true)
	TEST_IS(is_function, fastcall_function4_t, true)
#endif

#if defined(BURGER_SSE2) && (BURGER_MSVC >= 180000000)
	TEST_IS(is_function, vectorcall_function0_t, true)
	TEST_IS(is_function, vectorcall_function1_t, true)
	TEST_IS(is_function, vectorcall_function2_t, true)
	TEST_IS(is_function, vectorcall_function3_t, true)
	TEST_IS(is_function, vectorcall_function4_t, true)
#endif
#endif
	return uFailure;
#endif
}

BURGER_MSVC_POP_WARNING

/***************************************

	Test unique_ptr<>

***************************************/

static uint_t BURGER_API Testunique_ptr(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	{
		IntTest64x64_t* pIntTest = new IntTest64x64_t;
		Burger::unique_ptr<IntTest64x64_t> Test1(pIntTest);

		uint_t uTest = Test1.get() != pIntTest;
		uFailure |= uTest;
		ReportFailure("Burger::unique_ptr<IntTest64x64_t>.get()", uTest);

		uTest = Test1.release() != pIntTest;
		uFailure |= uTest;
		ReportFailure("Burger::unique_ptr<IntTest64x64_t>.release()", uTest);

		uTest = Test1.release() != nullptr;
		uFailure |= uTest;
		ReportFailure(
			"Burger::unique_ptr<IntTest64x64_t>.release() != nullptr", uTest);

		uTest = Test1.get() != nullptr;
		uFailure |= uTest;
		ReportFailure(
			"Burger::unique_ptr<IntTest64x64_t>.get() != nullptr", uTest);

		Test1 = pIntTest;
		uTest = Test1.get() != pIntTest;
		uFailure |= uTest;
		ReportFailure(
			"Burger::unique_ptr<IntTest64x64_t>.get() != pIntTest", uTest);

		Test1.reset();
		uTest = Test1.get() != nullptr;
		uFailure |= uTest;
		ReportFailure(
			"Burger::unique_ptr<IntTest64x64_t>.get() != nullptr", uTest);
	}
	return uFailure;
}

/***************************************

	Test ice_and<>

***************************************/

static uint_t BURGER_API Test_ice_and(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	uint_t uTest;

	uTest = Burger::ice_and<false, false>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<false,false>::value ", uTest);

	uTest = Burger::ice_and<false, true>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<false,true>::value ", uTest);

	uTest = Burger::ice_and<true, false>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<true,false>::value ", uTest);

	uTest = Burger::ice_and<true, true>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<true,true>::value ", uTest);

	uTest = Burger::ice_and<false, false, false>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<false, false,false>::value ", uTest);

	uTest = Burger::ice_and<false, false, true>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<false, false,true>::value ", uTest);

	uTest = Burger::ice_and<false, true, false>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<false, true,false>::value ", uTest);

	uTest = Burger::ice_and<false, true, true>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<false, true,true>::value ", uTest);

	uTest = Burger::ice_and<true, false, false>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<true, false,false>::value ", uTest);

	uTest = Burger::ice_and<true, false, true>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<true, false,true>::value ", uTest);

	uTest = Burger::ice_and<true, true, false>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<true, true,false>::value ", uTest);

	uTest = Burger::ice_and<true, true, true>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_and<true, true,true>::value ", uTest);
	return uFailure;
}

/***************************************

	Test ice_or<>

***************************************/

static uint_t BURGER_API Test_ice_or(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	uint_t uTest;

	uTest = Burger::ice_or<false, false>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<false,false>::value ", uTest);

	uTest = Burger::ice_or<false, true>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<false,true>::value ", uTest);

	uTest = Burger::ice_or<true, false>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<true,false>::value ", uTest);

	uTest = Burger::ice_or<true, true>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<true,true>::value ", uTest);

	uTest = Burger::ice_or<false, false, false>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<false, false,false>::value ", uTest);

	uTest = Burger::ice_or<false, false, true>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<false, false,true>::value ", uTest);

	uTest = Burger::ice_or<false, true, false>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<false, true,false>::value ", uTest);

	uTest = Burger::ice_or<false, true, true>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<false, true,true>::value ", uTest);

	uTest = Burger::ice_or<true, false, false>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<true, false,false>::value ", uTest);

	uTest = Burger::ice_or<true, false, true>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<true, false,true>::value ", uTest);

	uTest = Burger::ice_or<true, true, false>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<true, true,false>::value ", uTest);

	uTest = Burger::ice_or<true, true, true>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_or<true, true,true>::value ", uTest);
	return uFailure;
}

/***************************************

	Test ice_eq<>

***************************************/

static uint_t BURGER_API Test_ice_eq(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	uint_t uTest;

	uTest = Burger::ice_eq<false, false>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_eq<false,false>::value ", uTest);

	uTest = Burger::ice_eq<false, true>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_eq<false,true>::value ", uTest);

	uTest = Burger::ice_eq<true, false>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_eq<true,false>::value ", uTest);

	uTest = Burger::ice_eq<true, true>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_eq<true,true>::value ", uTest);

	return uFailure;
}

/***************************************

	Test ice_ne<>

***************************************/

static uint_t BURGER_API Test_ice_ne(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	uint_t uTest;

	uTest = Burger::ice_ne<false, false>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_ne<false,false>::value ", uTest);

	uTest = Burger::ice_ne<false, true>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_ne<false,true>::value ", uTest);

	uTest = Burger::ice_ne<true, false>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_ne<true,false>::value ", uTest);

	uTest = Burger::ice_ne<true, true>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_ne<true,true>::value ", uTest);

	return uFailure;
}

/***************************************

	Test ice_not<>

***************************************/

static uint_t BURGER_API Test_ice_not(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	uint_t uTest;

	uTest = Burger::ice_not<false>::value != TRUE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_ne<false>::value ", uTest);

	uTest = Burger::ice_not<true>::value != FALSE;
	uFailure |= uTest;
	ReportFailure("Burger::ice_ne<true>::value ", uTest);

	return uFailure;
}

/***************************************

	Test round_up_pointer<>

***************************************/

struct round_up_test_t {
	uintptr_t m_uPtr;
	uintptr_t m_uAlign;
	uintptr_t m_uResult;
};

static const round_up_test_t g_round_up_tests[] = {{0, 2, 0}, {1, 2, 2},
	{2, 2, 2}, {0, 1, 0}, {1, 1, 1}, {2, 1, 2}, {1, 8, 8},
	{9834893, 4, 9834896}};

static uint_t BURGER_API Testround_up_pointer(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const round_up_test_t* pWork = g_round_up_tests;
	uintptr_t uCount = BURGER_ARRAYSIZE(g_round_up_tests);
	do {
		const double* pReturn = Burger::round_up_pointer(
			reinterpret_cast<double*>(pWork->m_uPtr), pWork->m_uAlign);
		uint_t uTest = pReturn != reinterpret_cast<double*>(pWork->m_uResult);
		uFailure |= uTest;
		if (uTest) {
			Burger::String Text(
				"Burger::round_up_pointer((double *)0x%016llX,0x%016llX) = 0x%016llX, expected 0x%016llX",
				pWork->m_uPtr, pWork->m_uAlign,
				reinterpret_cast<uintptr_t>(pReturn), pWork->m_uResult);
			ReportFailure(Text.c_str(), uTest);
		}

		const short* pReturn2 = Burger::round_up_pointer(
			reinterpret_cast<short*>(pWork->m_uPtr), pWork->m_uAlign);
		uTest = pReturn2 != reinterpret_cast<short*>(pWork->m_uResult);
		uFailure |= uTest;
		if (uTest) {
			Burger::String Text(
				"Burger::round_up_pointer((short *)0x%016llX,0x%016llX) = 0x%016llX, expected 0x%016llX",
				pWork->m_uPtr, pWork->m_uAlign,
				reinterpret_cast<uintptr_t>(pReturn2), pWork->m_uResult);
			ReportFailure(Text.c_str(), uTest);
		}

		++pWork;
	} while (--uCount);

	uCount = 0;
	struct aligndouble_t {
		double a;
		char b;
	};
	const BURGER_CONSTEXPR uintptr_t uAlign =
		sizeof(aligndouble_t) - sizeof(double);
	do {
		const double* pReturn =
			Burger::round_up_pointer(reinterpret_cast<double*>(uCount));
		const uintptr_t uExpected = (uCount + uAlign - 1) & (~(uAlign - 1));
		const uint_t uTest = pReturn != reinterpret_cast<double*>(uExpected);
		uFailure |= uTest;
		if (uTest) {
			Burger::String Text(
				"Burger::round_up_pointer((double *)0x%016llX) = 0x%016llX, expected 0x%016llX",
				pWork->m_uPtr, reinterpret_cast<uintptr_t>(pReturn), uExpected);
			ReportFailure(Text.c_str(), uTest);
		}
	} while (++uCount < 17);

	uCount = 0;
	do {
		const short* pReturn =
			Burger::round_up_pointer(reinterpret_cast<short*>(uCount));
		const uintptr_t uExpected = (uCount + 1) & (~1);
		const uint_t uTest = pReturn != reinterpret_cast<short*>(uExpected);
		uFailure |= uTest;
		if (uTest) {
			Burger::String Text(
				"Burger::round_up_pointer((double *)0x%016llX) = 0x%016llX, expected 0x%016llX",
				pWork->m_uPtr, reinterpret_cast<uintptr_t>(pReturn), uExpected);
			ReportFailure(Text.c_str(), uTest);
		}
	} while (++uCount < 17);

	return uFailure;
}

/***************************************

	Test select_value()

***************************************/

static uint_t BURGER_API Testselect_value(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	uint_t uTest;

	uTest = Burger::select_value<Burger::is_signed<int>::value, uint_t, 2,
				5>::value != 2;
	uFailure |= uTest;
	ReportFailure(
		"select_value<Burger::is_signed<int>::value, uint_t, 2, 5>", uTest);

	uTest = Burger::select_value<Burger::is_signed<unsigned int>::value, uint_t,
				2, 5>::value != 5;
	uFailure |= uTest;
	ReportFailure(
		"Burger::select_value<Burger::is_signed<unsigned int>::value, uint_t, 2, 5>",
		uTest);

	// Should be a int8_t, so no compiler warning
	// A compiler warning here is considered a unit test failure
	int8_t iTest = Burger::select_value<Burger::is_signed<unsigned int>::value,
		int8_t, 6, 1>::value;
	uTest = iTest != 1;
	uFailure |= uTest;
	ReportFailure(
		"Burger::select_value<Burger::is_signed<unsigned int>::value, int8_t, 6, 1>",
		uTest);

	return uFailure;
}

/***************************************

	Test move()

***************************************/

class Mover {
public:
	// Flags to denote which function was called
	uint_t m_bConstructed;
	uint_t m_bCopyConstructor;
	uint_t m_bMoveConstructor;
	uint_t m_bCopyOperator;
	uint_t m_bMoveOperator;

	Mover() BURGER_NOEXCEPT: m_bConstructed(TRUE),
							 m_bCopyConstructor(FALSE),
							 m_bMoveConstructor(FALSE),
							 m_bCopyOperator(FALSE),
							 m_bMoveOperator(FALSE)
	{
	}

	Mover(const Mover&) BURGER_NOEXCEPT: m_bConstructed(FALSE),
										 m_bCopyConstructor(TRUE),
										 m_bMoveConstructor(FALSE),
										 m_bCopyOperator(FALSE),
										 m_bMoveOperator(FALSE)
	{
	}

	Mover& operator=(const Mover&) BURGER_NOEXCEPT
	{
		m_bConstructed = FALSE;
		m_bCopyConstructor = FALSE;
		m_bMoveConstructor = FALSE;
		m_bCopyOperator = TRUE;
		m_bMoveOperator = FALSE;
		return *this;
	}

#if defined(BURGER_RVALUE_REFERENCES)
	Mover(Mover&&) BURGER_NOEXCEPT: m_bConstructed(FALSE),
									m_bCopyConstructor(FALSE),
									m_bMoveConstructor(TRUE),
									m_bCopyOperator(FALSE),
									m_bMoveOperator(FALSE)
	{
	}

	Mover& operator=(Mover&&) BURGER_NOEXCEPT
	{
		m_bConstructed = FALSE;
		m_bCopyConstructor = FALSE;
		m_bMoveConstructor = FALSE;
		m_bCopyOperator = FALSE;
		m_bMoveOperator = TRUE;
		return *this;
	}
#endif

	// Reset the variables
	void Clear(void) BURGER_NOEXCEPT
	{
		m_bConstructed = FALSE;
		m_bCopyConstructor = FALSE;
		m_bMoveConstructor = FALSE;
		m_bCopyOperator = FALSE;
		m_bMoveOperator = FALSE;
	}
};

static uint_t BURGER_API Test_move(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	// Check the default constructor
	Mover TestObject;
	uint_t uTest = !TestObject.m_bConstructed;
	uFailure |= uTest;
	ReportFailure("Mover() failed ", uTest);

	// Check the copy constructor
	Mover Test2(TestObject);
	uTest = !Test2.m_bCopyConstructor;
	uFailure |= uTest;
	ReportFailure("Mover(const &) failed ", uTest);

	// Check the copy operator
	Test2 = TestObject;
	uTest = !Test2.m_bCopyOperator;
	uFailure |= uTest;
	ReportFailure("Mover::operator = (const &) failed ", uTest);

#if defined(BURGER_RVALUE_REFERENCES)
	// Check the move constructor
	Mover TestMove(static_cast<Mover&&>(TestObject));
	uTest = !TestMove.m_bMoveConstructor;
	uFailure |= uTest;
	ReportFailure("Mover(&&) failed ", uTest);

	// Check the move operator
	TestMove = static_cast<Mover&&>(TestObject);
	uTest = !TestMove.m_bMoveOperator;
	uFailure |= uTest;
	ReportFailure("Mover::operator = (&&) failed ", uTest);
#endif

	// At this point, the constructors work, so now test Burger::move()

	// Test a move constructor
	Mover TestMove1(Burger::move(TestObject));
#if !defined(BURGER_RVALUE_REFERENCES)
	uTest = !TestMove1.m_bCopyConstructor;
#else
	uTest = !TestMove1.m_bMoveConstructor;
#endif
	uFailure |= uTest;
	ReportFailure("Burger::move() Constructor failed ", uTest);

	// Test a move operator
	TestMove1 = Burger::move(TestObject);
#if !defined(BURGER_RVALUE_REFERENCES)
	uTest = !TestMove1.m_bCopyOperator;
#else
	uTest = !TestMove1.m_bMoveOperator;
#endif
	uFailure |= uTest;
	ReportFailure("Burger::move() operator failed ", uTest);

	return uFailure;
}

/***************************************

	Test algorithms

***************************************/

uint_t BURGER_API TestBralgorithm(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running Algorithm tests");
	}

	uint_t uResult = TestMinInt32();
	uResult |= TestMinInt64();
	uResult |= TestMinUInt32();
	uResult |= TestMinUInt64();
	uResult |= TestMinFloat();

	uResult |= TestMaxInt32();
	uResult |= TestMaxInt64();

	uResult |= Test_enable_if();
	uResult |= Test_integral_constant();
	uResult |= Test_is_same();
	uResult |= Test_conditional();
	uResult |= Test_remove_const();
	uResult |= Test_remove_volatile();
	uResult |= Test_remove_cv();
	uResult |= Test_add_const();
	uResult |= Test_add_volatile();
	uResult |= Test_add_cv();
	uResult |= Test_remove_reference();
	uResult |= Test_remove_pointer();
	uResult |= Test_is_const();
	uResult |= Test_is_volatile();
	uResult |= Test_is_void();
	uResult |= Test_is_floating_point();
	uResult |= Test_is_integral();
	uResult |= Test_is_arithmetic();
	uResult |= Test_is_signed();
	uResult |= Test_is_unsigned();
	uResult |= Test_is_pointer();
	uResult |= Test_is_lvalue_reference();
	uResult |= Test_is_rvalue_reference();
	uResult |= Test_is_reference();
	uResult |= Test_is_function();
	uResult |= Testunique_ptr();
	uResult |= Test_ice_and();
	uResult |= Test_ice_or();
	uResult |= Test_ice_eq();
	uResult |= Test_ice_ne();
	uResult |= Test_ice_not();
	uResult |= Testround_up_pointer();
	uResult |= Testselect_value();
	uResult |= Test_move();

	if (!uResult && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all Algorithm tests!");
	}
	return uResult;
}
