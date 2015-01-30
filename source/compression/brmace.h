/***************************************

	MACE decompressor

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACE_H__
#define __BRMACE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
struct MACEState_t {
	Int32 m_iSample1;		///< Last running samples
	Int32 m_iSample2;		///< Second temp sample
	Int32 m_iLastStep;		///< Mask with 0x8000 for + or - direction
	Int32 m_iTableIndex;	///< Which slope table
	Int32 m_iLastAmplitude;	///< m_iLastSlope * m_iLastStep
	Int32 m_iLastSlope;		///< Last Slope value
};
extern void BURGER_API MACEExp1to6(const Word8 *pInput,Word8 *pOutput,WordPtr uPacketCount,MACEState_t *pInputState,MACEState_t *pOutputState,Word uNumChannels,Word uWhichChannel);
extern void BURGER_API MACEExp1to3(const Word8 *pInput,Word8 *pOutput,WordPtr uPacketCount,MACEState_t *pInputState,MACEState_t *pOutputState,Word uNumChannels,Word uWhichChannel);
}
/* END */

#endif
