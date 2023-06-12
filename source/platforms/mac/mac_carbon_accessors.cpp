/***************************************

	CarbonAccessors.o compatible library

	This replaces Apple's CarbonAccessors.o file so profiling, debugging and
	other compiler features that are available with source level debugging
	become available to an application.

	This code is intended only for MacOS Classic targeted applications.

	Copyright (c) 2002-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "mac_carbon_accessors.h"

// Only build on PowerPC MacOS Classic targets.

#if defined(BURGER_MACCLASSIC) && !defined(DOXYGEN)

#include <AEDataModel.h>
#include <ControlDefinitions.h>
#include <Lists.h>
#include <LowMem.h>
#include <Menus.h>
#include <Quickdraw.h>
#include <TSMTE.h>
#include <TextServices.h>
#include <TextUtils.h>

// Quickdraw Error Low level address
#define QDOldPat 0
#define MBDFHndl reinterpret_cast<Handle*>(0x0B58)
#define QDErrLM reinterpret_cast<int16_t*>(0x0D6E)
#define TSMHndl reinterpret_cast<Handle*>(0x1750)

// Apple Event private data
enum {
	typeAEFlatten1 = FOUR_CHAR_CODE('dle1'),
	typeAEFlatten2 = FOUR_CHAR_CODE('dle2')
};

// 16 byte header for flattened data
struct AEFlatten {
	AEKeyword descKey;       // 'dle2'
	uint32_t notUsed;        // always zero
	DescType descriptorType; // Type of data that was flattened
	Size dataSize;           // Size in bytes of data following this header.
};

struct AEFlatten2: public AEFlatten {
	uint32_t notUsed1[16];
	DescType appleEventType; // 80
	Size appleEventIDSize;   // 84
	uint32_t appleEventID;   // 88
	uint32_t ss;
};

static BURGER_INLINE Ptr RoundUp(Ptr pInput)
{
	return Ptr((uintptr_t(pInput) + 1) & (~1));
}

//
// Below is some of Apple's Secret Sauce.
//

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Apple Events ===
#endif

/***************************************

	Initiailize an AEDesc

***************************************/

void AEInitializeDesc(AEDesc* desc)
{
	desc->descriptorType = typeNull;
	desc->dataHandle = nullptr;
}

static Boolean CheckIsRecord(char* pData, Size uLength)
{
	// TODO: Finish this
	if (uLength < (16 + 3)) {
		return FALSE;
	}
	Ptr pTemp = RoundUp(pData + 16);
	pTemp[0] = 0;
	return TRUE;
}

Boolean AECheckIsRecord(const AEDesc* theDesc)
{
	Handle hDataHandle = theDesc->dataHandle;
	if (!hDataHandle) {
		return FALSE;
	}
	SInt8 oldState = HGetState(hDataHandle);
	HLock(hDataHandle);
	Size uLength = GetHandleSize(hDataHandle);
	Boolean bResult = CheckIsRecord(*hDataHandle, uLength);
	HSetState(hDataHandle, oldState);
	return bResult;
}

/***************************************

	Copy the data to a supplied buffer

***************************************/

OSErr AEGetDescData(const AEDesc* theAEDesc, void* dataPtr, Size maximumSize)
{
	if (theAEDesc->dataHandle) {
		Size uLength = GetHandleSize(theAEDesc->dataHandle);
		// Crop to maximum size
		if (maximumSize > uLength) {
			maximumSize = uLength;
		}
		BlockMoveData(*theAEDesc->dataHandle, dataPtr, maximumSize);
	}
	return noErr;
}

Size AEGetDescDataSize(const AEDesc* theAEDesc)
{
	if (theAEDesc->descriptorType == typeNull) {
		return 0;
	}
	return GetHandleSize(theAEDesc->dataHandle);
}

OSErr AEReplaceDescData(
	DescType typeCode, const void* dataPtr, Size dataSize, AEDesc* theAEDesc)
{
	AEDisposeDesc(theAEDesc);
	return AECreateDesc(typeCode, dataPtr, dataSize, theAEDesc);
}

Size AESizeOfFlattenedDesc(const AEDesc* theAEDesc)
{
	return static_cast<Size>(AEGetDescDataSize(theAEDesc) + sizeof(AEFlatten));
}

OSStatus AEFlattenDesc(
	const AEDesc* theAEDesc, Ptr buffer, Size bufferSize, Size* actualSize)
{
	// Sanity check
	if ((!buffer && !actualSize) || !theAEDesc) {
		return paramErr;
	}

	// Get the size of the data to snapshot
	Size uLength = AEGetDescDataSize(theAEDesc);

	if (actualSize) {
		*actualSize = static_cast<Size>(uLength + sizeof(AEFlatten));
	}

	// Return data if requested
	if (buffer) {
		if (bufferSize < static_cast<Size>(uLength + sizeof(AEFlatten))) {
			return errAEBufferTooSmall;
		}

		AEFlatten* pFlatten = reinterpret_cast<AEFlatten*>(buffer);
		pFlatten->descKey = typeAEFlatten2;
		pFlatten->notUsed = 0;
		pFlatten->descriptorType = theAEDesc->descriptorType;
		pFlatten->dataSize = uLength;

		if (theAEDesc->descriptorType != typeNull) {
			BlockMoveData(
				*theAEDesc->dataHandle, buffer + sizeof(AEFlatten), uLength);
		}
	}
	return noErr;
}

