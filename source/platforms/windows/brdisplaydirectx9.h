/***************************************

    DirectX 9 manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDISPLAYDIRECTX9_H__
#define __BRDISPLAYDIRECTX9_H__

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#ifndef __BRGLOBALS_H__
#include "brglobals.h"
#endif

#ifndef __BRSIMPLEARRAY_H__
#include "brsimplearray.h"
#endif

#ifndef __BRFIXEDPOINT_H__
#include "brfixedpoint.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
struct IDirect3D9;
struct ID3DXMatrixStack;
struct IDirect3DSurface9;
struct HMONITOR__;

namespace Burger {

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class DisplayDirectX9 : public Display {
    BURGER_DISABLE_COPY(DisplayDirectX9);
	BURGER_RTTI_IN_CLASS();
public:

	class BufferFormatGroup;

	enum {
		DIRECTXRESETATTEMPTS=30			///< Number of times the app will retry restarting the display before giving up
	};
	
	struct DisplayMode_t {
		uint_t m_uWidth;			///< Width in pixels
		uint_t m_uHeight;			///< Height in pixels
		uint_t m_uRefreshRate;	///< Refresh rate in hertz, if zero, the display doesn't have a refresh rate
		uint_t m_uFormat;			///< Pixel format in D3DFORMAT
	};

	struct DeviceSettings_t {
		HWND__ *m_pDeviceWindow;			///< Pointer to the device window
		uint_t m_uAdapterOrdinal;				///< Video adapter number for which display is used
		uint_t m_uDeviceType;					///< D3DDEVTYPE Device type enumeration
		uint_t m_uAdapterFormat;				///< D3DFORMAT Pixel format of the adapter
		uint_t m_uBehaviorFlags;				///< D3DCREATE_* flags

		uint_t m_uBackBufferWidth;			///< Width in pixels of the back buffer
		uint_t m_uBackBufferHeight;			///< Height in pixels of the back buffer
		uint_t m_uBackBufferFormat;			///< D3DFORMAT Pixel format of the back buffer
		uint_t m_uBackBufferCount;			///< Number of back buffers
		uint_t m_uMultiSampleType;			///< D3DMULTISAMPLE_TYPE Anti-alias type
		uint_t m_uMultiSampleQuality;			///< Quality of the Anti-aliasing
		uint_t m_uSwapEffect;					///< D3DSWAPEFFECT Swap effect type
		uint_t m_uAutoDepthStencilFormat;		///< D3DFORMAT pixel format for stencil
		uint_t m_uFlags;						///< D3DPRESENTFLAG_* Presentation flags
		uint_t m_uFullScreen_RefreshRateInHz;	///< Refresh rate for VSync (set to 0 for a window)
		uint_t m_uPresentationInterval;		///< D3DPRESENT_INTERVAL_* valie
		uint_t m_bWindowed;					///< \ref TRUE for window mode
		uint_t m_bEnableAutoDepthStencil;		///< \ref TRUE if Auto stencil is enabled

		void BURGER_API SetPresentParameters(const _D3DPRESENT_PARAMETERS_ *pInput);
		void BURGER_API GetPresentParameters(_D3DPRESENT_PARAMETERS_ *pOutput) const;
		BURGER_INLINE uint_t IsMSAASwapChainCreated(void) const { return m_uMultiSampleType >= 2; }
		BURGER_INLINE uint_t IsVSyncEnabled(void) const { return m_uPresentationInterval != 0x80000000U; }
		void BURGER_API ApplyDefaults(void);
	};

	class DeviceInfo {
		uint_t m_uAdapterOrdinal;		///< Device ordinal type
		uint_t m_uDeviceType;			///< D3DDEVTYPE
		uint32_t m_D3DCaps[76];		///< D3DCAPS9 record
		SimpleArray <BufferFormatGroup*> m_BufferFormatList;	///< All of the settings for this device

	public:
		DeviceInfo(uint_t uAdapterOrdinal,uint_t uDeviceType);
		~DeviceInfo();
		BURGER_INLINE uint_t GetAdapterOrdinal(void) const { return m_uAdapterOrdinal; }
		BURGER_INLINE uint_t GetDeviceType(void) const { return m_uDeviceType; }
		BURGER_INLINE const _D3DCAPS9* GetCaps(void) const { return static_cast<const _D3DCAPS9 *>(static_cast<const void *>(m_D3DCaps)); }
		BURGER_INLINE uintptr_t GetBufferListSize(void) const { return m_BufferFormatList.size(); }
		BURGER_INLINE BufferFormatGroup * const * GetBufferList(void) const { return m_BufferFormatList.GetPtr(); }
		void BURGER_API AddToList(BufferFormatGroup *pBufferFormatGroup);
		const BufferFormatGroup * BURGER_API Find(uint_t uAdapterFormat,uint_t uBackBufferFormat,uint_t bWindowed) const;
	};

	class AdapterInfo {
		SimpleArray<DisplayMode_t> m_DisplayModeList;	///< Array of display modes
		SimpleArray<DeviceInfo*> m_DeviceInfoList;		///< Array of DeviceInfo* with pixel formats supported for rendering
		uint32_t m_AdapterIdentifier[276];				///< D3DADAPTER_IDENTIFIER9 record
		uint_t m_uAdapterOrdinal;							///< DirectX 9 ordinal value for this device

	public:
		AdapterInfo(uint_t uAdapterOrdinal);
		~AdapterInfo();
		BURGER_INLINE const _D3DADAPTER_IDENTIFIER9 *GetAdapterIdentifier(void) const { return static_cast<const _D3DADAPTER_IDENTIFIER9 *>(static_cast<const void *>(m_AdapterIdentifier)); }
		BURGER_INLINE uint_t GetAdapterOrdinal(void) const { return m_uAdapterOrdinal; }
		BURGER_INLINE uintptr_t GetDisplayModeListSize(void) const { return m_DisplayModeList.size(); }
		BURGER_INLINE const DisplayMode_t * GetDisplayModeList(void) const { return m_DisplayModeList.GetPtr(); }
		void BURGER_API AddToList(const DisplayMode_t *pDisplayMode);
		BURGER_INLINE uintptr_t GetDisplayInfoListSize(void) const { return m_DeviceInfoList.size(); }
		BURGER_INLINE DeviceInfo * const * GetDisplayInfoList(void) const { return m_DeviceInfoList.GetPtr(); }
		void BURGER_API AddToList(DeviceInfo *pDisplayInfo);
		const DeviceInfo * BURGER_API Find(uint_t uDeviceType) const;
	};

	class BufferFormatGroup {
	public:
		struct MSQuality_t {
			uint_t m_uMSType;			///< D3DMULTISAMPLE_TYPE Multisample
			uint_t m_uMaxQuality;		///< Maximum quality for this type
		};
	private:
		struct DSMSConflict_t {
			uint_t m_uDSFormat;		///< D3DFORMAT Depth/Stencil pixel format
			uint_t m_uMSType;			///< D3DMULTISAMPLE_TYPE Multisample type not compatible
		};

		SimpleArray<uint_t> m_DepthStencilFormatList;			///< List of D3DFORMATs for supported depth buffers
		SimpleArray<uint_t> m_PresentIntervalList;			///< List of D3DPRESENT flags
		SimpleArray<MSQuality_t> m_MultiSampleQualityList;	///< List of number of quality levels for each multi-sample type
		SimpleArray<DSMSConflict_t> m_DSMSConflictList;		///< List of D3D9EnumDSMSConflict_t for incompatible depth buffers
		const AdapterInfo* m_pAdapterInfo;	///< Parent adapter information
		const DeviceInfo* m_pDeviceInfo;	///< Parent device information
		uint_t m_uAdapterOrdinal;			///< Index into device list
		uint_t m_uDeviceType;				///< D3DDEVTYPE Hardware/software driver
		uint_t m_uAdapterFormat;			///< D3DFORMAT Display pixel format
		uint_t m_uBackBufferFormat;		///< D3DFORMAT Back buffer pixel format
		uint_t m_bWindowed;				///< \ref TRUE if windowed

	public:
		BufferFormatGroup(uint_t uAdapterOrdinal,uint_t uDeviceType,uint_t uAdapterFormat,uint_t uBackBufferFormat,uint_t bWindowed);
		~BufferFormatGroup();
		BURGER_INLINE void SetParent(const AdapterInfo *pAdapterInfo) { m_pAdapterInfo = pAdapterInfo; }
		BURGER_INLINE const AdapterInfo *GetAdapterInfo(void) const { return m_pAdapterInfo; }
		BURGER_INLINE void SetParent(const DeviceInfo *pDeviceInfo) { m_pDeviceInfo = pDeviceInfo; }
		BURGER_INLINE const DeviceInfo *GetDeviceInfo(void) const { return m_pDeviceInfo; }
		BURGER_INLINE uint_t GetAdapterOrdinal(void) const { return m_uAdapterOrdinal; }
		BURGER_INLINE uint_t GetDeviceType(void) const { return m_uDeviceType; }
		BURGER_INLINE uint_t GetAdapterFormat(void) const { return m_uAdapterFormat; }
		BURGER_INLINE uint_t GetBackBufferFormat(void) const { return m_uBackBufferFormat; }
		BURGER_INLINE uint_t IsWindowed(void) const { return m_bWindowed; }
		BURGER_INLINE uintptr_t GetMultiSampleQualityListSize(void) const { return m_MultiSampleQualityList.size(); }
		BURGER_INLINE const MSQuality_t * GetMultiSampleQualityList(void) const { return m_MultiSampleQualityList.GetPtr(); }
		void BURGER_API CreateConflictList(IDirect3D9 *pDirect3D9);
		void BURGER_API CreateDepthStencilFormatList(IDirect3D9 *pDirect3D9,const SimpleArray<uint_t>*pPossibleList);
		void BURGER_API CreatePresentIntervalList(uint_t uIntervalFlags,const SimpleArray<uint_t>*pPossibleList);
		void BURGER_API CreateMultiSampleTypeList(IDirect3D9 *pDirect3D9,const SimpleArray<uint_t>*pPossibleList,uint_t uMaxQuality);
		uint_t BURGER_API IsConflicted(uint_t uDSFormat,uint_t uMSType) const;
		float BURGER_API RankDevice(const DeviceSettings_t *pOptimalDeviceSettings,
			const _D3DDISPLAYMODE *pAdapterDesktopDisplayMode,
			uintptr_t *pBestModeIndex,uintptr_t *pBestMSAAIndex) const;
	};

	typedef uint_t (BURGER_API *IsDeviceOkayProc)(const _D3DCAPS9 *pCaps,uint_t uAdapterFormat,uint_t uBackBufferFormat,uint_t bWindowed,void *pData);
	typedef uint_t (BURGER_API *DeviceSettingsFilterProc)(DeviceSettings_t *pDeviceSettings,void *pData);
	typedef void (BURGER_API *CallbackProc)(void *pData);
	typedef uint_t (BURGER_API *DeviceCreatedProc)(IDirect3DDevice9 *pD3DDevice,const _D3DSURFACE_DESC *pBackBufferSurfaceDesc,void *pData);

	class Enumerator {
        BURGER_DISABLE_COPY(Enumerator);
	private:
		IDirect3D9* m_pD3D;								///< DirectX 9 device used for enumeration
		IsDeviceOkayProc m_pIsDeviceAcceptableFunc;		///< Callback for pruning requested devices
		void* m_pIsDeviceAcceptableFuncData;			///< Callback private data pointer
		SimpleArray<uint_t> m_DepthStencilPossibleList;	///< Array of D3DFORMAT Depth stencil formats found on the devices
		SimpleArray<uint_t> m_MultiSampleTypeList;		///< Array of D3DMULTISAMPLE_TYPE Anti-aliasing types fount
		SimpleArray<uint_t> m_PresentIntervalList;		///< Array of D3DPRESENT_INTERVAL_* flags for page flipping types
		SimpleArray<AdapterInfo*> m_AdapterInfoList;	///< Array of video adapters found

		uint_t m_uMinWidth;				///< Minimum acceptable display width
		uint_t m_uMaxWidth;				///< Maximum acceptable display width
		uint_t m_uMinHeight;				///< Minimum acceptable display height
		uint_t m_uMaxHeight;				///< Maximum acceptable display height
		uint_t m_uRefreshMin;				///< Minimum acceptable display refresh rate
		uint_t m_uRefreshMax;				///< Maximum acceptable display refresh rate
		uint_t m_uMultisampleQualityMax;	///< Maximum anti-aliasing quality

		uint8_t m_bHasEnumerated;			///< \ref TRUE if enumeration was performed
		uint8_t m_bRequirePostPixelShaderBlending;	///< Set to \ref TRUE if shader support is required
		uint8_t m_bSoftwareVP;			///< \ref TRUE if software vertex processing is acceptable
		uint8_t m_bHardwareVP;			///< \ref TRUE if hardware vertex processing is acceptable
		uint8_t m_bPureHarewareVP;		///< \ref TRUE if vertex processing done exclusively by GPU is acceptable
		uint8_t m_bMixedVP;				///< \ref TRUE if mixed software and hardware vertex processing is acceptable

	public:
		Enumerator();
		~Enumerator();
		BURGER_INLINE void SetRequirePostPixelShaderBlending(uint_t bRequire) { m_bRequirePostPixelShaderBlending = (bRequire!=0); }
		BURGER_INLINE void SetResolutionMinMax(uint_t uMinWidth,uint_t uMinHeight,uint_t uMaxWidth,uint_t uMaxHeight) {
			m_uMinWidth = uMinWidth;
			m_uMinHeight = uMinHeight;
			m_uMaxWidth = uMaxWidth;
			m_uMaxHeight = uMaxHeight;
		} 
		BURGER_INLINE void SetRefreshMinMax(uint_t uMin,uint_t uMax) {
			m_uRefreshMin = uMin;
			m_uRefreshMax = uMax;
		}
		BURGER_INLINE void SetMultisampleQualityMax(uint_t uMax) {
			m_uMultisampleQualityMax = Min(uMax,65535U);
		}

		BURGER_INLINE void SetSoftwareVertexProcessingFlag(uint_t bSoftwareVP) { m_bSoftwareVP = (bSoftwareVP!=0); }
		BURGER_INLINE void SetHardwareVertexProcessingFlag(uint_t bHardwareVP) { m_bHardwareVP = (bHardwareVP!=0); }
		BURGER_INLINE void SetPureHarewareVertexProcessingFlag(uint_t bPureHarewareVP) { m_bPureHarewareVP = (bPureHarewareVP!=0); }
		BURGER_INLINE void SetMixedVertexProcessingFlag(uint_t bMixedVP) { m_bMixedVP = (bMixedVP!=0); }
		BURGER_INLINE uint_t GetSoftwareVertexProcessingFlag(void) const { return m_bSoftwareVP; }
		BURGER_INLINE uint_t GetHardwareVertexProcessingFlag(void) const { return m_bHardwareVP; }
		BURGER_INLINE uint_t GetPureHarewareVertexProcessingFlag(void) const { return m_bPureHarewareVP; }
		BURGER_INLINE uint_t GetMixedVertexProcessingFlag(void) const { return m_bMixedVP; }
		BURGER_INLINE const SimpleArray<uint_t>* GetPossibleDepthStencilFormatList(void) const { return &m_DepthStencilPossibleList; }
		BURGER_INLINE const SimpleArray<uint_t>* GetPossibleMultisampleTypeList(void) const { return &m_MultiSampleTypeList; }
		BURGER_INLINE const SimpleArray<uint_t>* GetPossiblePresentIntervalList(void) const { return &m_PresentIntervalList; }
		void BURGER_API ResetPossibleDepthStencilFormats(void);
		void BURGER_API ResetPossibleMultisampleTypeList(void);
		void BURGER_API ResetPossiblePresentIntervalList(void);
		BURGER_INLINE uint_t HasEnumerated(void) const { return m_bHasEnumerated; }
		BURGER_INLINE const SimpleArray<AdapterInfo*> *GetAdapterInfoList(void) const { return &m_AdapterInfoList; }
		const AdapterInfo * BURGER_API GetAdapterInfo(uint_t uAdapterOrdinal) const;
		const DeviceInfo * BURGER_API GetDeviceInfo(uint_t uAdapterOrdinal,uint_t uDeviceType) const;
		const BufferFormatGroup * BURGER_API GetBufferFormatGroup(uint_t uAdapterOrdinal,uint_t uDeviceType,uint_t uAdapterFormat,uint_t uBackBufferFormat,uint_t bWindowed);
		BURGER_INLINE const BufferFormatGroup *GetBufferFormatGroup(const DeviceSettings_t *pDeviceSettings) { return GetBufferFormatGroup(pDeviceSettings->m_uAdapterOrdinal,pDeviceSettings->m_uDeviceType,pDeviceSettings->m_uAdapterFormat,pDeviceSettings->m_uBackBufferFormat,pDeviceSettings->m_bWindowed); }
		uint_t BURGER_API Enumerate(IDirect3D9 *pD3D9,IsDeviceOkayProc pIsDeviceOkayFunc = NULL,void *pData = NULL);
	private:
		void BURGER_API ClearAdapterInfoList(void);
		uint_t BURGER_API EnumerateDevices(AdapterInfo *pAdapterInfo,const SimpleArray<uint_t> *pAdapterFormatList);
		uint_t BURGER_API EnumerateBufferFormats(const _D3DCAPS9 *pCaps,const AdapterInfo *pAdapterInfo,DeviceInfo *pDeviceInfo,const SimpleArray<uint_t> *pAdapterFormatList);
	};

public:
	Enumerator m_Enumerator;				///< Enumerated devices
	IsDeviceOkayProc m_pDeviceFilter;		///< Pointer to function to filter devices
	void *m_pDeviceFilterData;				///< Pointer to data for the filter device function
	DeviceSettingsFilterProc m_pModifyDeviceSettingsFunc;	///< Pointer to function to filter device settings
	void *m_pModifyDeviceSettingsData;		///< Pointer to data for filter device setting function
	DeviceCreatedProc m_pDeviceCreatedFunc;	///< Pointer to function to call when the device was created (D3DPOOL_MANAGED)
	void *m_pDeviceCreatedFuncData;			///< Pointer to data for device was created function
	CallbackProc m_pDeviceDestroyedFunc;	///< Pointer to function to call when the device was destroyed (D3DPOOL_MANAGED)
	void *m_pDeviceDestroyedFuncData;		///< Pointer to data for device was destroyed function
	DeviceCreatedProc m_pDeviceResetFunc;	///< Pointer to function to call after the device was reset (D3DPOOL_DEFAULT)
	void *m_pDeviceResetFuncData;			///< Pointer to data for device was reset function
	CallbackProc m_pDeviceLostFunc;			///< Pointer to function to call when the device was lost (D3DPOOL_DEFAULT)
	void *m_pDeviceLostFuncData;			///< Pointer to data for device was lost function

	IDirect3D9 *m_pDirect3D9;						///< Pointer to the Direct3D9 context
	IDirect3DDevice9 *m_pDirect3DDevice9;			///< Pointer to the Direct3D9 device
	ID3DXMatrixStack *m_pD3DXMatrixStack;			///< Pointer to the current D3DMatrix stack
	IDirect3DSurface9 *m_pDefaultRenderTarget;	
	IDirect3DSurface9 *m_pCurrentRenderTarget;
	HMONITOR__ *m_AdapterMonitor;					///< the monitor of the adapter 
	uint_t m_bIgnoreSizeChange;						///< if true, don't reset the device upon window size change
	uint_t m_uFullScreenBackBufferWidthAtModeChange;	///< back buffer size of full screen mode right before switching to windowed mode. Used to restore to same resolution when toggling back to fullscreen
	uint_t m_uFullScreenBackBufferHeightAtModeChange;	///< back buffer size of full screen mode right before switching to windowed mode. Used to restore to same resolution when toggling back to fullscreen
	uint_t m_uWindowBackBufferWidthAtModeChange;		///< back buffer size of windowed mode right before switching to full screen mode. Used to restore to same resolution when toggling back to windowed mode
	uint_t m_uWindowBackBufferHeightAtModeChange;		///< back buffer size of windowed mode right before switching to full screen mode. Used to restore to same resolution when toggling back to windowed mode
	uint_t m_uWindowedStyleAtModeChange;				///< window style
	uint_t m_bTopmostWhileWindowed;					///< if TRUE, the windowed HWND is topmost 
	uint_t m_bDeviceObjectsReset;						///< if TRUE, then DeviceReset callback has been called successfully
	uint_t m_bDeviceObjectsCreated;					///< if TRUE, then DeviceCreated callback has been called (if non-NULL)
	uint_t m_bDeviceLost;								///< if TRUE, then the device is lost and needs to be reset
	uint_t m_bInsideDeviceCallback;					///< if TRUE, then the framework is inside an app device callback
	uint_t m_bDeviceCreated;
	uint_t m_bActive;
	uint_t m_bAutoChangeAdapter;

	uint_t m_bLostDevice;					///< TRUE if the device was lost (Minimized?)
	uint_t m_bPower2Textures;				///< TRUE if only power of two textures are allowed
	uint_t m_bMultiRenderTargets;			///< TRUE if multiple render targets are supported
	uint_t m_bSeparateAlphaBlend;			///< TRUE if separated alpha blending is supported
	uint_t m_bSceneBegun;					///< TRUE if BeginScene() was called
	uint_t m_bFullScreenGamma;			///< TRUE if full screen gamma was supported
	uint_t m_bCanCalibrateGamma;			///< TRUE if gamma can be calibrated
	uint_t m_bRasterSlopeScaleDepthBias;	///< TRUE if sloped scaling depth bias is supported in rasterizer
	uint_t m_bRasterDepthBias;			///< TRUE if depth bias is supported in rasterizer
	uint_t m_bIsnVidia;					///< TRUE if the video card was made by nVidia
	uint_t m_bIsATI;						///< TRUE if the video card was made by ATI
	uint_t m_bIsIntel;					///< TRUE if the video card was made by Intel
	uint_t m_bShowCursorWhenFullScreen;	///< TRUE if the cursor is visible in full screen mode
	uint_t m_bClipCursorWhenFullScreen;	///< TRUE if the cursor is clipped in full screen mode
	uint_t m_uBackBufferFormat;			///< Requested back buffer format
	uint_t m_uMaxTextureWidth;			///< Maximum pixels wide a texture can be
	uint_t m_uMaxTextureHeight;			///< Maximum pixels tall a texture can be
	uint_t m_uMaxTextureStages;			///< Maximum number of texture blend stages
	uint_t m_uMaxTextureSamplers;			///< Maximum number of texture samplers
	uint_t m_uMaxPossibleAnisotropy;		///< Maximum number of anisotropy filters
	uint_t m_uClearColor;					///< RGBA of the color to clear the screen with
	uint_t m_uMatrixStackDepth;			///< Number of matrices in the matrix stack
	float m_fClearDepth;				///< Z Value to clear the Z buffer with

	DeviceSettings_t m_D3D9Settings;	///< D3D device settings
	uint32_t m_D3DSurfaceDesc[8];			///< D3DSURFACE_DESC record
	uint32_t m_D3DCaps[76];				///< D3DCAPS9 record

public:
	DisplayDirectX9(GameApp *pGameApp);
	~DisplayDirectX9();
	uint_t Init(uint_t uWidth,uint_t uHeight,uint_t uDepth=32,uint_t uFlags=DEFAULTFLAGS) BURGER_OVERRIDE;
	void Shutdown(void) BURGER_OVERRIDE;
	void BeginScene(void) BURGER_OVERRIDE;
	void EndScene(void) BURGER_OVERRIDE;
	Texture *CreateTextureObject(void) BURGER_OVERRIDE;
	VertexBuffer *CreateVertexBufferObject(void) BURGER_OVERRIDE;
	void Resize(uint_t uWidth,uint_t uHeight) BURGER_OVERRIDE;
	void SetViewport(uint_t uX,uint_t uY,uint_t uWidth,uint_t uHeight) BURGER_OVERRIDE;
	void SetScissorRect(uint_t uX,uint_t uY,uint_t uWidth,uint_t uHeight) BURGER_OVERRIDE;
	void SetClearColor(float fRed,float fGreen,float fBlue,float fAlpha) BURGER_OVERRIDE;
	void SetClearDepth(float fDepth) BURGER_OVERRIDE;
	void Clear(uint_t uMask) BURGER_OVERRIDE;
	void Bind(Texture *pTexture,uint_t uIndex=0) BURGER_OVERRIDE;
	void Bind(Effect *pEffect) BURGER_OVERRIDE;
	void SetBlend(uint_t bEnable) BURGER_OVERRIDE;
	void SetBlendFunction(eSourceBlendFactor uSourceFactor,eDestinationBlendFactor uDestFactor) BURGER_OVERRIDE;
	void SetLighting(uint_t bEnable) BURGER_OVERRIDE;
	void SetZWrite(uint_t bEnable) BURGER_OVERRIDE;
	void SetDepthTest(eDepthFunction uDepthFunction) BURGER_OVERRIDE;
	void SetCullMode(eCullMode uCullMode) BURGER_OVERRIDE;
	void SetScissor(uint_t bEnable) BURGER_OVERRIDE;
	void DrawPrimitive(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer) BURGER_OVERRIDE;
	void DrawElements(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer) BURGER_OVERRIDE;
	IDirect3DVertexShader9 * BURGER_API CreateVertexShader(const void *pVertexShaderBinary) const;
	IDirect3DPixelShader9 * BURGER_API CreatePixelShader(const void *pPixelShaderBinary) const;

	BURGER_INLINE IDirect3D9 *GetDirect3D9(void) const { return m_pDirect3D9; }
	BURGER_INLINE IDirect3DDevice9 *GetDirect3DDevice9(void) const { return m_pDirect3DDevice9; }
	BURGER_INLINE ID3DXMatrixStack *GetD3DXMatrixStack(void) const { return m_pD3DXMatrixStack; }
	BURGER_INLINE uint_t OnlyPowerOf2(void) const { return m_bPower2Textures; }
	BURGER_INLINE uint_t IsVSynced(void) const { return m_D3D9Settings.IsVSyncEnabled(); }
	BURGER_INLINE uint_t FullScreenGammaSupported(void) const { return m_bFullScreenGamma; }
	BURGER_INLINE uint_t CanCalibrateGamma(void) const { return m_bCanCalibrateGamma; }
	BURGER_INLINE uint_t IsRasterSlopeScaleDepthBias(void) const { return m_bRasterSlopeScaleDepthBias; }
	BURGER_INLINE uint_t IsRasterDepthBias(void) const { return m_bRasterDepthBias; }
	BURGER_INLINE uint_t IsnVidia(void) const { return m_bIsnVidia; }
	BURGER_INLINE uint_t IsATI(void) const { return m_bIsATI; }
	BURGER_INLINE uint_t IsIntel(void) const { return m_bIsIntel; }
	BURGER_INLINE uint_t GetMaxTextureWidth(void) const { return m_uMaxTextureWidth; }
	BURGER_INLINE uint_t GetMaxTextureHeight(void) const { return m_uMaxTextureHeight; }
	BURGER_INLINE uint_t GetMaxTextureStages(void) const { return m_uMaxTextureStages; }
	BURGER_INLINE uint_t GetMaxPossibleAnisotropy(void) const { return m_uMaxPossibleAnisotropy; }
	BURGER_INLINE uint_t GetClearColor(void) const { return m_uClearColor; }
	BURGER_INLINE float GetClearDepth(void) const { return m_fClearDepth; }

	BURGER_INLINE const _D3DSURFACE_DESC* GetBackBufferSurfaceDesc9(void) const { return static_cast<const _D3DSURFACE_DESC *>(static_cast<const void *>(m_D3DSurfaceDesc)); }
	BURGER_INLINE const _D3DCAPS9* GetCaps(void) const { return static_cast<const _D3DCAPS9 *>(static_cast<const void *>(m_D3DCaps)); }
	IDirect3D9 * BURGER_API LoadDirect3D9(void);
	void BURGER_API InitState(void);
	uint_t BURGER_API GetAdapterOrdinalFromMonitor(HMONITOR__ *hMonitor,uint_t *pAdapterOrdinal);
	uint_t BURGER_API GetDesktopResolution(uint_t uAdapterOrdinal,uint_t *pWidth,uint_t *pHeight);
	void BURGER_API UpdateEnumeration(uint_t bForce);
	void BURGER_API UpdateBackBufferDesc(void);
	uint_t BURGER_API ToggleFullScreen(void);
	uint_t BURGER_API ToggleREF(void);
	void CheckForWindowSizeChange(void) BURGER_OVERRIDE;
	void CheckForWindowChangingMonitors(void) BURGER_OVERRIDE;
protected:
	uint_t BURGER_API ChangeDevice(const DeviceSettings_t *pNewSettings,uint_t bForceEnumeration,uint_t bClipWindowToSingleAdapter);
	uint_t BURGER_API Create3DEnvironment(void);
	void BURGER_API Cleanup3DEnvironment(uint_t bClearSettings);
	uint_t BURGER_API Reset3DEnvironment(void);
	void BURGER_API IssueDeviceLostCallback(void);
	uint_t BURGER_API IssueDeviceResetCallback(void);
	void BURGER_API SetupCursor(void) const;
	void BURGER_API ReleaseRenderTargets(void);
	uint_t BURGER_API SnapDeviceSettingsToEnumDevice(DeviceSettings_t *pDeviceSettings,uint_t bForceEnumeration);
};

extern long BURGER_API SetDeviceCursor(IDirect3DDevice9 *pDirect3DDevice9,HICON__ *hCursor);
extern uint_t BURGER_API GetD3DFORMATColorChannelBits(uint_t uD3DFORMAT);
extern uint_t BURGER_API GetD3DFORMATAlphaChannelBits(uint_t uD3DFORMAT);
extern uint_t BURGER_API GetD3DFORMATDepthBits(uint_t uD3DFORMAT);
extern uint_t BURGER_API GetD3DFORMATStencilBits(uint_t uD3DFORMAT);

#endif
}
/* END */

#endif
