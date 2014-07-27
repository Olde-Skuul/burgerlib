/***************************************

	4D fixed point matrix manager

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFIXEDMATRIX4D_H__
#define __BRFIXEDMATRIX4D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFIXEDPOINT_H__
#include "brfixedpoint.h"
#endif

#ifndef __BRFIXEDVECTOR4D_H__
#include "brfixedvector4d.h"
#endif

#ifndef __BRMATRIX4D_H__
#include "brmatrix4d.h"
#endif

/* BEGIN */
namespace Burger {
struct FixedMatrix4D_t {
	FixedVector4D_t x;	///< X row for the 4x4 Matrix
	FixedVector4D_t y;	///< Y row for the 4x4 Matrix
	FixedVector4D_t z;	///< Z row for the 4x4 Matrix
	FixedVector4D_t w;	///< W row for the 4x4 Matrix
	void BURGER_API Zero(void);
	void BURGER_API Identity(void);
	void BURGER_API Set(const Matrix4D_t *pInput);
	void BURGER_API Set(const FixedMatrix4D_t *pInput);
	void BURGER_API SetTranslate(Fixed32 fX,Fixed32 fY,Fixed32 fZ);
	void BURGER_API SetScale(Fixed32 fX,Fixed32 fY,Fixed32 fZ);
	void BURGER_API SetScale(Fixed32 fX,Fixed32 fY,Fixed32 fZ,Fixed32 fW);
};
extern const FixedMatrix4D_t s_FixedMatrix4DIdentity;
}
/* END */

#endif
