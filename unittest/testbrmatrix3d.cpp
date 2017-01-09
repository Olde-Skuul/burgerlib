/***************************************

	Unit tests for the Matrix3D_t library

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "testbrmatrix3d.h"
#include "common.h"
#include "brmatrix3d.h"
#include "brstringfunctions.h"

using namespace Burger;

//
// By using primes, it increases the chance of catching
// a term error in the multiplication
//

static Word TestMultiply(void)
{
	Word uResult = 0;
	Matrix3D_t Foo1;
	Matrix3D_t Foo2;
	Matrix3D_t Bar1;
	Matrix3D_t Bar2;
	Foo1.SetScale(3,5,7);
	Foo2.SetScale(11,13,17);
	Bar1.Multiply(&Foo1,11,13,17);
	Bar2.Multiply(&Foo1,&Foo2);
	Word uTest = MemoryCompare(&Bar1,&Bar2,sizeof(Bar1))!=0;
	ReportFailure("Fail " BURGER_MACRO_TO_STRING(__LINE__),uTest);
	uResult |= uTest;

	Bar1.TransposeMultiply(&Foo1,11,13,17);
	Bar2.Multiply(&Foo2,&Foo1);
	uTest = MemoryCompare(&Bar1,&Bar2,sizeof(Bar1))!=0;
	ReportFailure("Fail " BURGER_MACRO_TO_STRING(__LINE__),uTest);
	uResult |= uTest;

	Foo1.x.y = 5;
	Foo1.x.z = 7;
	Foo1.y.x = 11;
	Foo1.y.y = 13;
	Foo1.y.z = 17;
	Foo1.z.x = 19;
	Foo1.z.y = 23;
	Foo1.z.z = 29;
	Bar1.Multiply(&Foo1,11,13,17);
	Bar2.Multiply(&Foo1,&Foo2);
	uTest = MemoryCompare(&Bar1,&Bar2,sizeof(Bar1))!=0;
	ReportFailure("Fail " BURGER_MACRO_TO_STRING(__LINE__),uTest);
	uResult |= uTest;

	Bar1.TransposeMultiply(&Foo1,11,13,17);
	Bar2.Multiply(&Foo2,&Foo1);
	uTest = MemoryCompare(&Bar1,&Bar2,sizeof(Bar1))!=0;
	ReportFailure("Fail " BURGER_MACRO_TO_STRING(__LINE__),uTest);
	uResult |= uTest;
	return uResult;
}

//
// Perform all the tests for the Burgerlib FP Math library
//

int BURGER_API TestBrmatrix3d(void)
{
#if !defined(BURGER_68K)

	Word uTotal;	// Assume no failures

	Message("Running Matrix3D tests");
	uTotal = TestMultiply();

	return static_cast<int>(uTotal);
#else
	return 0;
#endif
}
