/***************************************

    2D Floating point vector manager

    Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brvector2d.h"
#include "brfixedvector2d.h"

/*! ************************************

    \struct Burger::Vector2D_t
    \brief 2 dimensional floating point vector

    This 8 byte vector contains x and y 32 bit floating point coordinates. A set
    of common functions for simple 2 dimensional math are part of the structure.

    It is expected to be 4 byte aligned and use scalar math. The members are
    hard coded to be "x" and "y" for maximum compatibility

    \note Since this is a structure, there is no constructor or destructor, so
        assume the data is uninitialized when creating this data type.

    \sa Burger::Vector3D_t, Burger::Vector4D_t or Burger::FixedVector2D_t

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Zero(void)
    \brief Set all values to zero
    \sa Set(float,float), SetX(float) or SetY(float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::One(void)
    \brief Set all values to 1.0f
    \sa Set(float,float), SetX(float) or SetY(float)

***************************************/

/*! ************************************

    \fn float Burger::Vector2D_t::GetX(void) const
    \brief Return the x component of the vector.

    \return The x component of the vector.

    \sa GetY(void) const or SetX(float)

***************************************/

/*! ************************************

    \fn float Burger::Vector2D_t::GetY(void) const
    \brief Return the y component of the vector.

    \return The y component of the vector.

    \sa GetX(void) const or SetY(float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::SetX(float fX)
    \brief Set the x component of the vector.

    \param fX The new x component of the vector.

    \sa GetX(void) const or SetY(float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::SetY(float fY)
    \brief Set the y component of the vector.

    \param fY The new y component of the vector.

    \sa GetY(void) const or SetX(float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Set(float fInput)
    \brief Set all of the members to specific value

    Sets the x and y values to a specific value

    \param fInput New value for all members
    \sa Zero(void), SetX(float), SetY(float) or Set(const Vector2D_t *)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Set(float fX,float fY)
    \brief Set the values to specific values

    Sets the x and y values to specific values

    \param fX New x value
    \param fY New y value
    \sa Zero(void), SetX(float), SetY(float) or Set(const Vector2D_t *)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Set(const Vector2D_t *pInput)
    \brief Copy a Vector2D_t

    Make a copy of a Vector2D_t

    \param pInput Valid pointer to a Vector2D_t structure to copy
    \sa Set(float,float), SetX(float), SetY(float) or Zero(void)

***************************************/

/*! ************************************

    \brief Convert a fixed point vector into a floating point vector

    \param pInput Valid pointer to a FixedVector2D_t structure to copy

    \sa Set(const Vector2D_t *), Set(float,float) or Zero(void)

***************************************/

void BURGER_API Burger::Vector2D_t::Set(
    const FixedVector2D_t* pInput) BURGER_NOEXCEPT
{
    FixedToFloat(&x, &pInput->x);
    FixedToFloat(&y, &pInput->y);
}

/*! ************************************

    \fn void Burger::Vector2D_t::Negate(void)
    \brief Negate a 2D vector

    Set the x and y value to -x and -y respectively
    \sa Negate(const Vector2D_t *) or Negate(float,float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Negate(float fX,float fY)
    \brief Negate the input and store it in this structure

    Set the x and y value to -fX and -fY respectively

    \param fX X value to negate
    \param fY Y value to negate
    \sa Negate(const Vector2D_t *) or Negate(void)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Negate(const Vector2D_t *pInput)
    \brief Make a copy of the input after it's been negated

    Copy the input data and negate it.

    \param pInput Valid pointer to a Vector2D_t structure to copy and negate
    \sa Negate(float,float) or Negate(void)

***************************************/

/*! ************************************

    \brief Interpolate between two vectors

    Given a factor between 0.0f and 1.0f inclusive, perform a linear scale
    between the two vectors and return pFrom if the factor is 0.0f and pTo if
    the factor is 1.0f

    No clamping is performed.

    \param pFrom Valid pointer to a Vector2D_t structure for a factor of 0.0f
    \param pTo Valid pointer to a Vector2D_t structure for a factor of 1.0f
    \param fFactor Scale value between 0.0f and 1.0f inclusive
    \sa Vector3D_t::Interpolate() and Vector4D_t::Interpolate()

***************************************/

