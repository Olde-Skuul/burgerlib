/***************************************

    Mouse Manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brmouse.h"
#include "brtick.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Mouse,Burger::Base);
#endif

/*! ************************************

	\class Burger::Mouse
	\brief Class for mouse input

	This class will handle the reading of mouse input on platforms
	that support mice.

	On some platforms, the reading of the mouse is handled by a background
	thread for low latency.

***************************************/



#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)

/*! ************************************

	\brief Initialize the mouse device for input

	Open the mouse device driver (if needed) and prepare for input
	
	\param pGameApp Pointer to the GameApp of
		the game that's requesting input.

***************************************/

Burger::Mouse::Mouse(GameApp *pGameApp) :
	m_pGameApp(pGameApp),
	m_MouseLock(),
	m_uX(0),
	m_uY(0),
	m_uBoundsX(640),
	m_uBoundsY(480),
	m_iDeltaX(0),
	m_iDeltaY(0),
	m_iMouseWheelX(0),
	m_iMouseWheelY(0),
	m_uButtons(0),
	m_uPressedButtons(0),
	m_bButtonSwap(FALSE),
	m_uArrayStart(0),
	m_uArrayEnd(0)
{
}

/*! ************************************

	\brief Release the mouse device

	Close the mouse device driver (if needed)

***************************************/

Burger::Mouse::~Mouse()
{
}

#endif

/*! ************************************

	\brief Peek at the next mouse event

	See if an event is pending from the mouse, if
	so, return the event without removing it from
	the queue.

	\param pEvent Pointer to a buffer to receive the event
	\return \ref FALSE if no event is pending, \ref TRUE if an event is pending
	\sa GetMouseEvent(MouseEvent_t *pEvent)

***************************************/

uint_t BURGER_API Burger::Mouse::PeekMouseEvent(MouseEvent_t *pEvent)
{
	m_MouseLock.Lock();
	uint_t uIndex = m_uArrayStart;	// Get the starting index
	uint_t uResult = FALSE;
	if (uIndex!=m_uArrayEnd) {		// Anything in the buffer?
		pEvent[0] = m_MouseEvents[uIndex];
		uResult = TRUE;
	}
	m_MouseLock.Unlock();
	// No event pending
	return uResult;
}

/*! ************************************

	\brief Get the next mouse event

	See if an event is pending from the mouse, if
	so, return the event after removing it from
	the queue.

	\param pEvent Pointer to a buffer to receive the event
	\return \ref FALSE if no event is pending, \ref TRUE if an event is pending
	\sa PeekMouseEvent(MouseEvent_t *pEvent)

***************************************/

uint_t BURGER_API Burger::Mouse::GetMouseEvent(MouseEvent_t *pEvent)
{
	m_MouseLock.Lock();
	uint_t uIndex = m_uArrayStart;		// Get the starting index
	uint_t uResult = FALSE;
	if (uIndex!=m_uArrayEnd) {			// Anything in the buffer?
		pEvent[0] = m_MouseEvents[uIndex];
		m_uArrayStart = (uIndex+1)&(MOUSEBUFFSIZE-1);	// Next event
		uResult = TRUE;
	}
	m_MouseLock.Unlock();
	return uResult;
}

/*! ************************************

	\brief Detect if a mouse is plugged in

	Scan if there are any mice present in the system and
	return \ref TRUE if one was found.

	On platforms that don't have mice, such as mobile
	devices, this always returns \ref FALSE

	\return \ref TRUE if a mouse is present, \ref FALSE if not.

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)
uint_t BURGER_API Burger::Mouse::IsPresent(void) const
{
	return FALSE;
}
#endif

/*! ************************************

	\brief Return the buttons pressed

	Return the state of the mouse buttons at this precise moment.

	\return Zero if no buttons are pressed or a bit
		mask of Mouse::eMouseButtons for all buttons that are pressed.
	\sa ReadButtonDowns()

***************************************/

uint32_t BURGER_API Burger::Mouse::ReadButtons(void)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
	uint_t uButtons = m_uButtons;
	m_MouseLock.Unlock();
	return uButtons;
}

/*! ************************************

	\brief Return the buttons clicked

	Since the last time this function was called,
	set bits for which mouse buttons were pressed and 
	then clear the flags. This function is used
	to handle mouse press events without the overhead
	of having to monitor for a mouse up / down
	cycle.

	\note This function can return multiple different mouse clicks
		at the same time. Test each bit individually if the user
		interface performs different actions for different buttons.

	\return Zero if no buttons have been pressed since the last call, or a bit
		mask of eMouseButtons for all buttons that were pressed.
	\sa ReadButtons()

***************************************/

