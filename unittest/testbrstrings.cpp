/***************************************

    Unit tests for the miscellaneous functions

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "testbrstrings.h"
#include "brglobalmemorymanager.h"
#include "brglobals.h"
#include "brosstringfunctions.h"
#include "brstringfunctions.h"
#include "common.h"

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

static uint_t BURGER_API TestWildcard(void)
{
	uint_t uFailure = FALSE;
	const Wildcard_t* pWork = s_Wildcards;
	uintptr_t uCount = BURGER_ARRAYSIZE(s_Wildcards);
	do {
		uint32_t uTester = Burger::Wildcardcmp(pWork->m_pTest, pWork->m_pWild);
		uint_t uTest = (uTester != pWork->m_uResult);
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

static uint_t BURGER_API TestParseQuotedString(void)
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
		uTest |= Burger::StringCompare(Buffer, pWork->m_pWild);
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

	Test GetEnvironmentString()

***************************************/

#define SAILORMOON \
	"\xE7\xBE\x8E\xE5\xB0\x91\xE5\xA5\xB3\xE6\x88" \
	"\xA6\xE5\xA3\xAB\xE3\x82\xBB\xE3\x83\xBC\xE3\x83\xA9\xE3\x83\xBC" \
	"\xE3\x83\xA0\xE3\x83\xBC\xE3\x83\xB3"

static uint_t BURGER_API TestGetEnvironmentString(uint_t uVerbose)
{
#if defined(BURGER_LINUX) || defined(BURGER_MACOSX) || defined(BURGER_WINDOWS)
    uint_t uFailure = 0;
    const char* pTest = Burger::GetEnvironmentString("JAPANESE");
    if (pTest) {
        uint_t uTest =
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

static uint_t TestStringStopAt(void)
{
	uint_t uFailure = FALSE;
	const Wildcard_t* pWork = s_StringStopAts;
	uintptr_t uCount = BURGER_ARRAYSIZE(s_StringStopAts);
	do {
		uintptr_t uTester = Burger::StringStopAt(pWork->m_pTest, pWork->m_pWild);
		uint_t uTest = (uTester != pWork->m_uResult);
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

static uint_t TestStringSkipOver(void)
{
	uint_t uFailure = FALSE;
	const Wildcard_t* pWork = s_StringSkipOvers;
	uintptr_t uCount = BURGER_ARRAYSIZE(s_StringSkipOvers);
	do {
		uintptr_t uTester =
			Burger::StringSkipOver(pWork->m_pTest, pWork->m_pWild);
		uint_t uTest = (uTester != pWork->m_uResult);
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

static uint_t TestStringToken(void)
{
	char Test[] = "abc,foo,kill,me";
	uint_t uFailure = FALSE;
	const char** ppWork = s_Tokens;
	uintptr_t uCount = BURGER_ARRAYSIZE(s_Tokens);
	char* pTemp = nullptr;
	const char* pToken =
		Burger::StringToken(Test,"i,", &pTemp);
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
			ReportFailure("Burger::StringToken() = %s, expected %s",
				uTest, pToken,ppWork[0]);
		}
		++ppWork;
		pToken = Burger::StringToken(nullptr, "i,", &pTemp);
	} while (--uCount && pToken);
	return uFailure;
}

//
// Perform all the tests for the Burgerlib Endian Manager
//

int BURGER_API TestBrstrings(uint_t uVerbose)
{
	uint_t uTotal; // Assume no failures

	if (uVerbose & VERBOSE_MSG) {
		Message("Running String tests");
	}

    // Test compiler switches
    uTotal = TestWildcard();
    uTotal |= TestParseQuotedString();
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
        Message("GetMacModelIdentifier() returned \"%s\"", TempString.GetPtr());
#endif
    }

    if (!uTotal && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all String tests!");
	}

	return static_cast<int>(uTotal);
}
