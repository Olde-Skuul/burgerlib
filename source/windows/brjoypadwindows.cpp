/***************************************

	Joypad/joystick Manager

	Windows specific version
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brjoypad.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)
#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif
#include "brgameapp.h"
#include "brglobals.h"
#include "brstringfunctions.h"
#include "brfixedpoint.h"
#include <windows.h>
#include <dinput.h>
#include <dinputd.h>
#include <Xinput.h>

// Needed for Code Warrior
#ifndef DIDFT_OPTIONAL
#define DIDFT_OPTIONAL 0x80000000
#endif

#define XINPUT_GAMEPAD_TRIGGER_THRESHOLD 30

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

#endif

/***************************************

	Detect joysticks and init default information

***************************************/

Burger::Joypad::Joypad(Burger::GameApp *pAppInstance)
{
	pAppInstance->SetJoypad(this);
	// Initialize everything
	MemoryClear(m_Data,sizeof(m_Data));
	m_pAppInstance = pAppInstance;
	m_uDeviceCount = 0;

	// Initialize the main direct input interface.
	IDirectInput8W* pDirectInput8W = Globals::GetDirectInput8Singleton();
	if (pDirectInput8W) {

		// Enumerate the devices, after this function, the number of gaming devices
		// and their GUIDs are known
		JoystickCallBack_t Joy;
		Joy.m_pGameDeviceGUID = m_GamePadGUID;
		Joy.m_uCount = 0;
		HRESULT hResult = pDirectInput8W->EnumDevices(DI8DEVCLASS_GAMECTRL,EnumJoysticksCallback,&Joy,DIEDFL_ALLDEVICES);
		if (hResult>=0 && Joy.m_uCount) {
			// Initialize the direct input interface for the keyboard.
			Word i = 0;
			JoypadData_t *pJoypadData = m_Data;
			do {
				IDirectInputDevice8W *pJoystickDeviceLocal = NULL;
				hResult = pDirectInput8W->CreateDevice(m_GamePadGUID[i],&pJoystickDeviceLocal,NULL);
				if (hResult>=0) {
					IDirectInputDevice8W *pJoystickDevice = pJoystickDeviceLocal;
					pJoypadData->m_pJoystickDevice = pJoystickDevice;
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
								pAppInstance->AddRoutine(Poll,this,RunQueue::PRIORITY_JOYPAD);
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
								is exactly 9000 (90 degrees), we will say anything between
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

void BURGER_API Burger::Joypad::Acquire(void)
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

void BURGER_API Burger::Joypad::Unacquire(void)
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

Word BURGER_API Burger::XInputStopRumbleOnAllControllers(void)
{
	// Set the vibration to off
	XINPUT_VIBRATION MyVibration;
	MyVibration.wLeftMotorSpeed = 0;
	MyVibration.wRightMotorSpeed = 0;

	Word32 i = 0;
	Word uResult = 0;
	do {
		// Only abort if XInput is not present, otherwise, issue the
		// command to every device, regardless of connection state
		Word uTemp = Globals::XInputSetState(i,&MyVibration);
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

Word BURGER_API Burger::XInputGetGamepadState(Word uWhich,XInputGamePad_t *pXInputGamePad,eXInputDeadZoneType uDeadZoneType)
{
	Word32 uResult;
	if ((uWhich >= 4) || !pXInputGamePad) {
		uResult = static_cast<Word32>(E_FAIL);
	} else {
		// Read in the data from the game pad
		XINPUT_STATE GamepadState;
		uResult = Globals::XInputGetState(uWhich,&GamepadState);

		// Test if XInput is present
		if (uResult!=ERROR_CALL_NOT_IMPLEMENTED) {

			// Test if this was an insertion or removal and report it

			// Get the old and new states
			Word bWasConnected = pXInputGamePad->m_bConnected!=0;	// Force boolean for & below
			Word bIsConnected = (uResult == ERROR_SUCCESS);

			// Save off the states as to how they were processed
			pXInputGamePad->m_bConnected = static_cast<Word8>(bIsConnected);
			pXInputGamePad->m_bRemoved = static_cast<Word8>(bWasConnected & (bIsConnected^1));
			pXInputGamePad->m_bInserted = static_cast<Word8>((bWasConnected^1) & bIsConnected);

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
				
				Int32 iThumbLX = GamepadState.Gamepad.sThumbLX;
				Int32 iThumbLY = GamepadState.Gamepad.sThumbLY;
				Int32 iThumbRX = GamepadState.Gamepad.sThumbRX;
				Int32 iThumbRY = GamepadState.Gamepad.sThumbRY;
				
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
				Word32 bButtons = GamepadState.Gamepad.wButtons;
				// Using the previous buttons, determine the ones "pressed"
				pXInputGamePad->m_uPressedButtons = (pXInputGamePad->m_uButtons ^ bButtons) & bButtons;
				pXInputGamePad->m_uButtons = bButtons;

				// Process the left trigger
				Word uTriggerValue = GamepadState.Gamepad.bLeftTrigger;
				Word bTriggerIsPressed = (uTriggerValue > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				Word bTriggerWasPressed = (pXInputGamePad->m_uLeftTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				pXInputGamePad->m_uLeftTrigger = static_cast<Word8>(uTriggerValue);
				pXInputGamePad->m_bPressedLeftTrigger = static_cast<Word8>(bTriggerIsPressed & (bTriggerWasPressed^1));

				// Process the right trigger
				uTriggerValue = GamepadState.Gamepad.bRightTrigger;
				bTriggerIsPressed = (uTriggerValue > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				bTriggerWasPressed = (pXInputGamePad->m_uRightTrigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD);
				pXInputGamePad->m_uRightTrigger = static_cast<Word8>(uTriggerValue);
				pXInputGamePad->m_bPressedRightTrigger = static_cast<Word8>(bTriggerIsPressed & (bTriggerWasPressed^1));
			}
		} else {
			// Zap the buffer if there is no XInput
			MemoryClear(pXInputGamePad,sizeof(XInputGamePad_t));
		}
	}
	return uResult;
}


#endif
