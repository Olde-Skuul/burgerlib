/***************************************

	3D fixed point matrix manager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BRFIXEDMATRIX3D_H__
#define __BRFIXEDMATRIX3D_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFIXEDPOINT_H__
#include "brfixedpoint.h"
#endif

#ifndef __BRFIXEDVECTOR3D_H__
#include "brfixedvector3d.h"
#endif

#ifndef __BRMATRIX3D_H__
#include "brmatrix3d.h"
#endif

/* BEGIN */
namespace Burger {
struct FixedMatrix3D_t {
	FixedVector3D_t x;	///< 32 bit fixed point X row for the 3D Matrix
	FixedVector3D_t y;	///< 32 bit fixed point Y row for the 3D Matrix
	FixedVector3D_t z;	///< 32 bit fixed point Z row for the 3D Matrix
	void BURGER_API Zero(void);
	void BURGER_API Identity(void);
	void BURGER_API Set(const Matrix3D_t *pInput);
	void BURGER_API Transpose(void);
	void BURGER_API Transpose(const FixedMatrix3D_t *pInput);
	void BURGER_API GetXRow(FixedVector3D_t *pOutput) const;
	void BURGER_API GetYRow(FixedVector3D_t *pOutput) const;
	void BURGER_API GetZRow(FixedVector3D_t *pOutput) const;
	void BURGER_API GetXColumn(FixedVector3D_t *pOutput) const;
	void BURGER_API GetYColumn(FixedVector3D_t *pOutput) const;
	void BURGER_API GetZColumn(FixedVector3D_t *pOutput) const;
	void BURGER_API SetXRow(const FixedVector3D_t *pInput);
	void BURGER_API SetYRow(const FixedVector3D_t *pInput);
	void BURGER_API SetZRow(const FixedVector3D_t *pInput);
	void BURGER_API SetXColumn(const FixedVector3D_t *pInput);
	void BURGER_API SetYColumn(const FixedVector3D_t *pInput);
	void BURGER_API SetZColumn(const FixedVector3D_t *pInput);
	void BURGER_API Multiply(Fixed32 fScale);
	void BURGER_API Multiply(const FixedMatrix3D_t *pInput,Fixed32 fScale);
};
extern const FixedMatrix3D_t s_FixedMatrix3DIdentity;
}
/* END */

#endif
