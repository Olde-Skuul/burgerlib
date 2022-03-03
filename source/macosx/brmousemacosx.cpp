/***************************************

    Mouse Manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brmouse.h"

#if defined(BURGER_MACOSX) || defined(DOXYGEN)

#if !defined(DOXYGEN)
#include "brglobals.h"
#import <CoreFoundation/CFDictionary.h>
#import <CoreFoundation/CFNumber.h>
#import <IOKit/hid/IOHIDBase.h>
#import <IOKit/hid/IOHIDManager.h>

static const CFStringRef g_BurgerMouse CFSTR("BurgerMouse");
#endif

/***************************************

	Initialize the mouse class

***************************************/

Burger::Mouse::Mouse(GameApp* pGameApp) :
	m_pGameApp(pGameApp), m_MouseLock(), m_pHIDManager(nullptr),
	m_uMiceCount(0), m_uX(0), m_uY(0), m_uBoundsX(640), m_uBoundsY(480),
	m_iDeltaX(0), m_iDeltaY(0), m_iMouseWheelX(0), m_iMouseWheelY(0),
	m_uButtons(0), m_uPressedButtons(0), m_bButtonSwap(FALSE), m_uArrayStart(0),
	m_uArrayEnd(0)
{
	// Back link to the game app
	CFMutableDictionaryRef pDictionary = Globals::CreateHIDDictionary(
		kHIDPage_GenericDesktop, kHIDUsage_GD_Mouse);
	if (pDictionary) {
		m_pHIDManager =
			IOHIDManagerCreate(kCFAllocatorDefault, kIOHIDOptionsTypeNone);
		if (m_pHIDManager) {
			CFRunLoopRef pRunLoop = CFRunLoopGetCurrent();
			IOHIDManagerRegisterDeviceMatchingCallback(
				m_pHIDManager, EnumerationCallback, this);
			IOHIDManagerScheduleWithRunLoop(
				m_pHIDManager, pRunLoop, g_BurgerMouse);
			IOHIDManagerSetDeviceMatching(m_pHIDManager, pDictionary);
			IOHIDManagerOpen(m_pHIDManager, kIOHIDOptionsTypeNone);

			// Handle the run loops
			Poll(this);

			// All scanned!
			IOHIDManagerUnscheduleFromRunLoop(
				m_pHIDManager, pRunLoop, g_BurgerMouse);
			IOHIDManagerRegisterDeviceMatchingCallback(
				m_pHIDManager, nullptr, nullptr);

			// Open all the located devices
			uint_t i;
			DeviceStruct* pRat = m_Mice;
			for (i = 0; i < m_uMiceCount; i++) {
				IOHIDDeviceRef pDevice = pRat->m_pDevice;
				if (IOHIDDeviceOpen(pDevice, kIOHIDOptionsTypeNone) !=
					kIOReturnSuccess) {
					pRat->m_pDevice = nullptr;  // Hmm. Toast it
					pRat->m_bUnplugged = FALSE; // Don't attempt to reconnect
				} else {
					IOHIDDeviceRegisterRemovalCallback(
						pDevice, DisconnectionCallback, this);
					IOHIDDeviceRegisterInputValueCallback(
						pDevice, InputCallback, this);
					IOHIDDeviceScheduleWithRunLoop(
						pDevice, pRunLoop, g_BurgerMouse);
				}
				++pRat;
			}
			pGameApp->AddRoutine(Poll, nullptr, this, RunQueue::PRIORITY_MOUSE);
		}
		CFRelease(pDictionary);
	}
}

/***************************************

	Shut down the mouse class

***************************************/

Burger::Mouse::~Mouse()
{
	m_pGameApp->RemoveRoutine(Poll, this);
	if (m_pHIDManager) {
		IOHIDManagerClose(m_pHIDManager, kIOHIDOptionsTypeNone);
		CFRelease(m_pHIDManager);
		m_pHIDManager = nullptr;
	}
	m_uMiceCount = 0;
}

/***************************************

	Return TRUE if a mouse is present in the device list

***************************************/

uint_t Burger::Mouse::IsPresent(void) const
{
	return TRUE;
}

//
// Called for each device found during enumeration
//

void Burger::Mouse::EnumerationCallback(
	void* pData, int iReturn, void* /* pSender */, IOHIDDeviceRef pDevice)
{
	// Accepted?
	if (iReturn == kIOReturnSuccess) {
		Mouse* pMouse = static_cast<Mouse*>(pData);
		// Will it overrun the mice buffer?
		if (pMouse->m_uMiceCount < MAX_MOUSE_DEVICE_COUNT) {
			DeviceStruct* pRat = &pMouse->m_Mice[pMouse->m_uMiceCount];
			pRat->m_pDevice = pDevice;
			Globals::GetHIDDeviceName(&pRat->m_Name, pDevice);
			// Only named mice are accepted
			if (pRat->m_Name.GetLength()) {
				pRat->m_bUnplugged = FALSE;
				++pMouse->m_uMiceCount;
			}
		}
	}
}

