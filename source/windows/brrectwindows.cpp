/***************************************

	Integer 2 dimensional Rect handlers

	Windows specific code

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brrect.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include <Windows.h>

/*! ************************************

	\brief Convert a Rect_t into a Windows RECT structure.

	Converts the contents of a Rect_t into a Windows RECT structure.

	\param pOutput Pointer to an uninitialized Windows RECT structure.
	\note This function is Windows or Xbox 360 only.

***************************************/

void Burger::Rect_t::Get(tagRECT* pOutput) const BURGER_NOEXCEPT
{
	pOutput->top = static_cast<LONG>(m_iTop);
	pOutput->left = static_cast<LONG>(m_iLeft);
	pOutput->bottom = static_cast<LONG>(m_iBottom);
	pOutput->right = static_cast<LONG>(m_iRight);
}

/*! ************************************

	\brief Convert a Windows RECT structure into a Rect_t

	Converts the contents of a Windows RECT structure into a Rect_t.

	\param pInput Pointer to a valid Windows RECT structure.
	\note This function is Windows or Xbox 360 only.

***************************************/

void Burger::Rect_t::Set(const tagRECT* pInput) BURGER_NOEXCEPT
{
	m_iLeft = static_cast<int32_t>(pInput->left);
	m_iTop = static_cast<int32_t>(pInput->top);
	m_iRight = static_cast<int32_t>(pInput->right);
	m_iBottom = static_cast<int32_t>(pInput->bottom);
}

#endif
