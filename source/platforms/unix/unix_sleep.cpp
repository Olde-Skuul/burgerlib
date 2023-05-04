/***************************************

	Sleep helper functions for Unix

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "unix_sleep.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)
#include <errno.h>
#include <time.h>

/*! ************************************

	\brief Sleep for a specific duration with nanosleep()

	nanosleep() can be interrupted and the delay aborted. If this is not
	desired, then leave the parameter bAlertable set to \ref FALSE. If
	bAlertable is set to \ref TRUE, then interrupt aborts is allowed.

	\param pInput Pointer to an initialized timespec with the desired delay.
	\param bAlertable \ref TRUE if interruption is allowed

	\return \ref kErrorNone on no error, \ref kErrorCancelled if cancelled

***************************************/

Burger::eError BURGER_API Burger::Unix::nanosleep_delay(
	timespec* pInput, uint_t bAlertable) BURGER_NOEXCEPT
{
	// Assume cancelled
	eError uResult = kErrorCancelled;
	do {
		// Make sure there is no error marked
		errno = 0;

		// Time delay
		timespec Delay;
		Delay.tv_sec = pInput->tv_sec;
		Delay.tv_nsec = pInput->tv_nsec;

		// Sleep
		int iResult = nanosleep(&Delay, pInput);

		// Note: pInput has the remaining time

		// No interruption?
		if (!iResult) {
			uResult = kErrorNone;
			break;
		}

	} while (!bAlertable && (errno == EINTR));

	return uResult;
}

#endif
