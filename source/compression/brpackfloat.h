/***************************************

	Floating point compression

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPACKFLOAT_H__
#define __BRPACKFLOAT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern float BURGER_API Unpack16ToFloat(Int16 iInput);
extern Int16 BURGER_API PackFloatTo16(float fInput);
extern float BURGER_API Unpack16ToFloat(Int16 iInput,Word32 uBaseExponent);
extern Int16 BURGER_API PackFloatTo16(float fInput,Word32 uBaseExponent);
}
/* END */

#endif
