/***************************************

	pair<T> compatible dual object template

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPAIR_H__
#define __BRPAIR_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
template<class T1,class T2>
class Pair {
public:
	T1 first;		///< First object in the pair
	T2 second;		///< Second object in the pair
	Pair() {}
	Pair(const T1 &rFirst,const T2 &rSecond) :
		first(rFirst),
		second(rSecond)
	{
	}
	Pair(const Pair<T1,T2> &rInput) :
		first(rInput.first),
		second(rInput.second)
	{
	}
	void operator=(const Pair<T1,T2> &rInput) {
		first=rInput.first;
		second=rInput.second;
	}
	typedef T1 first_type;
	typedef T2 second_type;
};
}
/* END */

#endif
