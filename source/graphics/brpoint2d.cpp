/***************************************

	Integer 2 dimensional Point handlers

	Copyright (c) 1995-2024 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brpoint2d.h"
#include "brendian.h"

// Documented here, all others use the DOXYGEN header guard

#if defined(BURGER_MAC)
#include <MacTypes.h>
#else

/*! ************************************

	\brief Definition of a MacOS Point

	Duplicate of the definition found in the MacOS header <MacTypes.h>

	This is to allow non-Mac desktop platforms to manipulate data used by
	MacOS functions and data sharing. It's assumed to be stored on disc in
	Big Endian format.

	\note This structure is only natively used on MacOS platforms.

	\sa Rect

***************************************/

struct Point {
	int16_t v; ///< Vertical (Y) coordinate
	int16_t h; ///< Height (X) coordinate
};

#endif

/*! ************************************

	\struct Burger::Point2D_t
	\brief Integer 2D coordinate.

	Simple container structure that holds an X and a Y coordinate with integer
	precision.

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

	\fn Burger::Point2D_t::SetX(int32_t iX)
	\brief Set the X coordinate

	This inline function sets the X coordinate for the point

	\param iX The new X coordinate for the point
	\sa Set(), GetX() or SetY()

***************************************/

/*! ************************************

	\fn Burger::Point2D_t::SetY(int32_t iY)
	\brief Set the Y coordinate

	This inline function sets the Y coordinate for the point

	\param iY The new Y coordinate for the point
	\sa Set(), GetY() or SetX()

***************************************/

/*! ************************************

	\fn Burger::Point2D_t::Set(int32_t iX,int32_t iY)
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

	Read two sixteen bit integers in Little Endian format from an open binary
	file and store the values as the new X and Y coordinates.

	\note The input is sixteen bit, so the values returned are in the range
		of -32768 and 32767

	\param fp Pointer to an open for reading FILE reference

	\return \ref kErrorNone if no error, non-zero on error

	\sa Read(Burger::File *) or Write(FILE *) const

***************************************/

Burger::eError Burger::Point2D_t::Read(FILE* fp) BURGER_NOEXCEPT
{
	int16_t Input[2];

	eError uResult = kErrorReadFailure;
	if (fread(Input, 1, 4, fp) == 4) {
		m_iX = LittleEndian::load(&Input[0]);
		m_iY = LittleEndian::load(&Input[1]);
		uResult = kErrorNone;
	}
	return uResult;
}

/*! ************************************

	\brief Write a point to a file

	Write the X and Y coordinates to a FILE stream as two sixteen bit little
	endian integers.

	\note The output is sixteen bit, so valid values are in the range
		of -32768 and 32767. If values in the Burger::Point2D_t are
		out of this range, data loss could occur.

	\param fp Pointer to an open for writing FILE reference

	\return \ref kErrorNone if no error, non-zero on error

	\sa Write(Burger::File *) const or Read(FILE *)

***************************************/

Burger::eError Burger::Point2D_t::Write(FILE* fp) const BURGER_NOEXCEPT
{
	int16_t Output[2];

	Output[0] = static_cast<int16_t>(LittleEndian::load(&m_iX));
	Output[1] = static_cast<int16_t>(LittleEndian::load(&m_iY));
	eError uResult = kErrorWriteFailure;
	if (fwrite(Output, 1, 4, fp) == 4) {
		uResult = kErrorNone;
	}
	return uResult;
}

/*! ************************************

	\brief Read a point from a file

	Read two sixteen bit integers in Little Endian format from an open binary
	file and store the values as the new X and Y coordinates.

	\note The input is sixteen bit, so the values returned are in the range of
		-32768 and 32767.

	\param fp Pointer to an open for reading Burger::File reference

	\return \ref kErrorNone if no error, non-zero on error

	\sa Read(FILE *) or Write(Burger::File *) const

***************************************/

Burger::eError Burger::Point2D_t::Read(Burger::File* fp) BURGER_NOEXCEPT
{
	int16_t Input[2];

	eError uResult = kErrorReadFailure;
	if (fp->read(Input, 4) == 4) {
		m_iX = LittleEndian::load(&Input[0]);
		m_iY = LittleEndian::load(&Input[1]);
		uResult = kErrorNone;
	}
	return uResult;
}

/*! ************************************

	\brief Write a point to a file

	Write the X and Y coordinates to a FILE stream as two sixteen bit little
	endian integers.

	\note The output is sixteen bit, so valid values are in the range of
	-32768 and 32767. If values in the Point2D_t are out of this range, data
	loss could occur.

	\param fp Pointer to an open for writing Burger::File reference

	\return \ref kErrorNone if no error, non-zero on error

	\sa Write(FILE *) const or Read(Burger::File *)

***************************************/

Burger::eError Burger::Point2D_t::Write(Burger::File* fp) const BURGER_NOEXCEPT
{
	int16_t Output[2];

	Output[0] = static_cast<int16_t>(LittleEndian::load(&m_iX));
	Output[1] = static_cast<int16_t>(LittleEndian::load(&m_iY));
	eError uResult = kErrorWriteFailure;
	if (fp->write(Output, 4) == 4) {
		uResult = kErrorNone;
	}
	return uResult;
}

/*! ************************************

	\brief Read in a MacOS Point from an InputMemoryStream

	Read in 4 bytes from the stream as big endian 16 bit chunks and fill in a
	MacOS Point structure using native endian. There is no need for endian
	conversion to the data after it's been read.

	No data validation is performed on the input.

	\param pPoint Pointer to MacOS Point structure
	\param pInput Pointer to InputMemoryStream to read from

	\return \ref kErrorDataStarvation if data is not sufficient in the stream
		or \ref kErrorNone.

	\sa get(Rect*, InputMemoryStream*),
		append(OutputMemoryStream*, const Point*)

***************************************/

Burger::eError BURGER_API Burger::get(
	Point* pPoint, InputMemoryStream* pInput) BURGER_NOEXCEPT
{
	if (pInput->BytesRemaining() < 4U) {
		return kErrorDataStarvation;
	}

	// Read in the 16 bit values, endian swap if needed
	pPoint->v = static_cast<int16_t>(pInput->GetBigShort());
	pPoint->h = static_cast<int16_t>(pInput->GetBigShort());
	return kErrorNone;
}

/*! ************************************

	\brief Write out a MacOS Point into an OutputMemoryStream

	Write out 4 bytes into the stream as big endian 16 bit chunks from a native
	endian MacOS Point structure.

	\param pOutput Pointer to OutputMemoryStream to write to
	\param pPoint Pointer to MacOS Point structure

	\return \ref kErrorNone if no error, OutputMemoryStream error codes if the
		function failed.

	\sa append(OutputMemoryStream*, const Rect*),
		get(Point*, InputMemoryStream*)

***************************************/

Burger::eError BURGER_API Burger::append(
	OutputMemoryStream* pOutput, const Point* pPoint) BURGER_NOEXCEPT
{
	// Save off the data to the stream
	eError uResult = pOutput->BigEndianAppend(static_cast<uint16_t>(pPoint->v));
	if (!uResult) {
		uResult = pOutput->BigEndianAppend(static_cast<uint16_t>(pPoint->h));
	}
	return uResult;
}
