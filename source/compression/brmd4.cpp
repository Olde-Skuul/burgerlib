/***************************************

	MD4 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/MD4
	and http://tools.ietf.org/html/rfc1320

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmd4.h"
#include "brendian.h"
#include "brfixedpoint.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\struct Burger::MD4_t
	\brief 16 byte array to contain an MD4 hash

	Full documentation on this hash format can be found here
	http://en.wikipedia.org/wiki/MD4

	\sa hash(MD4_t *,const void *,uintptr_t) and \ref MD4Hasher_t

***************************************/

/*! ************************************

	\struct Burger::MD4Hasher_t
	\brief Multi-pass MD4 hash generator

	This structure is needed to perform a multi-pass MD4 hash
	and contains cached data and a running checksum.
	\code
		Burger::MD4Hasher_t Context;
		// Initialize
		Context.init();
		// Process data in passes
		Context.process(Buffer1,sizeof(Buffer1));
		Context.process(Buffer2,sizeof(Buffer2));
		Context.process(Buffer3,sizeof(Buffer3));
		// Wrap up the processing
		Context.finalize();
		// Return the resulting hash
		memory_copy(pOutput,&Context.m_Hash,16);
	\endcode

	\sa \ref MD4_t or hash(MD4_t *,const void *,uintptr_t)

***************************************/

/*! ************************************

	\brief Initialize the MD4 hasher

	Call this function before any hashing is performed

	\sa process(const void *,uintptr_t) or finalize(void)

***************************************/

void BURGER_API Burger::MD4Hasher_t::init(void) BURGER_NOEXCEPT
{
	// Load magic initialization constants.

	uint32_t* pHash32 =
		static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash));

#if defined(BURGER_LITTLEENDIAN)
	pHash32[0] = 0x67452301;
	pHash32[1] = 0xefcdab89;
	pHash32[2] = 0x98badcfe;
	pHash32[3] = 0x10325476;
#else
	pHash32[0] = 0x01234567;
	pHash32[1] = 0x89abcdef;
	pHash32[2] = 0xfedcba98;
	pHash32[3] = 0x76543210;
#endif
	m_uByteCount = 0;
}

// Constants for MD4Transform routine.

#if !defined(DOXYGEN)
#define SHIFT411 3
#define SHIFT412 7
#define SHIFT413 11
#define SHIFT414 19
#define SHIFT421 3
#define SHIFT422 5
#define SHIFT423 9
#define SHIFT424 13
#define SHIFT431 3
#define SHIFT432 9
#define SHIFT433 11
#define SHIFT434 15

// FF, GG and HH are transformations for rounds 1, 2 and 3

#define FF4(a, b, c, d, x, s) a = RotateLeft(((b & c) | ((~b) & d)) + x + a, s)
#define GG4(a, b, c, d, x, s) \
	a = RotateLeft(((b & c) | (b & d) | (c & d)) + x + a + 0x5a827999U, s)
#define HH4(a, b, c, d, x, s) \
	a = RotateLeft((b ^ c ^ d) + x + a + 0x6ed9eba1U, s)

#endif

/*! ************************************

	\brief Process a single 64 byte block of data

	MD4 data is processed in 64 byte chunks. This function will process 64 bytes
	on input and update the hash and checksum

	\param pBlock Pointer to a buffer of 64 bytes of data to hash

	\sa process(const void *,uintptr_t), finalize(void) or init(void)

***************************************/

