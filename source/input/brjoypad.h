/***************************************

	Joypad/joystick Manager

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRJOYPAD_H__
#define __BRJOYPAD_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
namespace Burger {

#if defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || defined(DOXYGEN)

struct XInputGamePad_t {
	/** Left thumbstick X -1.0f to 1.0f */
	float m_fThumbLX;
	/** Left thumbstick Y -1.0f to 1.0f */
	float m_fThumbLY;
	/** Right thumbstick X -1.0f to 1.0f */
	float m_fThumbRX;
	/** Right thumbstick Y -1.0f to 1.0f */
	float m_fThumbRY;
	/** Left thumbstick X -32768 to 32767 */
	int32_t m_iThumbLX;
	/** Left thumbstick Y -32768 to 32767 */
	int32_t m_iThumbLY;
	/** Right thumbstick X -32768 to 32767 */
	int32_t m_iThumbRX;
	/** Right thumbstick Y -32768 to 32767 */
	int32_t m_iThumbRY;
	/** Current state of the buttons */
	uint32_t m_uButtons;
	/** Buttons that were "pressed" during this update */
	uint32_t m_uPressedButtons;
	/** Left trigger (0-255) */
	uint8_t m_uLeftTrigger;
	/** Right trigger (0-255) */
	uint8_t m_uRightTrigger;
	/** \ref TRUE if the left trigger was "Pressed" this update */
	uint8_t m_bPressedLeftTrigger;
	/** \ref TRUE if the right trigger was "Pressed" this update */
	uint8_t m_bPressedRightTrigger;
	/** \ref TRUE If the game pad is currently connected */
	uint8_t m_bConnected;
	/** \ref TRUE if the game pad was inserted during the last update */
	uint8_t m_bInserted;
	/** \ref TRUE if the game pad was removed during the last update */
	uint8_t m_bRemoved;
};

enum eXInputDeadZoneType {
	/** Don't apply a dead zone */
	XINPUTDEADZONE_NONE,
	/** Apply an axis based dead zone */
	XINPUTDEADZONE_CARDINAL,
	/** Apply an x/y based dead zone */
	XINPUTDEADZONE_CENTER
};

