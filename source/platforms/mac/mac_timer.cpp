/***************************************

	MacOS specific timer code

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "mac_timer.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)

/*! ************************************

	\namespace Burger::MacOS
	\brief Functions specific to MacOS

	For MacOS 7.5 to 9.2.2, these functions exists to support the operating
	system.

	While modern MacOS shares the same name with classic MacOS, they are not
	compatible. Use the MacOSX class of functions.

	\maconly

	\sa Burger::MacOSX, or Burger::iOS

***************************************/

/*! ************************************

	\fn Burger::MacOS::PollRTC601(void)
	\brief Poll the PowerPC 601 Real Time Clock register

	Poll the PowerPC 601 Real Time Clock register and debounce the value. Once
	debounced, return the 64 bit value.

	Done in assembly, because that's how you know it's done right

	\maconly

	\return 64 bit Real Time Clock register

***************************************/

/*! ************************************

	\fn Burger::MacOS::PollTBR603(void)
	\brief Poll the PowerPC 603 timer register

	Poll the PowerPC 603 Time Base Register and debounce the value. Once
	debounced, return the 64 bit value.

	\maconly

	\return 64 bit Time Base Register

***************************************/

#endif