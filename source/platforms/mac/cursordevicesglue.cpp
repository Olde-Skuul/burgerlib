/***************************************

    MacOS ONLY!!!!

    This code existed in CursorDevicesGlue.o
    Now it's here so that I can profile and debug it. Also the MRC compiler is
    much better than the .o file. Added support so this can be called from
    carbon apps

    Copyright (c) 1999-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtypes.h"

#if defined(BURGER_POWERPC) && defined(BURGER_MAC) && !defined(DOXYGEN)

#include <CursorDevices.h>
#include <Gestalt.h>
#include <MacErrors.h>
#include <Patches.h>
#include <Traps.h>

// Since Carbon doesn't define these in the headers,
// I have to define them here

#if defined(BURGER_MACCARBON)
#include "brcodelibrary.h"
#include "brglobals.h"

enum { kOSTrapType = 0, kToolboxTrapType = 1 };
typedef SignedByte TrapType;
#define F(x) (*x)     // Function pointer
#define E             // Remove the extern keyword
#define REF pLocals-> // Convert a call to a proc pointer
#else

// Classic, allows these functions

#define F(x) x   // Use the function as is
#define E extern // Allow the extern keyword
#define REF      // Call the proc directly
#endif

// ANSI "C" please...

#ifdef __cplusplus
extern "C" {
#endif

struct Locals_t {
    OSErr m_uFragError; // If TRUE then it's ok to use the ROM
    uint8_t m_bTested;  // If TRUE then Gestalt was called
    uint8_t m_bPadding; // Longword align
#if !defined(BURGER_MACCARBON)
}; // End here for Classic
#endif

// These are the prototypes to the code in ROM
// Or they are proc pointers in Carbon

E pascal OSErr F(CrsrDevMove)(
    CursorDevicePtr ourDevice, long deltaX, long deltaY);
E pascal OSErr F(CrsrDevMoveTo)(
    CursorDevicePtr ourDevice, long absX, long absY);
E pascal OSErr F(CrsrDevFlush)(CursorDevicePtr ourDevice);
E pascal OSErr F(CrsrDevButtons)(CursorDevicePtr ourDevice, short buttons);
E pascal OSErr F(CrsrDevButtonDown)(CursorDevicePtr ourDevice);
E pascal OSErr F(CrsrDevButtonUp)(CursorDevicePtr ourDevice);
E pascal OSErr F(CrsrDevButtonOp)(CursorDevicePtr ourDevice, short buttonNumber,
    ButtonOpcode opcode, long data);
E pascal OSErr F(CrsrDevSetButtons)(
    CursorDevicePtr ourDevice, short numberOfButtons);
E pascal OSErr F(CrsrDevSetAcceleration)(
    CursorDevicePtr ourDevice, Fixed acceleration);
E pascal OSErr F(CrsrDevDoubleTime)(
    CursorDevicePtr ourDevice, long durationTicks);
E pascal OSErr F(CrsrDevUnitsPerInch)(
    CursorDevicePtr ourDevice, Fixed resolution);
E pascal OSErr F(CrsrDevNextDevice)(CursorDevicePtr* ourDevice);
E pascal OSErr F(CrsrDevNewDevice)(CursorDevicePtr* ourDevice);
E pascal OSErr F(CrsrDevDisposeDevice)(CursorDevicePtr ourDevice);
#if defined(BURGER_MACCARBON)
pascal long (*CallUniversalProc)(
    UniversalProcPtr theProcPtr, ProcInfoType procInfo, ...);
pascal UniversalProcPtr (*NGetTrapAddress)(UInt16 trapNum, TrapType tTyp);
}; // End here for Carbon

// These are the MacOS prototypes
// Included here since Carbon makes them go away

extern pascal OSErr CursorDeviceMove(
    CursorDevicePtr ourDevice, long deltaX, long deltaY);
extern pascal OSErr CursorDeviceMoveTo(
    CursorDevicePtr ourDevice, long absX, long absY);
extern pascal OSErr CursorDeviceFlush(CursorDevicePtr ourDevice);
extern pascal OSErr CursorDeviceButtons(
    CursorDevicePtr ourDevice, short buttons);
extern pascal OSErr CursorDeviceButtonDown(CursorDevicePtr ourDevice);
extern pascal OSErr CursorDeviceButtonUp(CursorDevicePtr ourDevice);
extern pascal OSErr CursorDeviceButtonOp(CursorDevicePtr ourDevice,
    short buttonNumber, ButtonOpcode opcode, long data);
extern pascal OSErr CursorDeviceSetButtons(
    CursorDevicePtr ourDevice, short numberOfButtons);
extern pascal OSErr CursorDeviceSetAcceleration(
    CursorDevicePtr ourDevice, Fixed acceleration);
extern pascal OSErr CursorDeviceDoubleTime(
    CursorDevicePtr ourDevice, long durationTicks);
extern pascal OSErr CursorDeviceUnitsPerInch(
    CursorDevicePtr ourDevice, Fixed resolution);
extern pascal OSErr CursorDeviceNextDevice(CursorDevicePtr* ourDevice);
extern pascal OSErr CursorDeviceNewDevice(CursorDevicePtr* ourDevice);
extern pascal OSErr CursorDeviceDisposeDevice(CursorDevicePtr ourDevice);
#endif

#ifdef __cplusplus
}
#endif

// Clean up (But don't kill REF)
#undef E
#undef F

/***************************************

    This code isn't called much, so don't inline anything to conserve space

***************************************/

