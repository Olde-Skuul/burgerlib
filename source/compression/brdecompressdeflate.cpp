/***************************************

    Decompression manager version of Deflate

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdecompressdeflate.h"
#include "bradler32.h"
#include "brmemoryfunctions.h"
#include "brglobalmemorymanager.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DecompressDeflate,Burger::Decompress);
#endif

/*! ************************************

	\class Burger::DecompressDeflate
	\brief Decompress Deflate format
	
	Decompress data in Deflate format which is the format
	used by ZLIB (Documented here Burger::CompressDeflate )

	\sa Burger::Decompress and Burger::CompressDeflate

***************************************/

/*! ************************************

	\struct Burger::DecompressDeflate::DeflateHuft_t
	\brief Deflate huffman tree definition 

	Internal structure used by the Deflate decompressor to manage entries
	into the huffman tree. This structure is kept at a size that's a power
	of 2 for quick indexing

	\sa Burger::DecompressDeflate

***************************************/


/***************************************

	Standard defines. Do NOT CHANGE
	under penalty of DEATH!

***************************************/

#if !defined(DOXYGEN)
#define Z_OK 0
#define Z_STREAM_END 1
#define Z_NEED_DICT 2
#define Z_STREAM_ERROR (-2)
#define Z_DATA_ERROR (-3)
#define Z_MEM_ERROR (-4)
#define Z_BUF_ERROR (-5)

/// If BMAX needs to be larger than 16, then h and x[] should be uLong.
#define BMAX 15		///< maximum bit length of any code

#endif

/***************************************

	Masks for lower bits
	And'ing with mask[n] masks the lower n bits

***************************************/

static const uint_t g_DeflateMask[17] = {
	0x0000,0x0001,0x0003,0x0007,0x000F,0x001F,0x003F,0x007F,
	0x00FF,0x01FF,0x03FF,0x07FF,0x0FFF,0x1FFF,0x3FFF,0x7FFF,
	0xFFFF
};

// Tables for deflate from PKZIP's appnote.txt.

// Copy lengths for literal codes 257..285
// see note #13 above about 258
static const uint_t g_DeflateCopyLengths[31] = {
	3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31,
	35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0
};

// Extra bits for literal codes 257..285
// 112==invalid

static const uint_t g_DeflateExtraBits[31] = {
	0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 2, 2,
	3, 3, 3, 3, 4, 4, 4, 4, 5, 5, 5, 5, 0, 112, 112
};

// Copy offsets for distance codes 0..29
static const uint_t g_DeflateDistanceCodes[30] = {
	1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193,
	257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145,
	8193, 12289, 16385, 24577
};

// Extra bits for distance codes
static const uint_t g_DeflateDistanceExtraBits[30] = {
	0, 0, 0, 0, 1, 1, 2, 2, 3, 3, 4, 4, 5, 5, 6, 6,
	7, 7, 8, 8, 9, 9, 10, 10, 11, 11,
	12, 12, 13, 13
};

/*! ************************************

	\brief Table for preset literal codes

***************************************/

const Burger::DecompressDeflate::DeflateHuft_t Burger::DecompressDeflate::s_FixedTreeLiteral[] = {
	{96,7,256},{0,8, 80},{0,8,16},{ 84,8,115},
	{82,7, 31},{0,8,112},{0,8,48},{  0,9,192},
	{80,7, 10},{0,8, 96},{0,8,32},{  0,9,160},
	{ 0,8,  0},{0,8,128},{0,8,64},{  0,9,224},
	{80,7,  6},{0,8, 88},{0,8,24},{  0,9,144},
	{83,7, 59},{0,8,120},{0,8,56},{  0,9,208},
	{81,7, 17},{0,8,104},{0,8,40},{  0,9,176},
	{ 0,8,  8},{0,8,136},{0,8,72},{  0,9,240},
	{80,7,  4},{0,8, 84},{0,8,20},{ 85,8,227},
	{83,7, 43},{0,8,116},{0,8,52},{  0,9,200},
	{81,7, 13},{0,8,100},{0,8,36},{  0,9,168},
	{ 0,8,  4},{0,8,132},{0,8,68},{  0,9,232},
	{80,7,  8},{0,8, 92},{0,8,28},{  0,9,152},
	{84,7, 83},{0,8,124},{0,8,60},{  0,9,216},
	{82,7, 23},{0,8,108},{0,8,44},{  0,9,184},
	{ 0,8, 12},{0,8,140},{0,8,76},{  0,9,248},
	{80,7,  3},{0,8, 82},{0,8,18},{ 85,8,163},
	{83,7, 35},{0,8,114},{0,8,50},{  0,9,196},
	{81,7, 11},{0,8, 98},{0,8,34},{  0,9,164},
	{ 0,8,  2},{0,8,130},{0,8,66},{  0,9,228},
	{80,7,  7},{0,8, 90},{0,8,26},{  0,9,148},
	{84,7, 67},{0,8,122},{0,8,58},{  0,9,212},
	{82,7, 19},{0,8,106},{0,8,42},{  0,9,180},
	{ 0,8, 10},{0,8,138},{0,8,74},{  0,9,244},
	{80,7,  5},{0,8, 86},{0,8,22},{192,8,  0},
	{83,7, 51},{0,8,118},{0,8,54},{  0,9,204},
	{81,7, 15},{0,8,102},{0,8,38},{  0,9,172},
	{ 0,8,  6},{0,8,134},{0,8,70},{  0,9,236},
	{80,7,  9},{0,8, 94},{0,8,30},{  0,9,156},
	{84,7, 99},{0,8,126},{0,8,62},{  0,9,220},
	{82,7, 27},{0,8,110},{0,8,46},{  0,9,188},
	{ 0,8, 14},{0,8,142},{0,8,78},{  0,9,252},
	{96,7,256},{0,8, 81},{0,8,17},{ 85,8,131},
	{82,7, 31},{0,8,113},{0,8,49},{  0,9,194},
	{80,7, 10},{0,8, 97},{0,8,33},{  0,9,162},
	{ 0,8,  1},{0,8,129},{0,8,65},{  0,9,226},
	{80,7,  6},{0,8, 89},{0,8,25},{  0,9,146},
	{83,7, 59},{0,8,121},{0,8,57},{  0,9,210},
	{81,7, 17},{0,8,105},{0,8,41},{  0,9,178},
	{ 0,8,  9},{0,8,137},{0,8,73},{  0,9,242},
	{80,7,  4},{0,8, 85},{0,8,21},{ 80,8,258},
	{83,7, 43},{0,8,117},{0,8,53},{  0,9,202},
	{81,7, 13},{0,8,101},{0,8,37},{  0,9,170},
	{ 0,8,  5},{0,8,133},{0,8,69},{  0,9,234},
	{80,7,  8},{0,8, 93},{0,8,29},{  0,9,154},
	{84,7, 83},{0,8,125},{0,8,61},{  0,9,218},
	{82,7, 23},{0,8,109},{0,8,45},{  0,9,186},
	{ 0,8, 13},{0,8,141},{0,8,77},{  0,9,250},
	{80,7,  3},{0,8, 83},{0,8,19},{ 85,8,195},
	{83,7, 35},{0,8,115},{0,8,51},{  0,9,198},
	{81,7, 11},{0,8, 99},{0,8,35},{  0,9,166},
	{ 0,8,  3},{0,8,131},{0,8,67},{  0,9,230},
	{80,7,  7},{0,8, 91},{0,8,27},{  0,9,150},
	{84,7, 67},{0,8,123},{0,8,59},{  0,9,214},
	{82,7, 19},{0,8,107},{0,8,43},{  0,9,182},
	{ 0,8, 11},{0,8,139},{0,8,75},{  0,9,246},
	{80,7,  5},{0,8, 87},{0,8,23},{192,8,  0},
	{83,7, 51},{0,8,119},{0,8,55},{  0,9,206},
	{81,7, 15},{0,8,103},{0,8,39},{  0,9,174},
	{ 0,8,  7},{0,8,135},{0,8,71},{  0,9,238},
	{80,7,  9},{0,8, 95},{0,8,31},{  0,9,158},
	{84,7, 99},{0,8,127},{0,8,63},{  0,9,222},
	{82,7, 27},{0,8,111},{0,8,47},{  0,9,190},
	{ 0,8, 15},{0,8,143},{0,8,79},{  0,9,254},
	{96,7,256},{0,8, 80},{0,8,16},{ 84,8,115},
	{82,7, 31},{0,8,112},{0,8,48},{  0,9,193},
	{80,7, 10},{0,8, 96},{0,8,32},{  0,9,161},
	{ 0,8,  0},{0,8,128},{0,8,64},{  0,9,225},
	{80,7,  6},{0,8, 88},{0,8,24},{  0,9,145},
	{83,7, 59},{0,8,120},{0,8,56},{  0,9,209},
	{81,7, 17},{0,8,104},{0,8,40},{  0,9,177},
	{ 0,8,  8},{0,8,136},{0,8,72},{  0,9,241},
	{80,7,  4},{0,8, 84},{0,8,20},{ 85,8,227},
	{83,7, 43},{0,8,116},{0,8,52},{  0,9,201},
	{81,7, 13},{0,8,100},{0,8,36},{  0,9,169},
	{ 0,8,  4},{0,8,132},{0,8,68},{  0,9,233},
	{80,7,  8},{0,8, 92},{0,8,28},{  0,9,153},
	{84,7, 83},{0,8,124},{0,8,60},{  0,9,217},
	{82,7, 23},{0,8,108},{0,8,44},{  0,9,185},
	{ 0,8, 12},{0,8,140},{0,8,76},{  0,9,249},
	{80,7,  3},{0,8, 82},{0,8,18},{ 85,8,163},
	{83,7, 35},{0,8,114},{0,8,50},{  0,9,197},
	{81,7, 11},{0,8, 98},{0,8,34},{  0,9,165},
	{ 0,8,  2},{0,8,130},{0,8,66},{  0,9,229},
	{80,7,  7},{0,8, 90},{0,8,26},{  0,9,149},
	{84,7, 67},{0,8,122},{0,8,58},{  0,9,213},
	{82,7, 19},{0,8,106},{0,8,42},{  0,9,181},
	{ 0,8, 10},{0,8,138},{0,8,74},{  0,9,245},
	{80,7,  5},{0,8, 86},{0,8,22},{192,8,  0},
	{83,7, 51},{0,8,118},{0,8,54},{  0,9,205},
	{81,7, 15},{0,8,102},{0,8,38},{  0,9,173},
	{ 0,8,  6},{0,8,134},{0,8,70},{  0,9,237},
	{80,7,  9},{0,8, 94},{0,8,30},{  0,9,157},
	{84,7, 99},{0,8,126},{0,8,62},{  0,9,221},
	{82,7, 27},{0,8,110},{0,8,46},{  0,9,189},
	{ 0,8, 14},{0,8,142},{0,8,78},{  0,9,253},
	{96,7,256},{0,8, 81},{0,8,17},{ 85,8,131},
	{82,7, 31},{0,8,113},{0,8,49},{  0,9,195},
	{80,7, 10},{0,8, 97},{0,8,33},{  0,9,163},
	{ 0,8,  1},{0,8,129},{0,8,65},{  0,9,227},
	{80,7,  6},{0,8, 89},{0,8,25},{  0,9,147},
	{83,7, 59},{0,8,121},{0,8,57},{  0,9,211},
	{81,7, 17},{0,8,105},{0,8,41},{  0,9,179},
	{ 0,8,  9},{0,8,137},{0,8,73},{  0,9,243},
	{80,7,  4},{0,8, 85},{0,8,21},{ 80,8,258},
	{83,7, 43},{0,8,117},{0,8,53},{  0,9,203},
	{81,7, 13},{0,8,101},{0,8,37},{  0,9,171},
	{ 0,8,  5},{0,8,133},{0,8,69},{  0,9,235},
	{80,7,  8},{0,8, 93},{0,8,29},{  0,9,155},
	{84,7, 83},{0,8,125},{0,8,61},{  0,9,219},
	{82,7, 23},{0,8,109},{0,8,45},{  0,9,187},
	{ 0,8, 13},{0,8,141},{0,8,77},{  0,9,251},
	{80,7,  3},{0,8, 83},{0,8,19},{ 85,8,195},
	{83,7, 35},{0,8,115},{0,8,51},{  0,9,199},
	{81,7, 11},{0,8, 99},{0,8,35},{  0,9,167},
	{ 0,8,  3},{0,8,131},{0,8,67},{  0,9,231},
	{80,7,  7},{0,8, 91},{0,8,27},{  0,9,151},
	{84,7, 67},{0,8,123},{0,8,59},{  0,9,215},
	{82,7, 19},{0,8,107},{0,8,43},{  0,9,183},
	{ 0,8, 11},{0,8,139},{0,8,75},{  0,9,247},
	{80,7,  5},{0,8, 87},{0,8,23},{192,8,  0},
	{83,7, 51},{0,8,119},{0,8,55},{  0,9,207},
	{81,7, 15},{0,8,103},{0,8,39},{  0,9,175},
	{ 0,8,  7},{0,8,135},{0,8,71},{  0,9,239},
	{80,7,  9},{0,8, 95},{0,8,31},{  0,9,159},
	{84,7, 99},{0,8,127},{0,8,63},{  0,9,223},
	{82,7, 27},{0,8,111},{0,8,47},{  0,9,191},
	{ 0,8, 15},{0,8,143},{0,8,79},{  0,9,255}
};