static OSStatus unflattenDesc_Beta(Ptr buffer, AEDesc* result)
{
	OSStatus uError;
	AEFlatten* pFlatten = reinterpret_cast<AEFlatten*>(buffer);
	if (pFlatten->descriptorType == typeNull) {
		return noErr;
	}
	if (pFlatten->descriptorType != typeAppleEvent) {
		uError = PtrToHand(buffer + sizeof(AEFlatten), &result->dataHandle,
			pFlatten->dataSize);
		if (!uError) {
			result->descriptorType = pFlatten->descriptorType;
		}
		return uError;
	}

	// typeAppleEvent needs special case handling

	AEFlatten2* pFlatten2 = reinterpret_cast<AEFlatten2*>(buffer);
	Size uOffset = pFlatten2->appleEventIDSize;
	uError = PtrToHand(buffer + sizeof(AEFlatten), &result->dataHandle, 128);
	if (!uError) {
		// Round up to next even if needed
		Size uOffset2 = ((uOffset + 128 + 4 + 1) & (~1)) * 72;
		uError = PtrAndHand(buffer + sizeof(AEFlatten) + uOffset2,
			result->dataHandle, (pFlatten->dataSize - uOffset) - 4);
	}

	if (!uError) {
		uint32_t* pAddr = reinterpret_cast<uint32_t*>(*result->dataHandle);
		pAddr[0] = 0;
		pAddr[1] = 0;
		uOffset = static_cast<Size>((uOffset - pAddr[2]) - 4);
		pAddr[2] = static_cast<uint32_t>(uOffset);
		uError =
			AEPutAttributePtr(result, keyAddressAttr, pFlatten2->appleEventType,
				&pFlatten2->appleEventID, pFlatten2->appleEventIDSize);
	}
	return uError;
}

OSStatus AEUnflattenDesc(Ptr buffer, AEDesc* result)
{
	if (buffer && result) {
		// Force result to null, in case there is no data
		result->descriptorType = typeNull;
		result->dataHandle = nullptr;

		AEFlatten* pFlatten = reinterpret_cast<AEFlatten*>(buffer);

		// Old style?
		if (pFlatten->descKey == typeAEFlatten1) {
			return unflattenDesc_Beta(buffer, result);
		}

		// New style?
		if (pFlatten->descKey == typeAEFlatten2) {
			if (pFlatten->descriptorType == typeNull) {
				return noErr;
			}
			OSStatus uError = PtrToHand(buffer + sizeof(AEFlatten),
				&result->dataHandle, pFlatten->dataSize);
			if (!uError) {
				result->descriptorType = pFlatten->descriptorType;
			}
			return uError;
		}
	}
	return paramErr;
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Control Manager ===
#endif

/***************************************

	Control Manager accessors

***************************************/

pascal Rect* GetControlBounds(ControlRef control, Rect* bounds)
{
	if (control) {
		*bounds = (*control)->contrlRect;
	}
	return bounds;
}

pascal Boolean IsControlHilited(ControlRef control)
{
	return (*control)->contrlHilite != 0;
}

pascal UInt16 GetControlHilite(ControlRef control)
{
	return (*control)->contrlHilite;
}

pascal Handle GetControlDataHandle(ControlRef control)
{
	return (*control)->contrlData;
}

pascal WindowRef GetControlOwner(ControlRef control)
{
	return (*control)->contrlOwner;
}

pascal MenuRef GetControlPopupMenuHandle(ControlRef control)
{
	if (control) {
		const ControlRecord* pControl = *control;
		if (pControl->contrlData) {
			const PopupPrivateData* pMenu =
				reinterpret_cast<const PopupPrivateData*>(
					*pControl->contrlData);
			return pMenu->mHandle;
		}
	}
	return nullptr;
}

pascal short GetControlPopupMenuID(ControlRef control)
{
	if (control) {
		const ControlRecord* pControl = *control;
		if (pControl->contrlData) {
			const PopupPrivateData* pMenu =
				reinterpret_cast<const PopupPrivateData*>(
					*pControl->contrlData);
			return pMenu->mID;
		}
	}
	return 0;
}

pascal void SetControlBounds(ControlRef control, const Rect* bounds)
{
	if (control) {
		(*control)->contrlRect = *bounds;
	}
}

pascal void SetControlDataHandle(ControlRef control, Handle dataHandle)
{
	if (control) {
		(*control)->contrlData = dataHandle;
	}
}

pascal void SetControlPopupMenuHandle(ControlRef control, MenuRef popupMenu)
{
	if (control) {
		ControlPtr pControl = *control;
		if (pControl->contrlData) {
			PopupPrivateDataPtr pMenu =
				reinterpret_cast<PopupPrivateDataPtr>(*pControl->contrlData);
			pMenu->mHandle = popupMenu;
		}
	}
}

pascal void SetControlPopupMenuID(ControlRef control, short menuID)
{
	if (control) {
		ControlPtr pControl = *control;
		if (pControl->contrlData) {
			PopupPrivateDataPtr pMenu =
				reinterpret_cast<PopupPrivateDataPtr>(*pControl->contrlData);
			pMenu->mID = menuID;
		}
	}
}

pascal void GetParamText(
	StringPtr param0, StringPtr param1, StringPtr param2, StringPtr param3)
{
	StringPtr* hString;
	StringPtr pString;

	// Test if the value is requested
	if (param0) {
		// Force empty string
		param0[0] = 0;
		hString = LMGetDAStrings(0);
		// Was there a string?
		if (hString) {
			pString = *hString;
			// Copy the string into the return buffer
			BlockMoveData(pString, param0, pString[0] + 1);
		}
	}

	if (param1) {
		param1[0] = 0;
		hString = LMGetDAStrings(1);
		if (hString) {
			pString = *hString;
			BlockMoveData(pString, param1, pString[0] + 1);
		}
	}
	if (param2) {
		param2[0] = 0;
		hString = LMGetDAStrings(2);
		if (hString) {
			pString = *hString;
			BlockMoveData(pString, param2, pString[0] + 1);
		}
	}
	if (param3) {
		param3[0] = 0;
		hString = LMGetDAStrings(3);
		if (hString) {
			pString = *hString;
			BlockMoveData(pString, param3, pString[0] + 1);
		}
	}
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Quickdraw ===
#endif

/***************************************

	Get the root pointer to QuickDraw's globals

***************************************/

QDGlobals* GetQDGlobals(void)
{
	return reinterpret_cast<QDGlobals*>(
		(*reinterpret_cast<Handle>(LMGetCurrentA5())) -
		(sizeof(QDGlobals) - sizeof(GrafPtr)));
}

/***************************************

	Make sure the grafport is valid

***************************************/

CGrafPtr GetCorrectPort(CGrafPtr port)
{
	if (!port) {
		GrafPtr TempPort;
		GetPort(&TempPort);
		port = (CGrafPtr)TempPort;
	}
	return port;
}

/***************************************

	Set quickdraw error

***************************************/

pascal void SetQDError(int16_t err)
{
	QDErrLM[0] = err;
}

/***************************************

	Get the port pix map

***************************************/

pascal PixMapHandle GetPortPixMap(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);

	if (port) {
		if (port->portVersion < 0) {
			return port->portPixMap;
		}
	}
	SetQDError(paramErr);
	return nullptr;
}

/***************************************

	Get the port bit map for CopyBits()

***************************************/

pascal const BitMap* GetPortBitMapForCopyBits(CGrafPtr port)
{
	// Get the current port if not already set
	if (!port) {
		GrafPtr TempPort;
		GetPort(&TempPort);
		port = (CGrafPtr)TempPort;
		if (!port) {
			SetQDError(paramErr);
			return nullptr;
		}
	}
	return &reinterpret_cast<GrafPtr>(port)->portBits;
}

pascal Rect* GetPortBounds(CGrafPtr port, Rect* rect)
{
	// Valid port and region
	if (!rect || ((port = GetCorrectPort(port)) == nullptr)) {
		SetQDError(paramErr);
	} else {
		*rect = port->portRect;
	}
	return rect;
}

pascal Boolean IsPortRegionBeingDefined(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return FALSE;
	}
	return port->rgnSave != nullptr;
}

