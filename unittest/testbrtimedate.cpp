/***************************************

	Unit tests for the Date/Time manager

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "testbrtimedate.h"
#include "common.h"
#include "brtimedate.h"

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
// Perform all the tests for the Burgerlib Endian Manager
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
		Message("Verbose:  %s",Buffer);
		MyDate.DateToString(Buffer);
		Message("Normal:   %s",Buffer);
		MyDate.TimeToString(Buffer);
		Message("Military: %s",Buffer);
		MyDate.TimeToStringPM(Buffer);
		Message("AM/PM:   %s",Buffer);
	}
	return static_cast<int>(uResult);
}
