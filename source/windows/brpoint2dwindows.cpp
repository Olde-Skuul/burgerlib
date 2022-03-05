/***************************************

	Integer 2 dimensional Point handlers

	Windows specific code

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brpoint2d.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

/*! ************************************

	\brief Convert a Point2D_t into a Microsoft POINT structure.

	Converts the contents of a Point2D_t into a Microsoft POINT
	structure.

	\param pOutput Pointer to an uninitialized Microsoft POINT structure.
	\note This function is Windows and Xbox 360 only.

***************************************/

void Burger::Point2D_t::Get(tagPOINT* pOutput) const BURGER_NOEXCEPT
{
	pOutput->x = static_cast<LONG>(m_iX);
	pOutput->y = static_cast<LONG>(m_iY);
}

/*! ************************************

	\brief Convert a Microsoft POINT structure into a Point2D_t

	Converts the contents of a Microsoft POINT structure into a Point2D_t.

	\param pInput Pointer to a valid Microsoft POINT structure.
	\note This function is Windows and Xbox 360 only.

***************************************/

void Burger::Point2D_t::Set(const tagPOINT* pInput) BURGER_NOEXCEPT
{
	m_iX = static_cast<int>(pInput->x);
	m_iY = static_cast<int>(pInput->y);
}

#endif
