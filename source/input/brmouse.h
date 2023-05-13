/***************************************

    Mouse Manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMOUSE_H__
#define __BRMOUSE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRTHREAD_H__
#include "brthread.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

/* BEGIN */
struct IDirectInputDevice8W;
namespace Burger {
class Mouse : public Base {
    BURGER_DISABLE_COPY(Mouse);
	BURGER_RTTI_IN_CLASS();

public:
	enum {
		MOUSEBUFFSIZE=128			///< Number of mouse events in the event cache
	};

	enum eMouseButtons {
		BUTTON_LEFT=0x1,			///< Mask for the left mouse button
		BUTTON_RIGHT=0x2,			///< Mask for the right mouse button
		BUTTON_MIDDLE=0x4,			///< Mask for the center mouse button
		BUTTON_4=0x8,				///< Mask for the 4th mouse button
		BUTTON_5=0x10,				///< Mask for the 5th mouse button
		BUTTON_6=0x20,				///< Mask for the 6th mouse button
		BUTTON_7=0x40,				///< Mask for the 7th mouse button
		BUTTON_8=0x80,				///< Mask for the 8th mouse button
		BUTTON_9=0x100,				///< Mask for the 9th mouse button
		BUTTON_10=0x200,			///< Mask for the 10th mouse button
		BUTTON_11=0x400,			///< Mask for the 11th mouse button
		BUTTON_12=0x800,			///< Mask for the 12th mouse button
		BUTTON_13=0x1000,			///< Mask for the 13th mouse button
		BUTTON_14=0x2000,			///< Mask for the 14th mouse button
		BUTTON_15=0x4000,			///< Mask for the 15th mouse button
		BUTTON_16=0x8000			///< Mask for the 16th mouse button
	};

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	enum {
		MAX_MOUSE_DEVICE_COUNT=8	///< Maximum number of mice/trackpads OSX will track
	};
	struct DeviceStruct {
		__IOHIDDevice *m_pDevice;	///< Pointer to the device driver
		String m_Name;				///< Name of the device driver
		uint_t m_bUnplugged;			///< \ref TRUE if this device was unplugged
	};
#endif

	union MouseEvent_t {
		EventHeader_t m_Header;		///< Header shared by all data chunks
		MouseButtonEvent_t m_Button;		///< Data for a EVENT_BUTTONUP or EVENT_BUTTONDOWN event
		MouseWheelEvent_t m_Wheel;		///< Data for a EVENT_WHEEL event
		MouseMotionEvent_t m_Motion;		///< Data for a EVENT_MOVE event
		MousePositionEvent_t m_Position;	///< Data for a EVENT_POSITION event
	};

protected:
	GameApp *m_pGameApp;			///< Application instance
	CriticalSection m_MouseLock;	///< Lock for multi-threading

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	static uintptr_t BURGER_API WindowsMouseThread(void *pData);
	IDirectInputDevice8W *m_pMouseDevice;	///< Direct input device (Windows only)
	void *m_pMouseEvent;			///< Event signal for DirectInput (Windows only)
	Thread m_MouseThread;			///< Asynchronous thread monitoring DirectInput (Windows only)
	uint_t m_bAcquired;				///< \ref TRUE if DirectInput8 is active (Windows only)
	volatile uint32_t m_bQuit;		///< \ref TRUE when the thread is shutting down (Windows only)
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	__IOHIDManager *m_pHIDManager;	///< HID Manager pointer
	DeviceStruct m_Mice[MAX_MOUSE_DEVICE_COUNT];	///< Array of mice devices
	uint_t m_uMiceCount;				///< Number of mice devices found
#endif

	uint32_t m_uX;					///< Current X coordinate of the mouse
	uint32_t m_uY;					///< Current Y coordinate of the mouse
	uint32_t m_uBoundsX;				///< Screen width bounds for the mouse
	uint32_t m_uBoundsY;				///< Screen height bounds for the mouse
	int32_t m_iDeltaX;				///< Accumulated X motion of the mouse
	int32_t m_iDeltaY;				///< Accumulated Y motion of the mouse
	int32_t m_iMouseWheelX;			///< Accumulated X motion of the mouse wheel
	int32_t m_iMouseWheelY;			///< Accumulated Y motion of the mouse wheel
	uint32_t m_uButtons;				///< Current state of the mouse buttons
	uint32_t m_uPressedButtons;		///< Mouse buttons pressed since last read
	uint_t m_bButtonSwap;				///< Switch the left and right buttons for lefties
	uint_t m_uArrayStart;				///< Read index for m_KeyEvents
	uint_t m_uArrayEnd;				///< Write index for m_KeyEvents
	MouseEvent_t m_MouseEvents[MOUSEBUFFSIZE];	///< Circular buffer holding mouse events
public:
	Mouse(GameApp *pGameApp);
	virtual ~Mouse();
	uint_t BURGER_API PeekMouseEvent(MouseEvent_t *pEvent);
	uint_t BURGER_API GetMouseEvent(MouseEvent_t *pEvent);
	uint_t BURGER_API IsPresent(void) const;
	uint32_t BURGER_API ReadButtons(void);
	uint32_t BURGER_API ReadButtonDowns(void);
	void BURGER_API Read(uint32_t *pX,uint32_t *pY);
	void BURGER_API Read(int32_t *pX,int32_t *pY);
	int32_t BURGER_API ReadWheelX(void);
	int32_t BURGER_API ReadWheelY(void);
	void BURGER_API SetRange(uint32_t x,uint32_t y);
	void BURGER_API PostMousePosition(uint32_t x,uint32_t y,uint32_t uMSTimeStamp=0);
	void BURGER_API PostMouseMotion(int32_t x,int32_t y,uint32_t uMSTimeStamp=0);
	void BURGER_API PostMouseDown(uint32_t uMouseBits,uint32_t uMSTimeStamp=0);
	void BURGER_API PostMouseUp(uint32_t uMouseBits,uint32_t uMSTimeStamp=0);
	void BURGER_API PostMouseWheel(int32_t iWheelXMovement,int32_t iWheelYMovement,uint32_t uMSTimeStamp=0);
	uint_t BURGER_API PostMouseEvent(const EventHeader_t *pEvent);

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	void BURGER_API AcquireDirectInput(void) BURGER_NOEXCEPT;
	void BURGER_API UnacquireDirectInput(void) BURGER_NOEXCEPT;
	void BURGER_API ReadSystemMouseValues(void) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_MACOSX) || defined(DOXYGEN)
	static void EnumerationCallback(void *pData,int iReturn,void *pSender,__IOHIDDevice *pDevice);
	static void DisconnectionCallback(void *pData,int iReturn,void *pSender);
	static void InputCallback(void *pData,int iReturn,void *pSender,__IOHIDValue *pValue);
	static RunQueue::eReturnCode BURGER_API Poll(void *pData) BURGER_NOEXCEPT;
#endif

};
}
/* END */

#endif
