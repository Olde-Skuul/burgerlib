/***************************************

	Vertex buffer class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brvertexbuffer.h"

BURGER_CREATE_STATICRTTI_PARENT(Burger::VertexBuffer,Burger::ReferenceCounter);

#if defined(BURGER_WINDOWS) || !(defined(BURGER_XBOX360) || defined(BURGER_OPENGL_SUPPORTED)) || defined(DOXYGEN)

/*! ************************************

	\brief Default constructor
	
	Initialize to an empty vertex buffer

***************************************/

Burger::VertexBuffer::VertexBuffer()
{
}

/*! ************************************

	\brief Default destructor
	
	Release all data

	\sa Shutdown(void)

***************************************/

Burger::VertexBuffer::~VertexBuffer()
{
}

/*! ************************************

	\brief Load vertex data to VRAM

	Given a pointer to the display to render to and a structure
	describing the vertex data, upload the data to VRAM
	and retain information to allow rendering with
	graphics primitives

	\param pDisplay Pointer to the current display
	\param pDescription Pointer to an initialized structure that describes the data to upload to VRAM.
	\sa ReleaseData(void)

***************************************/

#if !defined(BURGER_WINDOWS) || defined(DOXYGEN)
Word Burger::VertexBuffer::LoadData(Display * /*pDisplay */,const VertexAoS_t * /* pDescription */)
{
	return 10;
}

/*! ************************************

	\brief Release vertex data from VRAM

	If any data was uploaded to VRAM, release it.

	\sa LoadData(Display *,const VertexAoS_t *)

***************************************/

void Burger::VertexBuffer::ReleaseData(void)
{
}
#endif

#endif