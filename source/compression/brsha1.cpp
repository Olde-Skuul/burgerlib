/***************************************

	SHA-1 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/SHA-1
	and http://tools.ietf.org/html/rfc3174

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsha1.h"
#include "brendian.h"
#include "brfixedpoint.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\struct Burger::SHA1_t
	\brief 20 byte array to contain a SHA-1 hash

	Full documentation on this hash format can be found here
	http://en.wikipedia.org/wiki/SHA-1

	\sa hash(SHA1_t *,const void *,uintptr_t) and \ref SHA1Hasher_t

***************************************/

/*! ************************************

	\struct Burger::SHA1Hasher_t
	\brief Multi-pass SHA1 hash generator

	This structure is needed to perform a multi-pass SHA-1 hash and contains
	cached data and a running checksum.

	\code
		Burger::SHA1Hasher_t Context;
		// Initialize
		Context.init();
		// Process data in passes
		Context.process(Buffer1,sizeof(Buffer1));
		Context.process(Buffer2,sizeof(Buffer2));
		Context.process(Buffer3,sizeof(Buffer3));
		// Wrap up the processing
		Context.finalize();
		// Return the resulting hash
		MemoryCopy(pOutput,&Context.m_Hash,20);
	\endcode

	\sa \ref SHA1_t or hash(SHA1_t *,const void *,uintptr_t)

***************************************/

/*! ************************************

	\brief Initialize the SHA-1 hasher

	Call this function before any hashing is performed

	\sa process(const void *,uintptr_t) or finalize(void)

***************************************/

void BURGER_API Burger::SHA1Hasher_t::init(void) BURGER_NOEXCEPT
{
	// Load magic initialization constants.

	uint32_t* pHash32 =
		static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash));

#if !defined(BURGER_LITTLEENDIAN)
	pHash32[0] = 0x67452301;
	pHash32[1] = 0xefcdab89;
	pHash32[2] = 0x98badcfe;
	pHash32[3] = 0x10325476;
	pHash32[4] = 0xc3d2e1f0;
#else
	pHash32[0] = 0x01234567;
	pHash32[1] = 0x89abcdef;
	pHash32[2] = 0xfedcba98;
	pHash32[3] = 0x76543210;
	pHash32[4] = 0xf0e1d2c3;
#endif
	m_uByteCount = 0;
}

/*! ************************************

	\brief Process a single 64 byte block of data

	SHA1 data is processed in 64 byte chunks. This function will process 64
	bytes on input and update the hash and checksum

	\param pBlock Pointer to a buffer of 64 bytes of data to hash

	\sa process(const void *,uintptr_t), finalize(void) or init(void)

***************************************/

void BURGER_API Burger::SHA1Hasher_t::process(
	const uint8_t pBlock[64]) BURGER_NOEXCEPT
{
	uint32_t DataBlock[16];
	uintptr_t i = 0;
	const uint32_t* pBlock32 =
		static_cast<const uint32_t*>(static_cast<const void*>(pBlock));
	do {
		DataBlock[i] = BigEndian::LoadAny(pBlock32 + i);
	} while (++i < 16U);

	// Make a copy of the hash integers
	uint32_t a = BigEndian::Load(
		static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 0)));
	uint32_t b = BigEndian::Load(
		static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 4)));
	uint32_t c = BigEndian::Load(
		static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 8)));
	uint32_t d = BigEndian::Load(
		static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 12)));
	uint32_t e = BigEndian::Load(
		static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 16)));

#if !defined(DOXYGEN)
#define blk(i) \
	(DataBlock[i & 15] = \
			RotateLeft(DataBlock[(i + 13) & 15] ^ DataBlock[(i + 8) & 15] ^ \
					DataBlock[(i + 2) & 15] ^ DataBlock[i & 15], \
				1))

	// (R0+R1), R2, R3, R4 are the different operations used in SHA1
#define R0(v, w, x, y, z, i) \
	z += RotateLeft(v, 5); \
	z += 0x5a827999; \
	z += DataBlock[i]; \
	z += (((x ^ y) & w) ^ y); \
	w = RotateLeft(w, 30)