/*! ************************************

	\brief Table for preset distance codes

***************************************/

const Burger::DecompressDeflate::DeflateHuft_t Burger::DecompressDeflate::s_FixedTreeDistance[] = {
	{80,5, 1}, {87,5, 257}, {83,5, 17}, { 91,5, 4097},
	{81,5, 5}, {89,5,1025}, {85,5, 65}, { 93,5,16385},
	{80,5, 3}, {88,5, 513}, {84,5, 33}, { 92,5, 8193},
	{82,5, 9}, {90,5,2049}, {86,5,129}, {192,5,24577},
	{80,5, 2}, {87,5, 385}, {83,5, 25}, { 91,5, 6145},
	{81,5, 7}, {89,5,1537}, {85,5, 97}, { 93,5,24577},
	{80,5, 4}, {88,5, 769}, {84,5, 49}, { 92,5,12289},
	{82,5,13}, {90,5,3073}, {86,5,193}, {192,5,24577}
};

// Order of the bit length code lengths
static const uint_t g_DeflateBorder[] = {
	16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15
};

/*! ************************************

	\brief Copy as much as possible from the sliding window to the output area

	\param iErrorCode Current error code
	\return Adjusted error code

***************************************/

int Burger::DecompressDeflate::Flush(int iErrorCode)
{
	// Local copies of source and destination pointers
	uint8_t *pOutput = m_pOutput;
	const uint8_t *pWindowRead = m_pWindowRead;

	// Compute number of bytes to copy as far as end of window
	uintptr_t uCount = static_cast<uintptr_t>((pWindowRead <= m_pWindowWrite ? m_pWindowWrite : &m_WindowBuffer[1<<MAX_WBITS]) - pWindowRead);
	if (uCount > m_uOutputChunkLength) {
		uCount = m_uOutputChunkLength;
	}
	// If there was a memory error, recover

	if (uCount && (iErrorCode == Z_BUF_ERROR)) {
		iErrorCode = Z_OK;
	}
	// Update counters
	m_uOutputChunkLength -= uCount;

	// Update check information
	m_uAlder32Checksum = CalcAdler32(pWindowRead,uCount,m_uAlder32Checksum);

	// Copy as far as end of window
	MemoryCopy(pOutput,pWindowRead,uCount);
	pOutput += uCount;
	pWindowRead += uCount;

	// See if more to copy at beginning of window
	if (pWindowRead == &m_WindowBuffer[1<<MAX_WBITS]) {
		// Wrap pointers
		pWindowRead = m_WindowBuffer;
		if (m_pWindowWrite == &m_WindowBuffer[1<<MAX_WBITS]) {
			m_pWindowWrite = m_WindowBuffer;
		}

		// Compute bytes to copy */
		uCount = static_cast<uintptr_t>(m_pWindowWrite - pWindowRead);
		if (uCount > m_uOutputChunkLength) {
			uCount = m_uOutputChunkLength;
		}
		if (uCount && iErrorCode == Z_BUF_ERROR) {
 			iErrorCode = Z_OK;
		}
		// Update counters
		m_uOutputChunkLength -= uCount;

		// Update check information
		m_uAlder32Checksum = CalcAdler32(pWindowRead,uCount,m_uAlder32Checksum);

		// Copy into the output
		MemoryCopy(pOutput,pWindowRead,uCount);
		pOutput += uCount;
		pWindowRead += uCount;
	}

	// Update pointers
	m_pOutput = pOutput;
	m_pWindowRead = pWindowRead;
	return iErrorCode;
}

/*! ************************************

	\brief Optimized decompression code

	Called with number of bytes left to write in window at least 258
	(the maximum string length) and number of input bytes available
	at least ten. The ten bytes are six bytes for the longest length/
	distance pair plus four bytes for overloading the bit buffer.

	\param uBitLength Length of the bit stream
	\param uBitDistance Bit distance for huffman decoding
	\param pHuffmanLength Pointer to the huffman table for length entries
	\param pHuffmanDistance Pointer to the huffman table for distance entries

	\return Error code

***************************************/

