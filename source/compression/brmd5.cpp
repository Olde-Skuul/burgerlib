/***************************************

	MD5 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/MD5
	and http://tools.ietf.org/html/rfc1321

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmd5.h"
#include "brendian.h"
#include "brstringfunctions.h"
#include "brfixedpoint.h"

/*! ************************************

	\struct Burger::MD5_t
	\brief 16 byte array to contain an MD5 hash
	
	Full documentation on this hash format can be found here
	http://en.wikipedia.org/wiki/MD5

	\sa Hash(MD5_t *,const void *,WordPtr) and Burger::MD5Hasher_t

***************************************/

/*! ************************************

	\struct Burger::MD5Hasher_t
	\brief Multi-pass MD5 hash generator

	This structure is needed to perform a multi-pass MD5 hash
	and contains cached data and a running checksum.
	\code
		Burger::MD5Hasher_t Context;
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

	\sa Burger::MD5_t or Hash(MD5_t *,const void *,WordPtr)

***************************************/

/*! ************************************

	\brief Initialize the MD5 hasher

	Call this function before any hashing is performed

	\sa Process(const void *,WordPtr) or Finalize(void)

***************************************/

void Burger::MD5Hasher_t::Init(void)
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

// Constants for MD5Transform routine.

#if !defined(DOXYGEN)
#define S11 7
#define S12 12
#define S13 17
#define S14 22
#define S21 5
#define S22 9
#define S23 14
#define S24 20
#define S31 4
#define S32 11
#define S33 16
#define S34 23
#define S41 6
#define S42 10
#define S43 15
#define S44 21

// F, G, H and I are basic MD5 functions.
#define F(x, y, z) (((x) & (y)) | ((~x) & (z)))
#define G(x, y, z) (((x) & (z)) | ((y) & (~z)))
#define H(x, y, z) ((x) ^ (y) ^ (z))
#define I(x, y, z) ((y) ^ ((x) | (~z)))

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
// Rotation is separate from addition to prevent recomputation.

#define FF(a, b, c, d, x, s, ac) { \
	(a) += F ((b), (c), (d)) + (x) + (Word32)(ac); \
	(a) = RotateLeft(a,s); \
	(a) += (b); \
}
#define GG(a, b, c, d, x, s, ac) { \
	(a) += G ((b), (c), (d)) + (x) + (Word32)(ac); \
	(a) = RotateLeft(a,s); \
	(a) += (b); \
}
#define HH(a, b, c, d, x, s, ac) { \
	(a) += H ((b), (c), (d)) + (x) + (Word32)(ac); \
	(a) = RotateLeft(a,s); \
	(a) += (b); \
}
#define II(a, b, c, d, x, s, ac) { \
	(a) += I ((b), (c), (d)) + (x) + (Word32)(ac); \
	(a) = RotateLeft(a,s); \
	(a) += (b); \
}
#endif

/*! ************************************

	\brief Process a single 64 byte block of data

	MD5 data is processed in 64 byte chunks. This function
	will process 64 bytes on input and update the hash and checksum

	\param pBlock Pointer to a buffer of 64 bytes of data to hash
	\sa Process(const void *,WordPtr), Finalize(void) or Init(void)

***************************************/

