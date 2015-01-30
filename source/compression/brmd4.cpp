/***************************************

	MD4 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/MD4
	and http://tools.ietf.org/html/rfc1320

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmd4.h"
#include "brstringfunctions.h"
#include "brendian.h"
#include "brfixedpoint.h"

/*! ************************************

	\struct Burger::MD4_t
	\brief 16 byte array to contain an MD4 hash
	
	Full documentation on this hash format can be found here
	http://en.wikipedia.org/wiki/MD4

	\sa Hash(MD4_t *,const void *,WordPtr) and Burger::MD4Hasher_t

***************************************/

/*! ************************************

	\struct Burger::MD4Hasher_t
	\brief Multi-pass MD4 hash generator

	This structure is needed to perform a multi-pass MD4 hash
	and contains cached data and a running checksum.
	\code
		Burger::MD4Hasher_t Context;
		// Initialize
		Context.Init();
		// Process data in passes
		Context.Process(Buffer1,sizeof(Buffer1));
		Context.Process(Buffer2,sizeof(Buffer2));
		Context.Process(Buffer3,sizeof(Buffer3));
		// Wrap up the processing
		Context.Finalize();
		// Return the resulting hash
		MemoryCopy(pOutput,&Context.m_Hash,16);
	\endcode

	\sa Burger::MD4_t or Hash(MD4_t *,const void *,WordPtr)

***************************************/

/*! ************************************

	\brief Initialize the MD4 hasher

	Call this function before any hashing is performed

	\sa Process(const void *,WordPtr) or Finalize(void)

***************************************/

void Burger::MD4Hasher_t::Init(void)
{
	// Load magic initialization constants.

#if defined(BURGER_LITTLEENDIAN)
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[0] = 0x67452301;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[1] = 0xefcdab89;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[2] = 0x98badcfe;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[3] = 0x10325476;
#else
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[0] = 0x01234567;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[1] = 0x89abcdef;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[2] = 0xfedcba98;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[3] = 0x76543210;
#endif
	m_uByteCount = 0;
}

// Constants for MD4Transform routine.

#if !defined(DOXYGEN)
#define S411 3
#define S412 7
#define S413 11
#define S414 19
#define S421 3
#define S422 5
#define S423 9
#define S424 13
#define S431 3
#define S432 9
#define S433 11
#define S434 15

// F, G and H are basic MD4 functions.

#define F4(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G4(x, y, z) (((x) & (y)) | ((x) & (z)) | ((y) & (z)))
#define H4(x, y, z) ((x) ^ (y) ^ (z))

// FF, GG and HH are transformations for rounds 1, 2 and 3
// Rotation is separate from addition to prevent recomputation

#define FF4(a, b, c, d, x, s) { \
	(a) += F4((b), (c), (d)) + (x); \
	(a) = RotateLeft(a,s); \
}

#define GG4(a, b, c, d, x, s) { \
	(a) += G4((b), (c), (d)) + (x) + 0x5a827999; \
	(a) = RotateLeft(a,s); \
}

#define HH4(a, b, c, d, x, s) { \
	(a) += H4((b), (c), (d)) + (x) + 0x6ed9eba1; \
	(a) = RotateLeft(a,s); \
}
#endif

/*! ************************************

	\brief Process a single 64 byte block of data

	MD4 data is processed in 64 byte chunks. This function
	will process 64 bytes on input and update the hash and checksum

	\param pBlock Pointer to a buffer of 64 bytes of data to hash
	\sa Process(const void *,WordPtr), Finalize(void) or Init(void)

***************************************/

