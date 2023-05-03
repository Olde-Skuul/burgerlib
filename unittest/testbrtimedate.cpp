/***************************************

	Unit tests for the Date/Time manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrtimedate.h"
#include "brcapturestdout.h"
#include "brfloatingpoint.h"
#include "brnumberstringhex.h"
#include "brstdouthelpers.h"
#include "brstructs.h"
#include "brthread.h"
#include "brtick.h"
#include "brtimedate.h"
#include "common.h"

/***************************************

	Test the clear function

***************************************/

static uint_t BURGER_API TestBurgerTimeDateClear(void) BURGER_NOEXCEPT
{
	Burger::TimeDate_t Buffer[3];

	BlastBuffer(Buffer, sizeof(Buffer));
	Buffer[1].Clear();
	uint_t uFailure =
		VerifyBuffer(Buffer, sizeof(Buffer), &Buffer[1], sizeof(Buffer[1]));
	if (Buffer[1].m_uYear || Buffer[1].m_bMonth || Buffer[1].m_bDay ||
		Buffer[1].m_bDayOfWeek || Buffer[1].m_bHour || Buffer[1].m_bMinute ||
		Buffer[1].m_bSecond || Buffer[1].m_usMilliseconds) {
		uFailure = TRUE;
	}
	ReportFailure("Burger::TimeDate_t::Clear() didn't set all entries to zero.",
		uFailure);
	return uFailure;
}

/***************************************

	Test creating the day of the week

***************************************/

static uint_t BURGER_API TestBurgerTimeCalcDayOfWeek(void) BURGER_NOEXCEPT
{
	Burger::TimeDate_t Test;

	Test.m_bMonth = 10;
	Test.m_bDay = 30;
	Test.m_uYear = 1963;
	uint_t uResult = Test.CalcDayOfWeek();
	uint_t uTest = uResult != 3;
	uint_t uFailure = uTest;
	ReportFailure(
		"Burger::TimeDate_t::CalcDayOfWeek(10-30-1963) didn't return Wednesday, got %u.",
		uFailure, uResult);

	Test.m_bMonth = 4;
	Test.m_bDay = 7;
	Test.m_uYear = 2022;
	uResult = Test.CalcDayOfWeek();
	uTest = uResult != 4;
	uFailure = uTest;
	ReportFailure(
		"Burger::TimeDate_t::CalcDayOfWeek(4-7-2022) didn't return Thursday, got %u.",
		uFailure, uResult);

	Test.m_bMonth = 8;
	Test.m_bDay = 29;
	Test.m_uYear = 1997;
	uResult = Test.CalcDayOfWeek();
	uTest = uResult != 5;
	uFailure = uTest;
	ReportFailure(
		"Burger::TimeDate_t::CalcDayOfWeek(8-29-1997) didn't return Friday, got %u.",
		uFailure, uResult);

	return uFailure;
}

//
// Perform all the tests for the Burgerlib Time Manager
//

int BURGER_API TestDateTime(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running Date/Time tests");
	}

	// Test compiler switches
	uint_t uResult = TestBurgerTimeDateClear();
	uResult |= TestBurgerTimeCalcDayOfWeek();

	if (!uResult && (uVerbose & VERBOSE_TIME)) {
		char Buffer[256];
		Burger::TimeDate_t MyDate;
		MyDate.GetTime();
		MyDate.DateToStringVerbose(Buffer);
		Message("TimeDate_t::DateToStringVerbose() %s", Buffer);
		MyDate.DateToString(Buffer);
		Message("TimeDate_t::DateToString() %s", Buffer);
		MyDate.TimeToString(Buffer);
		Message("TimeDate_t::TimeToString() %s", Buffer);
		MyDate.TimeToStringPM(Buffer);
		Message("TimeDate_t::TimeToStringPM() %s", Buffer);
	}
	return static_cast<int>(uResult);
}

//
// Test PrintHexDigit(uint_t)
//

