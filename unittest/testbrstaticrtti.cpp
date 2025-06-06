/***************************************

	Unit tests for the Static RTTI library

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrstaticrtti.h"
#include "brstaticrtti.h"
#include "brstringfunctions.h"
#include "common.h"

using namespace Burger;

//
// Create classes to test RTTI
//

class foo {
public:
	foo() {}
	virtual ~foo() {}
	BURGER_RTTI_IN_BASE_CLASS();
};
class foo2: public foo {
public:
	foo2() {}
	virtual ~foo2() {}
	BURGER_RTTI_IN_CLASS();
};
class foo3: public foo2 {
public:
	foo3() {}
	virtual ~foo3() {}
	BURGER_RTTI_IN_CLASS();
};
class foo4: public foo2 {
public:
	foo4() {}
	virtual ~foo4() {}
	BURGER_RTTI_IN_CLASS();
};
class foo5: public foo {
public:
	foo5() {}
	virtual ~foo5() {}
	BURGER_RTTI_IN_CLASS();
};
BURGER_CREATE_STATICRTTI_BASE(foo);
BURGER_CREATE_STATICRTTI_PARENT(foo2, foo);
BURGER_CREATE_STATICRTTI_PARENT(foo3, foo2);
BURGER_CREATE_STATICRTTI_PARENT(foo4, foo2);
BURGER_CREATE_STATICRTTI_PARENT(foo5, foo);

//
// Brute force tests
//

static uint_t TestRTTI(void) BURGER_NOEXCEPT
{
	uint_t uResult = 0;
	foo bar;
	foo2 bar2;
	foo3 bar3;
	foo4 bar4;
	foo5 bar5;
	foo* pBar = &bar;
	foo* pBar2 = &bar2;
	foo* pBar3 = &bar3;
	foo* pBar4 = &bar4;
	foo* pBar5 = &bar5;
	uint_t uTest;

	uTest = string_compare(pBar->get_class_name(), "foo") != 0;
	ReportFailure("pBar->get_class_name()!='foo'", uTest);
	uResult |= uTest;
	uTest = string_compare(pBar2->get_class_name(), "foo2") != 0;
	ReportFailure("pBar2->get_class_name()!='foo2'", uTest);
	uResult |= uTest;
	uTest = string_compare(pBar3->get_class_name(), "foo3") != 0;
	ReportFailure("pBar3->get_class_name()!='foo3'", uTest);
	uResult |= uTest;
	uTest = string_compare(pBar4->get_class_name(), "foo4") != 0;
	ReportFailure("pBar4->get_class_name()!='foo4'", uTest);
	uResult |= uTest;
	uTest = string_compare(pBar5->get_class_name(), "foo5") != 0;
	ReportFailure("pBar5->get_class_name()!='foo5'", uTest);
	uResult |= uTest;

	uTest = BURGER_STATICRTTI_ISTYPE(foo, pBar) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo,pBar)!=1", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo2, pBar) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo2,pBar)!=0", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo3, pBar) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo3,pBar)!=0", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo4, pBar) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo4,pBar)!=0", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo5, pBar) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo5,pBar)!=0", uTest);
	uResult |= uTest;

	uTest = BURGER_STATICRTTI_ISTYPE(foo, pBar2) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo,pBar2)!=1", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo2, pBar2) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo2,pBar2)!=1", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo3, pBar2) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo3,pBar2)!=0", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo4, pBar2) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo4,pBar2)!=0", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo5, pBar2) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo5,pBar2)!=0", uTest);
	uResult |= uTest;

	uTest = BURGER_STATICRTTI_ISTYPE(foo, pBar3) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo,pBar3)!=1", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo2, pBar3) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo2,pBar3)!=1", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo3, pBar3) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo3,pBar3)!=1", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo4, pBar3) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo4,pBar3)!=0", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo5, pBar3) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo5,pBar3)!=0", uTest);
	uResult |= uTest;

	uTest = BURGER_STATICRTTI_ISTYPE(foo, pBar4) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo,pBar4)!=1", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo2, pBar4) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo2,pBar4)!=1", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo3, pBar4) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo3,pBar4)!=0", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo4, pBar4) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo4,pBar4)!=1", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo5, pBar4) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo5,pBar4)!=0", uTest);
	uResult |= uTest;

	uTest = BURGER_STATICRTTI_ISTYPE(foo, pBar5) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo,pBar5)!=1", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo2, pBar5) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo2,pBar5)!=0", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo3, pBar5) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo3,pBar5)!=0", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo4, pBar5) != 0;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo4,pBar5)!=0", uTest);
	uResult |= uTest;
	uTest = BURGER_STATICRTTI_ISTYPE(foo5, pBar5) != 1;
	ReportFailure("BURGER_STATICRTTI_ISTYPE(foo5,pBar5)!=1", uTest);
	uResult |= uTest;

	// Will properly upcast
	foo5* pTemp = BURGER_RTTICAST(foo5, pBar5);
	uTest = pTemp == NULL;
	ReportFailure("BURGER_RTTICAST(foo5,pBar5)==NULL", uTest);
	uResult |= uTest;

	const foo5* pCTemp = BURGER_RTTICONSTCAST(foo5, pBar5);
	uTest = pCTemp == NULL;
	ReportFailure("BURGER_RTTICONSTCAST(foo5,pBar5)==NULL", uTest);
	uResult |= uTest;

	// Will return NULL since these classes are not a foo5
	pTemp = BURGER_RTTICAST(foo5, pBar4);
	uTest = pTemp != NULL;
	ReportFailure("BURGER_RTTICONSTCAST(foo5,pBar4)==NULL", uTest);
	uResult |= uTest;

	pCTemp = BURGER_RTTICONSTCAST(foo5, pBar3);
	uTest = pCTemp != NULL;
	ReportFailure("BURGER_RTTICONSTCAST(foo5,pBar3)==NULL", uTest);
	uResult |= uTest;
	return uResult;
}

//
// Perform all the tests for the Burgerlib Static RTTI
//

int BURGER_API TestBrstaticrtti(uint_t uVerbose) BURGER_NOEXCEPT
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running Static RTTI tests");
	}

	uint_t uTotal = TestRTTI();

	if (!uTotal && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all Static RTTI tests!");
	}
	return static_cast<int>(uTotal);
}