extern uint_t BURGER_API XInputStopRumbleOnAllControllers(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API XInputGetGamepadState(uint_t uWhich,
	XInputGamePad_t* pXInputGamePad,
	eXInputDeadZoneType uDeadZoneType = XINPUTDEADZONE_NONE) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
extern uint_t BURGER_API IsDeviceXInput(const GUID* pGuid) BURGER_NOEXCEPT;
#endif

class Joypad: public Base {
	BURGER_DISABLE_COPY(Joypad);
	BURGER_RTTI_IN_CLASS();

public:
	enum {
		/** Joystick Axis 1 minimum motion button */
		AXIS1MIN = 0x1,
		/** Joystick Axis 1 maximum motion button */
		AXIS1MAX = 0x2,
		/** Joystick Axis 2 minimum motion button */
		AXIS2MIN = 0x4,
		/** Joystick Axis 2 maximum motion button */
		AXIS2MAX = 0x8,
		/** Joystick Axis 3 minimum motion button */
		AXIS3MIN = 0x10,
		/** Joystick Axis 3 maximum motion button */
		AXIS3MAX = 0x20,
		/** Joystick Axis 4 minimum motion button */
		AXIS4MIN = 0x40,
		/** Joystick Axis 4 maximum motion button */
		AXIS4MAX = 0x80,
		/** POV left button */
		POVLEFT = 0x100,
		/** POV right button */
		POVRIGHT = 0x200,
		/** POV up button */
		POVUP = 0x400,
		/** POV down button */
		POVDOWN = 0x800,
		/** 1st joystick button */
		BUTTON1 = 0x1000,
		/** 2nd joystick button */
		BUTTON2 = 0x2000,
		/** 3rd joystick button */
		BUTTON3 = 0x4000,
		/** 4th joystick button */
		BUTTON4 = 0x8000,
		/** 5th joystick button */
		BUTTON5 = 0x10000,
		/** 6th joystick button */
		BUTTON6 = 0x20000,
		/** 7th joystick button */
		BUTTON7 = 0x40000,
		/** 8th joystick button */
		BUTTON8 = 0x80000,
		/** 9th joystick button */
		BUTTON9 = 0x100000,
		/** 10th joystick button */
		BUTTON10 = 0x200000,
		/** 11th joystick button */
		BUTTON11 = 0x400000,
		/** 12th joystick button */
		BUTTON12 = 0x800000,
		/** 13th joystick button */
		BUTTON13 = 0x1000000,
		/** 14th joystick button */
		BUTTON14 = 0x2000000,
		/** 15th joystick button */
		BUTTON15 = 0x4000000,
		/** 16th joystick button */
		BUTTON16 = 0x8000000,
		/** 17th joystick button */
		BUTTON17 = 0x10000000,
		/** 18th joystick button */
		BUTTON18 = 0x20000000,
		/** 19th joystick button */
		BUTTON19 = 0x40000000,
		/** 20th joystick button */
		BUTTON20 = 0x80000000
	};

	enum {
		/** Maximum number of joystick axises */
		MAXAXIS = 6,
#if defined(BURGER_XBOX360) || defined(BURGER_XBOXONE) || \
	defined(BURGER_PS3) || defined(BURGER_PS4)
		/** 4 joystick ports on the Xbox 360, Xbox ONE, PS3 and PS4 */
		MAXJOYSTICKS = 4,
#else
		/** Maximum number of simultaneous joysticks */
		MAXJOYSTICKS = 8,
#endif
		/** Maximum number of button controls */
		MAXBUTTONS = 20,
		/** Maximum number of POV controllers */
		MAXPOV = 1,
		/** Axis value when centered */
		CENTERAXISVALUE = 32768,
		/** Maximum Axis value */
		MAXAXISVALUE = 65535,
		/** POV Forward value */
		POVANALOGFORWARD = 0,
		/** POV Right value */
		POVANALOGRIGHT = 9000,
		/** POV Backward value */
		POVANALOGBACKWARD = 18000,
		/** POV Left value */
		POVANALOGLEFT = 27000,
		/** Maximum POV value */
		MAXPOVVALUE = 36000,
		/** Invalid POV value */
		INVALIDPOVVALUE = 0xFFFFFFFF,
		/** Slop value for POV to digital conversion (15 degrees) */
		POV_SLOP = 1500
	};

	struct JoypadRange_t {
		/** Minimum value for a digital \ref TRUE */
		uint_t m_uMin;
		/** Maximum value for a digital \ref TRUE */
		uint_t m_uMax;
	};

	struct JoypadData_t {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		/** DirectInput Device reference (WINDOWS only) */
		IDirectInputDevice8W* m_pJoystickDevice;
		/** DirectInput Device instance GUID (WINDOWS only) */
		GUID m_InstanceGUID;
		/** DirectInput Device product GUID (WINDOWS only) */
		GUID m_ProductGUID;
		/** Name of the device instance UTF-8 encoded (WINDOWS only) */
		String m_InstanceName;
		/** Name of the device UTF-8 encoded (WINDOWS only) */
		String m_ProductName;
#endif
		/** \ref TRUE If the game pad is currently connected */
		uint_t m_bConnected;
		/** \ref TRUE if the game pad was inserted during the last update */
		uint_t m_bInserted;
		/** \ref TRUE if the game pad was removed during the last update */
		uint_t m_bRemoved;
		/** Number of buttons on this joypad */
		uint_t m_uButtonCount;
		/** Number of POV units on this joypad */
		uint_t m_uPOVCount;
		/** Number of Axis units on this joypad */
		uint_t m_uAxisCount;
		/** Buttons currently pressed */
		uint32_t m_uButtonState;
		/** Buttons pressed between reads (Clicked) */
		uint32_t m_uButtonStatePressed;
		/** Axis data from the joypad (0-255) */
		uint32_t m_uAxis[MAXAXIS];
		/** Cache for percentages */
		uint_t m_uAxisPercents[MAXAXIS];
		/** Digital ranges */
		JoypadRange_t m_uAxisDigitalRanges[MAXAXIS];
	};

private:
	/** Application instances */
	GameApp* m_pAppInstance;

#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
	/** Structures for each XInput device */
	XInputGamePad_t m_XInputGamepads[4];
#endif

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	/** DirectInput devices found */
	uint_t m_bDirectInputFound;
	/** XInput devices found */
	uint_t m_bXInputFound;
	/** Number of devices found from DirectInput, not managed by XInput */
	uint_t m_uDirectInputDevices;

#endif

	/** Current input data from game devices */
	JoypadData_t m_Data[MAXJOYSTICKS];
	/** Number of game controller drivers found */
	uint_t m_uDeviceCount;

public:
	Joypad(GameApp* pAppInstance) BURGER_NOEXCEPT;
	virtual ~Joypad();
	uint32_t BURGER_API ReadButtons(uint_t uWhich) const BURGER_NOEXCEPT;
	uint_t BURGER_API ReadAbsolute(
		uint_t uWhich, uint_t uAxis) const BURGER_NOEXCEPT;
	int BURGER_API ReadDelta(uint_t uWhich, uint_t uAxis) const BURGER_NOEXCEPT;
	uint_t BURGER_API GetAxisCount(uint_t uWhich) const BURGER_NOEXCEPT;
	void BURGER_API SetDigital(
		uint_t uWhich, uint_t uAxis, uint_t uPercent = 20) BURGER_NOEXCEPT;
	BURGER_INLINE uint_t GetDeviceCount(void) const BURGER_NOEXCEPT
	{
		return m_uDeviceCount;
	}
	static RunQueue::eReturnCode BURGER_API Poll(void* pData) BURGER_NOEXCEPT;

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	void BURGER_API AcquireDirectInput(void) BURGER_NOEXCEPT;
	void BURGER_API UnacquireDirectInput(void) BURGER_NOEXCEPT;
#endif
};
}

/* END */

#endif
