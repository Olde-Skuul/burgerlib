/***************************************

	Assert redirection class

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brassert.h"
#include "brdebug.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(BURGER_WINDOWS)
#include "win_windows.h"
#endif

#if defined(BURGER_XBOX360)
#include <xtl.h>
#endif

#if defined(BURGER_SHIELD)
#include <unistd.h>
#endif

#if defined(BURGER_DARWIN) || defined(BURGER_UNIX)
#include <signal.h>
#include <unistd.h>
#endif

#if defined(BURGER_VITA)
#include <intrinsics.h>
#endif

#if defined(BURGER_WIIU)
#include <cafe/os.h>
#endif

#if defined(BURGER_MAC)
#include <Debugging.h>
#include <MacErrors.h>
#include <Processes.h>
#endif

/*! ************************************

	\def BURGER_ASSERT(conditional)

	\brief Invoke the debugger when a condition is met only when \ref _DEBUG is
		defined.

	\note On Release builds with \ref NDEBUG defined, this macro will perform no
		action and generate no code so the conditional will never be tested

	\param conditional A boolean that evaluates to \ref FALSE to call the assert
		function \ref Burger::do_assert(const char*, const char*, uint32_t)

	\sa BURGER_ASSERTTEST, or Burger::do_assert(
		const char*, const char*, uint32_t)

***************************************/

/*! ************************************

	\def BURGER_ASSERTTEST(conditional)

	\brief Always test a condition and if it's false, invoke the debugger.

	\param conditional A boolean that evaluates to \ref FALSE to call the assert
		function \ref Burger::do_assert(const char*, const char*, uint32_t)

	\note This macro will always invoke the conditional and will return the
		boolean of \ref TRUE or \ref FALSE in Release builds. If it's executing
		a Debug build, the \ref Burger::do_assert(
		const char*, const char*, uint32_t) function is called if the
		conditional evaluates to \ref FALSE

	\return The boolean evaluation of the conditional.

	\sa BURGER_ASSERT or Burger::do_assert(const char*, const char*, uint32_t)

***************************************/

/*! ************************************

	\struct Burger::assert_t
	\brief Container structure for assert support

	This global assert structure is used for supporting the ability to redirect
	an \ref do_assert(const char *, const char*, uint32_t) call.

	\sa do_assert(const char*, const char*, uint32_t), or BURGER_ASSERT

***************************************/

/*! ************************************

	\typedef Burger::assert_t::callback_t

	\brief Redirect an assert to the application's assert function

	When an assert is fired from the use of the macro \ref BURGER_ASSERT
	this function is called which redirects to the currently logged assert
	function.

	\param pThis Pointer to the application supplied data pointer
	\param pCondition String of the test condition that fired off the assert
	\param pFilename The value of the `__FILE__` macro at the location of the
		\ref BURGER_ASSERT macro
	\param uLineNumber The value of the `__LINE__` macro at the location of the
		\ref BURGER_ASSERT macro

	\return \ref FALSE. Only return a non-false value if a very special
		case requires it because it may cause unexpected behavior from
		the use of the \ref BURGER_ASSERT macro

***************************************/

/*! ************************************

	\brief Singleton instance for assert_t

	\sa assert_t or assert_t::default_assert

***************************************/

Burger::assert_t Burger::assert_t::g_Instance = {
	default_assert, // default Function pointer
	nullptr         // default "this" pointer
};

/*! ************************************

	\brief Overrides the default do_assert function

	The normal behavior for \ref do_assert(const char*, const char*, uint32_t)
	is to call printf() with the failure condition and then call \ref
	Debug::Fatal(). This behavior can be overridden by passing a new function
	pointer via this call.

	The default behavior can be restored by passing \ref nullptr for the
	pCallback parameter.

	\param pCallback Function to call when an assert is triggered, \ref nullptr
		restores defaults
	\param pThis void* of data to be passed to the assert function if triggered

***************************************/

void BURGER_API Burger::assert_t::set_callback(
	callback_t pCallback, void* pThis) BURGER_NOEXCEPT
{
	// nullptr?
	if (!pCallback) {
		pCallback = default_assert;
	}
	m_pCallback = pCallback;
	m_pThis = pThis;
}

