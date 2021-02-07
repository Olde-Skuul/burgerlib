/***************************************

    Simple structures

    Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

    \struct Burger::uint32_float_t
    \brief Class to create float values with integer constants

    Some "C" compilers are not 100% accurate in creating floating point
    constants. To get around this limitation, once the binary representation of
    a floating point number is determined, it will be used to assign the value
    so the binary representation does not change from compiler to compiler.

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
        uint32_t uEpsilon = Epsilon.GetWord();

        // Set the binary value as a 32 bit integer
        Epsilon.SetWord(0x34000000U);
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

    \fn Burger::uint32_float_t::GetWord(void) const
    \brief Accessor to load the 32 bit value in binary

    \return Floating point value as a binary representation in an integer
    \sa SetWord(uint32_t)

***************************************/

/*! ************************************

    \fn Burger::uint32_float_t::SetWord(uint32_t uInput)
    \brief Set the floating point value with a binary representation in an
        integer

    \param uInput 32 bit integer to store in the union
    \sa GetWord(void) const

***************************************/


/*! ************************************

    \struct Burger::uint64_double_t
    \brief Class to create double values with integer constants

    Some "C" compilers are not 100% accurate in creating floating point
    constants. To get around this limitation, once the binary representation of
    a floating point number is determined, it will be used to assign the value
    so the binary representation does not change from compiler to compiler.

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
        uint64_t uEpsilon = Epsilon.GetWord();

        // Set the binary value as a 64 bit integer
        Epsilon.SetWord(0x3CB0000000000000ULL);

        // Get the binary value as 32 bit integers
        uint32_t uLowEpsilon = Epsilon.GetLowWord();
        uint32_t uHiEpsilon = Epsilon.GetHiWord();

        // Set the binary value as 32 bit integers
        Epsilon.SetLowWord(0x00000000U);
        Epsilon.SetHiWord(0x3CB00000U);
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

    \fn Burger::uint64_double_t::GetWord(void) const
    \brief Accessor to load the 64 bit value in binary

    \return Floating point value as a binary representation in an integer
    \sa SetWord(uint64_t), SetLowWord(uint32_t) or SetHiWord(uint32_t)

***************************************/

/*! ************************************

    \fn Burger::uint64_double_t::SetWord(uint64_t uInput)
    \brief Set the floating point value with a binary representation in an
        integer

    \param uInput 64 bit integer to store in the union
    \sa GetWord(void) const, GetLowWord(void) const or GetHiWord(void) const

***************************************/

/*! ************************************

    \fn Burger::uint64_double_t::GetLowWord(void) const
    \brief Accessor to load the low 32 bits of the 64 bit word

    \return Lower 32 bits of the 64 bit word
    \sa GetHiWord(void) const or SetLowWord(uint32_t)

***************************************/

/*! ************************************

    \fn Burger::uint64_double_t::GetHiWord(void) const
    \brief Accessor to load the upper 32 bits of the 64 bit word

    \return Upper 32 bits of the 64 bit word
    \sa GetLowWord(void) const or SetHiWord(uint32_t)

***************************************/

/*! ************************************

    \fn Burger::uint64_double_t::SetLowWord(uint32_t uInput)
    \brief Accessor to load the low 32 bits of the 64 bit word

    \param uInput 32 bit integer to store in the low 32 bits of the double
    \sa SetHiWord(uint32_t) or GetLowWord(void) const

***************************************/

/*! ************************************

    \fn Burger::uint64_double_t::SetHiWord(uint32_t uInput)
    \brief Accessor to load the low 32 bits of the 64 bit word

    \param uInput 32 bit integer to store in the upper 32 bits of the double
    \sa SetLowWord(uint32_t) or GetHiWord(void) const

***************************************/
