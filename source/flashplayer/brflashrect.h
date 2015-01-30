/***************************************

	Flash player rectangle manager
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BRFLASHRECT_H__
#define __BRFLASHRECT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRVECTOR4D_H__
#include "brvector4d.h"
#endif

#ifndef __BRFLASHSTREAM_H__
#include "brflashstream.h"
#endif

#ifndef __BRFLASHMATRIX23_H__
#include "brflashmatrix23.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
class Rect : public Vector4D_t {
public:
	Rect() {
		Zero();
	}
	void BURGER_API Read(Stream* pStream);
	Word BURGER_API IsIntersecting(const Vector4D_t *pRect) const;
	void BURGER_API SetRect(const Matrix23 *pMatrix,const Vector4D_t *pRect);
	void BURGER_API TwipsToPixels(void);
	void BURGER_API PixelsToTwips(void);
	void BURGER_API Print(void) const;
};
}}
/* END */

#endif
