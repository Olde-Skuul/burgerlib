/***************************************

	Autorepeat manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brautorepeat.h"
#include "brtick.h"

/*! ************************************

	\class Burger::AutoRepeat
	\brief Autorepeat a joystick

	Sometimes it would be good to simulate an autorepeat feature in a joystick fire
	button. For each event that can have this feature, create a Burger::AutoRepeat and
	initialize it with the constructor and you can later change it with Init().
	Then call this function with data read from
	Burger::Joypad::ReadButtons(). If \ref TRUE is
	returned, then process the joystick button press. This is useful for
	rapid fire games.

	The structure needs to have constants set for the number of
	ticks to wait before auto firing and the time for the firing rate. These
	values are in 1/60th of a second and are compared to the call ReadTick(). Finally
	a bit mask of the joypad fire button bits of interest for this instance

***************************************/

/*! ************************************

	\brief Default constructor

	Create the class with the joypad bits of interest, the initial time delay
	in 1/60ths of a second and the repeat time delay in 1/60s of a second
	\param uButtons Bit mask of "buttons" that could be pressed to start rapid firing.
	\param uInitialTick Time in 1/60ths of a second for the initial timer to start rapid firing
	\param uRepeatTick Time in 1/60ths of a second for the rapid firing action

***************************************/

Burger::AutoRepeat::AutoRepeat(Word32 uButtons,Word32 uInitialTick,Word32 uRepeatTick)
{
	// Reset the class
	m_uFlags = 0;
	// Initialize the constants
	m_uButtons = uButtons;
	m_uInitialTick = uInitialTick;
	m_uRepeatTick = uRepeatTick;
}

/*! ************************************

	\fn Burger::AutoRepeat::Reset(void)
	\brief Reset the timer

	Reset the class to a "power up" state. Will not allow "firing" until the
	joypad bits are clear to prevent starting with firing enabled.

***************************************/

/*! ************************************

	\brief Change constants for the class

	Like the constructor, this function will set the class with the joypad bits of interest,
	the initial time delay
	in 1/60ths of a second and the repeat time delay in 1/60s of a second.

	This will supercede any precious settings

	\param uButtons Bit mask of "buttons" that could be pressed to start rapid firing.
	\param uInitialTick Time in 1/60ths of a second for the initial timer to start rapid firing
	\param uRepeatTick Time in 1/60ths of a second for the rapid firing action

***************************************/

void Burger::AutoRepeat::Init(Word32 uButtons,Word32 uInitialTick,Word32 uRepeatTick)
{
	m_uFlags = 0;
	m_uButtons = uButtons;
	m_uInitialTick = uInitialTick;
	m_uRepeatTick = uRepeatTick;
}

/*! ************************************

	\brief Autorepeat a joystick

	Sometimes it would be good to simulate an autorepeat feature in a joystick fire
	button. For each event that can have this feature, allocate an Burger::AutoRepeat and
	initialize it with zero. Then call this function with data read from
	Burger::Joypad::ReadButtons() masked with just the bits for the event. If \ref TRUE is
	returned, then process the joystick button press. This is useful for
	rapid fire games.

	The structure needs to have these constants set. InitialTick has the number of
	ticks to wait before auto firing and RepeatTick has the firing rate.

	\param uButtons Bit mask for either a single or group of joystick bits to process

	\return Returns \ref TRUE if the keydown should be processed by the application
	
	\sa Reset(), Init() and Burger::Joypad::ReadButtons()

***************************************/

Word Burger::AutoRepeat::IsItRepeating(Word32 uButtons)
{
	Word32 uNewMark = Tick::Read();		// Get the current time mark
	Word uFlags = m_uFlags;
	if (!(uFlags&INITIALIZED)) {		// Initialized?
		m_uTimeMark = uNewMark;			// Reset the timer
		if (m_uButtons & uButtons) {	// Initially held down?
			uFlags |= WAITFORKEYUP;		// No response until key up
		}
		uFlags |= INITIALIZED;			// Don't do this again...
	}

	// Assume not held down
	Word uResult = FALSE;
	if (!(m_uButtons & uButtons)) {		// Is it held down?
		// Nope, clear the held down bit
		uFlags &= ~(WAITFORKEYUP|HELDDOWNBEFORE|SECONDDELAY);
		// Reset the time mark
		m_uTimeMark = uNewMark;

	} else {

		// It's held down. In the "wait for key up" state? (Held down at the get go)
		if (!(m_uFlags & WAITFORKEYUP)) {

			// Is this a key down? (First time)
			if (!(uFlags & HELDDOWNBEFORE)) {

				// Mark as down, reset the timer and pass through an event
				uFlags |= HELDDOWNBEFORE;
				m_uTimeMark = uNewMark;
				uResult = TRUE;

			} else {
				// Held down for repeating, what's the delay?
				Word32 uDelay;
				if (m_uFlags & SECONDDELAY) {
					uDelay = m_uRepeatTick;
				} else {
					uDelay = m_uInitialTick;
				}
				// Has the amount of time elapsed for a new event?
				if (static_cast<Word32>(uNewMark-m_uTimeMark) >= uDelay) {
					// Adjust the mark to "push" the value ahead
					m_uTimeMark += uDelay;
					// Hmm still active? Extra long delay due to some wacky event?
					if (static_cast<Word32>(uNewMark-m_uTimeMark) >= uDelay) {
						// Failsafe for wrap around of timer
						m_uTimeMark = uNewMark;
					}
					// Use the second delay time from now on and pass in an event
					uFlags |= SECONDDELAY;
					uResult = TRUE;
				}
			}
		}
	}
	// Store the cached flags
	m_uFlags = uFlags;
	// Return TRUE on keypress event
	return uResult;
}