uint32_t BURGER_API Burger::Mouse::ReadButtonDowns(void)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
	uint_t uButtons = m_uPressedButtons;
	m_uPressedButtons = 0;	// Acknowledge the button events
	m_MouseLock.Unlock();
	return uButtons;
}

/*! ************************************

	\brief Return the current mouse location

	Set the mouse to cursor tracking mode and return the 
	current location of the mouse cursor on the screen

	\param pX Pointer to a value to receive the X coordinate of the mouse cursor or \ref NULL if this value is not requested
	\param pY Pointer to a value to receive the Y coordinate of the mouse cursor or \ref NULL if this value is not requested
	\sa Read(int32_t *,int32_t *)

***************************************/

void BURGER_API Burger::Mouse::Read(uint32_t *pX,uint32_t *pY)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
	if (pX) {
		pX[0] = m_uX;		// Capture the current mouse location
	}
	if (pY) {
		pY[0] = m_uY;
	}
	m_MouseLock.Unlock();
}

/*! ************************************

	\brief Return the mouse motion

	Set the mouse to relative tracking mode and return the 
	amount of motion the mouse has made since the last time it
	was read.

	\param pX Pointer to a value to receive the X coordinate of the mouse delta or \ref NULL if this value is not requested
	\param pY Pointer to a value to receive the Y coordinate of the mouse delta or \ref NULL if this value is not requested
	\sa Read(uint32_t *,uint32_t *)

***************************************/

void BURGER_API Burger::Mouse::Read(int32_t *pX,int32_t *pY)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
	if (pX) {
		pX[0] = m_iDeltaX;	// Capture the current mouse location
	}
	if (pY) {
		pY[0] = m_iDeltaY;
	}
	m_iDeltaX = 0;
	m_iDeltaY = 0;
	m_MouseLock.Unlock();
}

/*! ************************************

	\brief Return the mouse horizontal scroll wheel

	For some mice, the Apple Mighty Mouse for example, have a scroll
	ball which returns horizontal motion. This function returns that
	value.

	\returns A signed value of the amount of motion in the X axis from the mouse trackball.
	\sa ReadWheelY(void)

***************************************/

int32_t BURGER_API Burger::Mouse::ReadWheelX(void)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
	int32_t iResult = m_iMouseWheelX;
	m_iMouseWheelX = 0;
	m_MouseLock.Unlock();
	return iResult;
}

/*! ************************************

	\brief Return the mouse vertical scroll wheel

	Most mice have a vertical scroll wheel. Return the motion delta.

	\returns A signed value of the amount of motion in the Y axis from the mouse scroll wheel.
	\sa ReadWheelX(void)

***************************************/

int32_t BURGER_API Burger::Mouse::ReadWheelY(void)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
	int32_t iResult = m_iMouseWheelY;
	m_iMouseWheelY = 0;
	m_MouseLock.Unlock();
	return iResult;
}

/*! ************************************

	\brief Set the range of the mouse cursor

	On full screen applications, or systems where the mouse driver is 
	completely handled by Burgerlib, set the range of motion for
	the mouse so the driver can properly clamp the cursor to 
	the screen bounds.

	\param uBoundsX Width of the screen in pixels
	\param uBoundsY Height of the screen in pixels
	\sa PostMousePosition(uint32_t,uint32_t,uint32_t)

***************************************/

void BURGER_API Burger::Mouse::SetRange(uint32_t uBoundsX,uint32_t uBoundsY)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;

	// Minimum bounds must be 1,1 to keep 
	// -1 from underflowing

	if (!uBoundsX) {
		uBoundsX = 1;
	}
	if (!uBoundsY) {
		uBoundsY = 1;
	}

	m_uBoundsX = uBoundsX;
	m_uBoundsY = uBoundsY;

	// Set the new bounds
	if (m_uX>=uBoundsX) {
		m_uX=uBoundsX-1;
	}
	if (m_uY>=uBoundsY) {
		m_uY=uBoundsY-1;
	}
	m_MouseLock.Unlock();
}

/*! ************************************

	\brief Set the location of the mouse cursor

	Set's the location of the mouse cursor.
	It will be clamped by the cursor bounds.

	\param x X coordinate of the mouse cursor in pixels
	\param y Y coordinate of the mouse cursor in pixels
	\param uMSTimeStamp Timestamp in Milliseconds, 0 will fill in the value from Tick::ReadMilliseconds()
	\sa SetRange(uint32_t,uint32_t)

***************************************/

