/***************************************

	GIF File handler class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	static const Word cHashSize = 1U<<13U;			///< 8192 entry (13 bit) hash table
	static const Word cHashKeyMask = cHashSize-1U;	///< Mask for the hash size
	static const Word cLZBits = 12U;				///< Number of bits for an LZW token
	static const Word cLZLastCode = (1U<<cLZBits)-1U;	///< Largest allowable LZW token
	static const Word cStartingCode = (1U<<cLZBits);	///< Impossible LZW token for alerting encoding hasn't started
	enum eEncodingState {
		ENCODE_INPROGRESS,	///< Encoding is in progress
		ENCODE_FINAL		///< Encoding is complete, don't output to the stream anymore
	};
	OutputMemoryStream *m_pOutput;	///< Pointer to the stream to store the compressed data
	Word m_uColorDepth;				///< Number of bits of color data (8 is the default)
	Word m_uRunningCode;			///< Current LZW code
	Word m_uRunningBits;			///< Number of valid bits in the current LZW code
	Word m_uMaximumRunningCode;		///< If the current LZW matches this valid, increase bit size
	Word m_uCode;					///< Value being encoded
	Word m_uShiftState;				///< Number of bits in the bit bucket
	Word m_uShiftData;				///< Bit bucket
	eEncodingState m_eEncodingState;	///< Flag for determining if a flush is needed on destruction
	Word m_uBytesInBuffer;			///< Number of LZW bytes cached before a data flush
	Word8 m_Buffer[256];			///< Buffer of encoded LZW bytes
	Word m_HashTable[cHashSize];	///< Hash for quick match in the LZW buffer for encoding strings
public:
	GIFEncoder();
	static GIFEncoder * BURGER_API New(void);
	BURGER_INLINE static Word GetHashKey(Word uInput) { return ((uInput >> cLZBits) ^ uInput) & cHashKeyMask; }
	BURGER_INLINE static Word GetKey(Word uInput) { return uInput >> cLZBits; }
	BURGER_INLINE static Word PutKey(Word uInput) { return uInput << cLZBits; }
	BURGER_INLINE static Word GetCode(Word uInput) { return uInput & 0xFFFU; }
	BURGER_INLINE static Word PutCode(Word uInput) { return uInput & 0xFFFU; }
	BURGER_INLINE Word GetClearCode(void) const { return 1U << m_uColorDepth; }
	BURGER_INLINE Word GetEOFCode(void) const { return (1U << m_uColorDepth) + 1U; }
	void Init(OutputMemoryStream *pOutput,Word uDepth);
	void ClearHash(void);
	Word LookupHash(Word uInput);
	void AddHashEntry(Word uKey,Word uCode);
	void WriteByte(Word uInput);
	void WriteCode(Word uCode);
	void WritePixels(const Word8 *pInput,WordPtr uInputLength);
	void Flush(void);
};
class GIFDecoder {
	InputMemoryStream *m_pInput;	///< Pointer to packed data
	Word m_uBitBucket;				///< Current bit bucket
	Word m_uBitCount;				///< Current valid bit count in bucket
	Word m_uChunkCount;				///< Number of bytes in stream
	Word m_uLZWCodeSize;			///< Starting data size
	const Word8 *m_LZWTable[4096];	///< Dictionary pointers
	Word8 m_LZWArray[256]; 			///< 0-255 for default data
	Word m_LZWSize[4096]; 			///< Size of each entry
	void Init(void);
	void Reset(void);
	Word GetCode(Word uBitsRequested);
public:
	GIFDecoder();
	static GIFDecoder * BURGER_API New(void);
	const char *Unpack(Word8 *pOutput,WordPtr uOutputLength,InputMemoryStream *pInput);
};

class FileGIF {
	RGBAWord8_t m_Palette[256];		///< Palette found in the image
	RGBAWord8_t m_GlobalPalette[256];	///< Global GIF palette
	Word m_uLogicalWidth;			///< Parsed width of the image
	Word m_uLogicalHeight;			///< Parsed height of the image
	Word m_uColorShift;				///< Shift value for palette resolution
	Word16 m_uLoopCount;			///< GIF animation loop count
	Word16 m_uFrameDelay;			///< GIF animation time delay for current frame
	Word8 m_bTransparentColor;		///< GIF transparent color index
	Word8 m_bTransparentColorEnable;	///< \ref TRUE if this frame uses a transparent color index
	Word8 m_bAspectRatio;			///< GIF aspect ratio
	Word8 m_bBackgroundColorIndex;	///< GIF background color
	Word8 m_bGIF89;					///< GIF file read is 89 format, not 87
public:
	const char *ParseHeader(InputMemoryStream *pInput);
	const char *ParseExtension(InputMemoryStream *pInput);
	const char *ParseImage(Image *pOutput,InputMemoryStream *pInput);
	Word WriteHeader(OutputMemoryStream *pOutput);
	Word WriteImage(OutputMemoryStream *pOutput,const Image *pInput);
	Word WriteExtensionNetScape20(OutputMemoryStream *pOutput);
	Word WriteExtensionGraphicControl(OutputMemoryStream *pOutput);
	Word WriteTerminator(OutputMemoryStream *pOutput);

#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const Word32 FILETYPE = 0x47494666;		///< 'GIFf' Mac File type for GIF files (Byte swapped on little endian machines)
	static const Word32 AUXTYPE = 0x6F676C65;		///< 'ogle' Mac creator code for GIF files (Byte swapped on little endian machines)
#else
	static const Word32 FILETYPE = 0x66464947;		///< 'GIFf'
	static const Word32 AUXTYPE = 0x656C676F;		///< 'ogle'
#endif
	FileGIF();
	static FileGIF * BURGER_API New(void);
	Word Load(Image *pOutput,InputMemoryStream *pInput);
	Word LoadNextFrame(Image *pOutput,InputMemoryStream *pInput);
	Word Save(OutputMemoryStream *pOutput,const Image *pImage);
	Word AnimationSaveStart(OutputMemoryStream *pOutput,const Image *pImage,Word uLoopCount=0);
	Word AnimationSaveFrame(OutputMemoryStream *pOutput,const Image *pImage,Word uDelay=0);
	Word AnimationSaveFinish(OutputMemoryStream *pOutput);
	BURGER_INLINE Word GetAspectRatio(void) const { return m_bAspectRatio;}
	BURGER_INLINE void SetAspectRatio(Word bAspectRatio) { m_bAspectRatio = static_cast<Word8>(bAspectRatio);}
	BURGER_INLINE Word GetBackgroundColorIndex(void) const { return m_bBackgroundColorIndex;}
	BURGER_INLINE void SetBackgroundColorIndex(Word bBackGroundColorIndex) { m_bBackgroundColorIndex = static_cast<Word8>(bBackGroundColorIndex);}
	BURGER_INLINE Word GetTransparentColorIndex(void) const { return m_bTransparentColor;}
	BURGER_INLINE void SetTransparentColorIndex(Word bTransparentColor) { m_bTransparentColor = static_cast<Word8>(bTransparentColor);}
	BURGER_INLINE Word GetTransparentColorEnable(void) const { return m_bTransparentColorEnable;}
	BURGER_INLINE void SetTransparentColorEnable(Word bTransparentColorEnable) { m_bTransparentColorEnable = static_cast<Word8>(bTransparentColorEnable);}
	BURGER_INLINE Word GetLoopCount(void) const { return m_uLoopCount; }
	BURGER_INLINE void SetLoopCount(Word uLoopCount) { m_uLoopCount = static_cast<Word16>(uLoopCount); }
	BURGER_INLINE Word GetFrameDelay(void) const { return m_uFrameDelay; }
	BURGER_INLINE void SetFrameDelay(Word uFrameDelay) { m_uFrameDelay = static_cast<Word16>(uFrameDelay); }
	BURGER_INLINE const RGBAWord8_t *GetPalette(void) const { return m_Palette; }
	BURGER_INLINE RGBAWord8_t *GetPalette(void) { return m_Palette; }
	BURGER_INLINE void SetPalette(const RGBWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
	BURGER_INLINE void SetPalette(const RGBAWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256) { CopyPalette256(m_Palette,pInput,uStartIndex,uPaletteSize); }
};
}
/* END */

#endif
