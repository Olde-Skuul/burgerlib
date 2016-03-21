/***************************************

	Compression manager

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brcompressdeflate.h"
#include "bradler32.h"
#include "brstringfunctions.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::CompressDeflate,Burger::Compress);
#endif

/*
 *  ALGORITHM
 *
 *      The "deflation" process depends on being able to identify portions
 *      of the input text which are identical to earlier input (within a
 *      sliding window trailing behind the input currently being processed).
 *
 *      The most straightforward technique turns out to be the fastest for
 *      most input files: try all possible matches and select the longest.
 *      The key feature of this algorithm is that insertions into the string
 *      dictionary are very simple and thus fast, and deletions are avoided
 *      completely. Insertions are performed at each input character, whereas
 *      string matches are performed only when the previous match ends. So it
 *      is preferable to spend more time in matches to allow very fast string
 *      insertions and avoid deletions. The matching algorithm for small
 *      strings is inspired from that of Rabin & Karp. A brute force approach
 *      is used to find longer strings when a small match has been found.
 *      A similar algorithm is used in comic (by Jan-Mark Wams) and freeze
 *      (by Leonid Broukhis).
 *         A previous version of this file used a more sophisticated algorithm
 *      (by Fiala and Greene) which is guaranteed to run in linear amortized
 *      time, but has a larger average cost, uses more memory and is patented.
 *      However the F&G algorithm may be faster for some highly redundant
 *      files if the parameter max_chain_length (described below) is too large.
 *
 *  ACKNOWLEDGEMENTS
 *
 *      The idea of lazy evaluation of matches is due to Jan-Mark Wams, and
 *      I found it in 'freeze' written by Leonid Broukhis.
 *      Thanks to many people for bug reports and testing.
 *
 *  REFERENCES
 *
 *      Deutsch, L.P.,"DEFLATE Compressed Data Format Specification".
 *      Available in ftp://ds.internic.net/rfc/rfc1951.txt
 *
 *      A description of the Rabin and Karp algorithm is given in the book
 *         "Algorithms" by R. Sedgewick, Addison-Wesley, p252.
 *
 *      Fiala,E.R., and Greene,D.H.
 *         Data Compression with Finite Windows, Comm.ACM, 32,4 (1989) 490-595
 *
 */

/*! ************************************

	\class Burger::CompressDeflate
	\brief Compress data using Deflate Encoding

	This format is the one used by ZLIB.

	Deutsch, L.P.,"DEFLATE Compressed Data Format Specification".
	Available in http://www.ietf.org/rfc/rfc1951.txt

	A description of the Rabin and Karp algorithm is given in the book
	"Algorithms" by R. Sedgewick, Addison-Wesley, p252.

	Fiala,E.R., and Greene,D.H.
	Data Compression with Finite Windows, Comm.ACM, 32,4 (1989) 490-595

	\sa Burger::DecompressDeflate

***************************************/

#if !defined(DOXYGEN)
#define Z_NO_FLUSH      0
#define Z_PARTIAL_FLUSH 1	// will be removed, use Z_SYNC_FLUSH instead
#define Z_SYNC_FLUSH    2
#define Z_FULL_FLUSH    3
#define Z_FINISH        4	// Allowed flush values; see deflate() below for details

#define Z_OK            0
#define Z_STREAM_END    1
#define Z_NEED_DICT     2
#define Z_ERRNO        (-1)
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR   (-3)
#define Z_MEM_ERROR    (-4)
#define Z_BUF_ERROR    (-5)
#define Z_VERSION_ERROR (-6)
#endif

/*! ************************************

	\struct Burger::CompressDeflate::CodeData_t
	\brief Structure for each huffman tree entry

	\sa Burger::CompressDeflate

***************************************/

/*! ************************************

	\struct Burger::CompressDeflate::StaticTreeDesc_t
	\brief Structure for each static huffman tree entry

	\sa Burger::CompressDeflate

***************************************/

// extra bits for each length code

const int Burger::CompressDeflate::g_ExtraLengthBits[LENGTH_CODES] = {
	0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0
};

// extra bits for each distance code

const int Burger::CompressDeflate::g_ExtraDistanceBits[D_CODES] = {
	0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13
};

// extra bits for each bit length code

const int Burger::CompressDeflate::g_ExtraBitLengthBits[BL_CODES] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7};

const Word8 Burger::CompressDeflate::g_BitLengthOrder[BL_CODES] = {
	16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};

//
// The lengths of the bit length codes are sent in order of decreasing
// probability, to avoid transmitting the lengths for unused bit length codes.
//

const Burger::CompressDeflate::CodeData_t Burger::CompressDeflate::g_StaticLengthTrees[L_CODES+2] = {
	{{ 12},{  8}}, {{140},{  8}}, {{ 76},{  8}}, {{204},{  8}}, {{ 44},{  8}},
	{{172},{  8}}, {{108},{  8}}, {{236},{  8}}, {{ 28},{  8}}, {{156},{  8}},
	{{ 92},{  8}}, {{220},{  8}}, {{ 60},{  8}}, {{188},{  8}}, {{124},{  8}},
	{{252},{  8}}, {{  2},{  8}}, {{130},{  8}}, {{ 66},{  8}}, {{194},{  8}},
	{{ 34},{  8}}, {{162},{  8}}, {{ 98},{  8}}, {{226},{  8}}, {{ 18},{  8}},
	{{146},{  8}}, {{ 82},{  8}}, {{210},{  8}}, {{ 50},{  8}}, {{178},{  8}},
	{{114},{  8}}, {{242},{  8}}, {{ 10},{  8}}, {{138},{  8}}, {{ 74},{  8}},
	{{202},{  8}}, {{ 42},{  8}}, {{170},{  8}}, {{106},{  8}}, {{234},{  8}},
	{{ 26},{  8}}, {{154},{  8}}, {{ 90},{  8}}, {{218},{  8}}, {{ 58},{  8}},
	{{186},{  8}}, {{122},{  8}}, {{250},{  8}}, {{  6},{  8}}, {{134},{  8}},
	{{ 70},{  8}}, {{198},{  8}}, {{ 38},{  8}}, {{166},{  8}}, {{102},{  8}},
	{{230},{  8}}, {{ 22},{  8}}, {{150},{  8}}, {{ 86},{  8}}, {{214},{  8}},
	{{ 54},{  8}}, {{182},{  8}}, {{118},{  8}}, {{246},{  8}}, {{ 14},{  8}},
	{{142},{  8}}, {{ 78},{  8}}, {{206},{  8}}, {{ 46},{  8}}, {{174},{  8}},
	{{110},{  8}}, {{238},{  8}}, {{ 30},{  8}}, {{158},{  8}}, {{ 94},{  8}},
	{{222},{  8}}, {{ 62},{  8}}, {{190},{  8}}, {{126},{  8}}, {{254},{  8}},
	{{  1},{  8}}, {{129},{  8}}, {{ 65},{  8}}, {{193},{  8}}, {{ 33},{  8}},
	{{161},{  8}}, {{ 97},{  8}}, {{225},{  8}}, {{ 17},{  8}}, {{145},{  8}},
	{{ 81},{  8}}, {{209},{  8}}, {{ 49},{  8}}, {{177},{  8}}, {{113},{  8}},
	{{241},{  8}}, {{  9},{  8}}, {{137},{  8}}, {{ 73},{  8}}, {{201},{  8}},	
	{{ 41},{  8}}, {{169},{  8}}, {{105},{  8}}, {{233},{  8}}, {{ 25},{  8}},
	{{153},{  8}}, {{ 89},{  8}}, {{217},{  8}}, {{ 57},{  8}}, {{185},{  8}},
	{{121},{  8}}, {{249},{  8}}, {{  5},{  8}}, {{133},{  8}}, {{ 69},{  8}},
	{{197},{  8}}, {{ 37},{  8}}, {{165},{  8}}, {{101},{  8}}, {{229},{  8}},
	{{ 21},{  8}}, {{149},{  8}}, {{ 85},{  8}}, {{213},{  8}}, {{ 53},{  8}},
	{{181},{  8}}, {{117},{  8}}, {{245},{  8}}, {{ 13},{  8}}, {{141},{  8}},
	{{ 77},{  8}}, {{205},{  8}}, {{ 45},{  8}}, {{173},{  8}}, {{109},{  8}},
	{{237},{  8}}, {{ 29},{  8}}, {{157},{  8}}, {{ 93},{  8}}, {{221},{  8}},
	{{ 61},{  8}}, {{189},{  8}}, {{125},{  8}}, {{253},{  8}}, {{ 19},{  9}},
	{{275},{  9}}, {{147},{  9}}, {{403},{  9}}, {{ 83},{  9}}, {{339},{  9}},
	{{211},{  9}}, {{467},{  9}}, {{ 51},{  9}}, {{307},{  9}}, {{179},{  9}},	
	{{435},{  9}}, {{115},{  9}}, {{371},{  9}}, {{243},{  9}}, {{499},{  9}},
	{{ 11},{  9}}, {{267},{  9}}, {{139},{  9}}, {{395},{  9}}, {{ 75},{  9}},
	{{331},{  9}}, {{203},{  9}}, {{459},{  9}}, {{ 43},{  9}}, {{299},{  9}},	
	{{171},{  9}}, {{427},{  9}}, {{107},{  9}}, {{363},{  9}}, {{235},{  9}},
	{{491},{  9}}, {{ 27},{  9}}, {{283},{  9}}, {{155},{  9}}, {{411},{  9}},
	{{ 91},{  9}}, {{347},{  9}}, {{219},{  9}}, {{475},{  9}}, {{ 59},{  9}},
	{{315},{  9}}, {{187},{  9}}, {{443},{  9}}, {{123},{  9}}, {{379},{  9}},
	{{251},{  9}}, {{507},{  9}}, {{  7},{  9}}, {{263},{  9}}, {{135},{  9}},
	{{391},{  9}}, {{ 71},{  9}}, {{327},{  9}}, {{199},{  9}}, {{455},{  9}},
	{{ 39},{  9}}, {{295},{  9}}, {{167},{  9}}, {{423},{  9}}, {{103},{  9}},
	{{359},{  9}}, {{231},{  9}}, {{487},{  9}}, {{ 23},{  9}}, {{279},{  9}},
	{{151},{  9}}, {{407},{  9}}, {{ 87},{  9}}, {{343},{  9}}, {{215},{  9}},
	{{471},{  9}}, {{ 55},{  9}}, {{311},{  9}}, {{183},{  9}}, {{439},{  9}},
	{{119},{  9}}, {{375},{  9}}, {{247},{  9}}, {{503},{  9}}, {{ 15},{  9}},
	{{271},{  9}}, {{143},{  9}}, {{399},{  9}}, {{ 79},{  9}}, {{335},{  9}},
	{{207},{  9}}, {{463},{  9}}, {{ 47},{  9}}, {{303},{  9}}, {{175},{  9}},
	{{431},{  9}}, {{111},{  9}}, {{367},{  9}}, {{239},{  9}}, {{495},{  9}},
	{{ 31},{  9}}, {{287},{  9}}, {{159},{  9}}, {{415},{  9}}, {{ 95},{  9}},
	{{351},{  9}}, {{223},{  9}}, {{479},{  9}}, {{ 63},{  9}}, {{319},{  9}},
	{{191},{  9}}, {{447},{  9}}, {{127},{  9}}, {{383},{  9}}, {{255},{  9}},
	{{511},{  9}}, {{  0},{  7}}, {{ 64},{  7}}, {{ 32},{  7}}, {{ 96},{  7}},
	{{ 16},{  7}}, {{ 80},{  7}}, {{ 48},{  7}}, {{112},{  7}}, {{  8},{  7}},
	{{ 72},{  7}}, {{ 40},{  7}}, {{104},{  7}}, {{ 24},{  7}}, {{ 88},{  7}},
	{{ 56},{  7}}, {{120},{  7}}, {{  4},{  7}}, {{ 68},{  7}}, {{ 36},{  7}},
	{{100},{  7}}, {{ 20},{  7}}, {{ 84},{  7}}, {{ 52},{  7}}, {{116},{  7}},
	{{  3},{  8}}, {{131},{  8}}, {{ 67},{  8}}, {{195},{  8}}, {{ 35},{  8}},
	{{163},{  8}}, {{ 99},{  8}}, {{227},{  8}}
};

