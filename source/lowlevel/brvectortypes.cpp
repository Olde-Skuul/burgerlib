/***************************************

	Handle all the vector types for all CPUs and compilers

	Copyright (c) 1995-2025 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brvectortypes.h"

/*! ************************************

	\typedef vector128_t
	\brief 128 bit vector intrinsic.

	This type is used for register remapping to the vector units on several
	CPUs. It is not to be used for 128 bit integer math. Currently, only
	processors with vector units like the Playstation 2, 3, 4, 5, Xbox 360, XBox
	ONE, Pentium with SSE, ARM with NEON and PowerPC with Alti-Vec can use this
	intrinsic data type.

	\sa Burger::vector128_float_t, Burger::vector128_uint32_t,
		Burger::vector128_int32_t, Burger::vector128_uint16_t,
		Burger::vector128_uint8_t, BURGER_ARM, BURGER_PPC or BURGER_INTEL

***************************************/

/*! ************************************

	\struct Burger::vector128_float_t
	\brief Structure to create a 128 vector with 32 bit floating point constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 4 32 bit floats
	const Burger::vector128_float_t One_Vector = {{1.0f,1.0f,1.0f,1.0f}};

	\endcode

	\sa vector128_uint32_t, vector128_uint16_t, vector128_uint8_t,
		or vector128_int32_t

***************************************/

/*! ************************************

	\fn Burger::vector128_float_t::operator vector128_t() const
	\brief Accessor to load the 128 bit vector value

	\sa vector128_float_t or vector128_t

***************************************/

/*! ************************************

	\struct Burger::vector128_double_t
	\brief Structure to create a 128 vector with 64 bit floating point constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 2 64 bit floats
	const Burger::vector128_double_t One_Vector = {{1.0,-55.0}};

	\endcode

	\sa vector128_uint64_t, vector128_float_t, vector128_uint32_t,
		vector128_uint16_t, vector128_uint8_t, or vector128_int32_t

***************************************/

/*! ************************************

	\fn Burger::vector128_double_t::operator vector128_t() const
	\brief Accessor to load the 128 bit vector value

	\sa vector128_double_t or vector128_t

***************************************/

/*! ************************************

	\struct Burger::vector128_int32_t
	\brief Structure to create a 128 vector with 32 bit signed integer constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 4 32 bit signed integers
	const Burger::vector128_int32_t One_Vector = {{-1,1,-1,0x12345678}};

	\endcode

	\sa vector128_float_t, vector128_uint32_t, vector128_uint16_t,
		or vector128_uint8_t

***************************************/

/*! ************************************

	\fn Burger::vector128_int32_t::operator vector128_t() const
	\brief Accessor to load the 128 bit vector value

	\sa vector128_int32_t or vector128_t

***************************************/

/*! ************************************

	\struct Burger::vector128_uint64_t
	\brief Structure to create a 128 vector with 64 bit unsigned integer
		constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 2 64 bit unsigned integers
	const Burger::vector128_uint64_t One_Vector = {{1,0x123456789ABCDEFULL}};

	\endcode

	\sa vector128_double_t, vector128_uint32_t, vector128_uint16_t,
		vector128_uint8_t, or vector128_int32_t

***************************************/

/*! ************************************

	\fn Burger::vector128_uint64_t::operator vector128_t() const
	\brief Accessor to load the 128 bit vector value

	\sa vector128_uint64_t or vector128_t

***************************************/

/*! ************************************

	\struct Burger::vector128_uint32_t
	\brief Structure to create a 128 vector with 32 bit unsigned integer
		constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 4 32 bit unsigned integers
	const Burger::vector128_uint32_t One_Vector = {{1,1,1,0x12345678}};

	\endcode

	\sa vector128_float_t, vector128_uint16_t, vector128_uint8_t,
		or vector128_int32_t

***************************************/

/*! ************************************

	\fn Burger::vector128_uint32_t::operator vector128_t() const
	\brief Accessor to load the 128 bit vector value

	\sa vector128_uint32_t or vector128_t

***************************************/

/*! ************************************

	\struct Burger::vector128_uint16_t
	\brief Structure to create a 128 vector with 16 bit unsigned integer
		constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 8 16 bit unsigned integers
	const Burger::vector128_uint16_t One_Vector = {{1,2,3,4,5,6,7,0x1234}};

	\endcode

	\sa vector128_float_t, vector128_uint32_t, vector128_uint8_t,
		or vector128_int32_t

***************************************/

/*! ************************************

	\fn Burger::vector128_uint16_t::operator vector128_t() const
	\brief Accessor to load the 128 bit vector value

	\sa vector128_uint16_t or vector128_t

***************************************/

/*! ************************************

	\struct Burger::vector128_uint8_t
	\brief Structure to create a 128 vector with 8 bit unsigned integer
		constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 16 8 bit unsigned integers
	const Burger::vector128_uint8_t One_Vector =
		{{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}};

	\endcode

	\sa vector128_float_t, vector128_uint32_t, vector128_uint16_t,
		or vector128_int32_t

***************************************/

/*! ************************************

	\fn Burger::vector128_uint8_t::operator vector128_t() const
	\brief Accessor to load the 128 bit vector value

	\sa vector128_uint8_t or vector128_t

***************************************/
