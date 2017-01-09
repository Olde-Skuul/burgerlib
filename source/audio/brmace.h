/***************************************

	MACE decompresser

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRDECOMPRESS_H__
#include "brdecompress.h"
#endif

#ifndef __BRSOUNDDECOMPRESS_H__
#include "brsounddecompress.h"
#endif

/* BEGIN */
namespace Burger {
struct MACEState_t {
	Int32 m_iSample1;		///< Last running samples
	Int32 m_iSample2;		///< Second temp sample
	Int32 m_iLastStep;		///< Mask with 0x8000 for + or - direction
	Int32 m_iTableIndex;	///< Index to the slope table
	Int32 m_iLastAmplitude;	///< m_iLastSlope * m_iLastStep
	Int32 m_iLastSlope;		///< Last Slope value
	static const Int16 g_Table4Small[4];	///< Base coefficients for 2 bit samples
	static const Int16 g_Table4Big[512];	///< Log table for 2 bit samples
	static const Int16 g_Table8Small[8];	///< Base coefficients for 3 bit samples
	static const Int16 g_Table8Big[1024];	///< Log table for 3 bit samples
	void BURGER_API Clear(void);
	Int32 BURGER_API CoefficientStep(Word uTableOffset,Word bUse3Bits);
};
extern void BURGER_API MACEExp1to6(const Word8 *pInput,Word8 *pOutput,WordPtr uPacketCount,MACEState_t *pInputState,MACEState_t *pOutputState,Word uNumChannels,Word uWhichChannel);
extern void BURGER_API MACEExp1to3(const Word8 *pInput,Word8 *pOutput,WordPtr uPacketCount,MACEState_t *pInputState,MACEState_t *pOutputState,Word uNumChannels,Word uWhichChannel);

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
	Word m_uCacheSize;				///< Number of valid bytes in m_Cache
	eState m_eState;				///< State of the decompression
	Word8 m_Cache[6*2];				///< Temp compressed data buffer
public:
	static const Word32 Signature = 0x4D414333;		///< 'MAC3'
	DecompressMace3();
	virtual eError Reset(void);
	virtual eError Process(void *pOutput,WordPtr uOutputChunkSize,const void *pInput,WordPtr uInputChunkLength);
	static DecompressMace3 * BURGER_API New(void);
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
	Word m_uCacheSize;				///< Number of valid bytes in m_Cache
	eState m_eState;				///< State of the decompression
	Word8 m_Cache[6*2];				///< Temp compressed data buffer
public:
	static const Word32 Signature = 0x4D414336;		///< 'MAC6'
	DecompressMace6();
	virtual eError Reset(void);
	virtual eError Process(void *pOutput,WordPtr uOutputChunkSize,const void *pInput,WordPtr uInputChunkLength);
	static DecompressMace6 * BURGER_API New(void);
};

}
/* END */

#endif
