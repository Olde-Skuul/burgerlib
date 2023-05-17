/***************************************

	DirectX 9 manager class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplaydirectx9.h"
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brgameapp.h"
#include "brglobals.h"
#include "brmatrix4d.h"
#include "brmemoryfunctions.h"
#include "brstring16.h"
#include "brtexturedirectx9.h"
#include "brvertexbufferdirectx9.h"
#include "brwindowstypes.h"
#include "win_d3d9.h"
#include "win_user32.h"
#include "win_winutils.h"

#if !defined(DIRECTDRAW_VERSION) && !defined(DOXYGEN)
#define DIRECTDRAW_VERSION 0x700
#endif

#if !defined(DIRECT3D_VERSION) && !defined(DOXYGEN)
#define DIRECT3D_VERSION 0x900
#endif

#if !defined(WIN32_LEAN_AND_MEAN) && !defined(DOXYGEN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>
#include <d3d9.h>
#include <d3dx9math.h>
#include <ddraw.h>

#ifdef _DEBUG
#define PRINTHRESULT(hResult) /* m_pGameApp->Poll(); */ \
	if (hResult != D3D_OK) \
	Debug::Message( \
		"Error at line " BURGER_STRINGIZE(__LINE__) " with 0x%08X\n", hResult)
#else
#define PRINTHRESULT(hResult)
#endif

//
// Data primitive types
//

#if !defined(DOXYGEN)

static const D3DPRIMITIVETYPE g_Prims[] = {
	D3DPT_POINTLIST,     // PRIM_POINTS
	D3DPT_LINELIST,      // PRIM_LINES,
	D3DPT_LINESTRIP,     // PRIM_LINESTRIP,
	D3DPT_TRIANGLELIST,  // PRIM_TRIANGLES,
	D3DPT_TRIANGLESTRIP, // PRIM_TRIANGLESTRIP,
	D3DPT_TRIANGLEFAN    // PRIM_TRIANGLEFAN
};

static const DWORD g_Wrapping[] = {
	D3DTADDRESS_WRAP, // WRAP_REPEAT
	D3DTADDRESS_CLAMP // WRAP_CLAMP
};

static const DWORD g_Filter[] = {
	D3DTEXF_POINT, // FILTER_NEAREST,
	D3DTEXF_LINEAR // FILTER_LINEAR
};

static const DWORD g_SourceBlend[] = {
	D3DBLEND_ZERO,         // SRCBLEND_ZERO
	D3DBLEND_ONE,          // SRCBLEND_ONE
	D3DBLEND_SRCCOLOR,     // SRCBLEND_COLOR
	D3DBLEND_INVSRCCOLOR,  // SRCBLEND_ONE_MINUS_COLOR
	D3DBLEND_SRCALPHA,     // SRCBLEND_SRC_ALPHA
	D3DBLEND_INVSRCALPHA,  // SRCBLEND_ONE_MINUS_SRC_ALPHA
	D3DBLEND_DESTALPHA,    // SRCBLEND_DST_ALPHA,
	D3DBLEND_INVDESTALPHA, // SRCBLEND_ONE_MINUS_DST_ALPHA
	D3DBLEND_SRCALPHASAT   // SRCBLEND_SRC_ALPHA_SATURATE
};

static const DWORD g_DestBlend[] = {
	D3DBLEND_ZERO,         // DSTBLEND_ZERO
	D3DBLEND_ONE,          // DSTBLEND_ONE
	D3DBLEND_DESTCOLOR,    // DSTBLEND_COLOR
	D3DBLEND_INVDESTCOLOR, // DSTBLEND_ONE_MINUS_COLOR
	D3DBLEND_DESTALPHA,    // DSTBLEND_DST_ALPHA
	D3DBLEND_INVDESTALPHA, // DSTBLEND_ONE_MINUS_DST_ALPHA
	D3DBLEND_SRCALPHA,     // DSTBLEND_SRC_ALPHA
	D3DBLEND_INVSRCALPHA   // DSTBLEND_ONE_MINUS_SRC_ALPHA
};

static const DWORD g_WriteFunction[] = {
	D3DCMP_NEVER,        // DEPTHCMP_NEVER
	D3DCMP_LESS,         // DEPTHCMP_LESS
	D3DCMP_EQUAL,        // DEPTHCMP_EQUAL
	D3DCMP_LESSEQUAL,    // DEPTHCMP_LESSEQUAL
	D3DCMP_GREATER,      // DEPTHCMP_GREATER
	D3DCMP_NOTEQUAL,     // DEPTHCMP_NOTEQUAL
	D3DCMP_GREATEREQUAL, // DEPTHCMP_GREATEREQUAL
	D3DCMP_ALWAYS        // DEPTHCMP_ALWAYS
};

static const DWORD g_CullOperation[] = {
	D3DCULL_NONE, // CULL_NONE
	D3DCULL_CW,   // CULL_CLOCKWISE
	D3DCULL_CCW   // CULL_COUNTERCLOCKWISE
};

//
// This table is all the pixel formats for the actual screen sent
// to the display. These are all the known (As of 2016) of the
// formats that one can expect to obtain for drawing to the screen.
//
// Rest in Peace 8 bit paletted.
//

static const D3DFORMAT g_ValidAdapterFormats[] = {
	D3DFMT_X8R8G8B8,   // 32 bit R,G,B (8:8:8)
	D3DFMT_X1R5G5B5,   // 15 bit R,G,B (5:5:5)
	D3DFMT_R5G6B5,     // 16 bit R,G,B (5:6:5)
	D3DFMT_A2R10G10B10 // 32 bit A,R,G,B (10:10:10)
};

static const D3DDEVTYPE g_DeviceTypes[] = {
	D3DDEVTYPE_HAL, // Hardware abstracted device driver (Best)
	D3DDEVTYPE_SW,  // Software driver (Optimized with SSE2 or better)
	D3DDEVTYPE_REF  // Debug mode driver (Slow, and full of asserts for
					// debugging)
};

//
// Pixel formats for the back buffer to store the final
// image before presenting it to the display screen
//
// Rest in Peace 8 bit back buffer
//

static const D3DFORMAT g_BackBufferFormats[] = {
	D3DFMT_A8R8G8B8,    // 8:8:8:8
	D3DFMT_X8R8G8B8,    // X:8:8:8
	D3DFMT_A2R10G10B10, // 2:10:10:10
	D3DFMT_R5G6B5,      // 5:6:5
	D3DFMT_A1R5G5B5,    // 1:5:5:5
	D3DFMT_X1R5G5B5     // 5:5:5
};

#endif

/*! ************************************

	\struct Burger::DisplayDirectX9::DisplayMode_t
	\brief Structure to describe a DirectX 9 display mode

	This is a duplicate of the D3DDISPLAYMODE structure found in DirectX 9.

	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172548(v=vs.85).aspx

	\windowsonly

	\sa AdapterInfo

***************************************/

/*! ************************************

	\struct Burger::DisplayDirectX9::DeviceSettings_t
	\brief Structure to describe the state of a Direct3D9 device

	This is a duplicate of the D3DPRESENT_PARAMETERS structure found in DirectX
9 with some extra data for internal use by \ref DisplayDirectX9 for monitor /
adapter selection.

	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172588(v=vs.85).aspx

	\windowsonly

	\sa DisplayDirectX9

***************************************/

/*! ************************************

	\brief Convert a D3DPRESENT_PARAMETERS to a DeviceSettings_t

	Extract all of the values stored in a D3DPRESENT_PARAMETERS
	structure and copy them into this structure.

	\param pInput Pointer to a D3DPRESENT_PARAMETERS to copy from.
	\sa GetPresentParameters(D3DPRESENT_PARAMETERS *) const

***************************************/

void BURGER_API Burger::DisplayDirectX9::DeviceSettings_t::SetPresentParameters(
	const D3DPRESENT_PARAMETERS* pInput)
{
	m_uBackBufferWidth = pInput->BackBufferWidth;
	m_uBackBufferHeight = pInput->BackBufferHeight;
	m_uBackBufferFormat = pInput->BackBufferFormat;
	m_uBackBufferCount = pInput->BackBufferCount;
	m_uMultiSampleType = pInput->MultiSampleType;
	m_uMultiSampleQuality = pInput->MultiSampleQuality;

	m_uSwapEffect = pInput->SwapEffect;
	m_pDeviceWindow = pInput->hDeviceWindow;

	// Ensure these are boolean values
	m_bWindowed = pInput->Windowed != 0;
	m_bEnableAutoDepthStencil = pInput->EnableAutoDepthStencil != 0;

	m_uAutoDepthStencilFormat = pInput->AutoDepthStencilFormat;
	m_uFlags = pInput->Flags;
	m_uFullScreen_RefreshRateInHz = pInput->FullScreen_RefreshRateInHz;
	m_uPresentationInterval = pInput->PresentationInterval;
}

/*! ************************************

	\brief Convert a DeviceSettings_t to a D3DPRESENT_PARAMETERS

	Using values in this structure, completely fill in a D3DPRESENT_PARAMETERS
	structure for use in DirectX 9

	\param pOutput Pointer to a D3DPRESENT_PARAMETERS to fill in
	\sa SetPresentParameters(const D3DPRESENT_PARAMETERS *)

***************************************/

void BURGER_API Burger::DisplayDirectX9::DeviceSettings_t::GetPresentParameters(
	D3DPRESENT_PARAMETERS* pOutput) const
{
	pOutput->BackBufferWidth = m_uBackBufferWidth;
	pOutput->BackBufferHeight = m_uBackBufferHeight;
	pOutput->BackBufferFormat = static_cast<D3DFORMAT>(m_uBackBufferFormat);
	pOutput->BackBufferCount = m_uBackBufferCount;

	pOutput->MultiSampleType =
		static_cast<D3DMULTISAMPLE_TYPE>(m_uMultiSampleType);
	pOutput->MultiSampleQuality = m_uMultiSampleQuality;

	pOutput->SwapEffect = static_cast<D3DSWAPEFFECT>(m_uSwapEffect);
	pOutput->hDeviceWindow = m_pDeviceWindow;

	// These two values are always either 0 or 1
	pOutput->Windowed = static_cast<BOOL>(m_bWindowed);
	pOutput->EnableAutoDepthStencil =
		static_cast<BOOL>(m_bEnableAutoDepthStencil);

	pOutput->AutoDepthStencilFormat =
		static_cast<D3DFORMAT>(m_uAutoDepthStencilFormat);
	pOutput->Flags = m_uFlags;
	pOutput->FullScreen_RefreshRateInHz = m_uFullScreen_RefreshRateInHz;
	pOutput->PresentationInterval = m_uPresentationInterval;
}

/*! ************************************

	\fn uint_t
Burger::DisplayDirectX9::DeviceSettings_t::IsMSAASwapChainCreated(void) const
	\brief Test if Anti-Aliasing is enabled

	\return \ref TRUE if Anti-aliasing is turned on
	\sa IsVSyncEnabled(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::DisplayDirectX9::DeviceSettings_t::IsVSyncEnabled(void)
const \brief Test if vertical sync is enabled

	\return \ref TRUE if vertical sync is turned on
	\sa IsMSAASwapChainCreated(void) const

***************************************/

/*! ************************************

	\brief Set up the defaults for a DirectX 9 display

	Settings for default are as follows:
	* * Width 640
	* * Height 480
	* * Back buffer 8:8:8 RGB (32 bit with no alpha)
	* * Default refresh rate
	* * No Anti-Aliasing
	* * 24 bit depth buffer
	* * 8 bit stencil
	* * Windowed mode
	* * Focus window (\ref NULL)
	* * Immediate display mode (No VSync)

***************************************/

void BURGER_API Burger::DisplayDirectX9::DeviceSettings_t::ApplyDefaults(void)
{
	// Unknown window
	m_pDeviceWindow = NULL;
	m_uAdapterOrdinal = 0;              // Default adapter
	m_uDeviceType = D3DDEVTYPE_HAL;
	m_uAdapterFormat = D3DFMT_X8R8G8B8; // 32 bit color (Pretty much the only
										// one supported now)

	m_uBehaviorFlags =
		D3DCREATE_HARDWARE_VERTEXPROCESSING | D3DCREATE_FPU_PRESERVE;
	m_uBackBufferWidth = 640;
	m_uBackBufferHeight = 480;
	m_uBackBufferFormat = D3DFMT_X8R8G8B8;
	m_uBackBufferCount = 1;
	m_uMultiSampleType = D3DMULTISAMPLE_NONE;
	m_uMultiSampleQuality = 0;
	m_uSwapEffect = D3DSWAPEFFECT_DISCARD;
	m_uAutoDepthStencilFormat = D3DFMT_D24X8;
	m_uFlags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	m_uFullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_uPresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
	m_bWindowed = TRUE;
	m_bEnableAutoDepthStencil = TRUE;
}

/*! ************************************

	\class Burger::DisplayDirectX9::DeviceInfo
	\brief Information for a GPU device

	For each rendering device, this class contains a list of every
	supported pixel format for rendering. Devices are
	hardware, software, or reference. A copy of
	the D3DCAPS9 for the rendering device is also stored in this class

	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172513(v=vs.85).aspx

	\sa BufferFormatGroup or AdapterInfo

***************************************/

/*! ************************************

	\brief Create a default DeviceInfo

	Zero out the D3DCAPS9 record and the list and set
	the description defaults to the passed in values.

	\param uAdapterOrdinal Ordinal value for this adapter from DirectX 9
	\param uDeviceType Device type of adapter (Hardware/Software)

***************************************/

Burger::DisplayDirectX9::DeviceInfo::DeviceInfo(
	uint_t uAdapterOrdinal, uint_t uDeviceType):
	m_uAdapterOrdinal(uAdapterOrdinal),
	m_uDeviceType(uDeviceType),
	m_BufferFormatList()
{
	BURGER_STATIC_ASSERT(sizeof(m_D3DCaps) == sizeof(D3DCAPS9));
	MemoryClear(&m_D3DCaps, sizeof(m_D3DCaps));
}

/*! ************************************

	\brief Dispose of a DeviceInfo

	All BufferFormatGroup pointers added to this class are
	disposed of in the destructor.

***************************************/

Burger::DisplayDirectX9::DeviceInfo::~DeviceInfo()
{
	// Every entry in this list is under this class' control.

	// Dispose of every entry in the list.
	uintptr_t uCount = m_BufferFormatList.size();
	if (uCount) {
		BufferFormatGroup* const* ppBufferFormatGroup =
			m_BufferFormatList.GetPtr();
		do {
			Delete(ppBufferFormatGroup[0]);
			++ppBufferFormatGroup;
		} while (--uCount);
	}
}

/*! ************************************

	\fn uint_t Burger::DisplayDirectX9::DeviceInfo::GetAdapterOrdinal(void)
const \brief Get the adapter ordinal

	\return The adapter ordinal for which adapter this class is describing.

***************************************/

/*! ************************************

	\fn uint_t Burger::DisplayDirectX9::DeviceInfo::GetDeviceType(void) const
	\brief Get the adapter device type

	\return The adapter device type for which adapter this class is describing.

***************************************/

/*! ************************************

	\fn const _D3DCAPS9* Burger::DisplayDirectX9::DeviceInfo::GetCaps(void)
const \brief Obtain a pointer to the contained D3DCAPS9 structure

	Use this accessor to query the D3DCAPS9 for this specific rendering device.

	\note This is a read-only pointer by design. The contents are managed
internally. \return Read-only pointer to the D3DCAP9 data in this structure.

***************************************/

/*! ************************************

	\fn uintptr_t Burger::DisplayDirectX9::DeviceInfo::GetBufferListSize(void)
const \brief Get number of BufferFormatGroup entries stored in this class

	\return The size of the BufferFormatGroup list in entries.
	\sa GetBufferList(void) const

***************************************/

/*! ************************************

	\fn BufferFormatGroup * const *
Burger::DisplayDirectX9::DeviceInfo::GetBufferList(void) const \brief Get the
pointer to the base of the BufferFormatGroup * array

	\return A pointer to an array of BufferFormatGroup * entries
	\sa GetBufferListSize(void) const

***************************************/

/*! ************************************

	\brief Add a BufferFormatGroup * to the list

	Add the passed in pointer to the end of an internal list of entries
	that belong to this adapter.

	\param pBufferFormatGroup A pointer to a BufferFormatGroup * entry

***************************************/

void BURGER_API Burger::DisplayDirectX9::DeviceInfo::AddToList(
	BufferFormatGroup* pBufferFormatGroup)
{
	// Add to the end of the list
	m_BufferFormatList.push_back(pBufferFormatGroup);
}

/*! ************************************

	\brief Search for a BufferFormatGroup

	Given an adapter driver format, a back buffer format and if it's windowed,
locate the BufferFormatGroup that matches the criteria.

	\param uAdapterFormat Hardware/Software format
	\param uBackBufferFormat D3DFORMAT of the back buffer
	\param bWindowed \ref TRUE if in a window, \ref FALSE if full screen.
	\return A pointer to a BufferFormatGroup * entry or \ref NULL if not found

***************************************/

const Burger::DisplayDirectX9::BufferFormatGroup* BURGER_API
Burger::DisplayDirectX9::DeviceInfo::Find(
	uint_t uAdapterFormat, uint_t uBackBufferFormat, uint_t bWindowed) const
{
	const BufferFormatGroup* pResult = NULL;
	uintptr_t uCount = m_BufferFormatList.size();
	if (uCount) {
		BufferFormatGroup* const* ppList = m_BufferFormatList.GetPtr();
		do {
			const BufferFormatGroup* pBufferFormatGroup = ppList[0];
			++ppList;
			if ((pBufferFormatGroup->GetAdapterFormat() == uAdapterFormat) &&
				(pBufferFormatGroup->GetBackBufferFormat() ==
					uBackBufferFormat) &&
				(pBufferFormatGroup->IsWindowed() == bWindowed)) {
				pResult = pBufferFormatGroup;
				break;
			}
		} while (--uCount);
	}
	return pResult;
}

/*! ************************************

	\class Burger::DisplayDirectX9::AdapterInfo
	\brief Information for a display device

	For each display device, this class contains a list of every
	supported pixel resolution and a array of DeviceInfo classes
	describing every GPU/software renderer that is attached to it.
	Contained in this class is a copy of the D3DADAPTER_IDENTIFIER9
	structure obtained from the adapter.

	https://msdn.microsoft.com/en-us/library/windows/desktop/bb172505(v=vs.85).aspx

	\sa DeviceInfo or BufferFormatGroup

***************************************/

/*! ************************************

	\brief Create a default AdapterInfo

	All lists are empty on creation

	\param uAdapterOrdinal Adapter ordinal index

***************************************/

Burger::DisplayDirectX9::AdapterInfo::AdapterInfo(uint_t uAdapterOrdinal):
	m_DisplayModeList(), m_DeviceInfoList(), m_uAdapterOrdinal(uAdapterOrdinal)
{
	// Note: On 32 bit platforms, the structure is 275 32 bit words long where
	// on 64 bit machines it's 276 32 bits words long due to the inclusion of
	// the 64 bit entry DriverVersion which forces the structure to be 8 byte
	// aligned.

	// This is why the test below is >= instead of ==

	BURGER_STATIC_ASSERT(
		sizeof(m_AdapterIdentifier) >= sizeof(D3DADAPTER_IDENTIFIER9));

	MemoryClear(&m_AdapterIdentifier, sizeof(m_AdapterIdentifier));
}

/*! ************************************

	\brief Dispose of an AdapterInfo class

***************************************/

Burger::DisplayDirectX9::AdapterInfo::~AdapterInfo()
{
	// Every entry in this list is under this class' control.

	// Dispose of every entry in the list.
	uintptr_t uCount = m_DeviceInfoList.size();
	if (uCount) {
		DeviceInfo* const* ppDeviceInfo = m_DeviceInfoList.GetPtr();
		do {
			Delete(ppDeviceInfo[0]);
			++ppDeviceInfo;
		} while (--uCount);
	}
}

/*! ************************************

	\fn const _D3DADAPTER_IDENTIFIER9
*Burger::DisplayDirectX9::AdapterInfo::GetAdapterIdentifier(void) const \brief
Get pointer to D3DADAPTER_IDENTIFIER9 record for this adapter

	\windowsonly
	\return Pointer to the contained D3DADAPTER_IDENTIFIER9 structure

***************************************/

/*! ************************************

	\fn uint_t Burger::DisplayDirectX9::AdapterInfo::GetAdapterOrdinal(void)
const \brief Get ordinal index for the adapter this class describes

	\windowsonly
	\return Adapter ordinal index

***************************************/

/*! ************************************

	\fn uintptr_t
Burger::DisplayDirectX9::AdapterInfo::GetDisplayModeListSize(void) const \brief
Get number of entries in the display mode list

	\windowsonly
	\return Number of entries in the display mode list, zero means the list is
empty. \sa GetDisplayModeList(void) const

***************************************/

