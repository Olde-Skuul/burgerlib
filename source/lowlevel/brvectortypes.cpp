/***************************************

	Handle all the vector types for all CPUs and compilers

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brvectortypes.h"

/*! ************************************

	\struct Vector_128
	\brief 128 bit vector intrinsic.

	This integer is used for register remapping to the vector units on several
	CPUs. It is not to be used for 128 bit integer math. Currently, only
	processors with vector units like the Playstation 2, 3, 4, 5, Xbox 360, XBox
	ONE, Pentium with SSE, ARM with NEON and PowerPC with Alti-Vec can use this
	intrinsic data type.

	\sa Burger::Vector_128Float32, Burger::Vector_128Word32,
		Burger::Vector_128Int32, Burger::Vector_128Word16,
		Burger::Vector_128Word8, BURGER_ARM32, BURGER_POWERPC or BURGER_X86

***************************************/

/*! ************************************

	\struct Burger::Vector_128Float32
	\brief Structure to create a 128 vector with 32 bit floating point constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 4 32 bit floats
	const Burger::Vector_128Float32 One_Vector = {{1.0f,1.0f,1.0f,1.0f}};

	\endcode

	\sa Vector_128Word32, Vector_128Word16, Vector_128Word8, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Float32::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value

	\sa Vector_128Float32 or Vector_128

***************************************/

/*! ************************************

	\struct Burger::Vector_128Float64
	\brief Structure to create a 128 vector with 64 bit floating point constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 2 64 bit floats
	const Burger::Vector_128Float64 One_Vector = {{1.0,-55.0}};

	\endcode

	\sa Vector_128Word64, Vector_128Float32, Vector_128Word32, Vector_128Word16,
		Vector_128Word8, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Float64::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value

	\sa Vector_128Float64 or Vector_128

***************************************/

/*! ************************************

	\struct Burger::Vector_128Int32
	\brief Structure to create a 128 vector with 32 bit signed integer constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 4 32 bit signed integers
	const Burger::Vector_128Int32 One_Vector = {{-1,1,-1,0x12345678}};

	\endcode

	\sa Vector_128Float32, Vector_128Word32, Vector_128Word16, Vector_128Word8

***************************************/

/*! ************************************

	\fn Burger::Vector_128Int32::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value

	\sa Vector_128Int32 or Vector_128

***************************************/

/*! ************************************

	\struct Burger::Vector_128Word64
	\brief Structure to create a 128 vector with 64 bit unsigned integer
		constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 2 64 bit unsigned integers
	const Burger::Vector_128Word64 One_Vector = {{1,0x123456789ABCDEFULL}};

	\endcode

	\sa Vector_128Float64, Vector_128Word32, Vector_128Word16, Vector_128Word8,
		Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Word64::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value

	\sa Vector_128Word64 or Vector_128

***************************************/

/*! ************************************

	\struct Burger::Vector_128Word32
	\brief Structure to create a 128 vector with 32 bit unsigned integer
		constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 4 32 bit unsigned integers
	const Burger::Vector_128Word32 One_Vector = {{1,1,1,0x12345678}};

	\endcode

	\sa Vector_128Float32, Vector_128Word16, Vector_128Word8, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Word32::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value

	\sa Vector_128Word32 or Vector_128

***************************************/

/*! ************************************

	\struct Burger::Vector_128Word8
	\brief Structure to create a 128 vector with 8 bit unsigned integer
		constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 16 8 bit unsigned integers
	const Burger::Vector_128Word8 One_Vector =
		{{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}};

	\endcode

	\sa Vector_128Float32, Vector_128Word32, Vector_128Word16, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Word8::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value

	\sa Vector_128Word8 or Vector_128

***************************************/

/*! ************************************

	\struct Burger::Vector_128Word16
	\brief Structure to create a 128 vector with 16 bit unsigned integer
		constants

	There is no standard for initializing 128 bit vectors at compile time. This
	structure fixes the problem by creating a union between the 128 bit value
	and an array of data that maps directly over it.

	\code
	// Initialize with 8 16 bit unsigned integers
	const Burger::Vector_128Word16 One_Vector = {{1,2,3,4,5,6,7,0x1234}};

	\endcode

	\sa Vector_128Float32, Vector_128Word32, Vector_128Word8, Vector_128Int32

***************************************/

/*! ************************************

	\fn Burger::Vector_128Word16::operator Vector_128() const
	\brief Accessor to load the 128 bit vector value

	\sa Vector_128Word16 or Vector_128

***************************************/
