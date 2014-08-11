/***************************************

	Mouse Manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmouse.h"

#if defined(BURGER_WINDOWS)
#define DIRECTINPUT_VERSION 0x0800
#include "brwindowsapp.h"
#include "brglobals.h"
#include <dinput.h>

// Number of mouse events in the cache
// Has to be high since the player could be swiping the mouse and if
// it's a high resolution mouse, the events could stack up quickly

#define DIRECTINPUT_MOUSEBUFFERSIZE 128

//
// Initialize the mouse class
// 

Burger::Mouse::Mouse(GameApp *pAppInstance) :
	m_pAppInstance(pAppInstance),
	m_uX(0),
	m_uY(0),
	m_uBoundsX(640),
	m_uBoundsY(480),
	m_iDeltaX(0),
	m_iDeltaY(0),
	m_iMouseWheel(0),
	m_uButtons(0),
	m_uPressedButtons(0),
	m_pMouseDevice(NULL)
{
	pAppInstance->SetMouse(this);
	IDirectInput8W* pDirectInput8W;

	// First step, obtain DirectInput

	if (!Burger::Globals::DirectInput8Create(&pDirectInput8W)) {

		// Create a system mouse device (Merges all mice)

		IDirectInputDevice8W *pMouseDeviceLocal = NULL;
		HRESULT hResult = pDirectInput8W->CreateDevice(GUID_SysMouse,&pMouseDeviceLocal,NULL);
		if (hResult>=0) {
			// Set the default data format of x,y,wheel and 8 buttons
			IDirectInputDevice8W *pMouseDevice = pMouseDeviceLocal;
			hResult = pMouseDevice->SetDataFormat(&c_dfDIMouse2);
			if (hResult>=0) {

				// Play nice with the system and let others use the keyboard
				// Disable the windows key if the application is running in the foreground

				hResult = pMouseDevice->SetCooperativeLevel(static_cast<WindowsApp *>(pAppInstance)->GetWindow(),DISCL_FOREGROUND | DISCL_EXCLUSIVE);
				if (hResult>=0) {
					DIPROPDWORD MouseProperties;
					
					MouseProperties.diph.dwSize = sizeof(DIPROPDWORD);
					MouseProperties.diph.dwHeaderSize = sizeof(DIPROPHEADER);
					MouseProperties.diph.dwObj = 0;
					MouseProperties.diph.dwHow = DIPH_DEVICE;
					MouseProperties.dwData = DIRECTINPUT_MOUSEBUFFERSIZE;
					hResult = pMouseDevice->SetProperty(DIPROP_BUFFERSIZE,&MouseProperties.diph);
					if (hResult>=0) {
						// Obtain the default mouse position
						Acquire();
						pAppInstance->AddRoutine(Poll,this);
					}
				}
			}
			if (hResult<0) {
				// Uh oh...
				pMouseDevice->Unacquire();
				pMouseDevice->Release();
			} else {
				m_pMouseDevice = pMouseDevice;
			}
		}
	}
}

//
// Shut down the mouse class
//

Burger::Mouse::~Mouse()
{
	m_pAppInstance->SetMouse(NULL);
	m_pAppInstance->RemoveRoutine(Poll,this);
	// Was a device allocated?
	IDirectInputDevice8W *pMouseDevice = m_pMouseDevice;
	if (pMouseDevice) {
		// Release it!
		pMouseDevice->Unacquire();
		pMouseDevice->Release();
		m_pMouseDevice = NULL;
	}
}

//
// Poll DirectInput for mouse events
//

Burger::RunQueue::eReturnCode BURGER_API Burger::Mouse::Poll(void *pData)
{
	Mouse *pThis = static_cast<Mouse *>(pData);
	// Poll the mouse device for events
	IDirectInputDevice8W *pMouseDevice = pThis->m_pMouseDevice;
	if (pMouseDevice) {

		// Buffer to receive the keyboard events
		DIDEVICEOBJECTDATA MouseData[DIRECTINPUT_MOUSEBUFFERSIZE];
		DWORD uCount = DIRECTINPUT_MOUSEBUFFERSIZE;

		// Let's get the data from the keyboard device
		HRESULT hResult = pMouseDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),MouseData,&uCount,0);
		if (hResult<0) {
			// Try getting the keyboard again
			if ((hResult == DIERR_INPUTLOST) || (hResult == DIERR_NOTACQUIRED)) {
				hResult = pMouseDevice->Acquire();
				if (hResult>=0) {
					// The keyboard was reacquired, pull the data
					uCount = DIRECTINPUT_MOUSEBUFFERSIZE;
					hResult = pMouseDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),MouseData,&uCount,0);
				}
			}
		}

		// Was there any data read?
		if (hResult>=0) {
			Word i = uCount;
			if (i) {
				const DIDEVICEOBJECTDATA *pObject = MouseData;
				do {
					DWORD uDataType = pObject->dwOfs;
					DWORD uData = pObject->dwData;
					if (uDataType==DIMOFS_X) {
						int iOffsetX = static_cast<int>(uData);
						pThis->m_iDeltaX += iOffsetX;
						iOffsetX += pThis->m_uX;
						if (iOffsetX<0) {
							iOffsetX = 0;
						}
						if (static_cast<Word>(iOffsetX)>=pThis->m_uBoundsX) {
							iOffsetX = static_cast<int>(pThis->m_uBoundsX)-1;
						}
						pThis->m_uX = static_cast<Word>(iOffsetX);

					} else if (uDataType==DIMOFS_Y) {
						int iOffsetY = static_cast<int>(uData);
						pThis->m_iDeltaY += iOffsetY;
						iOffsetY += pThis->m_uY;
						if (iOffsetY<0) {
							iOffsetY = 0;
						}
						if (static_cast<Word>(iOffsetY)>=pThis->m_uBoundsY) {
							iOffsetY = static_cast<int>(pThis->m_uBoundsY)-1;
						}
						pThis->m_uY = static_cast<Word>(iOffsetY);

					} else if (uDataType==DIMOFS_Z) {
						pThis->m_iMouseWheel += static_cast<int>(uData);
					} else if ((uDataType>=DIMOFS_BUTTON0) && (uDataType<=DIMOFS_BUTTON7)) {
						uDataType-=DIMOFS_BUTTON0;
						Word uMask = 1U<<uDataType;
						Word uButtons = pThis->m_uButtons;
						if (uData&0x80) {
							uButtons |= uMask;
							pThis->m_uPressedButtons |= uMask;
						} else {
							uButtons &= (~uMask);
						}
						pThis->m_uButtons = uButtons;
					}
					++pObject;
				} while (--i);
			}
		}
	}
	return RunQueue::OKAY;
}

Word Burger::Mouse::IsPresent()
{
	return TRUE;
}

Word Burger::Mouse::ReadButtons(void)
{
	return m_uButtons;
}


void Burger::Mouse::Acquire(void)
{
	// Obtain the default mouse position
	POINT MousePos;
	GetCursorPos(&MousePos);
	ScreenToClient(static_cast<WindowsApp *>(m_pAppInstance)->GetWindow(),&MousePos);
	m_uX = MousePos.x;
	m_uY = MousePos.y;
	IDirectInputDevice8W *pMouseDevice = m_pMouseDevice;
	if (pMouseDevice) {
		pMouseDevice->Acquire();
	}
}

void Burger::Mouse::Unacquire(void)
{
	IDirectInputDevice8W *pMouseDevice = m_pMouseDevice;
	if (pMouseDevice) {
		pMouseDevice->Unacquire();
		// Obtain the default mouse position
		POINT MousePos;
		MousePos.x = m_uX;
		MousePos.y = m_uY;
		ClientToScreen(static_cast<WindowsApp *>(m_pAppInstance)->GetWindow(),&MousePos);
		SetCursorPos(MousePos.x,MousePos.y);
	}
}


#endif