/*! ************************************

	\fn const DisplayMode_t *
Burger::DisplayDirectX9::AdapterInfo::GetDisplayModeList(void) const \brief Get
the pointer to the base of the DisplayMode_t array

	\windowsonly
	\return Pointer to the DisplayMode_t array, it can be \ref NULL if the array
is empty. \sa GetDisplayModeListSize(void) const

***************************************/

/*! ************************************

	\brief Add a copy of a DisplayMode_t to the end of the list.

	Make a copy of the passed DisplayMode_t and append the copy to the end of
	the array contained in this class.

	\windowsonly
	\param pDisplayMode Pointer to the DisplayMode_t to copy.

***************************************/

void BURGER_API Burger::DisplayDirectX9::AdapterInfo::AddToList(
	const DisplayMode_t* pDisplayMode)
{
	m_DisplayModeList.push_back(pDisplayMode[0]);
}

/*! ************************************

	\fn uintptr_t
Burger::DisplayDirectX9::AdapterInfo::GetDisplayInfoListSize(void) const \brief
Get number of entries in the display info list

	\windowsonly
	\return Number of entries in the display info list, zero means the list is
empty. \sa GetDisplayInfoList(void) const

***************************************/

/*! ************************************

	\fn const DeviceInfo *
Burger::DisplayDirectX9::AdapterInfo::GetDisplayInfoList(void) const \brief Get
the pointer to the base of the DeviceInfo * array

	\windowsonly
	\return Pointer to the DeviceInfo * array, it can be \ref NULL if the array
is empty. \sa GetDisplayInfoListSize(void) const

***************************************/

/*! ************************************

	\brief Add a DeviceInfo * to the end of the list.

	Append a pointer to the DeviceInfo pointer array. This class takes ownership
	of the pointer and will dispose of it when the class is destructed.

	\windowsonly
	\param pDisplayInfo Pointer to the DeviceInfo to take ownership and append
to the pointer array.

***************************************/

void BURGER_API Burger::DisplayDirectX9::AdapterInfo::AddToList(
	DeviceInfo* pDisplayInfo)
{
	m_DeviceInfoList.push_back(pDisplayInfo);
}

/*! ************************************

	\brief Find a DeviceInfo * in the list.

	Iterate over the DeviceInfo pointer array and return
	the entry that matches the passed device type.

	\windowsonly
	\param uDeviceType Type of device to locate
	\return Pointer to the located DeviceInfo or \ref NULL if not found.

***************************************/

const Burger::DisplayDirectX9::DeviceInfo* BURGER_API
Burger::DisplayDirectX9::AdapterInfo::Find(uint_t uDeviceType) const
{
	// Assume not found.
	const DeviceInfo* pResult = NULL;
	uintptr_t uCount = m_DeviceInfoList.size();
	if (uCount) {
		// There are entries.
		DeviceInfo* const* ppList = m_DeviceInfoList.GetPtr();
		do {
			const DeviceInfo* pDeviceInfo = ppList[0];
			++ppList;
			// Match?
			if (pDeviceInfo->GetDeviceType() == uDeviceType) {
				// Return this entry
				pResult = pDeviceInfo;
				break;
			}
		} while (--uCount);
	}
	return pResult;
}

/*! ************************************

	\class Burger::DisplayDirectX9::BufferFormatGroup
	\brief Information for all pixel formats for a specific rendering device

	For each rendering device, this class contains a list of every
	supported pixel formats for the back buffers, including anti-aliasing,
	depth/stencil, and presentation intervals.

	\sa DSMSConflict_t, DeviceInfo or AdapterInfo

***************************************/

/*! ************************************

	\struct Burger::DisplayDirectX9::BufferFormatGroup::MSQuality_t
	\brief Structure to describe the maximum quality for a multisample type

	\windowsonly

	\sa BufferFormatGroup

***************************************/

/*! ************************************

	\struct Burger::DisplayDirectX9::BufferFormatGroup::DSMSConflict_t
	\brief Structure to describe incompatible D3DFORMAT / D3DMULTISAMPLE_TYPE
pairs

	\windowsonly

	\sa BufferFormatGroup

***************************************/

/*! ************************************

	\brief Create a default BufferFormatGroup

	\param uAdapterOrdinal Adapter ordinal index
	\param uDeviceType Hardware/software device type
	\param uAdapterFormat Pixel format of the adapter
	\param uBackBufferFormat Pixel format of the back buffer
	\param bWindowed \ref TRUE if in a window, \ref FALSE if full screen

***************************************/

Burger::DisplayDirectX9::BufferFormatGroup::BufferFormatGroup(
	uint_t uAdapterOrdinal, uint_t uDeviceType, uint_t uAdapterFormat,
	uint_t uBackBufferFormat, uint_t bWindowed):
	m_DepthStencilFormatList(),
	m_PresentIntervalList(),
	m_MultiSampleQualityList(),
	m_DSMSConflictList(),
	m_pAdapterInfo(NULL),
	m_pDeviceInfo(NULL),
	m_uAdapterOrdinal(uAdapterOrdinal),
	m_uDeviceType(uDeviceType),
	m_uAdapterFormat(uAdapterFormat),
	m_uBackBufferFormat(uBackBufferFormat),
	m_bWindowed(bWindowed != 0)
{
}

/*! ************************************

	\brief Dispose of a BufferFormatGroup

***************************************/

Burger::DisplayDirectX9::BufferFormatGroup::~BufferFormatGroup() {}

/*! ************************************

	\fn void Burger::DisplayDirectX9::BufferFormatGroup::SetParent(const
AdapterInfo *pAdapterInfo) \brief Set the parent AdapterInfo pointer

	\param pAdapterInfo Pointer to the parent AdapterInfo
	\sa GetAdapterInfo(void) const

***************************************/

/*! ************************************

	\fn const AdapterInfo *
Burger::DisplayDirectX9::BufferFormatGroup::GetAdapterInfo(void) const \brief
Get the parent AdapterInfo pointer

	\return Pointer to the parent AdapterInfo
	\sa SetParent(const AdapterInfo *)

***************************************/

/*! ************************************

	\fn void Burger::DisplayDirectX9::BufferFormatGroup::SetParent(const
DeviceInfo *pDeviceInfo) \brief Set the parent DeviceInfo pointer

	\param pDeviceInfo Pointer to the parent DeviceInfo
	\sa GetDeviceInfo(void) const

***************************************/

/*! ************************************

	\fn const DeviceInfo *
Burger::DisplayDirectX9::BufferFormatGroup::GetDeviceInfo(void) const \brief Get
the parent DeviceInfo pointer

	\return Pointer to the parent DeviceInfo
	\sa SetParent(const DeviceInfo *)

***************************************/

/*! ************************************

	\fn uint_t
Burger::DisplayDirectX9::BufferFormatGroup::GetAdapterOrdinal(void) const \brief
Get the adapter ordinal value

	\return The ordinal value index for the display adapter

***************************************/

/*! ************************************

	\fn uint_t Burger::DisplayDirectX9::BufferFormatGroup::GetDeviceType(void)
const \brief Get the device driver type

	\return The D3DDEVTYPE device type for the buffer group

***************************************/

/*! ************************************

	\fn uint_t
Burger::DisplayDirectX9::BufferFormatGroup::GetAdapterFormat(void) const \brief
Get the pixel format of the device adapter

	\return The D3DFORMAT of the pixel type for the adapter screen

***************************************/

/*! ************************************

	\fn uint_t
Burger::DisplayDirectX9::BufferFormatGroup::GetBackBufferFormat(void) const
	\brief Get the pixel format of the back buffer for the driver

	\return The D3DFORMAT of the pixel type for the back buffer

***************************************/

/*! ************************************

	\fn uint_t Burger::DisplayDirectX9::BufferFormatGroup::IsWindowed(void)
const \brief \ref TRUE if the driver is for a window, \ref FALSE for full
screen.

	\return The boolean value if the buffer is intended for a window or a full
screen context.

***************************************/

/*! ************************************

	\fn uintptr_t
Burger::DisplayDirectX9::BufferFormatGroup::GetMultiSampleQualityListSize(void)
const \brief Number of entries in the multisample quality list.

	\return Zero if the list is empty or the number of entries in the list.
	\sa GetMultiSampleQualityList(void) const

***************************************/

/*! ************************************

	\fn const MSQuality_t *
Burger::DisplayDirectX9::BufferFormatGroup::GetMultiSampleQualityList(void)
const \brief Get the base pointer to the multisample maximum quality list

	\return The base pointer to the quality list and it's possible to be \ref
NULL if the list is empty. \sa GetMultiSampleQualityListSize(void) const

***************************************/

/*! ************************************

	\brief Scan depth/stencil formats vs multi-sample types for conflicts

	Not all depth/stencil buffer formats are compatible with multi-sampling.
	This function tests all combinations and creates a list of
	incompatible pairs.

	\param pDirect3D9 Pointer to the Direct3D9 instance the device is attached
to.

***************************************/

void BURGER_API Burger::DisplayDirectX9::BufferFormatGroup::CreateConflictList(
	IDirect3D9* pDirect3D9)
{
	uintptr_t uDepthFormatListSize = m_DepthStencilFormatList.size();
	uintptr_t uMultiSampleListSize = m_MultiSampleQualityList.size();
	if (uDepthFormatListSize && uMultiSampleListSize) {
		const uint_t* pStencils = m_DepthStencilFormatList.GetPtr();
		do {
			D3DFORMAT uFormat = static_cast<D3DFORMAT>(pStencils[0]);
			++pStencils;
			uintptr_t uSampleCount = uMultiSampleListSize;
			const MSQuality_t* pTypes = m_MultiSampleQualityList.GetPtr();
			do {
				D3DMULTISAMPLE_TYPE uType =
					static_cast<D3DMULTISAMPLE_TYPE>(pTypes->m_uMSType);
				++pTypes;
				if (pDirect3D9->CheckDeviceMultiSampleType(m_uAdapterOrdinal,
						static_cast<D3DDEVTYPE>(m_uDeviceType), uFormat,
						static_cast<BOOL>(m_bWindowed), uType, NULL) < 0) {
					DSMSConflict_t Entry;
					Entry.m_uDSFormat = uFormat;
					Entry.m_uMSType = uType;
					m_DSMSConflictList.push_back(Entry);
				}
			} while (--uSampleCount);
		} while (--uDepthFormatListSize);
	}
}

/*! ************************************

	\brief Create the depth/stencil format list

	Given a list of possible depth / stencil combinations, test the device
	if they are valid and for those that are valid, add them to the
	internal depth/stencil format list.

	\param pDirect3D9 Pointer to the Direct3D9 instance the device is attached
to. \param pPossibleList Pointer to the array of possible depth / stencil
combinations.

***************************************/

void BURGER_API
Burger::DisplayDirectX9::BufferFormatGroup::CreateDepthStencilFormatList(
	IDirect3D9* pDirect3D9, const SimpleArray<uint_t>* pPossibleList)
{
	uintptr_t uStencilCount = pPossibleList->size();
	if (uStencilCount) {
		// Get the array pointer
		const uint_t* pStencils = pPossibleList->GetPtr();
		do {
			// It's a D3DFORMAT
			D3DFORMAT uDepthFormat = static_cast<D3DFORMAT>(pStencils[0]);
			++pStencils;

			// Perform the initial test
			if (pDirect3D9->CheckDeviceFormat(m_uAdapterOrdinal,
					static_cast<D3DDEVTYPE>(m_uDeviceType),
					static_cast<D3DFORMAT>(m_uAdapterFormat),
					D3DUSAGE_DEPTHSTENCIL, D3DRTYPE_SURFACE,
					uDepthFormat) >= 0) {

				// Test against the depth test
				if (pDirect3D9->CheckDepthStencilMatch(m_uAdapterOrdinal,
						static_cast<D3DDEVTYPE>(m_uDeviceType),
						static_cast<D3DFORMAT>(m_uAdapterFormat),
						static_cast<D3DFORMAT>(m_uBackBufferFormat),
						uDepthFormat) >= 0) {

					// Add this entry to the list since it's supported by
					// the renderer
					m_DepthStencilFormatList.push_back(uDepthFormat);
				}
			}
		} while (--uStencilCount);
	}
}

/*! ************************************

	\brief Enumerate DirectX 9 presentation flags for the device

	\param uIntervalFlags Valid presentation flags
	\param pPossibleList Pointer to the array of possible presentation values.

***************************************/

void BURGER_API
Burger::DisplayDirectX9::BufferFormatGroup::CreatePresentIntervalList(
	uint_t uIntervalFlags, const SimpleArray<uint_t>* pPossibleList)
{
	uintptr_t uPresentCount = pPossibleList->size();
	if (uPresentCount) {
		const uint_t* pPresents = pPossibleList->GetPtr();
		do {
			uint_t uPresent = pPresents[0];
			++pPresents;

			// Some modes are not supported in a window
			if (!m_bWindowed ||
				((uPresent != D3DPRESENT_INTERVAL_TWO) &&
					(uPresent != D3DPRESENT_INTERVAL_THREE) &&
					(uPresent != D3DPRESENT_INTERVAL_FOUR))) {

				// D3DPRESENT_INTERVAL_DEFAULT is always available.

				if ((uPresent == D3DPRESENT_INTERVAL_DEFAULT) ||
					(uIntervalFlags & uPresent)) {
					m_PresentIntervalList.push_back(uPresent);
				}
			}
		} while (--uPresentCount);
	}
}

/*! ************************************

	\brief Enumerate DirectX 9 multi-sample types for the device

	\param pDirect3D9 Pointer to the Direct3D9 instance the device is attached
to. \param pPossibleList Pointer to the array of possible multisample settings
	\param uMaxQuality Maximum quality allowed during enumeration

***************************************/

void BURGER_API
Burger::DisplayDirectX9::BufferFormatGroup::CreateMultiSampleTypeList(
	IDirect3D9* pDirect3D9, const SimpleArray<uint_t>* pPossibleList,
	uint_t uMaxQuality)
{
	uintptr_t uMultiCount = pPossibleList->size();
	if (uMultiCount) {
		const uint_t* pMultis = pPossibleList->GetPtr();
		do {
			DWORD uQuality;
			D3DMULTISAMPLE_TYPE uSampleType =
				static_cast<D3DMULTISAMPLE_TYPE>(pMultis[0]);

			if (pDirect3D9->CheckDeviceMultiSampleType(m_uAdapterOrdinal,
					static_cast<D3DDEVTYPE>(m_uDeviceType),
					static_cast<D3DFORMAT>(m_uBackBufferFormat),
					static_cast<BOOL>(m_bWindowed), uSampleType,
					&uQuality) >= 0) {

				// Clamp the quality, for performance reasons.
				MSQuality_t Temp;
				Temp.m_uMSType = uSampleType;
				Temp.m_uMaxQuality =
					Min(static_cast<uint_t>(uQuality), uMaxQuality + 1);
				m_MultiSampleQualityList.push_back(Temp);
			}
			++pMultis;
		} while (--uMultiCount);
	}
}

/*! ************************************

	\brief Test if a depth/stencil and multisample type are in conflict.

	Iterate over the conflict array and if the pair is found in the table
	this function will return \ref TRUE. If \ref FALSE is returned,
	the pair is valid for creating a DirectX 9 device.

	\windowsonly

	\param uDSFormat Depth/stencil pixel format
	\param uMSType Multisample data type
	\return \ref TRUE if the pair is not supported, \ref FALSE if there is no
conflict.

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::BufferFormatGroup::IsConflicted(
	uint_t uDSFormat, uint_t uMSType) const
{
	// No match
	uint_t bResult = FALSE;
	uintptr_t uCount = m_DSMSConflictList.size();
	if (uCount) {
		const DSMSConflict_t* pConflict = m_DSMSConflictList.GetPtr();
		do {
			if ((pConflict->m_uDSFormat == uDSFormat) &&
				(pConflict->m_uMSType == uMSType)) {
				bResult = TRUE;
				break;
			}
			++pConflict;
		} while (--uCount);
	}
	return bResult;
}

/*! ************************************

	\brief Generating a ranking based on closest match

	Returns a ranking number that describes how closely this device
	combo matches the optimal combo based on the match options and the optimal
device settings

	\param pOptimalDeviceSettings Pointer to the format requested
	\param pAdapterDesktopDisplayMode Pointer to DirectX 9 display mode to test
against \param pBestModeIndex Pointer to best found mode index \param
pBestMSAAIndex Pointer to best anti-aliasing index

	\return 0.0f for no match, with a higher value for closest match

***************************************/

float BURGER_API Burger::DisplayDirectX9::BufferFormatGroup::RankDevice(
	const DeviceSettings_t* pOptimalDeviceSettings,
	const _D3DDISPLAYMODE* pAdapterDesktopDisplayMode,
	uintptr_t* pBestModeIndex, uintptr_t* pBestMSAAIndex) const
{
	float fCurRanking = 0.0f;

	// Adapter match (Highest factor)

	if (m_uAdapterOrdinal == pOptimalDeviceSettings->m_uAdapterOrdinal) {
		fCurRanking = 1000.0f;
	}

	// Match the device (Next highest)

	if (m_uDeviceType == pOptimalDeviceSettings->m_uDeviceType) {
		fCurRanking += 100.0f;
	}

	// Slightly prefer HAL
	if (m_uDeviceType == D3DDEVTYPE_HAL) {
		fCurRanking += 0.1f;
	}

	// Windowed vs full screen
	if (m_bWindowed == pOptimalDeviceSettings->m_bWindowed) {
		fCurRanking += 10.0f;
	}

	// Pixel format of the adapter
	if (m_uAdapterFormat == pOptimalDeviceSettings->m_uAdapterFormat) {
		fCurRanking += 1.0f;
	} else {
		// Score based by the bit depths
		int iDelta = Abs(static_cast<int32_t>(
			GetD3DFORMATColorChannelBits(m_uAdapterFormat) -
			GetD3DFORMATColorChannelBits(
				pOptimalDeviceSettings->m_uAdapterFormat)));
		float fScale = Max(0.9f - static_cast<float>(iDelta) * 0.2f, 0.0f);
		fCurRanking += fScale;
	}

	// Special casing for full screen
	if (!m_bWindowed) {
		// Slightly prefer when it matches the desktop format or is
		// D3DFMT_X8R8G8B8
		if (GetD3DFORMATColorChannelBits(pAdapterDesktopDisplayMode->Format) >=
			8) {
			if (m_uAdapterFormat ==
				static_cast<uint_t>(pAdapterDesktopDisplayMode->Format)) {
				fCurRanking += 0.1f;
			}
		} else {
			if (m_uAdapterFormat == D3DFMT_X8R8G8B8) {
				fCurRanking += 0.1f;
			}
		}
	}

	// Vertex processing modes

	const D3DCAPS9* pCaps = m_pDeviceInfo->GetCaps();
	if ((pOptimalDeviceSettings->m_uBehaviorFlags &
			D3DCREATE_HARDWARE_VERTEXPROCESSING) ||
		(pOptimalDeviceSettings->m_uBehaviorFlags &
			D3DCREATE_MIXED_VERTEXPROCESSING)) {
		if (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
			fCurRanking += 1.0f;
		}
	}

	// Slightly prefer HW T&L
	if (pCaps->DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
		fCurRanking += 0.1f;
	}

	// Resolution
	uint_t bResolutionFound = FALSE;
	uint_t uBest = BURGER_MAXUINT;
	uintptr_t uBestModeIndex = 0; // Default to the first one
	uintptr_t uModeCount = m_pAdapterInfo->GetDisplayModeListSize();
	if (uModeCount) {
		uintptr_t uIndex = 0;
		const DisplayMode_t* pModes = m_pAdapterInfo->GetDisplayModeList();
		do {
			if (pModes->m_uFormat == m_uAdapterFormat) {

				if ((pModes->m_uWidth ==
						pOptimalDeviceSettings->m_uBackBufferWidth) &&
					(pModes->m_uHeight ==
						pOptimalDeviceSettings->m_uBackBufferHeight)) {
					bResolutionFound = TRUE;
				}

				int iCurrent =
					Abs(static_cast<int32_t>(pModes->m_uWidth -
						pOptimalDeviceSettings->m_uBackBufferWidth)) +
					Abs(static_cast<int32_t>(pModes->m_uHeight -
						pOptimalDeviceSettings->m_uBackBufferHeight));
				if (static_cast<uint_t>(iCurrent) < uBest) {
					uBest = static_cast<uint_t>(iCurrent);
					uBestModeIndex = uIndex;
				}
			}
			++pModes;
		} while (++uIndex < uModeCount);
	}
	// Save off the index with the best match
	pBestModeIndex[0] = uBestModeIndex;

	// If an exact match was found, jump the score!
	if (bResolutionFound) {
		fCurRanking += 1.0f;
	}

	// Back buffer format
	if (m_uBackBufferFormat == pOptimalDeviceSettings->m_uBackBufferFormat) {
		fCurRanking += 1.0f;
	} else {
		int iDelta = Abs(static_cast<int32_t>(
			GetD3DFORMATColorChannelBits(m_uBackBufferFormat) -
			GetD3DFORMATColorChannelBits(
				pOptimalDeviceSettings->m_uBackBufferFormat)));
		float fScale = Max(0.9f - static_cast<float>(iDelta) * 0.2f, 0.0f);
		fCurRanking += fScale;
	}

	// Check if this back buffer format is the same as
	// the adapter format since this is preferred.

	if (m_uBackBufferFormat == m_uAdapterFormat) {
		fCurRanking += 0.1f;
	}

	// Multi-sample
	uint_t bMultiSampleFound = FALSE;
	uintptr_t uBestMSAAIndex = 0;
	uintptr_t uSampleCount = m_MultiSampleQualityList.size();
	if (uSampleCount) {
		const MSQuality_t* pQualities = m_MultiSampleQualityList.GetPtr();
		uintptr_t uMSIndex = 0;
		do {
			if ((pQualities[uMSIndex].m_uMSType ==
					pOptimalDeviceSettings->m_uMultiSampleType) &&
				(pQualities[uMSIndex].m_uMaxQuality >
					pOptimalDeviceSettings->m_uMultiSampleQuality)) {
				bMultiSampleFound = TRUE;
				uBestMSAAIndex = uMSIndex;
				break;
			}
		} while (++uMSIndex < uSampleCount);
	}

	pBestMSAAIndex[0] = uBestMSAAIndex;
	if (bMultiSampleFound) {
		fCurRanking += 1.0f;
	}

	// Depth stencil match?
	if (m_DepthStencilFormatList.contains(
			pOptimalDeviceSettings->m_uAutoDepthStencilFormat)) {
		fCurRanking += 1.0f;
	}

	// Refresh rate

	uModeCount = m_pAdapterInfo->GetDisplayModeListSize();
	if (uModeCount) {
		const DisplayMode_t* pModes = m_pAdapterInfo->GetDisplayModeList();
		do {
			if (pModes->m_uFormat == m_uAdapterFormat) {
				if (pModes->m_uRefreshRate ==
					pOptimalDeviceSettings->m_uFullScreen_RefreshRateInHz) {
					fCurRanking += 1.0f;
					break;
				}
			}
			++pModes;
		} while (--uModeCount);
	}

	// Check that the present interval is supported by this buffer format

	if (m_PresentIntervalList.contains(
			pOptimalDeviceSettings->m_uPresentationInterval)) {
		fCurRanking += 1.0f;
	}

	// Return the final score (Higher is better)
	return fCurRanking;
}

