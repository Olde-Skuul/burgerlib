/***************************************

	SDBM hash manager (Service Data Base Management)

	Implemented following the documentation found in
	http::/www.cs.yorku.ca/~oz/hash.html

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brsdbmhash.h"
#include "brstringfunctions.h"

/*! ************************************

	\brief Hash data using the sdbm (Service Data Base Management) algorithm

	This algorithm was created for sdbm (a public-domain reimplementation of
	ndbm) database library.

	It was found to do well in scrambling bits, causing better distribution
	of the keys and fewer splits. it also happens to be a good general
	hashing function with good distribution. the actual function is
	hash(i) = hash(i - 1) * 65599 + str[i];

	Further reading: http://www.cs.yorku.ca/~oz/hash.html#sdbm

	\param pInput Pointer to the data to hash
	\param uInputCount Number of bytes of data to hash
	\param uHashSeed Value to seed the hash with.

	\return 32 bit hash value generated by the data.

	\sa SDBM_hash_case(const void *,uintptr_t,uint32_t)

***************************************/

uintptr_t BURGER_API Burger::SDBM_hash(const void* pInput,
	uintptr_t uInputCount, uint32_t uHashSeed) BURGER_NOEXCEPT
{
	if (uInputCount) {
		do {
			// Multiply by 65599 via *65536 + *64 - 1
			uHashSeed = (((uHashSeed << 16U) + (uHashSeed << 6U)) - uHashSeed) +
				static_cast<const uint8_t*>(pInput)[0];

			pInput = static_cast<const uint8_t*>(pInput) + 1;
		} while (--uInputCount);
	}
	return uHashSeed;
}

/*! ************************************

	\brief Hash string data using forced lower case with the sdbm (Service Data
		Base Management) algorithm

	This algorithm was created for sdbm (a public-domain reimplementation of
	ndbm) database library.

	It was found to do well in scrambling bits, causing better distribution
	of the keys and fewer splits. it also happens to be a good general
	hashing function with good distribution. the actual function is
	hash(i) = hash(i - 1) * 65599 + str[i];

	Further reading: http://www.cs.yorku.ca/~oz/hash.html#sdbm

	\note This function converts all upper case characters into lower case
		to yield a case insensitive hash

	\param pInput Pointer to the data to hash
	\param uInputCount Number of bytes of data to hash
	\param uHashSeed Value to seed the hash with.

	\return 32 bit hash value generated by the data.

	\sa SDBM_hash(const void *,uintptr_t,uint32_t)

***************************************/

uintptr_t BURGER_API Burger::SDBM_hash_case(const void* pInput,
	uintptr_t uInputCount, uint32_t uHashSeed) BURGER_NOEXCEPT
{
	if (uInputCount) {
		do {
			// Multiply by 65599 via *65536 + *64 - 1
			uHashSeed = (((uHashSeed << 16U) + (uHashSeed << 6U)) - uHashSeed) +
				ToLower(static_cast<const uint8_t*>(pInput)[0]);

			pInput = static_cast<const uint8_t*>(pInput) + 1;
		} while (--uInputCount);
	}
	return uHashSeed;
}
