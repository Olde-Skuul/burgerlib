/***************************************

    16 bit software manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brrenderersoftware16.h"

/*! ************************************

	\class Burger::RendererSoftware16
	\brief 16 bit software renderer

	This class contains functions to draw pixels in a 16 bit
	R:G:B buffer. It is 100% software driven, no hardware assist is
	performed at all

	\sa Renderer, RendererOpenGL and RendererSoftware8

***************************************/

/*! ************************************

	\brief Default constructor.
	
	Initializes all of the shared variables
	
***************************************/

Burger::RendererSoftware16::RendererSoftware16(void) :
	Renderer()
{
}



