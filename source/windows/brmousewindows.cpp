/***************************************

    Mouse Manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brmouse.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)
#define DIRECTINPUT_VERSION 0x0800
#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501				// Windows XP
#endif
#include "brgameapp.h"
#include "brglobals.h"
#include "brcriticalsection.h"
#include "bratomic.h"
#include "brtick.h"
#include <dinput.h>
#include <stdio.h>
#include <winreg.h>
#include <Setupapi.h>
#include <devguid.h>

// Size of the mouse buffer for reading events from DirectInput
#define DIRECTINPUT_MOUSEBUFFERSIZE 16

#endif

/*! ************************************

	\brief Windows thread for monitoring mouse events (Windows only)

	Burgerlib reads mouse events using DirectInput and
	asynchronous thread events.

	This function runs as a separate thread and processes
	all DirectInput events.

	\note This is used by the mouse manager, it's not intended
	to be used by applications.

	\param pData Pointer to the Mouse class instance
	\return Zero when asked to shut down

***************************************/

WordPtr BURGER_API Burger::Mouse::WindowsMouseThread(void *pData)
{
	// Get the pointer to the class instance
	Mouse *pThis = static_cast<Mouse *>(pData);
	DWORD uEventCode;
	for (;;) {
		// Sleep until an event occurred from DirectInput (Or shutdown)
		uEventCode = WaitForSingleObject(pThis->m_pMouseEvent,INFINITE);
		// Was the quit flag set for shutdown?
		if (pThis->m_bQuit) {
			break;
		}
		if (uEventCode==WAIT_OBJECT_0) {
			
			// Buffer to receive the mouse events
			DIDEVICEOBJECTDATA MouseData[DIRECTINPUT_MOUSEBUFFERSIZE];
			// Number of maximum entries to read!
			DWORD uCount = DIRECTINPUT_MOUSEBUFFERSIZE;
			IDirectInputDevice8W *pMouseDevice = pThis->m_pMouseDevice;

			// Let's get the data from the mouse device
			HRESULT hResult = pMouseDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),MouseData,&uCount,0);
			if (hResult<0) {
				// Error? Try getting the mouse again
				if (hResult == DIERR_INPUTLOST) {
					hResult = pMouseDevice->Acquire();
					if (hResult>=0) {
						// The mouse was reacquired, pull the data
						uCount = DIRECTINPUT_MOUSEBUFFERSIZE;
						hResult = pMouseDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),MouseData,&uCount,0);
					} else {
						pThis->m_bAcquired = FALSE;
					}
				} else {
					pThis->m_bAcquired = FALSE;
				}
			}

			// Was there any data read?
			if (hResult>=0) {
				Word i = uCount;
				if (i) {
					// Lock the data and update it
					pThis->m_MouseLock.Lock();
					const DIDEVICEOBJECTDATA *pObject = MouseData;
					do {
						// Offset into the data
						DWORD uDataOffset = pObject->dwOfs;
						// Data read in
						DWORD uData = pObject->dwData;
						Word32 uTimeStamp = Tick::ReadMilliseconds();

						// Mouse X motion event
						if (uDataOffset==DIMOFS_X) {
							// Update the mouse delta
							pThis->PostMouseMotion(static_cast<Int32>(uData),0,uTimeStamp);
						// Mouse Y motion event
						} else if (uDataOffset==DIMOFS_Y) {
							// Update the mouse delta
							pThis->PostMouseMotion(0,static_cast<Int32>(uData),uTimeStamp);
						// Handle the mouse Z
						} else if (uDataOffset==DIMOFS_Z) {
							pThis->PostMouseWheel(0,static_cast<Int32>(uData)/WHEEL_DELTA,uTimeStamp);

						// Handle the button downs
						} else if ((uDataOffset>=DIMOFS_BUTTON0) && (uDataOffset<=DIMOFS_BUTTON7)) {
							// Create the bitmask
							uDataOffset-=DIMOFS_BUTTON0;
							
							// If the mouse click is left or right, then switch them if
							// the user is left handed

							if (uDataOffset<2) {
								if (pThis->m_bButtonSwap) {
									uDataOffset^=1;
								}
							}
							Word uMask = 1U<<uDataOffset;
							// Mouse down?
							if (uData&0x80) {
								// Press the button
								pThis->PostMouseDown(uMask,uTimeStamp);
							} else {
								// Clear the button
								pThis->PostMouseUp(uMask,uTimeStamp);
							}
						}
						++pObject;
					} while (--i);
					pThis->m_MouseLock.Unlock();
				}
			}
		}
	}
	// Exit normally
	return 0;
}

