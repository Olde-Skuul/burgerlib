/***************************************

	Integer Rect handlers

	Mac OSX specific code
	
	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brrect.h"

#if defined(BURGER_MACOSX)
#include <Carbon/Carbon.h>

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

/*! ************************************
 
	\brief Convert a Rect_t into a MacOS _NSRect structure.
 
	Converts the contents of a Rect_t into a MacOSX
	_NSRect structure.
 
	\param pOutput Pointer to an uninitialized MacOSX _NSRect structure.
 
	\note This function is 32 bit MacOSX only. This	is what NSRect maps to.
 
***************************************/

#if (!defined(BURGER_64BITCPU)) || defined(DOXYGEN)

void Burger::Rect_t::Get(_NSRect *pOutput) const
{
	pOutput->origin.x = static_cast<CGFloat>(m_iLeft);
	pOutput->origin.y = static_cast<CGFloat>(m_iTop);
	pOutput->size.width = static_cast<CGFloat>(m_iRight-m_iLeft);
	pOutput->size.height = static_cast<CGFloat>(m_iBottom-m_iTop);
}

/*! ************************************
 
	\brief Convert a CGRect Rect structure into a Rect_t
 
	Converts the contents of a MacOSX CGRect structure
	into a Rect_t. Since it's converting a float or double into an int,
	data loss could occur.
 
	\param pInput Pointer to a valid MacOSX _NSRect structure.
 
	\note This function is 32 bit MacOSX only. This is what NSRect maps to.

***************************************/

void Burger::Rect_t::Set(const _NSRect *pInput)
{
	m_iLeft = static_cast<int>(pInput->origin.x);
	m_iTop = static_cast<int>(pInput->origin.y);
	m_iRight = static_cast<int>(pInput->size.width-pInput->origin.x);
	m_iBottom = static_cast<int>(pInput->size.height-pInput->origin.y);
}

#endif
#endif
