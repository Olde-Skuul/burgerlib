/***************************************

	Library loader functions specific to Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_loadlibrary.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#include "win_windows.h"

/*! ************************************

	\brief Call LoadLibraryA() without file error boxes

	When LoadLibraryA() is called in windows, it's possible that if the file is
	not found, windows will display an error message box mentioning that a DLL
	is missing. This function will prohibit this behavior by setting the
	ErrorMode to SEM_NOOPENFILEERRORBOX before the call to LoadLibraryA() and
	restoring the flag to the previous setting before function exit.

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms684175(v=vs.85).aspx

	\windowsonly

	\param pInput ASCII pathname of the DLL file to load.

	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success

	\sa LoadLibraryExA() or LoadLibraryW()

***************************************/

HINSTANCE BURGER_API Burger::Win32::LoadLibraryA(
	const char* pInput) BURGER_NOEXCEPT
{
	// Disable user interactive dialogs
	UINT uOldMode =
		SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	HINSTANCE hResult = ::LoadLibraryA(pInput);

	// Restore the dialog state
	SetErrorMode(uOldMode);
	return hResult;
}

/*! ************************************

	\brief Call LoadLibraryW() without file error boxes

	When LoadLibraryW() is called in windows, it's possible that if the file is
	not found, windows will display an error message box mentioning that a DLL
	is missing. This function will prohibit this behavior by setting the
	ErrorMode to SEM_NOOPENFILEERRORBOX before the call to LoadLibraryW() and
	restoring the flag to the previous setting before function exit.

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms684175(v=vs.85).aspx

	\windowsonly

	\param pInput UTF16 pathname of the DLL file to load.

	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success

	\sa LoadLibraryExW() or LoadLibraryA()

***************************************/

HINSTANCE BURGER_API Burger::Win32::LoadLibraryW(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	// Disable user interactive dialogs
	UINT uOldMode =
		SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	HINSTANCE hResult = ::LoadLibraryW(reinterpret_cast<LPCWSTR>(pInput));

	// Restore the dialog state
	SetErrorMode(uOldMode);
	return hResult;
}

/*! ************************************

	\brief Call LoadLibraryExA() without file error boxes

	When LoadLibraryExA() is called in windows, it's possible that if the file
	is not found, windows will display an error message box mentioning that a
	DLL is missing. This function will prohibit this behavior by setting the
	ErrorMode to SEM_NOOPENFILEERRORBOX before the call to LoadLibraryExA() and
	restoring the flag to the previous setting before function exit.

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms684179(v=vs.85).aspx

	\windowsonly

	\param pInput ASCII pathname of the DLL file to load.
	\param hFile This parameter is reserved for future use. It must be \ref
		nullptr.
	\param uFlags The action to be taken when loading the module.

	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success

	\sa LoadLibraryA() or LoadLibraryExW()

***************************************/

HINSTANCE BURGER_API Burger::Win32::LoadLibraryExA(
	const char* pInput, void* hFile, uint32_t uFlags) BURGER_NOEXCEPT
{
	// Disable user interactive dialogs
	UINT uOldMode =
		SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	HINSTANCE hResult = ::LoadLibraryExA(pInput, hFile, uFlags);

	// Restore the dialog state
	SetErrorMode(uOldMode);
	return hResult;
}

/*! ************************************

	\brief Call LoadLibraryExW() without file error boxes

	When LoadLibraryExW() is called in windows, it's possible that if the file
	is not found, windows will display an error message box mentioning that a
	DLL is missing. This function will prohibit this behavior by setting the
	ErrorMode to SEM_NOOPENFILEERRORBOX before the call to LoadLibraryExW() and
	restoring the flag to the previous setting before function exit.

	https://msdn.microsoft.com/en-us/library/windows/desktop/ms684179(v=vs.85).aspx

	\windowsonly

	\param pInput UTF16 pathname of the DLL file to load.
	\param hFile This parameter is reserved for future use. It must be \ref
		NULL.
	\param uFlags The action to be taken when loading the module.

	\return \ref NULL if the DLL was not loaded, a valid HINSTANCE on success

	\sa LoadLibraryExA() or LoadLibraryW()

***************************************/

HINSTANCE BURGER_API Burger::Win32::LoadLibraryExW(
	const uint16_t* pInput, void* hFile, uint32_t uFlags) BURGER_NOEXCEPT
{
	// Disable user interactive dialogs
	UINT uOldMode =
		SetErrorMode(SEM_NOOPENFILEERRORBOX | SEM_FAILCRITICALERRORS);

	HINSTANCE hResult =
		::LoadLibraryExW(reinterpret_cast<LPCWSTR>(pInput), hFile, uFlags);

	// Restore the dialog state
	SetErrorMode(uOldMode);
	return hResult;
}

#endif