int Burger::DecompressDeflate::Fast(uint_t uBitLength,uint_t uBitDistance,const DeflateHuft_t *pHuffmanLength,const DeflateHuft_t *pHuffmanDistance)
{
	// load input, output, bit values
	const uint8_t *pInput=m_pInput;
	uintptr_t uInputLength=m_uInputChunkLength;
	uint32_t uBitBucket=m_uBitBucket;
	uint_t uBitCount=m_uBitCount;
	uint8_t *pWindowWrite=m_pWindowWrite;
	uintptr_t uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?(m_pWindowRead-pWindowWrite-1):&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);

	// initialize masks
	uint_t uMaskLength = g_DeflateMask[uBitLength];
	uint_t uMaskDistance = g_DeflateMask[uBitDistance];

	int iErrorCode;
	// do until not enough input or output space for fast loop
	do {		
		// assume called with m >= 258 && n >= 10
		// get literal/length code
		while (uBitCount<20) {
			--uInputLength;
			uBitBucket|=(static_cast<uint32_t>(pInput[0]))<<uBitCount;
			++pInput;
			uBitCount+=8;
		}
		// max bits for literal/length code
		const DeflateHuft_t *pHuffman = pHuffmanLength + (uBitBucket & uMaskLength);
		uint_t uExtra = pHuffman->m_bExtraOperation;
		if (!uExtra) {
			// Literal code
			uBitBucket>>=pHuffman->m_bBitCount;
			uBitCount-=pHuffman->m_bBitCount;
			pWindowWrite[0] = static_cast<uint8_t>(pHuffman->m_uBase);
			++pWindowWrite;
			--uRemainingWindow;
			continue;
		}
		// Length code
		for (;;) {
			uBitBucket>>=pHuffman->m_bBitCount;
			uBitCount-=pHuffman->m_bBitCount;
			if (uExtra & 0x10) {
				// Get extra bits for length
				uExtra &= 0xF;
				uintptr_t uBytesToCopy = pHuffman->m_uBase + (uBitBucket & g_DeflateMask[uExtra]);
				uBitBucket>>=uExtra;
				uBitCount-=uExtra;

				// Decode distance base of block to copy
				while (uBitCount<15) {
					--uInputLength;
					uBitBucket|=static_cast<uint32_t>(pInput[0])<<uBitCount;
					++pInput;
					uBitCount+=8;
				}
				// max bits for distance code
				pHuffman = pHuffmanDistance + (uBitBucket & uMaskDistance);
				uExtra = pHuffman->m_bExtraOperation;
				for (;;) {
					uBitBucket>>=pHuffman->m_bBitCount;
					uBitCount-=pHuffman->m_bBitCount;
					if (uExtra & 0x10) {
						// get extra bits to add to distance base
						uExtra &= 0xF;
						while (uBitCount<uExtra) {
							--uInputLength;
							uBitBucket|=static_cast<uint32_t>(pInput[0])<<uBitCount;
							++pInput;
							uBitCount+=8;
						}
						// get extra bits (up to 13)
						uintptr_t uDistance = pHuffman->m_uBase + (uBitBucket & g_DeflateMask[uExtra]);
						uBitBucket>>= uExtra;
						uBitCount-=uExtra;

						// do the copy
						uRemainingWindow -= uBytesToCopy;
						const uint8_t *pSourceCopy;
						// offset before dest
						if (static_cast<uintptr_t>(pWindowWrite - m_WindowBuffer) >= uDistance) {
							// just copy
							pSourceCopy = pWindowWrite - uDistance;
							pWindowWrite[0] = pSourceCopy[0];		// minimum count is three,
							pWindowWrite[1] = pSourceCopy[1];		// so unroll loop a little
							pSourceCopy+=2;
							pWindowWrite+=2;
							uBytesToCopy-=2;				
						} else {
							// else offset after destination
							// bytes from offset to end
							uExtra = static_cast<uint_t>(uDistance - static_cast<uintptr_t>(pWindowWrite - m_WindowBuffer));
							pSourceCopy = &m_WindowBuffer[1<<MAX_WBITS] - uExtra;	// pointer to offset
							if (uBytesToCopy > uExtra) {		// if source crosses,
								uBytesToCopy -= uExtra;			// copy to end of window
								do {
									pWindowWrite[0] = pSourceCopy[0];
									++pSourceCopy;
									++pWindowWrite;
								} while (--uExtra);
								pSourceCopy = m_WindowBuffer;	// copy rest from start of window
							}
						}
						// copy all or what's left
						do {
							pWindowWrite[0] = pSourceCopy[0];
							++pSourceCopy;
							++pWindowWrite;
						} while (--uBytesToCopy);
						break;
					}
					if (uExtra & 0x40) {
						iErrorCode = Z_DATA_ERROR;
						goto ByeBye;
					}
					pHuffman = pHuffman + pHuffman->m_uBase + (uBitBucket & g_DeflateMask[uExtra]);
					uExtra = pHuffman->m_bExtraOperation;
				}
				break;
			}
			if (uExtra & 0x40) {
				if (uExtra & 0x20) {
					iErrorCode = Z_STREAM_END;
					goto ByeBye;
				}
				iErrorCode = Z_DATA_ERROR;
				goto ByeBye;
			}
			pHuffman = pHuffman + pHuffman->m_uBase + (uBitBucket & g_DeflateMask[uExtra]);
			uExtra = pHuffman->m_bExtraOperation;
			if (!uExtra) {
				uBitBucket>>=pHuffman->m_bBitCount;
				uBitCount-=pHuffman->m_bBitCount;
				pWindowWrite[0] = static_cast<uint8_t>(pHuffman->m_uBase);
				++pWindowWrite;
				--uRemainingWindow;
				break;
			}
		}
	} while ((uRemainingWindow >= 258) && (uInputLength >= 10));

	// Not enough input or output--restore pointers and return
	iErrorCode = Z_OK;
ByeBye:;
	uintptr_t uUsed = static_cast<uintptr_t>(m_uInputChunkLength-uInputLength);
	uUsed = (uBitCount>>3U) < uUsed ? (uBitCount>>3U) : uUsed;
	uInputLength+=uUsed;
	pInput-=uUsed;
	uBitCount-=static_cast<uint_t>(uUsed)<<3U;
	m_uBitBucket=uBitBucket;
	m_uBitCount=uBitCount;
	m_uInputChunkLength=uInputLength;
	m_pInput=pInput;
	m_pWindowWrite=pWindowWrite;
	return iErrorCode;
}

/*! ************************************

	\brief Process the decompression codes

	\param iErrorCode Current error code
	\return Adjusted error code

***************************************/