//
// Hard coded distance codes
//

const Burger::CompressDeflate::CodeData_t Burger::CompressDeflate::g_StaticDistanceTrees[D_CODES] = {
	{{ 0},{ 5}}, {{16},{ 5}}, {{ 8},{ 5}}, {{24},{ 5}}, {{ 4},{ 5}},
	{{20},{ 5}}, {{12},{ 5}}, {{28},{ 5}}, {{ 2},{ 5}}, {{18},{ 5}},	
	{{10},{ 5}}, {{26},{ 5}}, {{ 6},{ 5}}, {{22},{ 5}}, {{14},{ 5}},
	{{30},{ 5}}, {{ 1},{ 5}}, {{17},{ 5}}, {{ 9},{ 5}}, {{25},{ 5}},
	{{ 5},{ 5}}, {{21},{ 5}}, {{13},{ 5}}, {{29},{ 5}}, {{ 3},{ 5}},
	{{19},{ 5}}, {{11},{ 5}}, {{27},{ 5}}, {{ 7},{ 5}}, {{23},{ 5}}
};

const Word8 Burger::CompressDeflate::g_DistanceCodes[DIST_CODE_LEN] = {
	 0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,
	 8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10,
	10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
	11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
	12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
	13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
	14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0, 16, 17,
	18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22,
	23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
	26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
	27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
	27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
	28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
	29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
	29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
	29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
};

const Word8 Burger::CompressDeflate::g_LengthCodes[MAX_MATCH-MIN_MATCH+1]= {
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 12,
	13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
	17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,
	19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
	21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
	22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
	23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
	25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
	25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
	26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
	26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
	27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
};

const int Burger::CompressDeflate::g_BaseLengths[LENGTH_CODES] = {
	0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56,
	64, 80, 96, 112, 128, 160, 192, 224, 0
};

const int Burger::CompressDeflate::g_BaseDistances[D_CODES] = {
	   0,     1,     2,     3,     4,     6,     8,    12,    16,    24,
	  32,    48,    64,    96,   128,   192,   256,   384,   512,   768,
	1024,  1536,  2048,  3072,  4096,  6144,  8192, 12288, 16384, 24576
};

const Burger::CompressDeflate::StaticTreeDesc_t Burger::CompressDeflate::g_StaticLengthDescription = {
	g_StaticLengthTrees, g_ExtraLengthBits, LITERALS+1, L_CODES, MAX_BITS
};

const Burger::CompressDeflate::StaticTreeDesc_t Burger::CompressDeflate::g_StaticDistanceDescription = {
	g_StaticDistanceTrees, g_ExtraDistanceBits, 0,D_CODES, MAX_BITS
};

const Burger::CompressDeflate::StaticTreeDesc_t Burger::CompressDeflate::g_StaticBitLengthDescription = {
	NULL,g_ExtraBitLengthBits,0,BL_CODES, MAX_BL_BITS
};

/*! ************************************

	\brief Insert a 16 bit value in the output stream in Big Endian order
		
	\param uInput 16 bit value to store in the byte stream

***************************************/

void Burger::CompressDeflate::OutputBigEndian16(Word uInput)
{
	Word uPending = static_cast<Word>(m_iPending);
	Word8 *pOutput=&m_PendingBuffer[uPending];
	m_iPending = static_cast<int>(uPending+2);
	pOutput[0] = static_cast<Word8>(uInput>>8);
	pOutput[1] = static_cast<Word8>(uInput);
}

/*! ************************************

	\brief Flush the bit buffer, keeping at most 7 bits in it.

	Check the output bit bucket and flush up to 16 bits into the byte stream

***************************************/

void Burger::CompressDeflate::BitIndexFlush(void)
{
	Word uCount = m_uBitIndexValid;
	if (uCount>=8) {
		Word uPending = static_cast<Word>(m_iPending);
		Word8 *pOutput=&m_PendingBuffer[uPending];
		++uPending;
		Word uBucket = m_uBitIndexBuffer;
		pOutput[0] = static_cast<Word8>(uBucket);
		uCount -= 8U;
		uBucket = uBucket>>8U;
		if (uCount == 8) {			// Clear it out completely?
			pOutput[1] = static_cast<Word8>(uBucket);
			uCount = 0;
			uBucket = 0;
			++uPending;
		}
		m_uBitIndexValid = uCount;
		m_uBitIndexBuffer = uBucket;
		m_iPending = static_cast<int>(uPending);
	}
}

/*! ************************************

	\brief Flush the bit buffer and align the output on a byte boundary

***************************************/