#define R1(v, w, x, y, z, i) \
	z += RotateLeft(v, 5); \
	z += 0x5a827999; \
	z += blk(i); \
	z += (((x ^ y) & w) ^ y); \
	w = RotateLeft(w, 30)
#define R2(v, w, x, y, z, i) \
	z += RotateLeft(v, 5); \
	z += 0x6ed9eba1; \
	z += blk(i); \
	z += (w ^ x ^ y); \
	w = RotateLeft(w, 30)
#define R3(v, w, x, y, z, i) \
	z += RotateLeft(v, 5); \
	z += 0x8f1bbcdc; \
	z += blk(i); \
	z += (((w | x) & y) | (w & x)); \
	w = RotateLeft(w, 30)
#define R4(v, w, x, y, z, i) \
	z += RotateLeft(v, 5); \
	z += 0xca62c1d6; \
	z += blk(i); \
	z += (w ^ x ^ y); \
	w = RotateLeft(w, 30)
#endif

	// 4 rounds of 20 operations each. Loop unrolled.
	R0(a, b, c, d, e, 0);
	R0(e, a, b, c, d, 1);
	R0(d, e, a, b, c, 2);
	R0(c, d, e, a, b, 3);
	R0(b, c, d, e, a, 4);
	R0(a, b, c, d, e, 5);
	R0(e, a, b, c, d, 6);
	R0(d, e, a, b, c, 7);
	R0(c, d, e, a, b, 8);
	R0(b, c, d, e, a, 9);
	R0(a, b, c, d, e, 10);
	R0(e, a, b, c, d, 11);
	R0(d, e, a, b, c, 12);
	R0(c, d, e, a, b, 13);
	R0(b, c, d, e, a, 14);
	R0(a, b, c, d, e, 15);

	R1(e, a, b, c, d, 16);
	R1(d, e, a, b, c, 17);
	R1(c, d, e, a, b, 18);
	R1(b, c, d, e, a, 19);

	R2(a, b, c, d, e, 20);
	R2(e, a, b, c, d, 21);
	R2(d, e, a, b, c, 22);
	R2(c, d, e, a, b, 23);
	R2(b, c, d, e, a, 24);
	R2(a, b, c, d, e, 25);
	R2(e, a, b, c, d, 26);
	R2(d, e, a, b, c, 27);
	R2(c, d, e, a, b, 28);
	R2(b, c, d, e, a, 29);
	R2(a, b, c, d, e, 30);
	R2(e, a, b, c, d, 31);
	R2(d, e, a, b, c, 32);
	R2(c, d, e, a, b, 33);
	R2(b, c, d, e, a, 34);
	R2(a, b, c, d, e, 35);
	R2(e, a, b, c, d, 36);
	R2(d, e, a, b, c, 37);
	R2(c, d, e, a, b, 38);
	R2(b, c, d, e, a, 39);

	R3(a, b, c, d, e, 40);
	R3(e, a, b, c, d, 41);
	R3(d, e, a, b, c, 42);
	R3(c, d, e, a, b, 43);
	R3(b, c, d, e, a, 44);
	R3(a, b, c, d, e, 45);
	R3(e, a, b, c, d, 46);
	R3(d, e, a, b, c, 47);
	R3(c, d, e, a, b, 48);
	R3(b, c, d, e, a, 49);
	R3(a, b, c, d, e, 50);
	R3(e, a, b, c, d, 51);
	R3(d, e, a, b, c, 52);
	R3(c, d, e, a, b, 53);
	R3(b, c, d, e, a, 54);
	R3(a, b, c, d, e, 55);
	R3(e, a, b, c, d, 56);
	R3(d, e, a, b, c, 57);
	R3(c, d, e, a, b, 58);
	R3(b, c, d, e, a, 59);

	R4(a, b, c, d, e, 60);
	R4(e, a, b, c, d, 61);
	R4(d, e, a, b, c, 62);
	R4(c, d, e, a, b, 63);
	R4(b, c, d, e, a, 64);
	R4(a, b, c, d, e, 65);
	R4(e, a, b, c, d, 66);
	R4(d, e, a, b, c, 67);
	R4(c, d, e, a, b, 68);
	R4(b, c, d, e, a, 69);
	R4(a, b, c, d, e, 70);
	R4(e, a, b, c, d, 71);
	R4(d, e, a, b, c, 72);
	R4(c, d, e, a, b, 73);
	R4(b, c, d, e, a, 74);
	R4(a, b, c, d, e, 75);
	R4(e, a, b, c, d, 76);
	R4(d, e, a, b, c, 77);
	R4(c, d, e, a, b, 78);
	R4(b, c, d, e, a, 79);

	// Add in the adjusted hash (Store in big endian format)

	{
		BigEndian::Store(
			static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 0)),
			BigEndian::Load(
				static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 0))) +
				a);
		BigEndian::Store(
			static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 4)),
			BigEndian::Load(
				static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 4))) +
				b);
		BigEndian::Store(
			static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 8)),
			BigEndian::Load(
				static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 8))) +
				c);
		BigEndian::Store(
			static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 12)),
			BigEndian::Load(static_cast<uint32_t*>(
				static_cast<void*>(m_Hash.m_Hash + 12))) +
				d);
		BigEndian::Store(
			static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash + 16)),
			BigEndian::Load(static_cast<uint32_t*>(
				static_cast<void*>(m_Hash.m_Hash + 16))) +
				e);
	}
}

