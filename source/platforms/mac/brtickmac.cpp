/***************************************

	60Hz timer

	MacOS Carbon specific code

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_MAC)
#include "mac_timer.h"

#include <DateTimeUtils.h>
#include <DriverServices.h>
#include <Gestalt.h>
#include <LowMem.h>
#include <Math64.h>
#include <Timer.h>

#if !defined(DOXYGEN)
#if defined(BURGER_CFM)
#define LMGetTicks() TickCount()
#else
#define LMGetTicks() ((volatile uint32_t*)0x16A)[0]
#endif

/***************************************

	Accurate timers for Burgerlib
	Original code by Matt Slot <fprefect@ambrosiasw.com>

	Optimized and included into Burgerlib by Rebecca Ann Heineman
	<becky@burgerbecky.com>

	On PowerPC machines, we try several methods:
		* DriverServicesLib is available on all PCI PowerMacs, and perhaps
			some NuBus PowerMacs. If it is, we use UpTime() : Overhead = 2.1
			usec.
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

#if defined(BURGER_PPC)

// Which time to use?

enum eTimerType {
	// UpTime as is
	kTimerTypeUpTime,

	// UpTime using a scaled timebase
	kTimerTypeUpTimeScaled,

	// PPC 601 RTC
	kTimerType601RTC,

	// PPC 603 TBR
	kTimerType603TBR,

	// Microseconds
	kTimerTypeMicroseconds
};

/***************************************

	Return a pointer to a function in DriverServicesLib

	Note: This function seems to always fail on Rosetta

***************************************/

static void* BURGER_API LibRefGetFunctionInLib(StringPtr pFunctionName)
{
	// Possible error message
	Str255 ErrorString;

	// Function pointer found
	Ptr pFunction;

	// Connection ID
	CFragConnectionID uConnID;

	// See if the library can be loaded.
	if (GetSharedLibrary("\pDriverServicesLib", kCompiledCFragArch, kLoadCFrag,
			&uConnID, &pFunction, ErrorString)) {
		return nullptr;
	}

	// Yes this leaks a connection, but it will be released on program exit
	if (FindSymbol(uConnID, pFunctionName, &pFunction, nullptr)) {
		return nullptr;
	}
	return pFunction;
}

#endif
#endif

/***************************************

	\brief Initialize the low level timer manager

	Start up the low level timer

	\sa shutdown()

***************************************/

void BURGER_API Burger::Tick::init(void) BURGER_NOEXCEPT
{
	Tick* pThis = &Tick::g_Tick;
	if (!pThis->m_bInitialized) {

#if defined(BURGER_PPC)
		// Welcome to hell

		// Timing on 68000 machines is simple, use Microseconds(). Why? The
		// fastest 68000 mac is 40Mhz, so any timing faster than that is pretty
		// much pointless.

		// The PowerPC on the hand can hit gigahertz speed, so timing actually
		// matters. However, Apple, in their infinite wisdom, decided to not
		// implement any sort of high accuracy time until way late into the
		// PowerPC mac line with the inclusion of UpTime() and
		// AbsoluteToNanoseconds(). If both functions exist, then if it's native
		// PowerPC code, it's pretty accurate. Failing that, use direct PowerPC
		// 601 or PowerPC 603 real time clock instructions in order to get
		// timing values with the smallest cost to the calling function.

		// Note: Yes, there's functions in InputSprocket and OpenTransport, but
		// they aren't as fast as this method Microseconds is only used as a
		// last resort, because it's written in 68000 if UpTime does not exist.
		// This is slow. Your soul will feel the burn. So, toasty...

		// Which timer are we using?
		// Assume worst case
		uint_t uMethod = kTimerTypeMicroseconds;

		// Let's try the best method
		void* pUpTime = LibRefGetFunctionInLib("\pUpTime");
		void* pAbsoluteToNanoseconds =
			LibRefGetFunctionInLib("\pAbsoluteToNanoseconds");

		// Save the pointers for later
		pThis->m_pUpTime = pUpTime;

		// Is UpTime() a valid choice?
		if (pUpTime && pAbsoluteToNanoseconds) {

			// Looking good! Is the time manager native?
			long lTimeVersion;
			uMethod = kTimerTypeUpTimeScaled;

			// Check if it's native
			if (!Gestalt(gestaltTimeMgrVersion, &lTimeVersion)) {
				if (lTimeVersion > gestaltExtendedTimeMgr) {

					// Good, now get NanosecondsToAbsolute()
					void* pNanosecondsToAbsolute =
						LibRefGetFunctionInLib("\pNanosecondsToAbsolute");
					if (pNanosecondsToAbsolute) {

						// It's native
						uMethod = kTimerTypeUpTime;

						// Use this dirty trick to make sure the timebase is
						// indeed nanoseconds. If not, it's captured here
						Nanoseconds TempTime;
						TempTime.hi = 0;
						TempTime.lo = 1000000000U;

						// Convert nanoseconds to what it's really using. Most
						// of the time it returns the same value
						AbsoluteTime Scale =
							static_cast<AbsoluteTime (*)(Nanoseconds)>(
								pNanosecondsToAbsolute)(TempTime);

						// Save it
						pThis->m_uHighPrecisionFrequency =
							UnsignedWideToUInt64(Scale);
					}
				}
			}

			// Note: If uMethod is kTimerTypeUpTimeScaled, it will need the time
			// trick below, otherwise, it's a solid 1Ghz
		}

		// Method not found yet? Try 601/603
		if (uMethod == kTimerTypeMicroseconds) {

			// Gestalt result
			long lGestalCPU;

			// First, check if an original PPC 601 or 603
			if (!Gestalt(gestaltNativeCPUtype, &lGestalCPU)) {
				if (lGestalCPU == gestaltCPU601) {
					// Use 601 method
					uMethod = kTimerType601RTC;
				} else if (lGestalCPU > gestaltCPU601) {
					// Use 603+ method
					uMethod = kTimerType603TBR;
				}
			}
		}
		// Save the method being used
		pThis->m_uMethod = uMethod;

		// Microseconds is a known time
		if (uMethod == kTimerTypeMicroseconds) {
			pThis->m_uHighPrecisionFrequency = 1000000U;

		} else if (uMethod != kTimerTypeUpTime) {

			// Now calculate a scale factor to keep us accurate.
			uint32_t uTickMark;

			// Wait for the beginning of the very next tick
			// Get the anchor
			uint32_t uTempMark = LMGetTicks();
			do {
				// Check ...
				uTickMark = LMGetTicks();

				// Time?
			} while (uTempMark == uTickMark);

			// Poll the selected timer and prepare it (since we have time)
			uint64_t uStart = read_high_precision();

			// Wait for the exact 60th second to roll over
			do {
				uTempMark = LMGetTicks();
			} while (static_cast<uint32_t>(uTempMark - uTickMark) < 60U);

			// Poll the selected timer again and prepare it
			uint64_t uEnd = read_high_precision();

			// Calculate a scale value that will give microseconds per second.
			// Remember, there are actually 60.15 ticks in a second, not 60.
			pThis->m_uHighPrecisionFrequency =
				((uEnd - uStart) * 6000U) / 6015U;
		}
#endif

		// Get the speed of the most accurate timer
		pThis->m_uLast60HertzMark = 1;

		// Init the sub-timers
		pThis->m_1KHertz.init(1000U);

		// Not needs for 68K, since it used Microseconds()
#if !defined(BURGER_68K)
		pThis->m_1MHertz.init(1000000U);
#endif

		pThis->m_bInitialized = TRUE;
	}
}

