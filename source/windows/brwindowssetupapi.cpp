/***************************************

    Shims for setupapi.dll

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brwindowstypes.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#include <Windows.h>

#include <SetupAPI.h>

typedef HDEVINFO(WINAPI* SetupDiGetClassDevsAPtr)(
	CONST GUID* ClassGuid, PCSTR Enumerator, HWND hwndParent, DWORD Flags);
typedef HDEVINFO(WINAPI* SetupDiGetClassDevsWPtr)(
	CONST GUID* ClassGuid, PCWSTR Enumerator, HWND hwndParent, DWORD Flags);
typedef BOOL(WINAPI* SetupDiGetDeviceInterfaceDetailAPtr)(
	HDEVINFO DeviceInfoSet, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
	PSP_DEVICE_INTERFACE_DETAIL_DATA_A DeviceInterfaceDetailData,
	DWORD DeviceInterfaceDetailDataSize, PDWORD RequiredSize,
	PSP_DEVINFO_DATA DeviceInfoData);
typedef BOOL(WINAPI* SetupDiGetDeviceInterfaceDetailWPtr)(
	HDEVINFO DeviceInfoSet, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData,
	PSP_DEVICE_INTERFACE_DETAIL_DATA_W DeviceInterfaceDetailData,
	DWORD DeviceInterfaceDetailDataSize, PDWORD RequiredSize,
	PSP_DEVINFO_DATA DeviceInfoData);
typedef BOOL(WINAPI* SetupDiEnumDeviceInterfacesPtr)(HDEVINFO DeviceInfoSet,
	PSP_DEVINFO_DATA DeviceInfoData, CONST GUID* InterfaceClassGuid,
	DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);
typedef BOOL(WINAPI* SetupDiDestroyDeviceInfoListPtr)(HDEVINFO DeviceInfoSet);

// Unit tests for pointers

// SetupDiGetClassDevsAPtr gSetupDiGetClassDevsA = ::SetupDiGetClassDevsA;
// SetupDiGetClassDevsWPtr gSetupDiGetClassDevsW = ::SetupDiGetClassDevsW;
// SetupDiGetDeviceInterfaceDetailAPtr gSetupDiGetDeviceInterfaceDetailA =
//	::SetupDiGetDeviceInterfaceDetailA;
// SetupDiGetDeviceInterfaceDetailWPtr gSetupDiGetDeviceInterfaceDetailW =
//	::SetupDiGetDeviceInterfaceDetailW;
// SetupDiEnumDeviceInterfacesPtr gSetupDiEnumDeviceInterfaces =
//	::SetupDiEnumDeviceInterfaces;
// SetupDiDestroyDeviceInfoListPtr gSetupDiDestroyDeviceInfoList =
//	::SetupDiDestroyDeviceInfoList;

#endif

//
// setupapi.dll
//

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiGetClassDevsA

	Manually load Setupapi.dll if needed and call the Windows function
	SetupDiGetClassDevsA()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551069(v=vs.85).aspx

	\windowsonly

	\param ClassGuid A pointer to the GUID for a device setup class or a device
		interface class.
	\param Enumerator A pointer to a NULL-terminated string that specifies a PnP
		enumerator or instance ID
	\param hwndParent A handle to the top-level window to be used for a user
		interface that is associated with installing a device instance in the
		device information set.
	\param Flags A variable of type DWORD that specifies control options that
		filter the device information elements that are added to the device
		information set.

	\return If the operation succeeds, SetupDiGetClassDevs returns a handle to a
		device information set that contains all installed devices that matched
		the supplied parameters.

***************************************/

