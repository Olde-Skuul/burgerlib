/***************************************

	Unit tests for the miscellaneous functions

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrstrings.h"
#include "brglobalmemorymanager.h"
#include "brglobals.h"
#include "brmemoryfunctions.h"
#include "brosstringfunctions.h"
#include "brstringfunctions.h"
#include "common.h"

static char gLargeString[] =
	"This is a 100 character string "
	"This is a 100 character string "
	"This is a 100 character string woopie";

/***************************************

	Test the wild card compare function

***************************************/

struct Wildcard_t {
	const char* m_pTest;
	const char* m_pWild;
	uint_t m_uResult;
};

static const Wildcard_t s_Wildcards[] = {{"foo", "*o", FALSE},
	{"foo", "foo", FALSE}, {"foo", "*", FALSE}, {"", "*", FALSE},
	{"foo", "*oo", FALSE}, {"foo", "*f*", FALSE}, {"foo", "*fo*", FALSE},
	{"foo", "*f*o", FALSE}, {"foo", "*f*oo", FALSE}, {"foo", "*f*ooo", TRUE},
	{"foo", "*foo", FALSE}, {"foo", "?oo", FALSE}, {"foo", "f?o", FALSE},
	{"foo", "fo?", FALSE}, {"foo", "?fo?", TRUE}, {"foo", "*fo?", FALSE},
	{"fo", "*fo?", TRUE}, {"file.txt", "*.txt", FALSE},
	{"FILE.TXT", "*.txt", FALSE}};