void BURGER_API Burger::Vector2D_t::Interpolate(const Vector2D_t* pFrom,
    const Vector2D_t* pTo, float fFactor) BURGER_NOEXCEPT
{
    const float fFromX = pFrom->x;
    const float fFromY = pFrom->y;
    x = ((pTo->x - fFromX) * fFactor) + fFromX;
    y = ((pTo->y - fFromY) * fFactor) + fFromY;
}

/*! ************************************

    \fn float Burger::Vector2D_t::Dot(float fX,float fY) const
    \brief Return a dot product of two 2D vectors
    \param fX X value to dot against
    \param fY Y value to dot against

    \return Dot product of the two vectors
    \sa Dot(const Vector2D_t *) const

***************************************/

/*! ************************************

    \fn float Burger::Vector2D_t::Dot(const Vector2D_t *pInput) const
    \brief Return a dot product of two 2D vectors
    \param pInput Valid pointer to a Vector2D_t structure to perform a dot
        product against

    \return Dot product of the two vectors \sa Dot(float,float) const

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Add(const Vector2D_t *pInput)
    \brief Add another vector to this one

    \param pInput Vector to add
    \sa Add(float), Add(const Vector2D_t *,const Vector2D_t *) or Add(const
        Vector2D_t *,float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Add(float fInput)
    \brief Add the same scalar value to each of member values

    \param fInput Value to add
    \sa Add(const Vector2D_t *), Add(const Vector2D_t *,const Vector2D_t *) or
        Add(const Vector2D_t *,float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Add(const Vector2D_t *pInput1,
        const Vector2D_t *pInput2)

    \brief Add two vectors and store the result in this vector

    Initialize this vector with the result of the addition of the two input
    vectors.

    \param pInput1 First vector to add
    \param pInput2 Second vector to add
    \sa Add(float), Add(const Vector2D_t *) or Add(const Vector2D_t *,float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Add(const Vector2D_t *pInput,float fInput)
    \brief Add a vector and a scalar to every member of the vector and store the
        result in this vector.

    Initialize this vector with the result of the addition of the input vector
    and a scalar applied to every member.

    \param pInput Vector to add the scalar to
    \param fInput Scalar to add to the vector
    \sa Add(float), Add(const Vector2D_t *) or Add(const Vector2D_t *,const
        Vector2D_t *)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Sub(const Vector2D_t *pInput)
    \brief Subtract another vector from this one

    \param pInput Vector to subtract
    \sa Sub(float), Sub(const Vector2D_t *,const Vector2D_t *) or Sub(const
        Vector2D_t *,float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Sub(float fInput)
    \brief Subtract the same scalar value from each of member values

    \param fInput Value to subtract
    \sa Sub(const Vector2D_t *), Sub(const Vector2D_t *,const Vector2D_t *) or
        Sub(const Vector2D_t *,float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Sub(const Vector2D_t *pInput1,
        const Vector2D_t *pInput2)
    \brief Subtract two vectors and store the result in this vector

    Initialize this vector with the result of the subtraction of the two input
    vectors.

    \param pInput1 First vector to subtract from
    \param pInput2 Second vector to subtract with
    \sa Sub(float), Sub(const Vector2D_t *) or Sub(const Vector2D_t *,float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Sub(const Vector2D_t *pInput,float fInput)
    \brief Subtract a vector and a scalar to every member of the vector and
        store the result in this vector

    Initialize this vector with the result of the subtraction of the input
    vector and a scalar applied to every member.

    \param pInput Vector to subtract the scalar from
    \param fInput Scalar to subtract from the vector
    \sa Sub(float), Sub(const Vector2D_t *) or Sub(const Vector2D_t *,const
        Vector2D_t *)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Mul(const Vector2D_t *pInput)
    \brief Multiply another vector to this one

    \param pInput Vector to multiply
    \sa Mul(float), Mul(const Vector2D_t *,const Vector2D_t *) or Mul(const
        Vector2D_t *,float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Mul(float fInput)
    \brief Multiply the same scalar value to each of member values

    \param fInput Value to multiply
    \sa Mul(const Vector2D_t *), Mul(const Vector2D_t *,const Vector2D_t *) or
        Mul(const Vector2D_t *,float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Mul(const Vector2D_t *pInput1,
        const Vector2D_t *pInput2)
    \brief Multiply two vectors and store the result in this vector

    Initialize this vector with the result of the multiplication of the two
    input vectors.

    \param pInput1 First vector to multiply
    \param pInput2 Second vector to multiply
    \sa Mul(float), Mul(const Vector2D_t *) or Mul(const Vector2D_t *,float)

***************************************/

