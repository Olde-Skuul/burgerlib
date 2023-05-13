/***************************************

	MS-DOS version (DOS4GW dos extender)

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_MSDOS)
#include "brdosextender.h"
#include <conio.h>
#include <dos.h>
#include <stdlib.h>
#include <time.h>

/***************************************

	Create an interrupt that will fire at 60 times a second. Every time it
	fires, increment a variable that is read by the higher level code.

***************************************/

#if defined(BURGER_DOS4G)

/***************************************

	My timer 0 ISR which handles the different timings and
	dispatches to whatever other routines are appropriate

***************************************/

static void __interrupt __far MyIrq8(void)
{
	// Get the instance pointer
	Burger::Tick* pThis = Burger::Tick::get_instance();

	// Perform the tick
	++pThis->m_u60HertzTick;

	// Time for a TRUE irq #8
	pThis->m_uDelta += pThis->m_uStepUnits;

	// Has 18.2 hertz been hit?
	if (pThis->m_uDelta >= 0x10000) {

		// Remove the overflow
		pThis->m_uDelta -= 0x10000;

		// Fire off the other interrupts
		_chain_intr(pThis->m_pPreviousINT8);
	}

	// Acknowledge the interrupt
	outp(0x20, 0x20);
}

/***************************************

	\brief Initialize the low level timer manager

	Start up the low level timer

	\sa shutdown()

***************************************/

void BURGER_API Burger::Tick::init(void) BURGER_NOEXCEPT
{
	// Init the variables
	Tick* pThis = &g_Tick;
	if (!pThis->m_bInitialized) {
		pThis->m_uHighPrecisionFrequency = CLOCKS_PER_SEC;
		pThis->m_uLast60HertzMark = 1;
		pThis->m_uDelta = 0;
		pThis->m_u60HertzTick = 1;
		pThis->m_1KHertz.init(1000U);
		pThis->m_1MHertz.init(1000000U);

		// Get old timer 0 ISR
		pThis->m_pPreviousINT8 = _dos_getvect(8);

		// Set to my timer 0 ISR
		_dos_setvect(8, MyIrq8);

		// Get the new timer delta for 60 hertz
		const uint32_t uStepUnits = (1192030UL / 60UL);

		// Save the speed value
		pThis->m_uStepUnits = uStepUnits;

		// Change timer 0
		outp(0x43, 0x36);
		outp(0x40, uStepUnits);
		outp(0x40, uStepUnits >> 8U);
		pThis->m_bInitialized = TRUE;
	}
}

/***************************************

	\brief Shut down the low level timer manager

	Shut down the low level timer

	\sa init()

***************************************/

void BURGER_API Burger::Tick::shutdown(void) BURGER_NOEXCEPT
{
	Tick* pThis = &g_Tick;
	if (pThis->m_bInitialized) {

		// Change timer 0 back to 18.2 hertz
		outp(0x43, 0x36);
		outp(0x40, 0);
		outp(0x40, 0);

		// Restore the speed value internally
		pThis->m_uStepUnits = 0x10000;

		// Restore vectors
		_dos_setvect(8, pThis->m_pPreviousINT8);
		pThis->m_pPreviousINT8 = nullptr;
		pThis->m_bInitialized = FALSE;
	}
}

#else

/***************************************

	X32 Dos extender

***************************************/

#ifdef __cplusplus
extern "C" {
#endif
extern void interrupt Timer8Irq(void); /* Assembly */
extern void BURGER_API InitTimer8Irq(void);
extern uint_t BURGER_API MyIrq8(void);
#ifdef __cplusplus
}
#endif

/***************************************

	My timer 0 ISR which handles the different timings and
	dispatches to whatever other routines are appropriate

***************************************/

uint_t BURGER_API MyIrq8(void)
{
	// Get the instance pointer
	Burger::Tick* pThis = Burger::Tick::get_instance();

	// Perform the tick
	++pThis->m_u60HertzTick;

	// Time for a TRUE irq #8
	pThis->m_uDelta += pThis->m_uStepUnits;

	// Has 18.2 hertz been hit?
	if (pThis->m_uDelta >= 0x10000) {

		// Remove the overflow
		pThis->m_uDelta -= 0x10000;

		// Allow chaining
		return 1;
	}

	// Acknowledge the interrupt
	outp(0x20, 0x20);
	return 0;
}

/***************************************

	\brief Initialize the low level timer manager

	Start up the low level timer

	\sa shutdown()

***************************************/

void BURGER_API Burger::Tick::init(void) BURGER_NOEXCEPT
{
	// Init the variables
	Tick* pThis = &g_Tick;
	if (!pThis->m_bInitialized) {
		pThis->m_uHighPrecisionFrequency = CLOCKS_PER_SEC;
		pThis->m_uLast60HertzMark = 1;
		pThis->m_uDelta = 0;
		pThis->m_u60HertzTick = 1;
		pThis->m_1KHertz.init(1000U);
		pThis->m_1MHertz.init(1000000U);

		// Get old timer 0 Interrupt Service Routine
		pThis->m_pPreviousINT8 =
			static_cast<void(__interrupt __far*)()>(GetProtInt(8));

		pThis->m_uPreviousRealService = GetRealInt(8);

		// Init the sound IRQ code
		InitTimer8Irq();

		// Set to my timer 0 ISR
		SetBothInts(8, Timer8Irq);
		const uint32_t uStepUnits = 1192030UL / 60UL;

		// Save the speed value
		pThis->m_uStepUnits = uStepUnits;

		// Change timer 0
		outp(0x43, 0x36);
		outp(0x40, uStepUnits);
		outp(0x40, uStepUnits >> 8);

		pThis->m_bInitialized = TRUE;
	}
}

/***************************************

	\brief Shut down the low level timer manager

	Shut down the low level timer

	\sa init()

***************************************/

void BURGER_API Burger::Tick::shutdown(void) BURGER_NOEXCEPT
{
	Tick* pThis = &g_Tick;
	if (pThis->m_bInitialized) {

		// Restore timer 0
		outp(0x43, 0x36);
		outp(0x40, 0);
		outp(0x40, 0);

		// Restore the speed value
		pThis->m_uStepUnits = 0x10000;

		// Restore the vectors
		SetRealInt(8, pThis->m_uPreviousRealService);
		SetProtInt(8, pThis->m_pPreviousINT8);
		pThis->m_pPreviousINT8 = nullptr;
		pThis->m_bInitialized = FALSE;
	}
}

#endif

/***************************************

	Read a system tick
	MS-DOS common code

***************************************/

uint32_t BURGER_API Burger::Tick::read(void) BURGER_NOEXCEPT
{
	// Read from the interrupt timer
	return Tick::get_instance()->m_u60HertzTick;
}

#endif