void Burger::CompressDeflate::BitIndexFlushToByte(void)
{
	Word Count = m_uBitIndexValid;
	if (Count) {
		Word uPending = static_cast<Word>(m_iPending);
		Word8 *pOutput=&m_PendingBuffer[uPending];
		++uPending;
		Word uBucket = m_uBitIndexBuffer;
		pOutput[0] = static_cast<Word8>(uBucket);
		if (Count > 8) {			// Clear it out completely?
			pOutput[1] = static_cast<Word8>(uBucket>>8);
			++uPending;
		}
		m_iPending = static_cast<int>(uPending);
	}
	m_uBitIndexValid = 0;
	m_uBitIndexBuffer = 0;
}

/*! ************************************

	\brief Copy a stored block, storing first the length and its one's complement.

	\param pInput Pointer to the data block
	\param uInputLength Length of the data block

***************************************/

void Burger::CompressDeflate::CopyBlock(const Word8 *pInput,WordPtr uInputLength)
{
	BitIndexFlushToByte();	// align on byte boundary
	m_uLastEOBLength = 8;	// enough lookahead for inflate

	Word uPending = static_cast<Word>(m_iPending);
	Word8 *pOutput=&m_PendingBuffer[uPending];
	{
		pOutput[0] = static_cast<Word8>(uInputLength);
		pOutput[1] = static_cast<Word8>(uInputLength >> 8U);
		WordPtr uCompliment = ~uInputLength;
		pOutput[2] = static_cast<Word8>(uCompliment);
		pOutput[3] = static_cast<Word8>(uCompliment >> 8U);
	}
	m_iPending = static_cast<int>(uPending+4+uInputLength);
	// Perform a memory copy for the rest
	if (uInputLength) {
		pOutput+=4;
		do {
			pOutput[0] = pInput[0];
			++pInput;
			++pOutput;
		} while (--uInputLength);
	}
}

/*! ************************************

	\brief Determine if the data to compress is ASCII or BINARY

	Set the data type to ASCII or BINARY, using a crude approximation:
	binary if more than 20% of the bytes are <= 6 or >= 128, ASCII otherwise.

	Sets the variable m_eDataType to Z_ASCII or Z_BINARY as output

***************************************/

void Burger::CompressDeflate::SetDataType(void)
{
	Word uBinaryFrequency = 0;
	// Count the binary codes
	const CodeData_t *pTrees = m_DynamicLengthTrees;
	Word uIndex = 7;
	do {
		uBinaryFrequency += pTrees->m_FrequencyCount.m_uFrequency;
		++pTrees;
	} while (--uIndex);

	// Count the ASCII codes from 8-127
	Word uASCIIFrequency = 0;
	uIndex = 128-7;
	do {
		uASCIIFrequency += pTrees->m_FrequencyCount.m_uFrequency;
		++pTrees;
	} while (--uIndex);
	
	// Count the rest of the codes (128-255)
	uIndex = 256-128;
	do {
		uBinaryFrequency += pTrees->m_FrequencyCount.m_uFrequency;
		++pTrees;
	} while (--uIndex);

	// Which has more?
	m_eDataType = (uBinaryFrequency > (uASCIIFrequency >> 2) ? Z_BINARY : Z_ASCII);
}

/*! ************************************

	\brief Init a new deflate block

***************************************/

void Burger::CompressDeflate::InitBlock(void)
{
	/* Initialize the trees. */
	Word uIndex = L_CODES;
	CodeData_t *pTrees = m_DynamicLengthTrees;
	do {
		pTrees->m_FrequencyCount.m_uFrequency = 0;
		++pTrees;
	} while (--uIndex);

	uIndex = D_CODES;
	pTrees = m_DynamicDistanceTrees;
	do {
		pTrees->m_FrequencyCount.m_uFrequency = 0;
		++pTrees;
	} while (--uIndex);

	uIndex = BL_CODES;
	pTrees = m_BitLengthTrees;
	do {
		pTrees->m_FrequencyCount.m_uFrequency = 0;
		++pTrees;
	} while (--uIndex);

	m_DynamicLengthTrees[END_BLOCK].m_FrequencyCount.m_uFrequency = 1;
	m_uOptimalLength = 0;
	m_uStaticLength = 0;
	m_uLastLiteral = 0;
	m_uMatches = 0;
}

/*! ************************************

	\brief Initialize the tree data structures for a new zlib stream.

***************************************/

void Burger::CompressDeflate::StaticTreeInit(void)
{
	m_LiteralDescription.m_pDynamicTree = m_DynamicLengthTrees;
	m_LiteralDescription.m_pStaticTree = &g_StaticLengthDescription;

	m_DistanceDescription.m_pDynamicTree = m_DynamicDistanceTrees;
	m_DistanceDescription.m_pStaticTree = &g_StaticDistanceDescription;

	m_BitLengthDescription.m_pDynamicTree = m_BitLengthTrees;
	m_BitLengthDescription.m_pStaticTree = &g_StaticBitLengthDescription;

	m_uBitIndexBuffer = 0;		// No data in the bit buffer
	m_uBitIndexValid = 0;
	m_uLastEOBLength = 8;		// enough lookahead for inflate

	// Initialize the first block of the first file:
	InitBlock();
}

/*! ************************************

	\brief Restore the heap

	Restore the heap property by moving down the tree starting at node k,
	exchanging a node with the smallest of its two sons if necessary, stopping
	when the heap property is re-established (each father smaller than its
	two sons).

	\param pTree Pointer to the tree array to test
	\param k Heap index (Tested against m_iHeapLength)

***************************************/

void Burger::CompressDeflate::PQDownHeap(const CodeData_t *pTree,int k)
{
	int *pHeap = m_Heap;
	int v = pHeap[k];
	int j = k << 1;		// left son of k
	Word8 *pDepth = m_Depth;
	if (j <= m_iHeapLength) {
		do {
			// Set j to the smallest of the two sons:
			int n = pHeap[j+1];
			int m = pHeap[j];
			if ((j < m_iHeapLength) &&
				((pTree[n].m_FrequencyCount.m_uFrequency < pTree[m].m_FrequencyCount.m_uFrequency) ||
				(pTree[n].m_FrequencyCount.m_uFrequency == pTree[m].m_FrequencyCount.m_uFrequency && pDepth[n] <= pDepth[m]))) {
				++j;
				// m = HeapPtr[j];	// See below
				m = n;				// n == HeapPtr[j+1]
			}
			// Exit if v is smaller than both sons
			if (((pTree[v].m_FrequencyCount.m_uFrequency < pTree[m].m_FrequencyCount.m_uFrequency) ||
				(pTree[v].m_FrequencyCount.m_uFrequency == pTree[m].m_FrequencyCount.m_uFrequency && pDepth[v] <= pDepth[m]))) {
				break;
			}
			// Exchange v with the smallest son
			pHeap[k] = pHeap[j]; 
			k = j;

			// And continue down the tree, setting j to the left son of k
			j <<= 1;
		} while (j <= m_iHeapLength);
	}
	pHeap[k] = v;
}

/*! ************************************

	\brief Send the block data compressed using the given Huffman trees

***************************************/

void Burger::CompressDeflate::CompressBlock(const CodeData_t *pLengthTree,const CodeData_t *pDistanceTree)
{
	Word uLiteralIndex = 0;		// running index in l_buf
	if (m_uLastLiteral != 0) {
		do {
			Word uDistance = m_DataBuffer[uLiteralIndex];
			Word uLengthCode = m_LiteralBuffer[uLiteralIndex];	// match length or unmatched char (if dist == 0)
			++uLiteralIndex;
			if (!uDistance) {
				send_code(uLengthCode, pLengthTree);		// send a literal byte
			} else {
			// Here, lc is the match length - MIN_MATCH
				Word uCode = g_LengthCodes[uLengthCode];	// the code to send
				send_code(uCode+LITERALS+1,pLengthTree);	// send the length code
				Word uExtra = static_cast<Word>(g_ExtraLengthBits[uCode]);		// number of extra bits to send
				if (uExtra) {
					uLengthCode -= g_BaseLengths[uCode];
					SendBits(uLengthCode,uExtra);			// send the extra length bits
				}
				--uDistance;			// uDistance is now the match distance - 1
				uCode = (uDistance < 256) ? g_DistanceCodes[uDistance] : g_DistanceCodes[256+(uDistance>>7)];

				send_code(uCode,pDistanceTree);		// send the distance code
				uExtra = static_cast<Word>(g_ExtraDistanceBits[uCode]);
				if (uExtra) {
					uDistance -= g_BaseDistances[uCode];
					SendBits(uDistance, uExtra);	// send the extra distance bits
				}
			}
			// literal or match pair ?
			// Check that the overlay between pending_buf and d_buf+l_buf is ok:
		} while (uLiteralIndex < m_uLastLiteral);
	}
	send_code(END_BLOCK,pLengthTree);
	m_uLastEOBLength = pLengthTree[END_BLOCK].m_DataLength.m_uLength;
}

