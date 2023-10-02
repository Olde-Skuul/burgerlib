/***************************************

	Simple structures

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brstructs.h"

/*! ************************************

	\typedef Burger::Float80Bit

	This is a type to abstract the 80-bit floating point data type found in the
	x8087 and 680x0 math co-processors. Currently, this type is hardly used
	anymore with the exception of the audio frame rate used in AIFF files.

	Burgerlib only has functions to read and write this format into double and
	float.

	\note Use of this data type for runtime math is discouraged and only used
		for x86 code and AIFF audio file support.

***************************************/

/*! ************************************

	\union Burger::uint32_float_t
	\brief Union to create float values with integer constants

	Some "C" compilers are not 100% accurate in creating floating point
	constants. To get around this limitation, once the binary representation of
	a floating point number is determined, it will be used to assign the value
	so the binary representation does not change from compiler to compiler.

	This union is also used to obtain the binary representation of a 32 bit
	float in a cross compiler way.

	\code
	// Binary representation for Single Epsilon
	Burger::uint32_float_t Epsilon = {0x34000000U};

	void code(void)
	{
		// Fetch as a float
		float fEpsilon = Epsilon;

		// Set a new floating point value
		Epsilon = 1.5f;

		// Get the binary value as a 32 bit integer
		uint32_t uEpsilon = Epsilon.get_uint32();

		// Set the binary value as a 32 bit integer
		Epsilon.set_uint32(0x34000000U);
	}
	\endcode

	\sa uint64_double_t

***************************************/

/*! ************************************

	\fn Burger::uint32_float_t::operator float() const
	\brief Accessor to load the floating point value

	\sa uint32_float_t::operator = (float)

***************************************/

/*! ************************************

	\fn Burger::uint32_float_t::operator = (float fInput)
	\brief Update with a new floating point value

	\param fInput 32 bit floating point number to store in the union
	\sa uint32_float_t::operator float() const

***************************************/

/*! ************************************

	\fn Burger::uint32_float_t::get_float(void) const
	\brief Accessor to load the 32 float

	\return Floating point value
	\sa get_uint32(void) const

***************************************/

/*! ************************************

	\fn Burger::uint32_float_t::set_float(float fInput)
	\brief Set the floating point value with a binary representation in a
		float

	\param fInput 32 bit float to store in the union
	\sa set_uint32(uint32_t)

***************************************/

/*! ************************************

	\fn Burger::uint32_float_t::get_uint32(void) const
	\brief Accessor to load the 32 bit value in binary

	\return Floating point value as a binary representation in an integer
	\sa set_uint32(uint32_t)

***************************************/

/*! ************************************

	\fn Burger::uint32_float_t::set_uint32(uint32_t uInput)
	\brief Set the floating point value with a binary representation in an
		integer

	\param uInput 32 bit integer to store in the union
	\sa get_uint32(void) const

***************************************/

/*! ************************************

	\union Burger::uint64_double_t
	\brief Union to create double values with integer constants

	Some "C" compilers are not 100% accurate in creating floating point
	constants. To get around this limitation, once the binary representation of
	a floating point number is determined, it will be used to assign the value
	so the binary representation does not change from compiler to compiler.

	This union is also used to obtain the binary representation of a 64 bit
	float in a cross compiler way.

	\code
	// Binary representation for Double Epsilon
	Burger::uint64_double_t Epsilon = {0x3CB0000000000000ULL};

	void code(void)
	{
		// Fetch as a float
		double dEpsilon = Epsilon;

		// Set a new floating point value
		Epsilon = 1.5;

		// Get the binary value as a 64 bit integer
		uint64_t uEpsilon = Epsilon.get_uint64();

		// Set the binary value as a 64 bit integer
		Epsilon.set_uint64(0x3CB0000000000000ULL);

		// Get the binary value as 32 bit integers
		uint32_t uLowEpsilon = Epsilon.get_low_uint32();
		uint32_t uHiEpsilon = Epsilon.get_high_uint32();

		// Set the binary value as 32 bit integers
		Epsilon.set_low_uint32(0x00000000U);
		Epsilon.set_high_uint32(0x3CB00000U);
	}
	\endcode

	\sa uint32_float_t

***************************************/

/*! ************************************

	\fn Burger::uint64_double_t::operator double() const
	\brief Accessor to load the double precision floating point value

	\sa uint64_double_t::operator = (double)

***************************************/

/*! ************************************

	\fn Burger::uint64_double_t::operator = (double dInput)
	\brief Update with a new floating point value

	\param dInput 64 bit floating point number to store in the union
	\sa uint64_double_t::operator double() const

***************************************/

