/***************************************

	Shims for dinput.dll and dinput8.dll

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_dinput.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brwatcom.h"
#include "win_globals.h"
#include "win_platformshims.h"

#if !defined(DOXYGEN)

//
// Handle some annoying defines that some windows SDKs may or may not have
//

#include "win_windows.h"

#include <dinput.h>

// Function prototypes, lovingly stolen from dinput.h and dinput8.h
typedef HRESULT(WINAPI* DirectInput8CreatePtr)(HINSTANCE hinst, DWORD dwVersion,
	REFIID riidltf, LPVOID* ppvOut, LPUNKNOWN punkOuter);
typedef HRESULT(WINAPI* DirectInputCreateAPtr)(HINSTANCE hinst, DWORD dwVersion,
	LPDIRECTINPUTA* ppDI, LPUNKNOWN punkOuter);
typedef HRESULT(WINAPI* DirectInputCreateWPtr)(HINSTANCE hinst, DWORD dwVersion,
	LPDIRECTINPUTW* ppDI, LPUNKNOWN punkOuter);

/***************************************

	Unit tests for pointers, uncomment the values and see if there are compiler
	errors If they compile fine, the prototypes above are a match for the
	official headers

	Note: DirectInput only declares DInput8 or DInput but not both at the same
	time.

***************************************/

// DirectInput8CreatePtr gDirectInput8Create = ::DirectInput8Create;
// DirectInputCreateAPtr gDirectInputCreateA = ::DirectInputCreateA;
// DirectInputCreateWPtr gDirectInputCreateW = ::DirectInputCreateW;

// Singleton DirectInputW instance
static IDirectInputW* s_pDirectInputW;

// Singleton DirectInput8W instance
static IDirectInput8W* s_pDirectInput8W;

// Set to TRUE if atexit is called for release_input
static uint8_t s_bAtExit;

/*! ************************************

	\brief Release DirectInput singleton

	On application exit, release the DirectInput singletons.

	\windowsonly

***************************************/

static void BURGER_ANSIAPI release_dinput(void) BURGER_NOEXCEPT
{
	// Was DirectInput8 instantiated?
	if (s_pDirectInput8W) {
		s_pDirectInput8W->Release();
		s_pDirectInput8W = nullptr;
	}

	// Was DirectInput instantiated?
	if (s_pDirectInputW) {
		s_pDirectInputW->Release();
		s_pDirectInputW = nullptr;
	}
}

// A fix for atexit() callback warning below
#if defined(BURGER_WATCOM)
// converted function type has different #pragma from original function type
#pragma warning 601 9
#endif

/*! ************************************

	\brief Check if disposal function was added to atexit()

	If a DirectInput singleton was created, this function is called to ensure a
	disposal function is added to atexit()

	\windowsonly

***************************************/

static void add_release(void)
{
	// Was it not installed
	if (!s_bAtExit) {

		// Install it
		s_bAtExit = TRUE;
		atexit(release_dinput);
	}
}

#endif

/*! ************************************

	\brief Load in dinput.dll and call DirectInputCreateA

	To allow maximum compatibility, this function will manually load
	dinput.dll and then invoke DirectInputCreateA if present.

	\windowsonly

	\param hInst Instance of the application
	\param uVersion Requested version of DirectInput
	\param ppOutput Pointer to the location where the valid IDirectInputA
		pointer will be stored. If the function fails, this value is
		guaranteed to be \ref nullptr.
	\param pOuter Pointer to the IUnknown passed to DirectInputCreateA()

	\return Zero if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectInputCreateA(HINSTANCE__* hInst,
	uint32_t uVersion, IDirectInputA** ppOutput,
	IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectInputCreateA = load_function(kCall_DirectInputCreateA);
	HRESULT lResult = DIERR_NOTFOUND;
	if (pDirectInputCreateA) {
		lResult = static_cast<DirectInputCreateAPtr>(pDirectInputCreateA)(
			hInst, uVersion, ppOutput, pOuter);
	}
	return lResult;
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
		to be \ref nullptr.
	\param pOuter Pointer to the IUnknown passed to DirectInputCreateW()

	\return Zero if no error. Any other value means an error occurred

***************************************/