int Burger::DecompressDeflate::ProcessCodes(int iErrorCode)
{
	// copy input/output information to locals (UPDATE macro restores)
	const uint8_t *pInput=m_pInput;
	uintptr_t uInputLength=m_uInputChunkLength;
	uint32_t uBitBucket=m_uBitBucket;
	uint_t uBitCount=m_uBitCount;
	uint8_t *pWindowWrite=m_pWindowWrite;
	uintptr_t uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);

	// Process input and output based on current state
	for (;;) {
		uint_t j;
		const DeflateHuft_t *pHuffman;	// temporary pointer
		uint_t uExtra;					// extra bits or operation
		const uint8_t *pSourceCopy;		// pointer to copy strings from
		switch (m_eCodesMode) {
		// Starting?
		case CODES_START:
			// Fast decompression?
			if ((uRemainingWindow >= 258) && (uInputLength >= 10)) {
				m_uBitBucket=uBitBucket;
				m_uBitCount=uBitCount;
				m_uInputChunkLength=uInputLength;
				m_pInput=pInput;
				m_pWindowWrite=pWindowWrite;
				iErrorCode = Fast(m_bCodeLengthBits,m_bCodeDistanceBits,m_pCodeTreeLength,m_pCodeTreeDistance);
				pInput=m_pInput;
				uInputLength=m_uInputChunkLength;
				uBitBucket=m_uBitBucket;
				uBitCount=m_uBitCount;
				pWindowWrite=m_pWindowWrite;
				uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
				if (iErrorCode != Z_OK) {
					m_eCodesMode = (iErrorCode == Z_STREAM_END) ? CODES_WASH : CODES_ABORT;
					break;
				}
			}
			m_uCodeTreeNeed = m_bCodeLengthBits;
			m_pCodeTree = m_pCodeTreeLength;
			m_eCodesMode = CODES_LEN;
		case CODES_LEN:			/* i: get length/literal/eob next */
			j = m_uCodeTreeNeed;
			while (uBitCount<j) {
				if (!uInputLength) {
					goto Abort;
				}
				iErrorCode=Z_OK;
				--uInputLength;
				uBitBucket|=static_cast<uint32_t>(pInput[0])<<uBitCount;
				++pInput;
				uBitCount+=8;
			}
			pHuffman = m_pCodeTree + ((uint_t)uBitBucket & g_DeflateMask[j]);
			uBitBucket>>=(pHuffman->m_bBitCount);
			uBitCount-=(pHuffman->m_bBitCount);
			uExtra = (uint_t)(pHuffman->m_bExtraOperation);
			if (uExtra == 0) {				/* literal */
				m_uCodeLiteral = pHuffman->m_uBase;
				m_eCodesMode = CODES_LITERAL;
				break;
			}
			if (uExtra & 16) {				/* length */
				m_uCodeCopyGet = uExtra & 15;
				m_uCodeLength = pHuffman->m_uBase;
				m_eCodesMode = CODES_LENEXT;
				break;
			}
			if ((uExtra & 64) == 0) {		/* next table */
				m_uCodeTreeNeed = uExtra;
				m_pCodeTree = pHuffman + pHuffman->m_uBase;
				break;
			}
			if (uExtra & 32) {				/* end of block */
				m_eCodesMode = CODES_WASH;
				break;
			}
			m_eCodesMode = CODES_ABORT;			/* invalid code */
			iErrorCode = Z_DATA_ERROR;
			goto Abort;
		case CODES_LENEXT:			/* i: getting length extra (have base) */
			j = m_uCodeCopyGet;
			while(uBitCount<j) {
				if (!uInputLength) {
					goto Abort;
				}
				iErrorCode=Z_OK;
				--uInputLength;
				uBitBucket|=((uint32_t)(*pInput++))<<uBitCount;
				uBitCount+=8;
			}
			m_uCodeLength += (uBitBucket & g_DeflateMask[j]);
			uBitBucket>>=j;
			uBitCount-=j;
			m_uCodeTreeNeed = m_bCodeDistanceBits;
			m_pCodeTree = m_pCodeTreeDistance;
			m_eCodesMode = CODES_DIST;
		case CODES_DIST:			/* i: get distance next */
			j = m_uCodeTreeNeed;
			while(uBitCount<j) {
				if(!uInputLength) {
					goto Abort;
				}
				iErrorCode=Z_OK;
				--uInputLength;
				uBitBucket|=((uint32_t)(*pInput++))<<uBitCount;
				uBitCount+=8;
			}
			pHuffman = m_pCodeTree + (uBitBucket & g_DeflateMask[j]);
			uBitBucket>>=(pHuffman->m_bBitCount);
			uBitCount-=(pHuffman->m_bBitCount);
			uExtra = pHuffman->m_bExtraOperation;
			if (uExtra & 16) {				/* distance */
				m_uCodeCopyGet = uExtra & 15;
				m_uCodeCopyDistance = pHuffman->m_uBase;
				m_eCodesMode = CODES_DISTEXT;
				break;
			}
			if ((uExtra & 64) == 0) {		/* next table */
				m_uCodeTreeNeed = uExtra;
				m_pCodeTree = pHuffman + pHuffman->m_uBase;
				break;
			}
			m_eCodesMode = CODES_ABORT;			/* invalid code */
			iErrorCode = Z_DATA_ERROR;
			goto Abort;

		case CODES_DISTEXT:			/* i: getting distance extra */
			j = m_uCodeCopyGet;
			while (uBitCount<j) {
				if (!uInputLength) {
					goto Abort;
				}
				iErrorCode=Z_OK;
				--uInputLength;
				uBitBucket|=((uint32_t)(*pInput++))<<uBitCount;
				uBitCount+=8;
			}
			m_uCodeCopyDistance += (uBitBucket & g_DeflateMask[j]);
			uBitBucket>>=j;
			uBitCount-=j;
			m_eCodesMode = CODES_COPY;
		case CODES_COPY:			/* o: copying bytes in window, waiting for space */
			pSourceCopy = (uint_t)(pWindowWrite - m_WindowBuffer) < m_uCodeCopyDistance ?
			&m_WindowBuffer[1<<MAX_WBITS] - (m_uCodeCopyDistance - (pWindowWrite - m_WindowBuffer)) :
				pWindowWrite - m_uCodeCopyDistance;
			while (m_uCodeLength) {
				if (!uRemainingWindow) {
					if((pWindowWrite==&m_WindowBuffer[1<<MAX_WBITS])&&(m_pWindowRead!=m_WindowBuffer)) {
						pWindowWrite=m_WindowBuffer;
						uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
					}
					if(!uRemainingWindow) {
						m_pWindowWrite=pWindowWrite;
						iErrorCode=Flush(iErrorCode);
						pWindowWrite=m_pWindowWrite;
						uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
						if(pWindowWrite==&m_WindowBuffer[1<<MAX_WBITS]&&m_pWindowRead!=m_WindowBuffer){
							pWindowWrite=m_WindowBuffer;
							uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
						}
						if (!uRemainingWindow) {
							goto Abort;
						}
					}
				}
				iErrorCode=Z_OK;
				*pWindowWrite++=(uint8_t)(*pSourceCopy++);
				uRemainingWindow--;
				if (pSourceCopy == &m_WindowBuffer[1<<MAX_WBITS]) {
					pSourceCopy = m_WindowBuffer;
				}
				m_uCodeLength--;
			}
			m_eCodesMode = CODES_START;
			break;
		case CODES_LITERAL:			/* o: got literal, waiting for output space */
			if (!uRemainingWindow){
				if (pWindowWrite==&m_WindowBuffer[1<<MAX_WBITS]&&m_pWindowRead!=m_WindowBuffer){
					pWindowWrite=m_WindowBuffer;
					uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
				}
				if (!uRemainingWindow){
					m_pWindowWrite=pWindowWrite;
					iErrorCode=Flush(iErrorCode);
					pWindowWrite=m_pWindowWrite;
					uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
					if (pWindowWrite==&m_WindowBuffer[1<<MAX_WBITS]&&m_pWindowRead!=m_WindowBuffer){
						pWindowWrite=m_WindowBuffer;
						uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
					}
					if (!uRemainingWindow) {
						goto Abort;
					}
				}
			}
			iErrorCode=Z_OK;
			pWindowWrite[0]=static_cast<uint8_t>(m_uCodeLiteral);
			++pWindowWrite;
			--uRemainingWindow;
			m_eCodesMode = CODES_START;
			break;
		case CODES_WASH:				/* o: got eob, possibly more output */
			if (uBitCount > 7) {		/* return unused byte, if any */
				uBitCount -= 8;
				uInputLength++;
				pInput--;			/* can always return one */
			}
			m_pWindowWrite=pWindowWrite;
			iErrorCode=Flush(iErrorCode);
			pWindowWrite=m_pWindowWrite;
			uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
			if (m_pWindowRead != m_pWindowWrite) {
				goto Abort;
			}
			m_eCodesMode = CODES_END;
		case CODES_END:
			iErrorCode = Z_STREAM_END;
			goto Abort;
		case CODES_ABORT:			/* x: got error */
			iErrorCode = Z_DATA_ERROR;
			goto Abort;
		default:
			iErrorCode = Z_STREAM_ERROR;
			goto Abort;
		}
	}
Abort:;
	m_uBitBucket=uBitBucket;
	m_uBitCount=uBitCount;
	m_uInputChunkLength=uInputLength;
	m_pInput=pInput;
	m_pWindowWrite=pWindowWrite;
	return Flush(iErrorCode);
}

/*! ************************************

	\brief Reset the code state

	\param bCodeLengthBits Default code length bits
	\param bCodeDistanceBits Default code distance bits
	\param pCodeTreeLength Pointer to the code length huffman tree
	\param pCodeTreeDistance Pointer to the code distance huffman tree

***************************************/

void Burger::DecompressDeflate::CodesReset(uint_t bCodeLengthBits,uint_t bCodeDistanceBits,const DeflateHuft_t *pCodeTreeLength,const DeflateHuft_t *pCodeTreeDistance)
{
	m_eCodesMode = CODES_START;
	m_uCodeLength = 0;
	m_pCodeTree = NULL;
	m_uCodeTreeNeed = 0;
	m_uCodeLiteral = 0;
	m_uCodeCopyGet = 0;
	m_uCodeCopyDistance = 0;
	m_pCodeTreeLength = pCodeTreeLength;
	m_pCodeTreeDistance = pCodeTreeDistance;
	m_bCodeLengthBits = static_cast<uint8_t>(bCodeLengthBits);
	m_bCodeDistanceBits = static_cast<uint8_t>(bCodeDistanceBits);
}


/*
	Huffman code decoding is performed using a multi-level table lookup.
	The fastest way to decode is to simply build a lookup table whose
	size is determined by the longest code. However, the time it takes
	to build this table can also be a factor if the data being decoded
	is not very long. The most common codes are necessarily the
	shortest codes, so those codes dominate the decoding time, and hence
	the speed. The idea is you can have a shorter table that decodes the
	shorter, more probable codes, and then point to subsidiary tables for
	the longer codes. The time it costs to decode the longer codes is
	then traded against the time it takes to make longer tables.

	This results of this trade are in the variables lbits and dbits
	below. lbits is the number of bits the first level table for literal/
	length codes can decode in one step, and dbits is the same thing for
	the distance codes. Subsequent tables are also less than or equal to
	those sizes. These values may be adjusted either when all of the
	codes are shorter than that, in which case the longest code length in
	bits is used, or when the shortest code is *longer* than the requested
	table size, in which case the length of the shortest code in bits is
	used.

	There are two different values for the two tables, since they code a
	different number of possibilities each. The literal/length table
	codes 286 possible values, or in a flat code, a little over eight
	bits. The distance table codes 30 possible values, or a little less
	than five bits, flat. The optimum values for speed end up being
	about one bit more than those, so lbits is 8+1 and dbits is 5+1.
	The optimum values may differ though from machine to machine, and
	possibly even between compilers. Your mileage may vary.
*/

/*! ************************************

	\brief Create a huffman tree

	\param pSampleCounts Pointer to the sample count array
	\param uNumberSamples Number of sample count entries
	\param uMaxSample Maximum value of a sample
	\param pDefaultLengths Pointer to the default length
	\param pDefaultBits Pointer to the default bits
	\param ppNewTree Pointer to receive the new tree
	\param pNewTreeSize Pointer to receive the size of the new tree
	\param pExistingTree Pointer to an existing huffman tree
	\param pHuffmanCount Pointer to receive the adjusted huffman tree size
	\param pWorkArea Pointer to a work buffer

***************************************/

