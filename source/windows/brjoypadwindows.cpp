/***************************************

	Joypad/joystick Manager

	Windows specific version
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brjoypad.h"

#if defined(BURGER_WINDOWS)
#define DIRECTINPUT_VERSION 0x0800
#include "brwindowsapp.h"
#include "brglobals.h"
#include "brstringfunctions.h"
#include <windows.h>
#include <dinput.h>
#include <dinputd.h>

// Needed for Code Warrior
#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL 0x80000000
#endif

//
// Used by the device enumerator to collect the information on devices
//

struct JoystickCallBack_t {
	GUID *m_pGameDeviceGUID;	// Pointer to where to store the device GUIDs
	Word m_uCount;				// Number of devices found
};

//
// Raw data structure to read joypad input from DirectInput
//

struct JoypadRawData_t {
	Word32 m_uAxis[Burger::Joypad::MAXAXIS];		///< Axis data from the joypad (0-255)
	Word32 m_uPOV[Burger::Joypad::MAXPOV];			///< POV data from the joypad (0-65535)
	Word8 m_bButtons[Burger::Joypad::MAXBUTTONS];	///< Button data (FALSE/TRUE)
};

/***************************************

	This function is called for each and every game controller device
	It's used to obtain the GUID for all gaming devices.

***************************************/

static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCEW* pdidInstance,VOID* pThis)
{
	// Get the return value pointer
	JoystickCallBack_t *pJoystick = static_cast<JoystickCallBack_t *>(pThis);
	Word uIndex = pJoystick->m_uCount;
	// Get the GUID
	Burger::MemoryCopy(&pJoystick->m_pGameDeviceGUID[uIndex],&pdidInstance->guidInstance,sizeof(GUID));
	++uIndex;
	pJoystick->m_uCount = uIndex;
	// Stop if the maximum number of devices is reached
	if (uIndex<Burger::Joypad::MAXJOYSTICKS) {
		return DIENUM_CONTINUE;
	}
	return DIENUM_STOP;
}

//
// Used by the object enumerator to collect the information on the device
//

struct ObjectCallBack_t {
	IDirectInputDevice8W *m_pDevice;	// Pointer to the gamepad device driver
	Word m_uButtonCount;				// Number of buttons found
	Word m_uAxisCount;					// Number of axis found
	Word m_uPOVCount;					// Number of POV found
	DIDATAFORMAT Format;				// Data format to create for this game device
	DIOBJECTDATAFORMAT ObjectFormat[Burger::Joypad::MAXBUTTONS+Burger::Joypad::MAXAXIS+Burger::Joypad::MAXPOV];
	// GUID of each selected object
	GUID GUID[Burger::Joypad::MAXBUTTONS+Burger::Joypad::MAXAXIS+Burger::Joypad::MAXPOV];
};

/***************************************

	This function is called for each and every object in a game controller device
	It's used to count the objects and calibrate them to 0-255

***************************************/

