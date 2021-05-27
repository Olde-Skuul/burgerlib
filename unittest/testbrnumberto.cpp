/***************************************

    Unit tests for the number to ascii functions

    Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "testbrnumberto.h"
#include "brnumberto.h"
#include "brstringfunctions.h"
#include "common.h"

/***************************************

    Test NumberToAsciiHex(char *, T)

***************************************/

static uint_t BURGER_API TestNumberToAsciiHex(void) BURGER_NOEXCEPT
{
    char Buffer[32];
    uint_t uFailure = FALSE;

    Burger::NumberToAsciiHex(Buffer, static_cast<uint8_t>(0x1));
    uint_t uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "01"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer, static_cast<uint8_t>(0x1)) = %s",
        uTest, Buffer);

    Burger::NumberToAsciiHex(Buffer, static_cast<int16_t>(0x123));
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "0123"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer, static_cast<int16_t>(0x123)) = %s",
        uTest, Buffer);

    Burger::NumberToAsciiHex(Buffer, static_cast<int32_t>(0x123));
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "00000123"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer, static_cast<int32_t>(0x123)) = %s",
        uTest, Buffer);

    Burger::NumberToAsciiHex(Buffer, static_cast<int64_t>(0x123333));
    uTest =
        static_cast<uint_t>(Burger::StringCompare(Buffer, "0000000000123333"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer, static_cast<int64_t>(0x123333)) = %s",
        uTest, Buffer);

    Burger::NumberToAsciiHex(Buffer, 2.0f);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "40000000"));
    uFailure |= uTest;
    ReportFailure("Burger::NumberToAsciiHex(Buffer, 2.0f) = %s", uTest, Buffer);

    Burger::NumberToAsciiHex(Buffer, 6.0);
    uTest =
        static_cast<uint_t>(Burger::StringCompare(Buffer, "4018000000000000"));
    uFailure |= uTest;
    ReportFailure("Burger::NumberToAsciiHex(Buffer, 6.0) = %s", uTest, Buffer);

    Burger::NumberToAsciiHex(Buffer, static_cast<char>(0));
    Burger::NumberToAsciiHex(Buffer, static_cast<short>(0));
    Burger::NumberToAsciiHex(Buffer, static_cast<int>(0));
    Burger::NumberToAsciiHex(Buffer, static_cast<long>(0));
    Burger::NumberToAsciiHex(Buffer, static_cast<unsigned char>(0));
    Burger::NumberToAsciiHex(Buffer, static_cast<unsigned short>(0));
    Burger::NumberToAsciiHex(Buffer, static_cast<unsigned int>(0));
    Burger::NumberToAsciiHex(Buffer, static_cast<unsigned long>(0));
    Burger::NumberToAsciiHex(Buffer, static_cast<uintptr_t>(0));
    return uFailure;
}

/***************************************

    Test NumberToAsciiHex(char *, T, uint_t)

***************************************/

