/***************************************

    60Hz timer

    MacOS Carbon specific code

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_MAC)
#include "brcodelibrary.h"
#include <DateTimeUtils.h>
#include <Gestalt.h>
#include <LowMem.h>
#include <Timer.h>

#if TARGET_API_MAC_CARBON
#define LMGetTicks() TickCount()
#else
#define LMGetTicks() ((volatile uint32_t*)0x16A)[0]
#endif

/***************************************

	Get the 60hz timer

***************************************/

uint32_t Burger::Tick::Read(void) BURGER_NOEXCEPT
{
	return LMGetTicks(); /* Call the mac native function */
}

/***************************************

	Accurate timers for Burgerlib
	Original code by Matt Slot <fprefect@ambrosiasw.com>
	Optimized and included into Burgerlib by Rebecca Ann Heineman
	<becky@burgerbecky.com>

***************************************/

/***************************************

	Here is the "C" reference code. This works fine

***************************************/

#if 0
uint32_t BURGER_API ReadTickMicroseconds(void)
{
	UnsignedWide wide;
	Microseconds(&wide);	/* Get the value from MacOS */
	return wide.lo;			/* Return just the low 32 bits */
}

uint32_t BURGER_API ReadTickMilliseconds(void)
{
	unsigned long long wide;
	Microseconds((UnsignedWide *)&wide);	/* Get the time in microseconds */
	return (uint32_t)((wide / 1000ULL));
}
#endif

/***************************************

	For 68K macs, I just use Microseconds, it's accurate and fast
	I rewrote the routines in 68K asm for all the speed I can muster

***************************************/

#if !defined(BURGER_POWERPC)

/***************************************

	Here is the above code but written in 68K assembly.
	Of course this means that I can only use Metrowerks 68K compilers
	to compile this. If anyone cares in the future (It is 1999 after all)
	then I'll support the alternate compilers.

***************************************/

extern void __rt_divu64(void); // A Metrowerks internal function

// clang-format off
asm uint32_t BURGER_API Burger::Tick::ReadMicroseconds(void)
{
	0xA193	// _Microseconds
	rts		// Get out NOW!
}

asm uint32_t BURGER_API Burger::Tick::ReadMilliseconds(void)
{
	0xA193				// _Microseconds
	subq.w #8,a7		// Space for result of divide
	pea 1000			// Div by 1000
	clr.l	-(a7)		// 64 bit
	move.l	d0,-(a7)	// Save the returned value
	move.l	a0,-(a7)
	pea		16(a7)		// Address for the result
	jsr		__rt_divu64 // 64 bit divide
	move.l	4(a0),d0	// Get the low 32 bits of the result
	lea		28(a7),a7	// Fix the stack
	rts					// Exit
}
// clang-format on

#else

/***************************************

	On PowerPC machines, we try several methods:
		* DriverServicesLib is available on all PCI PowerMacs, and perhaps
			some NuBus PowerMacs. If it is, we use UpTime() : Overhead = 2.1 usec.
		* The PowerPC 601 has a built-in "real time clock" RTC, and we fall
			back to that, accessing it directly from asm. Overhead = 1.3 usec.
		* Later PowerPCs have an accurate "time base register" TBR, and we
			fall back to that, access it from PowerPC asm. Overhead = 1.3 usec.
		* We can also try Microseconds() which is emulated : Overhead = 36 usec.

	On PowerPC machines, we avoid the following:
		* OpenTransport is available on all PCI and some NuBus PowerMacs, but it
			uses UpTime() if available and falls back to Microseconds()
			otherwise.
		* InputSprocket is available on many PowerMacs, but again it uses
			UpTime() if available and falls back to Microseconds() otherwise.

	Another PowerPC note: certain configurations, especially 3rd party upgrade
	cards, may return inaccurate timings for the CPU or memory bus -- causing
	skew in various system routines (up to 20% drift!). The VIA chip is very
	accurate, and it's the basis for the Time Manager and Microseconds().
	Unfortunately, it's also very slow because the MacOS has to (a) switch to
	68K and (b) poll for a VIA event.

	We compensate for the drift by calibrating a floating point scale factor
	between our fast method and the accurate timer at startup, then convert
	each sample quickly on the fly. I'd rather not have the initialization
	overhead -- but it's simply necessary for accurate timing. You can drop
	it down to 30 ticks if you prefer, but that's as low as I'd recommend.

***************************************/

