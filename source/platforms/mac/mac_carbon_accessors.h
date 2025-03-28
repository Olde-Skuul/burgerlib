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

#ifndef __MAC_CARBON_ACCESSORS_H__
#define __MAC_CARBON_ACCESSORS_H__

#ifndef __MAC_TYPES_H__
#include "mac_types.h"
#endif

#ifndef __BRSTRUCTS_H__
#include "brstructs.h"
#endif

/* BEGIN */
#if defined(BURGER_MACCLASSIC) && !ACCESSOR_CALLS_ARE_FUNCTIONS && \
	!defined(DOXYGEN)
extern "C" {

extern void AEInitializeDesc(AEDesc* desc);
extern uint8_t AECheckIsRecord(const AEDesc* theDesc);
extern int16_t AEGetDescData(
	const AEDesc* theAEDesc, void* dataPtr, long maximumSize);
extern long AEGetDescDataSize(const AEDesc* theAEDesc);
extern int16_t AEReplaceDescData(unsigned long typeCode, const void* dataPtr,
	long dataSize, AEDesc* theAEDesc);
extern long AESizeOfFlattenedDesc(const AEDesc* theAEDesc);
extern signed long AEFlattenDesc(
	const AEDesc* theAEDesc, char* buffer, long bufferSize, long* actualSize);
extern signed long AEUnflattenDesc(char* buffer, AEDesc* result);

extern pascal Rect* GetControlBounds(ControlRecord** control, Rect* bounds);
extern pascal uint8_t IsControlHilited(ControlRecord** control);
extern pascal uint16_t GetControlHilite(ControlRecord** control);
extern pascal char** GetControlDataHandle(ControlRecord** control);
extern pascal WindowRef GetControlOwner(ControlRecord** control);
extern pascal MenuInfo** GetControlPopupMenuHandle(ControlRecord** control);
extern pascal short GetControlPopupMenuID(ControlRecord** control);
extern pascal void SetControlBounds(
	ControlRecord** control, const Rect* bounds);
extern pascal void SetControlDataHandle(
	ControlRecord** control, char** dataHandle);
extern pascal void SetControlPopupMenuHandle(
	ControlRecord** control, MenuInfo** popupMenu);
extern pascal void SetControlPopupMenuID(ControlRecord** control, short menuID);
extern pascal void GetParamText(
	uint8_t* param0, uint8_t* param1, uint8_t* param2, uint8_t* param3);

extern QDGlobals* GetQDGlobals(void);
extern CGrafPort* GetCorrectPort(CGrafPort* port);
extern pascal void SetQDError(int16_t err);
extern pascal PixMap** GetPortPixMap(CGrafPort* port);
extern pascal const BitMap* GetPortBitMapForCopyBits(CGrafPort* port);
extern pascal Rect* GetPortBounds(CGrafPort* port, Rect* rect);
extern pascal uint8_t IsPortRegionBeingDefined(CGrafPort* port);
extern pascal uint8_t IsPortPictureBeingDefined(CGrafPort* port);
extern pascal uint8_t IsPortPolyBeingDefined(CGrafPort* port);
extern pascal uint8_t IsPortOffscreen(CGrafPort* port);
extern pascal uint8_t IsPortColor(CGrafPort* port);
extern pascal MacRegion** GetPortVisibleRegion(
	CGrafPort* port, MacRegion** visRgn);
extern pascal MacRegion** GetPortClipRegion(
	CGrafPort* port, MacRegion** clipRgn);
extern pascal RGBColor* GetPortForeColor(CGrafPort* port, RGBColor* foreColor);
extern pascal RGBColor* GetPortBackColor(CGrafPort* port, RGBColor* backColor);
extern pascal PixPat** GetPortBackPixPat(CGrafPort* port, PixPat** backPattern);
extern pascal PixPat** GetPortPenPixPat(CGrafPort* port, PixPat** penPattern);
extern pascal PixPat** GetPortFillPixPat(CGrafPort* port, PixPat** fillPattern);
extern pascal short GetPortTextFont(CGrafPort* port);
extern pascal uint8_t GetPortTextFace(CGrafPort* port);
extern pascal short GetPortTextMode(CGrafPort* port);
extern pascal short GetPortFracHPenLocation(CGrafPort* port);
extern pascal short GetPortChExtra(CGrafPort* port);
extern pascal short GetPortPenVisibility(CGrafPort* port);
extern pascal long GetPortSpExtra(CGrafPort* port);
extern pascal short GetPortTextSize(CGrafPort* port);
extern pascal CQDProcs* GetPortGrafProcs(CGrafPort* port);
extern pascal RGBColor* GetPortOpColor(CGrafPort* port, RGBColor* opColor);
extern pascal RGBColor* GetPortHiliteColor(
	CGrafPort* port, RGBColor* hiliteColor);
extern pascal Rect* GetPixBounds(PixMap** pixMap, Rect* bounds);
extern pascal short GetPixDepth(PixMap** pixMap);
extern pascal char** GetPortPalette(CGrafPort* port);
extern pascal Point* GetPortPenSize(CGrafPort* port, Point* penSize);
extern pascal signed long GetPortPenMode(CGrafPort* port);
extern pascal Point* GetPortPenLocation(CGrafPort* port, Point* penLocation);
extern pascal uint8_t IsPortVisibleRegionEmpty(CGrafPort* port);
extern pascal uint8_t IsPortClipRegionEmpty(CGrafPort* port);
extern pascal void SectRegionWithPortClipRegion(
	CGrafPort* port, MacRegion** ioRegion);
extern pascal void SectRegionWithPortVisibleRegion(
	CGrafPort* port, MacRegion** ioRegion);
extern pascal char** SwapPortPicSaveHandle(
	CGrafPort* port, char** inPicSaveHdl);
extern pascal char** SwapPortPolySaveHandle(
	CGrafPort* port, char** inPolySaveHdl);
extern pascal char** SwapPortRegionSaveHandle(
	CGrafPort* port, char** inRegionSaveHdl);
extern pascal void SetPortBounds(CGrafPort* port, const Rect* rect);
extern pascal void SetPortVisibleRegion(CGrafPort* port, MacRegion** visRgn);
extern pascal void SetPortClipRegion(CGrafPort* port, MacRegion** clipRgn);
extern pascal void SetPortPenPixPat(CGrafPort* port, PixPat** penPattern);
extern pascal void SetPortBackPixPat(CGrafPort* port, PixPat** backPattern);
extern pascal void SetPortFillPixPat(CGrafPort* port, PixPat** penPattern);
extern pascal void SetPortOpColor(CGrafPort* port, const RGBColor* opColor);
extern pascal void SetPortPenSize(CGrafPort* port, const Point& penSize);
extern pascal void SetPortPenMode(CGrafPort* port, signed long penMode);
extern pascal void SetPortTextFont(CGrafPort* port, short txFont);
extern pascal void SetPortTextSize(CGrafPort* port, short txSize);
extern pascal void SetPortTextFace(CGrafPort* port, short face);
extern pascal void SetPortTextMode(CGrafPort* port, short mode);
extern pascal void SetPortFracHPenLocation(CGrafPort* port, short pnLocHFrac);
extern pascal void SetPortGrafProcs(CGrafPort* port, CQDProcs* procs);
extern void GrowPortGrafVars(CGrafPort* port);
extern pascal long GetQDGlobalsRandomSeed(void);
extern pascal BitMap* GetQDGlobalsScreenBits(BitMap* screenBits);
extern pascal Cursor* GetQDGlobalsArrow(Cursor* arrow);
extern pascal Pattern* GetQDGlobalsDarkGray(Pattern* dkGray);
extern pascal Pattern* GetQDGlobalsLightGray(Pattern* ltGray);
extern pascal Pattern* GetQDGlobalsGray(Pattern* gray);
extern pascal Pattern* GetQDGlobalsBlack(Pattern* black);
extern pascal Pattern* GetQDGlobalsWhite(Pattern* white);
extern pascal CGrafPort* GetQDGlobalsThePort(void);
extern pascal void SetQDGlobalsArrow(const Cursor* arrow);
extern pascal void SetQDGlobalsRandomSeed(long randomSeed);
extern pascal Rect* GetRegionBounds(MacRegion** region, Rect* bounds);
extern pascal uint8_t IsRegionRectangular(MacRegion** region);
extern pascal CGrafPort* CreateNewPort(void);
extern pascal void DisposePort(CGrafPort* port);

extern pascal uint8_t IsTSMTEDialog(DialogRef dialog);
extern pascal OpaqueTSMDocumentID* GetTSMTEDialogDocumentID(DialogRef dialog);
extern pascal TSMTERec** GetTSMTEDialogTSMTERecHandle(DialogRef dialog);
extern pascal void SetTSMTEDialogDocumentID(
	DialogRef dialog, OpaqueTSMDocumentID* documentID);
extern pascal void SetTSMTEDialogTSMTERecHandle(
	DialogRef dialog, TSMTERec** tsmteRecHandle);

extern pascal int16_t GetMenuID(MenuInfo** menu);
extern pascal int16_t GetMenuWidth(MenuInfo** menu);
extern pascal int16_t GetMenuHeight(MenuInfo** menu);
extern pascal uint8_t* GetMenuTitle(MenuInfo** menu, uint8_t title[256]);
extern pascal void SetMenuID(MenuInfo** menu, int16_t menuID);
extern pascal void SetMenuWidth(MenuInfo** menu, int16_t width);
extern pascal void SetMenuHeight(MenuInfo** menu, int16_t height);
extern pascal signed long SetMenuTitle(MenuInfo** menu, const uint8_t* title);

extern pascal void GetGlobalMouse(Point* globalMouse);

extern pascal Rect* GetListViewBounds(ListRec** list, Rect* view);
extern pascal CGrafPort* GetListPort(ListRec** list);
extern pascal Point* GetListCellIndent(ListRec** list, Point* indent);
extern pascal Point* GetListCellSize(ListRec** list, Point* size);
extern pascal Rect* GetListVisibleCells(ListRec** list, Rect* visible);
extern pascal ControlRecord** GetListVerticalScrollBar(ListRec** list);
extern pascal ControlRecord** GetListHorizontalScrollBar(ListRec** list);
extern pascal unsigned long GetListFlags(ListRec** list);
extern pascal unsigned long GetListSelectionFlags(ListRec** list);
extern pascal uint8_t GetListActive(ListRec** list);
extern pascal signed long GetListClickTime(ListRec** list);
extern pascal Point* GetListClickLocation(ListRec** list, Point* click);
extern pascal Point* GetListMouseLocation(ListRec** list, Point* mouse);
extern pascal ListClickLoopUPP GetListClickLoop(ListRec** list);
extern pascal signed long GetListRefCon(ListRec** list);
extern pascal char** GetListDefinition(ListRec** list);
extern pascal char** GetListUserHandle(ListRec** list);
extern pascal Rect* GetListDataBounds(ListRec** list, Rect* bounds);
extern pascal char** GetListDataHandle(ListRec** list);
extern pascal void SetListViewBounds(ListRec** list, const Rect* view);
extern pascal void SetListPort(ListRec** list, CGrafPort* port);
extern pascal void SetListCellIndent(ListRec** list, Point* indent);
extern pascal void SetListClickTime(ListRec** list, signed long time);
extern pascal void SetListClickLoop(ListRec** list, ListClickLoopUPP clickLoop);
extern pascal void SetListLastClick(ListRec** list, Point* lastClick);
extern pascal void SetListRefCon(ListRec** list, signed long refCon);
extern pascal void SetListFlags(ListRec** list, unsigned long listFlags);
extern pascal void SetListSelectionFlags(
	ListRec** list, unsigned long selectionFlags);
extern pascal void SetListUserHandle(ListRec** list, char** userHandle);

extern char* GrabSpecifiedCFMSymbol(
	const uint8_t* libName, const uint8_t* symName);
extern void CopyPascalStringToC(const uint8_t* src, char* dst);
extern void CopyCStringToPascal(const char* src, uint8_t dst[256]);
extern void c2pstrcpy(uint8_t dst[256], const char* src);
extern void p2cstrcpy(char* dst, const uint8_t* src);
extern pascal WindowRef GetWindowList(void);
}
#endif
/* END */

#endif