static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCEW *pObject,VOID* pThis)
{
	ObjectCallBack_t *pCallback = static_cast<ObjectCallBack_t *>(pThis);

	// Get the current object count
	Word uIndex = pCallback->Format.dwNumObjs;
	DIOBJECTDATAFORMAT *pCurrent = &pCallback->ObjectFormat[uIndex];
	Word bGotOne = FALSE;

	// Count the buttons on this device
	if (pObject->dwType & DIDFT_BUTTON) {
		if (pCallback->m_uButtonCount<Burger::Joypad::MAXBUTTONS) {
			pCurrent->dwOfs = BURGER_OFFSETOF(JoypadRawData_t,m_bButtons)+pCallback->m_uButtonCount;
			pCurrent->dwType = DIDFT_BUTTON | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL;
			pCurrent->dwFlags = 0;
			++pCallback->m_uButtonCount;
			bGotOne = TRUE;
		}
	} else
	// Convert the axes to use a 0-255 range value
	// enumerated axis in order to scale min/max values.
	if (pObject->dwType & DIDFT_AXIS) {
		if (pCallback->m_uAxisCount<Burger::Joypad::MAXAXIS) {
			DIPROPRANGE PropertyRange;
			PropertyRange.diph.dwSize = sizeof(DIPROPRANGE);
			PropertyRange.diph.dwHeaderSize = sizeof(DIPROPHEADER);
			PropertyRange.diph.dwHow = DIPH_BYID;
			PropertyRange.diph.dwObj = pObject->dwType; // Specify the enumerated axis
			PropertyRange.lMin = 0;
			PropertyRange.lMax = 255;
			// Set the range for the axis
			if (pCallback->m_pDevice->SetProperty(DIPROP_RANGE,&PropertyRange.diph)<0) {
				return DIENUM_STOP;
			}
			pCurrent->dwOfs = BURGER_OFFSETOF(JoypadRawData_t,m_uAxis)+(pCallback->m_uAxisCount*sizeof(Word32));
			pCurrent->dwType = DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL;
			pCurrent->dwFlags = DIDOI_ASPECTPOSITION;
			++pCallback->m_uAxisCount;
			bGotOne = TRUE;
		}
	} else
	if (pObject->dwType & DIDFT_POV) {
		if (pCallback->m_uPOVCount<Burger::Joypad::MAXPOV) {
			pCurrent->dwOfs = BURGER_OFFSETOF(JoypadRawData_t,m_uPOV)+(pCallback->m_uPOVCount*sizeof(Word32));
			pCurrent->dwType = DIDFT_POV | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL;
			pCurrent->dwFlags = DIDOI_ASPECTPOSITION;
			++pCallback->m_uPOVCount;
			bGotOne = TRUE;
		}
	}
	// Was there a valid input object found?
	if (bGotOne) {
		GUID *pGUID = &pCallback->GUID[uIndex];
		Burger::MemoryCopy(pGUID,&pObject->guidType,sizeof(GUID));
		pCurrent->pguid = pGUID;
		pCallback->Format.dwNumObjs = uIndex+1;
		if (uIndex>=(Burger::Joypad::MAXBUTTONS+Burger::Joypad::MAXAXIS+Burger::Joypad::MAXPOV)) {
			return DIENUM_STOP;
		}
	}
	return DIENUM_CONTINUE;
}

/***************************************

	Detect joysticks and init default information

***************************************/