void Burger::MD5Hasher_t::Process(const Word8 *pBlock)
{
	Word32 x[16];

	Word32 hasha = LittleEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[0]);
	Word32 hashb = LittleEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[1]);
	Word32 hashc = LittleEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[2]);
	Word32 hashd = LittleEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[3]);

	Word32 i = 0;
	do {
		x[i] = Burger::LittleEndian::LoadAny(&reinterpret_cast<const Word32 *>(pBlock)[i]);
	} while (++i<16);

	// Make a copy of the hash integers 
	Word32 a = hasha;
	Word32 b = hashb;
	Word32 c = hashc;
	Word32 d = hashd;

	// Round 1
	FF(a,b,c,d,x[ 0],S11,0xd76aa478); // 1
	FF(d,a,b,c,x[ 1],S12,0xe8c7b756); // 2
	FF(c,d,a,b,x[ 2],S13,0x242070db); // 3
	FF(b,c,d,a,x[ 3],S14,0xc1bdceee); // 4
	FF(a,b,c,d,x[ 4],S11,0xf57c0faf); // 5
	FF(d,a,b,c,x[ 5],S12,0x4787c62a); // 6
	FF(c,d,a,b,x[ 6],S13,0xa8304613); // 7
	FF(b,c,d,a,x[ 7],S14,0xfd469501); // 8
	FF(a,b,c,d,x[ 8],S11,0x698098d8); // 9
	FF(d,a,b,c,x[ 9],S12,0x8b44f7af); // 10
	FF(c,d,a,b,x[10],S13,0xffff5bb1); // 11
	FF(b,c,d,a,x[11],S14,0x895cd7be); // 12
	FF(a,b,c,d,x[12],S11,0x6b901122); // 13
	FF(d,a,b,c,x[13],S12,0xfd987193); // 14
	FF(c,d,a,b,x[14],S13,0xa679438e); // 15
	FF(b,c,d,a,x[15],S14,0x49b40821); // 16

	// Round 2
	GG(a,b,c,d,x[ 1],S21,0xf61e2562); // 17
	GG(d,a,b,c,x[ 6],S22,0xc040b340); // 18
	GG(c,d,a,b,x[11],S23,0x265e5a51); // 19
	GG(b,c,d,a,x[ 0],S24,0xe9b6c7aa); // 20
	GG(a,b,c,d,x[ 5],S21,0xd62f105d); // 21
	GG(d,a,b,c,x[10],S22,0x02441453); // 22
	GG(c,d,a,b,x[15],S23,0xd8a1e681); // 23
	GG(b,c,d,a,x[ 4],S24,0xe7d3fbc8); // 24
	GG(a,b,c,d,x[ 9],S21,0x21e1cde6); // 25
	GG(d,a,b,c,x[14],S22,0xc33707d6); // 26
	GG(c,d,a,b,x[ 3],S23,0xf4d50d87); // 27
	GG(b,c,d,a,x[ 8],S24,0x455a14ed); // 28
	GG(a,b,c,d,x[13],S21,0xa9e3e905); // 29
	GG(d,a,b,c,x[ 2],S22,0xfcefa3f8); // 30
	GG(c,d,a,b,x[ 7],S23,0x676f02d9); // 31
	GG(b,c,d,a,x[12],S24,0x8d2a4c8a); // 32

	// Round 3
	HH(a,b,c,d,x[ 5],S31,0xfffa3942); // 33
	HH(d,a,b,c,x[ 8],S32,0x8771f681); // 34
	HH(c,d,a,b,x[11],S33,0x6d9d6122); // 35
	HH(b,c,d,a,x[14],S34,0xfde5380c); // 36
	HH(a,b,c,d,x[ 1],S31,0xa4beea44); // 37
	HH(d,a,b,c,x[ 4],S32,0x4bdecfa9); // 38
	HH(c,d,a,b,x[ 7],S33,0xf6bb4b60); // 39
	HH(b,c,d,a,x[10],S34,0xbebfbc70); // 40
	HH(a,b,c,d,x[13],S31,0x289b7ec6); // 41
	HH(d,a,b,c,x[ 0],S32,0xeaa127fa); // 42
	HH(c,d,a,b,x[ 3],S33,0xd4ef3085); // 43
	HH(b,c,d,a,x[ 6],S34,0x04881d05); // 44
	HH(a,b,c,d,x[ 9],S31,0xd9d4d039); // 45
	HH(d,a,b,c,x[12],S32,0xe6db99e5); // 46
	HH(c,d,a,b,x[15],S33,0x1fa27cf8); // 47
	HH(b,c,d,a,x[ 2],S34,0xc4ac5665); // 48

	// Round 4

	II(a,b,c,d,x[ 0],S41,0xf4292244); // 49
	II(d,a,b,c,x[ 7],S42,0x432aff97); // 50
	II(c,d,a,b,x[14],S43,0xab9423a7); // 51
	II(b,c,d,a,x[ 5],S44,0xfc93a039); // 52
	II(a,b,c,d,x[12],S41,0x655b59c3); // 53
	II(d,a,b,c,x[ 3],S42,0x8f0ccc92); // 54
	II(c,d,a,b,x[10],S43,0xffeff47d); // 55
	II(b,c,d,a,x[ 1],S44,0x85845dd1); // 56
	II(a,b,c,d,x[ 8],S41,0x6fa87e4f); // 57
	II(d,a,b,c,x[15],S42,0xfe2ce6e0); // 58
	II(c,d,a,b,x[ 6],S43,0xa3014314); // 59
	II(b,c,d,a,x[13],S44,0x4e0811a1); // 60
	II(a,b,c,d,x[ 4],S41,0xf7537e82); // 61
	II(d,a,b,c,x[11],S42,0xbd3af235); // 62
	II(c,d,a,b,x[ 2],S43,0x2ad7d2bb); // 63
	II(b,c,d,a,x[ 9],S44,0xeb86d391); // 64

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