/*! ************************************

	\brief Read a new buffer from the current input stream.

	Update the adler32 and total number of bytes read.

***************************************/

WordPtr Burger::CompressDeflate::ReadBuffer(Word8 *pOutput,WordPtr uOutputSize)
{
	WordPtr uInputLength = m_uInputLength;

	if (uInputLength > uOutputSize) {
		uInputLength = uOutputSize;
	}
	if (uInputLength) {
		m_uInputLength -= uInputLength;
		if (!m_bNoHeader) {
			m_uAdler = CalcAdler32(m_pInput,uInputLength,m_uAdler);
		}
		MemoryCopy(pOutput,m_pInput,uInputLength);
		m_pInput += uInputLength;
	}
	return uInputLength;
}

/*! ************************************

	\brief Fill the window when the lookahead becomes insufficient.

	Updates m_uStringStart and m_uLookAhead.

	IN assertion: lookahead < MIN_LOOKAHEAD
	OUT assertions: m_uStringStart <= window_size-MIN_LOOKAHEAD
		At least one byte has been read, or avail_in == 0; reads are
		performed for at least two bytes (required for the zip translate_eol
		option -- not supported here).

***************************************/

void Burger::CompressDeflate::FillWindow(void)
{
	do {
		Word uMore = (c_uWindowSize - m_uLookAhead) - m_uStringStart;

		// If the window is almost full and there is insufficient lookahead,
		// move the upper half to the lower one to make room in the upper half.
		
		if (m_uStringStart >= c_uWSize+(c_uWSize-MIN_LOOKAHEAD)) {

			MemoryCopy(m_Window,m_Window+c_uWSize,c_uWSize);
			m_uMatchStart -= c_uWSize;
			m_uStringStart -= c_uWSize;		// we now have m_uStringStart >= MAX_DIST
			m_iBlockStart -= c_uWSize;

		/* Slide the hash table (could be avoided with 32 bit values
		at the expense of memory usage). We slide even when level == 0
		to keep the hash table consistent if we switch back to level > 0
		later. (Using level 0 permanently is not an optimal usage of
		zlib, so we don't care about this pathological case.)
		*/
			Word uLoop = c_uHashSize;		
			Word16 *pWork = &m_Head[c_uHashSize];
			do {
				--pWork;
				Word uTemp = pWork[0];
				pWork[0] = static_cast<Word16>((uTemp >= c_uWSize) ? (uTemp-c_uWSize) : 0);
			} while (--uLoop);

			uLoop = c_uWSize;
			pWork = &m_Previous[c_uWSize];
			do {
				--pWork;
				Word uTemp = pWork[0];
				pWork[0] = static_cast<Word16>((uTemp >= c_uWSize) ? (uTemp-c_uWSize) : 0);
				/* If n is not on any hash chain, prev[n] is garbage but
				* its value will never be used.
				*/
			} while (--uLoop);
			uMore += c_uWSize;
		}
		if (!m_uInputLength) {
			break;
		}

		/* If there was no sliding:
		*    m_uStringStart <= WSIZE+MAX_DIST-1 && m_uLookAhead <= MIN_LOOKAHEAD - 1 &&
		*    more == window_size - m_uLookAhead - m_uStringStart
		* => more >= window_size - (MIN_LOOKAHEAD-1 + WSIZE + MAX_DIST-1)
		* => more >= window_size - 2*WSIZE + 2
		* In the BIG_MEM or MMAP case (not yet supported),
		*   window_size == input_size + MIN_LOOKAHEAD  &&
		*   m_uStringStart + m_uLookAhead <= input_size => more >= MIN_LOOKAHEAD.
		* Otherwise, window_size == 2*WSIZE so more >= 2.
		* If there was sliding, more >= WSIZE. So in all cases, more >= 2.
		*/

		WordPtr uReadCount = ReadBuffer(m_Window + m_uStringStart + m_uLookAhead,uMore);
		m_uLookAhead += static_cast<Word>(uReadCount);

		// Initialize the hash value now that we have some input:
		if (m_uLookAhead >= MIN_MATCH) {
			m_uInsertHash = UpdateHash(m_Window[m_uStringStart],m_Window[m_uStringStart+1]);
		}
		/* If the whole input has less than MIN_MATCH bytes, ins_h is garbage,
		* but this is not important since only literal bytes will be emitted.
		*/
	} while ((m_uLookAhead < MIN_LOOKAHEAD) && m_uInputLength);
}

/*! ************************************

	\brief Compute the optimal bit lengths for a tree and update the total bit length for the current block.

	IN assertion: the fields freq and dad are set, heap[heap_max] and
		above are the tree nodes sorted by increasing frequency.
	OUT assertions: the field len is set to the optimal bit length, the
		array bl_count contains the frequencies for each bit length.
		The length opt_len is updated; static_len is also updated if stree is
		not null.

***************************************/

void Burger::CompressDeflate::GenerateBitLengths(const TreeDesc_t *pTreeDescription)
{
	CodeData_t *pDynamicTree = pTreeDescription->m_pDynamicTree;
	int iMaximumCode = pTreeDescription->m_iMaximumCode;
	const CodeData_t *pStaticTree = pTreeDescription->m_pStaticTree->m_pTree;
	const int *pExtraBits = pTreeDescription->m_pStaticTree->m_pExtraBits;
	int iExtraBase = pTreeDescription->m_pStaticTree->m_iExtraBase;
	int iMaxLength = pTreeDescription->m_pStaticTree->m_iMaxLength;
	int h;				/* heap index */
	int n, m;			/* iterate over the tree elements */
	int xbits;			/* extra bits */
	Word16 f;			/* frequency */
	int overflow = 0;	/* number of elements with bit length too large */

	int bits = 0;	// bit length
	do {
		m_BitLengthCount[bits] = 0;
	} while (++bits<=MAX_BITS);

	/* In a first pass, compute the optimal bit lengths (which may
	* overflow in the case of the bit length tree).
	*/
	pDynamicTree[m_Heap[m_iHeapMaximum]].m_DataLength.m_uLength = 0; /* root of the heap */

	for (h = m_iHeapMaximum+1; h < HEAP_SIZE; h++) {
		n = m_Heap[h];
		bits = pDynamicTree[pDynamicTree[n].m_DataLength.m_uDadBits].m_DataLength.m_uLength + 1;
		if (bits > iMaxLength) {
			bits = iMaxLength;
			overflow++;
		}
		pDynamicTree[n].m_DataLength.m_uLength = (Word16)bits;
		/* We overwrite tree[n].dl.dad which is no longer needed */

		if (n > iMaximumCode) {
			continue; /* not a leaf node */
		}
		m_BitLengthCount[bits]++;
		xbits = 0;
		if (n >= iExtraBase) {
			xbits = pExtraBits[n-iExtraBase];
		}
		f = pDynamicTree[n].m_FrequencyCount.m_uFrequency;
		m_uOptimalLength += (Word32)f * (bits + xbits);
		if (pStaticTree) {
			m_uStaticLength += (Word32)f * (pStaticTree[n].m_DataLength.m_uLength + xbits);
		}
	}
	if (overflow == 0) {
		return;
	}

	/* Find the first bit length which could increase: */
    do {
        bits = iMaxLength-1;
        while (m_BitLengthCount[bits] == 0) {
        	bits--;
        }
        m_BitLengthCount[bits]--;      /* move one leaf down the tree */
        m_BitLengthCount[bits+1] += 2; /* move one overflow item as its brother */
        m_BitLengthCount[iMaxLength]--;
        /* The brother of the overflow item also moves one step up,
         * but this does not affect bl_count[max_length]
         */
        overflow -= 2;
    } while (overflow > 0);

    /* Now recompute all bit lengths, scanning in increasing frequency.
     * h is still equal to HEAP_SIZE. (It is simpler to reconstruct all
     * lengths instead of fixing only the wrong ones. This idea is taken
     * from 'ar' written by Haruhiko Okumura.)
     */
    for (bits = iMaxLength; bits != 0; bits--) {
        n = m_BitLengthCount[bits];
        while (n != 0) {
            m = m_Heap[--h];
            if (m > iMaximumCode) continue;
            if (pDynamicTree[m].m_DataLength.m_uLength != (Word) bits) {
                m_uOptimalLength += static_cast<Word32>(((long)bits - (long)pDynamicTree[m].m_DataLength.m_uLength)
                              *(long)pDynamicTree[m].m_FrequencyCount.m_uFrequency);
                pDynamicTree[m].m_DataLength.m_uLength = (Word16)bits;
            }
            n--;
        }
    }
}