pascal Boolean IsPortPictureBeingDefined(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return FALSE;
	}
	return port->picSave != nullptr;
}

pascal Boolean IsPortPolyBeingDefined(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	return port->polySave != nullptr;
}

pascal Boolean IsPortOffscreen(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return FALSE;
	}
	return ((static_cast<uint_t>(port->portVersion) & 0xC001U) == 0xC001U);
}

pascal Boolean IsPortColor(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return FALSE;
	}
	return (port->portVersion < 0);
}

/***************************************

	Get the port's visible region

***************************************/

pascal RgnHandle GetPortVisibleRegion(CGrafPtr port, RgnHandle visRgn)
{
	// Valid port and region
	if (!visRgn || ((port = GetCorrectPort(port)) == nullptr)) {
		SetQDError(paramErr);
	} else {
		CopyRgn(port->visRgn, visRgn);
	}
	return visRgn;
}

pascal RgnHandle GetPortClipRegion(CGrafPtr port, RgnHandle clipRgn)
{
	// Valid port and region
	if (!clipRgn || ((port = GetCorrectPort(port)) == nullptr)) {
		SetQDError(paramErr);
	} else {
		CopyRgn(port->clipRgn, clipRgn);
	}
	return clipRgn;
}

pascal RGBColor* GetPortForeColor(CGrafPtr port, RGBColor* foreColor)
{
	// Valid port and pattern
	if (!foreColor || ((port = GetCorrectPort(port)) == nullptr) ||
		(port->portVersion >= 0)) {
		SetQDError(paramErr);
	} else {
		*foreColor = port->rgbFgColor;
	}
	return foreColor;
}

pascal RGBColor* GetPortBackColor(CGrafPtr port, RGBColor* backColor)
{
	// Valid port and pattern
	if (!backColor || ((port = GetCorrectPort(port)) == nullptr) ||
		(port->portVersion >= 0)) {
		SetQDError(paramErr);
	} else {
		*backColor = port->rgbBkColor;
	}
	return backColor;
}

pascal PixPatHandle GetPortBackPixPat(CGrafPtr port, PixPatHandle backPattern)
{
	// Valid port and pattern
	if (!backPattern || ((port = GetCorrectPort(port)) == nullptr) ||
		(port->portVersion >= 0)) {
		SetQDError(paramErr);
	} else {
		CopyPixPat(port->bkPixPat, backPattern);
	}
	return backPattern;
}

pascal PixPatHandle GetPortPenPixPat(CGrafPtr port, PixPatHandle penPattern)
{
	// Valid port and pattern
	if (!penPattern || ((port = GetCorrectPort(port)) == nullptr) ||
		(port->portVersion >= 0)) {
		SetQDError(paramErr);
	} else {
		CopyPixPat(port->pnPixPat, penPattern);
	}
	return penPattern;
}

pascal PixPatHandle GetPortFillPixPat(CGrafPtr port, PixPatHandle fillPattern)
{
	// Valid port and pattern
	if (!fillPattern || ((port = GetCorrectPort(port)) == nullptr) ||
		(port->portVersion >= 0)) {
		SetQDError(paramErr);
	} else {
		CopyPixPat(port->fillPixPat, fillPattern);
	}
	return fillPattern;
}

pascal short GetPortTextFont(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return 0;
	}
	return port->txFont;
}

pascal Style GetPortTextFace(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return 0;
	}
	return port->txFace;
}

