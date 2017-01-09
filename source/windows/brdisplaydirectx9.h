/***************************************

	DirectX 9 manager class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
namespace Burger {
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
class DisplayDirectX9 : public Display {
	BURGER_DISABLECOPYCONSTRUCTORS(DisplayDirectX9);
	BURGER_RTTI_IN_CLASS();
public:

	class BufferFormatGroup;

	enum {
		DIRECTXRESETATTEMPTS=30			///< Number of times the app will retry restarting the display before giving up
	};
	
	struct DisplayMode_t {
		Word m_uWidth;			///< Width in pixels
		Word m_uHeight;			///< Height in pixels
		Word m_uRefreshRate;	///< Refresh rate in hertz, if zero, the display doesn't have a refresh rate
		Word m_uFormat;			///< Pixel format in D3DFORMAT
	};

	struct DeviceSettings_t {
		HWND__ *m_pDeviceWindow;			///< Pointer to the device window
		Word m_uAdapterOrdinal;				///< Video adapter number for which display is used
		Word m_uDeviceType;					///< D3DDEVTYPE Device type enumeration
		Word m_uAdapterFormat;				///< D3DFORMAT Pixel format of the adapter
		Word m_uBehaviorFlags;				///< D3DCREATE_* flags

		Word m_uBackBufferWidth;			///< Width in pixels of the back buffer
		Word m_uBackBufferHeight;			///< Height in pixels of the back buffer
		Word m_uBackBufferFormat;			///< D3DFORMAT Pixel format of the back buffer
		Word m_uBackBufferCount;			///< Number of back buffers
		Word m_uMultiSampleType;			///< D3DMULTISAMPLE_TYPE Anti-alias type
		Word m_uMultiSampleQuality;			///< Quality of the Anti-aliasing
		Word m_uSwapEffect;					///< D3DSWAPEFFECT Swap effect type
		Word m_uAutoDepthStencilFormat;		///< D3DFORMAT pixel format for stencil
		Word m_uFlags;						///< D3DPRESENTFLAG_* Presentation flags
		Word m_uFullScreen_RefreshRateInHz;	///< Refresh rate for VSync (set to 0 for a window)
		Word m_uPresentationInterval;		///< D3DPRESENT_INTERVAL_* valie
		Word m_bWindowed;					///< \ref TRUE for window mode
		Word m_bEnableAutoDepthStencil;		///< \ref TRUE if Auto stencil is enabled

		void BURGER_API SetPresentParameters(const _D3DPRESENT_PARAMETERS_ *pInput);
		void BURGER_API GetPresentParameters(_D3DPRESENT_PARAMETERS_ *pOutput) const;
		BURGER_INLINE Word IsMSAASwapChainCreated(void) const { return m_uMultiSampleType >= 2; }
		BURGER_INLINE Word IsVSyncEnabled(void) const { return m_uPresentationInterval != 0x80000000U; }
		void BURGER_API ApplyDefaults(void);
	};

	class DeviceInfo {
		Word m_uAdapterOrdinal;		///< Device ordinal type
		Word m_uDeviceType;			///< D3DDEVTYPE
		Word32 m_D3DCaps[76];		///< D3DCAPS9 record
		SimpleArray <BufferFormatGroup*> m_BufferFormatList;	///< All of the settings for this device

	public:
		DeviceInfo(Word uAdapterOrdinal,Word uDeviceType);
		~DeviceInfo();
		BURGER_INLINE Word GetAdapterOrdinal(void) const { return m_uAdapterOrdinal; }
		BURGER_INLINE Word GetDeviceType(void) const { return m_uDeviceType; }
		BURGER_INLINE const _D3DCAPS9* GetCaps(void) const { return static_cast<const _D3DCAPS9 *>(static_cast<const void *>(m_D3DCaps)); }
		BURGER_INLINE WordPtr GetBufferListSize(void) const { return m_BufferFormatList.size(); }
		BURGER_INLINE BufferFormatGroup * const * GetBufferList(void) const { return m_BufferFormatList.GetPtr(); }
		void BURGER_API AddToList(BufferFormatGroup *pBufferFormatGroup);
		const BufferFormatGroup * BURGER_API Find(Word uAdapterFormat,Word uBackBufferFormat,Word bWindowed) const;
	};

	class AdapterInfo {
		SimpleArray<DisplayMode_t> m_DisplayModeList;	///< Array of display modes
		SimpleArray<DeviceInfo*> m_DeviceInfoList;		///< Array of DeviceInfo* with pixel formats supported for rendering
		Word32 m_AdapterIdentifier[276];				///< D3DADAPTER_IDENTIFIER9 record
		Word m_uAdapterOrdinal;							///< DirectX 9 ordinal value for this device

	public:
		AdapterInfo(Word uAdapterOrdinal);
		~AdapterInfo();
		BURGER_INLINE const _D3DADAPTER_IDENTIFIER9 *GetAdapterIdentifier(void) const { return static_cast<const _D3DADAPTER_IDENTIFIER9 *>(static_cast<const void *>(m_AdapterIdentifier)); }
		BURGER_INLINE Word GetAdapterOrdinal(void) const { return m_uAdapterOrdinal; }
		BURGER_INLINE WordPtr GetDisplayModeListSize(void) const { return m_DisplayModeList.size(); }
		BURGER_INLINE const DisplayMode_t * GetDisplayModeList(void) const { return m_DisplayModeList.GetPtr(); }
		void BURGER_API AddToList(const DisplayMode_t *pDisplayMode);
		BURGER_INLINE WordPtr GetDisplayInfoListSize(void) const { return m_DeviceInfoList.size(); }
		BURGER_INLINE DeviceInfo * const * GetDisplayInfoList(void) const { return m_DeviceInfoList.GetPtr(); }
		void BURGER_API AddToList(DeviceInfo *pDisplayInfo);
		const DeviceInfo * BURGER_API Find(Word uDeviceType) const;
	};

	class BufferFormatGroup {
	public:
		struct MSQuality_t {
			Word m_uMSType;			///< D3DMULTISAMPLE_TYPE Multisample
			Word m_uMaxQuality;		///< Maximum quality for this type
		};
	private:
		struct DSMSConflict_t {
			Word m_uDSFormat;		///< D3DFORMAT Depth/Stencil pixel format
			Word m_uMSType;			///< D3DMULTISAMPLE_TYPE Multisample type not compatible
		};

		SimpleArray<Word> m_DepthStencilFormatList;			///< List of D3DFORMATs for supported depth buffers
		SimpleArray<Word> m_PresentIntervalList;			///< List of D3DPRESENT flags
		SimpleArray<MSQuality_t> m_MultiSampleQualityList;	///< List of number of quality levels for each multi-sample type
		SimpleArray<DSMSConflict_t> m_DSMSConflictList;		///< List of D3D9EnumDSMSConflict_t for incompatible depth buffers
		const AdapterInfo* m_pAdapterInfo;	///< Parent adapter information
		const DeviceInfo* m_pDeviceInfo;	///< Parent device information
		Word m_uAdapterOrdinal;			///< Index into device list
		Word m_uDeviceType;				///< D3DDEVTYPE Hardware/software driver
		Word m_uAdapterFormat;			///< D3DFORMAT Display pixel format
		Word m_uBackBufferFormat;		///< D3DFORMAT Back buffer pixel format
		Word m_bWindowed;				///< \ref TRUE if windowed

	public:
		BufferFormatGroup(Word uAdapterOrdinal,Word uDeviceType,Word uAdapterFormat,Word uBackBufferFormat,Word bWindowed);
		~BufferFormatGroup();
		BURGER_INLINE void SetParent(const AdapterInfo *pAdapterInfo) { m_pAdapterInfo = pAdapterInfo; }
		BURGER_INLINE const AdapterInfo *GetAdapterInfo(void) const { return m_pAdapterInfo; }
		BURGER_INLINE void SetParent(const DeviceInfo *pDeviceInfo) { m_pDeviceInfo = pDeviceInfo; }
		BURGER_INLINE const DeviceInfo *GetDeviceInfo(void) const { return m_pDeviceInfo; }
		BURGER_INLINE Word GetAdapterOrdinal(void) const { return m_uAdapterOrdinal; }
		BURGER_INLINE Word GetDeviceType(void) const { return m_uDeviceType; }
		BURGER_INLINE Word GetAdapterFormat(void) const { return m_uAdapterFormat; }
		BURGER_INLINE Word GetBackBufferFormat(void) const { return m_uBackBufferFormat; }
		BURGER_INLINE Word IsWindowed(void) const { return m_bWindowed; }
		BURGER_INLINE WordPtr GetMultiSampleQualityListSize(void) const { return m_MultiSampleQualityList.size(); }
		BURGER_INLINE const MSQuality_t * GetMultiSampleQualityList(void) const { return m_MultiSampleQualityList.GetPtr(); }
		void BURGER_API CreateConflictList(IDirect3D9 *pDirect3D9);
		void BURGER_API CreateDepthStencilFormatList(IDirect3D9 *pDirect3D9,const SimpleArray<Word>*pPossibleList);
		void BURGER_API CreatePresentIntervalList(Word uIntervalFlags,const SimpleArray<Word>*pPossibleList);
		void BURGER_API CreateMultiSampleTypeList(IDirect3D9 *pDirect3D9,const SimpleArray<Word>*pPossibleList,Word uMaxQuality);
		Word BURGER_API IsConflicted(Word uDSFormat,Word uMSType) const;
		float BURGER_API RankDevice(const DeviceSettings_t *pOptimalDeviceSettings,
			const _D3DDISPLAYMODE *pAdapterDesktopDisplayMode,
			WordPtr *pBestModeIndex,WordPtr *pBestMSAAIndex) const;
	};

	typedef Word (BURGER_API *IsDeviceOkayProc)(const _D3DCAPS9 *pCaps,Word uAdapterFormat,Word uBackBufferFormat,Word bWindowed,void *pData);
	typedef Word (BURGER_API *DeviceSettingsFilterProc)(DeviceSettings_t *pDeviceSettings,void *pData);
	typedef void (BURGER_API *CallbackProc)(void *pData);
	typedef Word (BURGER_API *DeviceCreatedProc)(IDirect3DDevice9 *pD3DDevice,const _D3DSURFACE_DESC *pBackBufferSurfaceDesc,void *pData);

	class Enumerator {
		BURGER_DISABLECOPYCONSTRUCTORS(Enumerator);
	private:
		IDirect3D9* m_pD3D;								///< DirectX 9 device used for enumeration
		IsDeviceOkayProc m_pIsDeviceAcceptableFunc;		///< Callback for pruning requested devices
		void* m_pIsDeviceAcceptableFuncData;			///< Callback private data pointer
		SimpleArray<Word> m_DepthStencilPossibleList;	///< Array of D3DFORMAT Depth stencil formats found on the devices
		SimpleArray<Word> m_MultiSampleTypeList;		///< Array of D3DMULTISAMPLE_TYPE Anti-aliasing types fount
		SimpleArray<Word> m_PresentIntervalList;		///< Array of D3DPRESENT_INTERVAL_* flags for page flipping types
		SimpleArray<AdapterInfo*> m_AdapterInfoList;	///< Array of video adapters found

		Word m_uMinWidth;				///< Minimum acceptable display width
		Word m_uMaxWidth;				///< Maximum acceptable display width
		Word m_uMinHeight;				///< Minimum acceptable display height
		Word m_uMaxHeight;				///< Maximum acceptable display height
		Word m_uRefreshMin;				///< Minimum acceptable display refresh rate
		Word m_uRefreshMax;				///< Maximum acceptable display refresh rate
		Word m_uMultisampleQualityMax;	///< Maximum anti-aliasing quality

		Word8 m_bHasEnumerated;			///< \ref TRUE if enumeration was performed
		Word8 m_bRequirePostPixelShaderBlending;	///< Set to \ref TRUE if shader support is required
		Word8 m_bSoftwareVP;			///< \ref TRUE if software vertex processing is acceptable
		Word8 m_bHardwareVP;			///< \ref TRUE if hardware vertex processing is acceptable
		Word8 m_bPureHarewareVP;		///< \ref TRUE if vertex processing done exclusively by GPU is acceptable
		Word8 m_bMixedVP;				///< \ref TRUE if mixed software and hardware vertex processing is acceptable

	public:
		Enumerator();
		~Enumerator();
		BURGER_INLINE void SetRequirePostPixelShaderBlending(Word bRequire) { m_bRequirePostPixelShaderBlending = (bRequire!=0); }
		BURGER_INLINE void SetResolutionMinMax(Word uMinWidth,Word uMinHeight,Word uMaxWidth,Word uMaxHeight) {
			m_uMinWidth = uMinWidth;
			m_uMinHeight = uMinHeight;
			m_uMaxWidth = uMaxWidth;
			m_uMaxHeight = uMaxHeight;
		} 
		BURGER_INLINE void SetRefreshMinMax(Word uMin,Word uMax) {
			m_uRefreshMin = uMin;
			m_uRefreshMax = uMax;
		}
		BURGER_INLINE void SetMultisampleQualityMax(Word uMax) {
			m_uMultisampleQualityMax = Min(uMax,65535U);
		}

		BURGER_INLINE void SetSoftwareVertexProcessingFlag(Word bSoftwareVP) { m_bSoftwareVP = (bSoftwareVP!=0); }
		BURGER_INLINE void SetHardwareVertexProcessingFlag(Word bHardwareVP) { m_bHardwareVP = (bHardwareVP!=0); }
		BURGER_INLINE void SetPureHarewareVertexProcessingFlag(Word bPureHarewareVP) { m_bPureHarewareVP = (bPureHarewareVP!=0); }
		BURGER_INLINE void SetMixedVertexProcessingFlag(Word bMixedVP) { m_bMixedVP = (bMixedVP!=0); }
		BURGER_INLINE Word GetSoftwareVertexProcessingFlag(void) const { return m_bSoftwareVP; }
		BURGER_INLINE Word GetHardwareVertexProcessingFlag(void) const { return m_bHardwareVP; }
		BURGER_INLINE Word GetPureHarewareVertexProcessingFlag(void) const { return m_bPureHarewareVP; }
		BURGER_INLINE Word GetMixedVertexProcessingFlag(void) const { return m_bMixedVP; }
		BURGER_INLINE const SimpleArray<Word>* GetPossibleDepthStencilFormatList(void) const { return &m_DepthStencilPossibleList; }
		BURGER_INLINE const SimpleArray<Word>* GetPossibleMultisampleTypeList(void) const { return &m_MultiSampleTypeList; }
		BURGER_INLINE const SimpleArray<Word>* GetPossiblePresentIntervalList(void) const { return &m_PresentIntervalList; }
		void BURGER_API ResetPossibleDepthStencilFormats(void);
		void BURGER_API ResetPossibleMultisampleTypeList(void);
		void BURGER_API ResetPossiblePresentIntervalList(void);
		BURGER_INLINE Word HasEnumerated(void) const { return m_bHasEnumerated; }
		BURGER_INLINE const SimpleArray<AdapterInfo*> *GetAdapterInfoList(void) const { return &m_AdapterInfoList; }
		const AdapterInfo * BURGER_API GetAdapterInfo(Word uAdapterOrdinal) const;
		const DeviceInfo * BURGER_API GetDeviceInfo(Word uAdapterOrdinal,Word uDeviceType) const;
		const BufferFormatGroup * BURGER_API GetBufferFormatGroup(Word uAdapterOrdinal,Word uDeviceType,Word uAdapterFormat,Word uBackBufferFormat,Word bWindowed);
		BURGER_INLINE const BufferFormatGroup *GetBufferFormatGroup(const DeviceSettings_t *pDeviceSettings) { return GetBufferFormatGroup(pDeviceSettings->m_uAdapterOrdinal,pDeviceSettings->m_uDeviceType,pDeviceSettings->m_uAdapterFormat,pDeviceSettings->m_uBackBufferFormat,pDeviceSettings->m_bWindowed); }
		Word BURGER_API Enumerate(IDirect3D9 *pD3D9,IsDeviceOkayProc pIsDeviceOkayFunc = NULL,void *pData = NULL);
	private:
		void BURGER_API ClearAdapterInfoList(void);
		Word BURGER_API EnumerateDevices(AdapterInfo *pAdapterInfo,const SimpleArray<Word> *pAdapterFormatList);
		Word BURGER_API EnumerateBufferFormats(const _D3DCAPS9 *pCaps,const AdapterInfo *pAdapterInfo,DeviceInfo *pDeviceInfo,const SimpleArray<Word> *pAdapterFormatList);
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
	Word m_bIgnoreSizeChange;						///< if true, don't reset the device upon window size change
	Word m_uFullScreenBackBufferWidthAtModeChange;  ///< back buffer size of full screen mode right before switching to windowed mode. Used to restore to same resolution when toggling back to fullscreen
	Word m_uFullScreenBackBufferHeightAtModeChange; ///< back buffer size of full screen mode right before switching to windowed mode. Used to restore to same resolution when toggling back to fullscreen
	Word m_uWindowBackBufferWidthAtModeChange;		///< back buffer size of windowed mode right before switching to full screen mode. Used to restore to same resolution when toggling back to windowed mode
	Word m_uWindowBackBufferHeightAtModeChange;		///< back buffer size of windowed mode right before switching to full screen mode. Used to restore to same resolution when toggling back to windowed mode
	Word m_uWindowedStyleAtModeChange;				///< window style
	Word m_bTopmostWhileWindowed;					///< if TRUE, the windowed HWND is topmost 
	Word m_bDeviceObjectsReset;						///< if TRUE, then DeviceReset callback has been called successfully
	Word m_bDeviceObjectsCreated;					///< if TRUE, then DeviceCreated callback has been called (if non-NULL)
	Word m_bDeviceLost;								///< if TRUE, then the device is lost and needs to be reset
	Word m_bInsideDeviceCallback;					///< if TRUE, then the framework is inside an app device callback
	Word m_bDeviceCreated;
	Word m_bActive;
	Word m_bAutoChangeAdapter;

	Word m_bLostDevice;					///< TRUE if the device was lost (Minimized?)
	Word m_bPower2Textures;				///< TRUE if only power of two textures are allowed
	Word m_bMultiRenderTargets;			///< TRUE if multiple render targets are supported
	Word m_bSeparateAlphaBlend;			///< TRUE if separated alpha blending is supported
	Word m_bSceneBegun;					///< TRUE if BeginScene() was called
	Word m_bFullScreenGamma;			///< TRUE if full screen gamma was supported
	Word m_bCanCalibrateGamma;			///< TRUE if gamma can be calibrated
	Word m_bRasterSlopeScaleDepthBias;	///< TRUE if sloped scaling depth bias is supported in rasterizer
	Word m_bRasterDepthBias;			///< TRUE if depth bias is supported in rasterizer
	Word m_bIsnVidia;					///< TRUE if the video card was made by nVidia
	Word m_bIsATI;						///< TRUE if the video card was made by ATI
	Word m_bIsIntel;					///< TRUE if the video card was made by Intel
	Word m_bShowCursorWhenFullScreen;	///< TRUE if the cursor is visible in full screen mode
	Word m_bClipCursorWhenFullScreen;	///< TRUE if the cursor is clipped in full screen mode
	Word m_uBackBufferFormat;			///< Requested back buffer format
	Word m_uMaxTextureWidth;			///< Maximum pixels wide a texture can be
	Word m_uMaxTextureHeight;			///< Maximum pixels tall a texture can be
	Word m_uMaxTextureStages;			///< Maximum number of texture blend stages
	Word m_uMaxTextureSamplers;			///< Maximum number of texture samplers
	Word m_uMaxPossibleAnisotropy;		///< Maximum number of anisotropy filters
	Word m_uClearColor;					///< RGBA of the color to clear the screen with
	Word m_uMatrixStackDepth;			///< Number of matrices in the matrix stack
	float m_fClearDepth;				///< Z Value to clear the Z buffer with

	DeviceSettings_t m_D3D9Settings;	///< D3D device settings
	Word32 m_D3DSurfaceDesc[8];			///< D3DSURFACE_DESC record
	Word32 m_D3DCaps[76];				///< D3DCAPS9 record

public:
	DisplayDirectX9(GameApp *pGameApp);
	~DisplayDirectX9();
	virtual Word Init(Word uWidth,Word uHeight,Word uDepth=32,Word uFlags=DEFAULTFLAGS);
	virtual void Shutdown(void);
	virtual void BeginScene(void);
	virtual void EndScene(void);
	virtual Texture *CreateTextureObject(void);
	virtual VertexBuffer *CreateVertexBufferObject(void);
	virtual void Resize(Word uWidth,Word uHeight);
	virtual void SetViewport(Word uX,Word uY,Word uWidth,Word uHeight);
	virtual void SetScissorRect(Word uX,Word uY,Word uWidth,Word uHeight);
	virtual void SetClearColor(float fRed,float fGreen,float fBlue,float fAlpha);
	virtual void SetClearDepth(float fDepth);
	virtual void Clear(Word uMask);
	virtual void Bind(Texture *pTexture,Word uIndex=0);
	virtual void Bind(Effect *pEffect);
	virtual void SetBlend(Word bEnable);
	virtual void SetBlendFunction(eSourceBlendFactor uSourceFactor,eDestinationBlendFactor uDestFactor);
	virtual void SetLighting(Word bEnable);
	virtual void SetZWrite(Word bEnable);
	virtual void SetDepthTest(eDepthFunction uDepthFunction);
	virtual void SetCullMode(eCullMode uCullMode);
	virtual void SetScissor(Word bEnable);
	virtual void DrawPrimitive(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer);
	virtual void DrawElements(ePrimitiveType uPrimitiveType,VertexBuffer *pVertexBuffer);
	IDirect3DVertexShader9 * BURGER_API CreateVertexShader(const void *pVertexShaderBinary) const;
	IDirect3DPixelShader9 * BURGER_API CreatePixelShader(const void *pPixelShaderBinary) const;

	BURGER_INLINE IDirect3D9 *GetDirect3D9(void) const { return m_pDirect3D9; }
	BURGER_INLINE IDirect3DDevice9 *GetDirect3DDevice9(void) const { return m_pDirect3DDevice9; }
	BURGER_INLINE ID3DXMatrixStack *GetD3DXMatrixStack(void) const { return m_pD3DXMatrixStack; }
	BURGER_INLINE Word OnlyPowerOf2(void) const { return m_bPower2Textures; }
	BURGER_INLINE Word IsVSynced(void) const { return m_D3D9Settings.IsVSyncEnabled(); }
	BURGER_INLINE Word FullScreenGammaSupported(void) const { return m_bFullScreenGamma; }
	BURGER_INLINE Word CanCalibrateGamma(void) const { return m_bCanCalibrateGamma; }
	BURGER_INLINE Word IsRasterSlopeScaleDepthBias(void) const { return m_bRasterSlopeScaleDepthBias; }
	BURGER_INLINE Word IsRasterDepthBias(void) const { return m_bRasterDepthBias; }
	BURGER_INLINE Word IsnVidia(void) const { return m_bIsnVidia; }
	BURGER_INLINE Word IsATI(void) const { return m_bIsATI; }
	BURGER_INLINE Word IsIntel(void) const { return m_bIsIntel; }
	BURGER_INLINE Word GetMaxTextureWidth(void) const { return m_uMaxTextureWidth; }
	BURGER_INLINE Word GetMaxTextureHeight(void) const { return m_uMaxTextureHeight; }
	BURGER_INLINE Word GetMaxTextureStages(void) const { return m_uMaxTextureStages; }
	BURGER_INLINE Word GetMaxPossibleAnisotropy(void) const { return m_uMaxPossibleAnisotropy; }
	BURGER_INLINE Word GetClearColor(void) const { return m_uClearColor; }
	BURGER_INLINE float GetClearDepth(void) const { return m_fClearDepth; }

	BURGER_INLINE const _D3DSURFACE_DESC* GetBackBufferSurfaceDesc9(void) const { return static_cast<const _D3DSURFACE_DESC *>(static_cast<const void *>(m_D3DSurfaceDesc)); }
	BURGER_INLINE const _D3DCAPS9* GetCaps(void) const { return static_cast<const _D3DCAPS9 *>(static_cast<const void *>(m_D3DCaps)); }
	IDirect3D9 * BURGER_API LoadDirect3D9(void);
	void BURGER_API InitState(void);
	Word BURGER_API GetAdapterOrdinalFromMonitor(HMONITOR__ *hMonitor,Word *pAdapterOrdinal);
	Word BURGER_API GetDesktopResolution(Word uAdapterOrdinal,Word *pWidth,Word *pHeight);
	void BURGER_API UpdateEnumeration(Word bForce);
	void BURGER_API UpdateBackBufferDesc(void);
	Word BURGER_API ToggleFullScreen(void);
	Word BURGER_API ToggleREF(void);
	virtual void CheckForWindowSizeChange(void);
	virtual void CheckForWindowChangingMonitors(void);
protected:
	Word BURGER_API ChangeDevice(const DeviceSettings_t *pNewSettings,Word bForceEnumeration,Word bClipWindowToSingleAdapter);
	Word BURGER_API Create3DEnvironment(void);
	void BURGER_API Cleanup3DEnvironment(Word bClearSettings);
	Word BURGER_API Reset3DEnvironment(void);
	void BURGER_API IssueDeviceLostCallback(void);
	Word BURGER_API IssueDeviceResetCallback(void);
	void BURGER_API SetupCursor(void) const;
	void BURGER_API ReleaseRenderTargets(void);
	Word BURGER_API SnapDeviceSettingsToEnumDevice(DeviceSettings_t *pDeviceSettings,Word bForceEnumeration);
};

class DirectXPerfMarker {
	BURGER_DISABLECOPYCONSTRUCTORS(DirectXPerfMarker);
public:
	BURGER_INLINE DirectXPerfMarker(Word32 uColor,const Word16 *pMessage) 
	{
		Globals::D3DPERF_BeginEvent(uColor,pMessage);
	}
	BURGER_INLINE ~DirectXPerfMarker() 
	{
		Globals::D3DPERF_EndEvent();
	}
};

extern long BURGER_API SetDeviceCursor(IDirect3DDevice9 *pDirect3DDevice9,HICON__ *hCursor);
extern Word BURGER_API GetD3DFORMATColorChannelBits(Word uD3DFORMAT);
extern Word BURGER_API GetD3DFORMATAlphaChannelBits(Word uD3DFORMAT);
extern Word BURGER_API GetD3DFORMATDepthBits(Word uD3DFORMAT);
extern Word BURGER_API GetD3DFORMATStencilBits(Word uD3DFORMAT);

#endif
}
/* END */

#endif
