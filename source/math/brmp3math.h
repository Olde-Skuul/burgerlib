/***************************************

    Math functions for MP3 support

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMP3MATH_H__
#define __BRMP3MATH_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFLOATINGPOINT_H__
#include "brfloatingpoint.h"
#endif

/* BEGIN */
namespace Burger {
extern const uint32_float_t *g_pMP3CosinePoints[5];
extern const uint32_float_t g_MP3FHTCosines[4][2];
extern const uint32_float_t g_MP3DCT36Constants[18];
extern const uint32_float_t g_MP3DCT12Constants[3];
extern void BURGER_API MP3DCT64(float *pOutput1,float *pOutput2,const float *pInput);
extern void BURGER_API MP3FHT(float *pInput,WordPtr uCount);
extern void BURGER_API MP3DCT36(float *pSideSamples,float *pHybridOutput,float *pOutput,const float *pPrevious,const float *pBlockType);
extern void BURGER_API MP3DCT12(float *pSideSamples,float *pHybridOutput,float *pOutput,const float *pPrevious,const float *pBlockType);
}
/* END */

#endif
