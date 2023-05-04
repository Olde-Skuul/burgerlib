/***************************************

	Shims for the dbus library

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "unix_shims.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)

// Needed for dlsym
#include <dlfcn.h>
#include <errno.h>

#if !defined(DOXYGEN)
// Some old versions of Linux doesn't define this
#ifndef RTLD_DEFAULT
#define RTLD_DEFAULT nullptr
#endif
#endif

/*! ************************************

	\brief Shim for pthread_setname_np()

	Check if pthread_setname_np() is available, and if so, dispatch the call to
	the actual function. If the function is not available, always return EPERM.

	\note On Darwin platforms, the target_thread parameter doesn't exist. This
		is Darwin's fault.

	\param target_thread pthread_t of the thread to name (Not available on
		Darwin)
	\param name Pointer to a "C" for the new thread name

	\return Error code, or zero, or EPERM if function not found

***************************************/

#if defined(BURGER_DARWIN) && !defined(DOXYGEN)
int Burger::Unix::pthread_setname_np(const char* name)
{
	// Function pointer
	static int (*s_pthread_setname_np)(const char*);

	// Flag if checked
	static uint_t s_bChecked;

	// Load in the function pointer if needed
	if (!s_bChecked) {
		*reinterpret_cast<void**>(&s_pthread_setname_np) =
			dlsym(RTLD_DEFAULT, "pthread_setname_np");
		s_bChecked = TRUE;
	}

	// If the pointer exists, call it
	if (s_pthread_setname_np) {
		return s_pthread_setname_np(name);
	}
	// Always return this error if the function doesn't exist
	return EPERM;
}
#else
int Burger::Unix::pthread_setname_np(uintptr_t target_thread, const char* name)
{
	// Function pointer
	static int (*s_pthread_setname_np)(uintptr_t target_thread, const char*);

	// Flag if checked
	static uint_t s_bChecked;

	// Load in the function pointer if needed
	if (!s_bChecked) {
		*reinterpret_cast<void**>(&s_pthread_setname_np) =
			dlsym(RTLD_DEFAULT, "pthread_setname_np");
		s_bChecked = TRUE;
	}

	// If the pointer exists, call it
	if (s_pthread_setname_np) {
		return s_pthread_setname_np(target_thread, name);
	}
	// Always return this error if the function doesn't exist
	return EPERM;
}

#endif

#endif