void Burger::MD5Hasher_t::Process(const void *pInput,WordPtr uLength)
{
	// Compute number of bytes mod 64
	WordPtr index = (Word)(m_uByteCount & 0x3F);

	// Update number of bits (Perform a 64 bit add)
	m_uByteCount += uLength;

	WordPtr i = 64 - index;

	// Transform as many times as possible.

	if (uLength >= i) {		// Should I copy or pack?

		MemoryCopy(&m_CacheBuffer[index],pInput,i);
		Process(m_CacheBuffer);

		// Perform the checksum directly on the memory buffers

		if ((i+63)<uLength) {
			do {
	 			Process(&static_cast<const Word8 *>(pInput)[i]);
				i += 64;
			} while ((i+63) < uLength);
		}
		index = 0;
	} else {
		i = 0;
	}

	// Buffer remaining input
	MemoryCopy(&m_CacheBuffer[index],&static_cast<const Word8 *>(pInput)[i],uLength-i);
}

/*! ************************************

	\brief Finalize the hashing

	When multi-pass hashing is performed, this call is necessary to
	finalize the hash so that the generated checksum can
	be applied into the hash

	\sa Init(void), Process(const void *,WordPtr)

***************************************/

void Burger::MD5Hasher_t::Finalize(void)
{
	Word8 Padding[64];		// Pad array, first byte is 0x80, rest 0
	Padding[0] = 0x80;
	MemoryClear(&Padding[1],63);

	// Save number of bits

	Word64 uBitCountLE = LittleEndian::Load(m_uByteCount<<3);

	// Pad out to 56 mod 64.
	// Convert to 1-64
	WordPtr uPadLen = ((55-static_cast<WordPtr>(m_uByteCount))&0x3f)+1;	
	Process(Padding,uPadLen);

	// Append length (before padding)
	Process(&uBitCountLE,8);
}

/*! ************************************

	\brief Quickly create an MD5 key
	
	Given a buffer of data, generate the MD5 hash key

	\param pOutput Pointer to an unitialized MD5_t structure
	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash

	\sa Burger::MD5Hasher_t

***************************************/

void BURGER_API Burger::Hash(MD5_t *pOutput,const void *pInput,WordPtr uLength)
{
	Burger::MD5Hasher_t Context;
	// Initialize
	Context.Init();
	// Process all of the data
	Context.Process(pInput,uLength);
	// Wrap up the processing
	Context.Finalize();
	// Return the resulting hash
	MemoryCopy(pOutput,&Context.m_Hash,16);
}