#if defined(BURGER_MRC)
#if defined(BURGER_MACCARBON)
#pragma noinline_func LoadFrags
#endif

#pragma noinline_func CheckCursorCalls, GetCursorTrap

#elif defined(BURGER_METROWERKS)
#pragma dont_inline on
#endif

// Data for me
static Locals_t Locals;

/***************************************

    For Carbon, I need to link in InterfaceLib manually to be able to use the
    Cursor manager from a Carbon app in Classic.

    Note: This will fail on MacOSX. Hence the paranoid code checking.

***************************************/

#if defined(BURGER_MACCARBON)
static void LoadFrags(Locals_t* pLocals)
{
    Burger::CodeLibrary* pInterfaceLib = Burger::GetInterfaceLib();
    // Will succeed in classic
    if (pInterfaceLib) {
        // I Will call 680x0 code, sigh
        if (!pLocals->m_uFragError) {
            REF CallUniversalProc =
                (pascal long (*)(UniversalProcPtr, ProcInfoType,
                    ...))pInterfaceLib->get_function("CallUniversalProc");
            REF NGetTrapAddress = (pascal UniversalProcPtr(*)(
                UInt16, TrapType))pInterfaceLib->get_function("NGetTrapAddress");
            if (REF CallUniversalProc && REF NGetTrapAddress) {
                return;
            }
        } else {

            // Hooray, I'll call native PowerPC code

            REF CrsrDevMove = (pascal OSErr(*)(CursorDevice*, long,
                long))pInterfaceLib->get_function("CrsrDevMove");
            REF CrsrDevMoveTo = (pascal OSErr(*)(CursorDevice*, long,
                long))pInterfaceLib->get_function("CrsrDevMoveTo");
            REF CrsrDevFlush = (pascal OSErr(*)(
                CursorDevice*))pInterfaceLib->get_function("CrsrDevFlush");
            REF CrsrDevButtons = (pascal OSErr(*)(CursorDevice*,
                short))pInterfaceLib->get_function("CrsrDevButtons");
            REF CrsrDevButtonDown = (pascal OSErr(*)(
                CursorDevice*))pInterfaceLib->get_function("CrsrDevButtonDown");
            REF CrsrDevButtonUp = (pascal OSErr(*)(
                CursorDevice*))pInterfaceLib->get_function("CrsrDevButtonUp");
            REF CrsrDevButtonOp = (pascal OSErr(*)(CursorDevice*, short, short,
                long))pInterfaceLib->get_function("CrsrDevButtonOp");
            REF CrsrDevSetButtons = (pascal OSErr(*)(CursorDevice*,
                short))pInterfaceLib->get_function("CrsrDevSetButtons");
            REF CrsrDevSetAcceleration = (pascal OSErr(*)(CursorDevice*,
                long))pInterfaceLib->get_function("CrsrDevSetAcceleration");
            REF CrsrDevDoubleTime = (pascal OSErr(*)(CursorDevice*,
                long))pInterfaceLib->get_function("CrsrDevDoubleTime");
            REF CrsrDevUnitsPerInch = (pascal OSErr(*)(CursorDevice*,
                long))pInterfaceLib->get_function("CrsrDevUnitsPerInch");
            REF CrsrDevNextDevice = (pascal OSErr(*)(
                CursorDevice**))pInterfaceLib->get_function("CrsrDevNextDevice");
            REF CrsrDevNewDevice = (pascal OSErr(*)(
                CursorDevice**))pInterfaceLib->get_function("CrsrDevNewDevice");
            REF CrsrDevDisposeDevice =
                (pascal OSErr(*)(CursorDevice*))pInterfaceLib->get_function(
                    "CrsrDevDisposeDevice");
            if (REF CrsrDevMove && REF CrsrDevMoveTo && REF CrsrDevFlush &&
                REF CrsrDevButtons && REF CrsrDevButtonDown &&
                REF CrsrDevButtonUp && REF CrsrDevButtonOp &&
                REF CrsrDevSetButtons && REF CrsrDevSetAcceleration &&
                REF CrsrDevDoubleTime && REF CrsrDevUnitsPerInch &&
                REF CrsrDevNextDevice && REF CrsrDevNewDevice &&
                REF CrsrDevDisposeDevice) {
                return;
            }
        }
    }
    // Failure
    pLocals->m_uFragError = cfragNoSymbolErr;
}
#endif