/*! ************************************

    \fn void Burger::Vector2D_t::Mul(const Vector2D_t *pInput,float fInput)
    \brief Multiply a vector and a scalar to every member of the vector and
        store the result in this vector

    Initialize this vector with the result of the multiplication of the input
    vector and a scalar applied to every member.

    \param pInput Vector to multiply the scalar to
    \param fInput Scalar to multiply to the vector
    \sa Mul(float), Mul(const Vector2D_t *) or Mul(const Vector2D_t *,const
        Vector2D_t *)

***************************************/

/*! ************************************

    \fn float Burger::Vector2D_t::Cross(float fX,float fY) const
    \brief Return a cross product of two 2D vectors

    Calculate the cross product between two 2D vectors as if they were two 3D
    vectors and the z components were zero. The end result is the new z
    component for the perpendicular vector

    \param fX X value to cross against
    \param fY Y value to cross against
    \return Cross product of the two vectors
    \sa Cross(const Vector2D_t *) const

***************************************/

/*! ************************************

    \fn float Burger::Vector2D_t::Cross(const Vector2D_t *pInput) const
    \brief Return a cross product of two 2D vectors

    Calculate the cross product between two 2D vectors as if they were two 3D
    vectors and the z components were zero. The end result is the new z
    component for the perpendicular vector

    \param pInput Valid pointer to a Vector2D_t structure to perform a cross
        product against
    \return Cross product of the two vectors
    \sa Cross(float,float) const

***************************************/

/*! ************************************

    \fn float Burger::Vector2D_t::GetLengthSquared(void) const
    \brief Returns the square of the length of a 2D vector
    \return x*x+y*y
    \sa GetLengthFast(void) const or GetLength(void) const

***************************************/

/*! ************************************

    \fn float Burger::Vector2D_t::GetLength(void) const
    \brief Return the length of a vector (High precision)
    \return sqrt(x*x+y*y)
    \sa GetLengthFast(void) const or GetLengthSquared(void) const

***************************************/

/*! ************************************

    \fn float Burger::Vector2D_t::GetLengthFast(void) const
    \brief Return the length of a vector (Good precision)

    On some systems, lower precision instructions are used to increase speed at
    the cost of precision. If the calculation doesn't rely on high precision,
    use this function for higher performance

    \return sqrt(x*x+y*y)
    \sa GetLength(void) const or GetLengthSquared(void) const

***************************************/

/*! ************************************

    \brief Normalize a 2D vector to a specific length (High precision)

    Set the 2D vector's length to fInput

    \note fInput cannot be zero or negative. The vector will be set to 0.0f in
        this case

    \param fInput Length to set the vector to
    \sa SetLengthFast(float) or Normalize(void)

***************************************/

void BURGER_API Burger::Vector2D_t::SetLength(float fInput) BURGER_NOEXCEPT
{
    if (fInput > 0.0f) { // Handles NaN
        const float fLength = Sqrt((x * x) + (y * y));
        if (fLength > 0.0f) {          // Handles NaN
            fInput = fInput / fLength; // Scale to the new length
            x *= fInput;
            y *= fInput;
        } else {
            x = fInput;
            y = 0.0f;
        }
    } else {
        x = 0.0f;
        y = 0.0f;
    }
}

/*! ************************************

    \brief Normalize a 2D vector to a specific length (Good precision)

    Set the 2D vector's length to fInput

    \note fInput cannot be zero or negative. The vector will be set to 0.0f in
        this case

    On some systems, lower precision instructions are used to increase speed at
    the cost of precision. If the calculation doesn't rely on high precision,
    use this function for higher performance

    \param fInput Length to set the vector to
    \sa SetLength(float) or NormalizeFast(void)

***************************************/