/* ===========================================================================
 * Generate the codes for a given tree and bit counts (which need not be
 * optimal).
 * IN assertion: the array bl_count contains the bit length statistics for
 * the given tree and the field len is set for all tree elements.
 * OUT assertion: the field code is set for all tree elements of non
 *     zero code length.
 */
void Burger::CompressDeflate::GenerateCodes(CodeData_t *tree, int max_code, Word16 *bl_count)
{
    Word16 next_code[MAX_BITS+1]; /* next code value for each bit length */
    Word16 code = 0;              /* running code value */
    int bits;                  /* bit index */
    int n;                     /* code index */

    /* The distribution counts are first used to generate the code values
     * without bit reversal.
     */
    for (bits = 1; bits <= MAX_BITS; bits++) {
	    code = static_cast<Word16>((code + bl_count[bits-1]) << 1);
        next_code[bits] = code;
    }
    /* Check that the bit counts in bl_count are consistent. The last code
     * must be all ones.
     */

    for (n = 0;  n <= max_code; n++) {
        Word len = tree[n].m_DataLength.m_uLength;
        if (len) {
	        /* Now reverse the bits */
    	    tree[n].m_FrequencyCount.m_uCode = static_cast<Word16>(BitReverse(static_cast<Word32>(next_code[len]++), len));
		}
    }
}

/* ===========================================================================
 * Construct one Huffman tree and assigns the code bit strings and lengths.
 * Update the total bit length for the current block.
 * IN assertion: the field freq is set for all tree elements.
 * OUT assertions: the fields len and code are set to the optimal bit length
 *     and corresponding code. The length opt_len is updated; static_len is
 *     also updated if stree is not null. The field max_code is set.
 */
void Burger::CompressDeflate::BuildTree(TreeDesc_t *desc)
{
    CodeData_t *tree         = desc->m_pDynamicTree;
    const CodeData_t *stree  = desc->m_pStaticTree->m_pTree;
    int elems             = desc->m_pStaticTree->m_iElements;
    int n, m;          /* iterate over heap elements */
    int max_code = -1; /* largest code with non zero frequency */
    int node;          /* new node being created */

    /* Construct the initial heap, with least frequent element in
     * heap[SMALLEST]. The sons of heap[n] are heap[2*n] and heap[2*n+1].
     * heap[0] is not used.
     */
    m_iHeapLength = 0;
	m_iHeapMaximum = HEAP_SIZE;

    for (n = 0; n < elems; n++) {
        if (tree[n].m_FrequencyCount.m_uFrequency != 0) {
            m_Heap[++(m_iHeapLength)] = max_code = n;
            m_Depth[n] = 0;
        } else {
            tree[n].m_DataLength.m_uLength = 0;
        }
    }

    /* The pkzip format requires that at least one distance code exists,
     * and that at least one bit should be sent even if there is only one
     * possible code. So to avoid special checks later on we force at least
     * two codes of non zero frequency.
     */
    while (m_iHeapLength < 2) {
        node = m_Heap[++(m_iHeapLength)] = (max_code < 2 ? ++max_code : 0);
        tree[node].m_FrequencyCount.m_uFrequency = 1;
        m_Depth[node] = 0;
        m_uOptimalLength--; if (stree) m_uStaticLength -= stree[node].m_DataLength.m_uLength;
        /* node is 0 or 1 so it does not have extra bits */
    }
    desc->m_iMaximumCode = max_code;

    /* The elements heap[heap_len/2+1 .. heap_len] are leaves of the tree,
     * establish sub-heaps of increasing lengths:
     */
    for (n = m_iHeapLength/2; n >= 1; n--) PQDownHeap(tree, n);

    /* Construct the Huffman tree by repeatedly combining the least two
     * frequent nodes.
     */
    node = elems;              /* next internal node of the tree */
    do {
		n = m_Heap[SMALLEST];
		m_Heap[SMALLEST] = m_Heap[m_iHeapLength--];
		PQDownHeap(tree, SMALLEST);		/* n = node of least frequency */
        m = m_Heap[SMALLEST]; /* m = node of next least frequency */

        m_Heap[--(m_iHeapMaximum)] = n; /* keep the nodes sorted by frequency */
        m_Heap[--(m_iHeapMaximum)] = m;

        /* Create a new node father of n and m */
        tree[node].m_FrequencyCount.m_uFrequency = static_cast<Word16>(tree[n].m_FrequencyCount.m_uFrequency + tree[m].m_FrequencyCount.m_uFrequency);

        m_Depth[node] = (Word8) (((m_Depth[n] >= m_Depth[m]) ? m_Depth[n] : m_Depth[m]) + 1);
        tree[n].m_DataLength.m_uDadBits = tree[m].m_DataLength.m_uDadBits = (Word16)node;
        /* and insert the new node in the heap */
        m_Heap[SMALLEST] = node++;
        PQDownHeap(tree, SMALLEST);

    } while (m_iHeapLength >= 2);

    m_Heap[--(m_iHeapMaximum)] = m_Heap[SMALLEST];

    /* At this point, the fields freq and dad are set. We can now
     * generate the bit lengths.
     */
    GenerateBitLengths(desc);

    /* The field len is now set, we can generate the bit codes */
    GenerateCodes (tree, max_code, m_BitLengthCount);
}

/* ===========================================================================
 * Scan a literal or distance tree to determine the frequencies of the codes
 * in the bit length tree.
 */
void Burger::CompressDeflate::ScanTree(CodeData_t *tree,int max_code)
{
    int n;                     /* iterates over all tree elements */
    int prevlen = -1;          /* last emitted length */
    int curlen;                /* length of current code */
    int nextlen = tree[0].m_DataLength.m_uLength; /* length of next code */
    int count = 0;             /* repeat count of the current code */
    int max_count = 7;         /* max repeat count */
    int min_count = 4;         /* min repeat count */

    if (nextlen == 0) {
    	max_count = 138;
    	min_count = 3;
    }
    tree[max_code+1].m_DataLength.m_uLength = (Word16)0xffff; /* guard */

    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].m_DataLength.m_uLength;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            m_BitLengthTrees[curlen].m_FrequencyCount.m_uFrequency = static_cast<Word16>(m_BitLengthTrees[curlen].m_FrequencyCount.m_uFrequency+count);
        } else if (curlen != 0) {
            if (curlen != prevlen) m_BitLengthTrees[curlen].m_FrequencyCount.m_uFrequency++;
            m_BitLengthTrees[REP_3_6].m_FrequencyCount.m_uFrequency++;
        } else if (count <= 10) {
            m_BitLengthTrees[REPZ_3_10].m_FrequencyCount.m_uFrequency++;
        } else {
            m_BitLengthTrees[REPZ_11_138].m_FrequencyCount.m_uFrequency++;
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138, min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6, min_count = 3;
        } else {
            max_count = 7, min_count = 4;
        }
    }
}

/* ===========================================================================
 * Construct the Huffman tree for the bit lengths and return the index in
 * bl_order of the last bit length code to send.
 */
int Burger::CompressDeflate::BuildBitLengthTree(void)
{
    int max_blindex;  /* index of last bit length code of non zero freq */

    /* Determine the bit length frequencies for literal and distance trees */
    ScanTree(m_DynamicLengthTrees,m_LiteralDescription.m_iMaximumCode);
    ScanTree(m_DynamicDistanceTrees,m_DistanceDescription.m_iMaximumCode);

    /* Build the bit length tree: */
    BuildTree(&m_BitLengthDescription);
    /* opt_len now includes the length of the tree representations, except
     * the lengths of the bit lengths codes and the 5+5+4 bits for the counts.
     */

    /* Determine the number of bit length codes to send. The pkzip format
     * requires that at least 4 bit length codes be sent. (appnote.txt says
     * 3 but the actual value used is 4.)
     */
    for (max_blindex = BL_CODES-1; max_blindex >= 3; max_blindex--) {
        if (m_BitLengthTrees[g_BitLengthOrder[max_blindex]].m_DataLength.m_uLength != 0) break;
    }
    /* Update opt_len to include the bit length tree and counts */
    m_uOptimalLength += 3*(max_blindex+1) + 5+5+4;

    return max_blindex;
}

/* ===========================================================================
 * Send a stored block
 */
