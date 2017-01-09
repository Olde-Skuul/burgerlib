/***************************************

	Integer Rect handlers

	Mac OS specific code
	
	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brrect.h"

#if defined(BURGER_MAC)
#include <MacTypes.h>

/*! ************************************

	\brief Convert a Rect_t into a MacOS Rect structure.
	
	Converts the contents of a Rect_t into a MacOS/Quicktime
	for Windows Rect structure. Since it's converting an int into a short,
	data loss could occur if the values exceed 32767 or are less than -32768.
	
	\param pOutput Pointer to an uninitialized MacOS Rect structure.
	\note This function is MacOS or Windows using Quicktime
	only.

***************************************/

void Burger::Rect_t::Get(Rect *pOutput) const
{
	pOutput->top = static_cast<short>(m_iTop);
	pOutput->left = static_cast<short>(m_iLeft);
	pOutput->bottom = static_cast<short>(m_iBottom);
	pOutput->right = static_cast<short>(m_iRight);
}

/*! ************************************

	\brief Convert a MacOS Rect structure into a Rect_t
	
	Converts the contents of a MacOS/Quicktime for Windows Rect structure
	into a Rect_t. Since it's converting a short to an int, no
	data loss will occur.
	
	\param pInput Pointer to a valid MacOS Rect structure.
	\note This function is MacOS or Windows using Quicktime
	only.

***************************************/

void Burger::Rect_t::Set(const Rect *pInput)
{
	m_iLeft = pInput->left;
	m_iTop = pInput->top;
	m_iRight = pInput->right;
	m_iBottom = pInput->bottom;
}

#endif