int Burger::DecompressDeflate::BuildHuffmanTrees(const uint_t *pSampleCounts,uint_t uNumberSamples,uint_t uMaxSample,const uint_t *pDefaultLengths,const uint_t *pDefaultBits,
	DeflateHuft_t **ppNewTree,uint_t *pNewTreeSize,DeflateHuft_t *pExistingTree,uint_t *pHuffmanCount,uint_t *pWorkArea)
/* Given a list of code lengths and a maximum table size, make a set of
	tables to decode that set of codes. Return Z_OK on success, Z_BUF_ERROR
	if the given code set is incomplete (the tables are still built in this
	case), Z_DATA_ERROR if the input is invalid (an over-subscribed set of
	lengths), or Z_MEM_ERROR if not enough memory. */
{
	uint_t Counts[BMAX+1];				/* bit length count table */
	DeflateHuft_t *HuftPtrs[BMAX];	// table stack
	uint_t x[BMAX+1];				/* bit offsets, then code stack */
	DeflateHuft_t HuftEntry;	// table entry for structure assignment

	/* Generate counts for each bit length */
	Counts[0] = 0;				/* clear c[]--assume BMAX+1 is 16 */
	Counts[1] = 0;
	Counts[2] = 0;
	Counts[3] = 0;
	Counts[4] = 0;
	Counts[5] = 0;
	Counts[6] = 0;
	Counts[7] = 0;
	Counts[8] = 0;
	Counts[9] = 0;
	Counts[10] = 0;
	Counts[11] = 0;
	Counts[12] = 0;
	Counts[13] = 0;
	Counts[14] = 0;
	Counts[15] = 0;
	const uint_t *p = pSampleCounts;
	uint_t i = uNumberSamples;
	do {
		Counts[p[0]]++;				/* assume all entries <= BMAX */
		++p;
	} while (--i);
	if (Counts[0] == uNumberSamples) {			/* null input--all zero length codes */
		*ppNewTree = NULL;
		*pNewTreeSize = 0;
		return Z_OK;
	}


	/* Find minimum and maximum length, bound *m by those */
	int l = static_cast<int>(*pNewTreeSize);
	uint_t j = 1;
	do {
		if (Counts[j]) {
			break;
		}
	} while (++j<=BMAX);

	int k = static_cast<int>(j);						/* minimum code length */
	if ((uint_t)l < j) {
		l = static_cast<int>(j);
	}
	i = BMAX;
	do {
		if (Counts[i]) {
			break;
		}
	} while (--i);
	int g = static_cast<int>(i);						/* maximum code length */
	if ((uint_t)l > i) {
		l = static_cast<int>(i);
	}
	*pNewTreeSize = static_cast<uint_t>(l);


	/* Adjust last length count to fill out codes, if needed */
	int y;						/* number of dummy codes added */
	for (y = 1 << j; j < i; j++, y <<= 1) {
		if ((y -= Counts[j]) < 0) {
			return Z_DATA_ERROR;
		}
	}
	if ((y -= Counts[i]) < 0) {
		return Z_DATA_ERROR;
	}
	Counts[i] += y;


	/* Generate starting offsets into the value table for each length */
	x[1] = j = 0;
	p = Counts + 1;
	uint_t *xp = x + 2;
	while (--i) {					/* note that i == g from above */
		xp[0] = (j += p[0]);
		++p;
		++xp;
	}


	/* Make a table of values in order of bit lengths */
	p = pSampleCounts;
	i = 0;
	do {
		if ((j = p[0]) != 0) {
			pWorkArea[x[j]++] = i;
		}
		++p;
	} while (++i < uNumberSamples);
	uNumberSamples = x[g];					/* set n to length of v */

	/* Generate the Huffman codes and for each, make the table entries */
	x[0] = i = 0;					/* first Huffman code is zero */
	p = pWorkArea;						/* grab values in bit order */
	int h = -1;						/* no tables yet--level -1 */
	int w = -l;						/* bits decoded == (l * h) */
	HuftPtrs[0] = NULL;		/* just to keep compilers happy */
	DeflateHuft_t *q = NULL;		/* ditto */
	uint_t z = 0;						/* ditto */

	uint_t f;						/* i repeats in table every f entries */
	/* go through the bit lengths (k already is bits in shortest code) */
	for (; k <= g; k++) {
		uint_t a = Counts[k];
		while (a--) {
		/* here i is the Huffman code of length k bits for value *p */
		/* make tables up to required level */
			while (k > w + l) {
				h++;
				w += l;				/* previous table always l bits */

				/* compute minimum size table less than or equal to l bits */
				z = static_cast<uint_t>(g - w);
				z = z > (uint_t)l ? l : z;			/* table size upper limit */
				if ((f = static_cast<uint_t>(1 << (j = static_cast<uint_t>(k - w)))) > a + 1) {		/* try a k-w bit table */
					/* too few codes for k-w bit table */
					f -= a + 1;				/* deduct codes from patterns left */
					xp = Counts + k;
					if (j < z)
						while (++j < z) {	/* try smaller tables up to z bits */
							if ((f <<= 1) <= *++xp)
								break;			/* enough codes to use up j bits */
							f -= *xp;			/* else deduct codes from patterns */
						}
				}
				z = 1U << j;				/* table entries for j-bit table */

				/* allocate new table */
				if (*pHuffmanCount + z > MAXTREEDYNAMICSIZE) {			/* (note: doesn't matter for fixed) */
					return Z_MEM_ERROR;		/* not enough memory */
				}
				q = pExistingTree + *pHuffmanCount;
				HuftPtrs[h] = q;
				*pHuffmanCount += z;

				/* connect to last table, if there is one */
				if (h) {
					x[h] = i;				/* save pattern for backing up */
					HuftEntry.m_bBitCount = (uint8_t)l;		/* bits to dump before this table */
					HuftEntry.m_bExtraOperation = (uint8_t)j;		/* bits in this table */
					j = i >> (w - l);
					HuftEntry.m_uBase = (uint16_t)(q - HuftPtrs[h-1] - j);	/* offset to this table */
					HuftPtrs[h-1][j] = HuftEntry;			/* connect to last table */
				} else {
					*ppNewTree = q;					/* first table is returned result */
				}
			}

			/* set up table entry in r */
			HuftEntry.m_bBitCount = (uint8_t)(k - w);
			if (p >= pWorkArea + uNumberSamples) {
				HuftEntry.m_bExtraOperation = 128 + 64;		/* out of values--invalid code */
			} else if (*p < uMaxSample) {
				HuftEntry.m_bExtraOperation = (uint8_t)(*p < 256 ? 0 : 32 + 64);		/* 256 is end-of-block */
				HuftEntry.m_uBase = static_cast<uint16_t>(p[0]);			/* simple code is just the value */
				++p;
			} else {
				HuftEntry.m_bExtraOperation = (uint8_t)(pDefaultBits[*p - uMaxSample] + 16 + 64);/* non-simple--look up in lists */
				HuftEntry.m_uBase = static_cast<uint16_t>(pDefaultLengths[p[0] - uMaxSample]);
				++p;
			}

			/* fill code-like entries with r */
			f = 1U << (k - w);
			for (j = i >> w; j < z; j += f) {
				q[j] = HuftEntry;
			}
			/* backwards increment the k-bit code i */
			for (j = 1U << (k - 1); i & j; j >>= 1) {
				i ^= j;
			}
			i ^= j;

			/* backup over finished tables */
			uint_t mask = (1U << w) - 1U;		/* needed on HP, cc -O bug */
			while ((i & mask) != x[h]) {
				h--;					/* don't need to update q */
				w -= l;
				mask = (1U << w) - 1U;
			}
		}
	}
	/* Return Z_BUF_ERROR if we were given an incomplete table */
	return y != 0 && g != 1 ? Z_BUF_ERROR : Z_OK;
}

/*! ************************************

	\brief Set the default huffman trees

	\param pSampleCounts Pointer to the sample count array
	\param pNewTreeSize Pointer to receive the size of the new tree
	\param ppNewTree Pointer to receive the new tree
	\param pExistingTree Pointer to an existing huffman tree

***************************************/

int Burger::DecompressDeflate::TreesBits(const uint_t *pSampleCounts,uint_t *pNewTreeSize,DeflateHuft_t **ppNewTree,DeflateHuft_t *pExistingTree)
{
	uint_t WorkArea[19];
	MemoryClear(&WorkArea,sizeof(WorkArea));

	uint_t uHuffmanCount = 0;			// Huffmans used in the WorkArea
	int iErrorCode = BuildHuffmanTrees(pSampleCounts,19,19,NULL,NULL,ppNewTree,pNewTreeSize,pExistingTree,&uHuffmanCount,WorkArea);
	if (iErrorCode != Z_DATA_ERROR) {
		if (iErrorCode == Z_BUF_ERROR || *pNewTreeSize == 0) {
			iErrorCode = Z_DATA_ERROR;
		}
	}
	return iErrorCode;
}