void BURGER_API Burger::Vector2D_t::SetLengthFast(float fInput) BURGER_NOEXCEPT
{
    if (fInput > 0.0f) { // Handles NaN
        const float fLength = Sqrt((x * x) + (y * y));
        if (fLength > 0.0f) {          // Handles NaN
            fInput = fInput / fLength; // Scale to the new length
            x *= fInput;
            y *= fInput;
        } else {
            x = fInput;
            y = 0.0f;
        }
    } else {
        x = 0.0f;
        y = 0.0f;
    }
}

/*! ************************************

    \brief Returns the square of the distance between two 2D points

    Create a vector from the difference of two 2D points and return the vector's
    length squared

    \param fX X value of the other 2D point
    \param fY Y value of the other 2D point
    \return The distance between the two points, squared
    \sa GetDistanceSquared(const Vector2D_t *) const, GetDistance(float,float)
        const or GetDistanceFast(float,float) const

***************************************/

float BURGER_API Burger::Vector2D_t::GetDistanceSquared(
    float fX, float fY) const BURGER_NOEXCEPT
{
    // Don't worry about absolute values. The squares will remove it
    fX = x - fX;
    fY = y - fY;
    return (fX * fX) + (fY * fY);
}

/*! ************************************

    \brief Returns the square of the distance between two 2D points

    Create a vector from the difference of two 2D points and return the vector's
    length squared

    \param pInput Valid pointer to a Vector2D_t structure that contains the
        other 2D point
    \return The distance between the two points, squared
    \sa GetDistanceSquared(float,float) const, GetDistance(const Vector2D_t *)
        const or GetDistanceFast(const Vector2D_t *) const

***************************************/

float BURGER_API Burger::Vector2D_t::GetDistanceSquared(
    const Vector2D_t* pInput) const BURGER_NOEXCEPT
{
    // Don't worry about absolute values. The squares will remove it
    const float fX = x - pInput->x;
    const float fY = y - pInput->y;
    return (fX * fX) + (fY * fY);
}

/*! ************************************

    \brief Returns the square of the distance between two 2D points

    Create a vector from the difference of two 2D points and return the vector's
    length

    \param fX X value of the other 2D point
    \param fY Y value of the other 2D point
    \return The distance between the two points
    \sa GetDistance(const Vector2D_t *) const, GetDistanceSquared(float,float)
        const or GetDistanceFast(float,float) const

***************************************/

float BURGER_API Burger::Vector2D_t::GetDistance(
    float fX, float fY) const BURGER_NOEXCEPT
{
    fX = x - fX;
    fY = y - fY;
    return Sqrt((fX * fX) + (fY * fY));
}

/*! ************************************

    \brief Returns the square of the distance between two 2D points

    Create a vector from the difference of two 2D points and return the vector's
    length

    \param pInput Valid pointer to a Vector2D_t structure that contains the
        other 2D point
    \return The distance between the two points
    \sa GetDistance(float,float) const, GetDistanceSquared(const Vector2D_t *)
        const or GetDistanceFast(const Vector2D_t *) const

***************************************/

float BURGER_API Burger::Vector2D_t::GetDistance(
    const Vector2D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = x - pInput->x;
    const float fY = y - pInput->y;
    return Sqrt((fX * fX) + (fY * fY));
}

/*! ************************************

    \brief Returns the square of the distance between two 2D points (Good
        precision).

    Create a vector from the difference of two 2D points and return the vector's
    length

    On some systems, lower precision instructions are used to increase speed at
    the cost of precision. If the calculation doesn't rely on high precision,
    use this function for higher performance

    \param fX X value of the other 2D point
    \param fY Y value of the other 2D point
    \return The distance between the two points
    \sa GetDistanceFast(const Vector2D_t *) const,
        GetDistanceSquared(float,float) const or GetDistance(float,float) const

***************************************/

float BURGER_API Burger::Vector2D_t::GetDistanceFast(
    float fX, float fY) const BURGER_NOEXCEPT
{
    fX = x - fX;
    fY = y - fY;
    return Sqrt((fX * fX) + (fY * fY));
}