void Burger::MD4Hasher_t::Process(const Word8 *pBlock)
{
	Word32 x[16];

	// Convert endian on big endian machines
	Word32 hasha = LittleEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[0]);
	Word32 hashb = LittleEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[1]);
	Word32 hashc = LittleEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[2]);
	Word32 hashd = LittleEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[3]);

	Word i = 16;
	Word8 *pDest = reinterpret_cast<Word8 *>(x);
	do {
#if defined(BURGER_LITTLEENDIAN)
		pDest[0] = pBlock[0];
		pDest[1] = pBlock[1];
		pDest[2] = pBlock[2];
		pDest[3] = pBlock[3];
#else
		pDest[0] = pBlock[3];
		pDest[1] = pBlock[2];
		pDest[2] = pBlock[1];
		pDest[3] = pBlock[0];
#endif
		pBlock+=4;
		pDest+=4;
	} while (--i);

	// Make a copy of the hash integers 
	Word32 a = hasha;
	Word32 b = hashb;
	Word32 c = hashc;
	Word32 d = hashd;

	// Round 1
	FF4(a,b,c,d,x[ 0],S411); // 1
	FF4(d,a,b,c,x[ 1],S412); // 2
	FF4(c,d,a,b,x[ 2],S413); // 3
	FF4(b,c,d,a,x[ 3],S414); // 4
	FF4(a,b,c,d,x[ 4],S411); // 5
	FF4(d,a,b,c,x[ 5],S412); // 6
	FF4(c,d,a,b,x[ 6],S413); // 7
	FF4(b,c,d,a,x[ 7],S414); // 8
	FF4(a,b,c,d,x[ 8],S411); // 9
	FF4(d,a,b,c,x[ 9],S412); // 10
	FF4(c,d,a,b,x[10],S413); // 11
	FF4(b,c,d,a,x[11],S414); // 12
	FF4(a,b,c,d,x[12],S411); // 13
	FF4(d,a,b,c,x[13],S412); // 14
	FF4(c,d,a,b,x[14],S413); // 15
	FF4(b,c,d,a,x[15],S414); // 16

	// Round 2
	GG4(a,b,c,d,x[ 0],S421); // 17
	GG4(d,a,b,c,x[ 4],S422); // 18
	GG4(c,d,a,b,x[ 8],S423); // 19
	GG4(b,c,d,a,x[12],S424); // 20
	GG4(a,b,c,d,x[ 1],S421); // 21
	GG4(d,a,b,c,x[ 5],S422); // 22
	GG4(c,d,a,b,x[ 9],S423); // 23
	GG4(b,c,d,a,x[13],S424); // 24
	GG4(a,b,c,d,x[ 2],S421); // 25
	GG4(d,a,b,c,x[ 6],S422); // 26
	GG4(c,d,a,b,x[10],S423); // 27
	GG4(b,c,d,a,x[14],S424); // 28
	GG4(a,b,c,d,x[ 3],S421); // 29
	GG4(d,a,b,c,x[ 7],S422); // 30
	GG4(c,d,a,b,x[11],S423); // 31
	GG4(b,c,d,a,x[15],S424); // 32

	// Round 3
	HH4(a,b,c,d,x[ 0],S431); // 33
	HH4(d,a,b,c,x[ 8],S432); // 34
	HH4(c,d,a,b,x[ 4],S433); // 35
	HH4(b,c,d,a,x[12],S434); // 36
	HH4(a,b,c,d,x[ 2],S431); // 37
	HH4(d,a,b,c,x[10],S432); // 38
	HH4(c,d,a,b,x[ 6],S433); // 39
	HH4(b,c,d,a,x[14],S434); // 40
	HH4(a,b,c,d,x[ 1],S431); // 41
	HH4(d,a,b,c,x[ 9],S432); // 42
	HH4(c,d,a,b,x[ 5],S433); // 43
	HH4(b,c,d,a,x[13],S434); // 44
	HH4(a,b,c,d,x[ 3],S431); // 45
	HH4(d,a,b,c,x[11],S432); // 46
	HH4(c,d,a,b,x[ 7],S433); // 47
	HH4(b,c,d,a,x[15],S434); // 48

	// Add in the adjusted hash (Store in little endian format)

	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[0] = LittleEndian::Load(a+hasha);
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[1] = LittleEndian::Load(b+hashb);
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[2] = LittleEndian::Load(c+hashc);
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[3] = LittleEndian::Load(d+hashd);
}

/*! ************************************

	\brief Process an arbitrary number of input bytes

	Process input data into the hash. If data chunks are not 
	a multiple of 64 bytes, the excess will be cached and 
	a future call will continue the hashing where it left
	off.
	
	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash
	\sa Process(const Word8 *), Finalize(void)

***************************************/

void Burger::MD4Hasher_t::Process(const void *pInput,WordPtr uLength)
{
	// Compute number of bytes mod 64
	WordPtr uIndex = static_cast<WordPtr>(m_uByteCount & 0x3F);

	// Update number of bits
	m_uByteCount += uLength;
	
	WordPtr i = 64 - uIndex;

	// Transform as many times as possible.
	if (uLength >= i) {
		MemoryCopy(&m_CacheBuffer[uIndex],pInput,i);
		Process(m_CacheBuffer);

		if ((i+63)<uLength) {
			do {
				Process(&static_cast<const Word8 *>(pInput)[i]);
				i+=64;
			} while ((i+63)<uLength);
		}
		uIndex = 0;
	} else {
		i = 0;
	}

	// Buffer remaining input

	MemoryCopy(&m_CacheBuffer[uIndex],&static_cast<const Word8 *>(pInput)[i],uLength-i);
}

/*! ************************************

	\brief Finalize the hashing

	When multi-pass hashing is performed, this call is necessary to
	finalize the hash so that the generated checksum can
	be applied into the hash

	\sa Init(void), Process(const void *,WordPtr)

***************************************/

void Burger::MD4Hasher_t::Finalize(void)
{
	Word8 Padding[64];
	Padding[0] = 0x80;
	MemoryClear(&Padding[1],63);

	// Save number of bits

	Word64 uBitCountLE = LittleEndian::Load(m_uByteCount<<3);

	// Pad out to 56 mod 64.
	// Convert to 1-64

	WordPtr uPadLen = (((56-1)-static_cast<WordPtr>(m_uByteCount))&0x3F)+1;
	Process(Padding,uPadLen);

	// Append length (before padding)

	Process(&uBitCountLE,8);
}

/*! ************************************

	\brief Quickly create an MD4 key
	
	Given a buffer of data, generate the MD4 hash key

	\param pOutput Pointer to an unitialized MD4_t structure
	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash

	\sa Burger::MD4Hasher_t

***************************************/

void BURGER_API Burger::Hash(MD4_t *pOutput,const void *pInput,WordPtr uLength)
{
	MD4Hasher_t Context;
	// Initialize
	Context.Init();
	// Process all of the data
	Context.Process(pInput,uLength);
	// Wrap up the processing
	Context.Finalize();
	// Return the resulting hash
	MemoryCopy(pOutput,&Context.m_Hash,16);
}
