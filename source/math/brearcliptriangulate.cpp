/***************************************

	Ear Clip triangulate functions
	
	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#include "brearcliptriangulate.h"

/*! ************************************

	\brief Test two edges for intersection

	Test two edge line segments and determine if they are on top of each
	other, crossing or not crossing.
	
	\param pEdge1VSEdge2 Return negative if Edge1 crossing Edge2, 0 if touching, positive if not crossing
	\param pEdge2VSEdge1 Return negative if Edge2 crossing Edge1, 0 if touching, positive if not crossing
	\param pEdge1V1 Pointer to the first vertex of Edge1
	\param pEdge1V2 Pointer to the second vertex of Edge1
	\param pEdge2V1 Pointer to the first vertex of Edge2
	\param pEdge2V2 Pointer to the second vertex of Edge2

***************************************/

void BURGER_API Burger::EdgeIntersectTest(float *pEdge1VSEdge2,float *pEdge2VSEdge1,const Vector2D_t *pEdge1V1,const Vector2D_t *pEdge1V2,const Vector2D_t *pEdge2V1,const Vector2D_t *pEdge2V2)
{
	// Is the first edge just a point?

	if ((pEdge1V1->x == pEdge1V2->x) && (pEdge1V1->y == pEdge1V2->y)) {
		// Is the second edge just a point?
		if ((pEdge2V1->x == pEdge2V2->x) && (pEdge2V1->y == pEdge2V2->y)) {
			if ((pEdge2V1->x == pEdge1V1->x) && (pEdge2V1->y == pEdge1V1->y)) {
				// The edges are both dots and they are the same.
				*pEdge1VSEdge2 = 0.0f;
				*pEdge2VSEdge1 = 0.0f;
				return;
			}
		}
	}

	// Since the case that could cause the algorithm to go "TILT"
	// has been dealt with, let's proceed.	

	// See if Edge2 crosses line of Edge1.
	float fDeterminate1 = pEdge1V1->Determinant(pEdge1V2,pEdge2V1);
	float fDeterminate2 = pEdge1V1->Determinant(pEdge1V2,pEdge2V2);

	// Perform the test in which a line is on a line counts
	// as an intersection

	// 1.0f if it doesn't cross, 0 if equal, -1.0f if crossing
	*pEdge2VSEdge1 = Sign(fDeterminate1)*Sign(fDeterminate2);

	// See if Edge1 crosses line of Edge2.
	fDeterminate1 = pEdge2V1->Determinant(pEdge2V2,pEdge1V1);
	fDeterminate2 = pEdge2V1->Determinant(pEdge2V2,pEdge1V2);

	// 1.0f if it doesn't cross, 0 if equal, -1.0f if crossing
	*pEdge1VSEdge2 = Sign(fDeterminate1)*Sign(fDeterminate2);
}