void BURGER_API Burger::Mouse::PostMousePosition(uint32_t x,uint32_t y,uint32_t uMSTimeStamp)
{
	MousePositionEvent_t NewEvent;
	NewEvent.m_uEvent = EVENT_MOUSEPOSITION;
	NewEvent.m_uWhich = 0;
	NewEvent.m_uMSTimeStamp = uMSTimeStamp;
	NewEvent.m_uX = x;
	NewEvent.m_uY = y;
	PostMouseEvent(&NewEvent);
}

/*! ************************************

	\brief Set the location of the mouse cursor

	Set's the location of the mouse cursor.
	It will be clamped by the cursor bounds.

	\param x X coordinate of the mouse cursor in pixels
	\param y Y coordinate of the mouse cursor in pixels
	\param uMSTimeStamp Timestamp in Milliseconds, 0 will fill in the value from Tick::ReadMilliseconds()
	\sa SetRange(uint32_t,uint32_t)

***************************************/

void BURGER_API Burger::Mouse::PostMouseMotion(int32_t x,int32_t y,uint32_t uMSTimeStamp)
{
	MouseMotionEvent_t NewEvent;
	NewEvent.m_uEvent = EVENT_MOUSEMOVE;
	NewEvent.m_uWhich = 0;
	NewEvent.m_uMSTimeStamp = uMSTimeStamp;
	NewEvent.m_iDeltaX = x;
	NewEvent.m_iDeltaY = y;
	PostMouseEvent(&NewEvent);
}

/*! ************************************

	\brief Post a mouse button down event

	Manually post a mouse button down event.

	\param uMouseBits eMouseButtons enumeration of mouse buttons
	\param uMSTimeStamp Timestamp in Milliseconds, 0 will fill in the value from Tick::ReadMilliseconds()
	\sa PostMouseUp(uint32_t,uint32_t)

***************************************/

void BURGER_API Burger::Mouse::PostMouseDown(uint32_t uMouseBits,uint32_t uMSTimeStamp)
{
	MouseButtonEvent_t NewEvent;
	NewEvent.m_uEvent = EVENT_MOUSEDOWN;
	NewEvent.m_uWhich = 0;
	NewEvent.m_uMSTimeStamp = uMSTimeStamp;
	NewEvent.m_uButtons = uMouseBits;
	PostMouseEvent(&NewEvent);
}

/*! ************************************

	\brief Post a mouse button up event

	Manually post a mouse button up event.

	\param uMouseBits eMouseButtons enumeration of mouse buttons
	\param uMSTimeStamp Timestamp in Milliseconds, 0 will fill in the value from Tick::ReadMilliseconds()
	\sa PostMouseDown(uint32_t,uint32_t)

***************************************/

void BURGER_API Burger::Mouse::PostMouseUp(uint32_t uMouseBits,uint32_t uMSTimeStamp)
{
	MouseButtonEvent_t NewEvent;
	NewEvent.m_uEvent = EVENT_MOUSEUP;
	NewEvent.m_uWhich = 0;
	NewEvent.m_uMSTimeStamp = uMSTimeStamp;
	NewEvent.m_uButtons = uMouseBits;
	PostMouseEvent(&NewEvent);
}

/*! ************************************

	\brief Manually move the mouse wheel

	Move the mouse wheels. The first value is for VERTICAL motion and
	the second is horizontal

	\param iWheelYMovement Motion for the vertical mouse wheel
	\param iWheelXMovement Motion for the horizontal mouse wheel
	\param uMSTimeStamp Timestamp in Milliseconds, 0 will fill in the value from Tick::ReadMilliseconds()
	\sa PostMouseUp(uint32_t,uint32_t) or PostMouseDown(uint32_t,uint32_t)

***************************************/

void BURGER_API Burger::Mouse::PostMouseWheel(int32_t iWheelXMovement,int32_t iWheelYMovement,uint32_t uMSTimeStamp)
{
	MouseWheelEvent_t NewEvent;
	NewEvent.m_uEvent = EVENT_MOUSEWHEEL;
	NewEvent.m_uWhich = 0;
	NewEvent.m_uMSTimeStamp = uMSTimeStamp;
	NewEvent.m_iMouseWheelX = iWheelXMovement;
	NewEvent.m_iMouseWheelY = iWheelYMovement;
	PostMouseEvent(&NewEvent);
}

/*! ************************************

	\brief Post a mouse event

	Post the event in the ring buffer and update the current mouse state

	\param pEvent Pointer to a filled in Mouse event
	\return Zero if the event was posted, non-zero if the buffer is full

***************************************/