pascal short GetPortTextMode(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return srcOr;
	}
	return port->txMode;
}

pascal short GetPortFracHPenLocation(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return -32768;
	}
	return port->pnLocHFrac;
}

pascal short GetPortChExtra(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return 0;
	}
	return port->chExtra;
}

pascal short GetPortPenVisibility(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return 0;
	}
	return port->pnVis;
}

pascal Fixed GetPortSpExtra(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return 0;
	}
	return port->spExtra;
}

pascal short GetPortTextSize(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return 0;
	}
	return port->txSize;
}

pascal CQDProcsPtr GetPortGrafProcs(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return nullptr;
	}
	return port->grafProcs;
}

pascal RGBColor* GetPortOpColor(CGrafPtr port, RGBColor* opColor)
{
	// Valid port and color
	if (!opColor || ((port = GetCorrectPort(port)) == nullptr) ||
		(port->portVersion >= 0) || !(*port->grafVars)) {
		SetQDError(paramErr);
	} else {
		Handle hGrafVars = port->grafVars;
		const GrafVars* pGrafVars =
			reinterpret_cast<const GrafVars*>(*hGrafVars);
		*opColor = pGrafVars->rgbOpColor;
	}
	return opColor;
}

pascal RGBColor* GetPortHiliteColor(CGrafPtr port, RGBColor* hiliteColor)
{
	// Valid port and color
	if (!hiliteColor || ((port = GetCorrectPort(port)) == nullptr) ||
		(port->portVersion >= 0) || !(*port->grafVars)) {
		SetQDError(paramErr);
	} else {
		Handle hGrafVars = port->grafVars;
		const GrafVars* pGrafVars =
			reinterpret_cast<const GrafVars*>(*hGrafVars);
		*hiliteColor = pGrafVars->rgbHiliteColor;
	}
	return hiliteColor;
}

pascal Rect* GetPixBounds(PixMapHandle pixMap, Rect* bounds)
{
	// Valid pixmap and return address
	if (!bounds || !pixMap) {
		SetQDError(paramErr);
	} else {
		*bounds = (*pixMap)->bounds;
	}
	return bounds;
}

pascal short GetPixDepth(PixMapHandle pixMap)
{
	if (!pixMap) {
		SetQDError(paramErr);
		return 0;
	}
	return (*pixMap)->pixelSize;
}

pascal Handle GetPortPalette(CGrafPtr port)
{
	if (!port) {
		return nullptr;
	}
	Handle hGrafVars = port->grafVars;
	const GrafVars* pGrafVars = reinterpret_cast<const GrafVars*>(*hGrafVars);
	return pGrafVars->pmFgColor;
}

pascal Point* GetPortPenSize(CGrafPtr port, Point* penSize)
{
	// Valid port and point
	if (!penSize || ((port = GetCorrectPort(port)) == nullptr)) {
		SetQDError(paramErr);
	} else {
		*penSize = port->pnSize;
	}
	return penSize;
}

pascal SInt32 GetPortPenMode(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	if (!port) {
		SetQDError(paramErr);
		return 0;
	}
	return port->pnMode;
}

pascal Point* GetPortPenLocation(CGrafPtr port, Point* penLocation)
{
	// Valid port and point
	if (!penLocation || ((port = GetCorrectPort(port)) == nullptr)) {
		SetQDError(paramErr);
	} else {
		*penLocation = port->pnLoc;
	}
	return penLocation;
}

pascal Boolean IsPortVisibleRegionEmpty(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	// Valid port
	if (!port) {
		SetQDError(paramErr);
		// Force true on error
		return TRUE;
	}
	return EmptyRgn(port->visRgn);
}

pascal Boolean IsPortClipRegionEmpty(CGrafPtr port)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	// Valid port
	if (!port) {
		SetQDError(paramErr);
		// Force true on error
		return TRUE;
	}
	return EmptyRgn(port->clipRgn);
}

// NOTE: CarbonAccessors.o has a bug where it always calls SetQDError()
// This version doesn't have that bug

pascal void SectRegionWithPortClipRegion(CGrafPtr port, RgnHandle ioRegion)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	// Valid port and region
	if (!port || !ioRegion) {
		SetQDError(paramErr);
	} else {
		SectRgn(port->clipRgn, ioRegion, ioRegion);
	}
}

// NOTE: CarbonAccessors.o has a bug where it always calls SetQDError()
// This version doesn't have that bug

pascal void SectRegionWithPortVisibleRegion(CGrafPtr port, RgnHandle ioRegion)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	// Valid port and region
	if (!port || !ioRegion) {
		SetQDError(paramErr);
	} else {
		SectRgn(port->visRgn, ioRegion, ioRegion);
	}
}

/***************************************

	Quickdraw set / swap functions

***************************************/

pascal Handle SwapPortPicSaveHandle(CGrafPtr port, Handle inPicSaveHdl)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	Handle hResult = port->picSave;
	port->picSave = inPicSaveHdl;
	return hResult;
}

pascal Handle SwapPortPolySaveHandle(CGrafPtr port, Handle inPolySaveHdl)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	Handle hResult = port->polySave;
	port->polySave = inPolySaveHdl;
	return hResult;
}

pascal Handle SwapPortRegionSaveHandle(CGrafPtr port, Handle inRegionSaveHdl)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	Handle hResult = port->rgnSave;
	port->rgnSave = inRegionSaveHdl;
	return hResult;
}

pascal void SetPortBounds(CGrafPtr port, const Rect* rect)
{
	// Valid port and region
	if (!rect || ((port = GetCorrectPort(port)) == nullptr)) {
		SetQDError(paramErr);
	} else {
		port->portRect = *rect;
	}
}

