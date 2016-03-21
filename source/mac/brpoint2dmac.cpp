/***************************************

	Integer Point handlers

	Mac OS specific code

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brpoint2d.h"

#if defined(BURGER_MAC)
#include <MacTypes.h>

/*! ************************************

	\brief Convert a Burger::Point2D_t into a MacOS Point structure.
	
	Converts the contents of a Burger::Point2D_t into a MacOS/Quicktime
	for Windows Point structure. Since it's converting an int into a short,
	data loss could occur if the values exceed 32767 or are less than -32768.
	
	\param pOutput Pointer to an uninitialized MacOS Point structure.
	\note This function is MacOS or Windows using Quicktime
	only.

***************************************/

void Burger::Point2D_t::Get(Point *pOutput) const
{
	pOutput->v = static_cast<short>(m_iY);
	pOutput->h = static_cast<short>(m_iX);
}

/*! ************************************

	\brief Convert a MacOS Point structure into a Burger::Point2D_t
	
	Converts the contents of a MacOS/Quicktime for Windows Point structure
	into a Burger::Point2D_t. Since it's converting a short to an int, no
	data loss will occur.
	
	\param pInput Pointer to a valid MacOS Point structure.
	\note This function is MacOS or Windows using Quicktime
	only.

***************************************/

void Burger::Point2D_t::Set(const Point *pInput)
{
	m_iX = pInput->h;
	m_iY = pInput->v;
}

#endif
