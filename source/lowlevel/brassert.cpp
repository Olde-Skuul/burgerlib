/***************************************

	Assert redirection class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brassert.h"
#include "brglobals.h"

#include <stdio.h>
#include <stdlib.h>

#if defined(BURGER_WINDOWS)
#include <windows.h>
#endif

#if defined(BURGER_XBOX360)
#include <xtl.h>
#endif

#if defined(BURGER_ANDROID) && defined(BURGER_ARM)
#include <unistd.h>
#endif

#if defined(BURGER_MSVC)
#pragma warning(disable:4702)		// Disable unreachable code
#endif

#if defined(BURGER_IOS)
#include <signal.h>
#endif

/*! ************************************

	\def BURGER_ASSERT(conditional)
	
	\brief Invoke the debugger when a condition is met only when \ref _DEBUG is defined.
	
	\param conditional A boolean that evaluates to \ref FALSE to call the assert function Burger::Assert()
	\note On Release builds with \ref NDEBUG defined, this macro will perform no action and generate no code
	so the conditional will never be tested
	
	\sa BURGER_ASSERTTEST
	
***************************************/

/*! ************************************

	\def BURGER_ASSERTTEST(conditional)
	
	\brief Always test a condition and if it's false, invoke the debugger.
	
	\param conditional A boolean that evaluates to \ref FALSE to call the assert function Burger::Assert()
	
	\note This macro will always invoke the conditional and will return the boolean of \ref TRUE or \ref FALSE
	in Release builds. If it's executing a Debug build, the Burger::Assert() function is called if
	the conditional evaluates to \ref FALSE
	
	\return The boolean evaluation of the conditional.
	
***************************************/


/***************************************

	Default assert function

***************************************/

static int BURGER_API DefaultAssert(void * /* pThis */,const char *pCondition,const char *pFilename,Word uLineNumber)
{
	printf("Assertion from \"%s\" in file %s at line %u.\n",pCondition,pFilename,uLineNumber);
	Burger::Halt();
	exit(10);
	return FALSE;
}

/*! ************************************

	\typedef Burger::ProcAssert
	
	\brief Redirect an assert to the application's assert function
	
	When an assert is fired from the use of the macro \ref BURGER_ASSERT
	this function is called which redirects to the currently logged assert
	function. 
	
	\param pThis Pointer to the application supplied data pointer
	\param pCondition String of the test condition that fired off the assert
	\param pFilename The value of the "__FILE__" macro at the location of the \ref BURGER_ASSERT macro
	\param uLineNumber The value of the "__LINE__" macro at the location of the \ref BURGER_ASSERT macro
	
	\return \ref FALSE. Only return a non-false value if a very special
	case requires it because it may cause unexpected behavior from
	the use of the \ref BURGER_ASSERT macro
	
***************************************/

/// Private redirection function pointer and pThis

static Burger::ProcAssert Assert_pProcAssert = DefaultAssert;
static void *Assert_pThis;

/*! ************************************

	\brief Redirect an assert to the application's assert function
	
	When an assert is fired from the use of the macro \ref BURGER_ASSERT
	this function is called which redirects to the currently logged assert
	function. It's global by design.
	
	The default behavior is to call exit(10) to quit to the operating
	system with a printf to the console a string denoting the application
	failure condition.
	
	\param pCondition String of the test condition that fired off the assert
	\param pFilename The value of the "__FILE__" macro at the location of the \ref BURGER_ASSERT macro
	\param uLineNumber The value of the "__LINE__" macro at the location of the \ref BURGER_ASSERT macro
	
	\note This function normally does not return and calls exit() or some other
	shutdown function. If the function returns, usually via manual user input, it will return a conditional override
	which is usually \ref FALSE to match the condition that triggered the assert.
	
	\return Defaults to \ref FALSE. 
	
***************************************/

int BURGER_API Burger::Assert(const char *pCondition,const char *pFilename,Word uLineNumber)
{
	return Assert_pProcAssert(Assert_pThis,pCondition,pFilename,uLineNumber);
}

/*! ************************************

	\brief Overrides the default Assert function
	
	The normal behavior for Burger::Assert() is to call printf() with
	the failure condition and then call exit(10). This behavior can
	be overridden by passing a new function pointer via this
	call. The default behavior can be restored by
	passing \ref NULL for the pAssert parameter.
	
	\param pAssert Function to call when an assert is triggered
	\param pThis void * of data to be passed to the assert function if
	triggered
	
***************************************/

void BURGER_API Burger::AssertRedirect(Burger::ProcAssert pAssert,void *pThis)
{
	// Passed NULL?
	if (!pAssert) {
		pAssert = DefaultAssert;
	}
	// Set my new state	
	Assert_pProcAssert = pAssert;
	Assert_pThis = pThis;
}

/*! ************************************

	\brief Enter the debugger if present

	\note This call should never be issued in a
	Release build. It is intended to invoke a debugger.

***************************************/

#if defined(BURGER_WINDOWS)

// Windows version will test if a debugger is present and will
// trap if a debugger is found. Will do nothing if no
// debugger is attached

static Word8 g_HaltTested;
static int (WINAPI *g_pDebugTest)(void);

void BURGER_API Burger::Halt(void)
{
	// Did I get the reference 
	if (!g_HaltTested) {
		// Load in the kernel reference
		HINSTANCE hKernelRef = Globals::LoadLibraryA("kernel32.dll");
		if (hKernelRef) {
			// Get the routine
			g_pDebugTest = (int (WINAPI *)(void))GetProcAddress(hKernelRef,"IsDebuggerPresent");
		}
		g_HaltTested = TRUE;		// I am cool...
	}
	if (g_pDebugTest) {			// Is the pointer valid?
		if (g_pDebugTest()) {	// Is the debugger present?
#if defined(BURGER_X86)
			_asm int 3			// Trap!
#else
			__debugbreak();		// ARM and AMD64 version
#endif
		}
	}
}

#else


void BURGER_API Burger::Halt(void)
{
#if defined(BURGER_WATCOM) && defined(BURGER_MSDOS)
	_asm int 3
#elif defined(BURGER_XBOX360) || defined(BURGER_PS4)
// Xbox 360 / PS4 always has a debugger, either from the host or the kernel
	__debugbreak();
#elif defined(BURGER_PS3)
	__builtin_snpause();
#elif defined(BURGER_IOS)
	raise(SIGTRAP);
#elif defined(BURGER_ANDROID) && defined(BURGER_ARM)
	kill(getpid(),SIGINT);
#elif defined(BURGER_ANDROID) && (defined(BURGER_X86) || defined(BURGER_AMD64))
	__asm__( "int $3\n" : : );
#else
	// Generic version does nothing
#endif	
}

#endif