/*! ************************************

	\brief Default assert function

	Prints the message
	"Assertion from \"%%s\\" in file %s at line
	%u.\n", pCondition, pFilename, uLineNumber using Debug::Message(), and then
	calls \ref invoke_debugger(). If execution continues, it will call
	Debug::Fatal() with the same message and exit the application.

	\note This function normally does not return and calls Debug::Fatal().
		If the function returns, usually via manual user input in a debugger, it
		will return a conditional override which is 0 to match the condition
		that triggered the assert.

	\param pThis Ignored
	\param pCondition Message to print do denote the cause of the assertion
	\param pFilename Pointer to a "C" string of the source file name that
		triggered the assert
	\param uLineNumber Line number in the source file that triggered the assert

	\returns 0 (However, this function should never return)

	\sa Debug::Message(), Debug::Fatal(), and do_assert(
		const char*, const char*, uint32_t)

***************************************/

int BURGER_API Burger::assert_t::default_assert(void* /* pThis */,
	const char* pCondition, const char* pFilename,
	uint32_t uLineNumber) BURGER_NOEXCEPT
{
	Debug::Message("Assertion from \"%s\" in file %s at line %u.\n", pCondition,
		pFilename, uLineNumber);

	// If the debugger is attached, invoke it
	invoke_debugger();

	// Exit to operating system
	Debug::Fatal("Assertion from \"%s\" in file %s at line %u.\n", pCondition,
		pFilename, uLineNumber);

	// Always return 0, however Fatal() should abort execution
	return 0;
}

/*! ************************************

	\brief Redirect an assert to the application's assert function

	When an assert is fired from the use of the macro \ref BURGER_ASSERT
	this function is called which redirects to the currently logged assert
	function. It's global by design.

	Prints the message "Assertion from \"%s\" in file %s at line %u.\n",
	pCondition, pFilename, uLineNumber using Debug::Message(), and then
	calls \ref invoke_debugger(). If execution continues, it will call
	Debug::Fatal() with the same message and exit the application.

	\note This function normally does not return and calls Debug::Fatal() or
		some other shutdown function. If the function returns, usually via
		manual user input, it will return a conditional override which is
		usually 0 to match the condition that triggered the assert.

	\param pCondition "C" string of the test condition that fired off the assert
	\param pFilename The value of the `__FILE__` macro at the location of the
		\ref BURGER_ASSERT macro
	\param uLineNumber The value of the `__LINE__` macro at the location of the
		\ref BURGER_ASSERT macro

	\return Always 0.

	\sa assert_t::default_assert(), BURGER_ASSERT, or BURGER_ASSERTTEST

***************************************/

int BURGER_API Burger::do_assert(const char* pCondition, const char* pFilename,
	uint32_t uLineNumber) BURGER_NOEXCEPT
{
	const assert_t* pInstance = &assert_t::g_Instance;
	return pInstance->m_pCallback(
		pInstance->m_pThis, pCondition, pFilename, uLineNumber);
}

/*! ************************************

	\brief Enter the debugger if present

	On some platforms, this will only halt if a debugger is attached, if not,
	this function will return immediately

	\note This call should never be issued in a Release build. It is intended to
		invoke a debugger.

***************************************/

void BURGER_API Burger::invoke_debugger(void) BURGER_NOEXCEPT
{
	// Is the debugger present?

	if (is_debugger_present()) {

#if defined(BURGER_X86) && defined(BURGER_WINDOWS)
#if defined(BURGER_MINGW)
		__asm__("int $3\n" : :);
#else
		// Trap! Older compilers for Windows don't have __debugbreak()
		_asm int 3
#endif

#elif defined(BURGER_WATCOM) && defined(BURGER_MSDOS)
		_asm int 3

#elif defined(BURGER_XBOX) || defined(BURGER_XBOX360) || \
	defined(BURGER_XBOXONE) || defined(BURGER_WINDOWS)
		// Xbox 360 always has a debugger, either from the host or the kernel
		__debugbreak();

#elif defined(BURGER_PS3)
		__builtin_snpause();

#elif defined(BURGER_PS4) || defined(BURGER_PS5)
		__asm volatile("int $0x41");

#elif defined(BURGER_VITA)
		__builtin_breakpoint(0);

#elif defined(BURGER_WIIU)
		OSFatal("Application failure");

#elif defined(BURGER_DARWIN)
		raise(SIGTRAP);

#elif defined(BURGER_ANDROID) && defined(BURGER_INTEL)
		__asm__("int $3\n" : :);

#elif defined(BURGER_UNIX)
		kill(getpid(), SIGINT);

#elif defined(BURGER_CLANG)
		__builtin_debugtrap();

#elif defined(BURGER_GNUC)
		__builtin_trap();

#elif defined(BURGER_MAC)
		Debugger();

#else

// Not implemented on this platform.
#error invoke_debugger() not implemented, insert the appropriate code here
#endif
	}
}
