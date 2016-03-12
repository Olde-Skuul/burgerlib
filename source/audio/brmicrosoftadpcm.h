/***************************************

	Microsoft ADPCM decompresser

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRMICROSOFTADPCM_H__
#define __BRMICROSOFTADPCM_H__

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
struct ADPCMState_t {
	Int32 m_iFirstSample;	///< First audio sample
	Int32 m_iSecondSample;	///< Second audio sample
	Int32 m_iCoef1;			///< Coefficient 1 constant
	Int32 m_iCoef2;			///< Coefficient 2 constant
	Int32 m_iIndex;			///< Index into step size table
	Int32 BURGER_API Decode(Word uDeltaCode);
};
class DecompressMicrosoftADPCM : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
protected:
	enum eState {
		STATE_INIT,				///< Initial state
		STATE_INITMONO,			///< Start mono decoding
		STATE_GETDECODERMONO,	///< Process the mono decoder
		STATE_WRITESAMPLESMONO,	///< Set up to write out the samples from 4 bits to 16
		STATE_WRITINGSAMPLESMONO,	///< Write out the samples from 4 bits to 16
		STATE_INITSTEREO,		///< Start stereo decoding
		STATE_GETDECODERSTEREO,	///< Process the stereo decoder
		STATE_WRITESAMPLESSTEREO,	///< Set up to write out the samples from 4 bits to 16 in stereo
		STATE_WRITINGSAMPLESSTEREO,	///< Write out the samples from 4 bits to 16 in stereo
		STATE_FILLINGCACHE,		///< Input starved, waiting for more input
		STATE_CACHEFULL			///< Output is pending, waiting for an output buffer
	};
	ADPCMState_t m_Decoders[2];	///< Decoders for left and right channels
	Word m_uCacheCount;			///< Number of valid bytes in m_Cache
	Word m_uCacheSize;			///< Maximum size of byte of m_Cache
	Word m_uBlockSize;			///< Size of each compressed block (wBlockAlign)
	Word m_uSamplesPerBlock;	///< Number of samples to decompress per block (wSamplesPerBlock)
	Word m_uSamplesRemaining;	///< Number of samples left to decompress from a block in progress
	eState m_eState;			///< State of the decompression
	eState m_eNextState;		///< Pending state of the decompression after a cache fill/flush is performed
	Word8 m_Cache[32];			///< Data cache
public:
	static const Word32 Signature = 0x4D50434D;		///< 'MPCM'
	DecompressMicrosoftADPCM();
	virtual eError Reset(void);
	virtual eError Process(void *pOutput,WordPtr uOutputChunkSize,const void *pInput,WordPtr uInputChunkLength);
	static DecompressMicrosoftADPCM * BURGER_API New(void);
	BURGER_INLINE void SetBlockSize(Word uBlockSize) { m_uBlockSize = uBlockSize; }
	BURGER_INLINE void SetSamplesPerBlock(Word uSamplesPerBlock) { m_uSamplesPerBlock = uSamplesPerBlock; }
	void BURGER_API SetMonoDecoder(const void *pInput);
	void BURGER_API SetStereoDecoder(const void *pInput);
	Word BURGER_API ADPCMDecodeBlock(Int16 *pOutput,const Word8 *pInput,WordPtr uInputLength);
};
}

/* END */

#endif
