/***************************************

	Joypad/joystick Manager
	
	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brjoypad.h"
#include "brtick.h"
#include "brmemoryfunctions.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Joypad,Burger::Base);
#endif

/*! ************************************

	\brief Init joystick services

	Init the joystick services and detect if a joystick(s) is connected. No joystick manager
	call will operate properly unless this call is issued. You can call ~Joypad() to
	shut down the operation at any time.

	If you call this function again, the joysticks will be rescanned and your configuration
	may change. This is due to the fact that users could plug in a joystick at
	any time.

	\return The number of joysticks found or 0 in m_uDeviceCount if an error occurs.

	\note Win95/98/NT uses the multimedia manager to read the joystick. MacOS uses InputSprocket
		and MS-DOS and GS/OS uses direct joystick drivers. MS-DOS and GS/OS must be calibrated
		for accurate results.

	\sa ~Joypad()

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
Burger::Joypad::Joypad(GameApp *pAppInstance)
{
	m_pAppInstance = pAppInstance;
	MemoryClear(m_Data,sizeof(m_Data));
}
#endif

/*! ************************************

	\brief Shut down joystick services

	Stop joystick scanning and release all resources attached to reading the joystick.
	Useful for manual shutdown and restart of joystick services on Win95 and MacOS

	\note Win95/98/NT uses the multimedia manager to read the joystick. MacOS uses InputSprocket
		and MS-DOS and GS/OS uses direct joystick drivers. MS-DOS and GS/OS must be calibrated
		for accurate results.

	\sa Joypad(GameApp *)

***************************************/

#if (!defined(BURGER_WINDOWS) && !defined(BURGER_XBOX360)) || defined(DOXYGEN)
Burger::Joypad::~Joypad()
{
}
#endif

/*! ************************************

	\brief Return the game controller button states

	Return the current state of all the input buttons

	\return 32 bit value containing the true/false state of the buttons

***************************************/

Word32 BURGER_API Burger::Joypad::ReadButtons(Word uWhich) const
{
	Word32 uButtons = 0;
	if (uWhich<m_uDeviceCount) {
		const JoypadData_t *pJoypadData = &m_Data[uWhich];
		uButtons = pJoypadData->m_uButtonState;
	}
	return uButtons;
}

/*! ************************************

	\brief Read an analog joystick axis

	Get information from the device and axis of a joystick and return
	the value of the axis at this moment in time. The value returned is 0
	to 65535 with 0 being left/up and 65535 being right/down.

	Since reading a joystick may be slow, the function Poll() will
	actually perform the read and keep all the data in a cache so that the
	subsequent calls will have very little overhead. If you must be certain that
	data is freshly read, call Poll() before this call to make sure the
	data is fresh.

	\param uAxis Analog axis for the joystick
	\param uWhich Which joystick device to read

	\note Win95/98/NT uses the multimedia manager to read the joystick. MacOS uses InputSprocket
		and MS-DOS and GS/OS uses direct joystick drivers. MS-DOS and GS/OS must be calibrated
		for accurate results.

	\sa ReadDelta(), ReadButtons(), Poll(Word)

***************************************/

Word BURGER_API Burger::Joypad::ReadAbsolute(Word uWhich,Word uAxis) const
{
	Word uResult = CENTERAXISVALUE;
	// Bounds check
	if (uWhich<m_uDeviceCount) {
		const JoypadData_t *pJoypadData = &m_Data[uWhich];
		if (uAxis<pJoypadData->m_uAxisCount) {
			// Get the value
			uResult = pJoypadData->m_uAxis[uAxis];
		}
	}
	return uResult;
}

/*! ************************************

	\brief Read an analog joystick axis as delta

	Get information from the device and axis of a joystick and return
	the value of the axis at this moment in time. The value returned is -32768
	to 32767 with -32768 being left/up and 32767 being right/down.

	Since reading a joystick may be slow, the function Poll(Word) will
	actually perform the read and keep all the data in a cache so that the
	subsequent calls will have very little overhead. If you must be certain that
	data is freshly read, call Poll(Word) before this call to make sure the
	data is fresh.

	\param uAxis Analog axis for the joystick
	\param uWhich Which joystick device to read

	\sa ReadAbsolute(), ReadButtons(), Poll(Word)

***************************************/

int BURGER_API Burger::Joypad::ReadDelta(Word uWhich,Word uAxis) const
{
	// Convert absolute value to signed offset
	return static_cast<int>(ReadAbsolute(uWhich,uAxis))-CENTERAXISVALUE;
}

/*! ************************************

	\brief Return the number of analog axises a gamepad possesses

	Return the number of input axis that return analog data. Zero if none are
	found or there is no device present

	\param uWhich Which joystick device to affect

	\sa ReadAbsolute()

***************************************/

Word BURGER_API Burger::Joypad::GetAxisCount(Word uWhich) const
{
	Word uResult = 0;
	// Bounds check
	if (uWhich<m_uDeviceCount) {
		const JoypadData_t *pJoypadData = &m_Data[uWhich];
		uResult = pJoypadData->m_uAxisCount;
	}
	return uResult;

}

/*! ************************************

	\brief Set the boundaries for the joystick

	To simulate a digital joypad with a analog joystick, you need to
	define a bounds rect that has the dead area for digital motion.
	Burgerlib has an acceptable default but you can override it with this
	call.

	\param uWhich Which joystick device to affect
	\param uAxis Which analog axis to affect
	\param uPercent Percentage from center point for dead zone (20% is normal)

	\sa ReadButtons()

***************************************/

void BURGER_API Burger::Joypad::SetDigital(Word uWhich,Word uAxis,Word uPercent)
{
	// Bounds check
	if (uWhich<m_uDeviceCount) {
		JoypadData_t *pJoypadData = &m_Data[uWhich];
		if (uAxis<pJoypadData->m_uAxisCount) {
			// Save for later
			pJoypadData->m_uAxisPercents[uAxis] = uPercent;
			// Get the percentage
			Word uDistance = (uPercent*(MAXAXISVALUE/2))/100;
			JoypadRange_t *pRange = &pJoypadData->m_uAxisDigitalRanges[uAxis];
			// Save the lower value
			pRange->m_uMin = (MAXAXISVALUE/2)-uDistance;
			// Save the upper value
			pRange->m_uMax = (MAXAXISVALUE/2)+uDistance;
		}
	}
}

/*! ************************************

	\struct Burger::Joypad::JoypadRange_t
	\brief Range for each axis to convert analog data to digital data

***************************************/

/*! ************************************

	\struct Burger::Joypad::JoypadRawData_t
	\brief Current data read for a joypad

***************************************/

/*! ************************************

	\struct Burger::Joypad::AutoRepeat_t
	\brief Data to handle joystick button auto repeating

***************************************/

/*! ************************************

	\struct Burger::Joypad::JoypadData_t
	\brief Data record for each joypad

***************************************/