/*! ************************************

	\class Burger::DisplayDirectX9::Enumerator
	\brief DirectX 9 device enumerator

	This class will cycle through every DirectX 9 compatible device
	and query its capabilities to allow the DisplayDirectX9 class
	select the best match for the video resolution requested
	to initialize with. This class is only for DirectX 9, use
	Burger::DisplayDirectX11 to support Windows 8 or higher instead.

	\windowsonly

	\sa Burger::DisplayDirectX9 or Burger::DisplayDirectX11

***************************************/

/*! ************************************

	\brief Set up the DirectX 9 device enumerator to defaults

	* * Minimum width is 640
	* * Minimum height is 480
	* * Enable software, hardware and pure hardware renderer
	* * Require shader support

***************************************/

Burger::DisplayDirectX9::Enumerator::Enumerator():
	m_pD3D(NULL),
	m_pIsDeviceAcceptableFunc(NULL),
	m_pIsDeviceAcceptableFuncData(NULL),
	m_DepthStencilPossibleList(),
	m_MultiSampleTypeList(),
	m_PresentIntervalList(),
	m_AdapterInfoList(),
	m_uMinWidth(640),
	m_uMaxWidth(BURGER_MAXUINT),
	m_uMinHeight(480),
	m_uMaxHeight(BURGER_MAXUINT),
	m_uRefreshMin(0),
	m_uRefreshMax(BURGER_MAXUINT),
	m_uMultisampleQualityMax(65535),
	m_bHasEnumerated(FALSE),
	m_bRequirePostPixelShaderBlending(TRUE),
	m_bSoftwareVP(TRUE),
	m_bHardwareVP(TRUE),
	m_bPureHarewareVP(TRUE),
	m_bMixedVP(FALSE)

{
	ResetPossibleDepthStencilFormats();
	ResetPossibleMultisampleTypeList();
	ResetPossiblePresentIntervalList();
}

/*! ************************************

	\brief Release all allocated resources

***************************************/

Burger::DisplayDirectX9::Enumerator::~Enumerator()
{
	// Purge the AdapterInfo array
	ClearAdapterInfoList();
}

/*! ************************************

	\fn void
Burger::DisplayDirectX9::Enumerator::SetRequirePostPixelShaderBlending(uint_t
bRequire) \brief Set the flag making shaders a requirement

	If support for post pixel blending is required, call this function with \ref
TRUE before calling Enumerate(). If no post pixel blending is required (Usually
for applications that are pure software rendering), call this function with \ref
FALSE

	\param bRequire \ref FALSE for software rendering, \ref TRUE for shader
rendering

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *)

***************************************/

/*! ************************************

	\fn void Burger::DisplayDirectX9::Enumerator::SetResolutionMinMax(uint_t
uMinWidth,uint_t uMinHeight,uint_t uMaxWidth,uint_t uMaxHeight) \brief Set the
minimum and maximum acceptable resolutions

	If only certain display resolutions are acceptable, call this function with
the minimum and maximum settings.

	\param uMinWidth Minimum acceptable width
	\param uMinHeight Minimum acceptable height
	\param uMaxWidth Maximum acceptable width
	\param uMaxHeight Maximum acceptable height

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *)

***************************************/

/*! ************************************

	\fn void Burger::DisplayDirectX9::Enumerator::SetRefreshMinMax(uint_t
uMin,uint_t uMax) \brief Set the minimum and maximum acceptable refresh rates

	If only certain display refresh rates are acceptable, call this function
with the minimum and maximum settings.

	\param uMin Minimum acceptable refresh rate
	\param uMax Minimum acceptable refresh rate

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *)

***************************************/

/*! ************************************

	\fn void
Burger::DisplayDirectX9::Enumerator::SetMultisampleQualityMax(uint_t uMax)
	\brief Set the maximum acceptable anti-aliasing quality

	For performance, the maximum anti-aliasing quality can be clamped to a
maximum, 0 disables the use of anti-aliasing.

	\param uMax Minimum acceptable anti-aliasing quality

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *)

***************************************/

/*! ************************************

	\fn void
Burger::DisplayDirectX9::Enumerator::SetSoftwareVertexProcessingFlag(uint_t
bSoftwareVP) \brief Allow software vertex processed devices

	If this function is called with \ref TRUE, enable the use of software vertex
processed video device drivers. \ref FALSE to disable them.

	\param bSoftwareVP \ref TRUE to enable software vertex processing devices,
\ref FALSE to disable.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
SetHardwareVertexProcessingFlag(uint_t),
SetPureHarewareVertexProcessingFlag(uint_t), or
	SetMixedVertexProcessingFlag(uint_t)

***************************************/

/*! ************************************

	\fn void
Burger::DisplayDirectX9::Enumerator::SetHardwareVertexProcessingFlag(uint_t
bHardwareVP) \brief Allow hardware vertex processed devices

	If this function is called with \ref TRUE, enable the use of hardware vertex
processed video device drivers. \ref FALSE to disable them.

	\param bHardwareVP \ref TRUE to enable hardware vertex processing devices,
\ref FALSE to disable.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
SetSoftwareVertexProcessingFlag(uint_t),
SetPureHarewareVertexProcessingFlag(uint_t), or
	SetMixedVertexProcessingFlag(uint_t)

***************************************/

/*! ************************************

	\fn void
Burger::DisplayDirectX9::Enumerator::SetPureHarewareVertexProcessingFlag(uint_t
bPureHarewareVP) \brief Allow GPU vertex processed devices

	If this function is called with \ref TRUE, enable the use of GPU vertex
processed video device drivers. \ref FALSE to disable them.

	\param bPureHarewareVP \ref TRUE to enable GPU vertex processing devices,
\ref FALSE to disable.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
SetSoftwareVertexProcessingFlag(uint_t),
SetHardwareVertexProcessingFlag(uint_t), or SetMixedVertexProcessingFlag(uint_t)

***************************************/

/*! ************************************

	\fn void
Burger::DisplayDirectX9::Enumerator::SetMixedVertexProcessingFlag(uint_t
bMixedVP) \brief Allow mixed hardware / software vertex processed devices

	If this function is called with \ref TRUE, enable the use of mixed hardware
/ software vertex processed video device drivers. \ref FALSE to disable them.

	\param bMixedVP \ref TRUE to enable mixed hardware / software vertex
processing devices, \ref FALSE to disable.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
SetSoftwareVertexProcessingFlag(uint_t),
SetHardwareVertexProcessingFlag(uint_t), or
		SetPureHarewareVertexProcessingFlag(uint_t)

***************************************/

/*! ************************************

	\fn uint_t
Burger::DisplayDirectX9::Enumerator::GetSoftwareVertexProcessingFlag(void) const
	\brief Was software vertex processed devices allowed?

	\return \ref TRUE if software vertex rendering devices are allowed.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
GetHardwareVertexProcessingFlag(void) const,
		GetPureHarewareVertexProcessingFlag(void) const, or
GetMixedVertexProcessingFlag(void) const

***************************************/

/*! ************************************

	\fn uint_t
Burger::DisplayDirectX9::Enumerator::GetHardwareVertexProcessingFlag(void) const
	\brief Was hardware vertex processed devices allowed?

	\return \ref TRUE if hardware vertex rendering devices are allowed.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
GetSoftwareVertexProcessingFlag(void) const,
		GetPureHarewareVertexProcessingFlag(void) const, or
GetMixedVertexProcessingFlag(void) const

***************************************/

/*! ************************************

	\fn uint_t
Burger::DisplayDirectX9::Enumerator::GetPureHarewareVertexProcessingFlag(void)
const \brief Was GPU vertex processed devices allowed?

	\return \ref TRUE if GPU vertex rendering devices are allowed.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
GetSoftwareVertexProcessingFlag(void) const,
		GetHardwareVertexProcessingFlag(void) const, or
GetMixedVertexProcessingFlag(void) const

***************************************/

/*! ************************************

	\fn uint_t
Burger::DisplayDirectX9::Enumerator::GetMixedVertexProcessingFlag(void) const
	\brief Was mixed hardware / software vertex processed devices allowed?

	\return \ref TRUE if mixed hardware / software vertex rendering devices are
allowed.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
GetSoftwareVertexProcessingFlag(void) const,
		GetHardwareVertexProcessingFlag(void) const, or
GetPureHarewareVertexProcessingFlag(void) const

***************************************/

/*! ************************************

	\fn const SimpleArray<uint_t>*
Burger::DisplayDirectX9::Enumerator::GetPossibleDepthStencilFormatList(void)
const \brief Return the list of depth/stencil buffer formats supported

	After Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *) is called, a list of
supported depth / stencil buffer formats is generated. It is not guaranteed that
a specific depth / stencil is available for specific back buffer formats. If the
format is not in this list, the device cannot render in that format. The values
contained are DirectX 9 D3DFORMAT entries.

	\return Pointer to the list for depth / stencil formats in D3DFORMAT values.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
GetPossibleMultisampleTypeList(void) const, or
		GetPossiblePresentIntervalList(void) const

***************************************/

/*! ************************************

	\fn const SimpleArray<uint_t>*
Burger::DisplayDirectX9::Enumerator::GetPossibleMultisampleTypeList(void) const
	\brief Return the list of multi-sample buffer types available

	After Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *) is called, a list of
multi-sample buffer types is generated. It is not guaranteed that a specific
multi-sample buffer type is available for specific back buffer formats. If the
buffer type is not in this list, the device cannot be set to that type. The
values contained are DirectX 9 D3DMULTISAMPLE_TYPE entries.

	\return Pointer to the list for multi-sample buffer types in
D3DMULTISAMPLE_TYPE values.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
GetPossibleDepthStencilFormatList(void) const, or
		GetPossiblePresentIntervalList(void) const

***************************************/

/*! ************************************

	\fn const SimpleArray<uint_t>*
Burger::DisplayDirectX9::Enumerator::GetPossiblePresentIntervalList(void) const
	\brief Return the list of presentation flags available

	After Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *) is called, a list of
presentation flags is generated. If the presentation flag is not in this list,
the device cannot be set to that type. The values contained are DirectX 9
D3DPRESENT_INTERVAL_* flags.

	\return Pointer to the list for presentation flags in D3DPRESENT_INTERVAL_*
values.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *),
GetPossibleDepthStencilFormatList(void) const, or
		GetPossibleMultisampleTypeList(void) const

***************************************/

/*! ************************************

	\brief Reset the depth stencil list

	Clear out the depth stencil list, and insert the minimum list
	that is permitted with DirectX 9

	* * D3DFMT_D16
	* * D3DFMT_D15S1
	* * D3DFMT_D24X8
	* * D3DFMT_D24S8
	* * D3DFMT_D24X4S4
	* * D3DFMT_D32

	\sa ResetPossibleMultisampleTypeList(void) or
		ResetPossiblePresentIntervalList(void)

***************************************/

void BURGER_API
Burger::DisplayDirectX9::Enumerator::ResetPossibleDepthStencilFormats(void)
{
	m_DepthStencilPossibleList.resize(6);
	uint_t* pData = m_DepthStencilPossibleList.GetPtr();
	pData[0] = D3DFMT_D16;     // 16 bit depth
	pData[1] = D3DFMT_D15S1;   // 1 bit stencil
	pData[2] = D3DFMT_D24X8;   // 24 bit depth, no stencil
	pData[3] = D3DFMT_D24S8;   // 24 bit depth, 8 bit stencil
	pData[4] = D3DFMT_D24X4S4; // 24 bit depth, 4 bit stencil
	pData[5] = D3DFMT_D32;     // 32 bit depth
}

/*! ************************************

	\brief Reset the multi-sample type list

	Clear out the multi-sample type list, and insert the minimum list
	that is permitted with DirectX 9

	From D3DMULTISAMPLE_NONE to D3DMULTISAMPLE_16_SAMPLES levels.

	\sa ResetPossibleDepthStencilFormats(void) or
		ResetPossiblePresentIntervalList(void)

***************************************/

void BURGER_API
Burger::DisplayDirectX9::Enumerator::ResetPossibleMultisampleTypeList(void)
{
	m_MultiSampleTypeList.resize(17);
	uint_t* pData = m_MultiSampleTypeList.GetPtr();
	pData[0] = D3DMULTISAMPLE_NONE;
	pData[1] = D3DMULTISAMPLE_NONMASKABLE;
	pData[2] = D3DMULTISAMPLE_2_SAMPLES;
	pData[3] = D3DMULTISAMPLE_3_SAMPLES;
	pData[4] = D3DMULTISAMPLE_4_SAMPLES;
	pData[5] = D3DMULTISAMPLE_5_SAMPLES;
	pData[6] = D3DMULTISAMPLE_6_SAMPLES;
	pData[7] = D3DMULTISAMPLE_7_SAMPLES;
	pData[8] = D3DMULTISAMPLE_8_SAMPLES;
	pData[9] = D3DMULTISAMPLE_9_SAMPLES;
	pData[10] = D3DMULTISAMPLE_10_SAMPLES;
	pData[11] = D3DMULTISAMPLE_11_SAMPLES;
	pData[12] = D3DMULTISAMPLE_12_SAMPLES;
	pData[13] = D3DMULTISAMPLE_13_SAMPLES;
	pData[14] = D3DMULTISAMPLE_14_SAMPLES;
	pData[15] = D3DMULTISAMPLE_15_SAMPLES;
	pData[16] = D3DMULTISAMPLE_16_SAMPLES;
}

/*! ************************************

	\brief Reset the page flipping interval list

	Clear out the page flipping interval list, and insert the minimum list
	that is permitted with DirectX 9

	From D3DMULTISAMPLE_NONE to D3DPRESENT_INTERVAL_FOUR

	\sa ResetPossibleDepthStencilFormats(void) or
		ResetPossibleMultisampleTypeList(void)

***************************************/

void BURGER_API
Burger::DisplayDirectX9::Enumerator::ResetPossiblePresentIntervalList(void)
{
	m_MultiSampleTypeList.resize(6);
	uint_t* pData = m_MultiSampleTypeList.GetPtr();
	pData[0] = D3DPRESENT_INTERVAL_IMMEDIATE;
	pData[1] = D3DPRESENT_INTERVAL_DEFAULT;
	pData[2] = D3DPRESENT_INTERVAL_ONE;
	pData[3] = D3DPRESENT_INTERVAL_TWO;
	pData[4] = D3DPRESENT_INTERVAL_THREE;
	pData[5] = D3DPRESENT_INTERVAL_FOUR;
}

/*! ************************************

	\fn uint_t Burger::DisplayDirectX9::Enumerator::HasEnumerated(void) const
	\brief \ref TRUE if video devices were scanned

	After Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *) is called, this flag
is set.

	\return \ref TRUE if Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *) was
called.

	\sa Enumerate(IDirect3D9 *,IsDeviceOkayProc,void *)

***************************************/

/*! ************************************

	\fn const SimpleArray<AdapterInfo*> *
Burger::DisplayDirectX9::Enumerator::GetAdapterInfoList(void) const \brief
Return the pointer to the list of adapters

	\return Pointer to a list of pointers to AdapterInfo

***************************************/

/*! ************************************

	\brief Return the pointer to a specific video adapter

	\param uAdapterOrdinal Ordinal value for a specific display device.

	\return Pointer to a AdapterInfo for the found device or \ref NULL if not
found

***************************************/

const Burger::DisplayDirectX9::AdapterInfo* BURGER_API
Burger::DisplayDirectX9::Enumerator::GetAdapterInfo(
	uint_t uAdapterOrdinal) const
{
	const AdapterInfo* pResult = NULL;
	uintptr_t uCount = m_AdapterInfoList.size();
	if (uCount) {
		AdapterInfo* const* ppList = m_AdapterInfoList.GetPtr();
		do {
			const AdapterInfo* pAdapterInfo = ppList[0];
			if (pAdapterInfo->GetAdapterOrdinal() == uAdapterOrdinal) {
				pResult = pAdapterInfo;
				break;
			}
			++ppList;
		} while (--uCount);
	}
	return pResult;
}

/*! ************************************

	\brief Return the pointer to a specific video adapter

	\param uAdapterOrdinal Ordinal value for a specific display device.
	\param uDeviceType D3DDEVTYPE value for the specific device type

	\return Pointer to a DeviceInfo for the found device or \ref NULL if not
found

***************************************/

const Burger::DisplayDirectX9::DeviceInfo* BURGER_API
Burger::DisplayDirectX9::Enumerator::GetDeviceInfo(
	uint_t uAdapterOrdinal, uint_t uDeviceType) const
{
	const AdapterInfo* pAdapterInfo = GetAdapterInfo(uAdapterOrdinal);
	const DeviceInfo* pResult = NULL;
	if (pAdapterInfo) {
		pResult = pAdapterInfo->Find(uDeviceType);
	}
	return pResult;
}

/*! ************************************

	\brief Return the pointer to a specific video buffer format group

	Given a requested buffer format and window/full screen flag, locate
	if this buffer format is available, and return \ref NULL if not
	found or a valid pointer if located.

	\param uAdapterOrdinal Ordinal value for a specific display device.
	\param uDeviceType D3DDEVTYPE value for the specific device type
	\param uAdapterFormat D3DFORMAT value for the pixel format of the main
buffer \param uBackBufferFormat D3DFORMAT value for the pixel format of the back
buffer \param bWindowed \ref TRUE if the format is available for a window.

	\return Pointer to a BufferFormatGroup for the found pixel buffer or \ref
NULL if not found

***************************************/

const Burger::DisplayDirectX9::BufferFormatGroup* BURGER_API
Burger::DisplayDirectX9::Enumerator::GetBufferFormatGroup(
	uint_t uAdapterOrdinal, uint_t uDeviceType, uint_t uAdapterFormat,
	uint_t uBackBufferFormat, uint_t bWindowed)
{
	const DeviceInfo* pDeviceInfo = GetDeviceInfo(uAdapterOrdinal, uDeviceType);
	const BufferFormatGroup* pResult = NULL;
	if (pDeviceInfo) {
		pResult =
			pDeviceInfo->Find(uAdapterFormat, uBackBufferFormat, bWindowed);
	}
	return pResult;
}

