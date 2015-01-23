/***************************************

	Unit tests for the Date/Time manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#include "testbrdisplay.h"
#include "common.h"
#include "brdisplay.h"
#include "brarray.h"
#include "brmemoryansi.h"

/***************************************

	Test the clear function

***************************************/

static Word TestGetVideoModes(void)
{
	Burger::ClassArray<Burger::Display::VideoCardDescription> Modes;
	Word uFailure = Burger::Display::GetVideoModes(&Modes);
	
	ReportFailure("Burger::Display::GetVideoModes() didn't return zero.",uFailure);
	WordPtr uCount = Modes.size();
	if (uCount) {
		const Burger::Display::VideoCardDescription *pWork1 = Modes.GetPtr();
		do {
			Message("Device = %s",pWork1->m_DeviceName.GetPtr());
			Message("Monitor = %s",pWork1->m_MonitorName.GetPtr());
			Message("Device number = %u, Hardware = %u",pWork1->m_uDevNumber,pWork1->m_bHardwareAccelerated);
			WordPtr uCount2 = pWork1->m_Array.size();
			if (uCount2) {
				const Burger::Display::VideoMode_t *pMode = pWork1->m_Array.GetPtr();
				do {
					Message("Width %u, Height %u, Depth %u, Hertz %u, Flags 0x%08X",pMode->m_uWidth,pMode->m_uHeight,
						pMode->m_uDepth,pMode->m_uHertz,pMode->m_uFlags);
					++pMode;
				} while (--uCount2);
			}
			++pWork1;
		} while (--uCount);
	}
	return uFailure;
}

//
// Perform all the tests for the Burgerlib Endian Manager
//

int BURGER_API TestBrDisplay(void)
{	
	Burger::MemoryManagerGlobalANSI Memory;

	Word uResult;	// Assume no failures
	Message("Running Display tests");
	// Test compiler switches
	uResult = TestGetVideoModes();
	return static_cast<int>(uResult);
}