//
// Callback fires whenever a device is unplugged/lost/whatever.
//

void Burger::Mouse::DisconnectionCallback(
	void* pData, int iReturn, void* pSender)
{
	if (iReturn == kIOReturnSuccess) {
		Mouse* pMouse = static_cast<Mouse*>(pData);
		uint_t uCount = pMouse->m_uMiceCount;
		if (uCount) {
			DeviceStruct* pRat = pMouse->m_Mice;
			do {
				if (pRat->m_pDevice == pSender) {
					pRat->m_bUnplugged = TRUE;
				}
				++pRat;
			} while (--uCount);
		}
	}
}

//
// Callback when mouse events occur
//

void Burger::Mouse::InputCallback(
	void* pData, int iReturn, void* /* pSender */, IOHIDValueRef pValue)
{
	if (iReturn == kIOReturnSuccess) {
		Mouse* pMouse = static_cast<Mouse*>(pData);
		uint_t uCount = pMouse->m_uMiceCount;
		if (uCount) {
			IOHIDElementRef pElement = IOHIDValueGetElement(pValue);
#if 0
			IOHIDDeviceRef pDevice = IOHIDElementGetDevice(pElement);
			uint_t uRatNumber = BURGER_MAXUINT;
			const DeviceStruct *pRat = pMouse->m_Mice;
			do {
				if (pRat->m_pDevice == pDevice) {
					uRatNumber = pMouse->m_uMiceCount-uCount;
					break;
				}
				++pRat;
			} while (--uCount);
			if (uRatNumber==BURGER_MAXUINT) {
			}
#endif
			uint32_t uTime = static_cast<uint32_t>(IOHIDValueGetTimeStamp(pValue));
			CFIndex iValue = IOHIDValueGetIntegerValue(pValue);

			uint32_t uPage = IOHIDElementGetUsagePage(pElement);
			uint32_t uUsage = IOHIDElementGetUsage(pElement);
			switch (uPage) {
			case kHIDPage_GenericDesktop:
				if (iValue) {
					switch (uUsage) {
					case kHIDUsage_GD_X:
						pMouse->PostMouseMotion(
							static_cast<int32_t>(iValue), 0, uTime);
						break;
					case kHIDUsage_GD_Y:
						pMouse->PostMouseMotion(
							0, static_cast<int32_t>(iValue), uTime);
						break;
					case kHIDUsage_GD_Wheel:
						pMouse->PostMouseWheel(
							0, static_cast<int32_t>(iValue), uTime);
						break;
					default:
						printf("Unknown usage %u\n", uUsage);
						break;
					}
				}
				break;
			case kHIDPage_Button:
				// iValue == down
				// Usage = which 1.2.3.4
				if (iValue) {
					pMouse->PostMouseDown(1 << (uUsage - 1));
				} else {
					pMouse->PostMouseUp(1 << (uUsage - 1));
				}
				break;
				// Ignore this one
			case kHIDPage_Consumer:
				break;
			default:
				printf("Unknown page found %u\n", uPage);
				break;
			}
		}
	}
}

Burger::RunQueue::eReturnCode BURGER_API Burger::Mouse::Poll(void* pData) BURGER_NOEXCEPT
{
	while (CFRunLoopRunInMode(g_BurgerMouse, 0, TRUE) ==
		kCFRunLoopRunHandledSource) {
	}
#if 0
	uint_t i;
	Mouse *pMouse = static_cast<Mouse *>(pData);
	DeviceStruct *pRat = pMouse->m_Mice;
	for (i = 0; i < pMouse->m_uMiceCount; i++) {
		if (pRat->m_bUnplugged) {
			IOHIDDeviceRef pDevice = pRat->m_pDevice;
			if (pDevice) {
				if (IOHIDDeviceOpen(pDevice,kIOHIDOptionsTypeNone) == kIOReturnSuccess) {
					pRat->m_bUnplugged = FALSE;	// Connected!
					IOHIDDeviceRegisterRemovalCallback(pDevice,DisconnectionCallback,pMouse);
					IOHIDDeviceRegisterInputValueCallback(pDevice,InputCallback,pMouse);
					CFRunLoopRef pRunLoop = CFRunLoopGetCurrent();
					IOHIDDeviceScheduleWithRunLoop(pDevice,pRunLoop,g_BurgerMouse);
				}
			}
		}
		++pRat;
	}
#else
	BURGER_UNUSED(pData);
#endif
	return RunQueue::OKAY;
}

#endif
