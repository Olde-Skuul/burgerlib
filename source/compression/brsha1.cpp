/***************************************

	SHA-1 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/SHA-1
	and http://tools.ietf.org/html/rfc3174

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brsha1.h"
#include "brendian.h"
#include "brstringfunctions.h"
#include "brfixedpoint.h"

/*! ************************************

	\struct Burger::SHA1_t
	\brief 20 byte array to contain a SHA-1 hash
	
	Full documentation on this hash format can be found here
	http://en.wikipedia.org/wiki/SHA-1

	\sa Hash(SHA1_t *,const void *,WordPtr) and Burger::SHA1Hasher_t

***************************************/

/*! ************************************

	\struct Burger::SHA1Hasher_t
	\brief Multi-pass SHA1 hash generator

	This structure is needed to perform a multi-pass SHA-1 hash
	and contains cached data and a running checksum.
	\code
		Burger::SHA1Hasher_t Context;
		// Initialize
		Context.Init();
		// Process data in passes
		Context.Process(Buffer1,sizeof(Buffer1));
		Context.Process(Buffer2,sizeof(Buffer2));
		Context.Process(Buffer3,sizeof(Buffer3));
		// Wrap up the processing
		Context.Finalize();
		// Return the resulting hash
		MemoryCopy(pOutput,&Context.m_Hash,20);
	\endcode

	\sa Burger::SHA1_t or Hash(SHA1_t *,const void *,WordPtr)

***************************************/

/*! ************************************

	\brief Initialize the SHA-1 hasher

	Call this function before any hashing is performed

	\sa Process(const void *,WordPtr) or Finalize(void)

***************************************/

void Burger::SHA1Hasher_t::Init(void)
{
	// Load magic initialization constants.

#if !defined(BURGER_LITTLEENDIAN)
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[0] = 0x67452301;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[1] = 0xefcdab89;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[2] = 0x98badcfe;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[3] = 0x10325476;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[4] = 0xc3d2e1f0;
#else
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[0] = 0x01234567;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[1] = 0x89abcdef;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[2] = 0xfedcba98;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[3] = 0x76543210;
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[4] = 0xf0e1d2c3;
#endif
	m_uByteCount = 0;
}

/*! ************************************

	\brief Process a single 64 byte block of data

	MD5 data is processed in 64 byte chunks. This function
	will process 64 bytes on input and update the hash and checksum

	\param pBlock Pointer to a buffer of 64 bytes of data to hash
	\sa Process(const void *,WordPtr), Finalize(void) or Init(void)

***************************************/

void Burger::SHA1Hasher_t::Process(const Word8 *pBlock)
{
	Word32 block[16];

	Word32 hasha = BigEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[0]);
	Word32 hashb = BigEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[1]);
	Word32 hashc = BigEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[2]);
	Word32 hashd = BigEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[3]);
	Word32 hashe = BigEndian::Load(&reinterpret_cast<Word32 *>(m_Hash.m_Hash)[4]);

	Word32 i = 0;
	do {
		block[i] = Burger::BigEndian::LoadAny(&reinterpret_cast<const Word32 *>(pBlock)[i]);
	} while (++i<16);

	// Make a copy of the hash integers 
	Word32 a = hasha;
	Word32 b = hashb;
	Word32 c = hashc;
	Word32 d = hashd;
	Word32 e = hashe;

#if !defined(DOXYGEN)
#define blk(i) (block[i&15] = RotateLeft(block[(i+13)&15] ^ block[(i+8)&15] ^ block[(i+2)&15] ^ block[i&15],1))

	// (R0+R1), R2, R3, R4 are the different operations used in SHA1