/*! ************************************

    \brief Returns the square of the distance between two 2D points (Good
        precision).

    Create a vector from the difference of two 2D points and return the vector's
    length

    On some systems, lower precision instructions are used to increase speed at
    the cost of precision. If the calculation doesn't rely on high precision,
    use this function for higher performance

    \param pInput Valid pointer to a Vector2D_t structure that contains the
        other 2D point
    \return The distance between the two points
    \sa GetDistanceFast(float,float) const, GetDistanceSquared(const Vector2D_t
        *) const or GetDistance(const Vector2D_t *) const

***************************************/

float BURGER_API Burger::Vector2D_t::GetDistanceFast(
    const Vector2D_t* pInput) const BURGER_NOEXCEPT
{
    const float fX = x - pInput->x;
    const float fY = y - pInput->y;
    return Sqrt((fX * fX) + (fY * fY));
}

/*! ************************************

    \brief Normalize a 2D vector

    Set the 2D vector's length to 1.0f

    \sa Normalize(float,float), Normalize(const Vector2D_t *) or
        NormalizeFast(void)

***************************************/

void BURGER_API Burger::Vector2D_t::Normalize(void) BURGER_NOEXCEPT
{
    float fLengthSquared = (x * x) + (y * y);
    if (fLengthSquared > 0.0f) {                      // Handles NaN
        fLengthSquared = 1.0f / Sqrt(fLengthSquared); // Reciprocal
        x *= fLengthSquared;                          // Divide by 1.0f
        y *= fLengthSquared;
    }
}

/*! ************************************

    \brief Copy a normalized 2D vector

    Make a copy of a 2D vector and set the copy's length to 1.0f

    \param fX X value of the 2D vector to normalize
    \param fY Y value of the 2D vector to normalize
    \sa Normalize(void), Normalize(const Vector2D_t *) or
        NormalizeFast(float,float)

***************************************/

void BURGER_API Burger::Vector2D_t::Normalize(
    float fX, float fY) BURGER_NOEXCEPT
{
    float fLengthSquared = (fX * fX) + (fY * fY);
    if (fLengthSquared > 0.0f) {                      // Handles NaN
        fLengthSquared = 1.0f / Sqrt(fLengthSquared); // Reciprocal
        fX = fX * fLengthSquared;                     // Divide by 1.0f
        fY = fY * fLengthSquared;
    } else {
        fX = 0.0f;
        fY = 0.0f;
    }
    x = fX;
    y = fY;
}

/*! ************************************

    \brief Copy a normalized 2D vector

    Make a copy of a 2D vector and set the copy's length to 1.0f

    \param pInput Valid pointer to a Vector2D_t structure that will be copied
        and normalize
    \sa Normalize(void), Normalize(float,float) or NormalizeFast(const
        Vector2D_t *)

***************************************/

void BURGER_API Burger::Vector2D_t::Normalize(
    const Vector2D_t* pInput) BURGER_NOEXCEPT
{
    float fX = pInput->x;
    float fY = pInput->y;
    float fLengthSquared = (fX * fX) + (fY * fY);
    if (fLengthSquared > 0.0f) {                      // Handles NaN
        fLengthSquared = 1.0f / Sqrt(fLengthSquared); // Reciprocal
        fX = fX * fLengthSquared;                     // Divide by 1.0f
        fY = fY * fLengthSquared;
    } else {
        fX = 0.0f;
        fY = 0.0f;
    }
    x = fX;
    y = fY;
}

/*! ************************************

    \brief Normalize a 2D vector (Good precision)

    Set the 2D vector's length to 1.0f

    On some systems, lower precision instructions are used to increase speed at
    the cost of precision. If the calculation doesn't rely on high precision,
    use this function for higher performance

    \sa NormalizeFast(float,float), NormalizeFast(const Vector2D_t *) or
        Normalize(void)

***************************************/

void BURGER_API Burger::Vector2D_t::NormalizeFast(void) BURGER_NOEXCEPT
{
    float fLengthSquared = (x * x) + (y * y);
    if (fLengthSquared > 0.0f) {                      // Handles NaN
        fLengthSquared = 1.0f / Sqrt(fLengthSquared); // Reciprocal
        x *= fLengthSquared;                          // Divide by 1.0f
        y *= fLengthSquared;
    }
}

