/***************************************

    Joypad/joystick Manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	float m_fThumbLX;			///< Left thumbstick X -1.0f to 1.0f
	float m_fThumbLY;			///< Left thumbstick Y -1.0f to 1.0f
	float m_fThumbRX;			///< Right thumbstick X -1.0f to 1.0f
	float m_fThumbRY;			///< Right thumbstick Y -1.0f to 1.0f
	int32_t m_iThumbLX;			///< Left thumbstick X -32768 to 32767
	int32_t m_iThumbLY;			///< Left thumbstick Y -32768 to 32767
	int32_t m_iThumbRX;			///< Right thumbstick X -32768 to 32767
	int32_t m_iThumbRY;			///< Right thumbstick Y -32768 to 32767
	uint32_t m_uButtons;			///< Current state of the buttons
	uint32_t m_uPressedButtons;	///< Buttons that were "pressed" during this update
	uint8_t m_uLeftTrigger;		///< Left trigger (0-255)
	uint8_t m_uRightTrigger;		///< Right trigger (0-255)
	uint8_t m_bPressedLeftTrigger;	///< \ref TRUE if the left trigger was "Pressed" this update
	uint8_t m_bPressedRightTrigger;	///< \ref TRUE if the right trigger was "Pressed" this update
	uint8_t m_bConnected;			///< \ref TRUE If the game pad is currently connected
	uint8_t m_bInserted;			///< \ref TRUE if the game pad was inserted during the last update
	uint8_t m_bRemoved;			///< \ref TRUE if the game pad was removed during the last update
};

enum eXInputDeadZoneType {
	XINPUTDEADZONE_NONE,		///< Don't apply a dead zone
	XINPUTDEADZONE_CARDINAL,	///< Apply an axis based dead zone
	XINPUTDEADZONE_CENTER		///< Apply an x/y based dead zone
};

extern uint_t BURGER_API XInputStopRumbleOnAllControllers(void);
extern uint_t BURGER_API XInputGetGamepadState(uint_t uWhich,XInputGamePad_t *pXInputGamePad,eXInputDeadZoneType uDeadZoneType=XINPUTDEADZONE_NONE);
#endif

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
extern uint_t BURGER_API IsDeviceXInput(const GUID *pGuid);
#endif

class Joypad : public Base {
    BURGER_DISABLE_COPY(Joypad);
	BURGER_RTTI_IN_CLASS();
public:
	enum {
		AXIS1MIN=0x1,			///< Joystick Axis 1 minimum motion button
		AXIS1MAX=0x2,			///< Joystick Axis 1 maximum motion button
		AXIS2MIN=0x4,			///< Joystick Axis 2 minimum motion button
		AXIS2MAX=0x8,			///< Joystick Axis 2 maximum motion button
		AXIS3MIN=0x10,			///< Joystick Axis 3 minimum motion button
		AXIS3MAX=0x20,			///< Joystick Axis 3 maximum motion button
		AXIS4MIN=0x40,			///< Joystick Axis 4 minimum motion button
		AXIS4MAX=0x80,			///< Joystick Axis 4 maximum motion button
		POVLEFT=0x100,			///< POV left button
		POVRIGHT=0x200,			///< POV right button
		POVUP=0x400,			///< POV up button
		POVDOWN=0x800,			///< POV down button
		BUTTON1=0x1000,			///< 1st joystick button
		BUTTON2=0x2000,			///< 2nd joystick button
		BUTTON3=0x4000,			///< 3rd joystick button
		BUTTON4=0x8000,			///< 4th joystick button
		BUTTON5=0x10000,		///< 5th joystick button
		BUTTON6=0x20000,		///< 6th joystick button
		BUTTON7=0x40000,		///< 7th joystick button
		BUTTON8=0x80000,		///< 8th joystick button
		BUTTON9=0x100000,		///< 9th joystick button
		BUTTON10=0x200000,		///< 10th joystick button
		BUTTON11=0x400000,		///< 11th joystick button
		BUTTON12=0x800000,		///< 12th joystick button
		BUTTON13=0x1000000,		///< 13th joystick button
		BUTTON14=0x2000000,		///< 14th joystick button
		BUTTON15=0x4000000,		///< 15th joystick button
		BUTTON16=0x8000000,		///< 16th joystick button
		BUTTON17=0x10000000,	///< 17th joystick button
		BUTTON18=0x20000000,	///< 18th joystick button
		BUTTON19=0x40000000,	///< 19th joystick button
		BUTTON20=0x80000000		///< 20th joystick button
	};

	enum {
		MAXAXIS=6,				///< Maximum number of joystick axises
#if defined(BURGER_XBOX360) || defined(BURGER_XBOXONE) || defined(BURGER_PS3) || defined(BURGER_PS4)
		MAXJOYSTICKS=4,			///< 4 joystick ports on the Xbox 360, Xbox ONE, PS3 and PS4
#else
		MAXJOYSTICKS=8,			///< Maximum number of simultaneous joysticks
#endif
		MAXBUTTONS=20,				///< Maximum number of button controls
		MAXPOV=1,					///< Maximum number of POV controllers
		CENTERAXISVALUE=32768,		///< Axis value when centered
		MAXAXISVALUE=65535,			///< Maximum Axis value
		POVANALOGFORWARD=0,			///< POV Forward value
		POVANALOGRIGHT=9000,		///< POV Right value
		POVANALOGBACKWARD=18000,	///< POV Backward value
		POVANALOGLEFT=27000,		///< POV Left value
		MAXPOVVALUE=36000,			///< Maximum POV value
		INVALIDPOVVALUE=0xFFFFFFFF,	///< Invalid POV value
		POV_SLOP=1500				///< Slop value for POV to digital conversion (15 degrees)
	};

	struct JoypadRange_t {
		uint_t m_uMin;					///< Minimum value for a digital \ref TRUE
		uint_t m_uMax;					///< Maximum value for a digital \ref TRUE
	};

	struct JoypadData_t {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		IDirectInputDevice8W* m_pJoystickDevice;	///< DirectInput Device reference (WINDOWS only)
		GUID m_InstanceGUID;						///< DirectInput Device instance GUID (WINDOWS only)
		GUID m_ProductGUID;							///< DirectInput Device product GUID (WINDOWS only)
		String m_InstanceName;						///< Name of the device instance UTF-8 encoded (WINDOWS only)
		String m_ProductName;						///< Name of the device UTF-8 encoded (WINDOWS only)
#endif
		uint_t m_bConnected;				///< \ref TRUE If the game pad is currently connected
		uint_t m_bInserted;				///< \ref TRUE if the game pad was inserted during the last update
		uint_t m_bRemoved;				///< \ref TRUE if the game pad was removed during the last update
		uint_t m_uButtonCount;			///< Number of buttons on this joypad
		uint_t m_uPOVCount;				///< Number of POV units on this joypad
		uint_t m_uAxisCount;				///< Number of Axis units on this joypad
		uint32_t m_uButtonState;			///< Buttons currently pressed
		uint32_t m_uButtonStatePressed;	///< Buttons pressed between reads (Clicked)
		uint32_t m_uAxis[MAXAXIS];		///< Axis data from the joypad (0-255)
		uint_t m_uAxisPercents[MAXAXIS];	///< Cache for percentages
		JoypadRange_t m_uAxisDigitalRanges[MAXAXIS];	///< Digital ranges
	};

private:
	GameApp *m_pAppInstance;							///< Application instances

#if (defined(BURGER_WINDOWS) || defined(BURGER_XBOX360)) || defined(DOXYGEN)
	XInputGamePad_t m_XInputGamepads[4];				///< Structures for each XInput device
#endif

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	uint_t m_bDirectInputFound;							///< DirectInput devices found
	uint_t m_bXInputFound;								///< XInput devices found
	uint_t m_uDirectInputDevices;							///< Number of devices found from DirectInput, not managed by XInput
#endif

	JoypadData_t m_Data[MAXJOYSTICKS];					///< Current input data from game devices
	uint_t m_uDeviceCount;								///< Number of game controller drivers found				

public:
	Joypad(GameApp *pAppInstance);
	virtual ~Joypad();
	uint32_t BURGER_API ReadButtons(uint_t uWhich) const;
	uint_t BURGER_API ReadAbsolute(uint_t uWhich,uint_t uAxis) const;
	int BURGER_API ReadDelta(uint_t uWhich,uint_t uAxis) const;
	uint_t BURGER_API GetAxisCount(uint_t uWhich) const;
	void BURGER_API SetDigital(uint_t uWhich,uint_t uAxis,uint_t uPercent=20);
	BURGER_INLINE uint_t GetDeviceCount(void) const { return m_uDeviceCount; }
	static RunQueue::eReturnCode BURGER_API Poll(void *pData) BURGER_NOEXCEPT;

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	void BURGER_API AcquireDirectInput(void);
	void BURGER_API UnacquireDirectInput(void);
#endif

};
}

/* END */

#endif
