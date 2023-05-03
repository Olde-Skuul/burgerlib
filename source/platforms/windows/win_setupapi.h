/***************************************

	Shims for setupapi.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __WIN_SETUPAPI_H__
#define __WIN_SETUPAPI_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGUID_H__
#include "brguid.h"
#endif

#ifndef __WIN_TYPES_H__
#include "win_types.h"
#endif

/* BEGIN */
namespace Burger { namespace Win32 {
extern void* BURGER_API SetupDiGetClassDevsA(const GUID* ClassGuid,
	const char* Enumerator, HWND__* hwndParent, uint32_t Flags) BURGER_NOEXCEPT;
extern void* BURGER_API SetupDiGetClassDevsW(const GUID* ClassGuid,
	const uint16_t* Enumerator, HWND__* hwndParent,
	uint32_t Flags) BURGER_NOEXCEPT;
extern int BURGER_API SetupDiGetDeviceInterfaceDetailA(void* DeviceInfoSet,
	_SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData,
	_SP_DEVICE_INTERFACE_DETAIL_DATA_A* DeviceInterfaceDetailData,
	uint32_t DeviceInterfaceDetailDataSize, uint32_t* RequiredSize,
	_SP_DEVINFO_DATA* DeviceInfoData) BURGER_NOEXCEPT;
extern int BURGER_API SetupDiGetDeviceInterfaceDetailW(void* DeviceInfoSet,
	_SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData,
	_SP_DEVICE_INTERFACE_DETAIL_DATA_W* DeviceInterfaceDetailData,
	uint32_t DeviceInterfaceDetailDataSize, uint32_t* RequiredSize,
	_SP_DEVINFO_DATA* DeviceInfoData) BURGER_NOEXCEPT;
extern int BURGER_API SetupDiEnumDeviceInterfaces(void* DeviceInfoSet,
	_SP_DEVINFO_DATA* DeviceInfoData, const GUID* InterfaceClassGuid,
	uint32_t MemberIndex,
	_SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData) BURGER_NOEXCEPT;
extern int BURGER_API SetupDiDestroyDeviceInfoList(
	void* DeviceInfoSet) BURGER_NOEXCEPT;
}}
/* END */

#endif
