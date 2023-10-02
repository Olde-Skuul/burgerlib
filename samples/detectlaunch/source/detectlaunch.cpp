/***************************************

	Showcase of the use of DetectMultiLaunch

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "detectlaunch.h"

// Use burgerlib
using namespace Burger;

/***************************************

	Main entry point for a tool

	To run the test. Open two terminals. Run this application in both terminals
	at the same time. One should run for 10 seconds, and the other will
    immediately exit due to the other application running.

***************************************/

int BURGER_ANSIAPI main(int argc, const char** argv)
{
	// Init Burgerlib low level functions for a command line tool
	ConsoleApp App(argc, argv);

	// Create object to perform the test
	DetectMultiLaunch OneShot;

	// Create object, and if it already exists, exit
	if (OneShot.is_multi_launched("Unique string for this app")) {
		printf("This application is already running\n");
		return 0;
	}

	// Object didn't exist, so this is the only instance of this application
	// running.

	// Wait for 10 seconds so that another application can be run
	uint32_t uTimeMark = Tick::read_ms();
	uint32_t uSecond = 0;
	do {
		uint32_t uMark = Tick::read_ms();
		uint32_t uElapsed = (uMark - uTimeMark) / 1000U;
		if (uElapsed != uSecond) {
			printf("%u seconds elapsed\n", static_cast<uint_t>(uSecond));
			uSecond = uElapsed;
		}
	} while (uSecond < 10U);

	// Exit gracefully
	return 0;
}