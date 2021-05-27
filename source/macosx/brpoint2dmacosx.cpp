/***************************************

    Integer Point handlers

    MacOSX specific code

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brpoint2d.h"

#if defined(BURGER_MACOSX)
#include <Carbon/Carbon.h>

/*! ************************************

	\brief Convert a Burger::Point2D_t into a MacOS Point structure.

	Converts the contents of a Burger::Point2D_t into a MacOS/Quicktime for
	Windows Point structure. Since it's converting an int into a short, data
	loss could occur if the values exceed 32767 or are less than -32768.

	\param pOutput Pointer to an uninitialized MacOS Point structure.
	\note This function is MacOS or Windows using Quicktime
		only.

***************************************/

void Burger::Point2D_t::Get(Point* pOutput) const
{
	pOutput->v = static_cast<short>(m_iY);
	pOutput->h = static_cast<short>(m_iX);
}

/*! ************************************

	\brief Convert a MacOS Point structure into a Burger::Point2D_t

	Converts the contents of a MacOS/Quicktime for Windows Point structure into
	a Burger::Point2D_t. Since it's converting a short to an int, no data loss
	will occur.

	\param pInput Pointer to a valid MacOS Point structure.
	\note This function is MacOS or Windows using Quicktime
		only.

***************************************/

void Burger::Point2D_t::Set(const Point* pInput)
{
	m_iX = pInput->h;
	m_iY = pInput->v;
}

/*! ************************************

	\brief Convert a Burger::Point2D_t into a MacOSX/iOS CGPoint structure.

	Converts the contents of a Burger::Point2D_t into a MacOSX/iOS CGPoint
	structure. Since it's converting an int into a float, data loss could occur.

	\param pOutput Pointer to an uninitialized MacOSX/iOS CGPoint structure.
	\note This function is MacOSX or iOS only.

***************************************/

void Burger::Point2D_t::Get(CGPoint* pOutput) const
{
	pOutput->x = static_cast<CGFloat>(m_iX);
	pOutput->y = static_cast<CGFloat>(m_iY);
}

/*! ************************************

	\brief Convert a MacOSX/iOS CGPoint structure into a Burger::Point2D_t

	Converts the contents of a MacOSX/iOS CGPoint structure into a
	Burger::Point2D_t. Since it's converting a float to an int, data loss could
	occur.

	\param pInput Pointer to a valid MacOSX/iOS CGPoint structure.
	\note This function is MacOSX or iOS only.

***************************************/

void Burger::Point2D_t::Set(const CGPoint* pInput)
{
	m_iX = static_cast<int>(pInput->x);
	m_iY = static_cast<int>(pInput->y);
}

/*! ************************************

	\brief Convert a Burger::Point2D_t into a MacOSX _NSPoint structure.

	Converts the contents of a Burger::Point2D_t into a MacOSX _NSPoint
	structure. Since it's converting an int into a float, data loss could occur.

	\param pOutput Pointer to an uninitialized MacOSX _NSPoint structure.
	\note This function is 32 bit MacOSX only. This	is what NSPoint maps to.

***************************************/

#if (!defined(BURGER_64BITCPU)) || defined(DOXYGEN)

void Burger::Point2D_t::Get(_NSPoint* pOutput) const
{
	pOutput->x = static_cast<CGFloat>(m_iX);
	pOutput->y = static_cast<CGFloat>(m_iY);
}

/*! ************************************

	\brief Convert a MacOSX _NSPoint structure into a Burger::Point2D_t

	Converts the contents of a MacOSX _NSPoint structure into a
	Burger::Point2D_t. Since it's converting a float to an int, data loss could
	occur.

	\param pInput Pointer to a valid MacOSX _NSPoint structure.
	\note This function is 32 bit MacOSX only. This	is what NSPoint maps to.

***************************************/

void Burger::Point2D_t::Set(const _NSPoint* pInput)
{
	m_iX = static_cast<int>(pInput->x);
	m_iY = static_cast<int>(pInput->y);
}

#endif

#endif