/***************************************

    This is a local routine

    Return TRUE if the CDM code is not fixed

***************************************/

static OSErr CheckCursorCalls(Locals_t* pLocals)
{
    // Was it tested?
    if (!pLocals->m_bTested) {
        // Assume ok
        pLocals->m_uFragError = FALSE;
        long gestaltAnswer;
        // Get the bug check
        if (!Gestalt('bugx', &gestaltAnswer)) {
            if (gestaltAnswer & 0x100000) {
                // The fix is present
                pLocals->m_uFragError = TRUE;
            }
        }
#if defined(BURGER_MACCARBON)
        // Load in the procs (Could fail)
        LoadFrags(pLocals);
#endif
        // Don't call gestalt again
        pLocals->m_bTested = TRUE;
    }
    // Return the result of gestalt
    return pLocals->m_uFragError;
}

/***************************************

    Get the trap address

***************************************/

static UniversalProcPtr GetCursorTrap(const Locals_t* pLocals)
{
    BURGER_UNUSED(pLocals);
    return REF NGetTrapAddress(_CursorDeviceDispatch, kToolboxTrapType);
}

/***************************************

    Call CursorDeviceMove

***************************************/

pascal OSErr CursorDeviceMove(
    CursorDevicePtr ourDevice, long deltaX, long deltaY)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevMove(ourDevice, deltaX, deltaY);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))) |
            STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(long))),
        0, ourDevice, deltaX, deltaY));
}

/***************************************

    Call CursorDeviceMoveTo

***************************************/

pascal OSErr CursorDeviceMoveTo(CursorDevicePtr ourDevice, long absX, long absY)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevMoveTo(ourDevice, absX, absY);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))) |
            STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(long))),
        1, ourDevice, absX, absY));
}

/***************************************

    Call CursorDeviceFlush

***************************************/

pascal OSErr CursorDeviceFlush(CursorDevicePtr ourDevice)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevFlush(ourDevice);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))),
        2, ourDevice));
}

/***************************************

    Call CursorDeviceButtons

***************************************/

pascal OSErr CursorDeviceButtons(CursorDevicePtr ourDevice, short buttons)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevButtons(ourDevice, buttons);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))) |
            STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short))),
        3, ourDevice, buttons));
}

