/***************************************

	MD2 hash manager

	Implemented following the documentation found in
	http://en.wikipedia.org/wiki/MD2_(cryptography)
	and http://www.ietf.org/rfc/rfc1319.txt

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brmd2.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\struct Burger::MD2_t
	\brief 16 byte array to contain an MD2 hash

	Full documentation on this hash format can be found here
	http://en.wikipedia.org/wiki/MD2_(cryptography)

	\sa Hash(MD2_t *,const void *,uintptr_t) and Burger::MD2Hasher_t

***************************************/

/*! ************************************

	\struct Burger::MD2Hasher_t
	\brief Multi-pass MD2 hash generator

	This structure is needed to perform a multi-pass MD2 hash
	and contains cached data and a running checksum.
	\code
		Burger::MD2Hasher_t Context;
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

	\sa Burger::MD2_t or Hash(MD2_t *,const void *,uintptr_t)

***************************************/

// Permutation of 0..255 constructed from the digits of pi. It gives a
// "random" nonlinear byte substitution operation.

BURGER_ALIGN(static const uint8_t, g_MD2PiTable[256], 16) = {41, 46, 67, 201,
	162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6, 19, 98, 167, 5, 243, 192,
	199, 115, 140, 152, 147, 43, 217, 188, 76, 130, 202, 30, 155, 87, 60, 253,
	212, 224, 22, 103, 66, 111, 24, 138, 23, 229, 18, 190, 78, 196, 214, 218,
	158, 222, 73, 160, 251, 245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21,
	178, 7, 63, 148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144,
	50, 39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165, 181,
	209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210, 150, 164, 125,
	182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157, 112, 89, 100, 113, 135,
	32, 134, 91, 207, 101, 230, 45, 168, 2, 27, 96, 37, 173, 174, 176, 185, 246,
	28, 70, 97, 105, 52, 64, 126, 15, 85, 71, 163, 35, 221, 81, 175, 58, 195,
	92, 249, 206, 186, 197, 234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223,
	205, 244, 65, 129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225,
	123, 8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233, 203,
	213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228, 166, 119,
	114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237, 31, 26, 219, 153,
	141, 51, 159, 17, 131, 20};

/*! ************************************

	\brief Initialize the MD2 hasher
	\sa Process(const void *,uintptr_t) or Finalize(void)

***************************************/

void BURGER_API Burger::MD2Hasher_t::Init(void) BURGER_NOEXCEPT
{
	// Just erase the structure
	MemoryClear(this, sizeof(*this));
}

/*! ************************************

	\brief Process a single 16 byte block of data

	MD2 data is processed in 16 byte chunks. This function will process 16 bytes
	on input and update the hash and checksum

	\param pBlock Pointer to a buffer of 16 bytes of data to hash
	\sa Process(const void *,uintptr_t), Finalize(void) or Init(void)

***************************************/

void BURGER_API Burger::MD2Hasher_t::Process(
	const uint8_t* pBlock) BURGER_NOEXCEPT
{
	// This buffer is initialized with the input block xor'd with the hash
	uint8_t XorBuffer[16];
	// This buffer is initialized with the raw input data
	uint8_t TempBuffer[16];

	// Perform 18 passes to create the hash

	// The first pass doubles as an initialization pass

	// Must be a byte to prevent buffer overruns (256 byte buffers)
	uint8_t t = 0; // Must be initialized to ZERO!!!
	uint_t j = 0;
	do {
		// Init the xor buffer with the hash (Unmodified) and the input
		XorBuffer[j] = static_cast<uint8_t>(m_Hash.m_Hash[j] ^ pBlock[j]);
		// Process the hash
		t = static_cast<uint8_t>(m_Hash.m_Hash[j] ^ g_MD2PiTable[t]);
		m_Hash.m_Hash[j] = t;
	} while (++j < 16);

	// Process the input data and initialize the temp buffer
	j = 0;
	do {
		t = static_cast<uint8_t>(pBlock[j] ^ g_MD2PiTable[t]);
		TempBuffer[j] = t;
	} while (++j < 16);

	// Process the xor data
	j = 0;
	do {
		t = static_cast<uint8_t>(XorBuffer[j] ^ g_MD2PiTable[t]);
		XorBuffer[j] = t;
	} while (++j < 16);
	// t = static_cast<uint8_t>(t + 0);

	// Perform the remaining 17 passes on the internal buffers
	uint_t i = 1;
	do {
		j = 0;
		do {
			t = static_cast<uint8_t>(m_Hash.m_Hash[j] ^ g_MD2PiTable[t]);
			m_Hash.m_Hash[j] = t;
		} while (++j < 16);

		j = 0;
		do {
			t = static_cast<uint8_t>(TempBuffer[j] ^ g_MD2PiTable[t]);
			TempBuffer[j] = t;
		} while (++j < 16);

		j = 0;
		do {
			t = static_cast<uint8_t>(XorBuffer[j] ^ g_MD2PiTable[t]);
			XorBuffer[j] = t;
		} while (++j < 16);
		// Add in the pass number
		t = static_cast<uint8_t>(t + i);
	} while (++i < 18);

	// Update checksum for this block

	t = m_Checksum[15];
	i = 0;
	do {
		t = static_cast<uint8_t>(m_Checksum[i] ^ g_MD2PiTable[pBlock[i] ^ t]);
		m_Checksum[i] = t;
	} while (++i < 16);
}