static uint_t BURGER_API TestNumberToAsciiHexDigits(void) BURGER_NOEXCEPT
{
    char Buffer[32];
    uint_t uFailure = FALSE;
    uint_t uTest = 0;

    uint_t uLength = 1;
    do {
        Burger::NumberToAsciiHex(
            Buffer, static_cast<uint32_t>(0x12345678), uLength);
        uTest = static_cast<uint_t>(
            Burger::StringCompare(Buffer, &"12345678"[8 - uLength]));
        uFailure |= uTest;
        ReportFailure("Burger::NumberToAsciiHex(Buffer,0x12345678, %u) = %s",
            uTest, uLength, Buffer);

        Burger::NumberToAsciiHex(
            Buffer, static_cast<uint32_t>(3), uLength | Burger::LEADINGZEROS);
        uTest = static_cast<uint_t>(
            Burger::StringCompare(Buffer, &"00000003"[8 - uLength]));
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberToAsciiHex(Buffer, static_cast<uint32_t>(3), %u | Burger::LEADINGZEROS) = %s",
            uTest, uLength, Buffer);

    } while (++uLength < 9);

    uLength = 1;
    do {
        Burger::NumberToAsciiHex(
            Buffer, static_cast<uint64_t>(0x123456789ABCDEF0ULL), uLength);
        uTest = static_cast<uint_t>(
            Burger::StringCompare(Buffer, &"123456789ABCDEF0"[16 - uLength]));
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberToAsciiHex(Buffer,0x123456789ABCDEF0, %u) = %s",
            uTest, uLength, Buffer);

        Burger::NumberToAsciiHex(
            Buffer, static_cast<uint64_t>(3), uLength | Burger::LEADINGZEROS);
        uTest = static_cast<uint_t>(
            Burger::StringCompare(Buffer, &"0000000000000003"[16 - uLength]));
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberToAsciiHex(Buffer, static_cast<uint64_t>(3), %u | Burger::LEADINGZEROS) = %s",
            uTest, uLength, Buffer);
    } while (++uLength < 17);

    // Test template for byte
    Burger::NumberToAsciiHex(Buffer, static_cast<int8_t>(1), 0);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "1"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer,static_cast<uint8_t>(1), %u) = %s",
        uTest, 0, Buffer);
    Burger::NumberToAsciiHex(
        Buffer, static_cast<uint8_t>(1), Burger::LEADINGZEROS);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "01"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer,static_cast<uint8_t>(1), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);

    // Test template for short
    Burger::NumberToAsciiHex(Buffer, static_cast<int16_t>(1), 0);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "1"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer,static_cast<uint16_t>(1), %u) = %s",
        uTest, 0, Buffer);
    Burger::NumberToAsciiHex(
        Buffer, static_cast<uint16_t>(1), Burger::LEADINGZEROS);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "0001"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer,static_cast<uint16_t>(1), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);

    // Test signed int32
    Burger::NumberToAsciiHex(
        Buffer, static_cast<int32_t>(1), Burger::LEADINGZEROS);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "00000001"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer,static_cast<int32_t>(1), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);

    // Test float
    Burger::NumberToAsciiHex(Buffer, static_cast<float>(1.26218e-29f), 4);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "11"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer,static_cast<float>(1.26218e-29f), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);
    Burger::NumberToAsciiHex(
        Buffer, static_cast<float>(1.26218e-29f), Burger::LEADINGZEROS);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "0F800011"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer,static_cast<float>(1.26218e-29f), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);

    // Test double
    Burger::NumberToAsciiHex(
        Buffer, static_cast<double>(8.12988915401011e-262), 4);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "5962"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer,static_cast<double>(8.12988915401011e-262), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);
    Burger::NumberToAsciiHex(Buffer, static_cast<double>(8.12988915401011e-262),
        Burger::LEADINGZEROS);
    uTest =
        static_cast<uint_t>(Burger::StringCompare(Buffer, "09B9999CFDE15962"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAsciiHex(Buffer,static_cast<double>(8.12988915401011e-262), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);

    Burger::NumberToAsciiHex(Buffer, static_cast<char>(0), 0);
    Burger::NumberToAsciiHex(Buffer, static_cast<short>(0), 0);
    Burger::NumberToAsciiHex(Buffer, static_cast<int>(0), 0);
    Burger::NumberToAsciiHex(Buffer, static_cast<long>(0), 0);
    Burger::NumberToAsciiHex(Buffer, static_cast<unsigned char>(0), 0);
    Burger::NumberToAsciiHex(Buffer, static_cast<unsigned short>(0), 0);
    Burger::NumberToAsciiHex(Buffer, static_cast<unsigned int>(0), 0);
    Burger::NumberToAsciiHex(Buffer, static_cast<unsigned long>(0), 0);
    Burger::NumberToAsciiHex(Buffer, static_cast<uintptr_t>(0), 0);
    return uFailure;
}

/***************************************

    Test NumberToAscii(char *, T, uint_t)

***************************************/

static uint_t BURGER_API TestNumberToAscii(void) BURGER_NOEXCEPT
{
    char Buffer[32];
    uint_t uFailure = FALSE;
    uint_t uTest = 0;

    uint_t uLength = 1;
    do {
        Burger::NumberToAscii(
            Buffer, static_cast<uint32_t>(335419896), uLength);
        uTest = static_cast<uint_t>(
            Burger::StringCompare(Buffer, &"335419896"[9 - uLength]));
        uFailure |= uTest;
        ReportFailure("Burger::NumberToAscii(Buffer,335419896, %u) = %s", uTest,
            uLength, Buffer);

        Burger::NumberToAscii(
            Buffer, static_cast<uint32_t>(3), uLength | Burger::LEADINGZEROS);
        uTest = static_cast<uint_t>(
            Burger::StringCompare(Buffer, &"000000003"[9 - uLength]));
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberToAscii(Buffer, static_cast<uint32_t>(3), %u | Burger::LEADINGZEROS) = %s",
            uTest, uLength, Buffer);

    } while (++uLength < 10);

    uLength = 1;
    do {
        Burger::NumberToAscii(
            Buffer, static_cast<uint64_t>(1311768467463794321ULL), uLength);
        uTest = static_cast<uint_t>(Burger::StringCompare(
            Buffer, &"1311768467463794321"[19 - uLength]));
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberToAscii(Buffer,1311768467463794321ULL, %u) = %s",
            uTest, uLength, Buffer);

        Burger::NumberToAscii(
            Buffer, static_cast<uint64_t>(3), uLength | Burger::LEADINGZEROS);
        uTest = static_cast<uint_t>(Burger::StringCompare(
            Buffer, &"0000000000000000003"[19 - uLength]));
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberToAscii(Buffer, static_cast<uint64_t>(3), %u | Burger::LEADINGZEROS) = %s",
            uTest, uLength, Buffer);
    } while (++uLength < 20);

    // Test template for byte
    Burger::NumberToAscii(Buffer, static_cast<int8_t>(-92), 0);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "-92"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAscii(Buffer,static_cast<uint8_t>(1), %u) = %s", uTest,
        0, Buffer);
    Burger::NumberToAscii(
        Buffer, static_cast<uint8_t>(1), Burger::LEADINGZEROS);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "001"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAscii(Buffer,static_cast<uint8_t>(1), %u) = %s", uTest,
        Burger::LEADINGZEROS, Buffer);

    // Test template for short
    Burger::NumberToAscii(Buffer, static_cast<int16_t>(-3921), 0);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "-3921"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAscii(Buffer,static_cast<uint16_t>(1), %u) = %s",
        uTest, 0, Buffer);
    Burger::NumberToAscii(
        Buffer, static_cast<uint16_t>(1), Burger::LEADINGZEROS);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "00001"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAscii(Buffer,static_cast<uint16_t>(1), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);

    Burger::NumberToAscii(
        Buffer, static_cast<int16_t>(-1), Burger::LEADINGZEROS);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "-00001"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAscii(Buffer,static_cast<int16_t>(-1), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);

    // Test signed int32
    Burger::NumberToAscii(
        Buffer, static_cast<int32_t>(1), Burger::LEADINGZEROS);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "0000000001"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAscii(Buffer,static_cast<int32_t>(1), %u) = %s", uTest,
        Burger::LEADINGZEROS, Buffer);

    // Test float
    Burger::NumberToAscii(Buffer, static_cast<float>(1.25f), 4);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "1.250000"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAscii(Buffer,static_cast<float>(1.25f), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);
    Burger::NumberToAscii(
        Buffer, static_cast<float>(1.26f), Burger::LEADINGZEROS);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "1.260000"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAscii(Buffer,static_cast<float>(1.26218e-29f), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);

    // Test double
    Burger::NumberToAscii(Buffer, static_cast<double>(1.667), 4);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "1.667000"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAscii(Buffer,static_cast<double>(1.667), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);

    Burger::NumberToAscii(
        Buffer, static_cast<double>(1.667), Burger::LEADINGZEROS);
    uTest = static_cast<uint_t>(Burger::StringCompare(Buffer, "1.667000"));
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberToAscii(Buffer,static_cast<double>(1.667), %u) = %s",
        uTest, Burger::LEADINGZEROS, Buffer);

    Burger::NumberToAscii(Buffer, static_cast<char>(0));
    Burger::NumberToAscii(Buffer, static_cast<short>(0));
    Burger::NumberToAscii(Buffer, static_cast<int>(0));
    Burger::NumberToAscii(Buffer, static_cast<long>(0));
    Burger::NumberToAscii(Buffer, static_cast<unsigned char>(0));
    Burger::NumberToAscii(Buffer, static_cast<unsigned short>(0));
    Burger::NumberToAscii(Buffer, static_cast<unsigned int>(0));
    Burger::NumberToAscii(Buffer, static_cast<unsigned long>(0));
    Burger::NumberToAscii(Buffer, static_cast<uintptr_t>(0));
    Burger::NumberToAscii(Buffer, static_cast<float>(0));
    Burger::NumberToAscii(Buffer, static_cast<double>(0));

    Burger::NumberToAscii(Buffer, static_cast<char>(0), 0);
    Burger::NumberToAscii(Buffer, static_cast<short>(0), 0);
    Burger::NumberToAscii(Buffer, static_cast<int>(0), 0);
    Burger::NumberToAscii(Buffer, static_cast<long>(0), 0);
    Burger::NumberToAscii(Buffer, static_cast<unsigned char>(0), 0);
    Burger::NumberToAscii(Buffer, static_cast<unsigned short>(0), 0);
    Burger::NumberToAscii(Buffer, static_cast<unsigned int>(0), 0);
    Burger::NumberToAscii(Buffer, static_cast<unsigned long>(0), 0);
    Burger::NumberToAscii(Buffer, static_cast<uintptr_t>(0), 0);
    Burger::NumberToAscii(Buffer, static_cast<float>(0), 0);
    Burger::NumberToAscii(Buffer, static_cast<double>(0), 0);
    return uFailure;
}