void Burger::CompressDeflate::StoredBlock(const Word8 *buf,Word32 stored_len,int eof)
{
    SendBits(static_cast<Word>((STORED_BLOCK<<1)+eof), 3);  /* send block type */
    CopyBlock(buf,(Word)stored_len); /* with header */
}


/* ===========================================================================
 * Send a literal or distance tree in compressed form, using the codes in
 * bl_tree.
 */
void Burger::CompressDeflate::SendTree(CodeData_t *tree, int max_code)
{
    int n;                     /* iterates over all tree elements */
    int prevlen = -1;          /* last emitted length */
    int curlen;                /* length of current code */
    int nextlen = tree[0].m_DataLength.m_uLength; /* length of next code */
    int count = 0;             /* repeat count of the current code */
    int max_count = 7;         /* max repeat count */
    int min_count = 4;         /* min repeat count */

    /* tree[max_code+1].dl.len = -1; */  /* guard already set */
    if (nextlen == 0) {
    	max_count = 138;
    	min_count = 3;
	}
    for (n = 0; n <= max_code; n++) {
        curlen = nextlen; nextlen = tree[n+1].m_DataLength.m_uLength;
        if (++count < max_count && curlen == nextlen) {
            continue;
        } else if (count < min_count) {
            do {
            	send_code(static_cast<Word>(curlen),m_BitLengthTrees);
            } while (--count != 0);

        } else if (curlen != 0) {
            if (curlen != prevlen) {
                send_code(static_cast<Word>(curlen),m_BitLengthTrees); count--;
            }
            send_code(REP_3_6,m_BitLengthTrees);
            SendBits(static_cast<Word>(count-3), 2);

        } else if (count <= 10) {
            send_code(REPZ_3_10,m_BitLengthTrees);
            SendBits(static_cast<Word>(count-3), 3);

        } else {
            send_code(REPZ_11_138,m_BitLengthTrees);
            SendBits(static_cast<Word>(count-11), 7);
        }
        count = 0; prevlen = curlen;
        if (nextlen == 0) {
            max_count = 138;
            min_count = 3;
        } else if (curlen == nextlen) {
            max_count = 6;
            min_count = 3;
        } else {
            max_count = 7;
            min_count = 4;
        }
    }
}

/* ===========================================================================
 * Send the header for a block using dynamic Huffman trees: the counts, the
 * lengths of the bit length codes, the literal tree and the distance tree.
 * IN assertion: lcodes >= 257, dcodes >= 1, blcodes >= 4.
 */
void Burger::CompressDeflate::SendAllTrees(int lcodes,int dcodes,int blcodes)
{
	SendBits(static_cast<Word>(lcodes-257),5); /* not +255 as stated in appnote.txt */
	SendBits(static_cast<Word>(dcodes-1),5);
	SendBits(static_cast<Word>(blcodes-4),4); /* not -3 as stated in appnote.txt */
	int rank;                    /* index in bl_order */
	for (rank = 0; rank < blcodes; rank++) {
		SendBits(m_BitLengthTrees[g_BitLengthOrder[rank]].m_DataLength.m_uLength, 3);
	}
	SendTree(m_DynamicLengthTrees, lcodes-1); /* literal tree */
	SendTree(m_DynamicDistanceTrees, dcodes-1); /* distance tree */
}

/* ===========================================================================
 * Determine the best encoding for the current block: dynamic trees, static
 * trees or store, and output the encoded block to the zip file.
 */
void Burger::CompressDeflate::FlushBlock(const Word8 *buf,Word32 stored_len,Word bEOF)
{
    Word32 opt_lenb, static_lenb; /* opt_len and static_len in bytes */
    int max_blindex = 0;  /* index of last bit length code of non zero freq */

    /* Build the Huffman trees unless a stored block is forced */

	 /* Check if the file is ascii or binary */
	if (m_eDataType == Z_UNKNOWN) {
		SetDataType();
	}
	/* Construct the literal and distance trees */
	BuildTree(&m_LiteralDescription);

	BuildTree(&m_DistanceDescription);
	/* At this point, opt_len and static_len are the total bit lengths of
	 * the compressed block data, excluding the tree representations.
	 */

	/* Build the bit length tree for the above two trees, and get the index
	 * in bl_order of the last bit length code to send.
	 */
	max_blindex = BuildBitLengthTree();

	/* Determine the best encoding. Compute first the block length in bytes*/
	opt_lenb = (m_uOptimalLength+3+7)>>3;
	static_lenb = (m_uStaticLength+3+7)>>3;


	if (static_lenb <= opt_lenb) {
		opt_lenb = static_lenb;
    }

    if (stored_len+4 <= opt_lenb && buf != NULL) {
                       /* 4: two words for the lengths */
        /* The test buf != NULL is only necessary if LIT_BUFSIZE > WSIZE.
         * Otherwise we can't have processed more than WSIZE input bytes since
         * the last block flush, because compression would have been
         * successful. If LIT_BUFSIZE <= WSIZE, it is never too late to
         * transform a block into a stored block.
         */
        StoredBlock(buf,static_cast<Word32>(stored_len),static_cast<int>(bEOF));

    } else if (static_lenb == opt_lenb) {
        SendBits((STATIC_TREES<<1)+bEOF, 3);
        CompressBlock(g_StaticLengthTrees,g_StaticDistanceTrees);
    } else {
        SendBits((DYN_TREES<<1)+bEOF, 3);
        SendAllTrees(m_LiteralDescription.m_iMaximumCode+1,m_DistanceDescription.m_iMaximumCode+1,
                       max_blindex+1);
        CompressBlock(m_DynamicLengthTrees,m_DynamicDistanceTrees);
    }
    /* The above check is made mod 2^32, for files larger than 512 MB
     * and uLong implemented on 32 bits.
     */
    InitBlock();

    if (bEOF) {
        BitIndexFlushToByte();
    }
}


/* =========================================================================
 * Flush as much pending output as possible. All deflate() output goes
 * through this function so some applications may wish to modify it
 * to avoid allocating a large strm->next_out buffer and copying into it.
 * (See also read_buf()).
 */
void Burger::CompressDeflate::FlushPending(void)
{
    Word len = static_cast<Word>(m_iPending);
    if (len) {
		m_Output.Append(m_pPendingOutput,len);
		m_pPendingOutput  += len;
		m_iPending -= len;
		if (m_iPending == 0) {
			m_pPendingOutput = m_PendingBuffer;
		}
	}
}


/* ===========================================================================
 * Set match_start to the longest match starting at the given string and
 * return its length. Matches shorter or equal to prev_length are discarded,
 * in which case the result is equal to prev_length and match_start is
 * garbage.
 * IN assertions: cur_match is the head of the hash chain for the current
 *   string (strstart) and its distance is <= MAX_DIST, and prev_length >= 1
 * OUT assertion: the match length is not greater than s->lookahead.
 */