/*! ************************************

	\brief Process an arbitrary number of input bytes

	Process input data into the hash. If data chunks are not a multiple of 64
	bytes, the excess will be cached and a future call will continue the hashing
	where it left off.

	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash

	\sa process(const uint8_t[64]), finalize(void)

***************************************/

void BURGER_API Burger::SHA1Hasher_t::process(
	const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Compute number of bytes mod 64
	uintptr_t uIndex = static_cast<uintptr_t>(m_uByteCount) & 0x3FU;

	// Update number of bits (Perform a 64 bit add)
	m_uByteCount += uLength;

	uintptr_t i = 64U - uIndex;

	// Transform as many times as possible.

	if (uLength >= i) { // Should I copy or pack?

		MemoryCopy(&m_CacheBuffer[uIndex], pInput, i);
		process(m_CacheBuffer);

		// Perform the checksum directly on the memory buffers

		if ((i + 63U) < uLength) {
			do {
				process(static_cast<const uint8_t*>(pInput) + i);
				i += 64U;
			} while ((i + 63U) < uLength);
		}
		uIndex = 0;
	} else {
		i = 0;
	}

	// Buffer remaining input
	MemoryCopy(&m_CacheBuffer[uIndex], static_cast<const uint8_t*>(pInput) + i,
		uLength - i);
}

/*! ************************************

	\brief Finalize the hashing

	When multi-pass hashing is performed, this call is necessary to finalize the
	hash so that the generated checksum can be applied into the hash

	\sa init(void), process(const void *,uintptr_t)

***************************************/

void BURGER_API Burger::SHA1Hasher_t::finalize(void) BURGER_NOEXCEPT
{
	// Pad array, first byte is 0x80, rest 0
	uint8_t Padding[64];
	Padding[0] = 0x80;
	MemoryClear(&Padding[1], 63);

	// Save number of bits

	const uint64_t uBitCountBE = BigEndian::Load(m_uByteCount << 3U);

	// Pad out to 56 mod 64.
	// Convert to 1-64
	const uintptr_t uPadLen =
		((55U - static_cast<uintptr_t>(m_uByteCount)) & 0x3FU) + 1U;
	process(Padding, uPadLen);

	// Append length (before padding)
	process(&uBitCountBE, 8);
}

/*! ************************************

	\brief Quickly create a SHA-1 key

	Given a buffer of data, generate the SHA-1 hash key

	\param pOutput Pointer to an uninitialized SHA1_t structure
	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash

	\sa \ref SHA1Hasher_t

***************************************/

void BURGER_API Burger::hash(
	SHA1_t* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	SHA1Hasher_t Context;

	// Initialize
	Context.init();

	// Process all of the data
	Context.process(pInput, uLength);

	// Wrap up the processing
	Context.finalize();

	// Return the resulting hash
	MemoryCopy(pOutput, &Context.m_Hash, 20);
}
