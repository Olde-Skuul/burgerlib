/***************************************

    Display object manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdisplayobject.h"

#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::DisplayObject,Burger::ReferenceCounter);
#endif

/*! ************************************

	\class Burger::DisplayObject
	\brief Class for managing display objects

	This class is adds an object intended for rendering
	onto a GPU or other video display device where the
	GPU could flush the data and needs a mechanism to clear
	and / or restore graphic objects into a video display card.

	\sa Display, TextureOpenGL or TextureDirectX9

***************************************/

Burger::DisplayObject *Burger::DisplayObject::g_pHead = NULL;

/*! ************************************

	\brief Add this display object to the global linked list

	To allow all display objects to get an event such as a
	reload due to video display card loss, this function
	will add this DisplayObject to the global list

	\sa ~DisplayObject()

***************************************/

Burger::DisplayObject::DisplayObject(void)
{
	// Set the links to make this head of the list
	m_pPrevDisplayObject = NULL;

	// Get the previous head
	DisplayObject *pHead = g_pHead;
	m_pNextDisplayObject = pHead;
	g_pHead = this;		// Set the new head
	if (pHead) {
		// Set the new reverse link to this entry
		pHead->m_pPrevDisplayObject = this;
	}
}

/*! ************************************

	\brief Remove this display object from the global linked list

	To allow all display objects to get an event such as a
	reload due to video card loss, this function
	will remove this DisplayObject from the global list

	\sa DisplayObject(void)

***************************************/

Burger::DisplayObject::~DisplayObject()
{
	DisplayObject *pNext = m_pNextDisplayObject;
	DisplayObject *pPrev = m_pPrevDisplayObject;

	// Was this texture the head?
	if (g_pHead==this) {
		g_pHead = pNext;
	}
	// Unlink
	if (pPrev) {
		pPrev->m_pNextDisplayObject = pNext;
	}
	if (pNext) {
		pNext->m_pPrevDisplayObject = pPrev;
	}
	// Remove the links
	m_pNextDisplayObject = NULL;
	m_pPrevDisplayObject = NULL;
}

/*! ************************************

	\fn DisplayObject *Burger::DisplayObject::GetFirstObject(void)
	\brief Get the first display object in the global linked list

	To traverse the linked list of display objects, use this function to
	get the first display object and if not \ref NULL, use GetNext(void) const
	to get the next display object until the end of the linked list is
	reached when a \ref NULL is found.

	\return \ref NULL if no display objects exist or a valid DisplayObject pointer at the
	head of the list.

	\sa GetNext(void) const or GetPrevious(void) const

***************************************/

/*! ************************************

	\fn DisplayObject *Burger::DisplayObject::GetNext(void) const
	\brief Get the next display object in the global linked list

	Return the pointer to the next display object in the linked list.

	\return \ref NULL if no display objects exist or the next valid DisplayObject pointer.

	\sa GetFirstObject(void) or GetPrevious(void) const

***************************************/

/*! ************************************

	\fn DisplayObject *Burger::DisplayObject::GetPrevious(void) const
	\brief Get the previous display object in the global linked list

	Return the pointer to the previous display object in the linked list.

	\return \ref NULL if no display objects exist or the previous valid DisplayObject pointer.

	\sa GetFirstObject(void) or GetNext(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::DisplayObject::CheckLoad(Display *pDisplay)
	\brief Load data onto the video display card

	Allow the derived class to load a display object from disk
	or memory and convert into a proper data for uploading
	onto the display card.

	\return Zero if no error, non zero if the object couldn't be loaded
	\sa Release(Display *)

***************************************/

/*! ************************************

	\fn void Burger::DisplayObject::Release(Display *pDisplay)
	\brief Release hardware resources

	Allow the derived class to release hardware
	resources

	\sa CheckLoad(Display *)

***************************************/

/*! ************************************

	\brief Release all display objects

	Iterate over every display object in existence and release them
	from the display instance.

***************************************/

void BURGER_API Burger::DisplayObject::ReleaseAll(Display *pDisplay)
{
	DisplayObject *pHead = g_pHead;
	if (pHead) {
		do {
			pHead->Release(pDisplay);
			pHead = pHead->GetNext();
		} while (pHead);
	}
}

/*! ************************************

	\var const Burger::StaticRTTI Burger::DisplayObject::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/
