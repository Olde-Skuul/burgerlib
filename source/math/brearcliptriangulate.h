/***************************************

	Ear Clip triangulate functions
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BREARCLIPTRIANGULATE_H__
#define __BREARCLIPTRIANGULATE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRVECTOR2D_H__
#include "brvector2d.h"
#endif

/* BEGIN */
namespace Burger {
extern void BURGER_API EdgeIntersectTest(float *pEdge1VSEdge2,float *pEdge2VSEdge1,const Vector2D_t *pEdge1V1,const Vector2D_t *pEdge1V2,const Vector2D_t *pEdge2V1,const Vector2D_t *pEdge2V2);
}
/* END */

#endif