Word Burger::CompressDeflate::LongestMatch(Word cur_match)
{
    unsigned chain_length = c_uMaxChainLength;/* max hash chain length */
    register Word8 *scan = m_Window + m_uStringStart; /* current string */
    register Word8 *match;                       /* matched string */
    register int len;                           /* length of current match */
    int best_len = static_cast<int>(m_uPreviousLength);              /* best match length so far */
    int nice_match = c_iNiceMatch;             /* stop if match long enough */
    Word limit = m_uStringStart > (Word)(c_uWSize-MIN_LOOKAHEAD) ?
        m_uStringStart - (Word)(c_uWSize-MIN_LOOKAHEAD) : 0;
    /* Stop when cur_match becomes <= limit. To simplify the code,
     * we prevent matches with the string of window index 0.
     */
    Word16 *prev = m_Previous;
    Word wmask = c_uWMask;

    /* Compare two bytes at a time. Note: this is not always beneficial.
     * Try with and without -DUNALIGNED_OK to check.
     */
    register Word8 *strend = m_Window + m_uStringStart + MAX_MATCH - 1;
    register Word16 scan_start = *(Word16*)scan;
    register Word16 scan_end   = *(Word16*)(scan+best_len-1);


    /* The code is optimized for HASH_BITS >= 8 and MAX_MATCH-2 multiple of 16.
     * It is easy to get rid of this optimization if necessary.
     */

    /* Do not waste too much time if we already have a good match: */
    if (m_uPreviousLength >= c_uGoodMatch) {
        chain_length >>= 2;
    }
    /* Do not look for matches beyond the end of the input. This is necessary
     * to make deflate deterministic.
     */
    if ((Word)nice_match > m_uLookAhead) {
    	nice_match = static_cast<int>(m_uLookAhead);
	}
    do {
        match = m_Window + cur_match;

        /* Skip to next match if the match length cannot increase
         * or if the match length is less than 2:
         */
        /* This code assumes sizeof(unsigned short) == 2. Do not use
         * UNALIGNED_OK if your compiler uses a different size.
         */
        if (*(Word16*)(match+best_len-1) != scan_end ||
            *(Word16*)match != scan_start) continue;

        /* It is not necessary to compare scan[2] and match[2] since they are
         * always equal when the other bytes match, given that the hash keys
         * are equal and that HASH_BITS >= 8. Compare 2 bytes at a time at
         * strstart+3, +5, ... up to strstart+257. We check for insufficient
         * lookahead only every 4th comparison; the 128th check will be made
         * at strstart+257. If MAX_MATCH-2 is not a multiple of 8, it is
         * necessary to put more guard bytes at the end of the window, or
         * to check more often for insufficient lookahead.
         */
        scan++, match++;
        do {
        } while (*(Word16*)(scan+=2) == *(Word16*)(match+=2) &&
                 *(Word16*)(scan+=2) == *(Word16*)(match+=2) &&
                 *(Word16*)(scan+=2) == *(Word16*)(match+=2) &&
                 *(Word16*)(scan+=2) == *(Word16*)(match+=2) &&
                 scan < strend);
        /* The funny "do {}" generates better code on most compilers */

        /* Here, scan <= window+strstart+257 */
        if (*scan == *match) scan++;

        len = (MAX_MATCH - 1) - (int)(strend-scan);
        scan = strend - (MAX_MATCH-1);


        if (len > best_len) {
            m_uMatchStart = cur_match;
            best_len = len;
            if (len >= nice_match) break;
            scan_end = *(Word16*)(scan+best_len-1);
        }
    } while ((cur_match = prev[cur_match & wmask]) > limit
             && --chain_length != 0);

    if ((Word)best_len <= m_uLookAhead) return (Word)best_len;
    return m_uLookAhead;
}

/* ===========================================================================
 * Same as above, but achieves better compression. We use a lazy
 * evaluation for matches: a match is finally adopted only if there is
 * no better match at the next window position.
 */
Burger::CompressDeflate::eBlockState Burger::CompressDeflate::DeflateSlow(int flush)
{
    Word hash_head = 0;    /* head of hash chain */
    int bflush;              /* set if current block must be flushed */

    /* Process the input block. */
    for (;;) {
        /* Make sure that we always have enough lookahead, except
         * at the end of the input file. We need MAX_MATCH bytes
         * for the next match, plus MIN_MATCH bytes to insert the
         * string following the next match.
         */
        if (m_uLookAhead < MIN_LOOKAHEAD) {
            FillWindow();
            if (m_uLookAhead < MIN_LOOKAHEAD && flush == Z_NO_FLUSH) {
	        return STATE_NEEDMORE;
	    }
            if (m_uLookAhead == 0) break; /* flush the current block */
        }

        /* Insert the string window[strstart .. strstart+2] in the
         * dictionary, and set hash_head to the head of the hash chain:
         */
        if (m_uLookAhead >= MIN_MATCH) {
            hash_head = InsertString(m_uStringStart);
        }

        /* Find the longest match, discarding those <= prev_length.
         */
        m_uPreviousLength = m_uMatchLength;
		m_uPreviousMatch = m_uMatchStart;
        m_uMatchLength = MIN_MATCH-1;

        if (hash_head != 0 && m_uPreviousLength < c_uMaxLazyMatch &&
            m_uStringStart - hash_head <= (c_uWSize-MIN_LOOKAHEAD)) {
            /* To simplify the code, we prevent matches with the string
             * of window index 0 (in particular we have to avoid a match
             * of the string with itself at the start of the input file).
             */
            m_uMatchLength = LongestMatch (hash_head);
            /* longest_match() sets match_start */

            if (m_uMatchLength <= 5 && (
                 (m_uMatchLength == MIN_MATCH &&
                  m_uStringStart - m_uMatchStart > TOO_FAR))) {

                /* If prev_match is also MIN_MATCH, match_start is garbage
                 * but we will ignore the current match anyway.
                 */
                m_uMatchLength = MIN_MATCH-1;
            }
        }
        /* If there was a match at the previous step and the current
         * match is not better, output the previous match:
         */
        if (m_uPreviousLength >= MIN_MATCH && m_uMatchLength <= m_uPreviousLength) {
            Word max_insert = m_uStringStart + m_uLookAhead - MIN_MATCH;
            /* Do not insert strings in hash table beyond this. */

            bflush = static_cast<int>(TallyDistance(m_uStringStart -1 - m_uPreviousMatch,m_uPreviousLength - MIN_MATCH));

            /* Insert in hash table all strings up to the end of the match.
             * strstart-1 and strstart are already inserted. If there is not
             * enough lookahead, the last two strings are not inserted in
             * the hash table.
             */
            m_uLookAhead -= m_uPreviousLength-1;
            m_uPreviousLength -= 2;
            do {
                if (++m_uStringStart <= max_insert) {
                    hash_head = InsertString(m_uStringStart);
                }
            } while (--m_uPreviousLength != 0);
            m_bMatchAvailable = 0;
            m_uMatchLength = MIN_MATCH-1;
            m_uStringStart++;

            if (bflush) FlushBlock(0);

        } else if (m_bMatchAvailable) {
			/* If there was no match at the previous position, output a
			* single literal. If there was a match but the current match
			* is longer, truncate the previous match to a single literal.
			*/
			bflush = static_cast<int>(TallyLiteral(m_Window[m_uStringStart-1]));
			if (bflush) {
				FlushBlock(0);
			}
			m_uStringStart++;
			m_uLookAhead--;
        } else {
            /* There is no previous match to compare with, wait for
             * the next step to decide.
             */
            m_bMatchAvailable = 1;
            m_uStringStart++;
            m_uLookAhead--;
        }
    }
	if (m_bMatchAvailable) {
		bflush = static_cast<int>(TallyLiteral(m_Window[m_uStringStart-1]));
		m_bMatchAvailable = 0;
	}

	FlushBlock(flush == Z_FINISH);
	return flush == Z_FINISH ? STATE_FINISHDONE : STATE_BLOCKDONE;
}

/* ===========================================================================
 * Send one empty static block to give enough lookahead for inflate.
 * This takes 10 bits, of which 7 may remain in the bit buffer.
 * The current inflate code requires 9 bits of lookahead. If the
 * last two codes for the previous block (real code plus EOB) were coded
 * on 5 bits or less, inflate may have only 5+3 bits of lookahead to decode
 * the last real code. In this case we send two empty static blocks instead
 * of one. (There are no problems if the previous block is stored or fixed.)
 * To simplify the code, we assume the worst case of last real code encoded
 * on one bit only.
 */
void Burger::CompressDeflate::Align(void)
{
    SendBits(STATIC_TREES<<1, 3);
    send_code(END_BLOCK, g_StaticLengthTrees);
    BitIndexFlush();
    /* Of the 10 bits for the empty block, we have already sent
     * (10 - bi_valid) bits. The lookahead for the last real code (before
     * the EOB of the previous block) was thus at least one plus the length
     * of the EOB plus what we have just sent of the empty static block.
     */
    if ((1 + m_uLastEOBLength + 10 - m_uBitIndexValid) < 9) {
        SendBits(STATIC_TREES<<1, 3);
        send_code(END_BLOCK, g_StaticLengthTrees);
        BitIndexFlush();
    }
    m_uLastEOBLength = 7;
}

/* ========================================================================= */
int Burger::CompressDeflate::DeflateEnd(void)
{
	eState status = m_eState;
	if (status != INIT_STATE && status != BUSY_STATE &&
		status != FINISH_STATE) {
		return Z_STREAM_ERROR;
	}
	m_bInitialized = FALSE;
	return status == BUSY_STATE ? Z_DATA_ERROR : Z_OK;
}
/* ===========================================================================
 * Initialize the "longest match" routines for a new zlib stream
 */
void Burger::CompressDeflate::LongestMatchInit(void)
{
    ClearHash();

    /* Set the default configuration parameters:
     */

    m_uStringStart = 0;
    m_iBlockStart = 0L;
    m_uLookAhead = 0;
    m_uMatchLength = m_uPreviousLength = MIN_MATCH-1;
    m_bMatchAvailable = 0;
    m_uInsertHash = 0;
}