void* BURGER_API Burger::Windows::SetupDiGetClassDevsA(const GUID* ClassGuid,
	const char* Enumerator, HWND__* hwndParent, Word32 Flags)
{
	void* pSetupDiGetClassDevsA = LoadFunctionIndex(CALL_SetupDiGetClassDevsA);
	void* pResult = INVALID_HANDLE_VALUE; // Failure
	if (pSetupDiGetClassDevsA) {
		pResult = static_cast<SetupDiGetClassDevsAPtr>(pSetupDiGetClassDevsA)(
			ClassGuid, Enumerator, hwndParent, Flags);
	}
	return pResult;
}

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiGetClassDevsW

	Manually load Setupapi.dll if needed and call the Windows function
	SetupDiGetClassDevsW()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551069(v=vs.85).aspx

	\windowsonly

	\param ClassGuid A pointer to the GUID for a device setup class or a device
		interface class.
	\param Enumerator A pointer to a NULL-terminated string that specifies a PnP
		enumerator or instance ID \param hwndParent A handle to the top-level
		window to be used for a user interface that is associated with
		installing a device instance in the device information set.
	\param Flags A variable of type DWORD that specifies control options that
		filter the device information elements that are added to the device
		information set.

	\return If the operation succeeds, SetupDiGetClassDevs returns a handle to a
		device information set that contains all installed devices that matched
		the supplied parameters.

***************************************/

void* BURGER_API Burger::Windows::SetupDiGetClassDevsW(const GUID* ClassGuid,
	const Word16* Enumerator, HWND__* hwndParent, Word32 Flags)
{
	void* pSetupDiGetClassDevsW = LoadFunctionIndex(CALL_SetupDiGetClassDevsW);
	void* pResult = INVALID_HANDLE_VALUE; // Failure
	if (pSetupDiGetClassDevsW) {
		pResult = static_cast<SetupDiGetClassDevsWPtr>(pSetupDiGetClassDevsW)(
			ClassGuid, reinterpret_cast<PCWSTR>(Enumerator), hwndParent, Flags);
	}
	return pResult;
}

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiGetDeviceInterfaceDetailA

	Manually load Setupapi.dll if needed and call the Windows function
	SetupDiGetDeviceInterfaceDetailA()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551120(v=vs.85).aspx

	\windowsonly

	\param DeviceInfoSet A pointer to the device information set that contains
		the interface for which to retrieve details.
	\param DeviceInterfaceData A pointer to an SP_DEVICE_INTERFACE_DATA
		structure that specifies the interface in DeviceInfoSet for which to
		retrieve details.
	\param DeviceInterfaceDetailData A pointer to an
		SP_DEVICE_INTERFACE_DETAIL_DATA structure to receive information about
		the specified interface.
	\param DeviceInterfaceDetailDataSize The size of the
		DeviceInterfaceDetailData buffer.
	\param RequiredSize A pointer to a variable of type Word32 that receives the
		required size of the DeviceInterfaceDetailData buffer.
	\param DeviceInfoData A pointer to a buffer that receives information about
		the device that supports the requested interface.

	\return \ref TRUE if the function completed without error.

***************************************/

