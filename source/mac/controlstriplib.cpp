/***************************************

    MacOS Only!!!

    There was some oversight on the part of apple not to include PPC versions of
    these routines. So here they are.

    Addendum 2001 : There is now ControlStripLib from Apple so this is now
    obsolete. I only keep this file around for example code for PPC <-> 68k
    calls

    Copyright (c) 1999-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtypes.h"

#if defined(BURGER_POWERPC) && defined(BURGER_MACCLASSIC) && \
    !defined(DOXYGEN) && 0

#include <ControlStrip.h>

/***************************************

    Is the control strip visible?

***************************************/

static const uint16_t SBIsControlStripVisible68K[] = {
    0x554F, // SUBQ.W #$2,A7
    0x7000, // MOVEQ #$00,D0
    0xAAF2, // DC.W $AAF2 ; TB 02F2
    0x101F, // MOVE.B (A7)+,D0
    0x4E75  // RTS
};

pascal Boolean SBIsControlStripVisible(void)
{
    return static_cast<Boolean>(
        CallUniversalProc((UniversalProcPtr)SBIsControlStripVisible68K,
            kRegisterBased | RESULT_SIZE(SIZE_CODE(sizeof(Boolean)))));
}

/***************************************

    Hide or show the control strip

***************************************/

static const uint16_t SBShowHideControlStrip68K[] = {
    0x1F2F, 0x0004, // MOVE.B 4(A7),-(A7)
    0x303C, 0x0101, // MOV #$101,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x4E74, 0x0002  // RTD #2
};

pascal void SBShowHideControlStrip(Boolean showIt)
{
    CallUniversalProc((UniversalProcPtr)SBShowHideControlStrip68K,
        kPascalStackBased |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Boolean))),
        showIt);
}

/***************************************

    Safe to access Startup Disk?

***************************************/

static const uint16_t SBSafeToAccessStartupDisk68K[] = {
    0x554F, // SUBQ.W #$2,A7
    0x7002, // MOVEQ #$02,D0
    0xAAF2, // DC.W $AAF2 ; TB 02F2
    0x101F, // MOVE.B (A7)+,D0
    0x4E75  // RTS
};

pascal Boolean SBSafeToAccessStartupDisk(void)
{
    return static_cast<Boolean>(
        CallUniversalProc((UniversalProcPtr)SBSafeToAccessStartupDisk68K,
            kRegisterBased | RESULT_SIZE(SIZE_CODE(sizeof(Boolean)))));
}

/***************************************

    Open a module resource file

***************************************/

static const uint16_t SBOpenModuleResourceFile68K[] = {
    0x554F,         // SUBQ.W #2,A7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x303C, 0x0203, // MOV #$203,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0004  // RTD #4
};

pascal short SBOpenModuleResourceFile(OSType fileCreator)
{
    return static_cast<short>(
        CallUniversalProc((UniversalProcPtr)SBOpenModuleResourceFile68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(short))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(OSType))),
            fileCreator));
}

/***************************************

    Load a resource from a preferences file

***************************************/

static const uint16_t SBLoadPreferences68K[] = {
    0x554F,         // SUBQ.W #2,A7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x2F2F, 0x000E, // MOVE.L 14(A7),-(a7)
    0x303C, 0x0404, // MOV #$404,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0008  // RTD #8
};

pascal OSErr SBLoadPreferences(
    ConstStr255Param prefsResourceName, Handle* preferences)
{
    return static_cast<OSErr>(CallUniversalProc(
        (UniversalProcPtr)SBLoadPreferences68K,
        kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ConstStr255Param))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(Handle*))),
        prefsResourceName, preferences));
}

/***************************************

    Save a resource to a preferences file

***************************************/

static const uint16_t SBSavePreferences68K[] = {
    0x554F,         // SUBQ.W #2,A7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x2F2F, 0x000E, // MOVE.L 14(A7),-(a7)
    0x303C, 0x0405, // MOV #$405,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0008  // RTD #8
};

pascal OSErr SBSavePreferences(
    ConstStr255Param prefsResourceName, Handle preferences)
{
    return static_cast<OSErr>(CallUniversalProc(
        (UniversalProcPtr)SBSavePreferences68K,
        kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ConstStr255Param))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(Handle))),
        prefsResourceName, preferences));
}

/***************************************

    Get a string from a detached resource

***************************************/

static const uint16_t SBGetDetachedIndString68K[] = {
    0x2F2F, 0x0004, // MOVE.L 4(A7),-(a7)
    0x2F2F, 0x000C, // MOVE.L 12(A7),-(a7)
    0x3F2F, 0x0014, // MOVE.W 20(A7),-(a7)
    0x303C, 0x0506, // MOV #$506,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x4E75          // RTS
};

pascal void SBGetDetachedIndString(
    StringPtr theString, Handle stringList, short whichString)
{
    CallUniversalProc((UniversalProcPtr)SBGetDetachedIndString68K,
        kPascalStackBased |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(StringPtr))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(Handle))) |
            STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short))),
        theString, stringList, whichString);
}

/***************************************

    Set up a detached icon suite

***************************************/

