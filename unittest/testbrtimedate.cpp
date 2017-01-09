/***************************************

	Unit tests for the Date/Time manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "testbrtimedate.h"
#include "common.h"
#include "brtimedate.h"
#include "brstdouthelpers.h"
#include "brfloatingpoint.h"

/***************************************

	Test the clear function

***************************************/

static Word TestBurgerTimeDateClear(void)
{
	Burger::TimeDate_t Buffer[3];
	BlastBuffer(Buffer,sizeof(Buffer));
	Buffer[1].Clear();
	Word uFailure = VerifyBuffer(Buffer,sizeof(Buffer),&Buffer[1],sizeof(Buffer[1]));
	if (Buffer[1].m_uYear ||
		Buffer[1].m_bMonth ||
		Buffer[1].m_bDay ||
		Buffer[1].m_bDayOfWeek ||
		Buffer[1].m_bHour ||
		Buffer[1].m_bMinute ||
		Buffer[1].m_bSecond ||
		Buffer[1].m_usMilliseconds) {
		uFailure = TRUE;
	}
	ReportFailure("Burger::TimeDate_t::Clear() didn't set all entries to zero.",uFailure);
	return uFailure;
}

//
// Perform all the tests for the Burgerlib Time Manager
//

int BURGER_API TestDateTime(void)
{	
	Word uResult;	// Assume no failures

	Message("Running Date/Time tests");
	// Test compiler switches
	uResult = TestBurgerTimeDateClear();

	if (!uResult) {
		char Buffer[256];
		Burger::TimeDate_t MyDate;
		MyDate.GetTime();
		MyDate.DateToStringVerbose(Buffer);
		Message("Verbose: %s",Buffer);
		MyDate.DateToString(Buffer);
		Message("Normal: %s",Buffer);
		MyDate.TimeToString(Buffer);
		Message("Military: %s",Buffer);
		MyDate.TimeToStringPM(Buffer);
		Message("AM/PM: %s",Buffer);
	}
	return static_cast<int>(uResult);
}

//
// Perform all the tests for the Burgerlib Stdouthelpers Manager
//

int BURGER_API TestStdoutHelpers(Word uVerbose)
{	
	Word uResult = 0;	// Assume no failures
	if (uVerbose) {
		Message("Testing Stdout Helpers");
		Burger::PrintHexDigit(0x12345678);
		Message(" ");
		Burger::PrintHex(static_cast<Word8>(0x12));
		Message(" ");
		Burger::PrintHex(static_cast<Word16>(0x1234));
		Message(" ");
		Burger::PrintHex(static_cast<Word32>(0x12345678));
		Message(" ");
		Burger::PrintHex(static_cast<Word64>(0x123456789ABCDEFULL));
		Message(" ");
		Burger::PrintHex(Burger::g_fNan);
		Message(" ");
		Burger::PrintHex(Burger::g_dNan);
		Message(" ");
		Burger::PrintHex(1.0f);
		Message(" ");
		Burger::PrintHex(1.0);
		Message(" ");
	}
	return static_cast<int>(uResult);
}
