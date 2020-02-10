/***************************************

    Shims for d3d9.dll or d3dx9_43.dll

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

#include <d3d11.h>

typedef HRESULT(WINAPI* D3D11CreateDevicePtr)(IDXGIAdapter* pAdapter,
	D3D_DRIVER_TYPE DriverType, HMODULE Software, UINT Flags,
	CONST D3D_FEATURE_LEVEL* pFeatureLevels, UINT FeatureLevels,
	UINT SDKVersion, ID3D11Device** ppDevice, D3D_FEATURE_LEVEL* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext);
typedef HRESULT(WINAPI* CreateDXGIFactoryPtr)(REFIID riid, void** ppFactory);
typedef HRESULT(WINAPI* CreateDXGIFactory1Ptr)(REFIID riid, void** ppFactory);
typedef HRESULT(WINAPI* CreateDXGIFactory2Ptr)(
	UINT Flags, REFIID riid, void** ppFactory);

// Unit tests for pointers
// D3D11CreateDevicePtr gD3D11CreateDevice = ::D3D11CreateDevice;
// CreateDXGIFactoryPtr gCreateDXGIFactory = ::CreateDXGIFactory;
// CreateDXGIFactory1Ptr gCreateDXGIFactory1 = ::CreateDXGIFactory1;
// CreateDXGIFactory2Ptr gCreateDXGIFactory2 = ::CreateDXGIFactory2;

#endif

//
// d3d11.dll
//

/*! ************************************

	\brief Load in d3d11.dll and call D3D11CreateDevice

	To allow maximum compatibility, this function will manually load
	d3d11.dll if needed and then invoke D3D11CreateDevice.

	\windowsonly

	\param pAdapter A pointer to the video adapter to use when creating a
		device. Pass NULL to use the default adapter
	\param DriverType The D3D_DRIVER_TYPE, which represents the driver type to
		create.
	\param Software A handle to a DLL that implements a software rasterizer
	\param Flags The runtime layers to enable (see D3D11_CREATE_DEVICE_FLAG);
		values can be bitwise OR'd together.
	\param pFeatureLevels A pointer to an array of D3D_FEATURE_LEVELs, which
		determine the order of feature levels to attempt to create.
	\param FeatureLevels The number of elements in pFeatureLevels.
	\param SDKVersion The SDK version; use D3D11_SDK_VERSION.
	\param ppDevice Returns the address of a pointer to an ID3D11Device object
		that represents the device created.
	\param pFeatureLevel If successful, returns the first D3D_FEATURE_LEVEL from
		the pFeatureLevels array which succeeded
	\param ppImmediateContext Returns the address of a pointer to an
		ID3D11DeviceContext object that represents the device context

	\return S_OK if the call succeeded. Windows error if otherwise

***************************************/

Word BURGER_API Burger::Windows::D3D11CreateDevice(IDXGIAdapter* pAdapter,
	Word DriverType, HINSTANCE__* Software, Word Flags,
	const Word* pFeatureLevels, Word FeatureLevels, Word SDKVersion,
	ID3D11Device** ppDevice, Word* pFeatureLevel,
	ID3D11DeviceContext** ppImmediateContext)
{
	// Get the function pointer
	void* pD3D11CreateDevice = LoadFunctionIndex(CALL_D3D11CreateDevice);
	HRESULT uResult = DXGI_ERROR_INVALID_CALL;
	if (pD3D11CreateDevice) {
		uResult = static_cast<D3D11CreateDevicePtr>(pD3D11CreateDevice)(
			pAdapter, static_cast<D3D_DRIVER_TYPE>(DriverType), Software, Flags,
			static_cast<const D3D_FEATURE_LEVEL*>(
				static_cast<const void*>(pFeatureLevels)),
			FeatureLevels, SDKVersion, ppDevice,
			static_cast<D3D_FEATURE_LEVEL*>(static_cast<void*>(pFeatureLevel)),
			ppImmediateContext);
	}
	return static_cast<Word>(uResult);
}

//
// dxgi.dll
//

/*! ************************************

	\brief Load in dxgi.dll and call CreateDXGIFactory

	To allow maximum compatibility, this function will manually load
	dxgi.dll if needed and then invoke CreateDXGIFactory.

	\windowsonly

	\param pGuidFactory The globally unique identifier (GUID) of the
		IDXGIFactory object referenced by the ppFactory parameter
	\param ppFactory Address of a pointer to an IDXGIFactory object.

	\return S_OK if the call succeeded. Windows error if otherwise

***************************************/

Word BURGER_API Burger::Windows::CreateDXGIFactory(
	const GUID* pGuidFactory, void** ppFactory)
{
	// Get the function pointer
	void* pCreateDXGIFactory = LoadFunctionIndex(CALL_CreateDXGIFactory);
	HRESULT uResult = DXGI_ERROR_INVALID_CALL;
	if (pCreateDXGIFactory) {
		uResult = static_cast<CreateDXGIFactoryPtr>(pCreateDXGIFactory)(
			pGuidFactory[0], ppFactory);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dxgi.dll and call CreateDXGIFactory1

	To allow maximum compatibility, this function will manually load
	dxgi.dll if needed and then invoke CreateDXGIFactory1.

	\windowsonly

	\param pGuidFactory The globally unique identifier (GUID) of the
		IDXGIFactory1 object referenced by the ppFactory parameter
	\param ppFactory Address of a pointer to an IDXGIFactory1 object.

	\return S_OK if the call succeeded. Windows error if otherwise

***************************************/

Word BURGER_API Burger::Windows::CreateDXGIFactory1(
	const GUID* pGuidFactory, void** ppFactory)
{
	// Get the function pointer
	void* pCreateDXGIFactory1 = LoadFunctionIndex(CALL_CreateDXGIFactory1);
	HRESULT uResult = DXGI_ERROR_INVALID_CALL;
	if (pCreateDXGIFactory1) {
		uResult = static_cast<CreateDXGIFactory1Ptr>(pCreateDXGIFactory1)(
			pGuidFactory[0], ppFactory);
	}
	return static_cast<Word>(uResult);
}

/*! ************************************

	\brief Load in dxgi.dll and call CreateDXGIFactory2

	To allow maximum compatibility, this function will manually load
	dxgi.dll if needed and then invoke CreateDXGIFactory2.

	\windowsonly

	\param uFlags Valid values include the DXGI_CREATE_FACTORY_DEBUG (0x01)
		flag, and zero.
	\param pGuidFactory The globally unique identifier (GUID) of the
		IDXGIFactory2 object referenced by the ppFactory parameter
	\param ppFactory Address of a pointer to an IDXGIFactory2 object.

	\return S_OK if the call succeeded. Windows error if otherwise

***************************************/

Word BURGER_API Burger::Windows::CreateDXGIFactory2(
	Word uFlags, const GUID* pGuidFactory, void** ppFactory)
{
	// Get the function pointer
	void* pCreateDXGIFactory2 = LoadFunctionIndex(CALL_CreateDXGIFactory2);
	HRESULT uResult = DXGI_ERROR_INVALID_CALL;
	if (pCreateDXGIFactory2) {
		uResult = static_cast<CreateDXGIFactory2Ptr>(pCreateDXGIFactory2)(
			uFlags, pGuidFactory[0], ppFactory);
	}
	return static_cast<Word>(uResult);
}

#endif
