/***************************************

	Mouse Manager

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmouse.h"
#include "brtick.h"

/*! ************************************

	\class Burger::Mouse
	\brief Class for mouse input

	This class will handle the reading of mouse input on platforms
	that support mice.

	On some platforms, the reading of the mouse is handled by a background
	thread for low latency.

***************************************/

/*! ************************************

	\struct Burger::Mouse::MouseDataHeader_t
	\brief Base structure for mouse events

	All mouse events derive from this class. Based on the \ref eMouseEvent
	enumeration value in \ref m_eEvent, the derived class type can be determined

	\sa MouseButtonData_t, MouseMotionData_t, MousePositionData_t or MouseWheelData_t

***************************************/

/*! ************************************

	\struct Burger::Mouse::MouseButtonData_t
	\brief Structure for mouse button events

	\ref m_eEvent must be set to \ref EVENT_BUTTONDOWN or \ref EVENT_BUTTONUP.

	When posting a \ref EVENT_BUTTONDOWN or \ref EVENT_BUTTONUP event, only
	the \ref m_uButtons value needs to be set since \ref m_uX and \ref m_uY
	will be filled in by the PostMouseEvent() function.

	When receiving this event, all entries are valid.

	\sa MouseDataHeader_t, MouseMotionData_t, MousePositionData_t or MouseWheelData_t

***************************************/

/*! ************************************

	\struct Burger::Mouse::MouseMotionData_t
	\brief Structure for mouse motion events

	\ref m_eEvent must be set to \ref EVENT_MOVE.

	This structure is used for posting a mouse motion delta event.

	\sa MouseDataHeader_t, MouseButtonData_t, MousePositionData_t or MouseWheelData_t

***************************************/

/*! ************************************

	\struct Burger::Mouse::MousePositionData_t
	\brief Structure for mouse position events

	\ref m_eEvent must be set to \ref EVENT_POSITION.

	This structure is used for posting a mouse position event.
	The values will be clamped to the SetRange() parameters.

	\sa MouseDataHeader_t, MouseButtonData_t, MouseMotionData_t or MouseWheelData_t

***************************************/

/*! ************************************

	\struct Burger::Mouse::MouseWheelData_t
	\brief Structure for mouse wheel events

	\ref m_eEvent must be set to \ref EVENT_WHEEL.

	This structure is used for posting a mouse wheel event.

	\sa MouseDataHeader_t, MouseButtonData_t, MouseMotionData_t or MousePositionData_t

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MACOSX)) || defined(DOXYGEN)

/*! ************************************

	\brief Initialize the mouse device for input

	Open the mouse device driver (if needed) and prepare for input
	
	\param pAppInstance Pointer to the GameApp of
		the game that's requesting input.

***************************************/

Burger::Mouse::Mouse(GameApp *pAppInstance) :
	m_pAppInstance(pAppInstance),
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
	pAppInstance->SetMouse(this);
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

