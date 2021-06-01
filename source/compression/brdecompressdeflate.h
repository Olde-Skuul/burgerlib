/***************************************

    Decompression manager version of Deflate

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRDECOMPRESSDEFLATE_H__
#define __BRDECOMPRESSDEFLATE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDECOMPRESS_H__
#include "brdecompress.h"
#endif

/* BEGIN */
namespace Burger {
class DecompressDeflate : public Decompress {
	BURGER_RTTI_IN_CLASS();
protected:
	enum eState {
		STATE_METHOD,	///< Waiting for method byte
		STATE_FLAG,		///< Waiting for flag byte
		STATE_DICT4,	///< Four dictionary check bytes to go
		STATE_DICT3,	///< Three dictionary check bytes to go
		STATE_DICT2,	///< Two dictionary check bytes to go
		STATE_DICT1,	///< One dictionary check byte to go
		STATE_DICT0,	///< Waiting for inflateSetDictionary
		STATE_BLOCKS,	///< Decompressing blocks
		STATE_CHECK4,	///< Four check bytes to go
		STATE_CHECK3,	///< Three check bytes to go
		STATE_CHECK2,	///< Two check bytes to go
		STATE_CHECK1,	///< One check byte to go
		STATE_COMPLETE,	///< Decompression completed
		STATE_ABORT		///< Decompression aborted (Fatal)
	};
	enum eBlockMode {
		BLOCKMODE_TYPE,			///< Get type bits (3, including end bit)
		BLOCKMODE_LENS,			///< Get lengths for stored
		BLOCKMODE_STORED,		///< Processing stored block
		BLOCKMODE_TABLE,		///< Get table lengths
		BLOCKMODE_BTREE,		///< Get bit lengths tree for a dynamic block
		BLOCKMODE_DTREE,		///< Get length, distance trees for a dynamic block
		BLOCKMODE_CODES,		///< Processing fixed or dynamic block
		BLOCKMODE_DRY,			///< Output remaining window bytes
		BLOCKMODE_COMPLETED,	///< Block completed
		BLOCKMODE_ABORT			///< Block aborted (Fatal)
	};

	enum eCodesMode {
		CODES_START,	///< Set up for CODES_LEN
		CODES_LEN,		///< Input / Get length/literal/eob next
		CODES_LENEXT,	///< Input / Getting length extra (have base)
		CODES_DIST,		///< Input / Get distance next
		CODES_DISTEXT,	///< Input / Getting distance extra
		CODES_COPY,		///< Output / Copying bytes in window, waiting for space
		CODES_LITERAL,	///< Output / Got literal, waiting for output space
		CODES_WASH,		///< Output / Got end of byte, possibly still output waiting
		CODES_END,		///< Got end of byte and all data flushed
		CODES_ABORT		///< Aborted due to error
	};

	enum {
		FIXED_BLOCKLENGTH=9,		///< Length in 2^?? bits
		FIXED_BLOCKDISTANCE=5,		///< Distance in 2^?? bits
		MAX_WBITS=15,				///< Maximum bits for the depth of the huffman tree
		MAXTREEDYNAMICSIZE=1440,	///< Maximum size of the dynamic tree
		PRESET_DICT=0x20,			///< Preset dictionary flag in zlib header
		Z_DEFLATED=8				///< ZLib flag for deflation decompression
	};

	struct DeflateHuft_t {
		uint8_t m_bExtraOperation;	///< Number of extra bits or operation
		uint8_t m_bBitCount;			///< Number of bits in this code or subcode
		uint16_t m_uBase;				///< Literal, length base, distance base, or table offset
	};

	const uint8_t *m_pInput;		///< Packed data pointer
	uint8_t *m_pOutput;			///< Output data pointer
	const uint8_t *m_pWindowRead;	///< Window read pointer
	uint8_t *m_pWindowWrite;		///< Window write pointer
	const DeflateHuft_t *m_pCodeTree;	///< Pointer into tree
	const DeflateHuft_t *m_pCodeTreeLength;	///< Pointer to literal/length/eob tree
	const DeflateHuft_t *m_pCodeTreeDistance;	///< Pointer distance tree
	uint_t *m_pTreesLengths;			///< Pointer to bit lengths of codes
	DeflateHuft_t *m_pTreesHuffman;	///< Pointer to bit length decoding tree

