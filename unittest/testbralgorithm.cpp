/***************************************

    Unit tests for the Integer Math library

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
    Burger::Word32ToFloat m_Input1; // Source value 1
    Burger::Word32ToFloat m_Input2; // Source value 2
    Burger::Word32ToFloat m_Output; // Expected output
};

struct DoubleTest_t {
    Burger::Word64ToDouble m_Input1; // Source value 1
    Burger::Word64ToDouble m_Input2; // Source value 2
    Burger::Word64ToDouble m_Output; // Expected output
};

//
// Test Min(int32_t)
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

static uint_t BURGER_API TestMinInt32(void)
{
    uint_t uFailure = FALSE;
    const IntTest32x32_t* pWork = MinTestTableInt32;
    uintptr_t uCount = BURGER_ARRAYSIZE(MinTestTableInt32);
    do {
        int32_t iReturn = Burger::Min(pWork->m_iInput1, pWork->m_iInput2);
        uint_t uTest = iReturn != pWork->m_iOutput;
        uFailure |= uTest;
        ReportFailure(
            "Burger::Min((int32_t)0x%08X,(int32_t)0x%08X) = 0x%08X, expected 0x%08X",
            uTest, pWork->m_iInput1, pWork->m_iInput2, iReturn,
            pWork->m_iOutput);
        ++pWork;
    } while (--uCount);
    return uFailure;
}

//
// Test Min(int64_t)
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

static uint_t BURGER_API TestMinInt64(void)
{
    uint_t uFailure = FALSE;
    const IntTest64x64_t* pWork = MinTestTableInt64;
    uintptr_t uCount = BURGER_ARRAYSIZE(MinTestTableInt64);
    do {
        int64_t iReturn = Burger::Min(pWork->m_iInput1, pWork->m_iInput2);
        uint_t uTest = iReturn != pWork->m_iOutput;
        uFailure |= uTest;
        if (uTest) {
            Burger::String Text(
                "Burger::Min((int64_t)0x%016llX,(int64_t)0x%016llX) = 0x%016llX, expected 0x%016llX",
                pWork->m_iInput1, pWork->m_iInput2, iReturn, pWork->m_iOutput);
            ReportFailure(Text.GetPtr(), uTest);
        }
        ++pWork;
    } while (--uCount);
    return uFailure;
}

//
// Test Min(uint32_t)
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

static uint_t BURGER_API TestMinUInt32(void)
{
    uint_t uFailure = FALSE;
    const UIntTest32x32_t* pWork = MinTestTableUInt32;
    uintptr_t uCount = BURGER_ARRAYSIZE(MinTestTableUInt32);
    do {
        uint32_t uReturn = Burger::Min(pWork->m_uInput1, pWork->m_uInput2);
        uint_t uTest = uReturn != pWork->m_uOutput;
        uFailure |= uTest;
        ReportFailure(
            "Burger::Min((uint32_t)0x%08X,(uint32_t)0x%08X) = 0x%08X, expected 0x%08X",
            uTest, pWork->m_uInput1, pWork->m_uInput2, uReturn,
            pWork->m_uOutput);
        ++pWork;
    } while (--uCount);
    return uFailure;
}

//
// Test Min(uint64_t)
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

static uint_t BURGER_API TestMinUInt64(void)
{
    uint_t uFailure = FALSE;
    const UIntTest64x64_t* pWork = MinTestTableUInt64;
    uintptr_t uCount = BURGER_ARRAYSIZE(MinTestTableUInt64);
    do {
        uint64_t uReturn = Burger::Min(pWork->m_uInput1, pWork->m_uInput2);
        uint_t uTest = uReturn != pWork->m_uOutput;
        uFailure |= uTest;
        if (uTest) {
            Burger::String Text(
                "Burger::Min((int64_t)0x%016llX,(int64_t)0x%016llX) = 0x%016llX, expected 0x%016llX",
                pWork->m_uInput1, pWork->m_uInput2, uReturn, pWork->m_uOutput);
            ReportFailure(Text.GetPtr(), uTest);
        }
        ++pWork;
    } while (--uCount);
    return uFailure;
}

//
// Test Min(float)
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

static uint_t BURGER_API TestMinFloat(void)
{
    uint_t uFailure = FALSE;
    const FloatTest_t* pWork = MinTestTableFloat;
    uintptr_t uCount = BURGER_ARRAYSIZE(MinTestTableFloat);
    do {
        Burger::Word32ToFloat Return;
        Return.f = Burger::Min(pWork->m_Input1.f, pWork->m_Input2.f);
        uint_t uTest = Return.w != pWork->m_Output.w;
        uFailure |= uTest;
        ReportFailure(
            "Burger::Min((float)0x%08X,(float)0x%08X) = 0x%08X, expected 0x%08X",
            uTest, pWork->m_Input1.w, pWork->m_Input2.w, Return.w,
            pWork->m_Output.w);
        ++pWork;
    } while (--uCount);
    return uFailure;
}

//
// Test Max(int32_t)
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

static uint_t BURGER_API TestMaxInt32(void)
{
    uint_t uFailure = FALSE;
    const IntTest32x32_t* pWork = MaxTestTableInt32;
    uintptr_t uCount = BURGER_ARRAYSIZE(MaxTestTableInt32);
    do {
        int32_t iReturn = Burger::Max(pWork->m_iInput1, pWork->m_iInput2);
        uint_t uTest = iReturn != pWork->m_iOutput;
        uFailure |= uTest;
        ReportFailure(
            "Burger::Max((int32_t)0x%08X,(int32_t)0x%08X) = 0x%08X, expected 0x%08X",
            uTest, pWork->m_iInput1, pWork->m_iInput2, iReturn,
            pWork->m_iOutput);
        ++pWork;
    } while (--uCount);
    return uFailure;
}

//
// Test Max(int64_t)
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

static uint_t BURGER_API TestMaxInt64(void)
{
    uint_t uFailure = FALSE;
    const IntTest64x64_t* pWork = MaxTestTableInt64;
    uintptr_t uCount = BURGER_ARRAYSIZE(MaxTestTableInt64);
    do {
        int64_t iReturn = Burger::Max(pWork->m_iInput1, pWork->m_iInput2);
        uint_t uTest = iReturn != pWork->m_iOutput;
        uFailure |= uTest;
        if (uTest) {
            Burger::String Text(
                "Burger::Max((int64_t)0x%016llX,(int64_t)0x%016llX) = 0x%016llX, expected 0x%016llX",
                pWork->m_iInput1, pWork->m_iInput2, iReturn, pWork->m_iOutput);
            ReportFailure(Text.GetPtr(), uTest);
        }
        ++pWork;
    } while (--uCount);
    return uFailure;
}

/***************************************

    Test integral_constant<>

***************************************/

