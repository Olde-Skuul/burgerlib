/***************************************

    CarbonAccessors.o compatible library

    This replaces Apple's CarbonAccessors.o file so profiling, debugging and
    other compiler features that are available with source level debugging
    become available to an application.

    This code is intended only for MacOS Classic targeted applications.

    Copyright (c) 2002-2008 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brtypes.h"

// Only build on PowerPC MacOS Classic targets.

#if defined(BURGER_MACCLASSIC) && !defined(DOXYGEN)

#include "brmactypes.h"
#include <Quickdraw.h>

// Quickdraw Error Low level address
#define QDErrLM reinterpret_cast<int16_t*>(0xD6E)

//
// Below is some of Apple's Secret Sauce.
//

/***************************************

    Set quickdraw error

***************************************/

void SetQDError(int16_t err)
{
    QDErrLM[0] = err;
}

/***************************************

    Get the port pix map

***************************************/

PixMapHandle GetPortPixMap(CGrafPtr port)
{
    // Get the current port if not already set
    if (!port) {
        GrafPtr TempPort;
        GetPort(&TempPort);
        port = (CGrafPtr)TempPort;
    }

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

const BitMap* GetPortBitMapForCopyBits(CGrafPtr port)
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

/***************************************

    Get the port's visible region

***************************************/

RgnHandle GetPortVisibleRegion(CGrafPtr port, RgnHandle visRgn)
{
    if (visRgn) {
        // Get the current port if not already set
        if (!port) {
            GrafPtr TempPort;
            GetPort(&TempPort);
            port = (CGrafPtr)TempPort;
        }
        if (port) {
            CopyRgn(port->visRgn, visRgn);
            return visRgn;
        }
    }
    SetQDError(paramErr);
    return visRgn;
}

#endif