/*! ************************************

	\brief Create the dynamic huffman tree

	\param uNumberSamples Number of sample count entries
	\param uNumberDistance Number of distance sample entries
	\param pSampleCounts Pointer to the sample count array
	\param pNewTreeSize Pointer to receive the size of the new tree
	\param pNewDistanceSize Pointer to receive the size of the new distance tree
	\param ppNewTree Pointer to receive the new tree
	\param ppNewDistance Pointer to receive the new distance tree
	\param pExistingTree Pointer to an existing huffman tree

	\return Error code, if any

***************************************/

int Burger::DecompressDeflate::TreesDynamic(uint_t uNumberSamples,uint_t uNumberDistance,const uint_t *pSampleCounts,uint_t *pNewTreeSize,uint_t *pNewDistanceSize,
	DeflateHuft_t **ppNewTree,DeflateHuft_t **ppNewDistance,DeflateHuft_t *pExistingTree)
{
	uint_t WorkArea[288];
	MemoryClear(&WorkArea,sizeof(WorkArea));

	uint_t uHuffmanCount = 0;			// Huffmans used in the WorkArea
	// build literal/length tree
	int iErrorCode = BuildHuffmanTrees(pSampleCounts,uNumberSamples,257,g_DeflateCopyLengths,g_DeflateExtraBits,ppNewTree,pNewTreeSize,pExistingTree,&uHuffmanCount,WorkArea);
	if (iErrorCode != Z_OK || *pNewTreeSize == 0) {
		if (iErrorCode != Z_DATA_ERROR) {
			if (iErrorCode != Z_MEM_ERROR) {
				iErrorCode = Z_DATA_ERROR;
			}
		}
		return iErrorCode;
	}

	// build distance tree
	iErrorCode = BuildHuffmanTrees(pSampleCounts + uNumberSamples, uNumberDistance, 0, g_DeflateDistanceCodes, g_DeflateDistanceExtraBits, ppNewDistance, pNewDistanceSize, pExistingTree, &uHuffmanCount,WorkArea);
	if (iErrorCode != Z_OK || (*pNewDistanceSize == 0 && uNumberSamples > 257)) {
		if (iErrorCode != Z_DATA_ERROR) {
			if (iErrorCode == Z_BUF_ERROR) {
				iErrorCode = Z_DATA_ERROR;
			} else if (iErrorCode != Z_MEM_ERROR) {
				iErrorCode = Z_DATA_ERROR;
			}
		}
		return iErrorCode;
	}
	// done
	return Z_OK;
}


/*
	Notes beyond the 1.93a appnote.txt:

	1. Distance pointers never point before the beginning of the output
		stream.
	2. Distance pointers can point back across blocks, up to 32k away.
	3. There is an implied maximum of 7 bits for the bit length table and
		15 bits for the actual data.
	4. If only one code exists, then it is encoded using one bit. (Zero
		would be more efficient, but perhaps a little confusing.) If two
		codes exist, they are coded using one bit each (0 and 1).
	5. There is no way of sending zero distance codes--a dummy must be
		sent if there are none. (History: a pre 2.0 version of PKZIP would
		store blocks with no distance codes, but this was discovered to be
		too harsh a criterion.) Valid only for 1.93a. 2.04c does allow
		zero distance codes, which is sent as one code of zero bits in
		length.
	6. There are up to 286 literal/length codes. Code 256 represents the
		end-of-block. Note however that the static length tree defines
		288 codes just to fill out the Huffman codes. Codes 286 and 287
		cannot be used though, since there is no length base or extra bits
		defined for them. Similarily, there are up to 30 distance codes.
		However, static trees define 32 codes (all 5 bits) to fill out the
		Huffman codes, but the last two had better not show up in the data.
	7. Unzip can check dynamic Huffman blocks for complete code sets.
		The exception is that a single code would not be complete (see #4).
	8. The five bits following the block type is really the number of
		literal codes sent minus 257.
	9. Length codes 8,16,16 are interpreted as 13 length codes of 8 bits
		(1+6+6). Therefore, to output three times the length, you output
		three codes (1+1+1), whereas to output four times the same length,
		you only need two codes (1+3). Hmm.
	10. In the tree reconstruction algorithm, Code = Code + Increment
		only if BitLength(i) is not zero. (Pretty obvious.)
	11. Correction: 4 Bits: # of Bit Length codes - 4 (4 - 19)
	12. Note: length code 284 can represent 227-258, but length code 285
		really is 258. The last length deserves its own, short code
		since it gets used a lot in very redundant files. The length
		258 is special since 258 - 3 (the min match length) is 255.
	13. The literal/length and distance code bit lengths are read as a
		single stream of lengths. It is possible (and advantageous) for
		a repeat code (16, 17, or 18) to go across the boundary between
		the two sets of lengths.
*/

/*! ************************************

	\brief Reset the decompression state.

***************************************/

void Burger::DecompressDeflate::BlocksReset(void)
{
	Free(m_pTreesLengths);
	m_pTreesLengths = NULL;
	m_eBlockMode = BLOCKMODE_TYPE;
	m_uBitCount = 0;
	m_uBitBucket = 0;
	m_pWindowRead = m_WindowBuffer;
	m_pWindowWrite = m_WindowBuffer;
	m_uAlder32Checksum = 1;
}

/*! ************************************

	\brief Process the data blocks

	\param iErrorCode Default error code to return
	\return Error code, if any

***************************************/