/***************************************

	Initialize the mouse class

***************************************/

Burger::Mouse::Mouse(GameApp *pGameApp) :
	m_pGameApp(pGameApp),
	m_MouseLock(),
	m_pMouseDevice(NULL),
	m_pMouseEvent(NULL),
	m_MouseThread(),
	m_bAcquired(FALSE),
	m_bQuit(FALSE),
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
	// Read the left/right state from windows
	ReadSystemMouseValues();

	// First step, obtain DirectInput

	IDirectInput8W* pDirectInput8W = Windows::GetDirectInput8Singleton();
	if (pDirectInput8W) {

		// Create a system mouse device (Merges all mice)

		IDirectInputDevice8W *pMouseDeviceLocal = NULL;
		HRESULT hResult = pDirectInput8W->CreateDevice(GUID_SysMouse,&pMouseDeviceLocal,NULL);
		if (hResult>=0) {

			// Set the default data format of x, y, wheel and 8 buttons
			IDirectInputDevice8W *pMouseDevice = pMouseDeviceLocal;
			m_pMouseDevice = pMouseDevice;
			hResult = pMouseDevice->SetDataFormat(&c_dfDIMouse2);
			if (hResult>=0) {

				// Play nice with the system and let others use the keyboard
				// Disable the windows key if the application is running in the foreground

				hResult = pMouseDevice->SetCooperativeLevel(pGameApp->GetWindow(),DISCL_FOREGROUND | DISCL_EXCLUSIVE);
				if (hResult>=0) {
					DIPROPDWORD MouseProperties;
					
					MouseProperties.diph.dwSize = sizeof(DIPROPDWORD);
					MouseProperties.diph.dwHeaderSize = sizeof(DIPROPHEADER);
					MouseProperties.diph.dwObj = 0;
					MouseProperties.diph.dwHow = DIPH_DEVICE;
					MouseProperties.dwData = DIRECTINPUT_MOUSEBUFFERSIZE;
					hResult = pMouseDevice->SetProperty(DIPROP_BUFFERSIZE,&MouseProperties.diph);
					if (hResult>=0) {
						// Create an event for thread callbacks
						m_pMouseEvent = CreateEventW(NULL,FALSE,FALSE,NULL);
						hResult = pMouseDevice->SetEventNotification(m_pMouseEvent);
						m_MouseThread.Start(WindowsMouseThread,this);
						// Acquire Direct Input only if the application is full screen
						if (pGameApp->IsAppFullScreen()) {
							AcquireDirectInput();
							pGameApp->SetMouseOnScreen(TRUE);
						}
					}
				}
			}

			// If there was an error, shut down
			if (hResult<0) {
				// Uh oh...
				pMouseDevice->Unacquire();
				m_bAcquired = FALSE;
				pMouseDevice->SetEventNotification(NULL);
				if (m_pMouseEvent) {

					// Send the quit event
					m_bQuit = TRUE;
					SetEvent(m_pMouseEvent);
					
					// Wait for the thread to shut down
					m_MouseThread.Wait();
					CloseHandle(m_pMouseEvent);
					m_pMouseEvent = NULL;
				}
				// Release Direct Input
				pMouseDevice->Release();
				m_pMouseDevice = NULL;
			}
		}
	}
}

/***************************************

	Shut down the mouse class

***************************************/

