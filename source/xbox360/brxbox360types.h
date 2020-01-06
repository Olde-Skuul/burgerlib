/***************************************

    Typedefs specific to the Xbox 360

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRXBOX360TYPES_H__
#define __BRXBOX360TYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRVISUALSTUDIO_H__
#include "brvisualstudio.h"
#endif

/* BEGIN */

#if defined(BURGER_XBOX360) && !defined(DOXYGEN)

struct Direct3D;
struct D3DDevice;
struct D3DStateBlock;
struct D3DVertexDeclaration;
struct D3DVertexShader;
struct D3DPixelShader;
struct D3DResource;
struct D3DBaseTexture;
struct D3DTexture;
struct D3DVolumeTexture;
struct D3DCubeTexture;
struct D3DArrayTexture;
struct D3DLineTexture;
struct D3DVertexBuffer;
struct D3DIndexBuffer;
struct D3DSurface;
struct D3DVolume;
struct D3DQuery;
struct D3DPerfCounters;
struct D3DPerfCounterBatch;
struct D3DConstantBuffer;
struct D3DCommandBuffer;
struct D3DAsyncCommandBufferCall;
struct D3DOcclusionQueryBatch;
struct _FILETIME;
struct _SYSTEMTIME;
struct _GUID;
struct tagPOINT;
struct tagRECT;

struct BurgerCRITICAL_SECTION {
    union {
        uint32_t RawEvent[4];
    } Synchronization;
    uint32_t LockCount;
    uint32_t RecursionCount;
    void* OwningThread;
};

#endif
/* END */

#endif
