/***************************************

	Mouse Manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brmouse.h"

#if !defined(BURGER_WINDOWS)

Burger::Mouse::Mouse(GameApp *pAppInstance) :
	m_pAppInstance(pAppInstance),
	m_uX(0),
	m_uY(0),
	m_uBoundsX(640),
	m_uBoundsY(480),
	m_iDeltaX(0),
	m_iDeltaY(0),
	m_iMouseWheel(0),
	m_uButtons(0),
	m_uPressedButtons(0)
{
	pAppInstance->SetMouse(this);
}

Burger::Mouse::~Mouse()
{
}

Word Burger::Mouse::IsPresent()
{
	return FALSE;
}

Word Burger::Mouse::ReadButtons(void)
{
	return 0;
}

#endif

Word Burger::Mouse::ReadButtonDowns(void)
{
	Word uResult = m_uPressedButtons;
	m_uPressedButtons = 0;
	return uResult;
}

void Burger::Mouse::Read(Word *pX,Word *pY)
{
	if (pX) {
		pX[0] = m_uX;
	}
	if (pY) {
		pY[0] = m_uY;
	}
}

void Burger::Mouse::Read(int *pX,int *pY)
{
	if (pX) {
		pX[0] = m_iDeltaX;
	}
	if (pY) {
		pY[0] = m_iDeltaY;
	}
	m_iDeltaX = 0;
	m_iDeltaY = 0;
}

int Burger::Mouse::ReadWheel(void)
{
	int iResult = m_iMouseWheel;
	m_iMouseWheel = 0;
	return iResult;
}

void Burger::Mouse::SetRange(Word uBoundsX,Word uBoundsY)
{
	m_uBoundsX = uBoundsX;
	m_uBoundsY = uBoundsY;
}

void Burger::Mouse::SetPosition(Word x,Word y)
{
	if (x>=m_uBoundsX) {
		x = m_uBoundsX-1;
	}
	if (y>=m_uBoundsY) {
		y = m_uBoundsY-1;
	}
	m_uX = x;
	m_uY = y;
}