pascal void SetPortVisibleRegion(CGrafPtr port, RgnHandle visRgn)
{
	// Valid port and region
	if (!visRgn || ((port = GetCorrectPort(port)) == nullptr)) {
		SetQDError(paramErr);
	} else {
		CopyRgn(visRgn, port->visRgn);
	}
}

pascal void SetPortClipRegion(CGrafPtr port, RgnHandle clipRgn)
{
	// Valid port and region
	if (!clipRgn || ((port = GetCorrectPort(port)) == nullptr)) {
		SetQDError(paramErr);
	} else {
		CopyRgn(clipRgn, port->clipRgn);
	}
}

pascal void SetPortPenPixPat(CGrafPtr port, PixPatHandle penPattern)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);

	// Valid color port?
	if (!port || !penPattern || (port->portVersion >= 0)) {
		SetQDError(paramErr);
	} else {
		// Set to the port to modify
		GrafPtr TempPort2;
		GetPort(&TempPort2);
		if (TempPort2 != (GrafPtr)port) {
			SetPort((GrafPtr)port);
		}
		PixPatHandle hNewPixPat = NewPixPat();
		if (hNewPixPat) {
			// Copy the pixel pattern
			CopyPixPat(penPattern, hNewPixPat);

			// Mark to old pattern type
			(*hNewPixPat)->patType = QDOldPat;
			PenPixPat(hNewPixPat);
		}
		// Set to the previous port (Which initializes
		// the procs)
		if (TempPort2 != (GrafPtr)port) {
			SetPort(TempPort2);
		}
	}
}

pascal void SetPortBackPixPat(CGrafPtr port, PixPatHandle backPattern)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);

	// Valid color port?
	if (!port || !backPattern || (port->portVersion >= 0)) {
		SetQDError(paramErr);
	} else {
		// Set to the port to modify
		GrafPtr TempPort2;
		GetPort(&TempPort2);
		if (TempPort2 != (GrafPtr)port) {
			SetPort((GrafPtr)port);
		}
		PixPatHandle hNewPixPat = NewPixPat();
		if (hNewPixPat) {
			// Copy the pixel pattern
			CopyPixPat(backPattern, hNewPixPat);

			// Mark to old pattern type
			(*hNewPixPat)->patType = QDOldPat;
			BackPixPat(hNewPixPat);
		}
		// Set to the previous port (Which initializes
		// the procs)
		if (TempPort2 != (GrafPtr)port) {
			SetPort(TempPort2);
		}
	}
}

pascal void SetPortFillPixPat(CGrafPtr port, PixPatHandle penPattern)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);

	// Valid color port?
	if (!port || !penPattern || (port->portVersion >= 0)) {
		SetQDError(paramErr);
	} else {
		// Set to the port to modify
		GrafPtr TempPort2;
		GetPort(&TempPort2);
		if (TempPort2 != (GrafPtr)port) {
			SetPort((GrafPtr)port);
		}

		// Copy the pixel pattern
		CopyPixPat(penPattern, port->fillPixPat);

		// Mark to old pattern type
		(*port->fillPixPat)->patType = QDOldPat;

		// Set to the previous port (Which initializes
		// the procs)
		if (TempPort2 != (GrafPtr)port) {
			SetPort(TempPort2);
		}
	}
}

pascal void SetPortOpColor(CGrafPtr port, const RGBColor* opColor)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);

	// Valid color port?
	if (!port || (port->portVersion >= 0) || !*(port->grafVars)) {
		SetQDError(paramErr);
	} else {
		GrafVars* pGrafVars = reinterpret_cast<GrafVars*>(*(port->grafVars));
		pGrafVars->rgbOpColor = *opColor;
	}
}

pascal void SetPortPenSize(CGrafPtr port, const Point& penSize)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);

	// Valid port?
	if (!port) {
		SetQDError(paramErr);
	} else {
		port->pnSize = penSize;
	}
}

pascal void SetPortPenMode(CGrafPtr port, SInt32 penMode)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	// Valid port?
	if (!port) {
		SetQDError(paramErr);
	} else {
		port->pnMode = static_cast<short>(penMode);
	}
}

pascal void SetPortTextFont(CGrafPtr port, short txFont)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	port->txFont = txFont;
}

pascal void SetPortTextSize(CGrafPtr port, short txSize)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	port->txSize = txSize;
}

pascal void SetPortTextFace(CGrafPtr port, StyleParameter face)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	port->txFace = static_cast<StyleField>(face);
}

pascal void SetPortTextMode(CGrafPtr port, short mode)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	port->txMode = mode;
}

pascal void SetPortFracHPenLocation(CGrafPtr port, short pnLocHFrac)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);

	// Valid color port?
	if (!port || (port->portVersion >= 0)) {
		SetQDError(paramErr);
	} else {
		port->pnLocHFrac = pnLocHFrac;
	}
}

pascal void SetPortGrafProcs(CGrafPtr port, CQDProcsPtr procs)
{
	// Get the current port if not already set
	port = GetCorrectPort(port);
	// Valid port?
	if (!port) {
		SetQDError(paramErr);
	} else {
		// Set to the port to modify
		GrafPtr TempPort2;
		GetPort(&TempPort2);
		SetPort((GrafPtr)port);

		// Set the function pointers
		port->grafProcs = procs;

		// Set to the previous port (Which initializes
		// the procs)
		SetPort(TempPort2);
	}
}

/***************************************

	Grow the GrafVars for a printer record

***************************************/