/*! ************************************

	\fn Burger::DisplayDirectX9::BufferFormatGroup
*Burger::DisplayDirectX9::Enumerator::GetBufferFormatGroup(const
DeviceSettings_t *pDeviceSettings); \brief Return the pointer to a specific
video buffer format group

	Given a requested buffer format and window/full screen flag, locate
	if this buffer format is available, and return \ref NULL if not
	found or a valid pointer if located.

	\param pDeviceSettings Pointer to a DeviceSettings_t buffer that describes a
requested video mode.

	\return Pointer to a BufferFormatGroup for the found pixel buffer or \ref
NULL if not found

***************************************/

#if !defined(DOXYGEN)

//
// Sort the display modes (Helps when scanning for a compatible mode)
//

#if defined(BURGER_WATCOM)
#define QSORT_API
#else
#define QSORT_API BURGER_ANSIAPI
#endif

static int QSORT_API QSortModesFunc(const void* pData1, const void* pData2)
{
	const Burger::DisplayDirectX9::DisplayMode_t* pTest1 =
		static_cast<const Burger::DisplayDirectX9::DisplayMode_t*>(pData1);
	const Burger::DisplayDirectX9::DisplayMode_t* pTest2 =
		static_cast<const Burger::DisplayDirectX9::DisplayMode_t*>(pData2);

	int iResult;
	if (pTest1->m_uWidth > pTest2->m_uWidth) {
		iResult = 1;
	} else if (pTest1->m_uWidth < pTest2->m_uWidth) {
		iResult = -1;
	} else if (pTest1->m_uHeight > pTest2->m_uHeight) {
		iResult = 1;
	} else if (pTest1->m_uHeight < pTest2->m_uHeight) {
		iResult = -1;
	} else if (pTest1->m_uFormat > pTest2->m_uFormat) {
		iResult = 1;
	} else if (pTest1->m_uFormat < pTest2->m_uFormat) {
		iResult = -1;
	} else if (pTest1->m_uRefreshRate > pTest2->m_uRefreshRate) {
		iResult = 1;
	} else if (pTest1->m_uRefreshRate < pTest2->m_uRefreshRate) {
		iResult = -1;
	} else {
		iResult = 0;
	}
	return iResult;
}
#endif

/*! ************************************

	\brief Scan all devices

	Given a DirectX 9 device, scan every device mode and enumerate them

	\param pD3D9 Pointer to a DirectX 9 device
	\param pIsDeviceOkayFunc Pointer to a filter function, \ref NULL disables
callback filtering \param pData Pointer to a data pointer for the filter
function

	\return Positive value on success, negative on failure

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::Enumerator::Enumerate(
	IDirect3D9* pD3D9, IsDeviceOkayProc pIsDeviceOkayFunc, void* pData)
{
	m_bHasEnumerated = TRUE;
	m_pD3D = pD3D9;
	m_pIsDeviceAcceptableFunc = pIsDeviceOkayFunc;
	m_pIsDeviceAcceptableFuncData = pData;

	// Clear out any devices set up so enumeration is fresh
	ClearAdapterInfoList();

	// Number of video cards.
	UINT uAdapterCount = pD3D9->GetAdapterCount();
	if (uAdapterCount) {
		UINT uOrdinal = 0;
		do {
			// Create an adapter record
			AdapterInfo* pAdapterInfo =
				new (Alloc(sizeof(AdapterInfo))) AdapterInfo(uOrdinal);
			if (pAdapterInfo) {

				// Get the identifier data, the error is ignored for the return
				// value
				HRESULT iError = pD3D9->GetAdapterIdentifier(uOrdinal, 0,
					const_cast<D3DADAPTER_IDENTIFIER9*>(
						pAdapterInfo->GetAdapterIdentifier()));
				if (iError < 0) {
					Delete(pAdapterInfo);
				} else {

					// Clear the list, so enumeration can start
					SimpleArray<uint_t> TheD3DFormatList; // D3DFORMAT list

					uint_t uPixelIndex = 0;
					D3DDISPLAYMODE TheMode;
					do {
						D3DFORMAT uPixelFormat =
							g_ValidAdapterFormats[uPixelIndex];
						UINT uModeCount =
							pD3D9->GetAdapterModeCount(uOrdinal, uPixelFormat);
						if (uModeCount) {

							// Enumerate the width/heights for the pixel format
							UINT uMode = 0;
							do {
								// Get the mode
								pD3D9->EnumAdapterModes(
									uOrdinal, uPixelFormat, uMode, &TheMode);

								// Prequalify
								if ((TheMode.Width >= m_uMinWidth) &&
									(TheMode.Height >= m_uMinHeight) &&
									(TheMode.Width <= m_uMaxWidth) &&
									(TheMode.Height <= m_uMaxHeight) &&
									(TheMode.RefreshRate >= m_uRefreshMin) &&
									(TheMode.RefreshRate <= m_uRefreshMax)) {

									// This mode is acceptable, add it to the
									// list
									DisplayMode_t TempMode;
									TempMode.m_uWidth = TheMode.Width;
									TempMode.m_uHeight = TheMode.Height;
									TempMode.m_uRefreshRate =
										TheMode.RefreshRate;
									TempMode.m_uFormat = TheMode.Format;
									pAdapterInfo->AddToList(&TempMode);

									// Add this pixel format if not already
									// found
									if (!TheD3DFormatList.contains(
											TempMode.m_uFormat)) {
										TheD3DFormatList.push_back(
											TempMode.m_uFormat);
									}
								}
							} while (++uMode < uModeCount);
						}
					} while (++uPixelIndex <
						BURGER_ARRAYSIZE(g_ValidAdapterFormats));

					// Get the pixel format of the active display mode

					pD3D9->GetAdapterDisplayMode(uOrdinal, &TheMode);

					// See if it's not already in the list
					if (!TheD3DFormatList.contains(TheMode.Format)) {
						TheD3DFormatList.push_back(TheMode.Format);
					}

					// Sort the list of display modes
					qsort(const_cast<DisplayMode_t*>(
							  pAdapterInfo->GetDisplayModeList()),
						pAdapterInfo->GetDisplayModeListSize(),
						sizeof(DisplayMode_t), QSortModesFunc);

					// Pull down all the data in the adapter using the list of
					// acceptable pixel formats
					if (!EnumerateDevices(pAdapterInfo, &TheD3DFormatList) &&
						pAdapterInfo->GetDisplayInfoListSize()) {
						m_AdapterInfoList.push_back(pAdapterInfo);
					} else {
						// Toss it away
						Delete(pAdapterInfo);
					}
				}
			}
		} while (++uOrdinal < uAdapterCount);
	}
	return 0;
}

/*! ************************************

	\brief Release all AdapterInfo objects

***************************************/

void BURGER_API Burger::DisplayDirectX9::Enumerator::ClearAdapterInfoList(void)
{
	// Scan the list and dispose of the entries
	uintptr_t uCount = m_AdapterInfoList.size();
	if (uCount) {
		AdapterInfo* const* ppList = m_AdapterInfoList.GetPtr();
		do {
			Delete(ppList[0]);
			++ppList;
		} while (--uCount);
	}

	// Get rid of the list
	m_AdapterInfoList.clear();
}

/*! ************************************

	\brief Enumerate DirectX 9 devices for a specific adapter
	\param pAdapterInfo Pointer to the adapter to enumerate
	\param pAdapterFormatList Pixel formats to enumerate

	\return HRESULT from DirectX 9

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::Enumerator::EnumerateDevices(
	AdapterInfo* pAdapterInfo, const SimpleArray<uint_t>* pAdapterFormatList)
{
	HRESULT hr;

	uintptr_t uIndex = 0;
	do {
		D3DCAPS9 TheCaps;

		// Get the device capabilities
		if (m_pD3D->GetDeviceCaps(pAdapterInfo->GetAdapterOrdinal(),
				g_DeviceTypes[uIndex], &TheCaps) >= 0) {

			// Enumerate each Direct3D device type
			DeviceInfo* pDeviceInfo =
				new (Alloc(sizeof(DeviceInfo))) DeviceInfo(
					pAdapterInfo->GetAdapterOrdinal(), g_DeviceTypes[uIndex]);
			if (pDeviceInfo) {

				// Save the device index and type of device
				MemoryCopy(const_cast<D3DCAPS9*>(pDeviceInfo->GetCaps()),
					&TheCaps, sizeof(TheCaps));

				// Create a temp device to verify that it is really possible to
				// create a REF device [the developer DirectX redist has to be
				// installed]

				if (pDeviceInfo->GetDeviceType() != D3DDEVTYPE_HAL) {

					D3DDISPLAYMODE TheMode;
					m_pD3D->GetAdapterDisplayMode(0, &TheMode);

					D3DPRESENT_PARAMETERS ThePresent;
					MemoryClear(&ThePresent, sizeof(ThePresent));
					ThePresent.BackBufferWidth = 1;
					ThePresent.BackBufferHeight = 1;
					ThePresent.BackBufferFormat = TheMode.Format;
					ThePresent.BackBufferCount = 1;
					ThePresent.SwapEffect = D3DSWAPEFFECT_COPY;
					ThePresent.Windowed = TRUE;
					HWND pWindow = Win32::get_window();
					ThePresent.hDeviceWindow = pWindow;

					IDirect3DDevice9* pDevice = NULL;
					hr = m_pD3D->CreateDevice(pAdapterInfo->GetAdapterOrdinal(),
						static_cast<D3DDEVTYPE>(pDeviceInfo->GetDeviceType()),
						pWindow,
						D3DCREATE_HARDWARE_VERTEXPROCESSING |
							D3DCREATE_FPU_PRESERVE,
						&ThePresent, &pDevice);
					if (hr < 0) {
						Delete(pDeviceInfo);
						continue;
					}
					// Success! Release the temp device
					if (pDevice) {
						pDevice->Release();
					}
				}

				// Get info for each buffer format on this device
				if (!EnumerateBufferFormats(&TheCaps, pAdapterInfo, pDeviceInfo,
						pAdapterFormatList) &&
					pDeviceInfo->GetBufferListSize()) {
					// Only add if there was something that was added to the
					// list.
					pAdapterInfo->AddToList(pDeviceInfo);
				} else {
					// Surrender
					Delete(pDeviceInfo);
				}
			}
		}
	} while (++uIndex < BURGER_ARRAYSIZE(g_DeviceTypes));

	// No error is possible
	return 0;
}

/*! ************************************

	\brief Enumerate DirectX 9 buffer formats for a specific adapter

	\param pCaps Pointer to the D3DCAPS9 for the display device
	\param pAdapterInfo Pointer to the adapter to enumerate
	\param pDeviceInfo Pointer to the device attached to the adapter
	\param pAdapterFormatList Pixel formats to enumerate

	\return HRESULT from DirectX 9

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::Enumerator::EnumerateBufferFormats(
	const _D3DCAPS9* pCaps, const AdapterInfo* pAdapterInfo,
	DeviceInfo* pDeviceInfo, const SimpleArray<uint_t>* pAdapterFormatList)
{
	uintptr_t uFormatCount = pAdapterFormatList->size();
	if (uFormatCount) {
		uintptr_t uFormatIndex = 0;
		// See which adapter formats are supported by this device
		const uint_t* pFormats = pAdapterFormatList->GetPtr();
		do {
			D3DFORMAT uAdapterFormat =
				static_cast<D3DFORMAT>(pFormats[uFormatIndex]);
			uintptr_t uBackBufferIndex = 0;
			do {
				D3DFORMAT uBackBufferFormat =
					g_BackBufferFormats[uBackBufferIndex];
				uint_t bWindowed = 0;
				do {
					// If full screen, a valid list of modes is required to work
					if (!bWindowed && !pAdapterInfo->GetDisplayModeListSize()) {
						continue;
					}

					if (m_pD3D->CheckDeviceType(
							pAdapterInfo->GetAdapterOrdinal(),
							static_cast<D3DDEVTYPE>(
								pDeviceInfo->GetDeviceType()),
							uAdapterFormat, uBackBufferFormat,
							static_cast<BOOL>(bWindowed)) >= 0) {

						// If the backbuffer format doesn't support
						// D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING then alpha
						// test, pixel fog, render-target blending, color write
						// enable, and dithering. are not supported.

						if (m_bRequirePostPixelShaderBlending) {
							if (m_pD3D->CheckDeviceFormat(
									pAdapterInfo->GetAdapterOrdinal(),
									static_cast<D3DDEVTYPE>(
										pDeviceInfo->GetDeviceType()),
									uAdapterFormat,
									D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
									D3DRTYPE_TEXTURE, uBackBufferFormat) < 0) {
								continue;
							}
						}

						// Did the application submit a custom filter?

						if (m_pIsDeviceAcceptableFunc) {
							if (!m_pIsDeviceAcceptableFunc(pCaps,
									uAdapterFormat, uBackBufferFormat,
									bWindowed, m_pIsDeviceAcceptableFuncData)) {
								continue;
							}
						}

						// At this point, we have an
						// adapter/device/adapterformat/backbufferformat/iswindowed
						// DeviceCombo that is supported by the system and
						// acceptable to the app. We still need to find one or
						// more suitable depth/stencil buffer format,
						// multisample type, and present interval.
						BufferFormatGroup* pBufferFormatGroup = new (
							Alloc(sizeof(BufferFormatGroup)))
							BufferFormatGroup(pAdapterInfo->GetAdapterOrdinal(),
								pDeviceInfo->GetDeviceType(), uAdapterFormat,
								uBackBufferFormat, bWindowed);
						if (pBufferFormatGroup) {

							// Create the depth list
							pBufferFormatGroup->CreateDepthStencilFormatList(
								m_pD3D, &m_DepthStencilPossibleList);
							pBufferFormatGroup->CreateMultiSampleTypeList(
								m_pD3D, &m_MultiSampleTypeList,
								m_uMultisampleQualityMax);

							if (!pBufferFormatGroup
									 ->GetMultiSampleQualityListSize()) {
								Delete(pBufferFormatGroup);
							} else {
								pBufferFormatGroup->CreateConflictList(m_pD3D);
								pBufferFormatGroup->CreatePresentIntervalList(
									pCaps->PresentationIntervals,
									&m_PresentIntervalList);

								// Save off the parent record
								pBufferFormatGroup->SetParent(pAdapterInfo);
								pBufferFormatGroup->SetParent(pDeviceInfo);
								pDeviceInfo->AddToList(pBufferFormatGroup);
							}
						}
					}
				} while (++bWindowed < 2U);
			} while (
				++uBackBufferIndex < BURGER_ARRAYSIZE(g_BackBufferFormats));
		} while (++uFormatIndex < uFormatCount);
	}
	return 0;
}

//
// Initialize the D3D variables
//

Burger::DisplayDirectX9::DisplayDirectX9(GameApp* pGameApp):
	Display(pGameApp),
	m_Enumerator(),
	m_pDeviceFilter(NULL),
	m_pDeviceFilterData(NULL),
	m_pModifyDeviceSettingsFunc(NULL),
	m_pModifyDeviceSettingsData(NULL),
	m_pDeviceLostFunc(NULL),
	m_pDeviceLostFuncData(NULL),
	m_pDeviceDestroyedFunc(NULL),
	m_pDeviceDestroyedFuncData(NULL),
	m_pDeviceCreatedFunc(NULL),
	m_pDeviceCreatedFuncData(NULL),
	m_pDeviceResetFunc(NULL),
	m_pDeviceResetFuncData(NULL),
	m_pDirect3D9(NULL),
	m_pDirect3DDevice9(NULL),
	m_pD3DXMatrixStack(NULL),
	m_pDefaultRenderTarget(NULL),
	m_pCurrentRenderTarget(NULL),
	m_AdapterMonitor(NULL),
	m_bIgnoreSizeChange(FALSE),

	m_uFullScreenBackBufferWidthAtModeChange(0),
	m_uFullScreenBackBufferHeightAtModeChange(0),
	m_uWindowBackBufferWidthAtModeChange(0),
	m_uWindowBackBufferHeightAtModeChange(0),
	m_uWindowedStyleAtModeChange(FALSE),
	m_bTopmostWhileWindowed(FALSE),
	m_bDeviceObjectsReset(FALSE),
	m_bDeviceObjectsCreated(FALSE),
	m_bDeviceLost(FALSE),
	m_bInsideDeviceCallback(FALSE),
	m_bDeviceCreated(FALSE),
	m_bActive(TRUE),
	m_bAutoChangeAdapter(TRUE),

	m_bLostDevice(FALSE),
	m_bPower2Textures(FALSE),
	m_bMultiRenderTargets(FALSE),
	m_bSeparateAlphaBlend(FALSE),
	m_bSceneBegun(FALSE),
	m_bFullScreenGamma(FALSE),
	m_bCanCalibrateGamma(FALSE),
	m_bRasterSlopeScaleDepthBias(FALSE),
	m_bRasterDepthBias(FALSE),
	m_bIsnVidia(FALSE),
	m_bIsATI(FALSE),
	m_bIsIntel(FALSE),
	m_bShowCursorWhenFullScreen(FALSE),
	m_bClipCursorWhenFullScreen(FALSE),
	m_uBackBufferFormat(0),
	m_uMaxTextureWidth(0),
	m_uMaxTextureHeight(0),
	m_uMaxTextureStages(0),
	m_uMaxTextureSamplers(0),
	m_uMaxPossibleAnisotropy(0),
	m_uClearColor(0),
	m_uMatrixStackDepth(0),
	m_fClearDepth(1.0f)
{
	// Safety switch to verify the declaration of m_WindowPlacement matches the
	// real thing
	BURGER_STATIC_ASSERT(sizeof(m_WindowPlacement) == sizeof(WINDOWPLACEMENT));
	BURGER_STATIC_ASSERT(sizeof(m_D3DSurfaceDesc) == sizeof(D3DSURFACE_DESC));
	BURGER_STATIC_ASSERT(sizeof(m_D3DCaps) == sizeof(D3DCAPS9));
	MemoryClear(&m_D3D9Settings, sizeof(m_D3D9Settings));
}

//
// Shutdown
//

Burger::DisplayDirectX9::~DisplayDirectX9()
{
	Shutdown();
}

//
// Initialize a Direct3D9 context
//

uint_t Burger::DisplayDirectX9::Init(
	uint_t uWidth, uint_t uHeight, uint_t uDepth, uint_t uFlags)
{
	// Set the new size of the screen

	uFlags |= FULLPALETTEALLOWED;

	//
	// Create my directx9 instance and store it
	//

	IDirect3D9* pDirect3D9 = LoadDirect3D9();

	//
	// Was on already in existence?
	//
	if (!pDirect3D9) {
		return 10; // Boned?
	}

	// Set up the buffer settings needed for the DirectX 9 device
	m_uFlags = uFlags;
	DeviceSettings_t TheSettings;
	TheSettings.ApplyDefaults();
	TheSettings.m_pDeviceWindow = m_pGameApp->GetWindow();
	TheSettings.m_bWindowed = (uFlags & FULLSCREEN) == 0;
	TheSettings.m_uBackBufferWidth = uWidth;
	TheSettings.m_uBackBufferHeight = uHeight;
	if (uFlags & MULTITHREADED) {
		TheSettings.m_uBehaviorFlags |= D3DCREATE_MULTITHREADED;
	}

	SetWidthHeight(uWidth, uHeight);

	// Create a DirectX 9 device for this setting
	uint_t uResult = ChangeDevice(&TheSettings, FALSE, FALSE);
	if (!uResult) {

		// Save the states
		SetWidthHeight(m_D3D9Settings.m_uBackBufferWidth,
			m_D3D9Settings.m_uBackBufferHeight);
		m_uDepth = uDepth;
	}
	return uResult;

#if 0	
	//
	// Get the D3D matrix stack
	//

	if (!m_pD3DXMatrixStack) {
		if (Windows::D3DXCreateMatrixStack(0,&m_pD3DXMatrixStack)!=D3D_OK) {
			return 10;		// Boned?
		}
	}

	//
	// Create my D3D device
	//

	D3DPRESENT_PARAMETERS DisplayPresentParms;
	MemoryClear(&DisplayPresentParms,sizeof(DisplayPresentParms));

	// Shared values
	DisplayPresentParms.BackBufferWidth = m_uWidth;
	DisplayPresentParms.BackBufferHeight = m_uHeight;
	DisplayPresentParms.BackBufferCount = 1;
	DisplayPresentParms.MultiSampleType = D3DMULTISAMPLE_NONE;
	DisplayPresentParms.MultiSampleQuality = 0;
	DisplayPresentParms.SwapEffect = D3DSWAPEFFECT_DISCARD;
	DisplayPresentParms.hDeviceWindow = m_pGameApp->GetWindow();
	DisplayPresentParms.EnableAutoDepthStencil = TRUE;
	DisplayPresentParms.AutoDepthStencilFormat = D3DFMT_D24S8;		// 24 bit depth, 8 bit stencil
	DisplayPresentParms.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	DisplayPresentParms.PresentationInterval = D3DPRESENT_INTERVAL_ONE;

	if (m_uFlags & FULLSCREEN) {
		DisplayPresentParms.Windowed = FALSE;
		DisplayPresentParms.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;	// Insert requested refresh rate
		DisplayPresentParms.BackBufferFormat = D3DFMT_A8R8G8B8;
	} else {
		DisplayPresentParms.Windowed = TRUE;
		DisplayPresentParms.FullScreen_RefreshRateInHz = 0;
		DisplayPresentParms.BackBufferFormat = D3DFMT_UNKNOWN;	// Always set to unknown for windowed mode!!!
	}

	DWORD uDisplayFlags = D3DCREATE_FPU_PRESERVE;
	// Does it support hardware transformation?
	if (CurrentDeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) {
		// Check against for version 1.1 or higher
		if (LOWORD(CurrentDeviceCaps.VertexShaderVersion) < ((1<<8)+1)) {
			// No vertex shader support? Use software and hardware
			uDisplayFlags |= D3DCREATE_MIXED_VERTEXPROCESSING;
		} else {
			// Use hardware only
			uDisplayFlags |= D3DCREATE_HARDWARE_VERTEXPROCESSING;
		}
	} else {
		// Use SSE2 enhanced vertex processing instead of hardware
		uDisplayFlags |= D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}
	if (m_uFlags&MULTITHREADED) {
		uDisplayFlags |= D3DCREATE_MULTITHREADED;
	}
	

	// Store the default render target
	m_pDirect3DDevice9->GetRenderTarget(0,&m_pDefaultRenderTarget);
	m_pCurrentRenderTarget = NULL;
	return 0;
#endif
}

//
// Release the Direct3D context
//

void Burger::DisplayDirectX9::Shutdown(void)
{
	DisplayObject::ReleaseAll(this);

	//
	// Release the allocated data
	//

	if (m_pD3DXMatrixStack) {
		m_pD3DXMatrixStack->Release();
		m_pD3DXMatrixStack = NULL;
		m_uMatrixStackDepth = 0;
	}

	ReleaseRenderTargets();
	Cleanup3DEnvironment(TRUE);
	if (m_pDirect3D9) {
		m_pDirect3D9->Release();
		m_pDirect3D9 = NULL;
	}
}

//
// Start a D3D scene
//

void Burger::DisplayDirectX9::BeginScene(void)
{
	if (!m_bSceneBegun) {
		// Was the device lost, in pause or minimized?
		if (m_bDeviceLost || (m_iPauseRenderingCount > 0) || !m_bActive) {
			// Sleep instead of rendering
			::Sleep(50);
		}

		// If no device exists, try to create it.

		IDirect3DDevice9* pDirect3DDevice9 = m_pDirect3DDevice9;
		if (!pDirect3DDevice9) {
			if (m_bDeviceLost) {
				DeviceSettings_t TempSettings;
				MemoryCopy(
					&TempSettings, &m_D3D9Settings, sizeof(TempSettings));
				ChangeDevice(&TempSettings, FALSE, TRUE);
			}
			// Exit, because the game has to cycle
			return;
		}

		// Try to reset a display
		HRESULT hResult;
		if (m_bDeviceLost && !m_bRenderingPaused) {
			// Test the cooperative level to see if it's okay to render.
			hResult = pDirect3DDevice9->TestCooperativeLevel();
			if (hResult < 0) {
				if (hResult == D3DERR_DEVICELOST) {
					// The device has been lost but cannot be reset at this
					// time. So wait until it can be reset.
					return;
				}

				// If we are windowed, read the desktop format and
				// ensure that the Direct3D device is using the same format
				// since the user could have changed the desktop bitdepth
				if (m_D3D9Settings.m_bWindowed) {
					D3DDISPLAYMODE adapterDesktopDisplayMode;
					DeviceSettings_t DeviceSettings;
					MemoryCopy(&DeviceSettings, &m_D3D9Settings,
						sizeof(DeviceSettings));
					m_pDirect3D9->GetAdapterDisplayMode(
						DeviceSettings.m_uAdapterOrdinal,
						&adapterDesktopDisplayMode);
					if (DeviceSettings.m_uAdapterFormat !=
						static_cast<uint_t>(adapterDesktopDisplayMode.Format)) {
						DeviceSettings.m_uAdapterFormat = static_cast<uint_t>(
							adapterDesktopDisplayMode.Format);

						hResult =
							static_cast<HRESULT>(SnapDeviceSettingsToEnumDevice(
								&DeviceSettings, FALSE));
						if (hResult < 0) { // the call will fail if no valid
										   // devices were found
						}

						// Change to a Direct3D device created from the new
						// device settings. If there is an existing device, then
						// either reset or recreate the scene
						hResult = static_cast<HRESULT>(
							ChangeDevice(&DeviceSettings, FALSE, FALSE));
						if (hResult) {
							// If this fails, try to go fullscreen and if this
							// fails also shutdown.
							if (ToggleFullScreen()) {
							}
						}

						return;
					}
				}

				// Try to reset the device
				hResult = static_cast<HRESULT>(Reset3DEnvironment());
				if (hResult < 0) {
					if (D3DERR_DEVICELOST == hResult) {
						// The device was lost again, so continue waiting until
						// it can be reset.
						return;
					} else {
						// Reset failed, but the device wasn't lost so something
						// bad happened, so recreate the device to try to
						// recover
						DeviceSettings_t DeviceSettings;
						MemoryCopy(&DeviceSettings, &m_D3D9Settings,
							sizeof(DeviceSettings_t));
						if (ChangeDevice(&DeviceSettings, TRUE, FALSE)) {
							return;
						}
					}
				}
			}
			// Not lost anymore
			m_bDeviceLost = FALSE;
		}

		// Only start if rendering was enabled
		if (!m_bRenderingPaused) {
			// Start a scene
			hResult = pDirect3DDevice9->BeginScene();
			if (hResult == D3D_OK) {
				m_bSceneBegun = TRUE;
			}
		}
	}
}

//
// End a D3D scene and present it to the display
//

void Burger::DisplayDirectX9::EndScene(void)
{
	// Display the final result!
	IDirect3DDevice9* pDirect3DDevice9 = m_pDirect3DDevice9;
	if (pDirect3DDevice9) {

		// Was BeginScene called?
		if (m_bSceneBegun) {
			// Clean up
			pDirect3DDevice9->EndScene();
			m_bSceneBegun = FALSE;
		}

		// Present the scene for rendering
		HRESULT hResult = pDirect3DDevice9->Present(NULL, NULL, NULL, NULL);

		// Uh, oh... The device was lost, force a restart next time around

		if (hResult == D3DERR_DEVICELOST) {
			m_bDeviceLost = TRUE;
		} else if (hResult == D3DERR_DRIVERINTERNALERROR) {
			// Treat internal error as a lost device and reset
			// accordingly
			m_bDeviceLost = TRUE;
		}
	}
}

Burger::Texture* Burger::DisplayDirectX9::CreateTextureObject(void)
{
	return new (Alloc(sizeof(TextureDirectX9))) TextureDirectX9;
}

Burger::VertexBuffer* Burger::DisplayDirectX9::CreateVertexBufferObject(void)
{
	return new (Alloc(sizeof(VertexBufferDirectX9))) VertexBufferDirectX9;
}

void Burger::DisplayDirectX9::Resize(uint_t /* uWidth */, uint_t /* uHeight */)
{
	//	if (m_pDirect3DDevice9) {
	//		m_uWidth = uWidth;
	//		m_uHeight = uHeight;
	//		SetViewport(0,0,uWidth,uHeight);
	//	}
}

