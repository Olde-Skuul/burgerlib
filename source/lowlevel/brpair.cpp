/***************************************

	pair<T> compatible dual object template

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brpair.h"

/*! ************************************

	\class Burger::Pair
	\brief Template for creating an object pair

	For cases where two objects need to be paired together, this
	template will easily perform that data pairing.

	This is a functional equivalent to the STL
	template "pair".

	\sa HashMap

***************************************/

/*! ************************************

	\fn Burger::Pair::Pair()
	\brief Default constructor

	\sa Pair(const Pair<T1,T2> &) or Pair(const T1 &,const T2 &)

***************************************/

/*! ************************************

	\fn Burger::Pair::Pair(const T1 &rFirst,const T2 &rSecond)
	\brief Construct with instances

	\param rFirst Reference to the first object to copy
	\param rSecond Reference to the second object to copy

	\sa Pair() or Pair(const Pair<T1,T2> &)

***************************************/

/*! ************************************

	\fn Burger::Pair::Pair(const Pair<T1,T2> &rInput)
	\brief Copy constructor

	\param rInput Reference to the pair to copy

	\sa Pair() or Pair(const T1 &,const T2 &)

***************************************/

/*! ************************************

	\typedef Burger::Pair::first_type
	\brief Type of the first object in the pair

	\sa second_type

***************************************/

/*! ************************************

	\typedef Burger::Pair::second_type
	\brief Type of the second object in the pair

	\sa first_type

***************************************/