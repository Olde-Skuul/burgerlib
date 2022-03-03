/***************************************

    Joypad/joystick Manager

    Windows specific version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brjoypad.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501		// Windows XP
#endif

#include "brgameapp.h"
#include "brglobals.h"
#include "brmemoryfunctions.h"
#include "brfixedpoint.h"
#include <windows.h>
#include <dinput.h>
#include <dinputd.h>
#include <Xinput.h>
#include <WbemCli.h>

// Needed for Code Warrior
#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL 0x80000000
#endif

#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD 30

// Hidden value
#ifndef XINPUT_GAMEPAD_BIGBUTTON
#define XINPUT_GAMEPAD_BIGBUTTON 0x800
#endif

//
// Used by the device enumerator to collect the information on devices
//

class JoystickFound {
public:
	GUID m_InstanceGUID;			// Located instance GUID
	GUID m_ProductGUID;				// Located product GUID
	Burger::String m_InstanceName;	// Name of the DirectInput instance
	Burger::String m_ProductName;	// Name of the DirectInput device
};

class JoystickCallBack {
public:
	uint_t m_uCount;				// Number of devices found
	uint_t m_bXInputFound;		// TRUE if XInput devices should be skipped
	JoystickFound m_Joysticks[Burger::Joypad::MAXJOYSTICKS];	// Joysticks enumerated
};

//
// Raw data structure to read joypad input from DirectInput
//

struct JoypadRawData_t {
	uint32_t m_uAxis[Burger::Joypad::MAXAXIS];		///< Axis data from the joypad (0-255)
	uint32_t m_uPOV[Burger::Joypad::MAXPOV];			///< POV data from the joypad (0-65535)
	uint8_t m_bButtons[Burger::Joypad::MAXBUTTONS];	///< Button data (FALSE/TRUE)
};

//
// Known gamepads
// TODO: Integrate automatic button mapping for known gamepads
//

#if 0
static const GUID IID_ValveStreamingGamepad = {0x11FF28DE,0x0000,0x0000,{0x00,0x00,0x50,0x49,0x44,0x56,0x49,0x44} };
static const GUID IID_X360WirelessGamepad = {0x02A1045E,0x0000,0x0000,{0x00,0x00,0x50,0x49,0x44,0x56,0x49,0x44} };
static const GUID IID_X360WiredGamepad = {0x028E045E,0x0000,0x0000,{0x00,0x00,0x50,0x49,0x44,0x56,0x49,0x44} };
static const GUID IID_GamestopWiredGamepad = {0x04010E6F,0x0000,0x0000,{0x00,0x00,0x50,0x49,0x44,0x56,0x49,0x44} };
static const GUID IID_SNESUSBGamepad = {0x028E045E,0x0000,0x0000,{0x00,0x00,0x50,0x49,0x44,0x56,0x49,0x44} };
static const GUID IID_PSXUSBGamepad = {0x11288631,0x0000,0x0000,{0x00,0x00,0x50,0x49,0x44,0x56,0x49,0x44} };
#endif

/***************************************

	This function is called for each and every game controller device
	It's used to obtain the GUID for all gaming devices.

***************************************/

static BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCEW* pdidInstance,VOID* pThis)
{
	// Get the return value pointer
	JoystickCallBack *pJoystick = static_cast<JoystickCallBack *>(pThis);
	BOOL bResult = DIENUM_CONTINUE;

	// If XInput is present, test if the game pad is controlled by XInput
	if (!pJoystick->m_bXInputFound || !Burger::IsDeviceXInput(&pdidInstance->guidProduct)) {

		// This is a non-XInput device. Add it to my DirectInput list

		uint_t uIndex = pJoystick->m_uCount;
		JoystickFound *pFound = &pJoystick->m_Joysticks[uIndex];

		// Get the GUIDs from the device
		Burger::MemoryCopy(&pFound->m_InstanceGUID,&pdidInstance->guidInstance,sizeof(GUID));
		Burger::MemoryCopy(&pFound->m_ProductGUID,&pdidInstance->guidProduct,sizeof(GUID));
		// Convert the names to UTF-8
		pFound->m_InstanceName.Set(static_cast<const uint16_t *>(static_cast<const void *>(pdidInstance->tszInstanceName)));
		pFound->m_ProductName.Set(static_cast<const uint16_t *>(static_cast<const void *>(pdidInstance->tszProductName)));
		++uIndex;
		pJoystick->m_uCount = uIndex;

		// Stop if the maximum number of devices is reached (To prevent overflow)
		if (uIndex>=Burger::Joypad::MAXJOYSTICKS) {
			bResult = DIENUM_STOP;
		}
	}
	return bResult;
}

//
// Used by the object enumerator to collect the information on the device
//

