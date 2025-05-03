/***************************************

    GIF File handler class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEGIF_H__
#define __BRFILEGIF_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRIMAGE_H__
#include "brimage.h"
#endif

#ifndef __BRINPUTMEMORYSTREAM_H__
#include "brinputmemorystream.h"
#endif

#ifndef __BROUTPUTMEMORYSTREAM_H__
#include "broutputmemorystream.h"
#endif

#ifndef __BRPALETTE_H__
#include "brpalette.h"
#endif

/* BEGIN */
namespace Burger {
class GIFEncoder {
	static const uint_t cHashSize = 1U<<13U;			///< 8192 entry (13 bit) hash table
	static const uint_t cHashKeyMask = cHashSize-1U;	///< Mask for the hash size
	static const uint_t cLZBits = 12U;				///< Number of bits for an LZW token
	static const uint_t cLZLastCode = (1U<<cLZBits)-1U;	///< Largest allowable LZW token
	static const uint_t cStartingCode = (1U<<cLZBits);	///< Impossible LZW token for alerting encoding hasn't started
	enum eEncodingState {
		ENCODE_INPROGRESS,	///< Encoding is in progress
		ENCODE_FINAL		///< Encoding is complete, don't output to the stream anymore
	};
	OutputMemoryStream *m_pOutput;	///< Pointer to the stream to store the compressed data
	uint_t m_uColorDepth;				///< Number of bits of color data (8 is the default)
	uint_t m_uRunningCode;			///< Current LZW code
	uint_t m_uRunningBits;			///< Number of valid bits in the current LZW code
	uint_t m_uMaximumRunningCode;		///< If the current LZW matches this valid, increase bit size
	uint_t m_uCode;					///< Value being encoded
	uint_t m_uShiftState;				///< Number of bits in the bit bucket
	uint_t m_uShiftData;				///< Bit bucket
	eEncodingState m_eEncodingState;	///< Flag for determining if a flush is needed on destruction
	uint_t m_uBytesInBuffer;			///< Number of LZW bytes cached before a data flush
	uint8_t m_Buffer[256];			///< Buffer of encoded LZW bytes
	uint_t m_HashTable[cHashSize];	///< Hash for quick match in the LZW buffer for encoding strings
public:
	GIFEncoder();
	static GIFEncoder * BURGER_API new_object(void);
    static BURGER_INLINE uint_t GetHashKey(uint_t uInput) { return ((uInput >> cLZBits) ^ uInput) & cHashKeyMask; }
    static BURGER_INLINE uint_t GetKey(uint_t uInput) { return uInput >> cLZBits; }
    static BURGER_INLINE uint_t PutKey(uint_t uInput) { return uInput << cLZBits; }
    static BURGER_INLINE uint_t GetCode(uint_t uInput) { return uInput & 0xFFFU; }
    static BURGER_INLINE uint_t PutCode(uint_t uInput) { return uInput & 0xFFFU; }
	BURGER_INLINE uint_t GetClearCode(void) const { return 1U << m_uColorDepth; }
	BURGER_INLINE uint_t GetEOFCode(void) const { return (1U << m_uColorDepth) + 1U; }
	void Init(OutputMemoryStream *pOutput,uint_t uDepth);
	void ClearHash(void);
	uint_t LookupHash(uint_t uInput);
	void AddHashEntry(uint_t uKey,uint_t uCode);
	void WriteByte(uint_t uInput);
	void WriteCode(uint_t uCode);
	void WritePixels(const uint8_t *pInput,uintptr_t uInputLength);
	void Flush(void);
};
class GIFDecoder {
	InputMemoryStream *m_pInput;	///< Pointer to packed data
	uint_t m_uBitBucket;				///< Current bit bucket
	uint_t m_uBitCount;				///< Current valid bit count in bucket
	uint_t m_uChunkCount;				///< Number of bytes in stream
	uint_t m_uLZWCodeSize;			///< Starting data size
	const uint8_t *m_LZWTable[4096];	///< Dictionary pointers
	uint8_t m_LZWArray[256];			///< 0-255 for default data
	uint_t m_LZWSize[4096];			///< Size of each entry
	void Init(void);
	void Reset(void);
	uint_t GetCode(uint_t uBitsRequested);
public:
	GIFDecoder();
	static GIFDecoder * BURGER_API new_object(void);
	const char *Unpack(uint8_t *pOutput,uintptr_t uOutputLength,InputMemoryStream *pInput);
};

class FileGIF {
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
	RGBAWord8_t m_GlobalPalette[256];	///< Global GIF palette
	uint_t m_uLogicalWidth;			///< Parsed width of the image
	uint_t m_uLogicalHeight;			///< Parsed height of the image
	uint_t m_uColorShift;				///< Shift value for palette resolution
	uint16_t m_uLoopCount;			///< GIF animation loop count
	uint16_t m_uFrameDelay;			///< GIF animation time delay for current frame
	uint8_t m_bTransparentColor;		///< GIF transparent color index
	uint8_t m_bTransparentColorEnable;	///< \ref TRUE if this frame uses a transparent color index
	uint8_t m_bAspectRatio;			///< GIF aspect ratio
	uint8_t m_bBackgroundColorIndex;	///< GIF background color
	uint8_t m_bGIF89;					///< GIF file read is 89 format, not 87
public:
	const char *ParseHeader(InputMemoryStream *pInput);
	const char *ParseExtension(InputMemoryStream *pInput);
	const char *ParseImage(Image *pOutput,InputMemoryStream *pInput);
	uint_t WriteHeader(OutputMemoryStream *pOutput);
	uint_t WriteImage(OutputMemoryStream *pOutput,const Image *pInput);
	uint_t WriteExtensionNetScape20(OutputMemoryStream *pOutput);
	uint_t WriteExtensionGraphicControl(OutputMemoryStream *pOutput);
	uint_t WriteTerminator(OutputMemoryStream *pOutput);

#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const uint32_t FILETYPE = 0x47494666;		///< 'GIFf' Mac File type for GIF files (Byte swapped on little endian machines)
	static const uint32_t AUXTYPE = 0x6F676C65;		///< 'ogle' Mac creator code for GIF files (Byte swapped on little endian machines)
#else
	static const uint32_t FILETYPE = 0x66464947;		///< 'GIFf'
	static const uint32_t AUXTYPE = 0x656C676F;		///< 'ogle'
#endif
	FileGIF();
	static FileGIF * BURGER_API new_object(void);
	uint_t Load(Image *pOutput,InputMemoryStream *pInput);
	uint_t LoadNextFrame(Image *pOutput,InputMemoryStream *pInput);
	uint_t Save(OutputMemoryStream *pOutput,const Image *pImage);
	uint_t AnimationSaveStart(OutputMemoryStream *pOutput,const Image *pImage,uint_t uLoopCount=0);
	uint_t AnimationSaveFrame(OutputMemoryStream *pOutput,const Image *pImage,uint_t uDelay=0);
	uint_t AnimationSaveFinish(OutputMemoryStream *pOutput);
	BURGER_INLINE uint_t GetAspectRatio(void) const { return m_bAspectRatio;}
	BURGER_INLINE void SetAspectRatio(uint_t bAspectRatio) { m_bAspectRatio = static_cast<uint8_t>(bAspectRatio);}
	BURGER_INLINE uint_t GetBackgroundColorIndex(void) const { return m_bBackgroundColorIndex;}
	BURGER_INLINE void SetBackgroundColorIndex(uint_t bBackGroundColorIndex) { m_bBackgroundColorIndex = static_cast<uint8_t>(bBackGroundColorIndex);}
	BURGER_INLINE uint_t GetTransparentColorIndex(void) const { return m_bTransparentColor;}
	BURGER_INLINE void SetTransparentColorIndex(uint_t bTransparentColor) { m_bTransparentColor = static_cast<uint8_t>(bTransparentColor);}
	BURGER_INLINE uint_t GetTransparentColorEnable(void) const { return m_bTransparentColorEnable;}
	BURGER_INLINE void SetTransparentColorEnable(uint_t bTransparentColorEnable) { m_bTransparentColorEnable = static_cast<uint8_t>(bTransparentColorEnable);}
	BURGER_INLINE uint_t GetLoopCount(void) const { return m_uLoopCount; }
	BURGER_INLINE void SetLoopCount(uint_t uLoopCount) { m_uLoopCount = static_cast<uint16_t>(uLoopCount); }
	BURGER_INLINE uint_t GetFrameDelay(void) const { return m_uFrameDelay; }
	BURGER_INLINE void SetFrameDelay(uint_t uFrameDelay) { m_uFrameDelay = static_cast<uint16_t>(uFrameDelay); }
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	BURGER_INLINE void SetPalette(const RGBWord8_t *pInput,uint_t uStartIndex=0,uint_t uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
	BURGER_INLINE void SetPalette(const RGBAWord8_t *pInput,uint_t uStartIndex=0,uint_t uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
};
}
/* END */

#endif