void GrowPortGrafVars(CGrafPtr port)
{
	Handle hOldHandle = port->grafVars;
	Size uSize = GetHandleSize(hOldHandle);
	// Is it too small?
	if (uSize < 62) {
		// Use the handle zone where the memory was allocated from
		THz hOldZone = GetZone();
		SetZone(HandleZone(hOldHandle));

		// Get the updated buffer
		Handle hNew = NewHandleClear(172);
		if (hNew) {
			// Copy the data from the old handle to the new one
			BlockMoveData(*hOldHandle, *hNew, uSize);

			// Update and dispose of the old handle
			port->grafVars = hNew;
			DisposeHandle(hOldHandle);
		}
		SetZone(hOldZone);
	}
}

pascal long GetQDGlobalsRandomSeed(void)
{
	QDGlobals* pGlobals = GetQDGlobals();
	return pGlobals->randSeed;
}

pascal BitMap* GetQDGlobalsScreenBits(BitMap* screenBits)
{
	QDGlobals* pGlobals = GetQDGlobals();
	*screenBits = pGlobals->screenBits;
	return screenBits;
}

pascal Cursor* GetQDGlobalsArrow(Cursor* arrow)
{
	QDGlobals* pGlobals = GetQDGlobals();
	*arrow = pGlobals->arrow;
	return arrow;
}

pascal Pattern* GetQDGlobalsDarkGray(Pattern* dkGray)
{
	QDGlobals* pGlobals = GetQDGlobals();
	*dkGray = pGlobals->dkGray;
	return dkGray;
}

pascal Pattern* GetQDGlobalsLightGray(Pattern* ltGray)
{
	QDGlobals* pGlobals = GetQDGlobals();
	*ltGray = pGlobals->ltGray;
	return ltGray;
}

pascal Pattern* GetQDGlobalsGray(Pattern* gray)
{
	QDGlobals* pGlobals = GetQDGlobals();
	*gray = pGlobals->gray;
	return gray;
}

pascal Pattern* GetQDGlobalsBlack(Pattern* black)
{
	QDGlobals* pGlobals = GetQDGlobals();
	*black = pGlobals->black;
	return black;
}

pascal Pattern* GetQDGlobalsWhite(Pattern* white)
{
	QDGlobals* pGlobals = GetQDGlobals();
	*white = pGlobals->white;
	return white;
}

pascal CGrafPtr GetQDGlobalsThePort(void)
{
	QDGlobals* pGlobals = GetQDGlobals();
	return reinterpret_cast<CGrafPtr>(pGlobals->thePort);
}

pascal void SetQDGlobalsArrow(const Cursor* arrow)
{
	QDGlobals* pGlobals = GetQDGlobals();
	pGlobals->arrow = *arrow;
}

pascal void SetQDGlobalsRandomSeed(long randomSeed)
{
	QDGlobals* pGlobals = GetQDGlobals();
	pGlobals->randSeed = randomSeed;
}

pascal Rect* GetRegionBounds(RgnHandle region, Rect* bounds)
{
	if (!region) {
		SetQDError(paramErr);
	} else {
		*bounds = (*region)->rgnBBox;
	}
	return bounds;
}

pascal Boolean IsRegionRectangular(RgnHandle region)
{
	MacRegion* pRegion = *region;
	return pRegion->rgnSize == 10;
}

pascal CGrafPtr CreateNewPort(void)
{
	CGrafPtr pResult = reinterpret_cast<CGrafPtr>(NewPtr(sizeof(CGrafPort)));
	if (pResult) {
		OpenCPort(pResult);
	}
	return pResult;
}

