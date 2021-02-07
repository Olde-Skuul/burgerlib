/***************************************

    MS-DOS version (DOS4GW dos extender)

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtick.h"

#if defined(BURGER_MSDOS)
#include "brdosextender.h"
#include <stdlib.h>
#include <conio.h>
#include <dos.h>

/***************************************

	Tick counter for a 60 hertz
	timer

***************************************/

#ifdef __cplusplus
extern "C" {
#endif
volatile Word32 ReadTickTimeCount;		/* Inc every 1/60th of a second */
void (__interrupt __far *OldInt8)();	/* Old INT 8 vector */
#ifdef __cplusplus
}
#endif
static volatile Word32 TimerDivisor;
static volatile Word32 TimerCount;
static Bool Started;				/* True if started */
static Bool ExitIn;			/* TRUE if atexit() */

#if defined(BURGER_DOS4G)

/***************************************

	My timer 0 ISR which handles the different timings and
	dispatches to whatever other routines are appropriate

***************************************/

static void __interrupt __far MyIrq8(void)
{
	++ReadTickTimeCount;
	TimerCount+=TimerDivisor;	/* Time for a TRUE irq #8 */
	if (TimerCount>=0x10000) {
		TimerCount-=0x10000;	/* Atomic counter */
		_chain_intr(OldInt8);
	}
	outp(0x20,0x20);	/* Ack the interrupt */
}

/***************************************

	Shuts down the Sound Mgr
	Removes sound ISR and turns off whatever sound hardware was active

***************************************/

void UninstallTick(void)
{
	if (OldInt8) {
		outp(0x43,0x36);			/* Change timer 0 */
		outp(0x40,0);
		outp(0x40,0);
		TimerDivisor = 0x10000;		/* Save the speed value */
		_dos_setvect(8,OldInt8);	/* Restore vectors */
		OldInt8 = 0;
	}
}

/***************************************

	Install the tick manager

***************************************/

static void InstallTick(void)
{
	if (!OldInt8) {
		OldInt8 = _dos_getvect(8);	/* Get old timer 0 ISR */
	}
	_dos_setvect(8,MyIrq8);	/* Set to my timer 0 ISR */
	{
		Word32 speed;
		speed = (1192030UL/60UL);
		outp(0x43,0x36);		/* Change timer 0 */
		outp(0x40,speed);
		outp(0x40,speed >> 8);
		TimerDivisor = speed;	/* Save the speed value */
	}
}

#else

/***************************************

	X32 Dos extender

***************************************/

#ifdef __cplusplus
extern "C" {
#endif
extern void interrupt Timer8Irq(void);	/* Assembly */
extern void BURGER_API InitTimer8Irq(void);
extern Word BURGER_API MyIrq8(void);
#ifdef __cplusplus
}
#endif
static Word32 t0OldRealService;	/* Old real mode INT 8 vector */

/***************************************

	My timer 0 ISR which handles the different timings and
	dispatches to whatever other routines are appropriate

***************************************/

Word BURGER_API MyIrq8(void)
{
	++ReadTickTimeCount;
	TimerCount+=TimerDivisor;		/* Time for a TRUE irq #8 */
	if (TimerCount>=0x10000) {
		TimerCount-=0x10000;		/* Atomic counter */
		return 1;
	}
	outp(0x20,0x20);		/* Ack the interrupt */
	return 0;
}

/***************************************

	Shuts down the Sound Mgr
	Removes sound ISR and turns off whatever sound hardware was active

***************************************/

void BURGER_API UninstallTick(void)
{
	if (OldInt8) {
		outp(0x43,0x36);				/* Change timer 0 */
		outp(0x40,0);
		outp(0x40,0);
		TimerDivisor = 0x10000;			/* Save the speed value */
		SetRealInt(8,t0OldRealService);		/* Restore the vector */
		SetProtInt(8,OldInt8);	/* Restore vectors */
		OldInt8 = 0;
	}
}

/***************************************

	Starts up the Time manager
	Detects all additional sound hardware and installs my ISR

***************************************/

static void BURGER_API InstallTick(void)
{
	Word32 speed;
	OldInt8 = static_cast<void (__interrupt __far *)()>(GetProtInt(8));	/* Get old timer 0 ISR */
	t0OldRealService = GetRealInt(8);
	InitTimer8Irq();			/* Init the sound IRQ code */
	SetBothInts(8,Timer8Irq);	/* Set to my timer 0 ISR */
	speed = 1192030UL/60UL;
	outp(0x43,0x36);		/* Change timer 0 */
	outp(0x40,speed);
	outp(0x40,speed >> 8);
	TimerDivisor = speed;	/* Save the speed value */
}

#endif


/***************************************

	Read a system tick
	MS-DOS common code

***************************************/

Word32 BURGER_API Burger::Tick::Read(void)
{
	if (Started) {		/* Do I need to be initialized? */
		return ReadTickTimeCount;
	}
	if (!ExitIn) {
		ExitIn = TRUE;
		atexit(UninstallTick);
	}
	InstallTick();
	Started = TRUE;			/* I'm started */
	return ReadTickTimeCount;
}


#endif