static uint_t BURGER_API TestPrintHexDigit(void) BURGER_NOEXCEPT
{
	// Capture the string output
	Burger::CaptureStdout Capture;
	if (Capture.Init(512)) {
		return FALSE;
	}
	Burger::PrintHexDigit(0x12345678);
	Burger::String CapturedString;
	Capture.GetCapture(&CapturedString);
	Capture.Shutdown();

	const char* pExpected = "8";
	const uint_t uTest =
		Burger::StringCompare(CapturedString.c_str(), pExpected) != 0;
	ReportFailure("Burger::PrintHexDigit(0x12345678) = %s, expected %s", uTest,
		CapturedString.c_str(), pExpected);
	return uTest;
}

//
// Test PrintHex(uint8_t)
//

static uint_t BURGER_API TestPrintHexWord8(void) BURGER_NOEXCEPT
{
	// Capture the string output
	Burger::CaptureStdout Capture;
	if (Capture.Init(512)) {
		return FALSE;
	}
	Burger::PrintHex(static_cast<uint8_t>(0x12));
	Burger::String CapturedString;
	Capture.GetCapture(&CapturedString);
	Capture.Shutdown();

	const char* pExpected = "12";
	const uint_t uTest =
		Burger::StringCompare(CapturedString.c_str(), pExpected) != 0;
	ReportFailure(
		"Burger::PrintHex(static_cast<uint8_t>(0x12)) = %s, expected %s", uTest,
		CapturedString.c_str(), pExpected);
	return uTest;
}

//
// Test PrintHex(uint16_t)
//

static uint_t BURGER_API TestPrintHexWord16(void) BURGER_NOEXCEPT
{
	// Capture the string output
	Burger::CaptureStdout Capture;
	if (Capture.Init(512)) {
		return FALSE;
	}
	Burger::PrintHex(static_cast<uint16_t>(0x1234));
	Burger::String CapturedString;
	Capture.GetCapture(&CapturedString);
	Capture.Shutdown();

	const char* pExpected = "1234";
	const uint_t uTest =
		Burger::StringCompare(CapturedString.c_str(), pExpected) != 0;
	ReportFailure(
		"Burger::PrintHex(static_cast<uint16_t>(0x1234)) = %s, expected %s",
		uTest, CapturedString.c_str(), pExpected);
	return uTest;
}

//
// Test PrintHex(uint32_t)
//

static uint_t BURGER_API TestPrintHexWord32(void) BURGER_NOEXCEPT
{
	// Capture the string output
	Burger::CaptureStdout Capture;
	if (Capture.Init(512)) {
		return FALSE;
	}
	Burger::PrintHex(static_cast<uint32_t>(0x12345678));
	Burger::String CapturedString;
	Capture.GetCapture(&CapturedString);
	Capture.Shutdown();

	const char* pExpected = "12345678";
	const uint_t uTest =
		Burger::StringCompare(CapturedString.c_str(), pExpected) != 0;
	ReportFailure(
		"Burger::PrintHex(static_cast<uint32_t>(0x12345678)) = %s, expected %s",
		uTest, CapturedString.c_str(), pExpected);
	return uTest;
}

//
// Test PrintHex(uint64_t)
//

static uint_t BURGER_API TestPrintHexWord64(void) BURGER_NOEXCEPT
{
	// Capture the string output
	Burger::CaptureStdout Capture;
	if (Capture.Init(512)) {
		return FALSE;
	}
	Burger::PrintHex(static_cast<uint64_t>(0x123456789ABCDEFULL));
	Burger::String CapturedString;
	Capture.GetCapture(&CapturedString);
	Capture.Shutdown();

	const char* pExpected = "0123456789ABCDEF";
	const uint_t uTest =
		Burger::StringCompare(CapturedString.c_str(), pExpected) != 0;
	ReportFailure(
		"Burger::PrintHex(static_cast<uint64_t>(0x123456789ABCDEFULL)) = %s, expected %s",
		uTest, CapturedString.c_str(), pExpected);
	return uTest;
}

//
// Test PrintHex(float)
//

struct PrintFloat_t {
	uint32_t m_uData;
	const char* m_pData;
};

static const PrintFloat_t PrintHexFloatTestTable[] = {
	{0x7FFFFFFF, "7FFFFFFF"}, {0x3F800000, "3F800000"}};

