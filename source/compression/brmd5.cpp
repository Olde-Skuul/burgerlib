/***************************************

	MD5 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/MD5
	and http://tools.ietf.org/html/rfc1321

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmd5.h"
#include "brendian.h"
#include "brfixedpoint.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\struct Burger::MD5_t
	\brief 16 byte array to contain an MD5 hash

	Full documentation on this hash format can be found here
	http://en.wikipedia.org/wiki/MD5

	\sa hash(MD5_t *,const void *,uintptr_t) and \ref MD5Hasher_t

***************************************/

/*! ************************************

	\struct Burger::MD5Hasher_t
	\brief Multi-pass MD5 hash generator

	This structure is needed to perform a multi-pass MD5 hash
	and contains cached data and a running checksum.
	\code
		Burger::MD5Hasher_t Context;
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

	\sa \ref MD5_t or hash(MD5_t *,const void *,uintptr_t)

***************************************/

/*! ************************************

	\brief Initialize the MD5 hasher

	Call this function before any hashing is performed

	\sa process(const void *,uintptr_t) or finalize(void)

***************************************/

void BURGER_API Burger::MD5Hasher_t::init(void) BURGER_NOEXCEPT
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

// Constants for MD5Transform routine.

#if !defined(DOXYGEN)
#define SHIFT11 7
#define SHIFT12 12
#define SHIFT13 17
#define SHIFT14 22
#define SHIFT21 5
#define SHIFT22 9
#define SHIFT23 14
#define SHIFT24 20
#define SHIFT31 4
#define SHIFT32 11
#define SHIFT33 16
#define SHIFT34 23
#define SHIFT41 6
#define SHIFT42 10
#define SHIFT43 15
#define SHIFT44 21

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.

#define FF(a, b, c, d, x, s, ac) \
	a = RotateLeft(((b & c) | ((~b) & d)) + x + ac + a, s) + b
#define GG(a, b, c, d, x, s, ac) \
	a = RotateLeft(((b & d) | (c & (~d))) + x + ac + a, s) + b
#define HH(a, b, c, d, x, s, ac) a = RotateLeft((b ^ c ^ d) + x + ac + a, s) + b
#define II(a, b, c, d, x, s, ac) \
	a = RotateLeft((c ^ (b | (~d))) + x + ac + a, s) + b

#endif

/*! ************************************

	\brief Process a single 64 byte block of data

	MD5 data is processed in 64 byte chunks. This function will process 64 bytes
	on input and update the hash and checksum

	\param pBlock Pointer to a buffer of 64 bytes of data to hash

	\sa process(const void *,uintptr_t), finalize(void) or init(void)

***************************************/