/* ========================================================================= */
int Burger::CompressDeflate::DeflateReset(void)
{
    m_iPending = 0;
    m_pPendingOutput = m_PendingBuffer;

    if (m_bNoHeader < 0) {
        m_bNoHeader = 0; /* was set to -1 by deflate(..., Z_FINISH); */
    }
    m_eState = m_bNoHeader ? BUSY_STATE : INIT_STATE;
    m_uAdler = 1;
    m_iLastFlush = Z_NO_FLUSH;

    StaticTreeInit();
    LongestMatchInit();

    return Z_OK;
}


/* ========================================================================= */
int Burger::CompressDeflate::DeflateInit(void)
{
    /* We overlay pending_buf and d_buf+l_buf. This works since the average
     * output size for (length,distance) codes is <= 24 bits. */
	
	m_pPendingOutput = 0;
	m_eState = DEFAULT_STATE;
	m_iPending = 0;
	m_bNoHeader = 0;
	m_iLastFlush = 0;
	m_uInsertHash = 0;
	m_uMatchLength = 0;
	m_iBlockStart = 0;
	m_uPreviousMatch = 0;
	m_bMatchAvailable = 0;
	m_uStringStart = 0;
	m_uMatchStart = 0;
	m_uLookAhead = 0;
	m_uPreviousLength = 0;
	m_bInitialized = TRUE;
	MemoryClear(&m_DynamicLengthTrees,sizeof(m_DynamicLengthTrees));
	MemoryClear(&m_DynamicDistanceTrees,sizeof(m_DynamicDistanceTrees));
	MemoryClear(&m_BitLengthTrees,sizeof(m_BitLengthTrees));

	MemoryClear(&m_LiteralDescription,sizeof(m_LiteralDescription));
	MemoryClear(&m_DistanceDescription,sizeof(m_DistanceDescription));
	MemoryClear(&m_BitLengthDescription,sizeof(m_BitLengthDescription));
	MemoryClear(&m_BitLengthCount,sizeof(m_BitLengthCount));
	MemoryClear(&m_Heap,sizeof(m_Heap));
	MemoryClear(&m_Window,sizeof(m_Window));
	MemoryClear(&m_Previous,sizeof(m_Previous));
	MemoryClear(&m_Head,sizeof(m_Head));
	MemoryClear(&m_Depth,sizeof(m_Depth));
	MemoryClear(&m_DataBuffer,sizeof(m_DataBuffer));
	MemoryClear(&m_LiteralBuffer,sizeof(m_LiteralBuffer));
	MemoryClear(&m_PendingBuffer,sizeof(m_PendingBuffer));

	m_iHeapLength = 0;
	m_iHeapMaximum = 0;

	m_eDataType = Z_UNKNOWN;
	m_bMethod = 0;
	m_uLastLiteral = 0;
	m_uBitIndexValid = 0;
	m_uOptimalLength = 0;
	m_uStaticLength = 0;
	m_uMatches = 0;
	m_uLastEOBLength = 0;
	m_uBitIndexBuffer = 0;
    m_bNoHeader = 0;
    m_bMethod = (Word8)Z_DEFLATED;

    return DeflateReset();
}

/* ========================================================================= */
int Burger::CompressDeflate::PerformDeflate(int flush)
{
	int old_flush; /* value of flush param for previous deflate call */

	if (flush > Z_FINISH || flush < 0) {
		return Z_STREAM_ERROR;
	}

	if ((m_eState == FINISH_STATE && flush != Z_FINISH)) {
		return Z_STREAM_ERROR;
	}

	old_flush = m_iLastFlush;
	m_iLastFlush = flush;

	/* Write the zlib header */
	if (m_eState == INIT_STATE) {

		Word header = (Z_DEFLATED + ((c_uWBits-8)<<4)) << 8;
		Word level_flags = (Z_BEST_COMPRESSION-1) >> 1;

		if (level_flags > 3) level_flags = 3;
		header |= (level_flags << 6);
		if (m_uStringStart != 0) header |= PRESET_DICT;
		header += 31 - (header % 31);

		m_eState = BUSY_STATE;
		OutputBigEndian16(header);

		/* Save the adler32 of the preset dictionary: */
		if (m_uStringStart != 0) {
			OutputBigEndian16((Word)(m_uAdler >> 16));
			OutputBigEndian16((Word)(m_uAdler & 0xffff));
		}
		m_uAdler = 1U;
	}

	/* Flush as much pending output as possible */
	if (m_iPending != 0) {
		FlushPending();

		/* Make sure there is something to do and avoid duplicate consecutive
		* flushes. For repeated and useless calls with Z_FINISH, we keep
		* returning Z_STREAM_END instead of Z_BUFF_ERROR.
		*/
	} else if (m_uInputLength == 0 && flush <= old_flush &&
		flush != Z_FINISH) {
		return Z_BUF_ERROR;
	}

	/* User must not provide more input after the first FINISH: */
	if (m_eState == FINISH_STATE && m_uInputLength != 0) {
		return Z_BUF_ERROR;
	}

	/* Start a new block or continue the current one.
	*/
	if (m_uInputLength != 0 || m_uLookAhead != 0 ||
		(flush != Z_NO_FLUSH && m_eState != FINISH_STATE)) {
		eBlockState bstate;

		bstate = DeflateSlow(flush);

		if (bstate == STATE_FINISHSTARTED || bstate == STATE_FINISHDONE) {
			m_eState = FINISH_STATE;
		}
		if (bstate == STATE_NEEDMORE || bstate == STATE_FINISHSTARTED) {
			return Z_OK;
			/* If flush != Z_NO_FLUSH && avail_out == 0, the next call
			* of deflate should use the same flush parameter to make sure
			* that the flush is complete. So we don't have to output an
			* empty block here, this will be done at next call. This also
			* ensures that for a very small output buffer, we emit at most
			* one empty block.
			*/
		}
		if (bstate == STATE_BLOCKDONE) {
			if (flush == Z_PARTIAL_FLUSH) {
				Align();
			} else { /* FULL_FLUSH or SYNC_FLUSH */
				StoredBlock(NULL, 0, 0);
				/* For a full flush, this empty block will be recognized
				* as a special marker by inflate_sync().
				*/
				if (flush == Z_FULL_FLUSH) {
					ClearHash();             /* forget history */
				}
			}
			FlushPending();
		}
	}
	if (flush != Z_FINISH) return Z_OK;
	if (m_bNoHeader) return Z_STREAM_END;

	/* Write the zlib trailer (adler32) */
	OutputBigEndian16((Word)(m_uAdler >> 16));
	OutputBigEndian16((Word)(m_uAdler & 0xffff));
	FlushPending();
	/* If avail_out is zero, the application will call deflate again
	* to flush the rest.
	*/
	m_bNoHeader = -1; /* write the trailer only once! */
	return m_iPending != 0 ? Z_OK : Z_STREAM_END;
}


/*! ************************************

	\brief Default constructor

	Initializes the cache buffer.

***************************************/

Burger::CompressDeflate::CompressDeflate() :
	Compress(),
	m_bInitialized(FALSE)
{
	m_uSignature = Signature;
}

/*! ************************************

	\brief Reset the RLE compressor

	Resets the cache buffer.

***************************************/

Burger::Compress::eError Burger::CompressDeflate::Init(void)
{
	m_Output.Clear();
	if (DeflateInit()==Z_OK) {
		return Compress::COMPRESS_OKAY;
	}
	return Compress::COMPRESS_OUTOFMEMORY;
}

/*! ************************************

	\brief Compress the input data using RLE

	Compresses the data using RLE and stores the compressed
	data into an OutputMemoryStream

***************************************/

Burger::Compress::eError Burger::CompressDeflate::Process(const void *pInput,WordPtr uInputLength)
{
	if (!m_bInitialized) {
		if (DeflateInit()!=Z_OK) {
			return Compress::COMPRESS_OUTOFMEMORY;
		}
	}
	eError Error = COMPRESS_OKAY;
	if (uInputLength) {
		m_pInput = static_cast<const Word8*>(pInput);
		m_uInputLength = uInputLength;

		int err = PerformDeflate(Z_NO_FLUSH);
		if (err!=Z_OK && err!=Z_STREAM_END) {
			Error = COMPRESS_OUTOFMEMORY;
		}
	}
	return Error;
}

/*! ************************************

	\brief Finalize RLE compression

	If any data has been cached from the compression stream,
	flush it into the output

	\return Zero if no error, non-zero on error

***************************************/

Burger::Compress::eError Burger::CompressDeflate::Finalize(void)
{
	m_pInput = 0;
	m_uInputLength = 0;
	PerformDeflate(Z_FINISH);
	DeflateEnd();
	return COMPRESS_OKAY;
}

/*! ************************************

	\var const Burger::StaticRTTI Burger::CompressDeflate::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