typedef AbsoluteTime (*UpTimeProcPtr)(void);

#define WideToDouble(w) ((double)(w).hi * (65536.0 * 65536.0) + (double)(w).lo)
#define RTCToNano(w) ((double)(w).hi * PowerPCBillion + (double)(w).lo)
#define WideTo64bit(w) (*(unsigned long long*)&(w))

#ifdef __cplusplus
extern "C" {
#endif
extern void PollRTC601(uint64_t* Output);
extern void PollTBR603(uint64_t* Output);
extern void BurgerInitTimers(void);
#ifdef __cplusplus
}
#endif

uint_t PowerPCTimeMethod;			 /* How shall I read the timer? 0-3 */
double PowerPCScale = 1000000.0; /* Standard scale */
double PowerPCScale2 = 1000.0;   /* Microsecond accuracy */
UpTimeProcPtr PowerPCUpTime;	 /* UpTime pointer */
Fixed32 PowerPCFScale, PowerPCFScale2;
double PowerPCBillion = 1000000000.0;
double PowerPCThousand = 1000.0;
double PowerPCFix = 65536.0 * 65536.0 * 65536.0 * 16.0;

/***************************************

	Return a pointer to a DLL or shared library
	I do not release the library. This is a quick and dirty
	routine for getting things like DirectDraw and OpenTransport

***************************************/

static void* BURGER_API LibRefGetFunctionInLib(
	const char* pFilename, const char* pFunctionName)
{
	Burger::CodeLibrary MyLib;
	void* pFunction = NULL;

	if (MyLib.Init(pFilename)) { /* Load the library */
		pFunction =
			MyLib.GetFunction(pFunctionName); /* Load the function if found */
		if (!pFunction) {
			/* Return the proc pointer (NOTE: I don't release the reference!) */
			MyLib.Shutdown(); /* At least attempt to keep myself clean */
		}
	}
	return pFunction;
}

/***************************************

	Determine what timing method to use.
	UpTime, RTC, TBR or Microseconds()

***************************************/

void BurgerInitTimers(void)
{
	uint_t Method;
	long result;

	Method = 4; /* Assume Microseconds() */
	if (!Gestalt(gestaltNativeCPUtype, &result)) {
		if (result == gestaltCPU601) {
			Method = 3; /* Use 601 method */
		} else if (result > gestaltCPU601) {
			Method = 2; /* Use 603+ method */
		}
	}

	/* See if UpTime is present in DriverServicesLib */

	if (Method == 4) {
		PowerPCUpTime = (UpTimeProcPtr)LibRefGetFunctionInLib(
			"DriverServicesLib", "UpTime");

		/* If no DriverServicesLib, use Gestalt() to get the processor type.
			Only NuBus PowerMacs with old System Software won't have DSL, so
			we know it should either be a 601 or 603. */

		if (PowerPCUpTime) {
			Method = 1;
		}
	}
	PowerPCTimeMethod = Method; /* Save the method */

	/* Now calculate a scale factor to keep us accurate. */

	if (Method != 4) {
		uint32_t Mark, Mark2;
		unsigned long long usec1, usec2;
		UnsignedWide wide;

		/* Wait for the beginning of the very next tick */

		Mark2 = LMGetTicks(); /* Get the anchor */
		do {
			Mark = LMGetTicks(); /* Check ... */
		} while (Mark2 == Mark); /* Time? */

		/* Poll the selected timer and prepare it (since we have time) */

		if (Method == 1) {
			wide = PowerPCUpTime();
			usec1 = WideTo64bit(wide);
		} else if (Method == 2) {
			PollTBR603((uint64_t*)&wide);
			usec1 = WideTo64bit(wide);
		} else {
			PollRTC601((uint64_t*)&wide);
			usec1 = static_cast<unsigned long long>(RTCToNano(wide));
		}

		/* Wait for the exact 60th second to roll over */

		do {
			Mark2 = LMGetTicks();
		} while ((Mark2 - Mark) < 60);

		/* Poll the selected timer again and prepare it */

		if (Method == 1) {
			wide = PowerPCUpTime();
			usec2 = WideTo64bit(wide);
		} else if (Method == 2) {
			PollTBR603((uint64_t*)&wide);
			usec2 = WideTo64bit(wide);
		} else {
			PollRTC601((uint64_t*)&wide);
			usec2 = static_cast<unsigned long long>(RTCToNano(wide));
		}

		/* Calculate a scale value that will give microseconds per second.
			Remember, there are actually 60.15 ticks in a second, not 60. */

		PowerPCScale = 60000000.0 / ((usec2 - usec1) * 60.15);
		PowerPCScale2 = PowerPCScale / 1000.0;
		PowerPCFScale = static_cast<long>(PowerPCScale * 65536.0 * 65536.0);
		PowerPCFScale2 = static_cast<long>(PowerPCScale2 * 65536.0 * 65536.0);
	}
}