void BURGER_API Burger::MD5Hasher_t::process(
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
	FF(a, b, c, d, x0, SHIFT11, 0xd76aa478U);  // 1
	FF(d, a, b, c, x1, SHIFT12, 0xe8c7b756U);  // 2
	FF(c, d, a, b, x2, SHIFT13, 0x242070dbU);  // 3
	FF(b, c, d, a, x3, SHIFT14, 0xc1bdceeeU);  // 4
	FF(a, b, c, d, x4, SHIFT11, 0xf57c0fafU);  // 5
	FF(d, a, b, c, x5, SHIFT12, 0x4787c62aU);  // 6
	FF(c, d, a, b, x6, SHIFT13, 0xa8304613U);  // 7
	FF(b, c, d, a, x7, SHIFT14, 0xfd469501U);  // 8
	FF(a, b, c, d, x8, SHIFT11, 0x698098d8U);  // 9
	FF(d, a, b, c, x9, SHIFT12, 0x8b44f7afU);  // 10
	FF(c, d, a, b, x10, SHIFT13, 0xffff5bb1U); // 11
	FF(b, c, d, a, x11, SHIFT14, 0x895cd7beU); // 12
	FF(a, b, c, d, x12, SHIFT11, 0x6b901122U); // 13
	FF(d, a, b, c, x13, SHIFT12, 0xfd987193U); // 14
	FF(c, d, a, b, x14, SHIFT13, 0xa679438eU); // 15
	FF(b, c, d, a, x15, SHIFT14, 0x49b40821U); // 16

	// Round 2
	GG(a, b, c, d, x1, SHIFT21, 0xf61e2562U);  // 17
	GG(d, a, b, c, x6, SHIFT22, 0xc040b340U);  // 18
	GG(c, d, a, b, x11, SHIFT23, 0x265e5a51U); // 19
	GG(b, c, d, a, x0, SHIFT24, 0xe9b6c7aaU);  // 20
	GG(a, b, c, d, x5, SHIFT21, 0xd62f105dU);  // 21
	GG(d, a, b, c, x10, SHIFT22, 0x02441453U); // 22
	GG(c, d, a, b, x15, SHIFT23, 0xd8a1e681U); // 23
	GG(b, c, d, a, x4, SHIFT24, 0xe7d3fbc8U);  // 24
	GG(a, b, c, d, x9, SHIFT21, 0x21e1cde6U);  // 25
	GG(d, a, b, c, x14, SHIFT22, 0xc33707d6U); // 26
	GG(c, d, a, b, x3, SHIFT23, 0xf4d50d87U);  // 27
	GG(b, c, d, a, x8, SHIFT24, 0x455a14edU);  // 28
	GG(a, b, c, d, x13, SHIFT21, 0xa9e3e905U); // 29
	GG(d, a, b, c, x2, SHIFT22, 0xfcefa3f8U);  // 30
	GG(c, d, a, b, x7, SHIFT23, 0x676f02d9U);  // 31
	GG(b, c, d, a, x12, SHIFT24, 0x8d2a4c8aU); // 32

	// Round 3
	HH(a, b, c, d, x5, SHIFT31, 0xfffa3942U);  // 33
	HH(d, a, b, c, x8, SHIFT32, 0x8771f681U);  // 34
	HH(c, d, a, b, x11, SHIFT33, 0x6d9d6122U); // 35
	HH(b, c, d, a, x14, SHIFT34, 0xfde5380cU); // 36
	HH(a, b, c, d, x1, SHIFT31, 0xa4beea44U);  // 37
	HH(d, a, b, c, x4, SHIFT32, 0x4bdecfa9U);  // 38
	HH(c, d, a, b, x7, SHIFT33, 0xf6bb4b60U);  // 39
	HH(b, c, d, a, x10, SHIFT34, 0xbebfbc70U); // 40
	HH(a, b, c, d, x13, SHIFT31, 0x289b7ec6U); // 41
	HH(d, a, b, c, x0, SHIFT32, 0xeaa127faU);  // 42
	HH(c, d, a, b, x3, SHIFT33, 0xd4ef3085U);  // 43
	HH(b, c, d, a, x6, SHIFT34, 0x04881d05U);  // 44
	HH(a, b, c, d, x9, SHIFT31, 0xd9d4d039U);  // 45
	HH(d, a, b, c, x12, SHIFT32, 0xe6db99e5U); // 46
	HH(c, d, a, b, x15, SHIFT33, 0x1fa27cf8U); // 47
	HH(b, c, d, a, x2, SHIFT34, 0xc4ac5665U);  // 48

	// Round 4

	II(a, b, c, d, x0, SHIFT41, 0xf4292244U);  // 49
	II(d, a, b, c, x7, SHIFT42, 0x432aff97U);  // 50
	II(c, d, a, b, x14, SHIFT43, 0xab9423a7U); // 51
	II(b, c, d, a, x5, SHIFT44, 0xfc93a039U);  // 52
	II(a, b, c, d, x12, SHIFT41, 0x655b59c3U); // 53
	II(d, a, b, c, x3, SHIFT42, 0x8f0ccc92U);  // 54
	II(c, d, a, b, x10, SHIFT43, 0xffeff47dU); // 55
	II(b, c, d, a, x1, SHIFT44, 0x85845dd1U);  // 56
	II(a, b, c, d, x8, SHIFT41, 0x6fa87e4fU);  // 57
	II(d, a, b, c, x15, SHIFT42, 0xfe2ce6e0U); // 58
	II(c, d, a, b, x6, SHIFT43, 0xa3014314U);  // 59
	II(b, c, d, a, x13, SHIFT44, 0x4e0811a1U); // 60
	II(a, b, c, d, x4, SHIFT41, 0xf7537e82U);  // 61
	II(d, a, b, c, x11, SHIFT42, 0xbd3af235U); // 62
	II(c, d, a, b, x2, SHIFT43, 0x2ad7d2bbU);  // 63
	II(b, c, d, a, x9, SHIFT44, 0xeb86d391U);  // 64

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

void BURGER_API Burger::MD5Hasher_t::process(
	const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Compute number of bytes mod 64
	uintptr_t uIndex = static_cast<uintptr_t>(m_uByteCount) & 0x3FU;

	// Update number of bits (Perform a 64 bit add)
	m_uByteCount += uLength;

	uintptr_t i = 64U - uIndex;

	// Transform as many times as possible.

	// Should I copy or pack?
	if (uLength >= i) {

		memory_copy(&m_CacheBuffer[uIndex], pInput, i);
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
	memory_copy(&m_CacheBuffer[uIndex], static_cast<const uint8_t*>(pInput) + i,
		uLength - i);
}

/*! ************************************

	\brief Finalize the hashing

	When multi-pass hashing is performed, this call is necessary to finalize the
	hash so that the generated checksum can be applied into the hash

	\sa init(void), process(const void *,uintptr_t)

***************************************/

void BURGER_API Burger::MD5Hasher_t::finalize(void) BURGER_NOEXCEPT
{
	// Pad array, first byte is 0x80, rest 0
	uint8_t Padding[64];
	Padding[0] = 0x80;
	memory_clear(&Padding[1], 63);

	// Save number of bits

	const uint64_t uBitCountLE = LittleEndian::load(m_uByteCount << 3U);

	// Pad out to 56 mod 64.
	// Convert to 1-64
	const uintptr_t uPadLen =
		((55U - static_cast<uintptr_t>(m_uByteCount)) & 0x3FU) + 1U;
	process(Padding, uPadLen);

	// Append length (before padding)
	process(&uBitCountLE, 8);
}

/*! ************************************

	\brief Quickly create an MD5 key

	Given a buffer of data, generate the MD5 hash key

	\param pOutput Pointer to an unitialized MD5_t structure
	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash

	\sa \ref MD5Hasher_t

***************************************/

void BURGER_API Burger::hash(
	MD5_t* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	MD5Hasher_t Context;

	// Initialize
	Context.init();

	// Process all of the data
	Context.process(pInput, uLength);

	// Wrap up the processing
	Context.finalize();

	// Return the resulting hash
	memory_copy(pOutput, &Context.m_Hash, 16);
}