static uint_t BURGER_API Testintegral_constant(void)
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

static uint_t BURGER_API Testis_same(void)
{
    uint_t uFailure = FALSE;

    uint_t uTest = Burger::is_same<int, double>::value != FALSE;
    uFailure |= uTest;
    ReportFailure("Burger::is_same<int, double>::value", uTest);

    uTest = Burger::is_same<double, double>::value != TRUE;
    uFailure |= uTest;
    ReportFailure("Burger::is_same<double, double>::value = %d", uTest,
        Burger::is_same<int, double>::value);

    uTest = Burger::is_same<IntTest64x64_t, IntTest64x64_t>::value != TRUE;
    uFailure |= uTest;
    ReportFailure("Burger::is_same<IntTest64x64_t, IntTest64x64_t>::value = %d",
        uTest, Burger::is_same<int, double>::value);

    uTest = Burger::is_same<IntTest64x64_t, char>::value != FALSE;
    uFailure |= uTest;
    ReportFailure("Burger::is_same<IntTest64x64_t, char>::value = %d", uTest,
        Burger::is_same<int, double>::value);
    return uFailure;
}

/***************************************

    Test unique_ptr<>

***************************************/

static uint_t BURGER_API Testunique_ptr(void)
{
    uint_t uFailure = FALSE;
    uint_t uTest;

    {
        IntTest64x64_t* pIntTest = new IntTest64x64_t;
        Burger::unique_ptr<IntTest64x64_t> Test1(pIntTest);

        uTest = Test1.get() != pIntTest;
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

static uint_t BURGER_API Testice_and(void)
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

static uint_t BURGER_API Testice_or(void)
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

static uint_t BURGER_API Testice_eq(void)
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

static uint_t BURGER_API Testice_ne(void)
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

static uint_t BURGER_API Testice_not(void)
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

    Test algorithms

***************************************/

uint_t BURGER_API TestBralgorithm(uint_t uVerbose)
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

    uResult |= Testintegral_constant();
    uResult |= Testis_same();
    uResult |= Testunique_ptr();
    uResult |= Testice_and();
    uResult |= Testice_or();
    uResult |= Testice_eq();
    uResult |= Testice_ne();
    uResult |= Testice_not();

    if (!uResult && (uVerbose & VERBOSE_MSG)) {
        Message("Passed all Algorithm tests!");
    }
    return uResult;
}