/***************************************

    Test NumberStringLength(T)

***************************************/

struct NumberStringTests32_t {
    uint32_t m_uValue;
    uint_t m_uLength;
};

static const NumberStringTests32_t g_NumberStringTests[] = {{0, 1}, {9, 1},
    {92, 2}, {999, 3}, {1000, 4}, {12345, 5}, {683821, 6}, {9999999, 7},
    {10000000, 8}, {400000000, 9}, {2147483647, 10}};

static uint_t BURGER_API TestNumberStringLength(void) BURGER_NOEXCEPT
{
    uint_t uFailure = FALSE;
    uint_t uTest = 0;
    uint_t uResult = 0;

    // Test with a range of values
    const NumberStringTests32_t* pWork = g_NumberStringTests;
    uintptr_t uCount = BURGER_ARRAYSIZE(g_NumberStringTests);
    do {
        uint_t uLength = pWork->m_uLength;
        uResult = Burger::NumberStringLength(pWork->m_uValue);
        uTest = uResult != uLength;
        uFailure |= uTest;
        ReportFailure("Burger::NumberStringLength(%u) = %u, expected %u", uTest,
            pWork->m_uValue, uResult, uLength);

        uResult =
            Burger::NumberStringLength(static_cast<uint64_t>(pWork->m_uValue));
        uTest = uResult != uLength;
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberStringLength(static_cast<uint64_t>(%u)) = %u, expected %u",
            uTest, pWork->m_uValue, uResult, uLength);

        // Add space for the '-' sign except for the case of zero.
        if (pWork->m_uValue) {
            ++uLength;
        }
        uResult =
            Burger::NumberStringLength(-static_cast<int32_t>(pWork->m_uValue));
        uTest = uResult != uLength;
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberStringLength(static_cast<int32_t>(%u)) = %u, expected %u",
            uTest, pWork->m_uValue, uResult, uLength);

        uResult =
            Burger::NumberStringLength(-static_cast<int64_t>(pWork->m_uValue));
        uTest = uResult != uLength;
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberStringLength(static_cast<int64_t>(%u)) = %u, expected %u",
            uTest, pWork->m_uValue, uResult, uLength);

        ++pWork;
    } while (--uCount);

    // Test template cases
    uResult = Burger::NumberStringLength(static_cast<int8_t>(-25));
    uTest = uResult != 3;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberStringLength(static_cast<int8_t>(-25)) = %u, expected 3",
        uTest, uResult);

    uResult = Burger::NumberStringLength(static_cast<uint8_t>(44));
    uTest = uResult != 2;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberStringLength(static_cast<uint8_t>(44)) = %u, expected 2",
        uTest, uResult);

    uResult = Burger::NumberStringLength(-25);
    uTest = uResult != 3;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberStringLength(-25) = %u, expected 3", uTest, uResult);

    uResult = Burger::NumberStringLength(44U);
    uTest = uResult != 2;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberStringLength(44U) = %u, expected 2", uTest, uResult);

    uResult = Burger::NumberStringLength(44ULL);
    uTest = uResult != 2;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberStringLength(44ULL) = %u, expected 2", uTest, uResult);

    uResult = Burger::NumberStringLength(9999999999999999999ULL);
    uTest = uResult != 19;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberStringLength(9999999999999999999ULL) = %u, expected 19",
        uTest, uResult);

    uResult = Burger::NumberStringLength(10000000000000000000ULL);
    uTest = uResult != 20;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberStringLength(10000000000000000000ULL) = %u, expected 19",
        uTest, uResult);

    return uFailure;
}