/*! ************************************

	\fn Burger::uint64_double_t::get_double(void) const
	\brief Accessor to load the 64 float

	\return Floating point value
	\sa set_double(double), or get_uint64(void) const

***************************************/

/*! ************************************

	\fn Burger::uint64_double_t::set_double(double dInput)
	\brief Set the floating point value with a double

	\param dInput 64 bit float to store in the union
	\sa get_double(void) const, or set_uint64(uint64_t)

***************************************/

/*! ************************************

	\fn Burger::uint64_double_t::get_uint64(void) const
	\brief Accessor to load the 64 bit value in binary

	\return Floating point value as a binary representation in an integer

	\sa set_uint64(uint64_t), set_low_uint32(uint32_t) or
		set_high_uint32(uint32_t)

***************************************/

/*! ************************************

	\fn Burger::uint64_double_t::set_uint64(uint64_t uInput)
	\brief Set the floating point value with a binary representation in an
		integer

	\param uInput 64 bit integer to store in the union

	\sa get_uint64(void) const, get_low_uint32(void) const or
		get_high_uint32(void) const

***************************************/

/*! ************************************

	\fn Burger::uint64_double_t::get_low_uint32(void) const
	\brief Accessor to load the low 32 bits of the 64 bit word

	\return Lower 32 bits of the 64 bit word
	\sa get_high_uint32(void) const or set_low_uint32(uint32_t)

***************************************/

/*! ************************************

	\fn Burger::uint64_double_t::get_high_uint32(void) const
	\brief Accessor to load the upper 32 bits of the 64 bit word

	\return Upper 32 bits of the 64 bit word
	\sa get_low_uint32(void) const or set_high_uint32(uint32_t)

***************************************/

/*! ************************************

	\fn Burger::uint64_double_t::set_low_uint32(uint32_t uInput)
	\brief Accessor to load the low 32 bits of the 64 bit word

	\param uInput 32 bit integer to store in the low 32 bits of the double
	\sa set_high_uint32(uint32_t) or get_low_uint32(void) const

***************************************/

/*! ************************************

	\fn Burger::uint64_double_t::set_high_uint32(uint32_t uInput)
	\brief Accessor to load the low 32 bits of the 64 bit word

	\param uInput 32 bit integer to store in the upper 32 bits of the double
	\sa set_low_uint32(uint32_t) or get_high_uint32(void) const

***************************************/

// 32 bit floating point constants

