/***************************************

	Simple First In First Out queue

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brqueue.h"

/*! ************************************

	\struct Burger::Queue::Element
	\brief A single data chunk in the queue
	
	This private data chunk is the representation of each
	element in the Queue

	\sa Queue
	
***************************************/

/*! ************************************

	\fn Burger::Queue::Element::Element(const T & rData)
	\brief Constructor

	Make a copy of the data passed in by reference and clear
	the forward link.

	\param rData Reference to a data chunk to copy into the queue
	\sa Queue::push(const T&) or Queue
	
***************************************/


/*! ************************************

	\class Burger::Queue
	\brief A simple first in first out queue
	
	This class will put data into a singly linked list and insert
	at the end and extract from the beginning.

	\sa RunQueue
	
***************************************/

/*! ************************************

	\fn Burger::Queue::Queue()
	\brief Initialize to defaults

	Creates an empty queue

	\sa clear(void)
	
***************************************/

/*! ************************************

	\fn Burger::Queue::~Queue()
	\brief Dispose of all data remaining in the Queue

	Frees all data

	\sa clear(void)
	
***************************************/

/*! ************************************

	\fn void Burger::Queue::clear(void)
	\brief Dispose of all data in the Queue

	Frees all data and clears out the queue. The
	queue is still valid, only empty.

	\sa clear(void), size(void) const or empty(void) const
	
***************************************/

/*! ************************************

	\fn const T& Burger::Queue::back(void) const
	\brief Get a reference to the last element in the list.

	This will return a reference to the last entry in the list.
	
	\note It will crash if there is no data in the list. Call
	size(void) const or empty(void) const to determine if there is valid
	data to access.

	\return A constant reference to the data at the end of the list.
	\sa front(void) const, back(void), size(void) const or empty(void) const
	
***************************************/

/*! ************************************

	\fn T& Burger::Queue::back(void)
	\brief Get a reference to the last element in the list.

	This will return a reference to the last entry in the list.
	
	\note It will crash if there is no data in the list. Call
	size(void) const or empty(void) const to determine if there is valid
	data to access.

	\return A reference to the data at the end of the list.
	\sa front(void), back(void) const, size(void) const or empty(void) const
	
***************************************/

/*! ************************************

	\fn const T& Burger::Queue::front(void) const
	\brief Get a reference to the first element in the list.

	This will return a reference to the first entry in the list.
	
	\note It will crash if there is no data in the list. Call
	size(void) const or empty(void) const to determine if there is valid
	data to access.

	\return A constant reference to the data at the front of the list.
	\sa back(void) const, front(void), size(void) const or empty(void) const
	
***************************************/

/*! ************************************

	\fn T& Burger::Queue::front(void)
	\brief Get a reference to the first element in the list.

	This will return a reference to the first entry in the list.
	
	\note It will crash if there is no data in the list. Call
	size(void) const or empty(void) const to determine if there is valid
	data to access.

	\return A reference to the data at the front of the list.
	\sa back(void), front(void) const, size(void) const or empty(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::Queue::pop(void)
	\brief Remove the first element in the list.

	This will dispose of the first entry in the list.
	
	\note It will crash if there is no data in the list. Call
	size(void) const or empty(void) const to determine if there is valid
	data to access.

	\sa push(const T &), size(void) const or empty(void) const
	
***************************************/

/*! ************************************

	\fn void Burger::Queue::push(const T&rData)
	\brief Insert data at the end of the list.

	Allocate memory for a new entry and place it at the end of
	linked list.

	\param rData Reference to a data chunk to copy into the queue
	\sa pop(void), front(void) const or back(void) const
	
***************************************/

/*! ************************************

	\fn WordPtr Burger::Queue::size(void) const
	\brief Get the number of entries in the queue

	\return The number of entries remaining in the queue.

	\sa empty(void) const or clear(void)
	
***************************************/

/*! ************************************

	\fn Word Burger::Queue::empty(void) const
	\brief Returns \ref TRUE if the queue is empty

	\return \ref TRUE if there is no data in the queue.

	\sa size(void) const or clear(void)
	
***************************************/