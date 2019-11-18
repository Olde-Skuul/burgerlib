/***************************************

	Flash player color transform manager
	
	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BRFLASHCOLORTRANSFORM_H__
#define __BRFLASHCOLORTRANSFORM_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRPALETTE_H__
#include "brpalette.h"
#endif

#ifndef __BRFLASHSTREAM_H__
#include "brflashstream.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
class ColorTransform {
public:
	float m_fRedMultTerm;		///< Red scale value 0.0f to 1.0f
	float m_fGreenMultTerm;		///< Green scale value 0.0f to 1.0f
	float m_fBlueMultTerm;		///< Blue scale value 0.0f to 1.0f
	float m_fAlphaMultTerm;		///< Alpha scale value 0.0f to 1.0f
	float m_fRedAddTerm;		///< Red add value -255.0f to 255.0f
	float m_fGreenAddTerm;		///< Green add value -255.0f to 255.0f
	float m_fBlueAddTerm;		///< Blue add value -255.0f to 255.0f
	float m_fAlphaAddTerm;		///< Alpha add value -255.0f to 255.0f
	ColorTransform();
	void BURGER_API Identity(void);
	void BURGER_API Concatenate(const ColorTransform *pInput);
	void BURGER_API Transform(RGBAWord8_t *pOutput,const RGBAWord8_t *pInput) const;
	void BURGER_API ReadRGB(Stream* pStream);
	void BURGER_API ReadRGBA(Stream* pStream);
	void BURGER_API Clamp(void);
	void BURGER_API Print(void) const;
};
}}
/* END */

#endif