/*! ************************************

	\brief Process an arbitrary number of input bytes

	Process input data into the hash. If data chunks are not a multiple of 16
	bytes, the excess will be cached and a future call will continue the hashing
	where it left off.

	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash
	\sa Process(const uint8_t *), Finalize(void)

***************************************/

void BURGER_API Burger::MD2Hasher_t::Process(
	const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Process data in chunks of 16

	// Are there any bytes left over from a previous pass?
	uintptr_t uIndex = m_uCount;
	// Store the new remainder
	m_uCount = (uIndex + uLength) & 0xFU;

	// Number of bytes of input needed for a pass (1-16)
	uintptr_t i = 16 - uIndex;

	// Are there 16 bytes or more in the queue?

	if (uLength >= i) {
		// Update the internal cache (To clear out previously pending bytes)

		MemoryCopy(&m_CacheBuffer[uIndex], pInput, i);
		// Process this chunk
		Process(m_CacheBuffer);

		// Are there any more chunks of 16 bytes or more remaining?
		if ((i + 15) < uLength) {
			do {
				// Process the 16 byte chunk
				Process(static_cast<const uint8_t*>(pInput) + i);
				i += 16;
				// Continue until there's less than 16 bytes remaining
			} while ((i + 15) < uLength);
		}
		// Clear out the cache
		uIndex = 0;
	} else {
		// No bytes were processed
		i = 0;
	}

	// Buffer remaining input in the cache (Can be zero)

	MemoryCopy(&m_CacheBuffer[uIndex], static_cast<const uint8_t*>(pInput) + i,
		uLength - i);
}

/*! ************************************

	\brief Finalize the hashing

	When multi-pass hashing is performed, this call is necessary to finalize the
	hash so that the generated checksum can be applied into the hash

	\sa Init(void), Process(const void *,uintptr_t)

***************************************/

void BURGER_API Burger::MD2Hasher_t::Finalize(void) BURGER_NOEXCEPT
{
	uint8_t Padding[16];

	// Pad out to multiple of 16.
	const uintptr_t uLength = 16 - m_uCount;
	MemoryFill(Padding, static_cast<uint8_t>(uLength), uLength);
	Process(Padding, uLength);

	// Extend with checksum
	Process(m_Checksum, sizeof(m_Checksum));
}

/*! ************************************

	\brief Quickly create an MD2 key

	Given a buffer of data, generate the MD2 hash key

	\param pOutput Pointer to an unitialized MD2_t structure
	\param pInput Pointer to a buffer of data to hash
	\param uLength Number of bytes to hash

	\sa Burger::MD2Hasher_t

***************************************/

void BURGER_API Burger::Hash(
	MD2_t* pOutput, const void* pInput, uintptr_t uLength) BURGER_NOEXCEPT
{
	MD2Hasher_t Context;
	// Initialize
	Context.Init();
	// Process all of the data
	Context.Process(pInput, uLength);
	// Wrap up the processing
	Context.Finalize();
	// Return the resulting hash
	MemoryCopy(pOutput, &Context.m_Hash, 16);
}