static const uint16_t SBGetDetachIconSuite68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x3F2F, 0x000E, // MOVE.W 14(A7),-(a7)
    0x2F2F, 0x0012, // MOVE.L 18(A7),-(a7)
    0x303C, 0x0507, // MOV #$507,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x4E74, 0x000C  // RTD #12
};

pascal OSErr SBGetDetachIconSuite(
    Handle* theIconSuite, short theResID, unsigned long selector)
{
    return static_cast<OSErr>(
        CallUniversalProc((UniversalProcPtr)SBGetDetachedIndString68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Handle*))) |
                STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short))) |
                STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(unsigned long))),
            theIconSuite, theResID, selector));
}

/***************************************

    Manager a Pop-Up menu

***************************************/

static const uint16_t SBTrackPopupMenu68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x2F2F, 0x000E, // MOVE.L 14(A7),-(a7)
    0x303C, 0x0408, // MOV #$408,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0008  // RTD #8
};

pascal short SBTrackPopupMenu(const Rect* moduleRect, MenuHandle theMenu)
{
    return static_cast<short>(
        CallUniversalProc((UniversalProcPtr)SBTrackPopupMenu68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(short))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Rect*))) |
                STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(MenuHandle))),
            moduleRect, theMenu));
}

/***************************************

    Display and set an arbitrary parameter

***************************************/

static const uint16_t SBTrackSlider68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x3F2F, 0x000E, // MOVE.W 14(A7),-(a7)
    0x3F2F, 0x0012, // MOVE.W 18(A7),-(a7)
    0x303C, 0x0409, // MOV #$409,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x000C  // RTD #12
};

pascal short SBTrackSlider(
    const Rect* moduleRect, short ticksOnSlider, short initialValue)
{
    return static_cast<short>(
        CallUniversalProc((UniversalProcPtr)SBTrackSlider68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(short))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Rect*))) |
                STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short))) |
                STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short))),
            moduleRect, ticksOnSlider, initialValue));
}

/***************************************

    Display a help balloon

***************************************/

static const uint16_t SBShowHelpString68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x2F2F, 0x000E, // MOVE.L 14(A7),-(a7)
    0x303C, 0x040A, // MOV #$40A,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0008  // RTD #8
};

pascal OSErr SBShowHelpString(const Rect* moduleRect, StringPtr helpString)
{
    return static_cast<OSErr>(
        CallUniversalProc((UniversalProcPtr)SBShowHelpString68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Rect*))) |
                STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(StringPtr))),
            moduleRect, helpString));
}

/***************************************

    Find out the width of a bar graph by SDDrawBarGraph()

***************************************/

static const uint16_t SBGetBarGraphWidth68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x3F2F, 0x0006, // MOVE.W 6(A7),-(a7)
    0x303C, 0x010B, // MOV #$10B,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0004  // RTD #4
};

pascal short SBGetBarGraphWidth(short barCount)
{
    return static_cast<short>(
        CallUniversalProc((UniversalProcPtr)SBGetBarGraphWidth68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(short))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short))),
            barCount));
}

/***************************************

   Draw a bar graph

***************************************/

static const uint16_t SBDrawBarGraph68K[] = {
    0x3F2F, 0x0004, // MOVE.W 4(A7),-(a7)
    0x3F2F, 0x0008, // MOVE.W 8(A7),-(a7)
    0x3F2F, 0x000C, // MOVE.W 12(A7),-(a7)
    0x2F2F, 0x0010, // MOVE.L 16(A7),-(a7)
    0x303C, 0x050C, // MOV #$50C,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x4E74, 0x0010  // RTD #16
};

pascal void SBDrawBarGraph(
    short level, short barCount, short direction, Point barGraphTopLeft)
{
    CallUniversalProc((UniversalProcPtr)SBDrawBarGraph68K,
        kPascalStackBased |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short))) |
            STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short))) |
            STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(Point))),
        level, barCount, direction, barGraphTopLeft);
}

/***************************************

   Keep background applications from running while your modal dialog is visible.

***************************************/

static const uint16_t SBModalDialogInContext68K[] = {
    0x2F2F, 0x0004, // MOVE.L 4(A7),-(a7)
    0x2F2F, 0x000C, // MOVE.L 12(A7),-(a7)
    0x303C, 0x040D, // MOV #$40D,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x4E74, 0x0008  // RTD #8
};

pascal void SBModalDialogInContext(ModalFilterUPP filterProc, short* itemHit)
{
    CallUniversalProc((UniversalProcPtr)SBModalDialogInContext68K,
        kPascalStackBased |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(ModalFilterUPP))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short*))),
        filterProc, itemHit);
}

/***************************************

   Get the Font ID being used by the control strip
   Control Strip 1.2 or later

***************************************/

static const uint16_t SBGetControlStripFontID68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x303C, 0x020E, // MOV #$20E,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0004  // RTD #4
};

pascal OSErr SBGetControlStripFontID(short* fontID)
{
    return static_cast<OSErr>(
        CallUniversalProc((UniversalProcPtr)SBGetControlStripFontID68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short*))),
            fontID));
}