/***************************************

	Read the tick value in microsecond accuracy

***************************************/

#if defined(__MRC__) /* assembly version of this depends on CW libraries */

#pragma options opt = off

uint32_t Burger::Tick::ReadMicroseconds(void)
{
	UnsignedWide wide;
	uint_t Temp;
	double Foo;

	/* Initialize globals the first time we are called */

	Temp = PowerPCTimeMethod;
	if (!Temp) { /* Initialized? */
		BurgerInitTimers();
		Temp = PowerPCTimeMethod;
	}
	if (Temp == 1) {
		/* Use DriverServices if it's available -- it's fast and compatible */
		wide = PowerPCUpTime();
		Foo = (double)((unsigned long long*)&wide)[0];
		return (unsigned long long)(Foo * PowerPCScale);
	}
	if (Temp == 2) {
		/* On a recent PowerPC, we poll the TBR directly */
		PollTBR603((uint64_t*)&wide);
		Foo = (double)((unsigned long long*)&wide)[0];
		return (unsigned long long)(Foo * PowerPCScale);
	}
	if (Temp == 3) {
		/* On a 601, we can poll the RTC instead */
		PollRTC601((uint64_t*)&wide);
		return (unsigned long long)(RTCToNano(wide) * PowerPCScale);
	}
	/* If all else fails, suffer the mixed mode overhead */
	Microseconds(&wide);
	return wide.lo;
}

/***************************************

	Read the tick value in millisecond accuracy

***************************************/

uint32_t Burger::Tick::ReadMilliseconds(void)
{
	uint_t Temp;
	double Foo;
	UnsignedWide wide;

	/* Initialize globals the first time we are called */

	Temp = PowerPCTimeMethod;
	if (!Temp) { /* Initialized? */
		BurgerInitTimers();
		Temp = PowerPCTimeMethod;
	}

	if (Temp == 1) {
		/* Use DriverServices if it's available -- it's fast and compatible */
		wide = PowerPCUpTime();
		Foo = (double)((unsigned long long*)&wide)[0];
		return (unsigned long long)(Foo * PowerPCScale2);
	}
	if (Temp == 2) {
		/* On a recent PowerPC, we poll the TBR directly */
		PollTBR603((uint64_t*)&wide);
		Foo = (double)((unsigned long long*)&wide)[0];
		return (unsigned long long)(Foo * PowerPCScale2);
	}
	if (Temp == 3) {
		/* On a 601, we can poll the RTC instead */
		PollRTC601((uint64_t*)&wide);
		return (unsigned long long)(RTCToNano(wide) * PowerPCScale2);
	}
	/* If all else fails, suffer the mixed mode overhead */
	Microseconds(&wide);
	return (unsigned long long)(((double)WideTo64bit(wide)) * (1.0 / 1000.0));
}

#pragma options opt = speed

#endif
#endif
#endif