static uint_t BURGER_API TestWildcard(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const Wildcard_t* pWork = s_Wildcards;
	uintptr_t uCount = BURGER_ARRAYSIZE(s_Wildcards);
	do {
		const uint32_t uTester =
			Burger::Wildcardcmp(pWork->m_pTest, pWork->m_pWild);
		const uint_t uTest = (uTester != pWork->m_uResult);
		uFailure |= uTest;
		if (uTest) {
			ReportFailure("Burger::Wildcardcmp(%s,%s) = %u, expected %u", uTest,
				pWork->m_pTest, pWork->m_pWild, uTester, pWork->m_uResult);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test the parse quoted string function

***************************************/

static const Wildcard_t s_QuotedStrings[] = {{"foo", "foo", 3},
	{"\"foo\"", "foo", 5}, {"'foo'", "foo", 5}, {"", "", 0}, {" ", "", 0},
	{"\n", "", 0}, {"\t", "", 0}, {" foo", "", 0}, {"\tfoo", "", 0},
	{"\"\"\"foo\"\"\"", "\"foo\"", 9}, {"'''foo'''", "'foo'", 9},
	{"\"\"\"\"foo\"\"\"\"", "\"", 4}, {"''''foo''''", "'", 4},
	{"foo bar", "foo", 3}, {"foo\tbar", "foo", 3}, {"'foo bar'", "foo bar", 9},
	{"'foo\tbar'", "foo bar", 9}};

static uint_t BURGER_API TestParseQuotedString(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const Wildcard_t* pWork = s_QuotedStrings;
	uintptr_t uCount = BURGER_ARRAYSIZE(s_QuotedStrings);
	char Buffer[256];
	do {
		char* pTester =
			Burger::ParseQuotedString(Buffer, sizeof(Buffer), pWork->m_pTest);
		// Check the end mark
		uint_t uTest = (pTester != (pWork->m_pTest + pWork->m_uResult));
		// Check the result string
		uTest |= static_cast<uint_t>(Burger::StringCompare(Buffer, pWork->m_pWild));
		uFailure |= uTest;
		if (uTest) {
			ReportFailure(
				"Burger::ParseQuotedString(Buffer,\"%s\") = \"%s\", expected \"%s\"",
				uTest, pWork->m_pTest, Buffer, pWork->m_pWild);
		} else {
			// Test the consumer function
			pTester = Burger::ParseQuotedString(NULL, 0, pWork->m_pTest);
			uTest = (pTester != (pWork->m_pTest + pWork->m_uResult));
			uFailure |= uTest;
			if (uTest) {
				ReportFailure(
					"Burger::ParseQuotedString(NULL,\"%s\") returned wrong pointer",
					uTest, pWork->m_pTest);
			}
		}

		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test String() constructors

***************************************/

static uint_t BURGER_API TestStringConstructors(void) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;
	Burger::String EmptyString;

	// Make sure it's empty
	uint_t uTest =
		!EmptyString.empty() || EmptyString.c_str()[0] || EmptyString.length();
	uFailure |= uTest;
	ReportFailure("String() = Did not yield an empty string", uTest);

	Burger::String ShortString("Shorty");
	uTest = ShortString.empty() || ShortString.length() != 6 ||
		Burger::StringCompare("Shorty", ShortString.c_str());
	uFailure |= uTest;
	ReportFailure("String(\"Shorty\") = Did not match \"Shorty\"", uTest);

	Burger::String LongString(gLargeString);
	uTest = LongString.empty() ||
		LongString.length() != (sizeof(gLargeString) - 1) ||
		Burger::StringCompare(gLargeString, LongString.c_str());
	uFailure |= uTest;
	ReportFailure("String(\"%s\") = Did not match \"%s\"", uTest, gLargeString,
		gLargeString);

	// Test clear with large string
	LongString.clear();
	uTest = !LongString.empty() || LongString.c_str()[0] || LongString.length();
	uFailure |= uTest;
	ReportFailure("String::clear() = Did not yield an empty string", uTest);

	Burger::String DualString(gLargeString, gLargeString);
	uTest = DualString.empty() ||
		DualString.length() != ((sizeof(gLargeString) - 1) * 2) ||
		Burger::MemoryCompare(
			gLargeString, DualString.c_str(), sizeof(gLargeString) - 1) ||
		Burger::StringCompare(
			gLargeString, DualString.c_str() + (sizeof(gLargeString) - 1));
	uFailure |= uTest;
	ReportFailure("String(\"%s\",\"%s\") = Did not match", uTest, gLargeString,
		gLargeString);

	return uFailure;
}

/***************************************

	Test String::append()

***************************************/

static uint_t BURGER_API TestStringAppend(void) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;
	Burger::String Sample(gLargeString);

	Sample.push_back('a');
	Sample.push_back('b');
	Sample.push_back('c');
	uint_t uTest = Sample.length() != (sizeof(gLargeString) + 2) ||
		Burger::StringCompare(
			Sample.c_str() + (sizeof(gLargeString) - 1), "abc");
	uFailure |= uTest;
	ReportFailure("String::push_back() failure = %s", uTest, Sample.c_str());

	Sample.append("unit test");
	uTest = Sample.length() != (sizeof(gLargeString) + 2 + 9) ||
		Burger::StringCompare(
			Sample.c_str() + ((sizeof(gLargeString) - 1) + 3), "unit test");
	uFailure |= uTest;
	ReportFailure(
		"String::append(const char *) failure = %s", uTest, Sample.c_str());

	return uFailure;
}

/***************************************

	Test String::find()

***************************************/

static uint_t BURGER_API TestStringFind(void) BURGER_NOEXCEPT
{
	uint_t uFailure = 0;

	Burger::String str("There are two needles in this haystack with needles.");
	const Burger::String str2("needle");

	uintptr_t uResult = str.find('y');
	uint_t uTest = uResult != 32;
	uFailure |= uTest;
	ReportFailure("String::find('y') failure = %u, should be 32", uTest,
		static_cast<uint_t>(uResult));

	uResult = str.find(str2);
	uTest = uResult != 14;
	uFailure |= uTest;
	ReportFailure("String::find(str2) failure = %u, should be 14", uTest,
		static_cast<uint_t>(uResult));

	uResult = str.find("needles are small", uResult + 1, 6);
	uTest = uResult != 44;
	uFailure |= uTest;
	ReportFailure(
		"String::find(needles are small, 6) failure = %u, should be 44", uTest,
		static_cast<uint_t>(uResult));

	uResult = str.find("haystack");
	uTest = uResult != 30;
	uFailure |= uTest;
	ReportFailure("String::find(haystack) failure = %u, should be 30", uTest,
		static_cast<uint_t>(uResult));

	uResult = str.find('.');
	uTest = uResult != 51;
	uFailure |= uTest;
	ReportFailure("String::find('.') failure = %u, should be 51", uTest,
		static_cast<uint_t>(uResult));

	// Test if it works with Remove()
	str.Remove(str.find(str2), str2.length());
	uTest = Burger::StringCompare(str.c_str(),
				"There are two s in this haystack with needles.") != 0;
	uFailure |= uTest;
	ReportFailure(
		"str.Remove(str.find(str2), str2.length()) failure = %s, should be "
		"\"There are two s in this haystack with needles.\"",
		uTest, str.c_str());

	return uFailure;
}

/***************************************

	Test GetEnvironmentString()

***************************************/

#define SAILORMOON \
	"\xE7\xBE\x8E\xE5\xB0\x91\xE5\xA5\xB3\xE6\x88" \
	"\xA6\xE5\xA3\xAB\xE3\x82\xBB\xE3\x83\xBC\xE3\x83\xA9\xE3\x83\xBC" \
	"\xE3\x83\xA0\xE3\x83\xBC\xE3\x83\xB3"

static uint_t BURGER_API TestGetEnvironmentString(
	uint_t uVerbose) BURGER_NOEXCEPT
{
#if defined(BURGER_LINUX) || defined(BURGER_MACOSX) || defined(BURGER_WINDOWS)
	uint_t uFailure = 0;
	const char* pTest = Burger::GetEnvironmentString("JAPANESE");
	if (pTest) {
		const uint_t uTest =
			static_cast<uint_t>(Burger::StringCompare(pTest, SAILORMOON));
		uFailure |= uTest;
		ReportFailure(
			"Burger::GetEnvironmentString(\"JAPANESE\") = \"%s\", expected \"%s\"",
			uTest, pTest, pTest, SAILORMOON);
		Burger::Free(pTest);
	} else {
		if (uVerbose & VERBOSE_MSG) {
			Message("JAPANESE environment variable not found");
		}
	}
	return uFailure;
#else
	BURGER_UNUSED(uVerbose);
	return 0;
#endif
}

/***************************************

	Test Burger::StringStopAt()

***************************************/

static const Wildcard_t s_StringStopAts[] = {{nullptr, "*o", 0},
	{"foo", nullptr, 3}, {"foo", "", 3}, {"", "*", 0}, {"foo", "*o", 1},
	{"foo", "f*", 0}, {"foo", "*fo*", 0}, {"FILE.TXT", "*.txt", 4}};

static uint_t TestStringStopAt(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const Wildcard_t* pWork = s_StringStopAts;
	uintptr_t uCount = BURGER_ARRAYSIZE(s_StringStopAts);
	do {
		const uintptr_t uTester =
			Burger::StringStopAt(pWork->m_pTest, pWork->m_pWild);
		const uint_t uTest = (uTester != pWork->m_uResult);
		uFailure |= uTest;
		if (uTest) {
			ReportFailure("Burger::StringStopAt(%s,%s) = %u, expected %u",
				uTest, pWork->m_pTest, pWork->m_pWild,
				static_cast<uint_t>(uTester), pWork->m_uResult);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::StringSkipOver()

***************************************/

static const Wildcard_t s_StringSkipOvers[] = {{nullptr, "*o", 0},
	{"foo", nullptr, 0}, {"foo", "", 0}, {"", "*", 0}, {"foo", "*o", 0},
	{"foo", "f*", 1}, {"foo", "*fo*", 3}, {"FILE.TXT", "*.txt", 0}};

static uint_t TestStringSkipOver(void) BURGER_NOEXCEPT
{
	uint_t uFailure = FALSE;
	const Wildcard_t* pWork = s_StringSkipOvers;
	uintptr_t uCount = BURGER_ARRAYSIZE(s_StringSkipOvers);
	do {
		const uintptr_t uTester =
			Burger::StringSkipOver(pWork->m_pTest, pWork->m_pWild);
		const uint_t uTest = (uTester != pWork->m_uResult);
		uFailure |= uTest;
		if (uTest) {
			ReportFailure("Burger::StringSkipOver(%s,%s) = %u, expected %u",
				uTest, pWork->m_pTest, pWork->m_pWild,
				static_cast<uint_t>(uTester), pWork->m_uResult);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test Burger::StringToken()

***************************************/

static const char* s_Tokens[] = {"abc", "foo", "k", "ll", "me"};

static uint_t TestStringToken(void) BURGER_NOEXCEPT
{
	char Test[] = "abc,foo,kill,me";
	uint_t uFailure = FALSE;
	const char** ppWork = s_Tokens;
	uintptr_t uCount = BURGER_ARRAYSIZE(s_Tokens);
	char* pTemp = nullptr;
	const char* pToken = Burger::StringToken(Test, "i,", &pTemp);
	do {
		uint_t uTest;
		if (!pToken) {
			uTest = TRUE;
			pToken = "No Token found";
		} else {
			uTest = Burger::StringCompare(pToken, ppWork[0]) != 0;
		}
		uFailure |= uTest;
		if (uTest) {
			ReportFailure("Burger::StringToken() = %s, expected %s", uTest,
				pToken, ppWork[0]);
		}
		++ppWork;
		pToken = Burger::StringToken(nullptr, "i,", &pTemp);
	} while (--uCount && pToken);
	return uFailure;
}

//
// Perform all the tests for the Burgerlib Endian Manager
//

int BURGER_API TestBrstrings(uint_t uVerbose) BURGER_NOEXCEPT
{
	uint_t uTotal; // Assume no failures

	if (uVerbose & VERBOSE_MSG) {
		Message("Running String tests");
	}

	// Test compiler switches
	uTotal = TestWildcard();
	uTotal |= TestParseQuotedString();
	uTotal |= TestStringConstructors();
	uTotal |= TestStringAppend();
	uTotal |= TestStringFind();
	uTotal |= TestGetEnvironmentString(uVerbose);
	uTotal |= TestStringStopAt();
	uTotal |= TestStringSkipOver();
	uTotal |= TestStringToken();

	if (uVerbose & VERBOSE_MSG) {
		Burger::String TempString;

		Burger::GetUserLoginName(&TempString);
		Message("GetUserLoginName() returned \"%s\"", TempString.c_str());

		Burger::GetUserRealName(&TempString);
		Message("GetUserRealName() returned \"%s\"", TempString.c_str());

		Burger::GetMachineName(&TempString);
		Message("GetMachineName() returned \"%s\"", TempString.c_str());

#if defined(BURGER_MACOS)
		Burger::GetMacModelIdentifier(&TempString);
		Message("GetMacModelIdentifier() returned \"%s\"", TempString.c_str());
#endif
	}

	if (!uTotal && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all String tests!");
	}

	return static_cast<int>(uTotal);
}