const Burger::uint32_float_t Burger::g_fZero = {0x00000000U};
const Burger::uint32_float_t Burger::g_fNegZero = {0x80000000U};
const Burger::uint32_float_t Burger::g_fOne = {0x3F800000U};
const Burger::uint32_float_t Burger::g_fNegOne = {0xBF800000U};
const Burger::uint32_float_t Burger::g_fHalf = {0x3F000000U};
const Burger::uint32_float_t Burger::g_fNegHalf = {0xBF000000U};
const Burger::uint32_float_t Burger::g_fInf = {0x7F800000U};
const Burger::uint32_float_t Burger::g_fNegInf = {0xFF800000U};
const Burger::uint32_float_t Burger::g_fNan = {0x7FBFFFFFU};
const Burger::uint32_float_t Burger::g_fNegNan = {0xFFBFFFFFU};
const Burger::uint32_float_t Burger::g_fQNan = {0x7FFFFFFFU};
const Burger::uint32_float_t Burger::g_fNegQNan = {0xFFFFFFFFU};
const Burger::uint32_float_t Burger::g_fMin = {0x00800000U};
const Burger::uint32_float_t Burger::g_fNegMin = {0x80800000U};
const Burger::uint32_float_t Burger::g_fMax = {0x7F7FFFFFU};
const Burger::uint32_float_t Burger::g_fNegMax = {0xFF7FFFFFU};
const Burger::uint32_float_t Burger::g_fMinNoInteger = {0x4B000000};
const Burger::uint32_float_t Burger::g_fNegMinNoInteger = {0xCB000000};
const Burger::uint32_float_t Burger::g_fEpsilon = {0x34000000U};
const Burger::uint32_float_t Burger::g_fNegEpsilon = {0xB4000000U};
const Burger::uint32_float_t Burger::g_fQuarterPi = {0x3F490FDB};
const Burger::uint32_float_t Burger::g_fNegQuarterPi = {0xBF490FDB};
const Burger::uint32_float_t Burger::g_fHalfPi = {0x3FC90FDB};
const Burger::uint32_float_t Burger::g_fNegHalfPi = {0xBFC90FDB};
const Burger::uint32_float_t Burger::g_fPi = {0x40490FDB};
const Burger::uint32_float_t Burger::g_fNegPi = {0xC0490FDB};
const Burger::uint32_float_t Burger::g_fPi2 = {0x40C90FDB};
const Burger::uint32_float_t Burger::g_fNegPi2 = {0xC0C90FDB};
const Burger::uint32_float_t Burger::g_fPi4 = {0x41490FDB};
const Burger::uint32_float_t Burger::g_fNegPi4 = {0xC1490FDB};
const Burger::uint32_float_t Burger::g_fCosPiDiv6 = {0x3F5DB3D7};
const Burger::uint32_float_t Burger::g_fNegCosPiDiv6 = {0xBF5DB3D7};
const Burger::uint32_float_t Burger::g_fReciprocalHalfPi = {0x3F22F983};
const Burger::uint32_float_t Burger::g_fNegReciprocalHalfPi = {0xBF22F983};
const Burger::uint32_float_t Burger::g_fReciprocalPi = {0x3EA2F983};
const Burger::uint32_float_t Burger::g_fNegReciprocalPi = {0xBEA2F983};
const Burger::uint32_float_t Burger::g_fReciprocalPi2 = {0x3E22F983};
const Burger::uint32_float_t Burger::g_fNegReciprocalPi2 = {0xBE22F983};
const Burger::uint32_float_t Burger::g_fRadiansToDegrees = {0x42652EE1};
const Burger::uint32_float_t Burger::g_fNegRadiansToDegrees = {0xC2652EE1};
const Burger::uint32_float_t Burger::g_fDegreesToRadians = {0x3C8EFA35};
const Burger::uint32_float_t Burger::g_fNegDegreesToRadians = {0xBC8EFA35};
const Burger::uint32_float_t Burger::g_fSqrt2 = {0x3FB504F3};
const Burger::uint32_float_t Burger::g_fNegSqrt2 = {0xBFB504F3};
const Burger::uint32_float_t Burger::g_fLN2 = {0x3F317218};
const Burger::uint32_float_t Burger::g_fNegLN2 = {0xBF317218};
const Burger::uint32_float_t Burger::g_fLN10 = {0x40135D8E};
const Burger::uint32_float_t Burger::g_fNegLN10 = {0xC0135D8E};
const Burger::uint32_float_t Burger::g_f1Div255 = {0x3B808081};
const Burger::uint32_float_t Burger::g_f1Div3 = {0x3EAAAAAB};
const Burger::uint32_float_t Burger::g_f2Div3 = {0x3F2AAAAB};
const Burger::uint32_float_t Burger::g_f65536 = {0x47800000};

// 64 bit floating point constants

const Burger::uint64_double_t Burger::g_dZero = {0x0000000000000000ULL};
const Burger::uint64_double_t Burger::g_dNegZero = {0x8000000000000000ULL};
const Burger::uint64_double_t Burger::g_dOne = {0x3FF0000000000000ULL};
const Burger::uint64_double_t Burger::g_dNegOne = {0xBFF0000000000000ULL};
const Burger::uint64_double_t Burger::g_dHalf = {0x3FE0000000000000ULL};
const Burger::uint64_double_t Burger::g_dNegHalf = {0xBFE0000000000000ULL};
const Burger::uint64_double_t Burger::g_dInf = {0x7FF0000000000000ULL};
const Burger::uint64_double_t Burger::g_dNegInf = {0xFFF0000000000000ULL};
const Burger::uint64_double_t Burger::g_dNan = {0x7FF7FFFFFFFFFFFFULL};
const Burger::uint64_double_t Burger::g_dNegNan = {0xFFF7FFFFFFFFFFFFULL};
const Burger::uint64_double_t Burger::g_dQNan = {0x7FFFFFFFFFFFFFFFULL};
const Burger::uint64_double_t Burger::g_dNegQNan = {0xFFFFFFFFFFFFFFFFULL};
const Burger::uint64_double_t Burger::g_dMin = {0x0010000000000000ULL};
const Burger::uint64_double_t Burger::g_dNegMin = {0x8010000000000000ULL};
const Burger::uint64_double_t Burger::g_dMax = {0x7FEFFFFFFFFFFFFFULL};
const Burger::uint64_double_t Burger::g_dNegMax = {0xFFEFFFFFFFFFFFFFULL};
const Burger::uint64_double_t Burger::g_dMinNoInteger = {0x4330000000000000ULL};
const Burger::uint64_double_t Burger::g_dNegMinNoInteger = {
	0xC330000000000000ULL};