pascal void DisposePort(CGrafPtr port)
{
	CloseCPort(port);
	DisposePtr(reinterpret_cast<Ptr>(port));
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Text Services ===
#endif

/***************************************

	Text Services functions

***************************************/

pascal Boolean IsTSMTEDialog(DialogRef dialog)
{
	if (dialog) {
		const TSMDialogRecord* pRecord =
			reinterpret_cast<const TSMDialogRecord*>(dialog);
		if (pRecord->fTSMTERsvd[2] == kTSMTESignature) {
			if (pRecord->fDocID) {
				return TRUE;
			}
		}
	}
	return FALSE;
}

pascal TSMDocumentID GetTSMTEDialogDocumentID(DialogRef dialog)
{
	TSMDocumentID uResult = 0;
	if (dialog) {
		const TSMDialogRecord* pRecord =
			reinterpret_cast<const TSMDialogRecord*>(dialog);
		if (pRecord->fTSMTERsvd[2] == kTSMTESignature) {
			uResult = pRecord->fDocID;
		}
	}
	return uResult;
}

pascal TSMTERecHandle GetTSMTEDialogTSMTERecHandle(DialogRef dialog)
{
	TSMTERecHandle hResult = nullptr;
	if (dialog) {
		const TSMDialogRecord* pRecord =
			reinterpret_cast<const TSMDialogRecord*>(dialog);
		if (pRecord->fTSMTERsvd[2] == kTSMTESignature) {
			if (pRecord->fDocID) {
				hResult = pRecord->fTSMTERecH;
			}
		}
	}
	return hResult;
}

pascal void SetTSMTEDialogDocumentID(DialogRef dialog, TSMDocumentID documentID)
{
	if (dialog) {
		TSMDialogRecord* pRecord = reinterpret_cast<TSMDialogRecord*>(dialog);
		if (pRecord->fTSMTERsvd[2] == kTSMTESignature) {
			pRecord->fDocID = documentID;
		}
	}
}

pascal void SetTSMTEDialogTSMTERecHandle(
	DialogRef dialog, TSMTERecHandle tsmteRecHandle)
{
	if (dialog) {
		TSMDialogRecord* pRecord = reinterpret_cast<TSMDialogRecord*>(dialog);
		if (pRecord->fTSMTERsvd[2] == kTSMTESignature) {
			if (pRecord->fDocID) {
				pRecord->fTSMTERecH = tsmteRecHandle;
			}
		}
	}
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Menu Manager ===
#endif

/***************************************

	Get menu values

***************************************/

pascal MenuID GetMenuID(MenuRef menu)
{
	if (menu) {
		return (*menu)->menuID;
	}
	return 0;
}

pascal SInt16 GetMenuWidth(MenuRef menu)
{
	if (menu) {
		return (*menu)->menuWidth;
	}
	return 0;
}

pascal SInt16 GetMenuHeight(MenuRef menu)
{
	if (menu) {
		return (*menu)->menuHeight;
	}
	return 0;
}

pascal StringPtr GetMenuTitle(MenuRef menu, Str255 title)
{
	if (menu && title) {
		// Copy the string
		const MenuInfo* pMenu = *menu;
		BlockMoveData(pMenu->menuData, title, pMenu->menuData[0] + 1);

		// Test if this is a special menu without a title
		if (reinterpret_cast<const int16_t*>(title)[0] == 0x0501) {
			// Kill the title returned
			title[0] = 0;
		}
	}
	return title;
}

/***************************************

	Set menu values

***************************************/

pascal void SetMenuID(MenuRef menu, MenuID menuID)
{
	if (menu) {
		(*menu)->menuID = menuID;
	}
}

pascal void SetMenuWidth(MenuRef menu, SInt16 width)
{
	if (menu) {
		(*menu)->menuWidth = width;
	}
}

pascal void SetMenuHeight(MenuRef menu, SInt16 height)
{
	if (menu) {
		(*menu)->menuHeight = height;
	}
}

/***************************************

	Call the MDBF menu proc directly

***************************************/

static long CallMenuBar(short message, short parameter1, long uResult)
{
	// Get the menu bar header
	MenuBarHeader** hMenuBarHeader = (MenuBarHeader**)LMGetMenuList();
	if (!hMenuBarHeader) {
		uResult = 0;
	} else if ((intptr_t)hMenuBarHeader == -1) {
		uResult = 0;
	} else {
		// Get the resource ID of the menu bar proc
		SInt16 sResID = (*hMenuBarHeader)->mbResID;

		// Use the ROM
		LMSetROMMapInsert(255);
		// Temp loading enable
		LMSetTmpResLoad(255);

		// Load in the Menu Bar procedure
		Handle hResource = GetResource('MBDF', sResID);
		if (!hResource) {
			uResult = 0;
		} else {
			// Get the handle flag
			SInt8 oldState = HGetState(hResource);

			// Lock it down
			HLock(hResource);

			// Set the low memory global
			*MBDFHndl = hResource;

			// Call the MenuBarDefUPP
			uResult = CALL_FOUR_PARAMETER_UPP(
				reinterpret_cast<MenuBarDefUPP>(*hResource),
				uppMenuBarDefProcInfo, static_cast<short>(sResID & 0x7),
				message, parameter1, uResult);

			// Restore the flag
			HSetState(hResource, oldState);
		}
	}
	return uResult;
}

/***************************************

	Set the title text of a menu bar

***************************************/

pascal OSStatus SetMenuTitle(MenuRef menu, ConstStr255Param title)
{
	// Error handlers
	if (!menu) {
		return menuInvalidErr;
	}
	if (!title) {
		return paramErr;
	}

	// Replace the menu bar title text, use the
	Munger((Handle)menu, sizeof(MenuInfo) - sizeof(Str255), nullptr,
		(*menu)->menuData[0] + 1, title, title[0] + 1);

	GetMenuHandle((*menu)->menuID);

	// Update the menu bar size
	CallMenuBar(kMenuSizeMsg, 0, 0);

	// Force a menu bar update
	InvalMenuBar();
	return noErr;
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Mouse Manager ===
#endif

/***************************************

	Get the global mouse location

***************************************/

pascal void GetGlobalMouse(Point* globalMouse)
{
	*globalMouse = LMGetMouseLocation();
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === List Manager ===
#endif

/***************************************

	Get a value from a ListHandle

***************************************/

pascal Rect* GetListViewBounds(ListRef list, Rect* view)
{
	Rect* pTemp = &(*list)->rView;
	*view = *pTemp;
	return view;
}

pascal CGrafPtr GetListPort(ListRef list)
{
	return reinterpret_cast<CGrafPtr>((*list)->port);
}

pascal Point* GetListCellIndent(ListRef list, Point* indent)
{
	Point* pTemp = &(*list)->indent;
	*indent = *pTemp;
	return indent;
}

pascal Point* GetListCellSize(ListRef list, Point* size)
{
	Point* pTemp = &(*list)->cellSize;
	*size = *pTemp;
	return size;
}

pascal ListBounds* GetListVisibleCells(ListRef list, ListBounds* visible)
{
	ListBounds* pTemp = &(*list)->visible;
	*visible = *pTemp;
	return visible;
}

pascal ControlRef GetListVerticalScrollBar(ListRef list)
{
	return (*list)->vScroll;
}

pascal ControlRef GetListHorizontalScrollBar(ListRef list)
{
	return (*list)->hScroll;
}

pascal OptionBits GetListFlags(ListRef list)
{
	return (*list)->listFlags;
}

pascal OptionBits GetListSelectionFlags(ListRef list)
{
	return (*list)->selFlags;
}

pascal Boolean GetListActive(ListRef list)
{
	return (*list)->lActive;
}

pascal SInt32 GetListClickTime(ListRef list)
{
	return (*list)->clikTime;
}

pascal Point* GetListClickLocation(ListRef list, Point* click)
{
	Point* pTemp = &(*list)->clikLoc;
	*click = *pTemp;
	return click;
}

pascal Point* GetListMouseLocation(ListHandle list, Point* mouse)
{
	Point* pTemp = &(*list)->mouseLoc;
	*mouse = *pTemp;
	return mouse;
}

pascal ListClickLoopUPP GetListClickLoop(ListRef list)
{
	return (*list)->lClickLoop;
}

pascal SInt32 GetListRefCon(ListRef list)
{
	return (*list)->refCon;
}

pascal Handle GetListDefinition(ListRef list)
{
	return (*list)->listDefProc;
}

pascal Handle GetListUserHandle(ListRef list)
{
	return (*list)->userHandle;
}

pascal ListBounds* GetListDataBounds(ListHandle list, ListBounds* bounds)
{
	ListBounds* pTemp = &(*list)->dataBounds;
	*bounds = *pTemp;
	return bounds;
}

pascal DataHandle GetListDataHandle(ListHandle list)
{
	return (*list)->cells;
}

/***************************************

	Set a value in a ListHandle

***************************************/

pascal void SetListViewBounds(ListHandle list, const Rect* view)
{
	(*list)->rView = *view;
}

pascal void SetListPort(ListHandle list, CGrafPtr port)
{
	(*list)->port = reinterpret_cast<GrafPtr>(port);
}

pascal void SetListCellIndent(ListHandle list, Point* indent)
{
	if (list) {
		(*list)->indent = *indent;
	}
}

pascal void SetListClickTime(ListHandle list, SInt32 time)
{
	if (list) {
		(*list)->clikTime = time;
	}
}

pascal void SetListClickLoop(ListHandle list, ListClickLoopUPP clickLoop)
{
	if (list) {
		(*list)->lClickLoop = clickLoop;
	}
}

pascal void SetListLastClick(ListHandle list, Cell* lastClick)
{
	if (list) {
		(*list)->lastClick = *lastClick;
	}
}

pascal void SetListRefCon(ListHandle list, SInt32 refCon)
{
	if (list) {
		(*list)->refCon = refCon;
	}
}

pascal void SetListFlags(ListHandle list, OptionBits listFlags)
{
	if (list) {
		(*list)->listFlags = static_cast<SInt8>(listFlags);
	}
}

pascal void SetListSelectionFlags(ListHandle list, OptionBits selectionFlags)
{
	if (list) {
		(*list)->selFlags = static_cast<SInt8>(selectionFlags);
	}
}

pascal void SetListUserHandle(ListHandle list, Handle userHandle)
{
	if (list) {
		(*list)->userHandle = userHandle;
	}
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Misc functions ===
#endif

/***************************************

	Grab a specific symbol from a library

***************************************/

Ptr GrabSpecifiedCFMSymbol(ConstStr63Param libName, ConstStr255Param symName)
{
	Ptr symAddr = nullptr;
	CFragConnectionID connectionID;
	Ptr mainAddr;
	Str255 errMessage;

	// Load the shared library
	OSErr uError = GetSharedLibrary(libName, kPowerPCCFragArch, kFindCFrag,
		&connectionID, &mainAddr, errMessage);
	if (!uError) {
		// Get the symbol from the library
		CFragSymbolClass symClass;
		FindSymbol(connectionID, symName, &symAddr, &symClass);
	}
	return symAddr;
}

/***************************************

	Copy a pascal string to a C string
	Must be able to a conversion in place.

***************************************/

void CopyPascalStringToC(ConstStr255Param src, char* dst)
{
	if (src) {
		uint_t uLength = *src++;
		if (uLength) {
			do {
				*dst++ = static_cast<char>(*src++);
			} while (--uLength);
		}
	}
	*dst = 0;
}

/***************************************

	Copy a C string to a pascal string
	Must be able to a conversion in place.

***************************************/

void CopyCStringToPascal(const char* src, Str255 dst)
{
	uint_t uLength = 0;

	// Handle the case of an in place conversion
	if (static_cast<const void*>(src) == static_cast<const void*>(dst)) {
		uint8_t* pDest = dst + 1;
		register uint8_t uTemp;

		uTemp = static_cast<uint8_t>(*src++);
		if (uTemp) {
			do {
				// The order of reads and writes are important
				// due to the in place conversion of a "C"
				// string into a pascal string in buffer

				uint8_t uTemp2 = static_cast<uint8_t>(*src++);
				*pDest++ = uTemp;
				uTemp = uTemp2;

				// Overflow?
				if (++uLength == 255U) {
					break;
				}
			} while (uTemp);
		}
	} else if (src) {
		uint8_t* pDest = dst + 1;
		uint_t uCountdown = 255;
		register uint8_t uTemp;

		// When copying the string, stop at the final zero
		if ((uTemp = static_cast<uint8_t>(*src++)) != 0) {
			do {
				*pDest++ = uTemp;
				if (!--uCountdown) {
					break;
				}
			} while ((uTemp = static_cast<uint8_t>(*src++)) != 0);
		}
		uLength = 255U - uCountdown;
	}
	// Save the pascal length
	dst[0] = static_cast<uint8_t>(uLength);
}

/***************************************

	Copy a C string to a pascal string

***************************************/

void c2pstrcpy(Str255 dst, const char* src)
{
	CopyCStringToPascal(src, dst);
}

/***************************************

	Copy a pascal string to a C string

***************************************/

void p2cstrcpy(char* dst, ConstStr255Param src)
{
	CopyPascalStringToC(src, dst);
}

/***************************************

	Get the root pointer to the window list

***************************************/

pascal WindowRef GetWindowList(void)
{
	return LMGetWindowList();
}

#endif