HRESULT BURGER_API Burger::Win32::DirectInputCreateW(HINSTANCE__* hInst,
	uint32_t uVersion, IDirectInputW** ppOutput,
	IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectInputCreateW = load_function(kCall_DirectInputCreateW);
	HRESULT lResult = DIERR_NOTFOUND;
	if (pDirectInputCreateW) {
		lResult = static_cast<DirectInputCreateWPtr>(pDirectInputCreateW)(
			hInst, uVersion, ppOutput, pOuter);
	}
	return lResult;
}

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

HRESULT BURGER_API Burger::Win32::DirectInput8Create(HINSTANCE__* hInst,
	uint32_t uVersion, const GUID& rGUID, void** ppOutput,
	IUnknown* pOuter) BURGER_NOEXCEPT
{
	// Clear in case of error
	if (ppOutput) {
		ppOutput[0] = nullptr;
	}

	// Get the function pointer
	void* pDirectInput8Create = load_function(kCall_DirectInput8Create);
	HRESULT lResult = DIERR_NOTFOUND;
	if (pDirectInput8Create) {
		lResult = static_cast<DirectInput8CreatePtr>(pDirectInput8Create)(
			hInst, uVersion, rGUID, ppOutput, pOuter);
	}
	return lResult;
}

/*! ************************************

	\brief Return the singleton IDirectInputW pointer used by Burgerlib

	To allow multiple classes to use IDirectInputW without having to
	check if it was already created, use this function to create a
	global instance of IDirectInputW that will be released only
	on program exit.

	If DirectInput8 is required, use get_DirectInput8_singleton() instead.

	\windowsonly

	\param uVersion Version of DirectInput requested. Usually this is 0x700
		(Highest number before DirectInput8 is invoked)

	\return Pointer to the global IDirectInputW pointer or \ref nullptr if
		DirectInput couldn't be started.

***************************************/

IDirectInputW* BURGER_API Burger::Win32::get_DirectInput_singleton(
	uint32_t uVersion) BURGER_NOEXCEPT
{
	// Was there already a DirectInput instance?
	IDirectInputW* pDirectInputW = s_pDirectInputW;
	if (!pDirectInputW) {

		add_release();

		// Get DirectInput
		DirectInputCreateW(get_instance(), uVersion, &s_pDirectInputW);

		// Get the updated pointer (Or nullptr)
		pDirectInputW = s_pDirectInputW;
	}
	return pDirectInputW;
}

/*! ************************************

	\brief Return the singleton IDirectInput8W pointer used by Burgerlib

	To allow multiple classes to use IDirectInput8W without having to
	check if it was already created, use this function to create a
	global instance of IDirectInput8W that will be released only
	on program exit.

	If DirectInput is required, use get_DirectInput_singleton() instead.

	\windowsonly

	\return Pointer to the global IDirectInput8W pointer or \ref nullptr if
		DirectInput8 couldn't be started.

***************************************/

IDirectInput8W* BURGER_API Burger::Win32::get_DirectInput8_singleton(
	void) BURGER_NOEXCEPT
{
	// Was there already a DirectInput instance?
	IDirectInput8W* pDirectInput8W = s_pDirectInput8W;
	if (!pDirectInput8W) {

		add_release();

		// Get DirectInput 8
		DirectInput8Create(get_instance(), 0x800, IID_IDirectInput8W,
			reinterpret_cast<void**>(&s_pDirectInput8W));

		// Get the updated pointer (Or nullptr)
		pDirectInput8W = s_pDirectInput8W;
	}
	return pDirectInput8W;
}

#endif
