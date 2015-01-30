/***************************************

	Integer Rect handlers

	iOS specific code
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brrect.h"

#if defined(BURGER_IOS)
#include <CoreGraphics/CoreGraphics.h>

/*! ************************************
 
	\brief Convert a Rect_t into a MacOSX/iOS CGRect structure.

	Converts the contents of a Rect_t into a MacOSX/iOS
	CGRect structure.
 
	\param pOutput Pointer to an uninitialized MacOSX/iOS CGRect structure.
 
	\note This function is MacOSX or iOS only.
 
***************************************/

void Burger::Rect_t::Get(CGRect *pOutput) const
{
	pOutput->origin.x = static_cast<CGFloat>(m_iLeft);
	pOutput->origin.y = static_cast<CGFloat>(m_iTop);
	pOutput->size.width = static_cast<CGFloat>(m_iRight-m_iLeft);
	pOutput->size.height = static_cast<CGFloat>(m_iBottom-m_iTop);
}

/*! ************************************
 
	\brief Convert a CGRect structure into a Rect_t
 
	Converts the contents of a MacOSX/iOS CGRect structure
	into a Rect_t. Since it's converting a float or double into an int,
	data loss could occur.
 
	\param pInput Pointer to a valid MacOSX/iOS CGRect structure.
	\note This function is MacOSX or iOS only.
 
***************************************/

void Burger::Rect_t::Set(const CGRect *pInput)
{
	m_iLeft = static_cast<int>(pInput->origin.x);
	m_iTop = static_cast<int>(pInput->origin.y);
	m_iRight = static_cast<int>(pInput->size.width-pInput->origin.x);
	m_iBottom = static_cast<int>(pInput->size.height-pInput->origin.y);
}

#endif