/*! ************************************

    \brief Copy a normalized 2D vector

    Make a copy of a 2D vector and set the copy's length to 1.0f

    On some systems, lower precision instructions are used to increase speed at
    the cost of precision. If the calculation doesn't rely on high precision,
    use this function for higher performance

    \param fX X value of the 2D vector to normalize
    \param fY Y value of the 2D vector to normalize
    \sa NormalizeFast(void), NormalizeFast(const Vector2D_t *) or
        Normalize(float,float)

***************************************/

void BURGER_API Burger::Vector2D_t::NormalizeFast(
    float fX, float fY) BURGER_NOEXCEPT
{
    float fLengthSquared = (fX * fX) + (fY * fY);
    if (fLengthSquared > 0.0f) {                      // Handles NaN
        fLengthSquared = 1.0f / Sqrt(fLengthSquared); // Reciprocal
        fX = fX * fLengthSquared;                     // Divide by 1.0f
        fY = fY * fLengthSquared;
    } else {
        fX = 0.0f;
        fY = 0.0f;
    }
    x = fX;
    y = fY;
}

/*! ************************************

    \brief Copy a normalized 2D vector

    Make a copy of a 2D vector and set the copy's length to 1.0f

    On some systems, lower precision instructions are used to increase speed at
    the cost of precision. If the calculation doesn't rely on high precision,
    use this function for higher performance

    \param pInput Valid pointer to a Vector2D_t structure that will be copied
        and normalize
    \sa NormalizeFast(void), NormalizeFast(float,float) or Normalize(const
        Vector2D_t *)

***************************************/

void BURGER_API Burger::Vector2D_t::NormalizeFast(
    const Vector2D_t* pInput) BURGER_NOEXCEPT
{
    float fX = pInput->x;
    float fY = pInput->y;
    float fLengthSquared = (fX * fX) + (fY * fY);
    if (fLengthSquared > 0.0f) {                      // Handles NaN
        fLengthSquared = 1.0f / Sqrt(fLengthSquared); // Reciprocal
        fX = fX * fLengthSquared;                     // Divide by 1.0f
        fY = fY * fLengthSquared;
    } else {
        fX = 0.0f;
        fY = 0.0f;
    }
    x = fX;
    y = fY;
}

/*! ************************************

    \brief Compare two Vector2D_t's for bitwise equality

    Unlike operator==(const Vector2D_t &) const, this function performs
    a bitwise comparison, which in some cases is faster if pure
    equality detection is desired.

    \param pInput Pointer to the Vector2D_t to compare against
    \return \ref TRUE if equal, \ref FALSE if not
    \sa operator==(const Vector2D_t&) const

***************************************/

uint_t BURGER_API Burger::Vector2D_t::BitwiseEqual(
    const Vector2D_t* pInput) const BURGER_NOEXCEPT
{
    return (
        (static_cast<const uint32_t*>(
             static_cast<const void*>(&pInput->x))[0] ==
            static_cast<const uint32_t*>(static_cast<const void*>(&x))[0]) &&
        (static_cast<const uint32_t*>(
             static_cast<const void*>(&pInput->y))[0] ==
            static_cast<const uint32_t*>(static_cast<const void*>(&y))[0]));
}

/*! ************************************

    \fn float Burger::Vector2D_t::Determinant(const Vector2D_t* pInput1,const
        Vector2D_t *pInput2) const
    \brief Calculate the determinant between three points

    Using the formula ((pInput1->x-x) * (pInput2->y-y)) - ((pInput1->y-y) *
    (pInput2->x-x)), calculate the determinant and return the result. Used for
    determining if a point is on a side of a vector or on the vector.

    \param pInput1 Pointer to the point that produces the primary vector.
    \param pInput2 Pointer to the point that produces the vector to test with.
    \return Zero if they are identical directions, negative for one side,
        positive for the other side.

***************************************/

