/***************************************

	Unit tests for the miscellaneous functions

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

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

//
// Perform all the tests for the Burgerlib Endian Manager
//

int BURGER_API TestBrstrings(void)
{	
	Word uTotal;	// Assume no failures

	Message("Running String tests");
	// Test compiler switches
	uTotal = TestWildcard();
	return static_cast<int>(uTotal);
}