Burger::Mouse::~Mouse()
{
	// Was a device allocated?
	IDirectInputDevice8W *pMouseDevice = m_pMouseDevice;
	if (pMouseDevice) {
		// Release it!
		pMouseDevice->Unacquire();
		m_bAcquired = FALSE;
		
		// Turn off asynchronous events
		pMouseDevice->SetEventNotification(NULL);
		m_bQuit = TRUE;

		// Send the quit event
		SetEvent(m_pMouseEvent);
		
		// Wait for the thread to shut down
		m_MouseThread.Wait();
		
		// Release the events
		CloseHandle(m_pMouseEvent);
		m_pMouseEvent = NULL;
		
		// Release Direct Input
		pMouseDevice->Release();
		m_pMouseDevice = NULL;
	}
}

/***************************************

	Return TRUE if a mouse is present in the device list

***************************************/

Word Burger::Mouse::IsPresent(void) const
{
	GUID HIDGUID;
	// Get the HID GUID
	Windows::HidD_GetHidGuid(&HIDGUID);
	HDEVINFO hDevInfo = Windows::SetupDiGetClassDevsW(&HIDGUID,NULL,NULL,DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
	Word uResult = FALSE;
	if (hDevInfo!=INVALID_HANDLE_VALUE) {
		uResult = TRUE;
		// Start at the first device
		Word32 uIndex = 0;
		for (;;) {
			SP_DEVICE_INTERFACE_DATA DeviceInterface;
			DeviceInterface.cbSize = sizeof(DeviceInterface);
			if (!Windows::SetupDiEnumDeviceInterfaces(hDevInfo,NULL,&HIDGUID,uIndex,&DeviceInterface)) {
				// Likely reached the end of the list
				break;
			}
			// Query the device name
			Word8 Buffer[4096];
			SP_DEVICE_INTERFACE_DETAIL_DATA_W *pDeviceInterfaceDetailData = reinterpret_cast<SP_DEVICE_INTERFACE_DETAIL_DATA_W *>(Buffer);
			pDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA_W);
			SP_DEVINFO_DATA DeviceInfoData;
			DeviceInfoData.cbSize = sizeof(DeviceInfoData);
			// Get the details about this device
			if (!Windows::SetupDiGetDeviceInterfaceDetailW(hDevInfo,&DeviceInterface,pDeviceInterfaceDetailData,sizeof(Buffer),NULL,&DeviceInfoData)) {\
				break;
			}
			// Is this a mouse device?
			if (GUIDIsEqual(&DeviceInfoData.ClassGuid,&GUID_DEVCLASS_MOUSE)) {
				uResult = TRUE;
				break;
			}
			++uIndex;
		}
		// Clean up
		Windows::SetupDiDestroyDeviceInfoList(hDevInfo);
	}
	// Return TRUE or FALSE if there was a mouse in the device list
	return uResult;
}

/*! ************************************

	\brief Acquire DirectInput (Windows only)

	Call Acquire() on the DirectInput mouse device.

	\sa UnacquireDirectInput(void)

***************************************/

void Burger::Mouse::AcquireDirectInput(void)
{
	IDirectInputDevice8W *pMouseDevice = m_pMouseDevice;
	if (pMouseDevice) {
		HRESULT hResult = pMouseDevice->Acquire();
		if (hResult>=0) {
			m_bAcquired = TRUE;
			m_pGameApp->SetMouseOnScreen(TRUE);
		}
	}
}

/*! ************************************

	\brief Release DirectInput (Windows only)

	Call Unacquire() on the DirectInput mouse device.

	\sa AcquireDirectInput(void)

***************************************/

void Burger::Mouse::UnacquireDirectInput(void)
{
	IDirectInputDevice8W *pMouseDevice = m_pMouseDevice;
	if (pMouseDevice) {
		m_bAcquired = FALSE;
		pMouseDevice->Unacquire();
	}
}

/*! ************************************

	\brief Read System mouse constants (Windows only)

	On startup and when a WM_SETTINGCHANGE event is triggered,
	read the settings for the mouse button swap from
	Windows and record it so left/right is as the user requested it.

***************************************/

void BURGER_API Burger::Mouse::ReadSystemMouseValues(void)
{
	// Get the initial keyboard delay to mimic the user's desired response
	// This is set in the Keyboard windows control panel

	m_bButtonSwap = GetSystemMetrics(SM_SWAPBUTTON)!=0;
}


#endif