/***************************************

   Set the Font ID for the Control Strip
   Control Strip 1.2 or later

***************************************/

static const uint16_t SBSetControlStripFontID68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x303C, 0x010F, // MOV #$10F,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0004  // RTD #4
};

pascal OSErr SBSetControlStripFontID(short fontID)
{
    return static_cast<OSErr>(
        CallUniversalProc((UniversalProcPtr)SBSetControlStripFontID68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short))),
            fontID));
}

/***************************************

   Get the Font size in points for the Control Strip
   Control Strip 1.2 or later

***************************************/

static const uint16_t SBGetControlStripFontSize68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x303C, 0x0210, // MOV #$210,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0004  // RTD #4
};

pascal OSErr SBGetControlStripFontSize(short* fontSize)
{
    return static_cast<OSErr>(
        CallUniversalProc((UniversalProcPtr)SBGetControlStripFontSize68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short*))),
            fontSize));
}

/***************************************

   Set the Font size in points for the Control Strip
   Control Strip 1.2 or later

***************************************/

static const uint16_t SBSetControlStripFontSize68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x3F2F, 0x0006, // MOVE.W 6(A7),-(a7)
    0x303C, 0x0111, // MOV #$111,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0004  // RTD #4
};

pascal OSErr SBSetControlStripFontSize(short fontSize)
{
    return static_cast<OSErr>(
        CallUniversalProc((UniversalProcPtr)SBSetControlStripFontSize68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short))),
            fontSize));
}

/***************************************

   Get the show/hide hot key for the Control Strip
   Control Strip 1.2 or later

***************************************/

static const uint16_t SBGetShowHideHotKey68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x2F2F, 0x000E, // MOVE.L 14(A7),-(a7)
    0x303C, 0x0412, // MOV #$412,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0008  // RTD #8
};

pascal OSErr SBGetShowHideHotKey(short* modifiers, unsigned char* keyCode)
{
    return static_cast<OSErr>(
        CallUniversalProc((UniversalProcPtr)SBGetShowHideHotKey68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short*))) |
                STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(unsigned char*))),
            modifiers, keyCode));
}

/***************************************

   Set the show/hide hot key for the Control Strip
   Control Strip 1.2 or later

***************************************/

static const uint16_t SBSetShowHideHotKey68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x3F2F, 0x0006, // MOVE.W 6(A7),-(a7)
    0x1F2F, 0x000A, // MOVE.B 10(A7),-(a7)
    0x303C, 0x0213, // MOV #$213,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0008  // RTD #8
};

pascal OSErr SBSetShowHideHotKey(short modifiers, unsigned char keyCode)
{
    return static_cast<OSErr>(
        CallUniversalProc((UniversalProcPtr)SBSetShowHideHotKey68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(short))) |
                STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(unsigned char))),
            modifiers, keyCode));
}

/***************************************

   Is the show/hide hot key enabled?
   Control Strip 1.2 or later

***************************************/

static const uint16_t SBIsShowHideHotKeyEnabled68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x2F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x303C, 0x0214, // MOV #$214,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0004  // RTD #4
};

pascal OSErr SBIsShowHideHotKeyEnabled(Boolean* enabled)
{
    return static_cast<OSErr>(
        CallUniversalProc((UniversalProcPtr)SBIsShowHideHotKeyEnabled68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Boolean*))),
            enabled));
}

/***************************************

   Enable/disable the show/hide hot key.
   Control Strip 1.2 or later

***************************************/

static const uint16_t SBEnableShowHideHotKey68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x1F2F, 0x0006, // MOVE.B 6(A7),-(a7)
    0x303C, 0x0115, // MOV #$115,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0004  // RTD #4
};

pascal OSErr SBEnableShowHideHotKey(Boolean enabled)
{
    return static_cast<OSErr>(
        CallUniversalProc((UniversalProcPtr)SBEnableShowHideHotKey68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Boolean))),
            enabled));
}

/***************************************

   Pass a slider press event to Control Strip
   Control Strip 1.4 or later

***************************************/

static const uint16_t SBHitTrackSlider68K[] = {
    0x554F,         // SUBQ.W #2,a7
    0x1F2F, 0x0006, // MOVE.L 6(A7),-(a7)
    0x1F2F, 0x000E, // MOVE.W 14(A7),-(a7)
    0x1F2F, 0x0012, // MOVE.W 18(A7),-(a7)
    0x1F2F, 0x0016, // MOVE.L 22(A7),-(a7)
    0x303C, 0x0616, // MOV #$616,D0
    0xAAF2,         // DC.W $AAF2 ; TB 02F2
    0x301F,         // MOVE.W (A7)+,D0
    0x4E74, 0x0010  // RTD #16
};

pascal short SBHitTrackSlider(const Rect* moduleRect, short ticksOnSlider,
    short initialValue, Boolean* hit)
{
    return static_cast<short>(
        CallUniversalProc((UniversalProcPtr)SBHitTrackSlider68K,
            kPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(short))) |
                STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(Rect*))) |
                STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(short))) |
                STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short))) |
                STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(Boolean*))),
            moduleRect, ticksOnSlider, initialValue, hit));
}

#endif