static uint_t BURGER_API TestPrintHexFloat(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const PrintFloat_t* pWork = PrintHexFloatTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(PrintHexFloatTestTable);
	do {
		// Capture the string output
		Burger::CaptureStdout Capture;
		if (Capture.Init(512)) {
			break;
		}
		Burger::PrintHex(static_cast<const Burger::uint32_float_t*>(
			static_cast<const void*>(&pWork->m_uData))
							 ->f);
		Burger::String CapturedString;
		Capture.GetCapture(&CapturedString);
		Capture.Shutdown();

		const uint_t uTest =
			Burger::StringCompare(CapturedString.c_str(), pWork->m_pData) != 0;
		ReportFailure(
			"Burger::PrintHex(static_cast<float>(0x%08X)) = %s, expected %s",
			uTest, pWork->m_uData, CapturedString.c_str(), pWork->m_pData);
		uFailure |= uTest;
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test PrintHex(double)
//

struct PrintDouble_t {
	uint64_t m_uData;
	const char* m_pData;
};

static const PrintDouble_t PrintHexDoubleTestTable[] = {
	{0x7FFFFFFFFFFFFFFFULL, "7FFFFFFFFFFFFFFF"},
	{0x3FF0000000000000ULL, "3FF0000000000000"}};

static uint_t BURGER_API TestPrintHexDouble(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const PrintDouble_t* pWork = PrintHexDoubleTestTable;
	uintptr_t uCount = BURGER_ARRAYSIZE(PrintHexDoubleTestTable);
	do {
		// Capture the string output
		Burger::CaptureStdout Capture;
		if (Capture.Init(512)) {
			break;
		}
		Burger::PrintHex(static_cast<const Burger::uint64_double_t*>(
			static_cast<const void*>(&pWork->m_uData))
							 ->d);
		Burger::String CapturedString;
		Capture.GetCapture(&CapturedString);
		Capture.Shutdown();

		const uint_t uTest =
			Burger::StringCompare(CapturedString.c_str(), pWork->m_pData) != 0;
		if (uTest) {
			Burger::NumberStringHex Input(pWork->m_uData);
			ReportFailure(
				"Burger::PrintHex(static_cast<double>(0x%s)) = %s, expected %s",
				uTest, Input.c_str(), CapturedString.c_str(), pWork->m_pData);
		}
		uFailure |= uTest;
		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Test for generic types for proper remapping
//

static void TestPrintHexes(void) BURGER_NOEXCEPT
{
	Burger::CaptureStdout Capture;
	const Burger::eError uError = Capture.Init(1024);

	// Only process tests if text capture is supported on this platform.
	if (uError == Burger::kErrorNone) {
		Burger::PrintHex('a');
#if defined(BURGER_HAS_WCHAR_T)
		Burger::PrintHex(L'a');
#endif

		Burger::PrintHex(1);
		Burger::PrintHex(1U);
#if !defined(BURGER_LINUX) && !defined(BURGER_SWITCH)
		Burger::PrintHex(1LL);
		Burger::PrintHex(1ULL);
#endif
		Burger::PrintHex(1.0);
		Burger::PrintHex(1.0f);

		Burger::PrintHex(static_cast<char>(1));
		Burger::PrintHex(static_cast<signed char>(1));
		Burger::PrintHex(static_cast<unsigned char>(1));

		Burger::PrintHex(static_cast<short>(1));
		Burger::PrintHex(static_cast<signed short>(1));
		Burger::PrintHex(static_cast<unsigned short>(1));

		Burger::PrintHex(static_cast<int>(1));
		Burger::PrintHex(static_cast<unsigned int>(1));
		Burger::PrintHex(static_cast<signed char>(1));

		Burger::PrintHex(static_cast<long>(1));
		Burger::PrintHex(static_cast<unsigned long>(1));
		Burger::PrintHex(static_cast<signed long>(1));

#if !defined(BURGER_LINUX) && !defined(BURGER_SWITCH)
		Burger::PrintHex(static_cast<long long>(1));
		Burger::PrintHex(static_cast<unsigned long long>(1));
		Burger::PrintHex(static_cast<signed long long>(1));
#endif
	}
}

/***************************************

	Test the Tick manager

***************************************/

static void BURGER_API TestTick(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_TIME) {
		Message("Testing Tick::read()");
		uint32_t uMark = Burger::Tick::read() + Burger::Tick::kTicksPerSecond;
		uint32_t uCount = 1;
		do {
			uint32_t uNewMark;
			do {
				uNewMark = Burger::Tick::read();
			} while (uNewMark < uMark);
			uMark += Burger::Tick::kTicksPerSecond;
			Message("Tick %u is 0x%08X", uCount, uNewMark);
			fflush(stdout);
		} while (++uCount < 6);

		Message("Testing Tick::read_ms()");
		uMark = Burger::Tick::read_ms() + 1000U;
		uCount = 1;
		do {
			uint32_t uNewMark;
			do {
				uNewMark = Burger::Tick::read_ms();
			} while (uNewMark < uMark);
			uMark += 1000U;
			Message("Tick %u is 0x%08X", uCount, uNewMark);
			fflush(stdout);
		} while (++uCount < 6);

		Message("Testing Tick::read_us()");
		uMark = Burger::Tick::read_us() + 1000000U;
		uCount = 1;
		do {
			uint32_t uNewMark;
			do {
				uNewMark = Burger::Tick::read_us();
			} while (uNewMark < uMark);
			uMark += 1000000U;
			Message("Tick %u is 0x%08X", uCount, uNewMark);
			fflush(stdout);
		} while (++uCount < 6);
	}
}

// Used for thread manager testing
static uintptr_t BURGER_API Code(void* pInput) BURGER_NOEXCEPT
{
	++static_cast<uint32_t*>(pInput)[0];
	return static_cast<uint32_t*>(pInput)[0] + 10000;
}

/***************************************

	Test the Thread manager

***************************************/

static uint_t BURGER_API TestThread(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;

	if (uVerbose & VERBOSE_MSG) {
		Message("Testing Threads");
	}

	uint32_t uValue = 666;
	Burger::Thread bar;
	Burger::eError uResult = bar.start(Code, &uValue, "Thread1");
	if (uResult == Burger::kErrorNotSupportedOnThisPlatform) {
		Message("Threading not available on this platform");
	} else {
		bar.wait();

		uint_t uTest = uValue != 667;
		uFailure |= uTest;
		ReportFailure("Thread(666) returned %u, expected 667", uTest, uValue);

		uintptr_t uThreadResult = bar.get_result();
		uTest = uThreadResult != 10667;
		uFailure |= uTest;
		ReportFailure("Thread(666).get_result() returned %u, expected 10667",
			uTest, static_cast<uint32_t>(uThreadResult));

		uValue = 9999;
		Burger::Thread bar2;
		bar2.start(Code, &uValue, "Thread2");
		bar2.wait();

		uTest = uValue != 10000;
		uFailure |= uTest;
		ReportFailure(
			"Thread(9999) returned %u, expected 10000", uTest, uValue);

		uThreadResult = bar2.get_result();
		uTest = uThreadResult != 20000;
		uFailure |= uTest;
		ReportFailure("Thread(9999).get_result() returned %u, expected 20000",
			uTest, static_cast<uint32_t>(uThreadResult));
	}
	return uFailure;
}

//
// Perform all the tests for the Burgerlib Stdouthelpers Manager
//

int BURGER_API TestStdoutHelpers(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running Stdout Helpers tests");
	}

	uint_t uResult = TestPrintHexDigit();
	uResult |= TestPrintHexWord8();
	uResult |= TestPrintHexWord16();
	uResult |= TestPrintHexWord32();
	uResult |= TestPrintHexWord64();
	uResult |= TestPrintHexFloat();
	uResult |= TestPrintHexDouble();
	TestTick(uVerbose);
	uResult |= TestThread(uVerbose);
	TestPrintHexes();

	if (!uResult && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all Stdout Helpers tests!");
	}
	return static_cast<int>(uResult);
}
