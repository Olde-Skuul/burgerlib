/***************************************

    Integer Point handlers

    iOS specific code

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brpoint2d.h"

#if defined(BURGER_IOS)
#include <CoreGraphics/CoreGraphics.h>

/*! ************************************
 
	\brief Convert a Burger::Point2D_t into a MacOSX/iOS CGPoint structure.
 
	Converts the contents of a Burger::Point2D_t into a MacOSX/iOS
	CGPoint structure. Since it's converting an int into a float,
	data loss could occur.
 
	\param pOutput Pointer to an uninitialized MacOSX/iOS CGPoint structure.
	\note This function is MacOSX or iOS only.
 
***************************************/

void Burger::Point2D_t::Get(CGPoint *pOutput) const
{
	pOutput->x = static_cast<CGFloat>(m_iX);
	pOutput->y = static_cast<CGFloat>(m_iY);
}

/*! ************************************
 
	\brief Convert a MacOSX/iOS CGPoint structure into a Burger::Point2D_t
 
	Converts the contents of a MacOSX/iOS CGPoint structure
	into a Burger::Point2D_t. Since it's converting a float to an int,
	data loss could occur.
 
	\param pInput Pointer to a valid MacOSX/iOS CGPoint structure.
	\note This function is MacOSX or iOS only.
 
***************************************/

void Burger::Point2D_t::Set(const CGPoint *pInput)
{
	m_iX = static_cast<int>(pInput->x);
	m_iY = static_cast<int>(pInput->y);
}

#endif

