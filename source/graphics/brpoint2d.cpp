/***************************************

	Integer Point handlers

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brpoint2d.h"
#include "brendian.h"

/*! ************************************

	\struct Burger::Point2D_t
	\brief Integer 2D coordinate.
	
	Simple container structure that holds an X and a Y
	coordinate with integer precision.
	
	\sa Burger::Vector2D_t

***************************************/

/*! ************************************

	\fn Burger::Point2D_t::GetX(void) const
	\brief Retrieve the X coordinate
	
	\return The X coordinate of the point
	\sa GetY() or GetXPtr() 
	
***************************************/

/*! ************************************

	\fn Burger::Point2D_t::GetY(void) const
	\brief Retrieve the Y coordinate
	
	\return The Y coordinate of the point
	\sa GetX() or GetYPtr() 
	
***************************************/

/*! ************************************

	\fn Burger::Point2D_t::GetXPtr(void)
	\brief Retrieve the pointer to the X coordinate
	
	\return The pointer to the X coordinate of the point
	\sa GetYPtr() or GetX()
	
***************************************/

/*! ************************************

	\fn Burger::Point2D_t::GetYPtr(void)
	\brief Retrieve the pointer to the Y coordinate
	
	\return The pointer to the Y coordinate of the point
	\sa GetXPtr() or GetY()
	
***************************************/

/*! ************************************

	\fn Burger::Point2D_t::SetX(int iX)
	\brief Set the X coordinate
	
	This inline function sets the X coordinate for the point
	
	\param iX The new X coordinate for the point
	\sa Set(), GetX() or SetY()
	
***************************************/

/*! ************************************

	\fn Burger::Point2D_t::SetY(int iY)
	\brief Set the Y coordinate
	
	This inline function sets the Y coordinate for the point
	
	\param iY The new Y coordinate for the point
	\sa Set(), GetY() or SetX()
	
***************************************/

/*! ************************************

	\fn Burger::Point2D_t::Set(int iX,int iY)
	\brief Set the X and Y coordinates
	
	This inline function sets the X and Y coordinates for the point,
	effectively initializing the structure
	
	\param iX The new X coordinate for the point
	\param iY The new Y coordinate for the point
	\sa Clear(), SetX() or SetY()
	
***************************************/

/*! ************************************

	\fn Burger::Point2D_t::Clear(void)
	\brief Set the X and Y coordinates to zero
	
	This inline function sets the X and Y coordinates for the point to zero,
	effectively initializing the structure
	
	\sa Set()
	
***************************************/

/*! ************************************

	\brief Read a point from a file
	
	Read two sixteen bit integers in Little Endian format
	from an open binary file and store the values as the
	new X and Y coordinates.
	
	\note The input is sixteen bit, so the values returned are in the range
	of -32768 and 32767
	\param fp Pointer to an open for reading FILE reference
	\return Zero if no error, non-zero on error
	\sa Read(Burger::File *) or Write(FILE *) const

***************************************/

Word Burger::Point2D_t::Read(FILE *fp)
{
	Int16 Input[2];
	
	Word uResult = 10;
	if (fread(Input,1,4,fp)==4) {
		m_iX = LittleEndian::Load(&Input[0]);
		m_iY = LittleEndian::Load(&Input[1]);
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Write a point to a file
	
	Write the X and Y coordinates to a FILE
	stream as two sixteen bit little endian integers.
	
	\note The output is sixteen bit, so valid values are in the range
	of -32768 and 32767. If values in the Burger::Point2D_t are
	out of this range, data loss could occur.
	\param fp Pointer to an open for writing FILE reference
	\return Zero if no error, non-zero on error
	\sa Write(Burger::File *) const or Read(FILE *)

***************************************/

Word Burger::Point2D_t::Write(FILE *fp) const
{
	Int16 Output[2];
	
	Output[0] = static_cast<Int16>(LittleEndian::Load(&m_iX));
	Output[1] = static_cast<Int16>(LittleEndian::Load(&m_iY));
	Word uResult = 10;
	if (fwrite(Output,1,4,fp)==4) {
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Read a point from a file
	
	Read two sixteen bit integers in Little Endian format
	from an open binary file and store the values as the
	new X and Y coordinates.
	
	\note The input is sixteen bit, so the values returned are in the range
	of -32768 and 32767
	\param fp Pointer to an open for reading Burger::File reference
	\return Zero if no error, non-zero on error
	\sa Read(FILE *) or Write(Burger::File *) const

***************************************/

Word Burger::Point2D_t::Read(Burger::File *fp)
{
	Int16 Input[2];
	
	Word uResult = 10;
	if (fp->Read(Input,4)==4) {
		m_iX = LittleEndian::Load(&Input[0]);
		m_iY = LittleEndian::Load(&Input[1]);
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\brief Write a point to a file
	
	Write the X and Y coordinates to a FILE
	stream as two sixteen bit little endian integers.
	
	\note The output is sixteen bit, so valid values are in the range
	of -32768 and 32767. If values in the Burger::Point2D_t are
	out of this range, data loss could occur.
	\param fp Pointer to an open for writing Burger::File reference
	\return Zero if no error, non-zero on error
	\sa Write(FILE *) const or Read(Burger::File *)

***************************************/

Word Burger::Point2D_t::Write(Burger::File *fp) const
{
	Int16 Output[2];
	
	Output[0] = static_cast<Int16>(LittleEndian::Load(&m_iX));
	Output[1] = static_cast<Int16>(LittleEndian::Load(&m_iY));
	Word uResult = 10;
	if (fp->Write(Output,4)==4) {
		uResult = 0;
	}
	return uResult;
}

/*! ************************************

	\fn void Burger::Point2D_t::Get(Point *pOutput) const
	\brief Convert a Burger::Point2D_t into a MacOS Point structure.
	
	Converts the contents of a Burger::Point2D_t into a MacOS/Quicktime
	for Windows Point structure. Since it's converting an int into a short,
	data loss could occur if the values exceed 32767 or are less than -32768.
	
	\param pOutput Pointer to an uninitialized MacOS Point structure.
	\note This function is MacOS or Windows using Quicktime
	only.

***************************************/

/*! ************************************

	\fn void Burger::Point2D_t::Set(const Point *pInput)
	\brief Convert a MacOS Point structure into a Burger::Point2D_t
	
	Converts the contents of a MacOS/Quicktime for Windows Point structure
	into a Burger::Point2D_t. Since it's converting a short to an int, no
	data loss will occur.
	
	\param pInput Pointer to a valid MacOS Point structure.
	\note This function is MacOS or Windows using Quicktime
	only.

***************************************/

/*! ************************************

	\fn void Burger::Point2D_t::Get(tagPOINT *pOutput) const
	\brief Convert a Burger::Point2D_t into a Microsoft POINT structure.
	
	Converts the contents of a Burger::Point2D_t into a Microsoft POINT
	structure.
	
	\param pOutput Pointer to an uninitialized Microsoft POINT structure.
	\note This function is Windows and Xbox 360  only.

***************************************/

/*! ************************************

	\fn void Burger::Point2D_t::Set(const tagPOINT *pInput)
	\brief Convert a Microsoft POINT structure into a Burger::Point2D_t
	
	Converts the contents of a Microsoft POINT structure
	into a Burger::Point2D_t.
	
	\param pInput Pointer to a valid Microsoft POINT structure.
	\note This function is Windows and Xbox 360 only.

***************************************/