Burger::Joypad::Joypad(Burger::GameApp *pAppInstance)
{
	pAppInstance->SetJoypad(this);
	// Initialize everything
	Word i = 0;
	JoypadData_t *pJoypadData = m_Data;
	do {
		pJoypadData->m_pJoystickDevice = NULL;
		pJoypadData->m_uButtonCount = 0;
		pJoypadData->m_uPOVCount = 0;
		pJoypadData->m_uAxisCount = 0;
		++pJoypadData;
	} while (++i<MAXJOYSTICKS);
	m_pAppInstance = pAppInstance;
	m_uDeviceCount = 0;

	// Initialize the main direct input interface.
	IDirectInput8W* pDirectInput8W;
	if (!Burger::Globals::DirectInput8Create(&pDirectInput8W)) {

		// Enumerate the devices, after this function, the number of gaming devices
		// and their GUIDs are known
		JoystickCallBack_t Joy;
		Joy.m_pGameDeviceGUID = m_GamePadGUID;
		Joy.m_uCount = 0;
		HRESULT hResult = pDirectInput8W->EnumDevices(DI8DEVCLASS_GAMECTRL,EnumJoysticksCallback,&Joy,DIEDFL_ALLDEVICES);
		if (hResult>=0 && Joy.m_uCount) {
			// Initialize the direct input interface for the keyboard.
			i = 0;
			JoypadData_t *pJoypadData = m_Data;
			do {
				IDirectInputDevice8W *pJoystickDeviceLocal = NULL;
				hResult = pDirectInput8W->CreateDevice(m_GamePadGUID[i],&pJoystickDeviceLocal,NULL);
				if (hResult>=0) {
					IDirectInputDevice8W *pJoystickDevice = pJoystickDeviceLocal;
					pJoypadData->m_pJoystickDevice = pJoystickDevice;
					hResult = pJoystickDevice->SetCooperativeLevel(static_cast<WindowsApp *>(pAppInstance)->GetWindow(),DISCL_FOREGROUND | DISCL_EXCLUSIVE);
					if (hResult>=0) {
						//
						// Begin the scan for the input objects found in this device
						//
						ObjectCallBack_t Object;
						Object.m_pDevice = pJoystickDevice;
						Object.m_uButtonCount = 0;
						Object.m_uPOVCount = 0;
						Object.m_uAxisCount = 0;
						Object.Format.dwSize = sizeof(Object.Format);
						Object.Format.dwObjSize = sizeof(Object.ObjectFormat[0]);
						Object.Format.dwFlags = DIDF_ABSAXIS;
						Object.Format.dwDataSize = sizeof(JoypadRawData_t);
						Object.Format.dwNumObjs = 0;		// No objects yet!
						Object.Format.rgodf = Object.ObjectFormat;
						hResult = pJoystickDevice->EnumObjects(EnumObjectsCallback,&Object,DIDFT_ALL);
						if (hResult>=0) {
							hResult = pJoystickDevice->SetDataFormat(&Object.Format);
							if (hResult>=0) {
								// Accept this device!
								pJoypadData->m_uButtonCount=Object.m_uButtonCount;
								pJoypadData->m_uPOVCount=Object.m_uPOVCount;
								pJoypadData->m_uAxisCount=Object.m_uAxisCount;
								pAppInstance->AddRoutine(Poll,this);
							}
						}
					}
				}
				++pJoypadData;
			} while (++i<Joy.m_uCount);
			m_uDeviceCount = i;
			Word j = 0;
			do {
				Word k = 0;
				do {
					SetDigital(j,k);		// Create the digital bounds
				} while (++k<MAXAXIS);
			} while (++j<i);
		}
	}
}

/***************************************

	Shut down the joystick manager

***************************************/

Burger::Joypad::~Joypad()
{
	m_pAppInstance->SetJoypad(NULL);
	m_pAppInstance->RemoveRoutine(Poll,this);
	JoypadData_t *pJoypadData = m_Data;
	Word i = 0;
	do {
		IDirectInputDevice8W *pJoystickDevice = pJoypadData->m_pJoystickDevice;
		if (pJoystickDevice) {
			pJoystickDevice->Unacquire();
			pJoystickDevice->Release();
			pJoypadData->m_pJoystickDevice = NULL;
		}
		++pJoypadData;
	} while (++i<MAXJOYSTICKS);
	m_uDeviceCount = 0;
}

/***************************************

	Poll the game device

***************************************/

