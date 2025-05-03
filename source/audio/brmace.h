/***************************************

    MACE decompresser

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMACE_H__
#define __BRMACE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDECOMPRESS_H__
#include "brdecompress.h"
#endif

#ifndef __BRSOUNDDECOMPRESS_H__
#include "brsounddecompress.h"
#endif

/* BEGIN */
namespace Burger {
struct MACEState_t {
	int32_t m_iSample1;		///< Last running samples
	int32_t m_iSample2;		///< Second temp sample
	int32_t m_iLastStep;		///< Mask with 0x8000 for + or - direction
	int32_t m_iTableIndex;	///< Index to the slope table
	int32_t m_iLastAmplitude;	///< m_iLastSlope * m_iLastStep
	int32_t m_iLastSlope;		///< Last Slope value
	static const int16_t g_Table4Small[4];	///< Base coefficients for 2 bit samples
	static const int16_t g_Table4Big[512];	///< Log table for 2 bit samples
	static const int16_t g_Table8Small[8];	///< Base coefficients for 3 bit samples
	static const int16_t g_Table8Big[1024];	///< Log table for 3 bit samples
	void BURGER_API Clear(void);
	int32_t BURGER_API CoefficientStep(uint_t uTableOffset,uint_t bUse3Bits);
};
extern void BURGER_API MACEExp1to6(const uint8_t *pInput,uint8_t *pOutput,uintptr_t uPacketCount,MACEState_t *pInputState,MACEState_t *pOutputState,uint_t uNumChannels,uint_t uWhichChannel);
extern void BURGER_API MACEExp1to3(const uint8_t *pInput,uint8_t *pOutput,uintptr_t uPacketCount,MACEState_t *pInputState,MACEState_t *pOutputState,uint_t uNumChannels,uint_t uWhichChannel);

class DecompressMace3 : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
protected:
	enum eState {
		STATE_INIT,				///< Basic state
		STATE_FILLINGCACHE,		///< Waiting for input data
		STATE_PROCESSCACHE,		///< Decompress cache data
		STATE_CACHEFULL			///< Cache is full, need to output before more input
	};
	MACEState_t MaceStateLeft;		///< Decompression states for left/right channels
	MACEState_t MaceStateRight;		///< Decompression states for left/right channels
	uint_t m_uCacheSize;				///< Number of valid bytes in m_Cache
	eState m_eState;				///< State of the decompression
	uint8_t m_Cache[6*2];				///< Temp compressed data buffer
public:
	static const uint32_t Signature = 0x4D414333;		///< 'MAC3'
	DecompressMace3();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput, uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
	static DecompressMace3 * BURGER_API new_object(void);
};

class DecompressMace6 : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
protected:
	enum eState {
		STATE_INIT,				///< Basic state
		STATE_FILLINGCACHE,		///< Waiting for input data
		STATE_PROCESSCACHE,		///< Decompress cache data
		STATE_CACHEFULL			///< Cache is full, need to output before more input
	};
	MACEState_t MaceStateLeft;		///< Decompression states for left/right channels
	MACEState_t MaceStateRight;		///< Decompression states for left/right channels
	uint_t m_uCacheSize;				///< Number of valid bytes in m_Cache
	eState m_eState;				///< State of the decompression
	uint8_t m_Cache[6*2];				///< Temp compressed data buffer
public:
	static const uint32_t Signature = 0x4D414336;		///< 'MAC6'
	DecompressMace6();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput, uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
	static DecompressMace6 * BURGER_API new_object(void);
};

}
/* END */

#endif
