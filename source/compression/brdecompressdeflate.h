/***************************************

	Decompression manager version of Deflate

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
		Word8 m_bExtraOperation;	///< Number of extra bits or operation
		Word8 m_bBitCount;			///< Number of bits in this code or subcode
		Word16 m_uBase;				///< Literal, length base, distance base, or table offset
	};

	const Word8 *m_pInput;		///< Packed data pointer
	Word8 *m_pOutput;			///< Output data pointer
	const Word8 *m_pWindowRead;	///< Window read pointer
	Word8 *m_pWindowWrite;		///< Window write pointer
	const DeflateHuft_t *m_pCodeTree;	///< Pointer into tree
	const DeflateHuft_t *m_pCodeTreeLength;	///< Pointer to literal/length/eob tree
	const DeflateHuft_t *m_pCodeTreeDistance;	///< Pointer distance tree
	Word *m_pTreesLengths;			///< Pointer to bit lengths of codes
	DeflateHuft_t *m_pTreesHuffman;	///< Pointer to bit length decoding tree

	WordPtr m_uInputChunkLength;	///< Number of packed bytes remaining
	WordPtr m_uOutputChunkLength;	///< Number of bytes in the output buffer
	WordPtr m_uStoredBytesToCopy;	///< If STORED, bytes left to copy

	eState m_eState;			///< State of the decompression
	eBlockMode m_eBlockMode;	///< Current decompression block mode
	eCodesMode m_eCodesMode;	///< Current inflate_codes mode
	Word32 m_uChecksumOfChunk;	///< Computed checksum value of decompressed data
	Word32 m_uChecksumInStream;	///< 32 bit literal pulled from the big endian data stream (Must match DecompressDeflate::m_uChecksumInStream)
	Word32 m_uAlder32Checksum;	///< Checksum on output
	Word32 m_uBitBucket;		///< Bit buffer
	Word m_uCompressionMethod;	///< Compression method type
	Word m_bLastBlock;			///< TRUE if this block is the last block
	Word m_uBitCount;			///< Bits in bit buffer
	Word m_uCodeLength;			///< Length in bits of the last code
	Word m_uCodeTreeNeed;		///< Bits needed
	Word m_uCodeLiteral;		///< Literal code fetched
	Word m_uCodeCopyGet;		///< Bits to get for extra token
	Word m_uCodeCopyDistance;	///< Distance back to copy from
	Word m_uTreesTable;			///< Table lengths (14 bits)
	Word m_uTreesIndex;			///< Index into blends (or border)
	Word m_uTreesDepth;			///< Bit length tree depth
	Word8 m_bCodeLengthBits;	///< Length tree bits decoded per branch
	Word8 m_bCodeDistanceBits;	///< Distance tree bits decoder per branch

	Word8 m_WindowBuffer[1<<MAX_WBITS];	///< Sliding window
	DeflateHuft_t m_HuffmanTable[MAXTREEDYNAMICSIZE];	///< Single data chunk for tree space

	static const DeflateHuft_t s_FixedTreeLiteral[];
	static const DeflateHuft_t s_FixedTreeDistance[];

	int Flush(int iErrorCode);
	int Fast(Word uBitLength,Word uBitDistance,const DeflateHuft_t *pHuffmanLength,const DeflateHuft_t *pHuffmanDistance);
	int ProcessCodes(int iErrorCode);
	void CodesReset(Word bCodeLengthBits,Word bCodeDistanceBits,const DeflateHuft_t *pCodeTreeLength,const DeflateHuft_t *pCodeTreeDistance);
	static int BuildHuffmanTrees(const Word *pSampleCounts,Word uNumberSamples,Word uMaxSample,const Word *pDefaultLengths,const Word *pDefaultBits,DeflateHuft_t **ppNewTree,Word *pNewTreeSize,DeflateHuft_t *pExistingTree,Word *pHuffmanCount,Word *pWorkArea);
	static int TreesBits(const Word *pSampleCounts,Word *pNewTreeSize,DeflateHuft_t **ppNewTree,DeflateHuft_t *pExistingTree);
	static int TreesDynamic(Word uNumberSamples,Word uNumberDistance,const Word *pSampleCounts,Word *pNewTreeSize,Word *pNewDistanceSize,DeflateHuft_t **ppNewTree,DeflateHuft_t **ppNewDistance,DeflateHuft_t *pExistingTree);
	void BlocksReset(void);
	int ProcessBlocks(int iErrorCode);
public:
#if defined(BURGER_BIGENDIAN)
	static const Word32 Signature = 0x5A4C4942;		///< 'ZLIB'
#else
	static const Word32 Signature = 0x42494C5A;		///< 'ZLIB'
#endif
	DecompressDeflate();
	~DecompressDeflate();
	virtual eError Reset(void);
	virtual eError Process(void *pOutput,WordPtr uOutputChunkLength,const void *pInput,WordPtr uInputChunkLength);
};
extern Decompress::eError BURGER_API SimpleDecompressDeflate(void *pOutput,WordPtr uOutputChunkLength,const void *pInput,WordPtr uInputChunkLength);
}
/* END */

#endif