Burger::RunQueue::eReturnCode BURGER_API Burger::Joypad::Poll(void *pData)
{
	Joypad *pThis = static_cast<Joypad *>(pData);
	Word i = pThis->m_uDeviceCount;
	if (i) {
		JoypadData_t *pJoypadData = pThis->m_Data;
		do {
			IDirectInputDevice8W *pJoystickDevice = pJoypadData->m_pJoystickDevice;
			if (pJoystickDevice) {
				HRESULT hResult = pJoystickDevice->Poll();
				// If it failed, it's possible that it was not acquired
				if (hResult<0) {
					hResult = pJoystickDevice->Acquire();
					while (hResult == DIERR_INPUTLOST) {
						hResult = pJoystickDevice->Acquire();
					}
				} else {
					JoypadRawData_t State;
					hResult = pJoystickDevice->GetDeviceState(sizeof(State),&State);
					if (hResult>=0) {
						Word uButtonCount = pJoypadData->m_uButtonCount;
						if (uButtonCount) {
							Word32 uMask = BUTTON1;
							Word32 uButtons = 0;
							const BYTE *pButton = State.m_bButtons;
							do {
								if (pButton[0]) {
									uButtons |= uMask;
								}
								++pButton;
								uMask<<=1;
							} while (--uButtonCount);

							/*	Window returns the position of the hi-hat (POV) as
								-1 (JOY_POVCENTERED) or a value between 0 (0 degrees)
								and 36000 (360 degrees). 0 is forward, 9000 is right, 18000
								is backward, and 27000 is left. Rather than saying the hat
								is pressed to the right only when the Windows POV value
								is exactly 9000 (90 degreese), we will say anything between
								15 degrees (forward plus some slop) and 165 degrees (backward
								minus some slop) means the hat is to the right.
							*/

							// Get the hi-hat value
							Word uTemp = State.m_uPOV[0];
							// Convert to digital information
							if (uTemp != INVALIDPOVVALUE) {
								if ((uTemp > POVANALOGLEFT + POV_SLOP && uTemp <= MAXPOVVALUE) || (uTemp < POVANALOGRIGHT - POV_SLOP)) { /* we will consider the hat to be up if it is anywhere forward of left or right */
									uButtons += POVUP;
								}
								if ((uTemp > POVANALOGFORWARD + POV_SLOP) && (uTemp < POVANALOGBACKWARD - POV_SLOP)) { /* we will consider the hat to be to the ritgh up if it is anywhere to the right opf up or down */
									uButtons += POVRIGHT;
								}
								if ((uTemp > POVANALOGRIGHT + POV_SLOP) && (uTemp < POVANALOGLEFT - POV_SLOP)) {
									uButtons += POVDOWN;
								}
								if ((uTemp > POVANALOGBACKWARD + POV_SLOP) && (uTemp < MAXPOVVALUE - POV_SLOP)) {
									uButtons += POVLEFT;
								}
							}
							// Convert analog directions to digital info
							const JoypadRange_t *pJoypadRange = pJoypadData->m_uAxisDigitalRanges;
							Word j = 0;
							do {
								uTemp = static_cast<Word>(State.m_uAxis[j]);
								pJoypadData->m_uAxis[j] = uTemp;
								if (j<4) {
									if (uTemp<pJoypadRange[j].m_uMin) {		/* Test X axis */
										uButtons += (AXIS1MIN<<(j*2));
									}
									if (uTemp>=pJoypadRange[j].m_uMax) {
										uButtons += (AXIS1MAX<<(j*2));
									}
								}
							} while (++j<MAXAXIS);
	
							// Store the button states
							Word32 uCache = (pJoypadData->m_uButtonState ^ uButtons)&uButtons;
							pJoypadData->m_uButtonStatePressed |= uCache;
							pJoypadData->m_uButtonState = uButtons;
						}
					}
				}
			}
			++pJoypadData;
		} while (--i);
	}
	return RunQueue::OKAY;
}

void Burger::Joypad::Acquire(void)
{
	Word i = m_uDeviceCount;
	if (i) {
		JoypadData_t *pJoypadData = m_Data;
		do {
			IDirectInputDevice8W *pJoystickDevice = pJoypadData->m_pJoystickDevice;
			if (pJoystickDevice) {
				pJoystickDevice->Acquire();
			}
			++pJoypadData;
		} while (--i);
	}
}

void Burger::Joypad::Unacquire(void)
{
	Word i = m_uDeviceCount;
	if (i) {
		JoypadData_t *pJoypadData = m_Data;
		do {
			IDirectInputDevice8W *pJoystickDevice = pJoypadData->m_pJoystickDevice;
			if (pJoystickDevice) {
				pJoystickDevice->Unacquire();
			}
			++pJoypadData;
		} while (--i);
	}
}

#endif