struct ObjectCallBack_t {
	IDirectInputDevice8W *m_pDevice;	// Pointer to the gamepad device driver
	uint_t m_uButtonCount;				// Number of buttons found
	uint_t m_uAxisCount;					// Number of axis found
	uint_t m_uPOVCount;					// Number of POV found
	DIDATAFORMAT Format;				// Data format to create for this game device
	DIOBJECTDATAFORMAT ObjectFormat[Burger::Joypad::MAXBUTTONS+Burger::Joypad::MAXAXIS+Burger::Joypad::MAXPOV];
	// GUID of each selected object
	GUID GUID[Burger::Joypad::MAXBUTTONS+Burger::Joypad::MAXAXIS+Burger::Joypad::MAXPOV];
};

/***************************************

	This function is called for each and every object in a game controller device
	It's used to count the objects and calibrate them to 0 through MAXAXISVALUE

***************************************/

static BOOL CALLBACK EnumObjectsCallback(const DIDEVICEOBJECTINSTANCEW *pObject,VOID* pThis)
{
	ObjectCallBack_t *pCallback = static_cast<ObjectCallBack_t *>(pThis);

	// Get the current object count
	uint_t uIndex = pCallback->Format.dwNumObjs;
	DIOBJECTDATAFORMAT *pCurrent = &pCallback->ObjectFormat[uIndex];
	uint_t bGotOne = FALSE;

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
			PropertyRange.diph.dwObj = pObject->dwType;	// Specify the enumerated axis
			PropertyRange.lMin = 0;
			PropertyRange.lMax = Burger::Joypad::MAXAXISVALUE;
			// Set the range for the axis
			if (pCallback->m_pDevice->SetProperty(DIPROP_RANGE,&PropertyRange.diph)<0) {
				return DIENUM_STOP;
			}
			pCurrent->dwOfs = BURGER_OFFSETOF(JoypadRawData_t,m_uAxis)+(pCallback->m_uAxisCount*sizeof(uint32_t));
			pCurrent->dwType = DIDFT_AXIS | DIDFT_ANYINSTANCE | DIDFT_OPTIONAL;
			pCurrent->dwFlags = DIDOI_ASPECTPOSITION;
			++pCallback->m_uAxisCount;
			bGotOne = TRUE;
		}
	} else
	if (pObject->dwType & DIDFT_POV) {
		if (pCallback->m_uPOVCount<Burger::Joypad::MAXPOV) {
			pCurrent->dwOfs = BURGER_OFFSETOF(JoypadRawData_t,m_uPOV)+(pCallback->m_uPOVCount*sizeof(uint32_t));
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

#endif

/***************************************

	Detect joysticks and init default information

***************************************/

Burger::Joypad::Joypad(GameApp *pAppInstance) :
	m_pAppInstance(pAppInstance),
	m_bDirectInputFound(FALSE),
	m_bXInputFound(FALSE),
	m_uDirectInputDevices(0),
	m_uDeviceCount(0)
{
	// Initialize everything
	MemoryClear(m_Data,sizeof(m_Data));
	MemoryClear(m_XInputGamepads,sizeof(m_XInputGamepads));
	
	//
	// Start allocating joypad devices here
	//
	JoypadData_t *pJoypadData = m_Data;

	// Determine if the XInput API is available.
	if (Windows::LoadFunctionIndex(Windows::CALL_XInputGetState)) {

		// XInput was found, define the 4 controllers that it will manage
		m_bXInputFound = TRUE;
		m_uDeviceCount = 4;
		uint_t uXInputCount = 4;
		do {
			pJoypadData->m_uButtonCount = 12;		// XInput manages 12 buttons
			pJoypadData->m_uPOVCount = 1;			// One POV controller
			pJoypadData->m_uAxisCount = 6;			// Two thumbsticks and two triggers
			pJoypadData->m_InstanceName.Set("Gamepad for Xbox 360 (Controller)");
			pJoypadData->m_ProductName.Set("Gamepad for Xbox 360 (Controller)");
			//pJoypadData->m_bConnected = FALSE;
			++pJoypadData;
		} while (--uXInputCount);
	}

	// Initialize the main direct input interface.

	IDirectInput8W* pDirectInput8W = Windows::GetDirectInput8Singleton();
	if (pDirectInput8W) {

		//
		// Enumerate the devices, after this function, the number of gaming devices
		// and their GUIDs are known
		//

		// Construct the buffer
		JoystickCallBack EnumeratedJoysticks;
		EnumeratedJoysticks.m_uCount = 0;
		EnumeratedJoysticks.m_bXInputFound = m_bXInputFound;

		HRESULT hResult = pDirectInput8W->EnumDevices(DI8DEVCLASS_GAMECTRL,EnumJoysticksCallback,&EnumeratedJoysticks,DIEDFL_ALLDEVICES);

		// Error? Or something found?
		if ((hResult>=0) && EnumeratedJoysticks.m_uCount) {
			// Initialize the direct input interface for the keyboard.
			uint_t i = 0;
			JoystickFound *pFound = EnumeratedJoysticks.m_Joysticks;
			do {
				IDirectInputDevice8W *pJoystickDeviceLocal = NULL;

				hResult = pDirectInput8W->CreateDevice(pFound->m_InstanceGUID,&pJoystickDeviceLocal,NULL);
				if (hResult>=0) {
					IDirectInputDevice8W *pJoystickDevice = pJoystickDeviceLocal;
					
					pJoypadData->m_pJoystickDevice = pJoystickDevice;
					MemoryCopy(&pJoypadData->m_InstanceGUID,&pFound->m_InstanceGUID,sizeof(GUID));
					MemoryCopy(&pJoypadData->m_ProductGUID,&pFound->m_ProductGUID,sizeof(GUID));
					pJoypadData->m_InstanceName = pFound->m_InstanceName;
					pJoypadData->m_ProductName = pFound->m_ProductName;

					hResult = pJoystickDevice->SetCooperativeLevel(pAppInstance->GetWindow(),DISCL_FOREGROUND | DISCL_EXCLUSIVE);
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
							}
						}
					}
					++pJoypadData;
				}
				++pFound;
			} while (++i<EnumeratedJoysticks.m_uCount);
			m_uDeviceCount += i;
			m_uDirectInputDevices = i;
			m_bDirectInputFound = TRUE;
		}
	}

	//
	// All input devices have been logged.
	//


	// Create the digital bounds for all devices
	uint_t j = 0;
	do {
		uint_t k = 0;
		do {
			SetDigital(j,k);
		} while (++k<MAXAXIS);
	} while (++j<MAXJOYSTICKS);

	// Install the background task if any devices were found

	if (m_bXInputFound) {
		Windows::XInputEnable(TRUE);
	}

	if (m_bXInputFound || m_bDirectInputFound) {
		pAppInstance->AddRoutine(Poll,NULL,this,RunQueue::PRIORITY_JOYPAD);
	}
}

