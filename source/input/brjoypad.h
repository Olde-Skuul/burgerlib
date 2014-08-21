/***************************************

	Joypad/joystick Manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRJOYPAD_H__
#define __BRJOYPAD_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
namespace Burger {
class Joypad {
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
		MAXAXIS=6,			///< Maximum number of joystick axises
#if defined(BURGER_XBOX360) || defined(BURGER_PS3)
		MAXJOYSTICKS=4,		///< 4 joystick ports on the Xbox 360 and PS3
#else
		MAXJOYSTICKS=8,		///< Maximum number of simultaneous joysticks
#endif
		MAXBUTTONS=20,		///< Maximum number of button controls
		MAXPOV=1,			///< Maximum number of POV controllers
		MAXAXISVALUE=255,	///< Maximum Axis value
		POVANALOGFORWARD=0,		///< POV Forward value
		POVANALOGRIGHT=9000,	///< POV Right value
		POVANALOGBACKWARD=18000,	///< POV Backward value
		POVANALOGLEFT=27000,		///< POV Left value
		MAXPOVVALUE=36000,	///< Maximum POV value
		INVALIDPOVVALUE=0xFFFFFFFF,	///< Invalid POV value
		POV_SLOP=1500		///< Slop value for POV to digital conversion (15 degrees)
	};
	struct JoypadRange_t {
		Word m_uMin;					///< Minimum value for a digital \ref TRUE
		Word m_uMax;					///< Maximum value for a digital \ref TRUE
	};
	struct JoypadData_t {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		IDirectInputDevice8W* m_pJoystickDevice;	///< DirectInput Device reference (WINDOWS only)
#endif
		Word m_bActive;					///< \ref TRUE if it's active and/or plugged in
		Word m_uButtonCount;			///< Number of buttons on this joypad
		Word m_uPOVCount;				///< Number of POV units on this joypad
		Word m_uAxisCount;				///< Number of Axis units on this joypad
		Word32 m_uButtonState;			///< Buttons currently pressed
		Word32 m_uButtonStatePressed;	///< Buttons pressed between reads (Clicked)
		Word32 m_uAxis[MAXAXIS];		///< Axis data from the joypad (0-255)
		Word m_uAxisPercents[MAXAXIS];	///< Cache for percentages
		JoypadRange_t m_uAxisDigitalRanges[MAXAXIS];	///< Digital ranges
	};
private:
	static RunQueue::eReturnCode BURGER_API Poll(void *pData);
	Joypad(Joypad const &);						///< Prevent copying
	Joypad & operator = (Joypad const &);			///< Prevent copying
	GameApp *m_pAppInstance;							///< Application instances
#if defined(BURGER_WINDOWS)
	GUID m_GamePadGUID[MAXJOYSTICKS];					///< GUIDs for each joystick device
#endif
	JoypadData_t m_Data[MAXJOYSTICKS];					///< Current input data from game devices
	Word m_uDeviceCount;								///< Number of game controller drivers found				
public:
	Joypad(GameApp *pAppInstance);
	~Joypad();
	Word32 ReadButtons(Word uWhich) const;
	Word ReadAbsolute(Word uWhich,Word uAxis) const;
	int ReadDelta(Word uWhich,Word uAxis) const;
	Word GetAxisCount(Word uWhich) const;
	void SetDigital(Word uWhich,Word uAxis,Word uPercent=20);
	BURGER_INLINE Word GetDeviceCount(void) const { return m_uDeviceCount; }
#if defined(BURGER_WINDOWS)
	void Acquire(void);
	void Unacquire(void);
#endif
};
}

/* END */

#endif
