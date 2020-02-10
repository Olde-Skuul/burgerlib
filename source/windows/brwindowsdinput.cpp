/***************************************

    Shims for dinput.dll and dinput8.dll

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brwindowstypes.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brglobals.h"

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

#if !defined(DIRECTINPUT_VERSION)
#define DIRECTINPUT_VERSION 0x700
#endif

#include <Windows.h>

#include <dinput.h>

typedef HRESULT(WINAPI* DirectInput8CreatePtr)(HINSTANCE hinst, DWORD dwVersion,
	REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);
typedef HRESULT(WINAPI* DirectInputCreateAPtr)(HINSTANCE hinst, DWORD dwVersion,
	LPDIRECTINPUTA* ppDI, LPUNKNOWN punkOuter);
typedef HRESULT(WINAPI* DirectInputCreateWPtr)(HINSTANCE hinst, DWORD dwVersion,
	LPDIRECTINPUTW* ppDI, LPUNKNOWN punkOuter);

// Unit tests for pointers
// Note: DirectInput only declares DInput8 or DInput but not both at the same
// time.

// DirectInput8CreatePtr gDirectInput8Create = ::DirectInput8Create;
// DirectInputCreateAPtr gDirectInputCreateA = ::DirectInputCreateA;
// DirectInputCreateWPtr gDirectInputCreateW = ::DirectInputCreateW;

#endif

/*! ************************************

	\brief Load in dinput.dll and call DirectInputCreateA

	To allow maximum compatibility, this function will manually load dinput.dll
	and then invoke DirectInputCreateA if present.

	\windowsonly

	\param hInst Instance of the application
	\param uVersion Requested version of DirectInput
	\param ppOutput Pointer to the location where the valid IDirectInputA
		pointer will be stored. If the function fails, this value is guaranteed
		to be \ref NULL.
	\param pOuter Pointer to the IUnknown passed to DirectInputCreateA()

	\return Zero if no error. Any other value means an error occurred

***************************************/

Word BURGER_API Burger::Windows::DirectInputCreateA(HINSTANCE__* hInst,
	Word32 uVersion, IDirectInputA** ppOutput, IUnknown* pOuter)
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectInputCreateA = LoadFunctionIndex(CALL_DirectInputCreateA);
	HRESULT uResult = DIERR_NOTFOUND;
	if (pDirectInputCreateA) {
		uResult = static_cast<DirectInputCreateAPtr>(pDirectInputCreateA)(
			hInst, uVersion, ppOutput, pOuter);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dinput.dll and call DirectInputCreateW

	To allow maximum compatibility, this function will manually load
	dinput.dll and then invoke DirectInputCreateA if present.

	\windowsonly

	\param hInst Instance of the application
	\param uVersion Requested version of DirectInput
	\param ppOutput Pointer to the location where the valid IDirectInputA
		pointer will be stored. If the function fails, this value is guaranteed
		to be \ref NULL.
	\param pOuter Pointer to the IUnknown passed to DirectInputCreateW()

	\return Zero if no error. Any other value means an error occurred

***************************************/

Word BURGER_API Burger::Windows::DirectInputCreateW(HINSTANCE__* hInst,
	Word32 uVersion, IDirectInputW** ppOutput, IUnknown* pOuter)
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectInputCreateW = LoadFunctionIndex(CALL_DirectInputCreateW);
	HRESULT uResult = DIERR_NOTFOUND;
	if (pDirectInputCreateW) {
		uResult = static_cast<DirectInputCreateWPtr>(pDirectInputCreateW)(
			hInst, uVersion, ppOutput, pOuter);
	}
	return static_cast<Word>(uResult);
}

//
// dinput8.dll
//

/*! ************************************

    \brief Load in dinput8.dll and call DirectInput8Create

    To allow maximum compatibility, this function will manually load
    dinput8.dll and then invoke DirectInput8Create if present.

    \windowsonly

    \param hInst Instance of the application
    \param uVersion Requested version of DirectInput8
    \param rGUID Reference to the GUID of the DirectInput8 object to create
    \param ppOutput Pointer to the location where the valid IDirectInput8
        pointer will be stored. If the function fails, this value is guaranteed
        to be \ref nullptr.
    \param pOuter Pointer to the IUnknown passed to DirectInput8Create()

    \return Zero if no error. Any other value means an error occurred

***************************************/

Word BURGER_API Burger::Windows::DirectInput8Create(HINSTANCE__* hInst,
	Word32 uVersion, const GUID& rGUID, void** ppOutput, IUnknown* pOuter)
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectInput8Create = LoadFunctionIndex(CALL_DirectInput8Create);
	HRESULT uResult = DIERR_NOTFOUND;
	if (pDirectInput8Create) {
		uResult = static_cast<DirectInput8CreatePtr>(pDirectInput8Create)(
			hInst, uVersion, rGUID, ppOutput, pOuter);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Return the singleton IDirectInputW pointer used by Burgerlib

	To allow multiple classes to use IDirectInputW without having to
	check if it was already created, use this function to create a
	global instance of IDirectInputW that will be released only
	on program exit.

	If DirectInput8 is required, use GetDirectInput8Singleton() instead.

	\windowsonly

	\param uVersion Version of DirectInput requested. Usually this is 0x700
		(Highest number before DirectInput8 is invoked)

	\return Pointer to the global IDirectInputW pointer or \ref NULL if
		DirectInput couldn't be started.

***************************************/

IDirectInputW* BURGER_API Burger::Windows::GetDirectInputSingleton(
	Word uVersion)
{
	// Was there already a DirectInput instance?
	IDirectInputW* pDirectInputW = g_Globals.m_pDirectInputW;
	if (!pDirectInputW) {
		// Get DirectInput
		DirectInputCreateW(GetInstance(), uVersion, &g_Globals.m_pDirectInputW);
		// Get the updated pointer (Or NULL)
		pDirectInputW = g_Globals.m_pDirectInputW;
	}
	return pDirectInputW;
}

/*! ************************************

	\brief Return the singleton IDirectInput8W pointer used by Burgerlib

	To allow multiple classes to use IDirectInput8W without having to
	check if it was already created, use this function to create a
	global instance of IDirectInput8W that will be released only
	on program exit.

	If DirectInput is required, use GetDirectInputSingleton() instead.

	\windowsonly

	\return Pointer to the global IDirectInput8W pointer or \ref NULL if
		DirectInput8 couldn't be started.

***************************************/

IDirectInput8W* BURGER_API Burger::Windows::GetDirectInput8Singleton(void)
{
	// Was there already a DirectInput instance?
	IDirectInput8W* pDirectInput8W = g_Globals.m_pDirectInput8W;
	if (!pDirectInput8W) {
		// Get DirectInput 8
		DirectInput8Create(GetInstance(), 0x800, IID_IDirectInput8W,
			reinterpret_cast<void**>(&g_Globals.m_pDirectInput8W));
		// Get the updated pointer (Or NULL)
		pDirectInput8W = g_Globals.m_pDirectInput8W;
	}
	return pDirectInput8W;
}

#endif