void Burger::DisplayDirectX9::SetViewport(
	uint_t uX, uint_t uY, uint_t uWidth, uint_t uHeight)
{
	IDirect3DDevice9* pDevice = m_pDirect3DDevice9;
	if (pDevice) {
		D3DVIEWPORT9 Temp;
		pDevice->GetViewport(&Temp);
		Temp.X = uX;
		Temp.Y = uY;
		Temp.Width = uWidth;
		Temp.Height = uHeight;
		pDevice->SetViewport(&Temp);
	}
}

void Burger::DisplayDirectX9::SetScissorRect(
	uint_t uX, uint_t uY, uint_t uWidth, uint_t uHeight)
{
	RECT Temp;
	Temp.left = static_cast<LONG>(uX);
	Temp.top = static_cast<LONG>(uY);
	Temp.right = static_cast<LONG>(uX + uWidth);
	Temp.bottom = static_cast<LONG>(uY + uHeight);
	m_pDirect3DDevice9->SetScissorRect(&Temp);
}

void Burger::DisplayDirectX9::SetClearColor(
	float fRed, float fGreen, float fBlue, float fAlpha)
{
	m_uClearColor = D3DCOLOR_COLORVALUE(fRed, fGreen, fBlue, fAlpha);
}

void Burger::DisplayDirectX9::SetClearDepth(float fDepth)
{
	m_fClearDepth = fDepth;
}

void Burger::DisplayDirectX9::Clear(uint_t uMask)
{
	uint_t uDXMask = 0;
	if (uMask & CLEAR_COLOR) {
		uDXMask = D3DCLEAR_TARGET;
	}
	if (uMask & CLEAR_DEPTH) {
		uDXMask |= D3DCLEAR_ZBUFFER;
	}
	if (uMask & CLEAR_STENCIL) {
		uDXMask |= D3DCLEAR_STENCIL;
	}
	m_pDirect3DDevice9->Clear(
		0, NULL, uDXMask, m_uClearColor, m_fClearDepth, 0);
}

void Burger::DisplayDirectX9::Bind(Texture* pTexture, uint_t uIndex)
{
	BURGER_ASSERT(uIndex < BURGER_ARRAYSIZE(m_pBoundTextures));
	m_pBoundTextures[uIndex] = pTexture;

	// Get the texture ID
	IDirect3DDevice9* pDevice = m_pDirect3DDevice9;
	if (!pTexture) {
		pDevice->SetTexture(uIndex, NULL);
	} else {
		pTexture->CheckLoad(this);
		pDevice->SetSamplerState(
			0, D3DSAMP_ADDRESSU, g_Wrapping[pTexture->GetWrappingS()]);
		pDevice->SetSamplerState(
			0, D3DSAMP_ADDRESSV, g_Wrapping[pTexture->GetWrappingT()]);
		pDevice->SetSamplerState(
			0, D3DSAMP_MINFILTER, g_Filter[pTexture->GetMinFilter()]);
		pDevice->SetSamplerState(
			0, D3DSAMP_MAGFILTER, g_Filter[pTexture->GetMagFilter()]);
	}
}

void Burger::DisplayDirectX9::Bind(Effect* pEffect)
{
	IDirect3DDevice9* pDevice = m_pDirect3DDevice9;
	if (!pEffect) {
		pDevice->SetPixelShader(NULL);
		pDevice->SetVertexShader(NULL);
	} else {
		pEffect->CheckLoad(this);
		pDevice->SetPixelShader(pEffect->GetPixelShaderDX9());
		pDevice->SetVertexShader(pEffect->GetVertexShaderDX9());
	}
}

void Burger::DisplayDirectX9::SetBlend(uint_t bEnable)
{
	m_pDirect3DDevice9->SetRenderState(D3DRS_ALPHABLENDENABLE, bEnable != 0);
}

void Burger::DisplayDirectX9::SetBlendFunction(
	eSourceBlendFactor uSourceFactor, eDestinationBlendFactor uDestFactor)
{
	BURGER_ASSERT(uSourceFactor < BURGER_ARRAYSIZE(g_SourceBlend));
	BURGER_ASSERT(uDestFactor < BURGER_ARRAYSIZE(g_DestBlend));
	IDirect3DDevice9* pDevice = m_pDirect3DDevice9;
	pDevice->SetRenderState(D3DRS_SRCBLEND, g_SourceBlend[uSourceFactor]);
	pDevice->SetRenderState(D3DRS_DESTBLEND, g_DestBlend[uDestFactor]);
}

void Burger::DisplayDirectX9::SetLighting(uint_t bEnable)
{
	m_pDirect3DDevice9->SetRenderState(D3DRS_LIGHTING, bEnable != 0);
}

void Burger::DisplayDirectX9::SetZWrite(uint_t bEnable)
{
	m_pDirect3DDevice9->SetRenderState(D3DRS_ZWRITEENABLE, bEnable != 0);
}

void Burger::DisplayDirectX9::SetDepthTest(eDepthFunction uDepthFunction)
{
	BURGER_ASSERT(uDepthFunction < BURGER_ARRAYSIZE(g_WriteFunction));
	m_pDirect3DDevice9->SetRenderState(
		D3DRS_ZFUNC, g_WriteFunction[uDepthFunction]);
}

void Burger::DisplayDirectX9::SetCullMode(eCullMode uCullMode)
{
	BURGER_ASSERT(uCullMode < BURGER_ARRAYSIZE(g_CullOperation));
	m_pDirect3DDevice9->SetRenderState(
		D3DRS_CULLMODE, g_CullOperation[uCullMode]);
}

void Burger::DisplayDirectX9::SetScissor(uint_t bEnable)
{
	m_pDirect3DDevice9->SetRenderState(D3DRS_SCISSORTESTENABLE, bEnable != 0);
}

void Burger::DisplayDirectX9::DrawPrimitive(
	ePrimitiveType uPrimitiveType, VertexBuffer* pVertexBuffer)
{
	pVertexBuffer->CheckLoad(this);
	IDirect3DDevice9* pDevice = m_pDirect3DDevice9;
	pDevice->SetVertexDeclaration(pVertexBuffer->GetDX9VertexDescription());
	pDevice->SetStreamSource(
		0, pVertexBuffer->GetDX9VertexBuffer(), 0, pVertexBuffer->GetStride());
	uint_t uCount = pVertexBuffer->GetArrayEntryCount();
	switch (uPrimitiveType) {
	case PRIM_POINTS:
	default:
		break;
	case PRIM_LINES:
		uCount >>= 1;
		break;
	case PRIM_LINESTRIP:
	case PRIM_TRIANGLEFAN:
		--uCount;
		break;
	case PRIM_TRIANGLES:
		uCount /= 3;
		break;
	case PRIM_TRIANGLESTRIP:
		uCount -= 2;
		break;
	}
	pDevice->DrawPrimitive(g_Prims[uPrimitiveType], 0, uCount);
}

void Burger::DisplayDirectX9::DrawElements(
	ePrimitiveType uPrimitiveType, VertexBuffer* pVertexBuffer)
{
	pVertexBuffer->CheckLoad(this);
	IDirect3DDevice9* pDevice = m_pDirect3DDevice9;
	pDevice->SetVertexDeclaration(pVertexBuffer->GetDX9VertexDescription());
	pDevice->SetStreamSource(
		0, pVertexBuffer->GetDX9VertexBuffer(), 0, pVertexBuffer->GetStride());
	uint_t uCount = pVertexBuffer->GetArrayEntryCount();
	switch (uPrimitiveType) {
	case PRIM_POINTS:
	default:
		break;
	case PRIM_LINES:
		uCount >>= 1;
		break;
	case PRIM_LINESTRIP:
	case PRIM_TRIANGLEFAN:
		--uCount;
		break;
	case PRIM_TRIANGLES:
		uCount /= 3;
		break;
	case PRIM_TRIANGLESTRIP:
		uCount -= 2;
		break;
	}
	pDevice->DrawIndexedPrimitive(g_Prims[uPrimitiveType], 0, 0,
		pVertexBuffer->GetArrayEntryCount(), 0, uCount);
}

IDirect3DVertexShader9* BURGER_API Burger::DisplayDirectX9::CreateVertexShader(
	const void* pVertexShaderBinary) const
{
	IDirect3DVertexShader9* pResult = 0;
	HRESULT hr = m_pDirect3DDevice9->CreateVertexShader(
		static_cast<const DWORD*>(pVertexShaderBinary), &pResult);
	if (hr != D3D_OK) {
		pResult = 0;
	}
	return pResult;
}

IDirect3DPixelShader9* BURGER_API Burger::DisplayDirectX9::CreatePixelShader(
	const void* pPixelShaderBinary) const
{
	IDirect3DPixelShader9* pResult = 0;
	HRESULT hr = m_pDirect3DDevice9->CreatePixelShader(
		static_cast<const DWORD*>(pPixelShaderBinary), &pResult);
	if (hr != D3D_OK) {
		pResult = 0;
	}
	return pResult;
}

/*! ************************************

	\fn const D3DSURFACE_DESC
*Burger::DisplayDirectX9::GetBackBufferSurfaceDesc9(void) const

	\brief Return the pointer to a D3DSURFACE_DESC for the current device

	\windowsonly

	\return Pointer to a D3DSURFACE_DESC structure

***************************************/

/*! ************************************

	\fn const D3DCAPS9 *Burger::DisplayDirectX9::GetCaps(void) const

	\brief Return the pointer to a D3DCAPS9 for the current device

	\windowsonly

	\return Pointer to a D3DCAPS9 structure

***************************************/

/*! ************************************

	\brief Create an IDirect3D9 instance

	If an IDirect3D9 instance was already created, return
	a pointer to the pre-existing instance, otherwise,
	create one and set it as the default.

	\windowsonly
	\return The shared IDirect3D9 instance or \ref NULL if DirectX 9
		was not available.

***************************************/

IDirect3D9* BURGER_API Burger::DisplayDirectX9::LoadDirect3D9(void)
{
	//
	// Was on already in existence?
	//

	IDirect3D9* pDirect3D9 = m_pDirect3D9;
	if (!pDirect3D9) {
		// Create it
		pDirect3D9 = Win32::Direct3DCreate9(D3D_SDK_VERSION);
		// Store the instance (Or NULL if it failed)
		m_pDirect3D9 = pDirect3D9;
	}
	return pDirect3D9;
}

/*! ************************************

	\brief Set all of DirectX 9's rendering states to defaults

	When DirectX 9 is started up, this function is called to ensure
	that the rendering state is known.

	\note If a DirectX 9 device was not already started, this function does
nothing.

	States set:
	* * Vertex shader set to \ref NULL
	* * FVF set to D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1
	* * D3DRS_ZENABLE set to FALSE
	* * D3DRS_CULLMODE set to D3DCULL_NONE
	* * D3DRS_LIGHTING set to FALSE
	* * Texture state 0 D3DTSS_COLOROP = D3DTOP_MODULATE
	* *	D3DTSS_COLORARG1 = D3DTA_TEXTURE
	* * D3DTSS_COLORARG2 = D3DTA_DIFFUSE
	* * Alpha state 0 D3DTSS_ALPHAOP = D3DTOP_MODULATE
	* * D3DTSS_ALPHAARG1 = D3DTA_TEXTURE
	* * D3DTSS_ALPHAARG2 = D3DTA_DIFFUSE
	* * D3DRS_SEPARATEALPHABLENDENABLE set to TRUE
	* * Texture state 1 D3DTSS_COLOROP = D3DTOP_DISABLE
	* * D3DTSS_ALPHAOP = D3DTOP_DISABLE
	* * Matrix D3DTS_WORLD set to identity
	* * Matrix D3DTS_VIEW set to identity

	\windowsonly

***************************************/