/***************************************

	\brief Return the ticks per second at the system's highest precision

	This platform specific code will ask the operating system what is the
	highest precision timer tick rate and then will return that value.

	This value is cached and is available from get_high_precision_frequency()

	\sa get_high_precision_frequency(), or read_high_precision()

***************************************/

uint64_t BURGER_API Burger::Tick::get_high_precision_rate(void) BURGER_NOEXCEPT
{
	// 68K used Microseconds()
#if defined(BURGER_68K)
	return 1000000U;
#else
	// PowerPC is a buffet
	return g_Tick.m_uHighPrecisionFrequency;
#endif
}

/***************************************

	\brief Return the tick at the system's highest precision

	The value returns a tick that will increment at
	get_high_precision_frequency() ticks per second.

	\sa get_high_precision_frequency()

***************************************/

uint64_t BURGER_API Burger::Tick::read_high_precision(void) BURGER_NOEXCEPT
{
	// Tick value from MacOS
	UnsignedWide uTick;

	// Register version for result
	uint64_t uResult;

	// 68K mac has only one method
#if defined(BURGER_68K)
	Microseconds(&uTick);
	uResult = UnsignedWideToUInt64(uTick);
#else

	Tick* pThis = &g_Tick;

	// PowerPC Macs have many choices

	switch (pThis->m_uMethod) {

		// UpTime()
	case kTimerTypeUpTime:
	case kTimerTypeUpTimeScaled:
		uTick = static_cast<AbsoluteTime (*)(void)>(pThis->m_pUpTime)();
		uResult = UnsignedWideToUInt64(uTick);
		break;

		// PowerPC 601 Real Time Clock
	case kTimerType601RTC:
		uResult = MacOS::PollRTC601();
		break;

		// PowerPC 603 Time Base Register
	case kTimerType603TBR:
		uResult = MacOS::PollTBR603();
		break;

		// Microseconds for a "None of the above"
	default:
		Microseconds(&uTick);
		uResult = UnsignedWideToUInt64(uTick);
		break;
	}
#endif
	return uResult;
}

/***************************************

	\brief Retrieve the 60 hertz timer system time

	When init() is called, a 60 hertz timer is created and via a background
	interrupt or other means, will increment 60 times a second.

	The value can be zero for 1/60th of a second, so do not assume that a zero
	is an uninitialized state.

	\return 32 bit time value that increments 60 times a second
	\sa read_and_mark()

***************************************/

uint32_t Burger::Tick::read(void) BURGER_NOEXCEPT
{
	// Call the mac native function
	return LMGetTicks();
}

/***************************************

	\brief Retrieve the 1Mhz timer

	Upon application start up, a 1Mhz hertz timer is created and via
	a hardward timer, it will increment 1Mhz times a second.

	The value can be zero for 1/1,000,000th of a second, so do not assume that a
	zero is an uninitialized state.

	\note Due to hardware limitations, do NOT assume this timer is accurate to
		1/1,000,000th of a second. The granularity could be much courser,
		however, it will be incrementing at a rate to remain in sync to
		1,000,000 ticks a second.

	\return 32 bit time value that increments at 1Mhz
	\sa read() or read_ms()

***************************************/

#if defined(BURGER_68K)
uint32_t BURGER_API Burger::Tick::read_us(void) BURGER_NOEXCEPT
{
	// Get the value from MacOS
	UnsignedWide uTick;
	Microseconds(&uTick);

	// Return just the low 32 bits
	return uTick.lo;
}
#endif

#endif