Word BURGER_API Burger::Mouse::PeekMouseEvent(MouseEvent_t *pEvent)
{
	m_MouseLock.Lock();
	Word uIndex = m_uArrayStart;	// Get the starting index
	Word uResult = FALSE;
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

Word BURGER_API Burger::Mouse::GetMouseEvent(MouseEvent_t *pEvent)
{
	m_MouseLock.Lock();
	Word uIndex = m_uArrayStart;		// Get the starting index
	Word uResult = FALSE;
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
Word BURGER_API Burger::Mouse::IsPresent(void) const
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

Word32 BURGER_API Burger::Mouse::ReadButtons(void)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
	Word uButtons = m_uButtons;
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

Word32 BURGER_API Burger::Mouse::ReadButtonDowns(void)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
	Word uButtons = m_uPressedButtons;
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
	\sa Read(Int32 *,Int32 *)

***************************************/

void BURGER_API Burger::Mouse::Read(Word32 *pX,Word32 *pY)
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
	\sa Read(Word32 *,Word32 *)

***************************************/

void BURGER_API Burger::Mouse::Read(Int32 *pX,Int32 *pY)
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

Int32 BURGER_API Burger::Mouse::ReadWheelX(void)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
	Int32 iResult = m_iMouseWheelX;
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

Int32 BURGER_API Burger::Mouse::ReadWheelY(void)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
	Int32 iResult = m_iMouseWheelY;
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
	\sa PostMousePosition(Word32,Word32,Word32)

***************************************/

void BURGER_API Burger::Mouse::SetRange(Word32 uBoundsX,Word32 uBoundsY)
{
	m_MouseLock.Lock();
	m_uArrayStart = 0;		// Clear out the events
	m_uArrayEnd = 0;
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
	\sa SetRange(Word32,Word32)

***************************************/

void BURGER_API Burger::Mouse::PostMousePosition(Word32 x,Word32 y,Word32 uMSTimeStamp)
{
	MousePositionData_t NewEvent;
	NewEvent.m_eEvent = EVENT_POSITION;
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
	\sa SetRange(Word32,Word32)

***************************************/

void BURGER_API Burger::Mouse::PostMouseMotion(Int32 x,Int32 y,Word32 uMSTimeStamp)
{
	MouseMotionData_t NewEvent;
	NewEvent.m_eEvent = EVENT_MOVE;
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
	\sa PostMouseUp(Word32,Word32)

***************************************/

void BURGER_API Burger::Mouse::PostMouseDown(Word32 uMouseBits,Word32 uMSTimeStamp)
{
	MouseButtonData_t NewEvent;
	NewEvent.m_eEvent = EVENT_BUTTONDOWN;
	NewEvent.m_uMSTimeStamp = uMSTimeStamp;
	NewEvent.m_uButtons = uMouseBits;
	PostMouseEvent(&NewEvent);
}

/*! ************************************

	\brief Post a mouse button up event

	Manually post a mouse button up event.

	\param uMouseBits eMouseButtons enumeration of mouse buttons
	\param uMSTimeStamp Timestamp in Milliseconds, 0 will fill in the value from Tick::ReadMilliseconds()
	\sa PostMouseDown(Word32,Word32)

***************************************/

void BURGER_API Burger::Mouse::PostMouseUp(Word32 uMouseBits,Word32 uMSTimeStamp)
{
	MouseButtonData_t NewEvent;
	NewEvent.m_eEvent = EVENT_BUTTONUP;
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
	\sa PostMouseUp(Word32,Word32) or PostMouseDown(Word32,Word32)

***************************************/

void BURGER_API Burger::Mouse::PostMouseWheel(Int32 iWheelXMovement,Int32 iWheelYMovement,Word32 uMSTimeStamp)
{
	MouseWheelData_t NewEvent;
	NewEvent.m_eEvent = EVENT_WHEEL;
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

Word BURGER_API Burger::Mouse::PostMouseEvent(const MouseDataHeader_t *pEvent)
{
	m_MouseLock.Lock();
	Word uResult = 10;
	Word uEnd = m_uArrayEnd;
	// See if there's room in the buffer
	Word uTemp = (uEnd+1)&(MOUSEBUFFSIZE-1);
	if (uTemp!=m_uArrayStart) {
		// Didn't wrap, accept it!
		m_uArrayEnd = uTemp;
	
		// Insert the new event
		MouseEvent_t *pNewEvent = &m_MouseEvents[uEnd];

		// Add the proper time stamp
		Word32 uTime = pEvent->m_uMSTimeStamp;
		if (!uTime) {
			uTime = Tick::ReadMilliseconds();
		}
		pNewEvent->m_Header.m_uMSTimeStamp = uTime;

		eMouseEvent uMouseEvent = pEvent->m_eEvent;
		pNewEvent->m_Header.m_eEvent = uMouseEvent;
		switch (uMouseEvent) {
		case EVENT_BUTTONDOWN:
		case EVENT_BUTTONUP:
			{
				// Grab the current location of the mouse for recording
				pNewEvent->m_Button.m_uX = m_uX;
				pNewEvent->m_Button.m_uY = m_uY;
				Word uButtons = static_cast<const MouseButtonData_t *>(pEvent)->m_uButtons;
				pNewEvent->m_Button.m_uButtons = uButtons;
				if (uMouseEvent==EVENT_BUTTONDOWN) {
					// Add to the buttons
					m_uButtons |= uButtons;
					m_uPressedButtons |= uButtons;
				} else {
					// Clear the buttons
					m_uButtons &= (~uButtons);
				}
			}
			break;
		case EVENT_POSITION:
			{
				// Given a position, compare to the previous location to derive
				// the mouse motion for code that requests delta motion

				Word32 uX = static_cast<const MousePositionData_t *>(pEvent)->m_uX;
				Word32 uY = static_cast<const MousePositionData_t *>(pEvent)->m_uY;
				Int32 iX = static_cast<Int32>(uX-m_uX);
				Int32 iY = static_cast<Int32>(uY-m_uY);
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
		case EVENT_MOVE:
			{
				// Given delta motion, move the global cursor

				Int32 iX = static_cast<const MouseMotionData_t *>(pEvent)->m_iDeltaX;
				Int32 iY = static_cast<const MouseMotionData_t *>(pEvent)->m_iDeltaY;
				m_iDeltaX += iX;
				m_iDeltaY += iY;
				Word32 uX = m_uX;
				Word32 uY = m_uY;
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
		case EVENT_WHEEL:
			{
				// Store the mouse wheel event
				Int32 iX = static_cast<const MouseWheelData_t *>(pEvent)->m_iMouseWheelX;
				Int32 iY = static_cast<const MouseWheelData_t *>(pEvent)->m_iMouseWheelY;
				m_iMouseWheelX += iX;
				m_iMouseWheelY += iY;
				pNewEvent->m_Wheel.m_iMouseWheelX = iX;
				pNewEvent->m_Wheel.m_iMouseWheelY = iY;
			}
			break;
		}
		uResult = 0;
	}
	m_MouseLock.Unlock();
	return uResult;
}