/***************************************

    Test NumberHexStringLength(T)

***************************************/

struct NumberHexStringTests32_t {
    uint32_t m_uValue;
    uint_t m_uLength;
};

static const NumberHexStringTests32_t g_NumberHexStringTests[] = {
    {0x00000000, 1}, {0x00000009, 1}, {0x00000092, 2}, {0x00000999, 3},
    {0x00001000, 4}, {0x00012345, 5}, {0x00683821, 6}, {0x09999999, 7},
    {0x10000000, 8}, {0x40000000, 8}, {0x7FFFFFFF, 8}};

static uint_t BURGER_API TestNumberHexStringLength(void) BURGER_NOEXCEPT
{
    uint_t uFailure = FALSE;
    uint_t uTest = 0;
    uint_t uResult = 0;

    // Test with a range of values
    const NumberHexStringTests32_t* pWork = g_NumberHexStringTests;
    uintptr_t uCount = BURGER_ARRAYSIZE(g_NumberHexStringTests);
    do {
        const uint_t uLength = pWork->m_uLength;
        uResult = Burger::NumberHexStringLength(pWork->m_uValue);
        uTest = uResult != uLength;
        uFailure |= uTest;
        ReportFailure("Burger::NumberHexStringLength(%X) = %u, expected %u",
            uTest, pWork->m_uValue, uResult, uLength);

        uResult = Burger::NumberHexStringLength(
            static_cast<uint64_t>(pWork->m_uValue));
        uTest = uResult != uLength;
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberHexStringLength(static_cast<uint64_t>(%X)) = %u, expected %u",
            uTest, pWork->m_uValue, uResult, uLength);

        ++pWork;
    } while (--uCount);

    // Test template cases
    uResult = Burger::NumberHexStringLength(static_cast<uint8_t>(0x44));
    uTest = uResult != 2;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberHexStringLength(static_cast<uint8_t>(0x44)) = %u, expected 2",
        uTest, uResult);

    uResult = Burger::NumberHexStringLength(static_cast<uint16_t>(0x123));
    uTest = uResult != 3;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberHexStringLength(static_cast<uint16_t>(0x123U)) = %u, expected 3",
        uTest, uResult);

    uResult = Burger::NumberHexStringLength(0x44U);
    uTest = uResult != 2;
    uFailure |= uTest;
    ReportFailure("Burger::NumberHexStringLength(0x44U) = %u, expected 2",
        uTest, uResult);

    uResult = Burger::NumberHexStringLength(0x44ULL);
    uTest = uResult != 2;
    uFailure |= uTest;
    ReportFailure("Burger::NumberHexStringLength(0x44ULL) = %u, expected 2",
        uTest, uResult);

    uResult = Burger::NumberHexStringLength(0x9999999999999999ULL);
    uTest = uResult != 16;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberHexStringLength(0x9999999999999999ULL) = %u, expected 16",
        uTest, uResult);

    uResult = Burger::NumberHexStringLength(0xFFFFFFFFFFFFFFFFULL);
    uTest = uResult != 16;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberHexStringLength(0xFFFFFFFFFFFFFFFFULL) = %u, expected 16",
        uTest, uResult);

    return uFailure;
}