const Burger::uint64_double_t Burger::g_dEpsilon = {0x3CB0000000000000ULL};
const Burger::uint64_double_t Burger::g_dNegEpsilon = {0xBCB0000000000000ULL};
const Burger::uint64_double_t Burger::g_dQuarterPi = {0x3FE921FB54442D18ULL};
const Burger::uint64_double_t Burger::g_dNegQuarterPi = {0xBFE921FB54442D18ULL};
const Burger::uint64_double_t Burger::g_dHalfPi = {0x3FF921FB54442D18ULL};
const Burger::uint64_double_t Burger::g_dNegHalfPi = {0xBFF921FB54442D18ULL};
const Burger::uint64_double_t Burger::g_dPi = {0x400921FB54442D18ULL};
const Burger::uint64_double_t Burger::g_dNegPi = {0xC00921FB54442D18ULL};
const Burger::uint64_double_t Burger::g_dPi2 = {0x401921FB54442D18ULL};
const Burger::uint64_double_t Burger::g_dNegPi2 = {0xC01921FB54442D18ULL};
const Burger::uint64_double_t Burger::g_dPi4 = {0x402921FB54442D18ULL};
const Burger::uint64_double_t Burger::g_dNegPi4 = {0xC02921FB54442D18ULL};
const Burger::uint64_double_t Burger::g_dCosPiDiv6 = {0x3FEBB67AE8584CABULL};
const Burger::uint64_double_t Burger::g_dNegCosPiDiv6 = {0xBFEBB67AE8584CABULL};
const Burger::uint64_double_t Burger::g_dReciprocalHalfPi = {
	0x3FE45F306DC9C883ULL};
const Burger::uint64_double_t Burger::g_dNegReciprocalHalfPi = {
	0xBFE45F306DC9C883ULL};
const Burger::uint64_double_t Burger::g_dReciprocalPi = {0x3FD45F306DC9C883ULL};
const Burger::uint64_double_t Burger::g_dNegReciprocalPi = {
	0xBFD45F306DC9C883ULL};
const Burger::uint64_double_t Burger::g_dReciprocalPi2 = {
	0x3FC45F306DC9C883ULL};
const Burger::uint64_double_t Burger::g_dNegReciprocalPi2 = {
	0xBFC45F306DC9C883ULL};
const Burger::uint64_double_t Burger::g_dRadiansToDegrees = {
	0x404CA5DC1A63C1F8ULL};
const Burger::uint64_double_t Burger::g_dNegRadiansToDegrees = {
	0xC04CA5DC1A63C1F8ULL};
const Burger::uint64_double_t Burger::g_dDegreesToRadians = {
	0x3F91DF46A2529D39ULL};
const Burger::uint64_double_t Burger::g_dNegDegreesToRadians = {
	0xBF91DF46A2529D39ULL};
const Burger::uint64_double_t Burger::g_dSqrt2 = {0x3FF6A09E667F3BCDULL};
const Burger::uint64_double_t Burger::g_dNegSqrt2 = {0xBFF6A09E667F3BCDULL};
const Burger::uint64_double_t Burger::g_dLN2 = {0x3FE62E42FEFA39EFULL};
const Burger::uint64_double_t Burger::g_dNegLN2 = {0xBFE62E42FEFA39EFULL};
const Burger::uint64_double_t Burger::g_dLN10 = {0x40026BB1BBB55516ULL};
const Burger::uint64_double_t Burger::g_dNegLN10 = {0xC0026BB1BBB55516ULL};
const Burger::uint64_double_t Burger::g_d1Div255 = {0x3F70101010101010ULL};
const Burger::uint64_double_t Burger::g_d1Div3 = {0x3FD5555555555555ULL};
const Burger::uint64_double_t Burger::g_d2Div3 = {0x447696695DBD1CC3ULL};
const Burger::uint64_double_t Burger::g_d65536 = {0x40f0000000000000ULL};

#if defined(BURGER_X86) || defined(DOXYGEN)

// 8087 control word to set rounding to zero
const uint16_t Burger::g_u8087RoundToZero = 0x0F7F;

// 8087 control word to set rounding to +infinity
const uint16_t Burger::g_u8087RoundToInfinity = 0x0B7F;

// 8087 control word to set rounding to -infinity
const uint16_t Burger::g_u8087RoundToNegativeInfinity = 0x077F;

#endif