void BURGER_API Burger::DisplayDirectX9::InitState(void)
{
	IDirect3DDevice9* pDirect3DDevice9 = m_pDirect3DDevice9;
	if (pDirect3DDevice9) {
		pDirect3DDevice9->SetVertexShader(NULL);
		pDirect3DDevice9->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1);
		pDirect3DDevice9->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);
		pDirect3DDevice9->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		pDirect3DDevice9->SetRenderState(D3DRS_LIGHTING, FALSE);

		// Enable color modulation by diffuse color
		pDirect3DDevice9->SetTextureStageState(
			0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		pDirect3DDevice9->SetTextureStageState(
			0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		pDirect3DDevice9->SetTextureStageState(
			0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);

		// Enable alpha modulation by diffuse alpha
		pDirect3DDevice9->SetTextureStageState(
			0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
		pDirect3DDevice9->SetTextureStageState(
			0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		pDirect3DDevice9->SetTextureStageState(
			0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

		// Enable separate alpha blend function, if possible
		if (m_bSeparateAlphaBlend) {
			pDirect3DDevice9->SetRenderState(
				D3DRS_SEPARATEALPHABLENDENABLE, TRUE);
		}

		// Disable second texture stage, since we're done
		pDirect3DDevice9->SetTextureStageState(
			1, D3DTSS_COLOROP, D3DTOP_DISABLE);
		pDirect3DDevice9->SetTextureStageState(
			1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);

		// Set an identity world and view matrix

		pDirect3DDevice9->SetTransform(D3DTS_WORLD,
			reinterpret_cast<const D3DMATRIX*>(&s_Matrix4DIdentity));
		pDirect3DDevice9->SetTransform(D3DTS_VIEW,
			reinterpret_cast<const D3DMATRIX*>(&s_Matrix4DIdentity));
	}
}

/*! ************************************

	\brief Get the adapter ordinal from HMONITOR

	Given a HMONITOR pointer, return the ordinal index
	for that display adapter.

	\windowsonly
	\param hMonitor The HMONITOR value for the monitor to find
	\param pAdapterOrdinal Pointer to a value to receive the new adapter ordinal
	\return Zero on success, non-zero on failure

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::GetAdapterOrdinalFromMonitor(
	HMONITOR__* hMonitor, uint_t* pAdapterOrdinal)
{
	UpdateEnumeration(FALSE);

	const SimpleArray<AdapterInfo*>* pAdapterList =
		m_Enumerator.GetAdapterInfoList();
	uint_t uAdapterOrdinal = 0;
	uint_t uResult = static_cast<uint_t>(E_FAIL);
	uintptr_t uCount = pAdapterList->size();
	if (uCount) {
		AdapterInfo* const* ppAdapterInfo = pAdapterList->GetPtr();
		do {
			const AdapterInfo* pAdapterInfo = ppAdapterInfo[0];
			++ppAdapterInfo;
			// Get the monitor value
			HMONITOR hAdapterMonitor = m_pDirect3D9->GetAdapterMonitor(
				pAdapterInfo->GetAdapterOrdinal());
			// Match?
			if (hAdapterMonitor == hMonitor) {
				uAdapterOrdinal = pAdapterInfo->GetAdapterOrdinal();
				uResult = 0;
				break;
			}
		} while (--uCount);
	}
	pAdapterOrdinal[0] = uAdapterOrdinal;
	return uResult;
}

/*! ************************************

	\brief Get the desktop resolution of a display

	Given an ordinal index, return the resolution of the display
	as set for the desktop.

	\windowsonly
	\param uAdapterOrdinal Ordinal value for which adapter to query
	\param pWidth Pointer to where to store the width in pixels, \ref NULL will
disable width retrieval. \param pHeight Pointer to where to store the height in
pixels, \ref NULL will disable height retrieval. \return Zero on success,
non-zero on failure

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::GetDesktopResolution(
	uint_t uAdapterOrdinal, uint_t* pWidth, uint_t* pHeight)
{
	// Scan devices
	UpdateEnumeration(FALSE);
	const AdapterInfo* pAdapterInfo =
		m_Enumerator.GetAdapterInfo(uAdapterOrdinal);

	// Obtain the GDI name of the device
	char TempDeviceName[256];
	TempDeviceName[0] = 0;
	if (pAdapterInfo) {
		StringCopy(TempDeviceName, sizeof(TempDeviceName),
			pAdapterInfo->GetAdapterIdentifier()->DeviceName);
	}

	// Query the device
	DEVMODEA TempMode;
	MemoryClear(&TempMode, sizeof(TempMode));
	TempMode.dmSize = sizeof(TempMode);
	uint_t uResult = 0;

	if (!EnumDisplaySettingsA(
			TempDeviceName, ENUM_REGISTRY_SETTINGS, &TempMode)) {
		// Failed!
		uResult = 11;
	}

	// Store the results
	if (pWidth) {
		pWidth[0] = TempMode.dmPelsWidth;
	}
	if (pHeight) {
		pHeight[0] = TempMode.dmPelsHeight;
	}
	return uResult;
}

/*! ************************************

	\brief Enumerate devices if not already cached

	If the devices haven't already been or if the bForce flag is
	set, enumerate all display devices and adapters

	\windowsonly

	\param bForce \ref TRUE to force enumeration.

***************************************/

void BURGER_API Burger::DisplayDirectX9::UpdateEnumeration(uint_t bForce)
{
	if (bForce || !m_Enumerator.HasEnumerated()) {
		// Make sure the Direct3D9 instance was created
		IDirect3D9* pIDirect3D9 = LoadDirect3D9();
		if (pIDirect3D9) {
			m_Enumerator.Enumerate(
				pIDirect3D9, m_pDeviceFilter, m_pDeviceFilterData);
		}
	}
}

/*! ************************************

	\brief Load the back buffer description from the DirectX 9 device

	Cache the D3DSURFACE_DESC from the current DirectX 9
	device. If a DirectX 9 device was not started, or
	if an error occurred when the device was queried, this
	function will clear the internal cache.

	\windowsonly

***************************************/

void BURGER_API Burger::DisplayDirectX9::UpdateBackBufferDesc(void)
{
	IDirect3DSurface9* pBackBuffer = NULL;
	if (m_pDirect3DDevice9 &&
		(m_pDirect3DDevice9->GetBackBuffer(
			 0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer) >= 0)) {
		D3DSURFACE_DESC* pBBufferSurfaceDesc =
			const_cast<D3DSURFACE_DESC*>(GetBackBufferSurfaceDesc9());
		pBackBuffer->GetDesc(pBBufferSurfaceDesc);
		pBackBuffer->Release();
	} else {
		MemoryClear(m_D3DSurfaceDesc, sizeof(m_D3DSurfaceDesc));
	}
}

/*! ************************************

	\brief Toggle full screen and windowed mode

	If the game is running in a window, switch to full screen
	and vice versa.

	\windowsonly
	\return Zero on success, non-zero with error code

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::ToggleFullScreen(void)
{
	// Copy the settings for modification
	DeviceSettings_t NewSettings;
	MemoryCopy(&NewSettings, &m_D3D9Settings, sizeof(NewSettings));

	// Toggle full screen / windowed
	uint_t uResult = 0;
	if (NewSettings.m_bWindowed) {
		D3DDISPLAYMODE DesktopMode;
		HRESULT hr = m_pDirect3D9->GetAdapterDisplayMode(
			m_D3D9Settings.m_uAdapterOrdinal, &DesktopMode);
		if (hr >= 0) {
			NewSettings.m_bWindowed = FALSE;
			NewSettings.m_uBackBufferWidth = DesktopMode.Width;
			NewSettings.m_uBackBufferHeight = DesktopMode.Height;
			NewSettings.m_uBackBufferFormat = DesktopMode.Format;
		} else {
			uResult = static_cast<uint_t>(hr);
		}
	} else {
		// Get the window size
		NewSettings.m_bWindowed = TRUE;
		NewSettings.m_uBackBufferWidth = m_uWindowBackBufferWidthAtModeChange;
		NewSettings.m_uBackBufferHeight = m_uWindowBackBufferHeightAtModeChange;
		NewSettings.m_uFullScreen_RefreshRateInHz =
			0; // Windows don't use refresh rates
	}

	// No error in getting the desktop mode?
	if (!uResult) {

		// Save the settings to restore video
		DeviceSettings_t BackupSettings;
		MemoryCopy(&BackupSettings, &m_D3D9Settings, sizeof(BackupSettings));

		// Toggle modes
		uResult = ChangeDevice(&NewSettings, FALSE, FALSE);

		// If uResult == E_ABORT, then the settings were rejected, set things
		// back
		if (uResult && (uResult != static_cast<uint_t>(E_ABORT))) {
			// Failed creating device, try to switch back.
			if (ChangeDevice(&BackupSettings, FALSE, FALSE)) {
				// If this failed, then shutdown
				Shutdown();
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Toggle hardware and software renderer

	If the game is running in with a GPU, switch to
	software and vice versa

	\windowsonly
	\return Zero on success, non-zero with error code

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::ToggleREF(void)
{
	DeviceSettings_t NewSettings;
	MemoryCopy(&NewSettings, &m_D3D9Settings, sizeof(NewSettings));

	// Toggle between REF & HAL
	uint_t uResult = 0;
	if (NewSettings.m_uDeviceType == D3DDEVTYPE_HAL) {
		NewSettings.m_uDeviceType = D3DDEVTYPE_REF;
	} else if (NewSettings.m_uDeviceType == D3DDEVTYPE_REF) {
		NewSettings.m_uDeviceType = D3DDEVTYPE_HAL;
	} else {
		// Not supported
		uResult = static_cast<uint_t>(E_ABORT);
	}

	if (!uResult) {

		// Pick the settings
		uResult = SnapDeviceSettingsToEnumDevice(&NewSettings, FALSE);
		if (!uResult) {

			// Copy the settings for restore
			DeviceSettings_t BackupSettings;
			MemoryCopy(
				&BackupSettings, &m_D3D9Settings, sizeof(BackupSettings));

			// Create a Direct3D device using the new device settings.
			// If there is an existing device, then it will either reset or
			// recreate the scene.
			uResult = ChangeDevice(&NewSettings, FALSE, FALSE);

			// If uResult == E_ABORT, then the settings were rejected, set
			// things back
			if (uResult && (uResult != static_cast<uint_t>(E_ABORT))) {
				// Failed creating device, try to switch back.
				if (ChangeDevice(&BackupSettings, FALSE, FALSE)) {
					// If this failed, then shutdown
					Shutdown();
				}
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Resize the back buffer if the window size changed

	\windowsonly

***************************************/

void Burger::DisplayDirectX9::CheckForWindowSizeChange(void)
{
	// Skip the check for various reasons

	if (!m_bIgnoreSizeChange && m_bDeviceCreated &&
		m_D3D9Settings.m_bWindowed) {

		// Get the new client size
		RECT TheRect;
		GetClientRect(m_D3D9Settings.m_pDeviceWindow, &TheRect);

		// Did it change?
		if (static_cast<uint_t>(TheRect.right) !=
				m_D3D9Settings.m_uBackBufferWidth ||
			static_cast<uint_t>(TheRect.bottom) !=
				m_D3D9Settings.m_uBackBufferHeight) {

			// A new window size will require a new backbuffer size size
			// Tell ChangeDevice and D3D to size according to the HWND's client
			// rect

			DeviceSettings_t NewSettings;
			MemoryCopy(&NewSettings, &m_D3D9Settings, sizeof(NewSettings));
			NewSettings.m_uBackBufferWidth = 0;
			NewSettings.m_uBackBufferHeight = 0;
			ChangeDevice(&NewSettings, FALSE, FALSE);
		}
	}
}

/***************************************

	\brief Check if the window has changed monitors

	\windowsonly

***************************************/

void Burger::DisplayDirectX9::CheckForWindowChangingMonitors(void)
{
	// Skip this check for various reasons
	if (m_bAutoChangeAdapter && !m_bIgnoreSizeChange && m_bDeviceCreated &&
		m_D3D9Settings.m_bWindowed) {

		HMONITOR hWindowMonitor = Win32::MonitorFromWindow(
			m_D3D9Settings.m_pDeviceWindow, MONITOR_DEFAULTTOPRIMARY);
		if (hWindowMonitor != m_AdapterMonitor) {

			uint_t uNewAdapterOrdinal;
			if (!GetAdapterOrdinalFromMonitor(
					hWindowMonitor, &uNewAdapterOrdinal)) {
				// Find the closest valid device settings with the new ordinal
				DeviceSettings_t NewSettings;
				MemoryCopy(&NewSettings, &m_D3D9Settings, sizeof(NewSettings));

				NewSettings.m_uAdapterOrdinal = uNewAdapterOrdinal;
				if (!SnapDeviceSettingsToEnumDevice(&NewSettings, FALSE)) {

					// Create a Direct3D device using the new device settings.
					// If there is an existing device, then it will either reset
					// or recreate the scene.
					uint_t hr = ChangeDevice(&NewSettings, FALSE, FALSE);

					// If hr == E_ABORT, this means the app rejected the device
					// settings in the ModifySettingsCallback
					if (hr == static_cast<uint_t>(E_ABORT)) {
						// Turn off this feature since it got rejected
						m_bAutoChangeAdapter = FALSE;
					} else if (hr) {
						Shutdown();
						Pause(FALSE);
					}
				}
			}
		}
	}
}

/*! ************************************

	\brief Change or initialize a DirectX 9 device

	\windowsonly
	\param pNewSettings Pointer to a description of a display device to use
	\param bForceEnumeration \ref TRUE if enumerating devices is forced to occur
	\param bClipWindowToSingleAdapter \ref TRUE if spanning displays is disabled
	\return Zero on success, non-zero on error

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::ChangeDevice(
	const DeviceSettings_t* pNewSettings, uint_t bForceEnumeration,
	uint_t bClipWindowToSingleAdapter)
{
	WINDOWPLACEMENT TempWindowPlacement;

	// Copy the settings
	DeviceSettings_t NewSettings;
	MemoryCopy(&NewSettings, pNewSettings, sizeof(NewSettings));

	// Look for the closest match (Which may modify the settings)
	uint_t uResult =
		SnapDeviceSettingsToEnumDevice(&NewSettings, bForceEnumeration);
	if (!uResult) {

		// See if the application has installed a callback to reject devices
		// based on criteria

		DeviceSettingsFilterProc pCallbackModifyDeviceSettings =
			m_pModifyDeviceSettingsFunc;
		if (pCallbackModifyDeviceSettings && !m_pDirect3DDevice9) {
			if (!pCallbackModifyDeviceSettings(
					&NewSettings, m_pModifyDeviceSettingsData)) {
				// Use E_ABORT as a signal of rejection
				uResult = static_cast<uint_t>(E_ABORT);
			} else {
				// Do any fix up needed.
				uResult = SnapDeviceSettingsToEnumDevice(&NewSettings, FALSE);
			}
		}

		if (!uResult) {
			// Don't allow rendering while it's changing modes
			Pause(TRUE);

			// Make a copy of the current settings so they could be referred to
			// for reference
			DeviceSettings_t BackupSettings;
			MemoryCopy(
				&BackupSettings, &m_D3D9Settings, sizeof(BackupSettings));

			// Set the new settings
			MemoryCopy(&m_D3D9Settings, &NewSettings, sizeof(m_D3D9Settings));

			// When a WM_SIZE message is received, it calls
			// CheckForWindowSizeChange(). A WM_SIZE message might be sent when
			// adjusting the window, so tell CheckForWindowSizeChange() to
			// ignore size changes temporarily
			m_bIgnoreSizeChange = TRUE;

			// Take note if the backbuffer width & height are 0 now as they will
			// change after m_pDirect3DDevice9->Reset()
			uint_t bKeepCurrentWindowSize = FALSE;
			if (!m_D3D9Settings.m_uBackBufferWidth &&
				!m_D3D9Settings.m_uBackBufferHeight) {
				bKeepCurrentWindowSize = TRUE;
			}

			// Adjust window style when switching from windowed to full screen
			// and vice versa.

			if (m_D3D9Settings.m_bWindowed) {

				// Going to windowed mode
				if (BackupSettings.m_pDeviceWindow &&
					!BackupSettings.m_bWindowed) {
					// Going from full screen -> windowed
					m_uFullScreenBackBufferWidthAtModeChange =
						BackupSettings.m_uBackBufferWidth;
					m_uFullScreenBackBufferHeightAtModeChange =
						BackupSettings.m_uBackBufferHeight;

					// Restore windowed mode style
					SetWindowLongW(m_pGameApp->GetWindow(), GWL_STYLE,
						static_cast<LONG>(m_uWindowedStyleAtModeChange));
				}

				// If using the same window for windowed and full screen mode,
				// reattach menu if one exists
				if (m_pGameApp->GetMenu()) {
					::SetMenu(m_pGameApp->GetWindow(), m_pGameApp->GetMenu());
				}
			} else {
				// Going to full screen mode

				if (!BackupSettings.m_pDeviceWindow ||
					BackupSettings.m_bWindowed) {

					// Transitioning to full screen mode from a standard window
					// so save current window position/size/style now in case
					// the user toggles to windowed mode later

					MemoryClear(m_WindowPlacement, sizeof(m_WindowPlacement));
					WINDOWPLACEMENT* pWindowPlacement = GetWindowedPlacement();
					pWindowPlacement->length = sizeof(WINDOWPLACEMENT);
					GetWindowPlacement(
						m_pGameApp->GetWindow(), pWindowPlacement);

					uint_t bIsTopmost =
						(GetWindowLongW(m_pGameApp->GetWindow(), GWL_EXSTYLE) &
							WS_EX_TOPMOST) != 0;
					m_bTopmostWhileWindowed = bIsTopmost;
					LONG uStyle =
						GetWindowLongW(m_pGameApp->GetWindow(), GWL_STYLE);
					uStyle &= ~(WS_MAXIMIZE |
						WS_MINIMIZE); // remove minimize/maximize style
					m_uWindowedStyleAtModeChange = static_cast<uint_t>(uStyle);
					if (BackupSettings.m_pDeviceWindow) {
						m_uWindowBackBufferWidthAtModeChange =
							BackupSettings.m_uBackBufferWidth;
						m_uWindowBackBufferHeightAtModeChange =
							BackupSettings.m_uBackBufferHeight;
					}
				}

				// Hide the window to avoid animation of blank windows
				ShowWindow(m_pGameApp->GetWindow(), SW_HIDE);

				// Set FS window style
				SetWindowLongW(
					m_pGameApp->GetWindow(), GWL_STYLE, WS_POPUP | WS_SYSMENU);

				// If using the same window for windowed and full screen mode,
				// save and remove menu
				HMENU hMenu = ::GetMenu(m_pGameApp->GetWindow());
				m_pGameApp->SetMenu(hMenu);
				::SetMenu(m_pGameApp->GetWindow(), NULL);

				MemoryClear(&TempWindowPlacement, sizeof(WINDOWPLACEMENT));
				TempWindowPlacement.length = sizeof(WINDOWPLACEMENT);
				GetWindowPlacement(
					m_pGameApp->GetWindow(), &TempWindowPlacement);

				if (TempWindowPlacement.flags & WPF_RESTORETOMAXIMIZED) {
					// Restore the window to normal if the window was maximized
					// then minimized. This causes the WPF_RESTORETOMAXIMIZED
					// flag to be set which will cause SW_RESTORE to restore the
					// window from minimized to maximized which isn't what we
					// want
					TempWindowPlacement.flags &= ~WPF_RESTORETOMAXIMIZED;
					TempWindowPlacement.showCmd = SW_RESTORE;
					SetWindowPlacement(
						m_pGameApp->GetWindow(), &TempWindowPlacement);
				}
			}

			// Release the device if one was allocated

			if (m_pDirect3DDevice9) {
				Cleanup3DEnvironment(FALSE);
			}

			// Create the D3D device and call the app's device callbacks

			uResult = Create3DEnvironment();
			if (uResult) {
				// Abort and zap settings!
				Cleanup3DEnvironment(TRUE);
				Pause(FALSE);
				m_bIgnoreSizeChange = FALSE;
				return uResult;
			}

			// DirectX device was created, find the primary monitor

			HMONITOR hAdapterMonitor = m_pDirect3D9->GetAdapterMonitor(
				m_D3D9Settings.m_uAdapterOrdinal);
			m_AdapterMonitor = hAdapterMonitor;

			// Going from full screen -> windowed
			if (BackupSettings.m_pDeviceWindow && !BackupSettings.m_bWindowed &&
				m_D3D9Settings.m_bWindowed) {

				// Restore the show state, and positions/size of the window to
				// what it was It is important to adjust the window size after
				// resetting the device rather than beforehand to ensure that
				// the monitor resolution is correct and does not limit the size
				// of the new window.
				WINDOWPLACEMENT* pWindowPlacement = GetWindowedPlacement();
				SetWindowPlacement(m_pGameApp->GetWindow(), pWindowPlacement);

				// Also restore the z-order of window to previous state
				HWND hWndInsertAfter =
					m_bTopmostWhileWindowed ? HWND_TOPMOST : HWND_NOTOPMOST;
				SetWindowPos(m_pGameApp->GetWindow(), hWndInsertAfter, 0, 0, 0,
					0, SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE);
			}

			// Check to see if the window needs to be resized.
			// Handle cases where the window is minimized and maximized as well.

			uint_t bNeedToResize = FALSE;
			if (m_D3D9Settings.m_bWindowed && // only resize if in windowed mode
				!bKeepCurrentWindowSize) {    // only resize if
										   // pp.BackbufferWidth/Height were not
										   // 0
				uint_t uClientWidth;
				uint_t uClientHeight;
				if (IsIconic(m_pGameApp->GetWindow())) {
					// Window is currently minimized. To tell if it needs to
					// resize, get the client rect of window when its restored
					// the hard way using GetWindowPlacement()
					MemoryClear(&TempWindowPlacement, sizeof(WINDOWPLACEMENT));
					TempWindowPlacement.length = sizeof(WINDOWPLACEMENT);
					GetWindowPlacement(
						m_pGameApp->GetWindow(), &TempWindowPlacement);

					if ((TempWindowPlacement.flags & WPF_RESTORETOMAXIMIZED) &&
						(TempWindowPlacement.showCmd == SW_SHOWMINIMIZED)) {
						// WPF_RESTORETOMAXIMIZED means that when the window is
						// restored it will be maximized. So maximize the window
						// temporarily to get the client rect when the window is
						// maximized. GetSystemMetrics( SM_CXMAXIMIZED ) will
						// give this information if the window is on the primary
						// but this will work on multimon.
						ShowWindow(m_pGameApp->GetWindow(), SW_RESTORE);
						RECT TheClientRect;
						GetClientRect(m_pGameApp->GetWindow(), &TheClientRect);
						uClientWidth = static_cast<uint_t>(
							TheClientRect.right - TheClientRect.left);
						uClientHeight = static_cast<uint_t>(
							TheClientRect.bottom - TheClientRect.top);
						ShowWindow(m_pGameApp->GetWindow(), SW_MINIMIZE);
					} else {
						// Use wp.rcNormalPosition to get the client rect, but
						// wp.rcNormalPosition includes the window frame so
						// subtract it
						RECT TheFrameRect;
						MemoryClear(&TheFrameRect, sizeof(TheFrameRect));
						AdjustWindowRectEx(&TheFrameRect,
							m_uWindowedStyleAtModeChange,
							m_pGameApp->GetMenu() != NULL,
							static_cast<DWORD>(GetWindowLongPtrW(
								m_pGameApp->GetWindow(), GWL_EXSTYLE)));
						LONG iFrameWidth =
							TheFrameRect.right - TheFrameRect.left;
						LONG iFrameHeight =
							TheFrameRect.bottom - TheFrameRect.top;
						uClientWidth = static_cast<uint_t>(
							TempWindowPlacement.rcNormalPosition.right -
							TempWindowPlacement.rcNormalPosition.left -
							iFrameWidth);
						uClientHeight = static_cast<uint_t>(
							TempWindowPlacement.rcNormalPosition.bottom -
							TempWindowPlacement.rcNormalPosition.top -
							iFrameHeight);
					}
				} else {
					// Window is restored or maximized so just get its client
					// rect
					RECT rcClient;
					GetClientRect(m_pGameApp->GetWindow(), &rcClient);
					uClientWidth =
						static_cast<uint_t>(rcClient.right - rcClient.left);
					uClientHeight =
						static_cast<uint_t>(rcClient.bottom - rcClient.top);
				}

				// Now that we know the client rect, compare it against the back
				// buffer size to see if the client rect is already the right
				// size

				if (uClientWidth != m_D3D9Settings.m_uBackBufferWidth ||
					uClientHeight != m_D3D9Settings.m_uBackBufferHeight) {
					bNeedToResize = TRUE;
				}

				if (bClipWindowToSingleAdapter &&
					!IsIconic(m_pGameApp->GetWindow())) {
					// Get the rect of the monitor attached to the adapter
					MONITORINFO miAdapter;
					miAdapter.cbSize = sizeof(MONITORINFO);

					hAdapterMonitor = m_pDirect3D9->GetAdapterMonitor(
						m_D3D9Settings.m_uAdapterOrdinal);
					Win32::GetMonitorInfo(hAdapterMonitor, &miAdapter);
					HMONITOR hWindowMonitor = Win32::MonitorFromWindow(
						m_pGameApp->GetWindow(), MONITOR_DEFAULTTOPRIMARY);

					// Get the rect of the window
					RECT rcWindow;
					GetWindowRect(m_pGameApp->GetWindow(), &rcWindow);

					// Check if the window rect is fully inside the adapter's
					// virtual screen rect
					if ((rcWindow.left < miAdapter.rcWork.left) ||
						(rcWindow.right > miAdapter.rcWork.right) ||
						(rcWindow.top < miAdapter.rcWork.top) ||
						(rcWindow.bottom > miAdapter.rcWork.bottom)) {
						if (hWindowMonitor == hAdapterMonitor &&
							IsZoomed(m_pGameApp->GetWindow())) {
							// If the window is maximized and on the same
							// monitor as the adapter, then no need to clip to
							// single adapter as the window is already clipped
							// even though the rcWindow rect is outside of the
							// miAdapter.rcWork
						} else {
							bNeedToResize = TRUE;
						}
					}
				}
			}

			// Only resize window if needed

			if (bNeedToResize) {
				// Need to resize, so if window is maximized or minimized then
				// restore the window
				if (IsIconic(m_pGameApp->GetWindow())) {
					ShowWindow(m_pGameApp->GetWindow(), SW_RESTORE);
				}
				if (IsZoomed(m_pGameApp->GetWindow())) {
					// doing the IsIconic() check first also handles the
					// WPF_RESTORETOMAXIMIZED case
					ShowWindow(m_pGameApp->GetWindow(), SW_RESTORE);
				}

				if (bClipWindowToSingleAdapter) {
					// Get the rect of the monitor attached to the adapter
					MONITORINFO miAdapter;
					miAdapter.cbSize = sizeof(MONITORINFO);
					hAdapterMonitor = m_pDirect3D9->GetAdapterMonitor(
						m_D3D9Settings.m_uAdapterOrdinal);
					Win32::GetMonitorInfo(hAdapterMonitor, &miAdapter);

					// Get the rect of the monitor attached to the window
					MONITORINFO miWindow;
					miWindow.cbSize = sizeof(MONITORINFO);
					Win32::GetMonitorInfo(
						Win32::MonitorFromWindow(
							m_pGameApp->GetWindow(), MONITOR_DEFAULTTOPRIMARY),
						&miWindow);

					// Do something reasonable if the BackBuffer size is greater
					// than the monitor size
					LONG iAdapterMonitorWidth =
						miAdapter.rcWork.right - miAdapter.rcWork.left;
					LONG iAdapterMonitorHeight =
						miAdapter.rcWork.bottom - miAdapter.rcWork.top;

					// Get the rect of the window
					RECT rcWindow;
					GetWindowRect(m_pGameApp->GetWindow(), &rcWindow);

					// Make a window rect with a client rect that is the same
					// size as the backbuffer
					RECT rcResizedWindow;
					rcResizedWindow.left = 0;
					rcResizedWindow.right =
						static_cast<LONG>(m_D3D9Settings.m_uBackBufferWidth);
					rcResizedWindow.top = 0;
					rcResizedWindow.bottom =
						static_cast<LONG>(m_D3D9Settings.m_uBackBufferHeight);
					AdjustWindowRectEx(&rcResizedWindow,
						static_cast<DWORD>(
							GetWindowLongW(m_pGameApp->GetWindow(), GWL_STYLE)),
						m_pGameApp->GetMenu() != NULL,
						static_cast<DWORD>(GetWindowLongPtrW(
							m_pGameApp->GetWindow(), GWL_EXSTYLE)));

					LONG iWindowWidth =
						rcResizedWindow.right - rcResizedWindow.left;
					LONG iWindowHeight =
						rcResizedWindow.bottom - rcResizedWindow.top;

					if (iWindowWidth > iAdapterMonitorWidth) {
						iWindowWidth = iAdapterMonitorWidth;
					}
					if (iWindowHeight > iAdapterMonitorHeight) {
						iWindowHeight = iAdapterMonitorHeight;
					}

					if ((rcResizedWindow.left < miAdapter.rcWork.left) ||
						(rcResizedWindow.top < miAdapter.rcWork.top) ||
						(rcResizedWindow.right > miAdapter.rcWork.right) ||
						(rcResizedWindow.bottom > miAdapter.rcWork.bottom)) {
						LONG iWindowOffsetX =
							(iAdapterMonitorWidth - iWindowWidth) / 2;
						LONG iWindowOffsetY =
							(iAdapterMonitorHeight - iWindowHeight) / 2;

						rcResizedWindow.left =
							miAdapter.rcWork.left + iWindowOffsetX;
						rcResizedWindow.top =
							miAdapter.rcWork.top + iWindowOffsetY;
						rcResizedWindow.right = miAdapter.rcWork.left +
							iWindowOffsetX + iWindowWidth;
						rcResizedWindow.bottom = miAdapter.rcWork.top +
							iWindowOffsetY + iWindowHeight;
					}

					// Resize the window. It is important to adjust the window
					// size after resetting the device rather than beforehand to
					// ensure that the monitor resolution is correct and does
					// not limit the size of the new window.
					SetWindowPos(m_pGameApp->GetWindow(), 0,
						rcResizedWindow.left, rcResizedWindow.top, iWindowWidth,
						iWindowHeight, SWP_NOZORDER);
				} else {
					// Make a window rect with a client rect that is the same
					// size as the backbuffer
					RECT rcWindow;
					rcWindow.left = 0;
					rcWindow.right =
						static_cast<LONG>(m_D3D9Settings.m_uBackBufferWidth);
					rcWindow.top = 0;
					rcWindow.bottom =
						static_cast<LONG>(m_D3D9Settings.m_uBackBufferHeight);
					AdjustWindowRectEx(&rcWindow,
						static_cast<DWORD>(
							GetWindowLongW(m_pGameApp->GetWindow(), GWL_STYLE)),
						m_pGameApp->GetMenu() != NULL,
						static_cast<DWORD>(GetWindowLongPtrW(
							m_pGameApp->GetWindow(), GWL_EXSTYLE)));

					// Resize the window. It is important to adjust the window
					// size after resetting the device rather than beforehand to
					// ensure that the monitor resolution is correct and does
					// not limit the size of the new window.
					int cx = static_cast<int>(rcWindow.right - rcWindow.left);
					int cy = static_cast<int>(rcWindow.bottom - rcWindow.top);
					SetWindowPos(m_pGameApp->GetWindow(), HWND_TOP, 0, 0, cx,
						cy, SWP_NOZORDER | SWP_NOMOVE);
				}

				// Its possible that the new window size is not what we asked
				// for. No window can be sized larger than the desktop, so see
				// if the Windows OS resized the window to something smaller to
				// fit on the desktop. Also if WM_GETMINMAXINFO will put a limit
				// on the smallest/largest window size.
				RECT rcClient;
				GetClientRect(m_pGameApp->GetWindow(), &rcClient);
				uint_t uClientWidth =
					static_cast<uint_t>(rcClient.right - rcClient.left);
				uint_t uClientHeight =
					static_cast<uint_t>(rcClient.bottom - rcClient.top);

				if ((uClientWidth != m_D3D9Settings.m_uBackBufferWidth) ||
					(uClientHeight != m_D3D9Settings.m_uBackBufferHeight)) {
					// If its different, then resize the backbuffer again. This
					// time create a backbuffer that matches the client rect of
					// the current window w/o resizing the window.
					DeviceSettings_t SettingsCopy;
					MemoryCopy(
						&SettingsCopy, &m_D3D9Settings, sizeof(m_D3D9Settings));
					SettingsCopy.m_uBackBufferWidth = 0;
					SettingsCopy.m_uBackBufferHeight = 0;

					uResult = ChangeDevice(
						&SettingsCopy, FALSE, bClipWindowToSingleAdapter);
					if (uResult) {
						// Danger Will Robinson! Danger! Danger!
						Cleanup3DEnvironment(TRUE);
						Pause(FALSE);
						m_bIgnoreSizeChange = FALSE;
						return uResult;
					}
				}
			}

			// Make the window visible
			if (!IsWindowVisible(m_pGameApp->GetWindow())) {
				ShowWindow(m_pGameApp->GetWindow(), SW_SHOW);
			}

			// Ensure that the display doesn't power down when full screen but
			// does when windowed
			if (!m_D3D9Settings.m_bWindowed) {
				SetThreadExecutionState(ES_DISPLAY_REQUIRED | ES_CONTINUOUS);
			} else {
				SetThreadExecutionState(ES_CONTINUOUS);
			}

			m_bIgnoreSizeChange = FALSE;
			Pause(FALSE);
			m_bDeviceCreated = TRUE;
			uResult = 0;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Create the D3D Device

	\windowsonly

	\return Zero if no error, non-zero on error

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::Create3DEnvironment(void)
{
	// Try to create the device with the chosen settings
	D3DPRESENT_PARAMETERS NewDeviceSettings;
	m_D3D9Settings.GetPresentParameters(&NewDeviceSettings);

	HRESULT hr = m_pDirect3D9->CreateDevice(m_D3D9Settings.m_uAdapterOrdinal,
		static_cast<D3DDEVTYPE>(m_D3D9Settings.m_uDeviceType),
		NewDeviceSettings.hDeviceWindow, m_D3D9Settings.m_uBehaviorFlags,
		&NewDeviceSettings, &m_pDirect3DDevice9);

	// Capture the modified settings (In case it was a window with width and
	// height of 0)
	m_D3D9Settings.SetPresentParameters(&NewDeviceSettings);

	// Lost device? Post a pending restart
	if (hr == D3DERR_DEVICELOST) {
		m_bDeviceLost = TRUE;
		hr = 0;
	} else if (hr >= 0) {
		// Update back buffer desc before calling app's device callbacks
		UpdateBackBufferDesc();

		// Setup cursor based on current settings (window/fullscreen mode, show
		// cursor state, clip cursor state)
		SetupCursor();

		// Grab a local copy of the active device CAPS
		const D3DCAPS9* pCaps = GetCaps();
		m_pDirect3DDevice9->GetDeviceCaps(const_cast<D3DCAPS9*>(pCaps));

		// Set up values to be queried after device has been selected
		D3DADAPTER_IDENTIFIER9 AdapterIdentifier;
		hr = m_pDirect3D9->GetAdapterIdentifier(
			m_D3D9Settings.m_uAdapterOrdinal, 0, &AdapterIdentifier);
		if (hr != D3D_OK) {
			MemoryClear(&AdapterIdentifier, sizeof(AdapterIdentifier));
		}
		// Capture the vendor ID of the card that's being requested
		m_bIsnVidia = (AdapterIdentifier.VendorId == 0x10DE);
		m_bIsATI = (AdapterIdentifier.VendorId == 0x1002);
		m_bIsIntel = (AdapterIdentifier.VendorId == 0x8086);

		// Save the gamma caps
		m_bFullScreenGamma = (pCaps->Caps2 & D3DCAPS2_FULLSCREENGAMMA) != 0;
		m_bCanCalibrateGamma = (pCaps->Caps2 & D3DCAPS2_CANCALIBRATEGAMMA) != 0;

		// Save the depth bias supported flags
		m_bRasterSlopeScaleDepthBias =
			(pCaps->RasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS) != 0;
		m_bRasterDepthBias =
			(pCaps->RasterCaps & D3DPRASTERCAPS_DEPTHBIAS) != 0;

		// Grab some constants
		m_uMaxTextureStages = pCaps->MaxTextureBlendStages;
		m_uMaxTextureSamplers = pCaps->MaxSimultaneousTextures;
		m_uMaxPossibleAnisotropy = pCaps->MaxAnisotropy;
		m_uMaxTextureWidth = pCaps->MaxTextureWidth;
		m_uMaxTextureHeight = pCaps->MaxTextureHeight;
		m_bMultiRenderTargets = (pCaps->NumSimultaneousRTs >= 2);
		m_bSeparateAlphaBlend =
			(pCaps->PrimitiveMiscCaps & D3DPMISCCAPS_SEPARATEALPHABLEND) != 0;

		//
		// Determine if power of 2 textures are required
		//

		m_bPower2Textures =
			(pCaps->TextureCaps &
				(D3DPTEXTURECAPS_NONPOW2CONDITIONAL | D3DPTEXTURECAPS_POW2)) ==
			D3DPTEXTURECAPS_POW2;

		m_uFlags = (m_uFlags & (~FULLSCREEN)) | m_D3D9Settings.m_bWindowed ?
			INWINDOW :
			FULLSCREEN;
		m_bSceneBegun = FALSE;

		// Update the device stats text
		UpdateEnumeration(FALSE);

		// Call the app's device created callback if non-NULL
		DeviceCreatedProc pCallbackDeviceCreated = m_pDeviceCreatedFunc;
		hr = S_OK;
		if (pCallbackDeviceCreated) {
			m_bInsideDeviceCallback = TRUE;
			hr = static_cast<HRESULT>(pCallbackDeviceCreated(m_pDirect3DDevice9,
				GetBackBufferSurfaceDesc9(), m_pDeviceCreatedFuncData));
			m_bInsideDeviceCallback = FALSE;
		}

		// No error?
		if (!hr) {
			// Did the callback dispose of the device?
			if (!m_pDirect3DDevice9) {
				hr = E_FAIL;
			} else {
				// m_pDeviceCreatedFunc was called successfully
				m_bDeviceObjectsCreated = TRUE;

				// Call the app's device reset callback if non-NULL

				hr = static_cast<HRESULT>(IssueDeviceResetCallback());
				if (!hr) {
					if (!m_pDirect3DDevice9) {
						hr = E_FAIL;
					} else {
						// m_pDeviceResetFuncData successfully called
						m_bDeviceObjectsReset = TRUE;
					}
				}
			}
		}
	}
	return static_cast<uint_t>(hr);
}

/*! ************************************

	\brief Release the D3D device

	This will call the device lost and device destroyed
	callbacks if applicable. Normally, this function
	will not clear out the requested device settings unless
	bClearSettings is set to \ref TRUE.

	\windowsonly
	\param bClearSettings Clear out the video settings if \ref TRUE.

***************************************/

void BURGER_API Burger::DisplayDirectX9::Cleanup3DEnvironment(
	uint_t bClearSettings)
{
	// Only do work if the device is present
	if (m_pDirect3DDevice9) {

		// Process any pending reset commands
		IssueDeviceLostCallback();

		// Was m_pDeviceCreatedFunc called successfully before?
		if (m_bDeviceObjectsCreated) {
			// Callbacks in progress
			m_bInsideDeviceCallback = TRUE;

			// Callback to clear out any resources before
			// disposing of the DirectX 9 device

			CallbackProc pCallbackDeviceDestroyed = m_pDeviceDestroyedFunc;
			if (pCallbackDeviceDestroyed) {
				pCallbackDeviceDestroyed(m_pDeviceDestroyedFuncData);
			}
			// Release all tracked textures
			DisplayObject::ReleaseAll(this);

			m_bDeviceObjectsCreated = FALSE;
			m_bInsideDeviceCallback = FALSE;
		}

		// Release the D3D device

		if (m_pDirect3DDevice9) {
			m_pDirect3DDevice9->Release();
			m_pDirect3DDevice9 = NULL;
		}

		// Don't reset if not required
		if (bClearSettings) {
			MemoryClear(&m_D3D9Settings, sizeof(m_D3D9Settings));
		}
		// No device attached, so clear out the cache
		MemoryClear(m_D3DSurfaceDesc, sizeof(m_D3DSurfaceDesc));
		MemoryClear(m_D3DCaps, sizeof(m_D3DCaps));
		m_bDeviceCreated = FALSE;
	}
}

/*! ************************************

	\brief Reset the D3D device

	This will call the device lost destroyed
	callbacks if applicable and then issue
	the Reset() function on the DirectX 9 device

	\windowsonly
	\return Zero if no error, non-zero on error

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::Reset3DEnvironment(void)
{
	// Issue any callbacks needed before reset
	IssueDeviceLostCallback();
	ReleaseRenderTargets();

	// Reset the device
	D3DPRESENT_PARAMETERS Parms;
	m_D3D9Settings.GetPresentParameters(&Parms);

	HRESULT hr = m_pDirect3DDevice9->Reset(&Parms);
	if (hr >= 0) {
		// Update the parms if anything was changed
		m_D3D9Settings.SetPresentParameters(&Parms);

		// Update back buffer desc before calling app's device callbacks
		UpdateBackBufferDesc();

		// Setup cursor based on current settings (window/fullscreen mode, show
		// cursor state, clip cursor state)
		SetupCursor();

		// Call the app's OnDeviceReset callback

		hr = static_cast<HRESULT>(IssueDeviceResetCallback());

		// m_pDeviceResetFuncData successfully called
		m_bDeviceObjectsReset = TRUE;
		if (hr) {
			// If callback failed, cleanup now
			IssueDeviceLostCallback();
		}
		m_pDirect3DDevice9->GetRenderTarget(0, &m_pDefaultRenderTarget);
		InitState();
	}
	return static_cast<uint_t>(hr);
}

/*! ************************************

	\brief Process the object reset flag.

	If the Object Reset flag was set, handle objects that need to
	be reset internally, and call an application reset function
	if one was present. After this call is complete, the
	Object Reset flag will be cleared.

	This function is called before a Reset() call is issued on the
	DirectX 9 device.

	\windowsonly

***************************************/

void BURGER_API Burger::DisplayDirectX9::IssueDeviceLostCallback(void)
{
	// Was m_pDeviceResetFuncData successfully called?
	if (m_bDeviceObjectsReset) {
		m_bInsideDeviceCallback = TRUE;
		CallbackProc pCallbackDeviceLost = m_pDeviceLostFunc;
		if (pCallbackDeviceLost) {
			pCallbackDeviceLost(m_pDeviceLostFuncData);
		}
		m_bDeviceObjectsReset = FALSE;
		m_bInsideDeviceCallback = FALSE;
	}
}

/*! ************************************

	\brief Call the device was reset callback

	After a device was reset or successfully created, call the
	device reset callback

	\windowsonly
	\return Zero if no error, or non-zero if there was a failure.

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::IssueDeviceResetCallback(void)
{
	uint_t uResult = 0;
	DeviceCreatedProc pCallbackDeviceReset = m_pDeviceResetFunc;
	if (pCallbackDeviceReset) {
		m_bInsideDeviceCallback = TRUE;
		uResult = pCallbackDeviceReset(m_pDirect3DDevice9,
			GetBackBufferSurfaceDesc9(), m_pDeviceResetFuncData);
		m_bInsideDeviceCallback = FALSE;
	}
	return uResult;
}

/*! ************************************

	\brief Enable / Disable hardware cursor

	For full screen mode, the cursor needs to be
	copied from the HCURSOR into a DirectX 9 surface
	and DirectX 9 notified of the update.

	\windowsonly

***************************************/

void BURGER_API Burger::DisplayDirectX9::SetupCursor(void) const
{
	uint_t bIsWindowed = m_D3D9Settings.m_bWindowed;
	HWND pWindow = m_pGameApp->GetWindow();
	// Show the cursor again if returning to full screen
	IDirect3DDevice9* pD3D9Device = m_pDirect3DDevice9;
	if (pD3D9Device && !bIsWindowed) {
		SetCursor(NULL); // Turn off Windows cursor in full screen mode
		if (m_bShowCursorWhenFullScreen) {
			// Convert cursor to a DirectX 9 shape
			HCURSOR hCursor = reinterpret_cast<HCURSOR>(
				GetClassLongPtrW(pWindow, GCLP_HCURSOR));
			SetDeviceCursor(pD3D9Device, hCursor);
			pD3D9Device->ShowCursor(TRUE);
		} else {
			pD3D9Device->ShowCursor(FALSE);
		}
	}

	// Clip cursor if requested
	if (!bIsWindowed && m_bClipCursorWhenFullScreen) {
		// Confine cursor to full screen window
		RECT TheRect;
		GetWindowRect(pWindow, &TheRect);
		ClipCursor(&TheRect);
	} else {
		ClipCursor(NULL);
	}
}

/*! ************************************

	\brief Release render targets

	Release the render targets tracked by this class

	\windowsonly

***************************************/

void BURGER_API Burger::DisplayDirectX9::ReleaseRenderTargets(void)
{
	if (m_pDefaultRenderTarget) {
		m_pDefaultRenderTarget->Release();
		m_pDefaultRenderTarget = NULL;
	}
	if (m_pCurrentRenderTarget) {
		m_pCurrentRenderTarget->Release();
		m_pCurrentRenderTarget = NULL;
	}
}

/*! ************************************

	\brief Find the closest match to a device

	Find the best combination of:
	* * Adapter Ordinal
	* * Device Type
	* * Adapter Format
	* * Back Buffer Format
	* * Windowed

	Given what's available on the system and the match options combined with the
device settings input. This combination of settings is encapsulated by the
BufferFormatGroup class.

	\windowsonly

	\param pDeviceSettings Pointer to a proposed device settings
	\param bForceEnumeration \ref TRUE if enumerating devices is forced to occur

	\return Zero for no error, non-zero on failure

***************************************/

uint_t BURGER_API Burger::DisplayDirectX9::SnapDeviceSettingsToEnumDevice(
	DeviceSettings_t* pDeviceSettings, uint_t bForceEnumeration)
{
	// If this is in a VM, only allow window mode
	if (GetSystemMetrics(SM_REMOTESESSION)) {
		pDeviceSettings->m_bWindowed = TRUE;
	}

	// Enumerate the devices because this code relies on the tables
	// to be generated
	UpdateEnumeration(bForceEnumeration);

	// Get the list of display adapters to choose from
	const SimpleArray<AdapterInfo*>* pAdapterList =
		m_Enumerator.GetAdapterInfoList();

	// Init to defaults
	const BufferFormatGroup* pBestFormatGroup = NULL;
	uintptr_t uBestModeIndex = 0;
	uintptr_t uBestMSAAIndex = 0;

	uintptr_t uAdapterCount = pAdapterList->size();
	if (uAdapterCount) {

		// Rankings are always positive, so -1.0f is "Not initialized"
		float fBestRanking = -1.0f;
		AdapterInfo* const* ppAdapterInfo = pAdapterList->GetPtr();
		do {
			// Get the adapter
			const AdapterInfo* pAdapterInfo = ppAdapterInfo[0];
			++ppAdapterInfo;

			// Get the desktop display mode of adapter
			D3DDISPLAYMODE TheDesktopMode;
			m_pDirect3D9->GetAdapterDisplayMode(
				pAdapterInfo->GetAdapterOrdinal(), &TheDesktopMode);

			uintptr_t uDeviceInfoCount = pAdapterInfo->GetDisplayInfoListSize();
			if (uDeviceInfoCount) {
				// Enum all the device types supported by this adapter to find
				// the best device settings
				DeviceInfo* const* ppDeviceInfo =
					pAdapterInfo->GetDisplayInfoList();
				do {
					const DeviceInfo* pDeviceInfo = ppDeviceInfo[0];
					++ppDeviceInfo;

					uintptr_t uBufferCount = pDeviceInfo->GetBufferListSize();
					if (uBufferCount) {

						// Enum all the device settings combinations. A device
						// settings combination is a unique set of an adapter
						// format, back buffer format, and IsWindowed.

						BufferFormatGroup* const* ppBufferFormatGroup =
							pDeviceInfo->GetBufferList();
						do {
							const BufferFormatGroup* pBufferFormatGroup =
								ppBufferFormatGroup[0];
							++ppBufferFormatGroup;

							// If windowed mode the adapter format has to be the
							// same as the desktop display mode format so skip
							// any that don't match

							if (pBufferFormatGroup->IsWindowed() &&
								((pBufferFormatGroup->GetAdapterFormat() !=
									static_cast<uint_t>(
										TheDesktopMode.Format)))) {
								continue;
							}

							// Skip any combo that doesn't meet the preserve
							// match options
							uintptr_t uBestMode;
							uintptr_t uBestMSAA;

							// Get a ranking number that describes how closely
							// this device combo matches the optimal combo
							float fCurRanking =
								pBufferFormatGroup->RankDevice(pDeviceSettings,
									&TheDesktopMode, &uBestMode, &uBestMSAA);

							// If this buffer group matches the input device
							// better then save it
							if (fCurRanking > fBestRanking) {
								// Set the group
								pBestFormatGroup = pBufferFormatGroup;
								fBestRanking = fCurRanking;
								uBestModeIndex = uBestMode;
								uBestMSAAIndex = uBestMSAA;
							}
						} while (--uBufferCount);
					}
				} while (--uDeviceInfoCount);
			}
		} while (--uAdapterCount);
	}

	// If no best device combination was found then fail
	if (!pBestFormatGroup) {
		return static_cast<uint_t>(E_FAIL);
	}

	// Here we go! Update the settings to reflect the actual
	// buffer that was the closest match

	pDeviceSettings->m_uAdapterOrdinal = pBestFormatGroup->GetAdapterOrdinal();
	pDeviceSettings->m_uDeviceType = pBestFormatGroup->GetDeviceType();
	pDeviceSettings->m_uAdapterFormat = pBestFormatGroup->GetAdapterFormat();
	pDeviceSettings->m_uBackBufferFormat =
		pBestFormatGroup->GetBackBufferFormat();

	// Add the settings for full screen
	if (!pBestFormatGroup->IsWindowed()) {
		const DisplayMode_t* pWhichMode =
			pBestFormatGroup->GetAdapterInfo()->GetDisplayModeList() +
			uBestModeIndex;
		// Grab the size of the display
		pDeviceSettings->m_uBackBufferWidth = pWhichMode->m_uWidth;
		pDeviceSettings->m_uBackBufferHeight = pWhichMode->m_uHeight;
		pDeviceSettings->m_uFullScreen_RefreshRateInHz =
			pWhichMode->m_uRefreshRate;
	}

	// Window to attach
	pDeviceSettings->m_pDeviceWindow = m_pGameApp->GetWindow();

	// Anti-Aliasing settings
	const BufferFormatGroup::MSQuality_t* pQuality =
		pBestFormatGroup->GetMultiSampleQualityList() + uBestMSAAIndex;
	uint_t uMaxQuality = pQuality->m_uMaxQuality;
	if (uMaxQuality) {
		--uMaxQuality;
	}
	// Use the clamping override
	if (pDeviceSettings->m_uMultiSampleQuality > uMaxQuality) {
		pDeviceSettings->m_uMultiSampleQuality = uMaxQuality;
	}

	pDeviceSettings->m_uMultiSampleType = pQuality->m_uMSType;
	pDeviceSettings->m_bWindowed = pBestFormatGroup->IsWindowed();

	// Good to go!
	return 0;
}

/*! ************************************

	\brief Convert a Windows HCURSOR for DirectX9

	Convert a HCURSOR into a DirectX 9 surface and set
	up DirectX to use this surface as a cursor. Hot
	spots are retained.

	\windowsonly
	\param pDirect3DDevice9 Pointer to the current Direct3D 9 device
	\param hCursor HCURSOR object to convert
	\return HRESULT returned by Windows. Positive numbers mean success.

***************************************/

long BURGER_API Burger::SetDeviceCursor(
	IDirect3DDevice9* pDirect3DDevice9, HICON__* hCursor)
{
	HRESULT iResult = E_FAIL;
	ICONINFO TheIconInfo;
	MemoryClear(&TheIconInfo, sizeof(TheIconInfo));

	// Is there an icon with the cursor?
	if (GetIconInfo(hCursor, &TheIconInfo)) {

		// Obtain the mask of the cursor
		BITMAP TheBitMap;
		if (GetObject(TheIconInfo.hbmMask, sizeof(BITMAP), &TheBitMap)) {

			DWORD uWidth = static_cast<DWORD>(TheBitMap.bmWidth);
			DWORD uOriginalHeight = static_cast<DWORD>(TheBitMap.bmHeight);

			DWORD uHeight = uOriginalHeight;
			if (!TheIconInfo.hbmColor) {
				uHeight = uHeight >> 1U; // It's B&W
			}

			// Initialize for cleanup code
			LPDIRECT3DSURFACE9 pCursorSurface = NULL;

			// Create a DirectX 9 surface for the cursor
			iResult =
				pDirect3DDevice9->CreateOffscreenPlainSurface(uWidth, uHeight,
					D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pCursorSurface, NULL);

			// Create a surface for the full screen cursor
			if (iResult >= 0) {

				BITMAPINFO TheBitMapInfo;
				MemoryClear(&TheBitMapInfo, sizeof(TheBitMapInfo));
				TheBitMapInfo.bmiHeader.biSize =
					sizeof(TheBitMapInfo.bmiHeader);
				TheBitMapInfo.bmiHeader.biWidth = static_cast<LONG>(uWidth);
				TheBitMapInfo.bmiHeader.biHeight =
					static_cast<LONG>(uOriginalHeight);
				TheBitMapInfo.bmiHeader.biPlanes = 1;
				TheBitMapInfo.bmiHeader.biBitCount = 32; // 32 bits per pixel
				TheBitMapInfo.bmiHeader.biCompression =
					BI_RGB;                              // Not compressed

				// Get the screen data type
				HDC hScreenDC = GetDC(NULL);

				// Get a context for the mask to generate
				HDC hMaskDC = CreateCompatibleDC(hScreenDC);

				// Assume failure
				iResult = E_FAIL;
				if (hMaskDC) {

					// Convert the mask to the current mode
					HGDIOBJ hOldMask =
						SelectObject(hMaskDC, TheIconInfo.hbmMask);
					COLORREF* pArrayMask = static_cast<COLORREF*>(
						Alloc(sizeof(COLORREF) * uWidth * uOriginalHeight));
					GetDIBits(hMaskDC, TheIconInfo.hbmMask, 0, uOriginalHeight,
						pArrayMask, &TheBitMapInfo, DIB_RGB_COLORS);
					// Set it back
					SelectObject(hMaskDC, hOldMask);

					HDC hColorDC = NULL;
					COLORREF* pArrayColor = NULL;
					if (TheIconInfo.hbmColor) {
						hColorDC = CreateCompatibleDC(hScreenDC);
						if (hColorDC) {
							SelectObject(hColorDC, TheIconInfo.hbmColor);
							pArrayColor = static_cast<COLORREF*>(
								Alloc(sizeof(COLORREF) * uWidth * uHeight));
							GetDIBits(hColorDC, TheIconInfo.hbmColor, 0,
								uHeight, pArrayColor, &TheBitMapInfo,
								DIB_RGB_COLORS);
						}
					}

					// Can a transfer occur?

					if (!TheIconInfo.hbmColor || hColorDC) {

						// Transfer cursor image into the surface

						if (uHeight && uWidth) {
							// Lock the new surface
							D3DLOCKED_RECT TheLockRect;
							pCursorSurface->LockRect(&TheLockRect, NULL, 0);

							DWORD* pBitmap =
								static_cast<DWORD*>(TheLockRect.pBits);
							DWORD y = 0;
							do {
								DWORD x = 0;
								do {
									COLORREF crMask;
									COLORREF crColor;
									if (!TheIconInfo.hbmColor) {
										crColor = pArrayMask
											[(uWidth * ((uHeight - 1) - y)) +
												x];
										crMask = pArrayMask
											[(uWidth *
												 ((uOriginalHeight - 1) - y)) +
												x];
									} else {
										crColor = pArrayColor
											[(uWidth * ((uHeight - 1) - y)) +
												x];
										crMask = pArrayMask
											[(uWidth * ((uHeight - 1) - y)) +
												x];
									}
									COLORREF uTemp;
									if (!crMask) {
										// Max alpha
										uTemp = 0xff000000 | crColor;
									} else {
										// Nothing to draw (Alpha 0)
										uTemp = 0;
									}
									// Save off the pixel
									pBitmap[0] = uTemp;
									++pBitmap;
								} while (++x < uWidth);
							} while (++y < uHeight);
							pCursorSurface->UnlockRect();
						}

						// Set up the cursor hot spot and cursor shape
						// to DirectX 9 and set the result
						iResult = pDirect3DDevice9->SetCursorProperties(
							TheIconInfo.xHotspot, TheIconInfo.yHotspot,
							pCursorSurface);
						if (iResult >= 0) {
							iResult = S_OK; // Force success to zero
						}
					}

					// Release the buffers and contexts
					Free(pArrayColor);
					if (hColorDC) {
						DeleteDC(hColorDC);
					}
					Free(pArrayMask);
					if (hMaskDC) {
						DeleteDC(hMaskDC);
					}
				}
				if (hScreenDC) {
					ReleaseDC(NULL, hScreenDC);
				}
			}

			// Release the surface, if one was created
			if (pCursorSurface) {
				pCursorSurface->Release();
			}
		}

		// GetIconInfo creates these objects, dispose of them on exit

		if (TheIconInfo.hbmMask) {
			DeleteObject(TheIconInfo.hbmMask);
		}
		if (TheIconInfo.hbmColor) {
			DeleteObject(TheIconInfo.hbmColor);
		}
	}
	return iResult;
}

/*! ************************************

	\brief Get the number of bits for a single color channel

	Using a DirectX 9 D3DFORMAT value, return the
	number of bits needed to hold a smallest color value
	in a single pixel.

	Return 0 if the value is invalid

	\note For formats like D3DFMT_R5G6B5 where the color
	channels have different widths, the smallest width is returned,
	which in this example is five.

	\windowsonly
	\param uD3DFORMAT D3DFORMAT value
	\return Number of bits per pixel, 0 on error
	\sa GetD3DFORMATAlphaChannelBits(uint_t), GetD3DFORMATDepthBits(uint_t) or
GetD3DFORMATStencilBits(uint_t)

***************************************/

uint_t BURGER_API Burger::GetD3DFORMATColorChannelBits(uint_t uD3DFORMAT)
{
	switch (uD3DFORMAT) {

	case D3DFMT_A16B16G16R16:
		return 16;

	case D3DFMT_A2B10G10R10:
	case D3DFMT_A2R10G10B10:
		return 10;

	case D3DFMT_R8G8B8:
	case D3DFMT_A8R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_A8B8G8R8:
		return 8;

	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_A1R5G5B5:
		return 5;

	case D3DFMT_A4R4G4B4:
	case D3DFMT_X4R4G4B4:
		return 4;

	case D3DFMT_R3G3B2:
	case D3DFMT_A8R3G3B2:
		return 2;

	default:
		return 0;
	}
}

/*! ************************************

	\brief Get the number of bits for the alpha channel

	Using a DirectX 9 D3DFORMAT value, return the
	number of bits needed to hold the alpha value
	in a single pixel.

	Return 0 if the value is invalid or there is no alpha component

	\windowsonly
	\param uD3DFORMAT D3DFORMAT value
	\return Number of bits for alpha in a pixel, 0 on error or no alpha
	\sa GetD3DFORMATColorChannelBits(uint_t), GetD3DFORMATDepthBits(uint_t) or
GetD3DFORMATStencilBits(uint_t)

***************************************/

uint_t BURGER_API Burger::GetD3DFORMATAlphaChannelBits(uint_t uD3DFORMAT)
{
	switch (uD3DFORMAT) {
	case D3DFMT_A16B16G16R16:
		return 16;

	case D3DFMT_A8R8G8B8:
	case D3DFMT_A8R3G3B2:
	case D3DFMT_A8B8G8R8:
		return 8;

	case D3DFMT_A4R4G4B4:
		return 4;

	case D3DFMT_A2B10G10R10:
	case D3DFMT_A2R10G10B10:
		return 2;

	case D3DFMT_A1R5G5B5:
		return 1;

	case D3DFMT_R8G8B8:
	case D3DFMT_X8R8G8B8:
	case D3DFMT_R5G6B5:
	case D3DFMT_X1R5G5B5:
	case D3DFMT_R3G3B2:
	case D3DFMT_X4R4G4B4:
	default:
		return 0;
	}
}

/*! ************************************

	\brief Get the number of bits for the depth channel

	Using a DirectX 9 D3DFORMAT value, return the
	number of bits needed to hold the depth value
	in a single pixel.

	Return 0 if the value is invalid or there is no depth component

	\windowsonly
	\param uD3DFORMAT D3DFORMAT value
	\return Number of bits for depth in a pixel, 0 on error or no depth
	\sa GetD3DFORMATColorChannelBits(uint_t),
GetD3DFORMATAlphaChannelBits(uint_t) or GetD3DFORMATStencilBits(uint_t)

***************************************/

uint_t BURGER_API Burger::GetD3DFORMATDepthBits(uint_t uD3DFORMAT)
{
	switch (uD3DFORMAT) {
	case D3DFMT_D32F_LOCKABLE:
	case D3DFMT_D32:
		return 32;

	case D3DFMT_D24X8:
	case D3DFMT_D24S8:
	case D3DFMT_D24X4S4:
	case D3DFMT_D24FS8:
		return 24;

	case D3DFMT_D16_LOCKABLE:
	case D3DFMT_D16:
		return 16;

	case D3DFMT_D15S1:
		return 15;

	default:
		return 0;
	}
}

/*! ************************************

	\brief Get the number of bits for the stencil channel

	Using a DirectX 9 D3DFORMAT value, return the
	number of bits needed to hold the stencil value
	in a single pixel.

	Return 0 if the value is invalid or there is no stencil component

	\windowsonly
	\param uD3DFORMAT D3DFORMAT value
	\return Number of bits for stencil in a pixel, 0 on error or no stencil
	\sa GetD3DFORMATColorChannelBits(uint_t),
GetD3DFORMATAlphaChannelBits(uint_t) or GetD3DFORMATDepthBits(uint_t)

***************************************/

uint_t BURGER_API Burger::GetD3DFORMATStencilBits(uint_t uD3DFORMAT)
{
	switch (uD3DFORMAT) {
	case D3DFMT_D16_LOCKABLE:
	case D3DFMT_D16:
	case D3DFMT_D32F_LOCKABLE:
	case D3DFMT_D32:
	case D3DFMT_D24X8:
	default:
		return 0;

	case D3DFMT_D15S1:
		return 1;

	case D3DFMT_D24X4S4:
		return 4;

	case D3DFMT_D24S8:
	case D3DFMT_D24FS8:
		return 8;
	}
}

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DisplayDirectX9, Burger::Display);
#endif

/*! ************************************

	\var const Burger::StaticRTTI Burger::DisplayDirectX9::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent (If any)

***************************************/

#endif