int Burger::DecompressDeflate::ProcessBlocks(int iErrorCode)
{
	/* copy input/output information to locals (UPDATE macro restores) */
	const uint8_t *pInput=m_pInput;
	uintptr_t uInputChunkLength=m_uInputChunkLength;
	uint32_t uBitBucket=m_uBitBucket;
	uint_t uBitCount=m_uBitCount;
	uint8_t *pWindowWrite=m_pWindowWrite;
	uintptr_t uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);

	/* process input based on current state */
	uint_t t;				/* temporary storage */
	for (;;) {
		switch (m_eBlockMode) {
		case BLOCKMODE_TYPE:
			while(uBitCount<3) {
				if (!uInputChunkLength) {
					goto Abort;
				}
				iErrorCode = Z_OK;
				--uInputChunkLength;
				uBitBucket|=static_cast<uint32_t>(pInput[0])<<uBitCount;
				++pInput;
				uBitCount+=8;
			}
			t = uBitBucket & 7U;
			uBitBucket>>=3U;			/* Accept 3 bits */
			uBitCount-=3;
			m_bLastBlock = t & 1U;
			switch (t&6) {
			case 0:							/* stored */
				t = uBitCount & 7U;					/* go to byte boundary */
				uBitBucket>>=t;
				uBitCount-=t;
				m_eBlockMode = BLOCKMODE_LENS;				/* get length of stored block */
				break;
			case 2:							/* fixed */
				CodesReset(FIXED_BLOCKLENGTH,FIXED_BLOCKDISTANCE,s_FixedTreeLiteral,s_FixedTreeDistance);
				m_eBlockMode = BLOCKMODE_CODES;
				break;
			case 4:							/* dynamic */
				m_eBlockMode = BLOCKMODE_TABLE;
				break;
			case 6:							/* illegal */
				m_eBlockMode = BLOCKMODE_ABORT;
				iErrorCode = Z_DATA_ERROR;
				goto Abort;
			}
			break;
		case BLOCKMODE_LENS:
			while (uBitCount<32) {
				if(!uInputChunkLength) {
					goto Abort;
				}
				iErrorCode=Z_OK;
				--uInputChunkLength;
				uBitBucket|=static_cast<uint32_t>(pInput[0])<<uBitCount;
				++pInput;
				uBitCount+=8;
			}
			if ((((~uBitBucket) >> 16) & 0xffff) != (uBitBucket & 0xffff)) {
				m_eBlockMode = BLOCKMODE_ABORT;
				iErrorCode = Z_DATA_ERROR;
				goto Abort;
			}
			m_uStoredBytesToCopy = uBitBucket & 0xFFFFU;
			uBitBucket = uBitCount = 0;						/* dump bits */
			m_eBlockMode = m_uStoredBytesToCopy ? BLOCKMODE_STORED : (m_bLastBlock ? BLOCKMODE_DRY : BLOCKMODE_TYPE);
			break;
		case BLOCKMODE_STORED:
			if (!uInputChunkLength) {
				goto Abort;
			}

			if (!uRemainingWindow) {
				if((pWindowWrite==&m_WindowBuffer[1<<MAX_WBITS])&&m_pWindowRead!=m_WindowBuffer) {
					pWindowWrite=m_WindowBuffer;
					uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
				}
				if (!uRemainingWindow) {
					m_pWindowWrite=pWindowWrite;
					iErrorCode=Flush(iErrorCode);
					pWindowWrite=m_pWindowWrite;
					uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
					if(pWindowWrite==&m_WindowBuffer[1<<MAX_WBITS]&&m_pWindowRead!=m_WindowBuffer) {
						pWindowWrite=m_WindowBuffer;
						uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
					}
					if (!uRemainingWindow) {
						goto Abort;
					}
				}
			}
			iErrorCode=Z_OK;
			{
				uintptr_t uCopySize = m_uStoredBytesToCopy;
				if (uCopySize > uInputChunkLength) {
					uCopySize = static_cast<uint_t>(uInputChunkLength);
				}
				if (uCopySize > uRemainingWindow) {
					uCopySize = uRemainingWindow;
				}
				MemoryCopy(pWindowWrite,pInput,uCopySize);
				pInput += uCopySize;
				uInputChunkLength -= uCopySize;
				pWindowWrite += uCopySize;
				uRemainingWindow -= uCopySize;
				if ((m_uStoredBytesToCopy -= uCopySize) != 0) {
					break;
				}
			}
			m_eBlockMode = m_bLastBlock ? BLOCKMODE_DRY : BLOCKMODE_TYPE;
			break;

		case BLOCKMODE_TABLE:
			while (uBitCount<14) {
				if (!uInputChunkLength) {
					goto Abort;
				}
				iErrorCode=Z_OK;
				--uInputChunkLength;
				uBitBucket|=static_cast<uint32_t>(pInput[0])<<uBitCount;
				++pInput;
				uBitCount+=8;
			}
			t = uBitBucket & 0x3fffU;
			m_uTreesTable = t;
			if ((t & 0x1f) > 29 || ((t >> 5) & 0x1f) > 29) {
				m_eBlockMode = BLOCKMODE_ABORT;
				iErrorCode = Z_DATA_ERROR;
				goto Abort;
			}
			t = 258 + (t & 0x1f) + ((t >> 5) & 0x1f);
			if ((m_pTreesLengths = static_cast<uint_t*>(AllocClear(t*sizeof(uint_t)))) == NULL) {
 				iErrorCode = Z_MEM_ERROR;
				goto Abort;
			}
			uBitBucket>>=14;
			uBitCount-=14;
			m_uTreesIndex = 0;
			m_eBlockMode = BLOCKMODE_BTREE;
		case BLOCKMODE_BTREE:
			while (m_uTreesIndex < 4 + (m_uTreesTable >> 10)) {
				while(uBitCount<3) {
					if(!uInputChunkLength) {
						goto Abort;
					}
					iErrorCode=Z_OK;
					--uInputChunkLength;
					uBitBucket|=static_cast<uint32_t>(pInput[0])<<uBitCount;
					++pInput;
					uBitCount+=8;
				}
				m_pTreesLengths[g_DeflateBorder[m_uTreesIndex++]] = (uint_t)uBitBucket & 7;
				uBitBucket>>=3;
				uBitCount-=3;
			}
			while (m_uTreesIndex < 19) {
				m_pTreesLengths[g_DeflateBorder[m_uTreesIndex++]] = 0;
			}
			m_uTreesDepth = 7;
			t = static_cast<uint_t>(TreesBits(m_pTreesLengths, &m_uTreesDepth,&m_pTreesHuffman, m_HuffmanTable));
			if (t != Z_OK) {
				Free(m_pTreesLengths);
				m_pTreesLengths = 0;
				iErrorCode = static_cast<int>(t);
				if (iErrorCode == Z_DATA_ERROR) {
					m_eBlockMode = BLOCKMODE_ABORT;
				}
				goto Abort;
			}
			m_uTreesIndex = 0;
			m_eBlockMode = BLOCKMODE_DTREE;
		case BLOCKMODE_DTREE:
			while (t = m_uTreesTable,
				m_uTreesIndex < 258 + (t & 0x1f) + ((t >> 5) & 0x1f)) {
				t = m_uTreesDepth;
				while (uBitCount<t) {
					if(!uInputChunkLength) {
						goto Abort;
					}
					iErrorCode=Z_OK;
					--uInputChunkLength;
					uBitBucket|=((uint32_t)(pInput[0]))<<uBitCount;
					uBitCount+=8;
					++pInput;
				}
				DeflateHuft_t *h = m_pTreesHuffman + (uBitBucket & g_DeflateMask[t]);
				t = h->m_bBitCount;
				int c = h->m_uBase;
				if (c < 16) {
					uBitBucket>>=t;
					uBitCount-=t;
					m_pTreesLengths[m_uTreesIndex++] = static_cast<uint_t>(c);
				} else {	/* c == 16..18 */
					uint_t i, j;
					if (c==18) {
						i = 7;
						j = 11;
					} else {
						i = c-14U;
						j = 3;
					}
					while(uBitCount<(t+i)) {
						if (!uInputChunkLength) {
							goto Abort;
						}
						iErrorCode=Z_OK;
						--uInputChunkLength;
						uBitBucket|=static_cast<uint32_t>(pInput[0])<<uBitCount;
						++pInput;
						uBitCount+=8;
					}
					uBitBucket>>=t;
					uBitCount-=t;
					j += (uBitBucket & g_DeflateMask[i]);
					uBitBucket>>=i;
					uBitCount-=i;
					i = m_uTreesIndex;
					t = m_uTreesTable;
					if (i + j > 258 + (t & 0x1f) + ((t >> 5) & 0x1f) ||
						(c == 16 && i < 1)) {
						Free(m_pTreesLengths);
						m_pTreesLengths = 0;
						m_eBlockMode = BLOCKMODE_ABORT;
						iErrorCode = Z_DATA_ERROR;
						goto Abort;
					}
					c = (c == 16) ? static_cast<int>(m_pTreesLengths[i - 1]) : 0;
					do {
						m_pTreesLengths[i] = static_cast<uint_t>(c);
						++i;
					} while (--j);
					m_uTreesIndex = i;
				}
			}
			m_pTreesHuffman = NULL;
			{
				DeflateHuft_t *pCodeTreeLength,*pCodeTreeDistance;

				uint_t bCodeLengthBits = 9;			/* must be <= 9 for lookahead assumptions */
				uint_t bCodeDistanceBits = 6;			/* must be <= 9 for lookahead assumptions */
				t = m_uTreesTable;
				t = static_cast<uint_t>(TreesDynamic(257 + (t & 0x1f), 1 + ((t >> 5) & 0x1f),m_pTreesLengths,&bCodeLengthBits,&bCodeDistanceBits,&pCodeTreeLength,&pCodeTreeDistance,m_HuffmanTable));
				Free(m_pTreesLengths);
				m_pTreesLengths = NULL;
				if (t != Z_OK) {
					if (t == (uint_t)Z_DATA_ERROR) {
						m_eBlockMode = BLOCKMODE_ABORT;
					}
					iErrorCode = static_cast<int>(t);
					goto Abort;
				}
				CodesReset(bCodeLengthBits,bCodeDistanceBits,pCodeTreeLength,pCodeTreeDistance);
			}
			m_eBlockMode = BLOCKMODE_CODES;
		case BLOCKMODE_CODES:
			m_uBitBucket=uBitBucket;
			m_uBitCount=uBitCount;
			m_uInputChunkLength=uInputChunkLength;
			m_pInput=pInput;
			m_pWindowWrite=pWindowWrite;
			iErrorCode = ProcessCodes(iErrorCode);
			if (iErrorCode != Z_STREAM_END) {
				return Flush(iErrorCode);
			}
			iErrorCode = Z_OK;
			pInput=m_pInput;
			uInputChunkLength=m_uInputChunkLength;
			uBitBucket=m_uBitBucket;
			uBitCount=m_uBitCount;
			pWindowWrite=m_pWindowWrite;
			uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
			if (!m_bLastBlock) {
				m_eBlockMode = BLOCKMODE_TYPE;
				break;
			}
			m_eBlockMode = BLOCKMODE_DRY;
		case BLOCKMODE_DRY:
			m_pWindowWrite=pWindowWrite;
			iErrorCode=Flush(iErrorCode);
			pWindowWrite=m_pWindowWrite;
			uRemainingWindow=static_cast<uintptr_t>(pWindowWrite<m_pWindowRead?m_pWindowRead-pWindowWrite-1:&m_WindowBuffer[1<<MAX_WBITS]-pWindowWrite);
			if (m_pWindowRead != m_pWindowWrite) {
				goto Abort;
			}
			m_eBlockMode = BLOCKMODE_COMPLETED;
		case BLOCKMODE_COMPLETED:
			iErrorCode = Z_STREAM_END;
			goto Abort;
		case BLOCKMODE_ABORT:
			iErrorCode = Z_DATA_ERROR;
			goto Abort;
		default:
			iErrorCode = Z_STREAM_ERROR;
			goto Abort;
		}
	}
Abort:;
	m_uBitBucket=uBitBucket;
	m_uBitCount=uBitCount;
	m_uInputChunkLength=uInputChunkLength;
	m_pInput=pInput;
	m_pWindowWrite=pWindowWrite;
	return Flush(iErrorCode);
}

/*! ************************************

	\brief Default constructor

***************************************/

Burger::DecompressDeflate::DecompressDeflate() :
	Decompress(),
	m_pInput(NULL),
	m_pOutput(NULL),
	m_pTreesLengths(NULL)
{
	Reset();
}

/*! ************************************

	\brief Release any allocated memory

***************************************/

Burger::DecompressDeflate::~DecompressDeflate()
{
	BlocksReset();
}


