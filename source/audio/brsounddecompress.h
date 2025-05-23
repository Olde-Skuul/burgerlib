/***************************************

    Sound decompression classes

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRSOUNDDECOMPRESS_H__
#define __BRSOUNDDECOMPRESS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDECOMPRESS_H__
#include "brdecompress.h"
#endif

#ifndef __BRSOUND_H__
#include "brsound.h"
#endif

/* BEGIN */
namespace Burger {
class DecompressAudio : public Decompress {
	BURGER_RTTI_IN_CLASS();
protected:
	SoundManager::eDataType m_eDataType;	///< Data type the decompresser will create
	uint_t m_bStereo;							///< \ref TRUE if data is stereo
	DecompressAudio(SoundManager::eDataType uDataType);
public:
	BURGER_INLINE SoundManager::eDataType GetDataType(void) const { return m_eDataType; }
	BURGER_INLINE void SetMono(void) { m_bStereo = FALSE; }
	BURGER_INLINE void SetStereo(void) { m_bStereo = TRUE; }
	BURGER_INLINE uint_t IsStereo(void) const { return m_bStereo; }
};

class DecompressUnsigned8BitAudio : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
public:
	static const uint32_t Signature = 0x41595445;		///< 'BYTE'
	DecompressUnsigned8BitAudio();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput, uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
	static DecompressUnsigned8BitAudio * BURGER_API new_object(void);
};

class DecompressSigned8BitAudio : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
public:
	static const uint32_t Signature = 0x43484152;		///< 'CHAR'
	DecompressSigned8BitAudio();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput, uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
	static DecompressSigned8BitAudio * BURGER_API new_object(void);
};

class Decompress16BitBEAudio : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
#if !defined(BURGER_BIGENDIAN)
protected:
	enum eState {
		STATE_INIT,			///< Basic state
		STATE_FILLINGCACHE,	///< Waiting for input data
		STATE_CACHEFULL		///< Cache is full, need to output before more input
	};
	uint_t m_uCacheSize;		///< Number of bytes in the cache
	eState m_eState;		///< Cache state
	uint8_t m_Cache[2];		///< Cache for data being swapped
#endif
public:
	static const uint32_t Signature = 0x42453136;		///< 'BE16'
	Decompress16BitBEAudio();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput, uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
	static Decompress16BitBEAudio * BURGER_API new_object(void);
};

class Decompress16BitLEAudio : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
#if defined(BURGER_BIGENDIAN)
protected:
	enum eState {
		STATE_INIT,			///< Basic state
		STATE_FILLINGCACHE,	///< Waiting for input data
		STATE_CACHEFULL		///< Cache is full, need to output before more input
	};
	uint_t m_uCacheSize;		///< Number of bytes in the cache
	eState m_eState;		///< Cache state
	uint8_t m_Cache[2];		///< Cache for data being swapped
#endif
public:
	static const uint32_t Signature = 0x4C453136;		///< 'LE16'
	Decompress16BitLEAudio();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput, uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
	static Decompress16BitLEAudio * BURGER_API new_object(void);
};

class Decompress32BitBEAudio : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
#if !defined(BURGER_BIGENDIAN)
protected:
	enum eState {
		STATE_INIT,			///< Basic state
		STATE_FILLINGCACHE,	///< Waiting for input data
		STATE_CACHEFULL		///< Cache is full, need to output before more input
	};
	uint_t m_uCacheCount;		///< Number of valid bytes in m_Cache
	eState m_eState;		///< Cache state
	uint8_t m_Cache[4];		///< Cache for data being swapped
#endif
public:
	static const uint32_t Signature = 0x42453332;		///< 'BE32'
	Decompress32BitBEAudio();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput, uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
	static Decompress32BitBEAudio * BURGER_API new_object(void);
};

class Decompress32BitLEAudio : public DecompressAudio {
	BURGER_RTTI_IN_CLASS();
#if defined(BURGER_BIGENDIAN)
protected:
	enum eState {
		STATE_INIT,			///< Basic state
		STATE_FILLINGCACHE,	///< Waiting for input data
		STATE_CACHEFULL		///< Cache is full, need to output before more input
	};
	uint_t m_uCacheCount;		///< Number of valid bytes in m_Cache
	eState m_eState;		///< Cache state
	uint8_t m_Cache[4];		///< Cache for data being swapped
#endif
public:
	static const uint32_t Signature = 0x4C453332;		///< 'LE32'
	Decompress32BitLEAudio();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput, uintptr_t uOutputChunkSize,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
	static Decompress32BitLEAudio * BURGER_API new_object(void);
};
}
/* END */

#endif