/***************************************

    Call CursorDeviceButtonDown

***************************************/

pascal OSErr CursorDeviceButtonDown(CursorDevicePtr ourDevice)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevButtonDown(ourDevice);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))),
        4, ourDevice));
}

/***************************************

    Call CursorDeviceButtonUp

***************************************/

pascal OSErr CursorDeviceButtonUp(CursorDevicePtr ourDevice)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevButtonUp(ourDevice);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))),
        5, ourDevice));
}

/***************************************

    Call CursorDeviceButtonOp

***************************************/

pascal OSErr CursorDeviceButtonOp(CursorDevicePtr ourDevice, short buttonNumber,
    ButtonOpcode opcode, long data)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevButtonOp(ourDevice, buttonNumber, opcode, data);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))) |
            STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short))) |
            STACK_ROUTINE_PARAMETER(4, SIZE_CODE(sizeof(ButtonOpcode))) |
            STACK_ROUTINE_PARAMETER(5, SIZE_CODE(sizeof(long))),
        6, ourDevice, buttonNumber, opcode, data));
}

/***************************************

    Call CursorDeviceSetButtons

***************************************/

pascal OSErr CursorDeviceSetButtons(
    CursorDevicePtr ourDevice, short numberOfButtons)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevSetButtons(ourDevice, numberOfButtons);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))) |
            STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(short))),
        7, ourDevice, numberOfButtons));
}

/***************************************

    Call CursorDeviceSetAcceleration

***************************************/

pascal OSErr CursorDeviceSetAcceleration(
    CursorDevicePtr ourDevice, Fixed acceleration)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevSetAcceleration(ourDevice, acceleration);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))) |
            STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Fixed))),
        8, ourDevice, acceleration));
}

/***************************************

    Call CursorDeviceDoubleTime

***************************************/

pascal OSErr CursorDeviceDoubleTime(
    CursorDevicePtr ourDevice, long durationTicks)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevDoubleTime(ourDevice, durationTicks);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))) |
            STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(long))),
        9, ourDevice, durationTicks));
}

/***************************************

    Call CursorDeviceUnitsPerInch

***************************************/

pascal OSErr CursorDeviceUnitsPerInch(
    CursorDevicePtr ourDevice, Fixed resolution)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevUnitsPerInch(ourDevice, resolution);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))) |
            STACK_ROUTINE_PARAMETER(3, SIZE_CODE(sizeof(Fixed))),
        10, ourDevice, resolution));
}

/***************************************

    Call CursorDeviceNextDevice

***************************************/

pascal OSErr CursorDeviceNextDevice(CursorDevicePtr* ourDevice)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevNextDevice(ourDevice);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr*))),
        11, ourDevice));
}

/***************************************

    Call CursorDeviceNewDevice

***************************************/

pascal OSErr CursorDeviceNewDevice(CursorDevicePtr* ourDevice)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevNewDevice(ourDevice);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr*))),
        12, ourDevice));
}

/***************************************

    Call CursorDeviceDisposeDevice

***************************************/

pascal OSErr CursorDeviceDisposeDevice(CursorDevicePtr ourDevice)
{
    Locals_t* pLocals = &Locals;
    OSErr uOSErr = CheckCursorCalls(pLocals);
#if defined(BURGER_MACCARBON)
    if (uOSErr < 0) {
        return uOSErr;
    }
#endif
    if (uOSErr) {
        return REF CrsrDevDisposeDevice(ourDevice);
    }
    return static_cast<OSErr>(REF CallUniversalProc(GetCursorTrap(pLocals),
        kD0DispatchedPascalStackBased | RESULT_SIZE(SIZE_CODE(sizeof(OSErr))) |
            STACK_ROUTINE_PARAMETER(1, SIZE_CODE(sizeof(long))) |
            STACK_ROUTINE_PARAMETER(2, SIZE_CODE(sizeof(CursorDevicePtr))),
        13, ourDevice));
}

#endif