/***************************************

	Shut down the joystick manager

***************************************/

Burger::Joypad::~Joypad()
{
	m_pAppInstance->RemoveRoutine(Poll,this);

	// Make sure the controllers are not rumbling
	XInputStopRumbleOnAllControllers();
	Windows::XInputEnable(FALSE);

	JoypadData_t *pJoypadData = m_Data;
	uint_t i = 0;
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

	Poll the game devices

***************************************/

Burger::RunQueue::eReturnCode BURGER_API Burger::Joypad::Poll(void *pData) BURGER_NOEXCEPT
{
	Joypad *pThis = static_cast<Joypad *>(pData);

	// Start data scanning here

	JoypadData_t *pJoypadData = pThis->m_Data;

	// Handle XInput devices first

	if (pThis->m_bXInputFound) {
		uint_t uWhich = 0;
		do {
			XINPUT_STATE State;
			// Test if this was an insertion or removal and report it
			// Get the old and new states

			uint_t bIsConnected = (Windows::XInputGetState(uWhich,&State) == ERROR_SUCCESS);
			uint_t bWasConnected = pJoypadData->m_bConnected;

			// Save off the states as to how they were processed
			pJoypadData->m_bConnected = static_cast<uint8_t>(bIsConnected);
			pJoypadData->m_bRemoved = static_cast<uint8_t>(bWasConnected & (bIsConnected^1));
			pJoypadData->m_bInserted = static_cast<uint8_t>((bWasConnected^1) & bIsConnected);

			if (bIsConnected) {
				uint32_t uButtons;
				uint_t uXBoxButtons = State.Gamepad.wButtons;

				// The code is the "right" way to do it, but it has
				// too many branches. The code that follows assumes
				// the equates never change, so it uses bit twiddling instead

#if (XINPUT_GAMEPAD_DPAD_LEFT==4) && (XINPUT_GAMEPAD_DPAD_RIGHT==8) && (XINPUT_GAMEPAD_DPAD_UP==1) && (XINPUT_GAMEPAD_DPAD_DOWN==2)
				uButtons = ((uXBoxButtons&(XINPUT_GAMEPAD_DPAD_LEFT|XINPUT_GAMEPAD_DPAD_RIGHT))<<6U);	// Bits 8-9
				uButtons += ((uXBoxButtons&(XINPUT_GAMEPAD_DPAD_UP|XINPUT_GAMEPAD_DPAD_DOWN))<<10U);		// Bits 10-11

				uButtons += (uXBoxButtons&(XINPUT_GAMEPAD_A|XINPUT_GAMEPAD_B|XINPUT_GAMEPAD_X|XINPUT_GAMEPAD_Y));	// Bits 12-15
				uButtons += ((uXBoxButtons&(XINPUT_GAMEPAD_LEFT_SHOULDER|XINPUT_GAMEPAD_RIGHT_SHOULDER))<<8U);		// Bits 16-17
				uButtons += ((uXBoxButtons&(XINPUT_GAMEPAD_BACK))<<13U);		// Bit 18
				uButtons += ((uXBoxButtons&(XINPUT_GAMEPAD_START))<<15U);		// Bit 19
				uButtons += ((uXBoxButtons&(XINPUT_GAMEPAD_LEFT_THUMB|XINPUT_GAMEPAD_RIGHT_THUMB))<<14U);			// Bits 20-21
				uButtons += ((uXBoxButtons&(XINPUT_GAMEPAD_BIGBUTTON))<<11);	// Bit 22
#else
				uButtons = 0;
				// The equates have changed! Use the safe code
				if (uXBoxButtons&XINPUT_GAMEPAD_DPAD_LEFT) {
					uButtons = POVLEFT;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_DPAD_RIGHT) {
					uButtons |= POVRIGHT;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_DPAD_UP) {
					uButtons |= POVUP;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_DPAD_DOWN) {
					uButtons |= POVDOWN;
				}

				if (uXBoxButtons&XINPUT_GAMEPAD_A) {
					uButtons |= BUTTON1;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_B) {
					uButtons |= BUTTON2;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_X) {
					uButtons |= BUTTON3;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_Y) {
					uButtons |= BUTTON4;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_LEFT_SHOULDER) {
					uButtons |= BUTTON5;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_RIGHT_SHOULDER) {
					uButtons |= BUTTON6;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_BACK) {
					uButtons |= BUTTON7;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_START) {
					uButtons |= BUTTON8;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_LEFT_THUMB) {
					uButtons |= BUTTON9;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_RIGHT_THUMB) {
					uButtons |= BUTTON10;
				}
				if (uXBoxButtons&XINPUT_GAMEPAD_BIGBUTTON) {
					uButtons |= BUTTON11;
				}
#endif

				// Convert analog directions to digital info
				const JoypadRange_t *pJoypadRange = pJoypadData->m_uAxisDigitalRanges;

				uint_t uTemp = static_cast<uint16_t>(State.Gamepad.sThumbLY)^0x8000U;
				pJoypadData->m_uAxis[0] = uTemp;
				if (uTemp<pJoypadRange[0].m_uMin) {		// Test X axis
					uButtons += AXIS1MIN;
				}
				if (uTemp>=pJoypadRange[0].m_uMax) {
					uButtons += AXIS1MAX;
				}

				// Test Y axis
				uTemp = static_cast<uint16_t>(State.Gamepad.sThumbLX)^0x8000U;
				pJoypadData->m_uAxis[1] = uTemp;
				if (uTemp<pJoypadRange[1].m_uMin) {
					uButtons += AXIS2MIN;
				}
				if (uTemp>=pJoypadRange[1].m_uMax) {
					uButtons += AXIS2MAX;
				}

				uTemp = static_cast<uint16_t>(State.Gamepad.sThumbRY)^0x8000U;
				pJoypadData->m_uAxis[2] = uTemp;
				if (uTemp<pJoypadRange[2].m_uMin) {		/* Test X axis */
					uButtons += AXIS3MIN;
				}
				if (uTemp>=pJoypadRange[2].m_uMax) {
					uButtons += AXIS3MAX;
				}
				// Test Y axis
				uTemp = static_cast<uint16_t>(State.Gamepad.sThumbRX)^0x8000U;
				pJoypadData->m_uAxis[3] = uTemp;
				if (uTemp<pJoypadRange[3].m_uMin) {
					uButtons += AXIS4MIN;
				}
				if (uTemp>=pJoypadRange[3].m_uMax) {
					uButtons += AXIS4MAX;
				}

				pJoypadData->m_uAxis[4] = (static_cast<uint32_t>(State.Gamepad.bLeftTrigger)<<8U)+State.Gamepad.bLeftTrigger;
				pJoypadData->m_uAxis[5] = (static_cast<uint32_t>(State.Gamepad.bRightTrigger)<<8U)+State.Gamepad.bRightTrigger;

				// Store the button states
				uint32_t uCache = (pJoypadData->m_uButtonState ^ uButtons)&uButtons;
				pJoypadData->m_uButtonStatePressed |= uCache;
				pJoypadData->m_uButtonState = uButtons;
			}
			++pJoypadData;
		} while (++uWhich<4);
	}

	//
	// Poll all of the DirectInput devices

	uint_t i = pThis->m_uDirectInputDevices;
	if (i) {
		do {
			// Only poll if it's initialized
			IDirectInputDevice8W *pJoystickDevice = pJoypadData->m_pJoystickDevice;
			if (pJoystickDevice) {

				// Ask DirectInput to read the data
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
						uint_t uButtonCount = pJoypadData->m_uButtonCount;
						if (uButtonCount) {
							uint32_t uMask = BUTTON1;
							uint32_t uButtons = 0;
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
								is exactly 9000 (90 degrees), we will say anything between
								15 degrees (forward plus some slop) and 165 degrees (backward
								minus some slop) means the hat is to the right.
							*/

							// Get the hi-hat value
							uint_t uTemp = State.m_uPOV[0];
							// Convert to digital information
							if (uTemp != INVALIDPOVVALUE) {
								if ((uTemp > POVANALOGLEFT + POV_SLOP && uTemp <= MAXPOVVALUE) || (uTemp < POVANALOGRIGHT - POV_SLOP)) { /* we will consider the hat to be up if it is anywhere forward of left or right */
									uButtons += POVUP;
								}
								if ((uTemp > POVANALOGFORWARD + POV_SLOP) && (uTemp < POVANALOGBACKWARD - POV_SLOP)) { /* we will consider the hat to be to the right up if it is anywhere to the right of up or down */
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
							uint_t j = 0;
							do {
								uTemp = static_cast<uint_t>(State.m_uAxis[j]);
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
							uint32_t uCache = (pJoypadData->m_uButtonState ^ uButtons)&uButtons;
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

/*! ************************************

	\brief Acquire DirectInput joystick devices

	When the application gains focus, call this function to acquire the joysticks

	Burgerlib does this call automatically.

	\windowsonly
	
***************************************/

void BURGER_API Burger::Joypad::AcquireDirectInput(void)
{
	uint_t i = m_uDeviceCount;
	if (i) {
		JoypadData_t *pJoypadData = m_Data;
		do {
			// Only acquire devices marked as DirectInput
			IDirectInputDevice8W *pJoystickDevice = pJoypadData->m_pJoystickDevice;
			if (pJoystickDevice) {
				pJoystickDevice->Acquire();
			}
			++pJoypadData;
		} while (--i);
	}
}

/*! ************************************

	\brief Unacquire DirectInput joystick devices

	When the application loses focus, call this function to release the joysticks

	Burgerlib does this call automatically.

	\windowsonly
	
***************************************/

void BURGER_API Burger::Joypad::UnacquireDirectInput(void)
{
	uint_t i = m_uDeviceCount;
	if (i) {
		JoypadData_t *pJoypadData = m_Data;
		do {
			// Only release devices marked as DirectInput
			IDirectInputDevice8W *pJoystickDevice = pJoypadData->m_pJoystickDevice;
			if (pJoystickDevice) {
				pJoystickDevice->Unacquire();
			}
			++pJoypadData;
		} while (--i);
	}
}

/*! ************************************

	\struct Burger::XInputGamePad_t
	\brief Persistent state of an XInput gamepad

	If the function XInputGetGamepadState() is called, it uses this structure
	to maintain the state of the gamepad. An application should allocate four of
	these structures and assign them to gamepad 0 through 3, calling
	the function XInputGetGamepadState() indexed to the associated structure
	to read and track the state of the gamepads. XInput, by design, only
	supports a maximum of 4 gamepads.

	Burgerlib's \ref Joypad class maintains it's own copies of this structure.
	This class is exposed for programmers who wish to directly access XInput
	for their own needs.

	\windowsonly
	
***************************************/

/*! ************************************

	\brief Turn off rumbling on XInput managed controllers

	If XInput is available, this function will clear the vibration state to
	off on all four controllers.

	This function is useful when the application is exiting or entering pause mode

	\windowsonly

	\return Zero if no error. Any other value means an error occurred, ERROR_CALL_NOT_IMPLEMENTED means XInput was not found
	
***************************************/

uint_t BURGER_API Burger::XInputStopRumbleOnAllControllers(void)
{
	// Set the vibration to off
	XINPUT_VIBRATION MyVibration;
	MyVibration.wLeftMotorSpeed = 0;
	MyVibration.wRightMotorSpeed = 0;

	uint32_t i = 0;
	uint_t uResult = 0;
	do {
		// Only abort if XInput is not present, otherwise, issue the
		// command to every device, regardless of connection state
		uint_t uTemp = Windows::XInputSetState(i,&MyVibration);
		if (uTemp==ERROR_CALL_NOT_IMPLEMENTED) {
			uResult = uTemp;
			break;
		}
	} while (++i<4);
	return uResult;
}

/*! ************************************

	\brief Read a controller using XInput

	If XInput is available, this function will read in the data from
	a gamepad controller.

	The XInputGamePad_t structure is assumed to be persistent and initialized by the
	application to zero before calling this function with the data. The function will
	maintain state using the contents of the XInputGamePad_t structure during
	successive calls. Calling this function with an XInputGamePad_t structure
	that had never been initially filled with zeros will yield undefined behavior.

	\windowsonly

	\param uWhich Which controller to read, 0-3 are valid
	\param pXInputGamePad Pointer to a structure that will receive the state.
	\param uDeadZoneType Type of post processing on the raw thumbstick data.
	\return Zero if no error. Any other value means an error occurred, ERROR_CALL_NOT_IMPLEMENTED means XInput was not found
	
***************************************/

uint_t BURGER_API Burger::XInputGetGamepadState(uint_t uWhich,XInputGamePad_t *pXInputGamePad,eXInputDeadZoneType uDeadZoneType)
{
	uint32_t uResult;
	if ((uWhich >= 4) || !pXInputGamePad) {
		uResult = static_cast<uint32_t>(E_FAIL);
	} else {
		// Read in the data from the game pad
		XINPUT_STATE GamepadState;
		uResult = Windows::XInputGetState(uWhich,&GamepadState);

		// Test if XInput is present
		if (uResult!=ERROR_CALL_NOT_IMPLEMENTED) {

			// Test if this was an insertion or removal and report it

			// Get the old and new states
			uint_t bWasConnected = pXInputGamePad->m_bConnected!=0;	// Force boolean for & below
			uint_t bIsConnected = (uResult == ERROR_SUCCESS);

			// Save off the states as to how they were processed
			pXInputGamePad->m_bConnected = static_cast<uint8_t>(bIsConnected);
			pXInputGamePad->m_bRemoved = static_cast<uint8_t>(bWasConnected & (bIsConnected^1));
			pXInputGamePad->m_bInserted = static_cast<uint8_t>((bWasConnected^1) & bIsConnected);

			// No error from here on out.
			uResult = 0;

			// Don't update rest of the state if not connected
			if (bIsConnected) {

				// Was this an insertion? If so, nuke the state
				// of the entire structure from orbit, just to be sure

				if (pXInputGamePad->m_bInserted) {
					MemoryClear(pXInputGamePad,sizeof(XInputGamePad_t));
					// Restore these two values because the MemoryClear()
					// erased them
					pXInputGamePad->m_bConnected = TRUE;
					pXInputGamePad->m_bInserted = TRUE;
				}

				// Load in the thumbstick values
				
				int32_t iThumbLX = GamepadState.Gamepad.sThumbLX;
				int32_t iThumbLY = GamepadState.Gamepad.sThumbLY;
				int32_t iThumbRX = GamepadState.Gamepad.sThumbRX;
				int32_t iThumbRY = GamepadState.Gamepad.sThumbRY;
				
				// Do any special processing for the thumb sticks

				// Handle dead zones on a per axis basis
				switch (uDeadZoneType) {
				case XINPUTDEADZONE_CARDINAL:
					// If the values are in the dead zone, set to zero
					if (Abs(iThumbLX) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
						iThumbLX = 0;
					}
					if (Abs(iThumbLY) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) {
						iThumbLY = 0;
					}
					// Note: The constant is different for the right thumbstick vs the left
					if (Abs(iThumbRX) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
						iThumbRX = 0;
					}
					if (Abs(iThumbRY) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) {
						iThumbRY = 0;
					}
					break;
				case XINPUTDEADZONE_CENTER:
					// Handle dead zones on a center region basis
					if ((Abs(iThumbLX) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
						(Abs(iThumbLY) < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {
						iThumbLX = 0;
						iThumbLY = 0;
					}

					if ((Abs(iThumbRX) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
						(Abs(iThumbRY) < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) {
						iThumbRX = 0;
						iThumbRY = 0;
					}
					break;
				default:
					break;
				}

				// Store the final result in the structure
				pXInputGamePad->m_iThumbLX = iThumbLX;
				pXInputGamePad->m_iThumbLY = iThumbLY;
				pXInputGamePad->m_iThumbRX = iThumbRX;
				pXInputGamePad->m_iThumbRY = iThumbRY;
				
				// Store the floating point versions
				pXInputGamePad->m_fThumbLX = static_cast<float>(iThumbLX) * (1.0f/32767.0f);
				pXInputGamePad->m_fThumbLY = static_cast<float>(iThumbLY) * (1.0f/32767.0f);
				pXInputGamePad->m_fThumbRX = static_cast<float>(iThumbRX) * (1.0f/32767.0f);
				pXInputGamePad->m_fThumbRY = static_cast<float>(iThumbRY) * (1.0f/32767.0f);

				// Process the pressed buttons
				uint32_t bButtons = GamepadState.Gamepad.wButtons;
				// Using the previous buttons, determine the ones "pressed"
				pXInputGamePad->m_uPressedButtons = (pXInputGamePad->m_uButtons ^ bButtons) & bButtons;
				pXInputGamePad->m_uButtons = bButtons;

				// Process the left trigger
				uint_t uTriggerValue = GamepadState.Gamepad.bLeftTrigger;
				uint_t bTriggerIsPressed = (uTriggerValue > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				uint_t bTriggerWasPressed = (pXInputGamePad->m_uLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				pXInputGamePad->m_uLeftTrigger = static_cast<uint8_t>(uTriggerValue);
				pXInputGamePad->m_bPressedLeftTrigger = static_cast<uint8_t>(bTriggerIsPressed & (bTriggerWasPressed^1));

				// Process the right trigger
				uTriggerValue = GamepadState.Gamepad.bRightTrigger;
				bTriggerIsPressed = (uTriggerValue > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				bTriggerWasPressed = (pXInputGamePad->m_uRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				pXInputGamePad->m_uRightTrigger = static_cast<uint8_t>(uTriggerValue);
				pXInputGamePad->m_bPressedRightTrigger = static_cast<uint8_t>(bTriggerIsPressed & (bTriggerWasPressed^1));
			}
		} else {
			// Zap the buffer if there is no XInput
			MemoryClear(pXInputGamePad,sizeof(XInputGamePad_t));
		}
	}
	return uResult;
}

/*! ************************************

	\brief Test if a device GUID belongs to an XInput device

	To allow DirectInput and XInput to work side-by-side, all devices
	that are supported by XInput should be ignored when enumerated for
	DirectInput control.

	Given a GUID of a device, test the Plug and Play (PNP) device ID
	if it contains the substring "IG_", because if it does, it's
	an XInput supported device.

	\note This code is compatible with Windows XP, however it does
	follow Microsoft's recommendation of using an IWbemLocator
	if the code detects it's running on Windows Vista or later.

	\windowsonly

	\return \ref FALSE if the device is not supported by XInput, \ref TRUE if so.
	
***************************************/

uint_t BURGER_API Burger::IsDeviceXInput(const GUID *pGuid)
{
	// Assume it's not an XInput device
	uint_t bResult = FALSE;

	// Microsoft recommends using WbemLocator for finding devices that are using XInput,
	// however, this requires Vista or higher

	if (Windows::IsVistaOrGreater()) {
		// Start up CoInitialize() to allow creating instances
		uint_t bCleanupCOM = (CoInitialize(NULL)>=0);

		// Create WMI
		IWbemLocator *pIWbemLocator = NULL;
		if ((CoCreateInstance(CLSID_WbemLocator,NULL,CLSCTX_INPROC_SERVER,IID_IWbemLocator,(LPVOID*)&pIWbemLocator)>=0) && 
			pIWbemLocator) {

			// Connect to WMI 
			IWbemServices *pIWbemServices = NULL;
			if ((pIWbemLocator->ConnectServer((const BSTR)(L"\\\\.\\root\\cimv2"),NULL,NULL,NULL,0,NULL,NULL,&pIWbemServices)>=0) &&
				pIWbemServices) {

				// Switch security level to IMPERSONATE. 
				CoSetProxyBlanket(pIWbemServices,RPC_C_AUTHN_WINNT,RPC_C_AUTHZ_NONE,NULL,
					RPC_C_AUTHN_LEVEL_CALL,RPC_C_IMP_LEVEL_IMPERSONATE,NULL,EOAC_NONE);

				// Get the PNPEntity list
				IEnumWbemClassObject *pEnumDevices = NULL;
				if ((pIWbemServices->CreateInstanceEnum((const BSTR)(L"Win32_PNPEntity"),0,NULL,&pEnumDevices)>=0) &&
					pEnumDevices) {
				
					// Array of devices
					IWbemClassObject *DevicePointers[20];
					ULONG uDevice;
				
					// Loop over all devices
					do {
 						// Get 20 at a time
						ULONG uReturned;
						MemoryClear(DevicePointers,sizeof(DevicePointers));
						if ((pEnumDevices->Next(10000,BURGER_ARRAYSIZE(DevicePointers),DevicePointers,&uReturned)<0) ||
							!uReturned) {
							// Error, or ran out of devices
							break;
						}

						// Iterate the group of devices obtained
						uDevice = 0;
						do {
							// For each device, get its device ID
							VARIANT MyVariant;
							if ((DevicePointers[uDevice]->Get((const BSTR)(L"DeviceID"),0,&MyVariant,NULL,NULL)>=0) &&
								(MyVariant.vt == VT_BSTR)) {

								const uint16_t *pVariantName = reinterpret_cast<const uint16_t *>(MyVariant.bstrVal);
								if (pVariantName) {
									// Check if the device ID contains "IG_". If it does, then it's an XInput device
									// This information can not be found from DirectInput

									if (StringString(pVariantName,(const uint16_t *)L"IG_")) {

										// If it does, then get the VID/PID from var.bstrVal
										uint32_t uFoundVendorID = 0;
										const uint16_t *pFound = StringString(pVariantName,(const uint16_t *)L"VID_");
										if (pFound) {
											uFoundVendorID = AsciiHexToInteger(pFound+4,4);
										}
										uint32_t uFoundPeripheralID = 0;
										pFound = StringString(pVariantName,(const uint16_t *)L"PID_" );
										if (pFound) {
											uFoundPeripheralID = AsciiHexToInteger(pFound+4,4);
										}

										// Check the Peripheral ID to the one in the GUID
										uint32_t uVIDPID = (uFoundVendorID&0xFFFF) + (uFoundPeripheralID<<16);
										if (uVIDPID == pGuid->Data1) {
											// Paydirt!
											bResult = TRUE;
											break;
										}
									}
								}
							}
							if (DevicePointers[uDevice]) {
								DevicePointers[uDevice]->Release();
								DevicePointers[uDevice] = NULL;
							}
						} while (++uDevice<uReturned);
					} while (!bResult);

					//
					// Clean up any stragglers
					//

					uDevice = 0;
					do {
						if (DevicePointers[uDevice]) {
							DevicePointers[uDevice]->Release();
							//DevicePointers[uDevice] = NULL;
						}
					} while (++uDevice<BURGER_ARRAYSIZE(DevicePointers));
				}

				//
				// Clean up the device list
				//
				if (pEnumDevices) {
					pEnumDevices->Release();
					//pEnumDevices = NULL;
				}
			}

			//
			// Clean up the services
			//

			if (pIWbemServices) {
				pIWbemServices->Release();
				//pIWbemServices = NULL;
			}
		}

		//
		// Clean up the locater
		//

		if (pIWbemLocator) {
			pIWbemLocator->Release();
			//pIWbemLocator = NULL;
		}

		//
		// If CoInitialize() was successful, release it
		//
		if (bCleanupCOM) {
			CoUninitialize();
		}
	} else {

		//
		// This is compatible with Windows XP
		// Note to self, don't EVER trust sample code from a Microsoft web site. They
		// will LIE to you about whether a piece of code (See above) would work
		// on older versions of Windows.
		//

		UINT uDeviceCount;

		// Get the device count from the raw device list

		if (!GetRawInputDeviceList(NULL,&uDeviceCount,sizeof(RAWINPUTDEVICELIST)) && uDeviceCount) {

			// Make a buffer for the list
			RAWINPUTDEVICELIST *pList = static_cast<RAWINPUTDEVICELIST *>(Alloc(sizeof(RAWINPUTDEVICELIST)*uDeviceCount));
			if (pList) {

				// Read in the list and continue if the list has anything
				if (GetRawInputDeviceList(pList,&uDeviceCount,sizeof(RAWINPUTDEVICELIST))!=BURGER_MAXUINT) {

					// Iterate over the list

					uint16_t DeviceName[256];
					RID_DEVICE_INFO RawDeviceInfo;
					RawDeviceInfo.cbSize = sizeof(RawDeviceInfo);

					const RAWINPUTDEVICELIST *pWorkList = pList;
					uint_t i = uDeviceCount;
					do {

						// Only test HID devices
						if (pWorkList->dwType == RIM_TYPEHID) {

							// Get the device information
							UINT uSize = sizeof(RawDeviceInfo);
							if (GetRawInputDeviceInfoW(pWorkList->hDevice,RIDI_DEVICEINFO,&RawDeviceInfo,&uSize)!=BURGER_MAXUINT) {

								// Is this device a match for the GUID?
								if ((RawDeviceInfo.hid.dwVendorId == (pGuid->Data1&0xFFFF)) &&
									(RawDeviceInfo.hid.dwProductId == (pGuid->Data1>>16))) {

									// Now comes the moment of truth. Get the driver's name
									uSize = BURGER_ARRAYSIZE(DeviceName);
									if (GetRawInputDeviceInfoW(pWorkList->hDevice,RIDI_DEVICENAME,DeviceName,&uSize)!=BURGER_MAXUINT) {
										// XInput device?
										if (StringString(DeviceName,(const uint16_t *)L"IG_")) {
											// Gotcha!
											bResult = TRUE;
										}
									}
									// Exit the loop now, because the device was found.
									break;
								}
							}
						}
						++pWorkList;
					} while (--i);
				}
				Free(pList);
			}
		}
	}

	// Return TRUE or FALSE if found

	return bResult;
}
#endif