/***************************************

    Test NumberOctalStringLength(T)

***************************************/

struct NumberOctalStringTests32_t {
    uint32_t m_uValue;
    uint_t m_uLength;
};

static const NumberOctalStringTests32_t g_NumberOctalStringTests[] = {
    {0x00000000, 1}, {0x00000009, 2}, {0x00000092, 3}, {0x00000999, 4},
    {0x00001000, 5}, {0x00012345, 6}, {0x00683821, 8}, {0x09999999, 10},
    {0x10000000, 10}, {0x40000000, 11}, {0x7FFFFFFF, 11}};

static uint_t BURGER_API TestNumberOctalStringLength(void) BURGER_NOEXCEPT
{
    uint_t uFailure = FALSE;
    uint_t uTest = 0;
    uint_t uResult = 0;

    // Test with a range of values
    const NumberOctalStringTests32_t* pWork = g_NumberOctalStringTests;
    uintptr_t uCount = BURGER_ARRAYSIZE(g_NumberOctalStringTests);
    do {
        const uint_t uLength = pWork->m_uLength;
        uResult = Burger::NumberOctalStringLength(pWork->m_uValue);
        uTest = uResult != uLength;
        uFailure |= uTest;
        ReportFailure("Burger::NumberOctalStringLength(%X) = %u, expected %u",
            uTest, pWork->m_uValue, uResult, uLength);

        uResult = Burger::NumberOctalStringLength(
            static_cast<uint64_t>(pWork->m_uValue));
        uTest = uResult != uLength;
        uFailure |= uTest;
        ReportFailure(
            "Burger::NumberOctalStringLength(static_cast<uint64_t>(%X)) = %u, expected %u",
            uTest, pWork->m_uValue, uResult, uLength);

        ++pWork;
    } while (--uCount);

    // Test template cases
    uResult = Burger::NumberOctalStringLength(static_cast<uint8_t>(0x44));
    uTest = uResult != 3;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberOctalStringLength(static_cast<uint8_t>(0x44)) = %u, expected 3",
        uTest, uResult);

    uResult = Burger::NumberOctalStringLength(static_cast<uint16_t>(0x123));
    uTest = uResult != 3;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberOctalStringLength(static_cast<uint16_t>(0x123U)) = %u, expected 3",
        uTest, uResult);

    uResult = Burger::NumberOctalStringLength(0x44U);
    uTest = uResult != 3;
    uFailure |= uTest;
    ReportFailure("Burger::NumberOctalStringLength(0x44U) = %u, expected 3",
        uTest, uResult);

    uResult = Burger::NumberOctalStringLength(0x44ULL);
    uTest = uResult != 3;
    uFailure |= uTest;
    ReportFailure("Burger::NumberOctalStringLength(0x44ULL) = %u, expected 3",
        uTest, uResult);

    uResult = Burger::NumberOctalStringLength(0x9999999999999999ULL);
    uTest = uResult != 22;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberOctalStringLength(0x9999999999999999ULL) = %u, expected 22",
        uTest, uResult);

    uResult = Burger::NumberOctalStringLength(0xFFFFFFFFFFFFFFFFULL);
    uTest = uResult != 22;
    uFailure |= uTest;
    ReportFailure(
        "Burger::NumberOctalStringLength(0xFFFFFFFFFFFFFFFFULL) = %u, expected 22",
        uTest, uResult);

    return uFailure;
}

//
// Perform all the tests for the Burgerlib Endian Manager
//

int BURGER_API TestBrnumberto(uint_t uVerbose) BURGER_NOEXCEPT
{
    if (uVerbose & VERBOSE_MSG) {
        Message("Running Number to tests");
    }

    uint_t uResult = TestNumberToAsciiHex();
    uResult |= TestNumberToAsciiHexDigits();
    uResult |= TestNumberToAscii();
    uResult |= TestNumberStringLength();
    uResult |= TestNumberHexStringLength();
    uResult |= TestNumberOctalStringLength();

    // Intentional failures
    // char Buffer[32];
    // Burger::NumberToAsciiHex(Buffer, "Will not compile");

    if (!uResult && (uVerbose & VERBOSE_MSG)) {
        Message("Passed all Number to tests!");
    }

    return static_cast<int>(uResult);
}
