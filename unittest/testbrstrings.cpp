/***************************************

	Unit tests for the miscellaneous functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrstrings.h"
#include "brstringfunctions.h"
#include "common.h"

/***************************************

	Test the wildcard compare function

***************************************/

struct Wildcard_t {
	const char *m_pTest;
	const char *m_pWild;
	Word m_uResult;
};

static const Wildcard_t s_Wildcards[] = {
	{"foo","*o",FALSE},
	{"foo","foo",FALSE},
	{"foo","*",FALSE},
	{"","*",FALSE},
	{"foo","*oo",FALSE},
	{"foo","*f*",FALSE},
	{"foo","*fo*",FALSE},
	{"foo","*f*o",FALSE},
	{"foo","*f*oo",FALSE},
	{"foo","*f*ooo",TRUE},
	{"foo","*foo",FALSE},
	{"foo","?oo",FALSE},
	{"foo","f?o",FALSE},
	{"foo","fo?",FALSE},
	{"foo","?fo?",TRUE},
	{"foo","*fo?",FALSE},
	{"fo","*fo?",TRUE},
	{"file.txt","*.txt",FALSE},
	{"FILE.TXT","*.txt",FALSE}
};

static Word TestWildcard(void)
{
	Word uFailure = FALSE;
	const Wildcard_t *pWork = s_Wildcards;
	WordPtr uCount = BURGER_ARRAYSIZE(s_Wildcards);
	do {
		Word32 uTester = Burger::Wildcardcmp(pWork->m_pTest,pWork->m_pWild);
		Word uTest = (uTester != pWork->m_uResult);
		uFailure |= uTest;
		if (uTest) {
			ReportFailure("Burger::Wildcardcmp(%s,%s) = %u, expected %u",uTest,pWork->m_pTest,pWork->m_pWild,uTester,pWork->m_uResult);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test the parse quoted string function

***************************************/

static const Wildcard_t s_QuotedStrings[] = {
	{"foo","foo",3},
	{"\"foo\"","foo",5},
	{"'foo'","foo",5},
	{"","",0},
	{" ","",0},
	{"\n","",0},
	{"\t","",0},
	{" foo","",0},
	{"\tfoo","",0},
	{"\"\"\"foo\"\"\"","\"foo\"",9},
	{"'''foo'''","'foo'",9},
	{"\"\"\"\"foo\"\"\"\"","\"",4},
	{"''''foo''''","'",4},
	{"foo bar","foo",3},
	{"foo\tbar","foo",3},
	{"'foo bar'","foo bar",9},
	{"'foo\tbar'","foo bar",9}
};

static Word TestParseQuotedString(void)
{
	Word uFailure = FALSE;
	const Wildcard_t *pWork = s_QuotedStrings;
	WordPtr uCount = BURGER_ARRAYSIZE(s_QuotedStrings);
	char Buffer[256];
	do {
		char *pTester = Burger::ParseQuotedString(Buffer,sizeof(Buffer),pWork->m_pTest);
		// Check the end mark
		Word uTest = (pTester != (pWork->m_pTest+pWork->m_uResult));
		// Check the result string
		uTest |= Burger::StringCompare(Buffer,pWork->m_pWild);
		uFailure |= uTest;
		if (uTest) {
			ReportFailure("Burger::ParseQuotedString(Buffer,\"%s\") = \"%s\", expected \"%s\"",uTest,pWork->m_pTest,Buffer,pWork->m_pWild);
		} else {
			// Test the consumer function
			pTester = Burger::ParseQuotedString(NULL,0,pWork->m_pTest);
			uTest = (pTester != (pWork->m_pTest+pWork->m_uResult));
			uFailure |= uTest;
			if (uTest) {
				ReportFailure("Burger::ParseQuotedString(NULL,\"%s\") returned wrong pointer",uTest,pWork->m_pTest);
			}
		}

		++pWork;
	} while (--uCount);
	return uFailure;
}

//
// Perform all the tests for the Burgerlib Endian Manager
//

int BURGER_API TestBrstrings(void)
{	
	Word uResult;	// Assume no failures

	Message("Running String tests");
	// Test compiler switches
	uResult = TestWildcard();
	uResult |= TestParseQuotedString();
	return static_cast<int>(uResult);
}