/*! ************************************

    \brief Determine if a point is inside of a triangle

    Test the point against the three arcs generated by the three triangle points
    and if the point is within all of the arcs, return \ref TRUE since the point
    is inside the triangle.

    \note This function will also return \ref TRUE if the point is on the
        triangle edges.

    \param pVertex1 Pointer to the first vertex of the triangle.
    \param pVertex2 Pointer to the second vertex of the triangle.
    \param pVertex3 Pointer to the third vertex of the triangle.
    \return \ref TRUE if the point is inside the triangle, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::Vector2D_t::IsPointInTriangle(
    const Vector2D_t* pVertex1, const Vector2D_t* pVertex2,
    const Vector2D_t* pVertex3) const BURGER_NOEXCEPT
{
    uint_t uResult = FALSE;
    // Use >= to include tests that are on the arc
    if (pVertex1->Determinant(pVertex2, this) >= 0) {
        if (pVertex2->Determinant(pVertex3, this) >= 0) {
            if (pVertex3->Determinant(pVertex1, this) >= 0) {
                // Point is inside the triangle
                uResult = TRUE;
            }
        }
    }
    return uResult;
}

/*! ************************************

    \fn float & Burger::Vector2D_t::operator[](uintptr_t uInput)
    \brief Access the members as an array

    \param uInput 0 for x or 1 for y, any other value is an error
    \sa operator[](uintptr_t) const

***************************************/

/*! ************************************

    \fn const float & Burger::Vector2D_t::operator[](uintptr_t uInput) const
    \brief Access the members as an array

    \param uInput 0 for x or 1 for y, any other value is an error
    \sa operator[](uintptr_t)

***************************************/

/*! ************************************

    \fn uint_t Burger::Vector2D_t::operator == (const Vector2D_t& rInput) const
    \brief Compare two Vector2D_t's for equality

    \param rInput Reference to the Vector2D_t to compare against
    \return \ref TRUE if equal, \ref FALSE if not
    \sa operator!=(const Vector2D_t&) const

***************************************/

/*! ************************************

    \fn uint_t Burger::Vector2D_t::operator != (const Vector2D_t& rInput) const
    \brief Compare two Vector2D_t's for inequality

    \param rInput Reference to the Vector2D_t to compare against
    \return \ref TRUE if not equal, \ref FALSE if equal
    \sa operator==(const Vector2D_t&) const

***************************************/

/*! ************************************

    \fn Burger::Vector2D_t::operator const float *() const
    \brief Convert to a const float pointer

    This convenience operator converts the Vector2D_t into a float pointer to
    pass to other APIs that treat this as an array of 32 bit floats.

    \return The pointer to the object typecast as a (const float *)

***************************************/

/*! ************************************

    \struct Burger::Word32ToVector2D_t {
    \brief Structure to create a Vector2D_t with 32 bit unsigned integer
        constants

    There is no standard for initializing 32 bit floats at compile time without
    generating startup glue code. This structure fixes the problem by creating a
    union between the Vector2D_t structure and an array of data that maps
    directly over it.

    \code
    // Initialize with 2 32 bit unsigned integers
    const Burger::Word32ToVector2D_t Burger::g_Vector2DInfinity =
        {{0x7F800000U,0x7F800000U}};

    \endcode

***************************************/

/*! ************************************

    \fn Burger::Word32ToVector2D_t::operator const Vector2D_t & () const
    \brief Accessor to load the Vector2D_t

    \sa Burger::Vector3D_t or Burger::Vector4D_t

***************************************/

/** Constant of 0.0f,0.0f */
const Burger::Vector2D_t Burger::g_Vector2DZero = {0.0f, 0.0f};

/** Constant of 1.0f,1.0f */
const Burger::Vector2D_t Burger::g_Vector2DOne = {1.0f, 1.0f};

/** Constant of 1.0f,0.0f */
const Burger::Vector2D_t Burger::g_Vector2DOneX = {1.0f, 0.0f};

/** Constant of 0.0f,1.0f */
const Burger::Vector2D_t Burger::g_Vector2DOneY = {0.0f, 1.0f};

/** Constant of Burger::g_fMax,Burger::g_fMax */
const Burger::Word32ToVector2D_t Burger::g_Vector2DMax = {
    {0x7F7FFFFFU, 0x7F7FFFFFU}};

/** Constant of Burger::g_fInf,Burger::g_fInf */
const Burger::Word32ToVector2D_t Burger::g_Vector2DInfinity = {
    {0x7F800000U, 0x7F800000U}};