	uintptr_t m_uInputChunkLength;	///< Number of packed bytes remaining
	uintptr_t m_uOutputChunkLength;	///< Number of bytes in the output buffer
	uintptr_t m_uStoredBytesToCopy;	///< If STORED, bytes left to copy

	eState m_eState;			///< State of the decompression
	eBlockMode m_eBlockMode;	///< Current decompression block mode
	eCodesMode m_eCodesMode;	///< Current inflate_codes mode
	uint32_t m_uChecksumOfChunk;	///< Computed checksum value of decompressed data
	uint32_t m_uChecksumInStream;	///< 32 bit literal pulled from the big endian data stream (Must match DecompressDeflate::m_uChecksumInStream)
	uint32_t m_uAlder32Checksum;	///< Checksum on output
	uint32_t m_uBitBucket;		///< Bit buffer
	uint_t m_uCompressionMethod;	///< Compression method type
	uint_t m_bLastBlock;			///< TRUE if this block is the last block
	uint_t m_uBitCount;			///< Bits in bit buffer
	uint_t m_uCodeLength;			///< Length in bits of the last code
	uint_t m_uCodeTreeNeed;		///< Bits needed
	uint_t m_uCodeLiteral;		///< Literal code fetched
	uint_t m_uCodeCopyGet;		///< Bits to get for extra token
	uint_t m_uCodeCopyDistance;	///< Distance back to copy from
	uint_t m_uTreesTable;			///< Table lengths (14 bits)
	uint_t m_uTreesIndex;			///< Index into blends (or border)
	uint_t m_uTreesDepth;			///< Bit length tree depth
	uint8_t m_bCodeLengthBits;	///< Length tree bits decoded per branch
	uint8_t m_bCodeDistanceBits;	///< Distance tree bits decoder per branch

	uint8_t m_WindowBuffer[1<<MAX_WBITS];	///< Sliding window
	DeflateHuft_t m_HuffmanTable[MAXTREEDYNAMICSIZE];	///< Single data chunk for tree space

	static const DeflateHuft_t s_FixedTreeLiteral[];
	static const DeflateHuft_t s_FixedTreeDistance[];

	int Flush(int iErrorCode);
	int Fast(uint_t uBitLength,uint_t uBitDistance,const DeflateHuft_t *pHuffmanLength,const DeflateHuft_t *pHuffmanDistance);
	int ProcessCodes(int iErrorCode);
	void CodesReset(uint_t bCodeLengthBits,uint_t bCodeDistanceBits,const DeflateHuft_t *pCodeTreeLength,const DeflateHuft_t *pCodeTreeDistance);
	static int BuildHuffmanTrees(const uint_t *pSampleCounts,uint_t uNumberSamples,uint_t uMaxSample,const uint_t *pDefaultLengths,const uint_t *pDefaultBits,DeflateHuft_t **ppNewTree,uint_t *pNewTreeSize,DeflateHuft_t *pExistingTree,uint_t *pHuffmanCount,uint_t *pWorkArea);
	static int TreesBits(const uint_t *pSampleCounts,uint_t *pNewTreeSize,DeflateHuft_t **ppNewTree,DeflateHuft_t *pExistingTree);
	static int TreesDynamic(uint_t uNumberSamples,uint_t uNumberDistance,const uint_t *pSampleCounts,uint_t *pNewTreeSize,uint_t *pNewDistanceSize,DeflateHuft_t **ppNewTree,DeflateHuft_t **ppNewDistance,DeflateHuft_t *pExistingTree);
	void BlocksReset(void);
	int ProcessBlocks(int iErrorCode);
public:
	static const uint32_t Signature = 0x5A4C4942;		///< 'ZLIB'
	DecompressDeflate();
	virtual ~DecompressDeflate();
	eError Reset(void) BURGER_OVERRIDE;
	eError Process(void *pOutput, uintptr_t uOutputChunkLength,const void *pInput, uintptr_t uInputChunkLength) BURGER_OVERRIDE;
};
extern eError BURGER_API SimpleDecompressDeflate(void *pOutput,uintptr_t uOutputChunkLength,const void *pInput,uintptr_t uInputChunkLength);
}
/* END */

#endif

