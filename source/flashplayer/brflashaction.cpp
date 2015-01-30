/***************************************

	Flash player action buffer
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#include "brflashaction.h"

BURGER_CREATE_STATICRTTI_PARENT(Burger::Flash::SharedBuffer,Burger::ReferenceCounter);

/*! ************************************

	\class Burger::Flash::SharedBuffer
	\brief Shared data buffer

	A shared buffer that's reference counted so multiple
	action script items can manager a single copy
	of this buffer

	\sa ReferenceCounter

***************************************/

/*! ************************************

	\brief Default constructor

	Initialize to an empty buffer

	\sa ~SharedBuffer()

***************************************/

Burger::Flash::SharedBuffer::SharedBuffer() :
	ReferenceCounter(),
	m_Buffer()
{
	m_Buffer.push_back(1);
}

/*! ************************************

	\brief Release all data

	Release the buffer and shut down the class

	\sa SharedBuffer()

***************************************/

Burger::Flash::SharedBuffer::~SharedBuffer()
{
}

/*! ************************************

	\fn WordPtr Burger::Flash::SharedBuffer::GetDataSize(void) const
	\brief Get the size of the buffer in bytes

	\return The number of bytes in the buffer
	\sa GetData(void) const

***************************************/

/*! ************************************

	\fn void Burger::Flash::SharedBuffer::Append(Word8 uInput)
	\brief Append a byte at the end of the buffer

	\return The number of bytes in the buffer
	\sa GetData(void) const or GetBuffer(void)

***************************************/

/*! ************************************

	\fn const Word8 * Burger::Flash::SharedBuffer::GetData(void) const
	\brief Get the pointer to the buffer

	\return The number of bytes in the buffer
	\sa GetBuffer(void)

***************************************/

/*! ************************************

	\fn SimpleArray<Word8>* Burger::Flash::SharedBuffer::GetBuffer(void)
	\brief Get the pointer to the SimpleArray

	\return A pointer to the internal SimpleArray
	\sa GetData(void) const

***************************************/

/*! ************************************

	\var const Burger::StaticRTTI Burger::Flash::SharedBuffer::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/