/*! ************************************

	\brief Reset the decompressor
	\return Zero if no error, non-zero if error

***************************************/

Burger::eError Burger::DecompressDeflate::Reset(void)
{
	m_uTotalOutput = 0;
	m_uTotalInput = 0;

	// Create inflate_blocks state

	m_eBlockMode = BLOCKMODE_TYPE;
	m_eState = STATE_METHOD;
	BlocksReset();
	return kErrorNone;
}

/*! ************************************

	\brief Decompress data using Deflate compression

	Using the Deflate (ZLIB) compression algorithm, decompress the data

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to compress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code
	\sa Burger::SimpleDecompressDeflate()

***************************************/

Burger::eError Burger::DecompressDeflate::Process(void *pOutput,uintptr_t uOutputChunkLength,const void *pInput,uintptr_t uInputChunkLength)
{
	m_uInputLength = uInputChunkLength;
	m_uOutputLength = uOutputChunkLength;

	int iError = Z_OK;		
	if (uInputChunkLength) {
		// Store the output buffer details to the current state

		m_pOutput = static_cast<uint8_t *>(pOutput);
		m_uOutputChunkLength = uOutputChunkLength;

		// This interruptible code flow is possible because "continue"
		// is not intercepted by the switch statement
		// So, "break" means exit the loop and "continue"
		// means perform the loop again
		iError = Z_BUF_ERROR;		// Assume a buffer error
		for (;;) {
			uint_t uTemp;				// Bit bucket
			switch (m_eState) {
			case STATE_METHOD:
				if (!uInputChunkLength) {
					break;
				}
				--uInputChunkLength;
				uTemp = static_cast<const uint8_t *>(pInput)[0];
				pInput = static_cast<const uint8_t *>(pInput)+1;
				m_uCompressionMethod = uTemp;
				if (((uTemp & 0xf) != Z_DEFLATED) ||
					(((uTemp >> 4U) + 8U) > MAX_WBITS)) {
					m_eState = STATE_ABORT;
					iError = Z_DATA_ERROR;
					break;
				}
				// Flag mode
				m_eState = STATE_FLAG;
				iError=Z_OK;

			case STATE_FLAG:
				if (!uInputChunkLength) {
					break;
				}
				--uInputChunkLength;
				uTemp = static_cast<const uint8_t *>(pInput)[0];
				pInput = static_cast<const uint8_t *>(pInput)+1;
				if (((m_uCompressionMethod << 8U) + uTemp) % 31U) {
					m_eState = STATE_ABORT;
					iError = Z_DATA_ERROR;
					break;
				}
				// Jump to blocks?
				if (!(uTemp & PRESET_DICT)) {
					m_eState = STATE_BLOCKS;
					iError=Z_OK;
					continue;
				}
				m_eState = STATE_DICT4;
				iError=Z_OK;
			case STATE_DICT4:
				if (!uInputChunkLength) {
					break;
				}
				--uInputChunkLength;
				m_uChecksumInStream = static_cast<uint32_t>(static_cast<const uint8_t *>(pInput)[0]) << 24U;
				pInput = static_cast<const uint8_t *>(pInput)+1;
				m_eState = STATE_DICT3;
				iError=Z_OK;
			case STATE_DICT3:
				if (!uInputChunkLength) {
					break;
				}
				--uInputChunkLength;
				m_uChecksumInStream += static_cast<uint32_t>(static_cast<const uint8_t *>(pInput)[0]) << 16U;
				pInput = static_cast<const uint8_t *>(pInput)+1;
				m_eState = STATE_DICT2;
				iError=Z_OK;
			case STATE_DICT2:
				if (!uInputChunkLength) {
					break;
				}
				--uInputChunkLength;
				m_uChecksumInStream += static_cast<uint32_t>(static_cast<const uint8_t *>(pInput)[0]) << 8U;
				pInput = static_cast<const uint8_t *>(pInput)+1;
				m_eState = STATE_DICT1;
				iError=Z_OK;
			case STATE_DICT1:
				if (!uInputChunkLength) {
					break;
				}
				--uInputChunkLength;
				m_uChecksumInStream += static_cast<uint32_t>(static_cast<const uint8_t *>(pInput)[0]);
				pInput = static_cast<const uint8_t *>(pInput)+1;
				m_eState = STATE_DICT0;
				iError = Z_NEED_DICT;
				break;

			case STATE_DICT0:
				m_eState = STATE_ABORT;
				iError = Z_STREAM_ERROR;
				break;

			// Decompress with blocks

			case STATE_BLOCKS:
				m_uInputChunkLength = uInputChunkLength;
				m_pInput = static_cast<const uint8_t *>(pInput);
				iError = ProcessBlocks(iError);
				uInputChunkLength = m_uInputChunkLength;
				pInput = m_pInput;
				if (iError == Z_DATA_ERROR) {
					m_eState = STATE_ABORT;
					iError = Z_STREAM_ERROR;
					break;
				}
				if (iError != Z_STREAM_END) {
					break;
				}
				m_uInputChunkLength = uInputChunkLength;
				m_pInput = static_cast<const uint8_t *>(pInput);
				m_uChecksumOfChunk = m_uAlder32Checksum;
				BlocksReset();
				uInputChunkLength = m_uInputChunkLength;
				pInput = m_pInput;
				m_eState = STATE_CHECK4;
				iError = Z_OK;

			// Test checksum
			case STATE_CHECK4:
				if (!uInputChunkLength) {
					break;
				}
				--uInputChunkLength;
                m_uChecksumInStream = static_cast<uint32_t>(static_cast<const uint8_t *>(pInput)[0]) << 24U;
                pInput = static_cast<const uint8_t *>(pInput) + 1;
				m_eState = STATE_CHECK3;
				iError=Z_OK;
			case STATE_CHECK3:
				if (!uInputChunkLength) {
					break;
				}
				--uInputChunkLength;
				m_uChecksumInStream += static_cast<uint32_t>(static_cast<const uint8_t *>(pInput)[0]) << 16U;
				pInput = static_cast<const uint8_t *>(pInput)+1;
				m_eState = STATE_CHECK2;
				iError=Z_OK;
			case STATE_CHECK2:
				if (!uInputChunkLength) {
					break;
				}
				--uInputChunkLength;
				m_uChecksumInStream += static_cast<uint32_t>(static_cast<const uint8_t *>(pInput)[0]) << 8U;
				pInput = static_cast<const uint8_t *>(pInput)+1;
				m_eState = STATE_CHECK1;
				iError=Z_OK;
			case STATE_CHECK1:
				if (!uInputChunkLength) {
					break;
				}
				--uInputChunkLength;
				m_uChecksumInStream += static_cast<uint32_t>(static_cast<const uint8_t *>(pInput)[0]);
				pInput = static_cast<const uint8_t *>(pInput)+1;
				
				// Is there a checksum match?

				if (m_uChecksumOfChunk != m_uChecksumInStream) {
					m_eState = STATE_ABORT;
					iError = Z_DATA_ERROR;
					break;
				}
				m_eState = STATE_COMPLETE;
			case STATE_COMPLETE:
				iError = Z_STREAM_END;
				break;
			case STATE_ABORT:
				iError = Z_DATA_ERROR;
				break;
			default:
				iError = Z_STREAM_ERROR;
				break;
			}
			break;
		}
		uOutputChunkLength = m_uOutputChunkLength;
	}

	// How did the decompression go?

	m_uOutputLength -= uOutputChunkLength;
	m_uInputLength -= uInputChunkLength;
	m_uTotalOutput += m_uOutputLength;
	m_uTotalInput += m_uInputLength;

	if ((iError!=Z_OK) && (iError!=Z_STREAM_END)) {
		return kErrorDataCorruption;
	}

	// Output buffer not big enough?
	if (uOutputChunkLength) {
		return kErrorDataStarvation;
	}

	// Input data remaining?
	if (uInputChunkLength) {
		return kErrorBufferTooSmall;
	}
	// Still in checksum?
	if (m_eState!=STATE_COMPLETE) {
		return kErrorDataStarvation;
	}
	// Decompression is complete
	return kErrorNone;
}


/*! ************************************

	\brief Decompress data using Deflate compression

	Using the Deflate (ZLIB) compression algorithm, decompress the data in one pass

	\param pOutput Pointer to the buffer to accept the decompressed data
	\param uOutputChunkLength Number of bytes in the output buffer
	\param pInput Pointer to data to compress
	\param uInputChunkLength Number of bytes in the data to decompress

	\return Decompress::eError code with zero if no failure, non-zero is an error code
	\sa DecompressDeflate::Process()

***************************************/

Burger::eError BURGER_API Burger::SimpleDecompressDeflate(void *pOutput,uintptr_t uOutputChunkLength,const void *pInput,uintptr_t uInputChunkLength)
{
	DecompressDeflate *pDecompress = New<DecompressDeflate>();
	pDecompress->DecompressDeflate::Reset();
	eError uError = pDecompress->DecompressDeflate::Process(pOutput,uOutputChunkLength,pInput,uInputChunkLength);
	Delete(pDecompress);
	return uError;
}

/*! ************************************

	\var const Burger::StaticRTTI Burger::DecompressDeflate::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