Word BURGER_API Burger::Windows::SetupDiGetDeviceInterfaceDetailA(
	void* DeviceInfoSet, _SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData,
	_SP_DEVICE_INTERFACE_DETAIL_DATA_A* DeviceInterfaceDetailData,
	Word32 DeviceInterfaceDetailDataSize, Word32* RequiredSize,
	_SP_DEVINFO_DATA* DeviceInfoData)
{
	void* pSetupDiGetDeviceInterfaceDetailA =
		LoadFunctionIndex(CALL_SetupDiGetDeviceInterfaceDetailA);
	BOOL uResult = FALSE; // Failure
	if (pSetupDiGetDeviceInterfaceDetailA) {
		uResult = static_cast<SetupDiGetDeviceInterfaceDetailAPtr>(
			pSetupDiGetDeviceInterfaceDetailA)(DeviceInfoSet,
			DeviceInterfaceData, DeviceInterfaceDetailData,
			DeviceInterfaceDetailDataSize,
			reinterpret_cast<DWORD*>(RequiredSize), DeviceInfoData);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiGetDeviceInterfaceDetailW

	Manually load Setupapi.dll if needed and call the Windows function
	SetupDiGetDeviceInterfaceDetailW()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551120(v=vs.85).aspx

	\windowsonly

	\param DeviceInfoSet A pointer to the device information set that contains
		the interface for which to retrieve details.
	\param DeviceInterfaceData A pointer to an SP_DEVICE_INTERFACE_DATA
		structure that specifies the interface in DeviceInfoSet for which to
		retrieve details.
	\param DeviceInterfaceDetailData A pointer to an
		SP_DEVICE_INTERFACE_DETAIL_DATA structure to receive information about
		the specified interface.
	\param DeviceInterfaceDetailDataSize The size of the
		DeviceInterfaceDetailData buffer.
	\param RequiredSize A pointer to a variable of type Word32 that receives the
		required size of the DeviceInterfaceDetailData buffer.
	\param DeviceInfoData A pointer to a buffer that receives information about
		the device that supports the requested interface.

	\return \ref TRUE if the function completed without error.

***************************************/

Word BURGER_API Burger::Windows::SetupDiGetDeviceInterfaceDetailW(
	void* DeviceInfoSet, _SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData,
	_SP_DEVICE_INTERFACE_DETAIL_DATA_W* DeviceInterfaceDetailData,
	Word32 DeviceInterfaceDetailDataSize, Word32* RequiredSize,
	_SP_DEVINFO_DATA* DeviceInfoData)
{
	void* pSetupDiGetDeviceInterfaceDetailW =
		LoadFunctionIndex(CALL_SetupDiGetDeviceInterfaceDetailW);
	BOOL uResult = FALSE; // Failure
	if (pSetupDiGetDeviceInterfaceDetailW) {
		uResult = static_cast<SetupDiGetDeviceInterfaceDetailWPtr>(
			pSetupDiGetDeviceInterfaceDetailW)(DeviceInfoSet,
			DeviceInterfaceData, DeviceInterfaceDetailData,
			DeviceInterfaceDetailDataSize,
			reinterpret_cast<DWORD*>(RequiredSize), DeviceInfoData);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiEnumDeviceInterfaces

	Manually load Setupapi.dll if needed and call the Windows function
	SetupDiEnumDeviceInterfaces()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff551015(v=vs.85).aspx

	\windowsonly

	\param DeviceInfoSet A pointer to a device information set that contains the
		device interfaces for which to return information.
	\param DeviceInfoData A pointer to an SP_DEVINFO_DATA structure that
		specifies a device information element in DeviceInfoSet.
	\param InterfaceClassGuid A pointer to a GUID that specifies the device
		interface class for the requested interface.
	\param MemberIndex A zero-based index into the list of interfaces in the
		device information set.
	\param DeviceInterfaceData A pointer to a caller-allocated buffer that
		contains, on successful return, a completed SP_DEVICE_INTERFACE_DATA
		structure that identifies an interface that meets the search parameters.

	\return \ref TRUE if the function completed without error.

***************************************/

Word BURGER_API Burger::Windows::SetupDiEnumDeviceInterfaces(
	void* DeviceInfoSet, _SP_DEVINFO_DATA* DeviceInfoData,
	const GUID* InterfaceClassGuid, Word32 MemberIndex,
	_SP_DEVICE_INTERFACE_DATA* DeviceInterfaceData)
{
	void* pSetupDiEnumDeviceInterfaces =
		LoadFunctionIndex(CALL_SetupDiEnumDeviceInterfaces);
	BOOL uResult = FALSE; // Failure
	if (pSetupDiEnumDeviceInterfaces) {
		uResult = static_cast<SetupDiEnumDeviceInterfacesPtr>(
			pSetupDiEnumDeviceInterfaces)(DeviceInfoSet, DeviceInfoData,
			InterfaceClassGuid, MemberIndex, DeviceInterfaceData);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in setupapi.dll and call SetupDiDestroyDeviceInfoList

	Manually load Setupapi.dll if needed and call the Windows function
	SetupDiDestroyDeviceInfoList()

	http://msdn.microsoft.com/en-us/library/windows/hardware/ff550996(v=vs.85).aspx

	\windowsonly

	\param DeviceInfoSet A handle to the device information set to delete.

	\return \ref TRUE if the function completed without error.

***************************************/

Word BURGER_API Burger::Windows::SetupDiDestroyDeviceInfoList(
	void* DeviceInfoSet)
{
	void* pSetupDiDestroyDeviceInfoList =
		LoadFunctionIndex(CALL_SetupDiDestroyDeviceInfoList);
	BOOL uResult = FALSE; // Failure
	if (pSetupDiDestroyDeviceInfoList) {
		uResult = static_cast<SetupDiDestroyDeviceInfoListPtr>(
			pSetupDiDestroyDeviceInfoList)(DeviceInfoSet);
	}
	return static_cast<Word>(uResult);
}

#endif