#define R0(v,w,x,y,z,i) z += ((w&(x^y))^y) + block[i] + 0x5a827999 + RotateLeft(v,5); w=RotateLeft(w,30);
#define R1(v,w,x,y,z,i) z += ((w&(x^y))^y) + blk(i) + 0x5a827999 + RotateLeft(v,5); w=RotateLeft(w,30);
#define R2(v,w,x,y,z,i) z += (w^x^y) + blk(i) + 0x6ed9eba1 + RotateLeft(v,5); w=RotateLeft(w,30);
#define R3(v,w,x,y,z,i) z += (((w|x)&y)|(w&x)) + blk(i) + 0x8f1bbcdc + RotateLeft(v,5); w=RotateLeft(w,30);
#define R4(v,w,x,y,z,i) z += (w^x^y) + blk(i) + 0xca62c1d6 + RotateLeft(v,5); w=RotateLeft(w,30);
#endif

	// 4 rounds of 20 operations each. Loop unrolled.
	R0(a,b,c,d,e, 0);
	R0(e,a,b,c,d, 1);
	R0(d,e,a,b,c, 2);
	R0(c,d,e,a,b, 3);
	R0(b,c,d,e,a, 4);
	R0(a,b,c,d,e, 5);
	R0(e,a,b,c,d, 6);
	R0(d,e,a,b,c, 7);
	R0(c,d,e,a,b, 8);
	R0(b,c,d,e,a, 9);
	R0(a,b,c,d,e,10);
	R0(e,a,b,c,d,11);
	R0(d,e,a,b,c,12);
	R0(c,d,e,a,b,13);
	R0(b,c,d,e,a,14);
	R0(a,b,c,d,e,15);
	R1(e,a,b,c,d,16);
	R1(d,e,a,b,c,17);
	R1(c,d,e,a,b,18);
	R1(b,c,d,e,a,19);
	R2(a,b,c,d,e,20);
	R2(e,a,b,c,d,21);
	R2(d,e,a,b,c,22);
	R2(c,d,e,a,b,23);
	R2(b,c,d,e,a,24);
	R2(a,b,c,d,e,25);
	R2(e,a,b,c,d,26);
	R2(d,e,a,b,c,27);
	R2(c,d,e,a,b,28);
	R2(b,c,d,e,a,29);
	R2(a,b,c,d,e,30);
	R2(e,a,b,c,d,31);
	R2(d,e,a,b,c,32);
	R2(c,d,e,a,b,33);
	R2(b,c,d,e,a,34);
	R2(a,b,c,d,e,35);
	R2(e,a,b,c,d,36);
	R2(d,e,a,b,c,37);
	R2(c,d,e,a,b,38);
	R2(b,c,d,e,a,39);
	R3(a,b,c,d,e,40);
	R3(e,a,b,c,d,41);
	R3(d,e,a,b,c,42);
	R3(c,d,e,a,b,43);
	R3(b,c,d,e,a,44);
	R3(a,b,c,d,e,45);
	R3(e,a,b,c,d,46);
	R3(d,e,a,b,c,47);
	R3(c,d,e,a,b,48);
	R3(b,c,d,e,a,49);
	R3(a,b,c,d,e,50);
	R3(e,a,b,c,d,51);
	R3(d,e,a,b,c,52);
	R3(c,d,e,a,b,53);
	R3(b,c,d,e,a,54);
	R3(a,b,c,d,e,55);
	R3(e,a,b,c,d,56);
	R3(d,e,a,b,c,57);
	R3(c,d,e,a,b,58);
	R3(b,c,d,e,a,59);
	R4(a,b,c,d,e,60);
	R4(e,a,b,c,d,61);
	R4(d,e,a,b,c,62);
	R4(c,d,e,a,b,63);
	R4(b,c,d,e,a,64);
	R4(a,b,c,d,e,65);
	R4(e,a,b,c,d,66);
	R4(d,e,a,b,c,67);
	R4(c,d,e,a,b,68);
	R4(b,c,d,e,a,69);
	R4(a,b,c,d,e,70);
	R4(e,a,b,c,d,71);
	R4(d,e,a,b,c,72);
	R4(c,d,e,a,b,73);
	R4(b,c,d,e,a,74);
	R4(a,b,c,d,e,75);
	R4(e,a,b,c,d,76);
	R4(d,e,a,b,c,77);
	R4(c,d,e,a,b,78);
	R4(b,c,d,e,a,79);

	// Add in the adjusted hash (Store in little endian format)

	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[0] = BigEndian::Load(a+hasha);
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[1] = BigEndian::Load(b+hashb);
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[2] = BigEndian::Load(c+hashc);
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[3] = BigEndian::Load(d+hashd);
	reinterpret_cast<Word32 *>(m_Hash.m_Hash)[4] = BigEndian::Load(e+hashe);
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

void Burger::SHA1Hasher_t::Process(const void *pInput,WordPtr uLength)
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

void Burger::SHA1Hasher_t::Finalize(void)
{
	Word8 Padding[64];		// Pad array, first byte is 0x80, rest 0
	Padding[0] = 0x80;
	MemoryClear(&Padding[1],63);

	// Save number of bits

	Word64 uBitCountBE = BigEndian::Load(m_uByteCount<<3);

	// Pad out to 56 mod 64.
	// Convert to 1-64
	WordPtr uPadLen = ((55-static_cast<WordPtr>(m_uByteCount))&0x3f)+1;	
	Process(Padding,uPadLen);

	// Append length (before padding)
	Process(&uBitCountBE,8);
}

/*! ************************************

	\brief Quickly create a SHA-1 key
	
	Given a buffer of data, generate the SHA-1 hash key

	\param pOutput Pointer to an unitialized SHA1_t structure
	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash

	\sa Burger::SHA1Hasher_t

***************************************/

void BURGER_API Burger::Hash(SHA1_t *pOutput,const void *pInput,WordPtr uLength)
{
	Burger::SHA1Hasher_t Context;
	// Initialize
	Context.Init();
	// Process all of the data
	Context.Process(pInput,uLength);
	// Wrap up the processing
	Context.Finalize();
	// Return the resulting hash
	MemoryCopy(pOutput,&Context.m_Hash,20);
}