void BURGER_API Burger::MD4Hasher_t::process(
	const uint8_t pBlock[64]) BURGER_NOEXCEPT
{
	// Prefetch the 64 bytes into registers (Taking into account data
	// misalignment)

	const uint32_t* pBlock32 =
		static_cast<const uint32_t*>(static_cast<const void*>(pBlock));
	const uint32_t x0 = LittleEndian::load_unaligned(pBlock32);
	const uint32_t x1 = LittleEndian::load_unaligned(pBlock32 + 1);
	const uint32_t x2 = LittleEndian::load_unaligned(pBlock32 + 2);
	const uint32_t x3 = LittleEndian::load_unaligned(pBlock32 + 3);
	const uint32_t x4 = LittleEndian::load_unaligned(pBlock32 + 4);
	const uint32_t x5 = LittleEndian::load_unaligned(pBlock32 + 5);
	const uint32_t x6 = LittleEndian::load_unaligned(pBlock32 + 6);
	const uint32_t x7 = LittleEndian::load_unaligned(pBlock32 + 7);
	const uint32_t x8 = LittleEndian::load_unaligned(pBlock32 + 8);
	const uint32_t x9 = LittleEndian::load_unaligned(pBlock32 + 9);
	const uint32_t x10 = LittleEndian::load_unaligned(pBlock32 + 10);
	const uint32_t x11 = LittleEndian::load_unaligned(pBlock32 + 11);
	const uint32_t x12 = LittleEndian::load_unaligned(pBlock32 + 12);
	const uint32_t x13 = LittleEndian::load_unaligned(pBlock32 + 13);
	const uint32_t x14 = LittleEndian::load_unaligned(pBlock32 + 14);
	const uint32_t x15 = LittleEndian::load_unaligned(pBlock32 + 15);

	// Convert endian on big endian machines
	uint32_t* pHash32 =
		static_cast<uint32_t*>(static_cast<void*>(m_Hash.m_Hash));

	// Make a copy of the hash integers
	uint32_t a = LittleEndian::load(pHash32);
	uint32_t b = LittleEndian::load(pHash32 + 1);
	uint32_t c = LittleEndian::load(pHash32 + 2);
	uint32_t d = LittleEndian::load(pHash32 + 3);

	// Round 1
	FF4(a, b, c, d, x0, SHIFT411);  // 1
	FF4(d, a, b, c, x1, SHIFT412);  // 2
	FF4(c, d, a, b, x2, SHIFT413);  // 3
	FF4(b, c, d, a, x3, SHIFT414);  // 4
	FF4(a, b, c, d, x4, SHIFT411);  // 5
	FF4(d, a, b, c, x5, SHIFT412);  // 6
	FF4(c, d, a, b, x6, SHIFT413);  // 7
	FF4(b, c, d, a, x7, SHIFT414);  // 8
	FF4(a, b, c, d, x8, SHIFT411);  // 9
	FF4(d, a, b, c, x9, SHIFT412);  // 10
	FF4(c, d, a, b, x10, SHIFT413); // 11
	FF4(b, c, d, a, x11, SHIFT414); // 12
	FF4(a, b, c, d, x12, SHIFT411); // 13
	FF4(d, a, b, c, x13, SHIFT412); // 14
	FF4(c, d, a, b, x14, SHIFT413); // 15
	FF4(b, c, d, a, x15, SHIFT414); // 16

	// Round 2
	GG4(a, b, c, d, x0, SHIFT421);  // 17
	GG4(d, a, b, c, x4, SHIFT422);  // 18
	GG4(c, d, a, b, x8, SHIFT423);  // 19
	GG4(b, c, d, a, x12, SHIFT424); // 20
	GG4(a, b, c, d, x1, SHIFT421);  // 21
	GG4(d, a, b, c, x5, SHIFT422);  // 22
	GG4(c, d, a, b, x9, SHIFT423);  // 23
	GG4(b, c, d, a, x13, SHIFT424); // 24
	GG4(a, b, c, d, x2, SHIFT421);  // 25
	GG4(d, a, b, c, x6, SHIFT422);  // 26
	GG4(c, d, a, b, x10, SHIFT423); // 27
	GG4(b, c, d, a, x14, SHIFT424); // 28
	GG4(a, b, c, d, x3, SHIFT421);  // 29
	GG4(d, a, b, c, x7, SHIFT422);  // 30
	GG4(c, d, a, b, x11, SHIFT423); // 31
	GG4(b, c, d, a, x15, SHIFT424); // 32

	// Round 3
	HH4(a, b, c, d, x0, SHIFT431);  // 33
	HH4(d, a, b, c, x8, SHIFT432);  // 34
	HH4(c, d, a, b, x4, SHIFT433);  // 35
	HH4(b, c, d, a, x12, SHIFT434); // 36
	HH4(a, b, c, d, x2, SHIFT431);  // 37
	HH4(d, a, b, c, x10, SHIFT432); // 38
	HH4(c, d, a, b, x6, SHIFT433);  // 39
	HH4(b, c, d, a, x14, SHIFT434); // 40
	HH4(a, b, c, d, x1, SHIFT431);  // 41
	HH4(d, a, b, c, x9, SHIFT432);  // 42
	HH4(c, d, a, b, x5, SHIFT433);  // 43
	HH4(b, c, d, a, x13, SHIFT434); // 44
	HH4(a, b, c, d, x3, SHIFT431);  // 45
	HH4(d, a, b, c, x11, SHIFT432); // 46
	HH4(c, d, a, b, x7, SHIFT433);  // 47
	HH4(b, c, d, a, x15, SHIFT434); // 48

	// Add in the adjusted hash (Store in little endian format)

	LittleEndian::store(pHash32, LittleEndian::load(pHash32) + a);
	LittleEndian::store(pHash32 + 1, LittleEndian::load(pHash32 + 1) + b);
	LittleEndian::store(pHash32 + 2, LittleEndian::load(pHash32 + 2) + c);
	LittleEndian::store(pHash32 + 3, LittleEndian::load(pHash32 + 3) + d);
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

void BURGER_API Burger::MD4Hasher_t::process(
	const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Compute number of bytes mod 64
	uintptr_t uIndex = static_cast<uintptr_t>(m_uByteCount & 0x3FU);

	// Update number of bits
	m_uByteCount += uLength;

	uintptr_t i = 64U - uIndex;

	// Transform as many times as possible.
	if (uLength >= i) {
		memory_copy(&m_CacheBuffer[uIndex], pInput, i);
		process(m_CacheBuffer);

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

	memory_copy(&m_CacheBuffer[uIndex], static_cast<const uint8_t*>(pInput) + i,
		uLength - i);
}

/*! ************************************

	\brief Finalize the hashing

	When multi-pass hashing is performed, this call is necessary to finalize the
	hash so that the generated checksum can be applied into the hash

	\sa init(void), process(const void *,uintptr_t)

***************************************/

void BURGER_API Burger::MD4Hasher_t::finalize(void) BURGER_NOEXCEPT
{
	uint8_t Padding[64];
	Padding[0] = 0x80U;
	memory_clear(&Padding[1], 63U);

	// Save number of bits

	const uint64_t uBitCountLE = LittleEndian::load(m_uByteCount << 3U);

	// Pad out to 56 mod 64.
	// Convert to 1-64

	const uintptr_t uPadLen =
		(((56U - 1U) - static_cast<uintptr_t>(m_uByteCount)) & 0x3FU) + 1U;
	process(Padding, uPadLen);

	// Append length (before padding)

	process(&uBitCountLE, 8);
}

/*! ************************************

	\brief Quickly create an MD4 key

	Given a buffer of data, generate the MD4 hash key

	\param pOutput Pointer to an unitialized MD4_t structure
	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash

	\sa \ref MD4Hasher_t

***************************************/

void BURGER_API Burger::hash(
	MD4_t* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	MD4Hasher_t Context;

	// Initialize
	Context.init();

	// Process all of the data
	Context.process(pInput, uLength);

	// Wrap up the processing
	Context.finalize();

	// Return the resulting hash
	memory_copy(pOutput, &Context.m_Hash, 16);
}