uint_t BURGER_API Burger::Mouse::PostMouseEvent(const EventHeader_t *pEvent)
{
	m_MouseLock.Lock();
	uint_t uResult = 10;
	uint_t uEnd = m_uArrayEnd;
	// See if there's room in the buffer
	uint_t uTemp = (uEnd+1)&(MOUSEBUFFSIZE-1);
	if (uTemp!=m_uArrayStart) {
		// Didn't wrap, accept it!
		m_uArrayEnd = uTemp;
	
		// Insert the new event
		MouseEvent_t *pNewEvent = &m_MouseEvents[uEnd];

		// Add the proper time stamp
		uint32_t uTime = pEvent->m_uMSTimeStamp;
		if (!uTime) {
			uTime = Tick::ReadMilliseconds();
		}
		pNewEvent->m_Header.m_uMSTimeStamp = uTime;

		eEvent uMouseEvent = pEvent->m_uEvent;
		pNewEvent->m_Header.m_uEvent = uMouseEvent;
		pNewEvent->m_Header.m_uWhich = pEvent->m_uWhich;

		switch (uMouseEvent) {
		case EVENT_MOUSEDOWN:
		case EVENT_MOUSEUP:
			{
				// Grab the current location of the mouse for recording
				pNewEvent->m_Button.m_uX = m_uX;
				pNewEvent->m_Button.m_uY = m_uY;
				uint_t uButtons = static_cast<const MouseButtonEvent_t *>(pEvent)->m_uButtons;
				pNewEvent->m_Button.m_uButtons = uButtons;
				if (uMouseEvent==EVENT_MOUSEDOWN) {
					// Add to the buttons
					m_uButtons |= uButtons;
					m_uPressedButtons |= uButtons;
				} else {
					// Clear the buttons
					m_uButtons &= (~uButtons);
				}
			}
			break;

		case EVENT_MOUSEPOSITION:
			{
				// Given a position, compare to the previous location to derive
				// the mouse motion for code that requests delta motion

				uint32_t uX = static_cast<const MousePositionEvent_t *>(pEvent)->m_uX;
				uint32_t uY = static_cast<const MousePositionEvent_t *>(pEvent)->m_uY;
				int32_t iX = static_cast<int32_t>(uX-m_uX);
				int32_t iY = static_cast<int32_t>(uY-m_uY);
				m_iDeltaX += iX;		// Update the delta motion
				m_iDeltaY += iY;
				
				// Clamp to bounds
				if (uX>=m_uBoundsX) {
					uX = m_uBoundsX-1;
				}
				if (uY>=m_uBoundsY) {
					uY = m_uBoundsY-1;
				}
				m_uX = uX;
				m_uY = uY;
				pNewEvent->m_Position.m_uX = uX;
				pNewEvent->m_Position.m_uY = uY;
			}
			break;

		case EVENT_MOUSEMOVE:
			{
				// Given delta motion, move the global cursor

				int32_t iX = static_cast<const MouseMotionEvent_t *>(pEvent)->m_iDeltaX;
				int32_t iY = static_cast<const MouseMotionEvent_t *>(pEvent)->m_iDeltaY;
				m_iDeltaX += iX;
				m_iDeltaY += iY;
				uint32_t uX = m_uX;
				uint32_t uY = m_uY;
				uX += iX;
				uY += iY;
				// Handle wrap around by checking the delta's signs
				if (uX>=m_uBoundsX) {
					if (iX<0) {
						uX = 0;
					} else {
						uX = m_uBoundsX-1;
					}
				}
				if (uY>=m_uBoundsY) {
					if (iY<0) {
						uY = 0;
					} else {
						uY = m_uBoundsY-1;
					}
				}
				m_uX = uX;
				m_uY = uY;
				pNewEvent->m_Motion.m_iDeltaX = iX;
				pNewEvent->m_Motion.m_iDeltaY = iY;
			}
			break;

		case EVENT_MOUSEWHEEL:
			{
				// Store the mouse wheel event
				int32_t iX = static_cast<const MouseWheelEvent_t *>(pEvent)->m_iMouseWheelX;
				int32_t iY = static_cast<const MouseWheelEvent_t *>(pEvent)->m_iMouseWheelY;
				m_iMouseWheelX += iX;
				m_iMouseWheelY += iY;
				pNewEvent->m_Wheel.m_iMouseWheelX = iX;
				pNewEvent->m_Wheel.m_iMouseWheelY = iY;
			}
		default:
			break;
		}
		uResult = 0;
	}
	m_MouseLock.Unlock();
	return uResult;
}
