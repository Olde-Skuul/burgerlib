/***************************************

    Palette manager class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brpalette.h"
#include "brrenderer.h"
#include "brfixedpoint.h"

/*! ************************************

	\struct Burger::RGBWord8_t
	\brief Red, Green and Blue 8 bit values

	Three byte structure that contains the red, green and blue
	(in that order) color components where the value of
	0 mean absence of color and 255 is the maximum color
	
	\sa Burger::RGBAWord8_t, Burger::RGBWord16_t and Burger::RGBFloat_t

***************************************/

/*! ************************************

	\var Burger::RGBWord8_t::Aqua
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 0,255,255 :</td><td style="width:100%; background-color:#00FFFF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Black
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 0,0,0 :</td><td style="width:100%; background-color:#000000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Blue
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 0,0,255 :</td><td style="width:100%; background-color:#0000FF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Fuchsia
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 255,0,255 :</td><td style="width:100%; background-color:#FF00FF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Gray
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 128,128,128 :</td><td style="width:100%; background-color:#808080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Green
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 0,128,0 :</td><td style="width:100%; background-color:#008000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Lime
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 0,255,0 :</td><td style="width:100%; background-color:#00FF00">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Maroon
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 128,0,0 :</td><td style="width:100%; background-color:#800000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Navy
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 0,0,128 :</td><td style="width:100%; background-color:#000080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Olive
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 128,128,0 :</td><td style="width:100%; background-color:#808000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Orange
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 255,165,0 :</td><td style="width:100%; background-color:#FFA500">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Purple
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 128,0,128 :</td><td style="width:100%; background-color:#800080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Red
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 255,0,0 :</td><td style="width:100%; background-color:#FF0000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Silver
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 192,192,192 :</td><td style="width:100%; background-color:#C0C0C0">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Teal
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 0,128,128 :</td><td style="width:100%; background-color:#008080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::White
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 255,255,255 :</td><td style="width:100%; background-color:#FFFFFF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBWord8_t::Yellow
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGB 255,255,0 :</td><td style="width:100%; background-color:#FFFF00">&nbsp;</td></table> \endhtmlonly

***************************************/

const Burger::RGBWord8_t Burger::RGBWord8_t::Aqua = {0,255,255};
const Burger::RGBWord8_t Burger::RGBWord8_t::Black = {0,0,0};
const Burger::RGBWord8_t Burger::RGBWord8_t::Blue = {0,0,255};
const Burger::RGBWord8_t Burger::RGBWord8_t::Fuchsia = {255,0,255};
const Burger::RGBWord8_t Burger::RGBWord8_t::Gray = {128,128,128};
const Burger::RGBWord8_t Burger::RGBWord8_t::Green = {0,128,0};
const Burger::RGBWord8_t Burger::RGBWord8_t::Lime = {0,255,0};
const Burger::RGBWord8_t Burger::RGBWord8_t::Maroon = {128,0,0};
const Burger::RGBWord8_t Burger::RGBWord8_t::Navy = {0,0,128};
const Burger::RGBWord8_t Burger::RGBWord8_t::Olive = {128,128,0};
const Burger::RGBWord8_t Burger::RGBWord8_t::Orange = {255,165,0};
const Burger::RGBWord8_t Burger::RGBWord8_t::Purple = {128,0,128};
const Burger::RGBWord8_t Burger::RGBWord8_t::Red = {255,0,0};
const Burger::RGBWord8_t Burger::RGBWord8_t::Silver = {192,192,192};
const Burger::RGBWord8_t Burger::RGBWord8_t::Teal = {0,128,128};
const Burger::RGBWord8_t Burger::RGBWord8_t::White = {255,255,255};
const Burger::RGBWord8_t Burger::RGBWord8_t::Yellow = {255,255,0};

/*! ************************************

	\fn uint32_t Burger::RGBWord8_t::ToRGB(void) const
	\brief Convert the color to an RGB 32 bit integer

	Convert the color into a 32 bit integer whereas R<<16+G<<8+B
	
	\return A 32 bit integer in native endian with the color in the lower 24 bits
	\sa FromRGB(uint32_t) or ToBGR(void) const

***************************************/

/*! ************************************

	\fn uint32_t Burger::RGBWord8_t::ToBGR(void) const
	\brief Convert the color to a BGR 32 bit integer

	Convert the color into a 32 bit integer whereas B<<16+G<<8+R
	
	\return A 32 bit integer in native endian with the color in the lower 24 bits
	\sa FromBGR(uint32_t) or ToRGB(void) const

***************************************/

/*! ************************************

	\fn void Burger::RGBWord8_t::FromRGB(uint32_t uColor)
	\brief Initialize the color with a 32 bit integer

	Convert the color from a 32 bit integer whereas R<<16+G<<8+B
	and store the Red, Green and Blue components
	
	\param uColor 32 bit color value that is formatted as R<<16+G<<8+B
	\sa ToRGB(void) const or FromBGR(uint32_t)

***************************************/

/*! ************************************

	\fn void Burger::RGBWord8_t::FromBGR(uint32_t uColor)
	\brief Initialize the color with a 32 bit integer

	Convert the color from a 32 bit integer whereas B<<16+G<<8+R
	and store the Red, Green and Blue components
	
	\param uColor 32 bit color value that is formatted as B<<16+G<<8+R
	\sa ToBGR(void) const or FromRGB(uint32_t)

***************************************/

/*! ************************************

	\brief Interpolates between two colors

	Interpolate with linear scaling between two colors. 0.0f will return the From
	color and 1.0f will return the To color and all factors in between
	will yield the proper mix of the two colors.

	\param pFrom Pointer to color convert from
	\param pTo Pointer to color to convert o
	\param fFactor Interpolation factor between 0.0f to 1.0f (0.0f = pFrom, 1.0f = pTo)

***************************************/

void BURGER_API Burger::RGBWord8_t::Interpolate(const RGBWord8_t *pFrom,const RGBWord8_t *pTo,float fFactor)
{
	Fixed32 lFactor = BURGER_FLOAT_TO_FIXED(fFactor);
	m_uRed = static_cast<uint8_t>(fixed_to_int_nearest((pFrom->m_uRed-pTo->m_uRed)*lFactor)+pFrom->m_uRed);
	m_uGreen = static_cast<uint8_t>(fixed_to_int_nearest((pFrom->m_uGreen-pTo->m_uGreen)*lFactor)+pFrom->m_uGreen);
	m_uBlue = static_cast<uint8_t>(fixed_to_int_nearest((pFrom->m_uBlue-pTo->m_uBlue)*lFactor)+pFrom->m_uBlue);
}

/*! ************************************

	\struct Burger::RGBAWord8_t
	\brief Red, Green,Blue and Alpha 8 bit values

	Four byte structure that contains the red, green, blue and alpha
	(in that order) color components where the value of
	0 mean absence of color and 255 is the maximum color
	
	\sa Burger::RGBWord8_t, Burger::RGBAWord16_t and Burger::RGBAFloat_t

***************************************/

/*! ************************************

	\var Burger::RGBAWord8_t::Aqua
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,255,255,255 :</td><td style="width:100%; background-color:#00FFFF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Black
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0,0,255 :</td><td style="width:100%; background-color:#000000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Blue
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0,255,255 :</td><td style="width:100%; background-color:#0000FF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Fuchsia
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 255,0,255,255 :</td><td style="width:100%; background-color:#FF00FF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Gray
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 128,128,128,255 :</td><td style="width:100%; background-color:#808080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Green
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,128,0,255 :</td><td style="width:100%; background-color:#008000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Lime
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,255,0,255 :</td><td style="width:100%; background-color:#00FF00">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Maroon
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 128,0,0,255 :</td><td style="width:100%; background-color:#800000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Navy
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0,128,255 :</td><td style="width:100%; background-color:#000080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Olive
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 128,128,0,255 :</td><td style="width:100%; background-color:#808000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Orange
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 255,165,0,255 :</td><td style="width:100%; background-color:#FFA500">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Purple
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 128,0,128,255 :</td><td style="width:100%; background-color:#800080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Red
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 255,0,0,255 :</td><td style="width:100%; background-color:#FF0000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Silver
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 192,192,192,255 :</td><td style="width:100%; background-color:#C0C0C0">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Teal
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,128,128,255 :</td><td style="width:100%; background-color:#008080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::White
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 255,255,255,255 :</td><td style="width:100%; background-color:#FFFFFF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAWord8_t::Yellow
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 255,255,0,255 :</td><td style="width:100%; background-color:#FFFF00">&nbsp;</td></table> \endhtmlonly

***************************************/

const Burger::RGBAWord8_t Burger::RGBAWord8_t::Aqua = {0,255,255,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Black = {0,0,0,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Blue = {0,0,255,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Fuchsia = {255,0,255,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Gray = {128,128,128,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Green = {0,128,0,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Lime = {0,255,0,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Maroon = {128,0,0,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Navy = {0,0,128,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Olive = {128,128,0,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Orange = {255,165,0,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Purple = {128,0,128,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Red = {255,0,0,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Silver = {192,192,192,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Teal = {0,128,128,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::White = {255,255,255,255};
const Burger::RGBAWord8_t Burger::RGBAWord8_t::Yellow = {255,255,0,255};

/*! ************************************

	\fn uint32_t Burger::RGBAWord8_t::ToARGB(void) const
	\brief Convert the color to an ARGB 32 bit integer

	Convert the color into a 32 bit integer whereas A<<24+R<<16+G<<8+B
	
	\return A 32 bit integer in native endian with the color
	\sa ToABGR(void) const or FromARGB(uint32_t)

***************************************/

/*! ************************************

	\fn uint32_t Burger::RGBAWord8_t::ToABGR(void) const
	\brief Convert the color to a ABGR 32 bit integer

	Convert the color into a 32 bit integer whereas A<<24+B<<16+G<<8+R
	
	\return A 32 bit integer in native endian with the color
	\sa ToARGB(void) const or FromABGR(uint32_t)

***************************************/

/*! ************************************

	\fn void Burger::RGBAWord8_t::FromARGB(uint32_t uColor)
	\brief Initialize the color with a 32 bit integer

	Convert the color from a 32 bit integer whereas A<<24+R<<16+G<<8+B
	and store the Alpha, Red, Green and Blue components
	
	\param uColor 32 bit color value that is formatted as A<<24+R<<16+G<<8+B
	\sa ToARGB(void) const or FromABGR(uint32_t)

***************************************/

/*! ************************************

	\fn void Burger::RGBAWord8_t::FromABGR(uint32_t uColor)
	\brief Initialize the color with a 32 bit integer

	Convert the color from a 32 bit integer whereas A<<24+B<<16+G<<8+R
	and store the Alpha, Red, Green and Blue components
	
	\param uColor 32 bit color value that is formatted as A<<24+B<<16+G<<8+R
	\sa ToABGR(void) const or FromARGB(uint32_t)

***************************************/

/*! ************************************

	\brief Interpolates between two colors

	Interpolate with linear scaling between two colors. 0.0f will return the From
	color and 1.0f will return the To color and all factors in between
	will yield the proper mix of the two colors.

	\param pFrom Pointer to color convert from
	\param pTo Pointer to color to convert o
	\param fFactor Interpolation factor between 0.0f to 1.0f (0.0f = pFrom, 1.0f = pTo)

***************************************/

void BURGER_API Burger::RGBAWord8_t::Interpolate(const RGBAWord8_t *pFrom,const RGBAWord8_t *pTo,float fFactor)
{
	Fixed32 lFactor = BURGER_FLOAT_TO_FIXED(fFactor);
	m_uRed = static_cast<uint8_t>(fixed_to_int_nearest((pFrom->m_uRed-pTo->m_uRed)*lFactor)+pFrom->m_uRed);
	m_uGreen = static_cast<uint8_t>(fixed_to_int_nearest((pFrom->m_uGreen-pTo->m_uGreen)*lFactor)+pFrom->m_uGreen);
	m_uBlue = static_cast<uint8_t>(fixed_to_int_nearest((pFrom->m_uBlue-pTo->m_uBlue)*lFactor)+pFrom->m_uBlue);
	m_uAlpha = static_cast<uint8_t>(fixed_to_int_nearest((pFrom->m_uAlpha-pTo->m_uAlpha)*lFactor)+pFrom->m_uAlpha);
}

/*! ************************************

	\struct Burger::RGBWord16_t
	\brief Red, Green and Blue 16 bit values

	Six byte structure that contains the red, green and blue
	(in that order) color components where the value of
	0 mean absence of color and 65535 is the maximum color
	
	\sa Burger::RGBAWord16_t, Burger::RGBWord8_t and Burger::RGBFloat_t

***************************************/

/*! ************************************

	\struct Burger::RGBAWord16_t
	\brief Red, Green,Blue and Alpha 16 bit values

	Eight byte structure that contains the red, green, blue and alpha
	(in that order) color components where the value of
	0 mean absence of color and 65535 is the maximum color
	
	\sa Burger::RGBWord16_t, Burger::RGBAWord8_t and Burger::RGBAFloat_t

***************************************/

/*! ************************************

	\struct Burger::RGBFloat_t
	\brief Red, Green and Blue 32 bit floating point values
	
	Structure that contains the red, green and blue
	(in that order) color components where the value of
	0.0f mean absence of color and 1.0f is the maximum color
	
	\sa Burger::RGBAFloat_t, Burger::RGBWord8_t or Burger::RGBWord16_t

***************************************/

/*! ************************************

	\var Burger::RGBFloat_t::Aqua
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,1.0f,1.0f :</td><td style="width:100%; background-color:#00FFFF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Black
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0,0 :</td><td style="width:100%; background-color:#000000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Blue
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0,1.0f :</td><td style="width:100%; background-color:#0000FF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Fuchsia
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 1.0f,0,1.0f :</td><td style="width:100%; background-color:#FF00FF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Gray
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0.5f,0.5f,0.5f :</td><td style="width:100%; background-color:#808080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Green
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0.5f,0 :</td><td style="width:100%; background-color:#008000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Lime
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,1.0f,0 :</td><td style="width:100%; background-color:#00FF00">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Maroon
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0.5f,0,0 :</td><td style="width:100%; background-color:#800000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Navy
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0,0.5f :</td><td style="width:100%; background-color:#000080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Olive
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0.5f,0.5f,0 :</td><td style="width:100%; background-color:#808000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Orange
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 1.0f,165.0f/255.0f,0 :</td><td style="width:100%; background-color:#FFA500">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Purple
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0.5f,0,0.5f :</td><td style="width:100%; background-color:#800080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Red
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 1.0f,0,0 :</td><td style="width:100%; background-color:#FF0000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Silver
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0.75f,0.75f,0.75f :</td><td style="width:100%; background-color:#C0C0C0">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Teal
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0.5f,0.5f :</td><td style="width:100%; background-color:#008080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::White
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 1.0f,1.0f,1.0f :</td><td style="width:100%; background-color:#FFFFFF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBFloat_t::Yellow
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 1.0f,1.0f,0 :</td><td style="width:100%; background-color:#FFFF00">&nbsp;</td></table> \endhtmlonly

***************************************/

const Burger::RGBFloat_t Burger::RGBFloat_t::Aqua = {0,1.0f,1.0f};
const Burger::RGBFloat_t Burger::RGBFloat_t::Black = {0,0,0};
const Burger::RGBFloat_t Burger::RGBFloat_t::Blue = {0,0,1.0f};
const Burger::RGBFloat_t Burger::RGBFloat_t::Fuchsia = {1.0f,0,1.0f};
const Burger::RGBFloat_t Burger::RGBFloat_t::Gray = {0.5f,0.5f,0.5f};
const Burger::RGBFloat_t Burger::RGBFloat_t::Green = {0,0.5f,0};
const Burger::RGBFloat_t Burger::RGBFloat_t::Lime = {0,1.0f,0};
const Burger::RGBFloat_t Burger::RGBFloat_t::Maroon = {0.5f,0,0};
const Burger::RGBFloat_t Burger::RGBFloat_t::Navy = {0,0,0.5f};
const Burger::RGBFloat_t Burger::RGBFloat_t::Olive = {0.5f,0.5f,0};
const Burger::RGBFloat_t Burger::RGBFloat_t::Orange = {1.0f,165.0f/255.0f,0};
const Burger::RGBFloat_t Burger::RGBFloat_t::Purple = {0.5f,0,0.5f};
const Burger::RGBFloat_t Burger::RGBFloat_t::Red = {1.0f,0,0};
const Burger::RGBFloat_t Burger::RGBFloat_t::Silver = {0.75f,0.75f,0.75f};
const Burger::RGBFloat_t Burger::RGBFloat_t::Teal = {0,0.5f,0.5f};
const Burger::RGBFloat_t Burger::RGBFloat_t::White = {1.0f,1.0f,1.0f};
const Burger::RGBFloat_t Burger::RGBFloat_t::Yellow = {1.0f,1.0f,0};

/*! ************************************

	\fn Burger::RGBFloat_t::operator Vector3D_t &()
	\brief Convert the color to a Vector3D_t

	Since the RGBFloat_t structure is identical to a 
	Vector3D_t, this operator will allow the color to
	be used with the math operators that use a Vector3D_t
	as input or output.
	
	\return A reference to this object cast as a Vector3D_t

	\sa RGBFloat_t::operator const Vector3D_t &() const or RGBFloat_t::operator = (const Vector3D_t &)

***************************************/

/*! ************************************

	\fn Burger::RGBFloat_t::operator const Vector3D_t &() const
	\brief Convert the color to a const Vector3D_t

	Since the RGBFloat_t structure is identical to a 
	Vector3D_t, this operator will allow the color to
	be used with the math operators that use a Vector3D_t
	as input.
	
	\return A reference to this object cast as a const Vector3D_t

	\sa RGBFloat_t::operator Vector3D_t &() or RGBFloat_t::operator = (const Vector3D_t &)

***************************************/

/*! ************************************

	\fn Burger::RGBFloat_t::operator = (const Vector3D_t &)
	\brief Convert a Vector3D_t into a RGBFloat_t

	Since the RGBFloat_t structure is identical to a 
	Vector3D_t, this operator will copy a Vector3D_t into
	this color structure
	
	\param rInput Reference to a Vector3D_t to copy into this structure
	\return A reference to this object

	\sa RGBFloat_t::operator Vector3D_t &() or RGBFloat_t::operator const Vector3D_t &() const

***************************************/

/*! ************************************

	\brief Interpolate between colors using the HSL color wheel

	Interpolate between two colors using the HSL color wheel to make
	a smoother color shift.
	
	\param pInput1 Pointer to the first color
	\param pInput2 Pointer to the second color
	\param fFactor Interpolation value between 0.0f and 1.0f
	\param bDirection \ref FALSE for Red to Green to Blue, \ref TRUE for Red to Blue to Green
	\sa HSL_t::Interpolate(const HSL_t*,const HSL_t*,float,uint_t);

***************************************/

void BURGER_API Burger::RGBFloat_t::HSLInterpolate(const RGBFloat_t *pInput1,const RGBFloat_t *pInput2,float fFactor,uint_t bDirection)
{
	HSL_t HSL1;
	HSL_t HSL2;
	HSL_t NewHSL;

	Convert(&HSL1,pInput1);		// Convert to HSL
	Convert(&HSL2,pInput2);
	NewHSL.Interpolate(&HSL1,&HSL2,fFactor,bDirection);	// Get the interpolated value
	Convert(this,&NewHSL);		// Return as RGB
}

/*! ************************************

	\struct Burger::RGBAFloat_t
	\brief Red, Green,Blue and Alpha 32 bit floating point values
	
	Structure that contains the red, green, blue and alpha
	(in that order) color components where the value of
	0.0f mean absence of color and 1.0f is the maximum color
	
	\sa Burger::RGBFloat_t, Burger::RGBAWord8_t or Burger::RGBAWord16_t

***************************************/


/*! ************************************

	\var Burger::RGBAFloat_t::Aqua
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,1.0f,1.0f,1.0f :</td><td style="width:100%; background-color:#00FFFF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Black
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0,0,1.0f :</td><td style="width:100%; background-color:#000000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Blue
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0,1.0f,1.0f :</td><td style="width:100%; background-color:#0000FF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Fuchsia
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 1.0f,0,1.0f,1.0f :</td><td style="width:100%; background-color:#FF00FF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Gray
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0.5f,0.5f,0.5f,1.0f :</td><td style="width:100%; background-color:#808080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Green
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0.5f,0,1.0f :</td><td style="width:100%; background-color:#008000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Lime
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,1.0f,0,1.0f :</td><td style="width:100%; background-color:#00FF00">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Maroon
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0.5f,0,0,1.0f :</td><td style="width:100%; background-color:#800000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Navy
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0,0.5f,1.0f :</td><td style="width:100%; background-color:#000080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Olive
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0.5f,0.5f,0,1.0f :</td><td style="width:100%; background-color:#808000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Orange
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 1.0f,165.0f/255.0f,0,1.0f :</td><td style="width:100%; background-color:#FFA500">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Purple
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0.5f,0,0.5f,1.0f :</td><td style="width:100%; background-color:#800080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Red
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 1.0f,0,0,1.0f :</td><td style="width:100%; background-color:#FF0000">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Silver
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0.75f,0.75f,0.75f,1.0f :</td><td style="width:100%; background-color:#C0C0C0">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Teal
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 0,0.5f,0.5f,1.0f :</td><td style="width:100%; background-color:#008080">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::White
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 1.0f,1.0f,1.0f,1.0f :</td><td style="width:100%; background-color:#FFFFFF">&nbsp;</td></table> \endhtmlonly

	\var Burger::RGBAFloat_t::Yellow
	\htmlonly <table border="0" style="width:100%"><td style="white-space:nowrap">RGBA 1.0f,1.0f,0,1.0f :</td><td style="width:100%; background-color:#FFFF00">&nbsp;</td></table> \endhtmlonly

***************************************/

const Burger::RGBAFloat_t Burger::RGBAFloat_t::Aqua = {0,1.0f,1.0f,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Black = {0,0,0,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Blue = {0,0,1.0f,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Fuchsia = {1.0f,0,1.0f,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Gray = {0.5f,0.5f,0.5f,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Green = {0,0.5f,0,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Lime = {0,1.0f,0,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Maroon = {0.5f,0,0,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Navy = {0,0,0.5f,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Olive = {0.5f,0.5f,0,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Orange = {1.0f,165.0f/255.0f,0,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Purple = {0.5f,0,0.5f,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Red = {1.0f,0,0,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Silver = {0.75f,0.75f,0.75f,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Teal = {0,0.5f,0.5f,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::White = {1.0f,1.0f,1.0f,1.0f};
const Burger::RGBAFloat_t Burger::RGBAFloat_t::Yellow = {1.0f,1.0f,0,1.0f};

/*! ************************************

	\fn Burger::RGBAFloat_t::operator Vector4D_t &()
	\brief Convert the color to a Vector4D_t

	Since the RGBAFloat_t structure is identical to a 
	Vector4D_t, this operator will allow the color to
	be used with the math operators that use a Vector4D_t
	as input or output.
	
	\return A reference to this object cast as a Vector4D_t

	\sa RGBAFloat_t::operator const Vector4D_t &() const or RGBAFloat_t::operator = (const Vector4D_t &)

***************************************/

/*! ************************************

	\fn Burger::RGBAFloat_t::operator const Vector4D_t &() const
	\brief Convert the color to a const Vector4D_t

	Since the RGBAFloat_t structure is identical to a 
	Vector4D_t, this operator will allow the color to
	be used with the math operators that use a Vector4D_t
	as input.
	
	\return A reference to this object cast as a const Vector4D_t

	\sa RGBAFloat_t::operator Vector4D_t &() or RGBAFloat_t::operator = (const Vector4D_t &)

***************************************/

/*! ************************************

	\fn Burger::RGBAFloat_t::operator = (const Vector4D_t &)
	\brief Convert a Vector4D_t into a RGBFloat_t

	Since the RGBAFloat_t structure is identical to a 
	Vector4D_t, this operator will copy a Vector4D_t into
	this color structure
	
	\param rInput Reference to a Vector4D_t to copy into this structure
	\return A reference to this object

	\sa RGBAFloat_t::operator Vector4D_t &() or RGBAFloat_t::operator const Vector4D_t &() const

***************************************/


/*! ************************************

	\struct Burger::HSL_t
	\brief Hue, Saturation and Intensity 32 bit floating point values
	
	Structure that contains the hue, saturation and intensity
	(in that order) color components where the value of
	0.0f mean absence of color and 1.0f is the maximum color
	
	\sa RGBFloat_t

***************************************/

/*! ************************************

	\brief Interpolate between two HSL colors

	Given two colors, determine the linear interpolation between
	them. The direction of the color wheel can be switched with
	bDirection being \ref FALSE for red->green->blue while
	\ref TRUE goes blue->green->red.

	\param pInput1 Pointer to the first color
	\param pInput2 Pointer to the second color
	\param fFactor Interpolation value between 0.0f and 1.0f
	\param bDirection \ref FALSE for Red to Green to Blue, \ref TRUE for Red to Blue to Green
		
***************************************/

void BURGER_API Burger::HSL_t::Interpolate(const HSL_t *pInput1,const HSL_t *pInput2,float fFactor,uint_t bDirection)
{
	// Interpolate the hue

	float fHue1 = pInput1->m_fHue;		// Cache
	float fHue2 = pInput2->m_fHue;

	if (!bDirection) {				// Red->Green->Blue
		if (fHue2 >= fHue1) {		// Increase to tween?
			fHue1 = fHue1 + (fFactor * (fHue2 - fHue1));
		} else {
			fHue1 = fHue1 + (fFactor * (1.0f - (fHue1 - fHue2)));
			// Did it wrap?
			if (fHue1 > 1.0f) {
				fHue1 -= 1.0f;
			}
		}
	} else {
		// Blue->Green->Red
		if (fHue1 >= fHue2) {
			fHue1 = fHue1 - (fFactor * (fHue1 - fHue2));
		} else {
			fHue1 = fHue1 - (fFactor * (1.0f - (fHue2 - fHue1)));
			// Did it wrap?
			if (fHue1 < 0.0f) {
				fHue1 += 1.0f;
			}
		}
	}
	m_fHue = fHue1;

	// Interpolate saturation
	m_fSaturation = Burger::interpolate(pInput1->m_fSaturation,pInput2->m_fSaturation,fFactor);

	// Interpolate luminosity
	m_fLuminance = Burger::interpolate(pInput1->m_fLuminance,pInput2->m_fLuminance,fFactor);
}


/*! ************************************

	\class Burger::Palette
	\brief Static functions to manage palettes

	This is a set of subroutines that will take a 256 color, RGB triplet palette
	and manage the colors within.

***************************************/

/*! ************************************

	\brief Convert 15 bit color to 24 bit color

	Given a 15 bit RGB pixel value, convert it to a
	24 bit RGB triplet

	\param pOutput Pointer to the output buffer.
	\param uInput 15 bit RGB value in 5:5:5 format.

	\sa FromRGB15(RGBAWord8_t *,uint_t), FromRGB16(RGBWord8_t *,uint_t), Renderer::RGB5ToRGB8Table

***************************************/

void BURGER_API Burger::Palette::FromRGB15(RGBWord8_t *pOutput,uint_t uInput)
{
	pOutput->m_uRed = Renderer::RGB5ToRGB8Table[(uInput>>10U)&0x1FU];	// Red
	pOutput->m_uGreen = Renderer::RGB5ToRGB8Table[(uInput>>5U)&0x1FU];	// Green
	pOutput->m_uBlue = Renderer::RGB5ToRGB8Table[uInput&0x1FU];			// Blue
}

/*! ************************************

	\brief Convert 15 bit color to 32 bit color

	Given a 15 bit RGB pixel value, convert it to a
	32 bit RGB quad. Alpha will be set to 255

	\param pOutput Pointer to the output buffer.
	\param uInput 15 bit RGB value in 5:5:5 format.

	\sa FromRGB16(RGBAWord8_t *,uint_t), FromRGB15(RGBWord8_t *,uint_t), Renderer::RGB5ToRGB8Table

***************************************/

void BURGER_API Burger::Palette::FromRGB15(RGBAWord8_t *pOutput,uint_t uInput)
{
	pOutput->m_uRed = Renderer::RGB5ToRGB8Table[(uInput>>10U)&0x1FU];	// Red
	pOutput->m_uGreen = Renderer::RGB5ToRGB8Table[(uInput>>5U)&0x1FU];	// Green
	pOutput->m_uBlue = Renderer::RGB5ToRGB8Table[uInput&0x1FU];			// Blue
	pOutput->m_uAlpha = 255;											// Alpha
}

/*! ************************************

	\brief Convert 1:5:5:5 bit color to 32 bit color

	Given a 1:5:5:5 bit RGB pixel value, convert it to a
	32 bit RGB quad. Alpha will be set to 0 or 255

	\param pOutput Pointer to the output buffer.
	\param uInput 16 bit RGB value in 1:5:5:5 format.

	\sa FromRGB16(RGBAWord8_t *,uint_t), FromRGB15(RGBWord8_t *,uint_t), Renderer::RGB5ToRGB8Table

***************************************/

void BURGER_API Burger::Palette::FromRGB1555(RGBAWord8_t *pOutput,uint_t uInput)
{
	pOutput->m_uRed = Renderer::RGB5ToRGB8Table[(uInput>>10U)&0x1FU];	// Red
	pOutput->m_uGreen = Renderer::RGB5ToRGB8Table[(uInput>>5U)&0x1FU];	// Green
	pOutput->m_uBlue = Renderer::RGB5ToRGB8Table[uInput&0x1FU];			// Blue
	pOutput->m_uAlpha = static_cast<uint8_t>((uInput&0x8000) ? 0xFFU : 0);	// Alpha
}

/*! ************************************

	\brief Convert 16 bit color to 24 bit color

	Given a 16 bit RGB pixel value, convert it to a
	24 bit RGB triplet

	\param pOutput Pointer to the output buffer.
	\param uInput 16 bit RGB value in 5:6:5 format.

	\sa FromRGB15(RGBWord8_t *,uint_t), FromRGB16(RGBAWord8_t *,uint_t), Renderer::RGB5ToRGB8Table, Renderer::RGB6ToRGB8Table

***************************************/

void BURGER_API Burger::Palette::FromRGB16(RGBWord8_t *pOutput,uint_t uInput)
{
	pOutput->m_uRed = Renderer::RGB5ToRGB8Table[(uInput>>11U)&0x1FU];	// Red
	pOutput->m_uGreen = Renderer::RGB6ToRGB8Table[(uInput>>5U)&0x3FU];	// Green
	pOutput->m_uBlue = Renderer::RGB5ToRGB8Table[uInput&0x1F];			// Blue
}

/*! ************************************

	\brief Convert 16 bit color to 32 bit color

	Given a 16 bit RGB pixel value, convert it to a
	32 bit RGB quad. Alpha will be set to 255

	\param pOutput Pointer to the output buffer.
	\param uInput 16 bit RGB value in 5:6:5 format.

	\sa FromRGB15(RGBAWord8_t *,uint_t), FromRGB16(RGBWord8_t *,uint_t), Renderer::RGB5ToRGB8Table, Renderer::RGB6ToRGB8Table

***************************************/

void BURGER_API Burger::Palette::FromRGB16(RGBAWord8_t *pOutput,uint_t uInput)
{
	pOutput->m_uRed = Renderer::RGB5ToRGB8Table[(uInput>>11U)&0x1FU];	// Red
	pOutput->m_uGreen = Renderer::RGB6ToRGB8Table[(uInput>>5U)&0x3FU];	// Green
	pOutput->m_uBlue = Renderer::RGB5ToRGB8Table[uInput&0x1F];			// Blue
	pOutput->m_uAlpha = 255;											// Alpha
}

/*! ************************************

	\brief Convert 24 bit color to 15 bit color

	Given a 24 bit RGB triplet stored as an array of bytes
	organized as Red, Green and Blue, convert it to a
	15 bit RGB pixel value

	\param pInput Pointer to a 24 bit RGB triplet
	\return 24 bit color value as 5:5:5 color
	\sa FromRGB15() or ToRGB16()

***************************************/

uint_t BURGER_API Burger::Palette::ToRGB15(const RGBWord8_t *pInput)
{
	uint_t uColor = (pInput->m_uRed<<7U)&0x7C00U;	// Red
	uColor += (pInput->m_uGreen<<2U)&0x03E0U;	// Green
	uColor += static_cast<uint_t>(pInput->m_uBlue>>3U);			// Blue
	return uColor;
}

/*! ************************************

	\fn uint_t Burger::Palette::ToRGB15(const RGBAWord8_t *pInput)
	\brief Convert 32 bit color to 15 bit color

	Given a 32 bit RGBA quad stored as an array of bytes
	organized as Red, Green, Blue and Alpha convert it to a
	15 bit RGB pixel value. Alpha is ignored

	\param pInput Pointer to a 32 bit RGBA quad
	\return 32 bit color value as 5:5:5 color
	\sa FromRGB15() or ToRGB16()

***************************************/

/*! ************************************

	\brief Convert 24 bit color to 16 bit color

	Given a 24 bit RGB triplet stored as an array of bytes
	organized as Red, Green and Blue, convert it to a
	16 bit RGB pixel value

	\param pInput Pointer to a 24 bit RGB triplet
	\return 24 bit color value as 5:6:5 color
	\sa FromRGB16() or ToRGB15()

***************************************/

uint_t BURGER_API Burger::Palette::ToRGB16(const RGBWord8_t *pInput)
{
	uint_t uColor = (pInput->m_uRed<<8U)&0xF800U;	// Red
	uColor += (pInput->m_uGreen<<3U)&0x07E0U;	// Green
	uColor += static_cast<uint_t>(pInput->m_uBlue>>3U);				// Blue
	return uColor;
}

/*! ************************************

	\fn uint_t Burger::Palette::ToRGB16(const RGBAWord8_t *pInput)
	\brief Convert 32 bit color to 16 bit color

	Given a 32 bit RGBA quad stored as an array of bytes
	organized as Red, Green, Blue and Alpha convert it to a
	16 bit RGB pixel value. Alpha is ignored

	\param pInput Pointer to a 32 bit RGBA quad
	\return 32 bit color value as 5:6:5 color
	\sa FromRGB16() or ToRGB15()

***************************************/

/*! ************************************

	\brief Convert 24 bit color to current depth

	Given a 24 bit RGB triplet, convert it to a
	pixel value of the current depth. For 32 bit
	destination pixels depths, the alpha is set to 255

	\note This function can only return an 8, 15, 16, 24 or 32 bit quantity.
		Any other depth will force a return of 0.

	\param pInput Pointer to a 24 bit RGB triplet
	\param pDisplay Display context to determine the depth and base palette

	\return 24 bit color value as 15, 16 or 8 bit color

	\sa ToRGB16(), ToRGB15(), FindColorIndex(), ToDisplay(uint_t,uint_t,uint_t,const Display *)

***************************************/

uint_t BURGER_API Burger::Palette::ToDisplay(const RGBWord8_t *pInput,const Display *pDisplay)
{
	uint_t uResult;
	switch (pDisplay->GetDepth()) {
	default:
		uResult = 0;
		break;
	case 8:
		uResult = FindColorIndex(pDisplay->GetPalette()+3,pInput->m_uRed,pInput->m_uGreen,pInput->m_uBlue,254)+1;
		break;
	case 15:
		uResult = (pInput->m_uRed<<7U)&0x7C00U;		// Red
		uResult += (pInput->m_uGreen<<2U)&0x03E0U;	// Green
		uResult += static_cast<uint_t>((pInput->m_uBlue)>>3U);			// Blue
		break;
	case 16:
		uResult = (pInput->m_uRed<<8U)&0xF800U;
		uResult += (pInput->m_uGreen<<3U)&0x7E0U;
		uResult += static_cast<uint_t>(pInput->m_uBlue>>3U);
		break;
	case 24:
		uResult = static_cast<uint_t>((pInput->m_uRed<<16U)+(pInput->m_uGreen<<8U)+pInput->m_uBlue);
		break;
	case 32:
		uResult = static_cast<uint_t>((pInput->m_uRed<<16U)+(pInput->m_uGreen<<8U)+pInput->m_uBlue)+0xFF000000U;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Convert 24 bit color to current depth

	Given a red, green and blue value, convert it to a
	pixel value of the current depth. For 32 bit destinations the
	alpha is set to 255

	\note This function can only return an 8, 15, 16, 24 or 32 bit quantity.
		Any other value will force a return of 0.
	
	\param uRed Value of red (0-255)
	\param uGreen Value of green (0-255)
	\param uBlue Value of blue (0-255)
	\param pDisplay Display context to determine the depth and base palette

	\return Color value as 8, 15, 16, 24 or 32 bit color

	\sa ToRGB16(), ToRGB15(), FindColorIndex(), ToDisplay(uint32_t,const Display *)

***************************************/

uint_t BURGER_API Burger::Palette::ToDisplay(uint_t uRed,uint_t uGreen,uint_t uBlue,const Display *pDisplay)
{
	uint_t uResult;
	switch(pDisplay->GetDepth()) {
	default:
		uResult = 0;
		break;
	case 8:
		uResult = FindColorIndex(pDisplay->GetPalette()+3,uRed,uGreen,uBlue,254)+1;
		break;
	case 15:
		uResult = (uRed<<7U)&0x7C00U;		// Red
		uResult += (uGreen<<2U)&0x03E0U;	// Green
		uResult += uBlue>>3U;				// Blue
		break;
	case 16:
		uResult = (uRed<<8U)&0xF800U;
		uResult += (uGreen<<3U)&0x7E0U;
		uResult += uBlue>>3U;
		break;
	case 24:
		uResult = (uRed<<16U)+(uGreen<<8U)+uBlue;
		break;
	case 32:
		uResult = (uRed<<16U)+(uGreen<<8U)+uBlue+0xFF000000U;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Convert 24 bit packed color to current depth

	Given a red/green/blue uint32_t value, convert it to a
	pixel value of the current depth.

	If the input value has an alpha component, it's ignored

	For 32 bit destinations the alpha is set to 255

	\note This function can only return an 8, 15, 16, 24 or 32 bit quantity.
		Any other value will force a return of 0.

	\param uRGBColor Value of red ((0-255)<<16) + Green ((0-255)<<8) + Blue (0-255)
	\param pDisplay Display context to determine the depth and base palette

	\return Color value as 8, 15, 16, 24 or 32 bit color

	\sa ToRGB16(), ToRGB15(), FindColorIndex(), ToDisplay(uint_t,uint_t,uint_t,const Display *)

***************************************/

uint_t BURGER_API Burger::Palette::ToDisplay(uint32_t uRGBColor,const Display *pDisplay)
{
	uint_t uRed = (uRGBColor>>16U)&0xFFU;		// Extract Red/Green/Blue
	uint_t uGreen = (uRGBColor>>8U)&0xFFU;
	uint_t uBlue = uRGBColor&0xFFU;
	uint_t uResult;
	switch (pDisplay->GetDepth()) {
	default:
		uResult = 0;
		break;
	case 8:
		uResult = FindColorIndex(pDisplay->GetPalette()+3,uRed,uGreen,uBlue,254)+1;
		break;
	case 15:
		uResult = (uRed<<7U)&0x7C00U;		// Red
		uResult += (uGreen<<2U)&0x03E0U;	// Green
		uResult += uBlue>>3U;				// Blue
		break;	
	case 16:
		uResult = (uRed<<8U)&0xF800U;
		uResult += (uGreen<<3U)&0x7E0U;
		uResult += uBlue>>3U;
		break;
	case 24:
		uResult = uRGBColor&0xFFFFFFU;
		break;
	case 32:
		uResult = uRGBColor|0xFF000000U;
		break;
	}
	return uResult;
}


/*! ************************************

	\brief Lookup a color in an 8 bit palette

	Given an 8 bit red, green and blue value,
	return the pixel value that is the most closest match.

	The palette is an array of bytes that is sizeof(uint8_t)*uCount*3

	\param pPalette Pointer to an 8 bit palette to map to (768 bytes)
	\param uRed Red value (0-255)
	\param uGreen Green value (0-255)
	\param uBlue Blue value (0-255)
	\param uCount Number of colors in the palette (The array byte size is this value multiplied by 3)
	\return Color index that is the closest match. (0-(Count-1))
	\sa ToDisplay(uint32_t,const Display *) or ToDisplay(uint_t,uint_t,uint_t,const Display *)

***************************************/

uint_t BURGER_API Burger::Palette::FindColorIndex(const uint8_t *pPalette,uint_t uRed,uint_t uGreen,uint_t uBlue,uint_t uCount)
{
	uint_t uClosestIndex = 0;					// Index found
	if (uCount) {
		uint32_t uClosestDist = BURGER_MAXUINT;	// Use a bogus number
		uRed=uRed+255;						// Adjust so that the negative index is positive
		uGreen=uGreen+255;
		uBlue=uBlue+255;
		uint_t i = 0;
		do {
			uint_t uTemp = pPalette[0];		// Get the difference
			uTemp = uRed-uTemp;
			uint_t uNewDelta = Renderer::ByteSquareTable[uTemp];	// Get the square
			uTemp = pPalette[1];
			uTemp = uGreen-uTemp;
			uTemp = Renderer::ByteSquareTable[uTemp];
			uNewDelta = uNewDelta+uTemp;
			uTemp = pPalette[2];
			uTemp = uBlue-uTemp;
			uTemp = Renderer::ByteSquareTable[uTemp];
			uNewDelta = uNewDelta+uTemp;
			if (uNewDelta < uClosestDist) {	// Closer?
				uClosestIndex = i;			// This color is the closest match
				if (!uNewDelta) {			// Perfect match?
					break;
				}
				uClosestDist = uNewDelta;		// Save the distance
			}
			pPalette = pPalette+3;			// Next index
		} while (++i<uCount);				// All colors checked?
	}
	return uClosestIndex;					// Return the lookup
}

/*! ************************************

	\brief Convert an 8 bit palette to a true color array

	Given an 8 bit palette (256 RGB values)
	create a color lookup table for drawing to a screen
	using this palette

	\param pOutput Pointer to an array of 256 Words
	\param pInput Pointer to an array of 256 3 byte RGB triplets
	\param pDisplay Display context to determine the depth and base palette
	\sa Make8BitLookupTable(uint_t *,RezFile *,uint_t,const Display *), ToDisplay(const RGBWord8_t *,const Display *)

***************************************/

void BURGER_API Burger::Palette::Make8BitLookupTable(uint_t *pOutput,const uint8_t *pInput,const Display *pDisplay)
{
	uint_t i = 256;
	do {
		pOutput[0] = ToDisplay(reinterpret_cast<const RGBWord8_t *>(pInput),pDisplay);	// Convert RGB to native
		++pOutput;	// Next output pointer
		pInput+=3;	// Next triplet
	} while (--i);	// All done?
}

/*! ************************************

	\brief Convert an 8 bit palette to a true color array

	Given an 8 bit palette (256 RGB values)
	create a 16 bit color lookup table for drawing to a screen
	using this palette. The palette is read in from a resource
	file. The resource is released on exit.

	\param pOutput Pointer to an array of 256 Words
	\param pInput Pointer to the reference of the resource file
	\param uRezNum Which resource ID to load
	\param pDisplay Display context to determine the depth and base palette
	\sa Make8BitLookupTable(uint_t *,const uint8_t *,const Display *), ToDisplay(const RGBWord8_t *,const Display *)

***************************************/

void BURGER_API Burger::Palette::Make8BitLookupTable(uint_t *pOutput,Burger::RezFile *pInput,uint_t uRezNum,const Display *pDisplay)
{
	const uint8_t *pPalette = static_cast<const uint8_t *>(pInput->Load(uRezNum));	// Load the resource
	if (pPalette) {										// Ok?
		Make8BitLookupTable(pOutput,pPalette,pDisplay);	// Create the table
		pInput->Release(uRezNum);						// Release the resource
	}
}

/*! ************************************

	\brief Create an 8 bit remap table

	Create a pixel data conversion table by taking
	the a source palette and trying to find the best match
	in a destination palette.
	The returned data will allow you to quickly color map a 256 color
	image onto any palette.

	\param pOutput Pointer to an array of 256 bytes
	\param pNewPalette Pointer to a palette of 256 colors (768 bytes) to map to
	\param pOldPalette Pointer to the palette of the shape (768 bytes) to map from
	\sa FindColorIndex(), MakeRemapLookupMasked()

***************************************/

void BURGER_API Burger::Palette::MakeRemapLookup(uint8_t *pOutput,const uint8_t *pNewPalette,const uint8_t *pOldPalette)
{
	pNewPalette+=3;
	uint_t i = 256;
	do {
		pOutput[0] = static_cast<uint8_t>(FindColorIndex(pNewPalette,pOldPalette[0],pOldPalette[1],pOldPalette[2],254)+1);
		++pOutput;
		pOldPalette += 3;
	} while (--i);		// All 256 colors done?
}

/*! ************************************

	\brief Create an 8 bit remap table

	Create a pixel data conversion table by taking
	the a source palette and trying to find the best match
	in a destination palette.
	The returned data will allow you to quickly color map a 256 color
	image onto any palette.

	\note This will not alter color #0 and will not map to either color #0
	or color #255. Color #255 will be remapped to an equivalent color.

	\param pOutput Pointer to an array of 256 bytes
	\param pNewPalette Pointer to a palette of 256 colors (768 bytes) to map to
	\param pOldPalette Pointer to the palette of the shape (768 bytes) to map from
	\sa FindColorIndex(), MakeRemapLookup()

***************************************/

void BURGER_API Burger::Palette::MakeRemapLookupMasked(uint8_t *pOutput,const uint8_t *pNewPalette,const uint8_t *pOldPalette)
{
	pNewPalette+=3;
	pOldPalette+=3;		// Skip the first color
	uint_t i = 255;
	pOutput[0] = 0;
	++pOutput;
	do {
		pOutput[0] = static_cast<uint8_t>(FindColorIndex(pNewPalette,pOldPalette[0],pOldPalette[1],pOldPalette[2],254)+1);
		++pOutput;
		pOldPalette += 3;
	} while (--i);		// All 256 colors done?
}

/*! ************************************

	\brief Create an 8 bit mask table

	Create a pixel mask table to make the MaskColor index
	transparent. This table will have the first 256 bytes containing
	the value to 'OR' with. The second 256 bytes will be the mask value.

	What effectively occurs is that the first 256 bytes are 0-255 in
	that order and the last 256 bytes are 0. The mask color will have a
	mask of 255 and a color of zero. This is to create a table
	to draw masked shapes without having to incur branch penalties
	on processors that have problems with mispredicted branches.

	\param pOutput Pointer to an array of 512 bytes
	\param uMaskIndex Color index to mask as invisible (0-255) or (-1) for not used
	\sa MakeRemapLookup()

***************************************/

void BURGER_API Burger::Palette::MakeColorMasks(uint8_t *pOutput,uint_t uMaskIndex)
{
	uint_t i = 0;
	uint8_t *pMask = pOutput+256;
	do {
		pOutput[i] = static_cast<uint8_t>(i);		// Save the or mask
		pMask[i] = 0;							// Save the and mask
	} while (++i<256);
	if (uMaskIndex<256) {						// Used?
		pMask[uMaskIndex] = 0xFFU;				// Set the and mask for invisible
		pOutput[uMaskIndex] = 0;				// Set the or mask for invisible
	}
}

/*! ************************************

	\brief Create a remap table to brighten or darken a shape

	If you wish to remap an 8 bit image by altering the brightness
	of one or more colors, this routine will create a remap table
	based on the new brightness.
	Give the palette for the shape and pass the red, green and blue
	scaling adjustments in percentage points.
	100 is treated as 1.0, 50 is .5 and 200 is 2.0 intensity

	\param pOutput Pointer to an array of 256 bytes for remap table
	\param pInput Pointer to an 8 bit palette (768 bytes)
	\param uRedAdjust Red percentage adjust (0-1000)
	\param uGreenAdjust Green percentage adjust (0-1000)
	\param uBlueAdjust Blue percentage adjust (0-1000)
	\sa MakeRemapLookup()

***************************************/

void BURGER_API Burger::Palette::MakeFadeLookup(uint8_t *pOutput,const uint8_t *pInput,uint_t uRedAdjust,uint_t uGreenAdjust,uint_t uBlueAdjust)
{
	uint8_t PaletteBuffer[768];		// Local palette

	uint8_t *pWork = PaletteBuffer;	// Create the new table using a temp palette
	const uint_t RANGEBITS = 6;		// 6 bits of fraction
	uRedAdjust = uRedAdjust<<RANGEBITS;			// Scale up for fraction
	uGreenAdjust = uGreenAdjust<<RANGEBITS;
	uBlueAdjust = uBlueAdjust<<RANGEBITS;
	const uint_t PERCENT = 100;		// 1.0 in percentages
	uRedAdjust = uRedAdjust/PERCENT;			// Create a fixed point number
	uGreenAdjust = uGreenAdjust/PERCENT;
	uBlueAdjust = uBlueAdjust/PERCENT;
	uint_t i = 0;						// All 256 colors
	do {
		uint_t uTemp = pInput[0];
		uTemp = uTemp * uRedAdjust;		// Perform the RGB scale
		uTemp = uTemp >> RANGEBITS;	// Isolate the integer
		if (uTemp>=256) {			// Too high?
			uTemp = 255;
		}
		pWork[0] = static_cast<uint8_t>(uTemp);

		uTemp = pInput[1];
		uTemp = uTemp * uGreenAdjust;
		uTemp = uTemp >> RANGEBITS;	// Perform the RGB scale
		if (uTemp>=256) {
			uTemp = 255;
		}
		pWork[1] = static_cast<uint8_t>(uTemp);

		uTemp = pInput[2];
		uTemp = uTemp * uBlueAdjust;
		uTemp = uTemp >> RANGEBITS;	// Perform the RGB scale
		if (uTemp>=256) {
			uTemp = 255;
		}
		pWork[2] = static_cast<uint8_t>(uTemp);
		pInput=pInput+3;
		pWork=pWork+3;
	} while (++i<256);
	pInput = pInput-768;							// Restore the input pointer
	MakeRemapLookup(pOutput,pInput,PaletteBuffer);	// Perform the actual work
}


/*! ************************************

	\fn void Burger::CopyPalette(RGBWord8_t *pOutput,const RGBWord8_t *pInput,uintptr_t uEntries)
	\brief Copy an array of RGBWord8_t entries

	Given a pointer to an array of RGBWord8_t color entries, copy the colors into
	another array of RGBWord8_t entries

	\param pOutput Pointer to the palette to copy to
	\param pInput Pointer to the palette to copy from
	\param uEntries Number of color entries in the palette
	\sa CopyPalette(RGBWord8_t *,const RGBAWord8_t *,uintptr_t)

***************************************/

/*! ************************************

	\brief Copy an array of RGBWord8_t entries from an array of RGBAWord8_t entries

	Given a pointer to an array of RGBAWord8_t color entries, copy the colors into
	another array of RGBWord8_t entries while losing the alpha value

	\param pOutput Pointer to the palette to copy to
	\param pInput Pointer to the palette to copy from
	\param uEntries Number of color entries in the palette
	\sa CopyPalette(RGBWord8_t *,const RGBWord8_t *,uintptr_t)

***************************************/

void BURGER_API Burger::CopyPalette(RGBWord8_t *pOutput,const RGBAWord8_t *pInput,uintptr_t uEntries)
{
	if (uEntries) {
		do {
			pOutput->m_uRed = pInput->m_uRed;
			pOutput->m_uGreen = pInput->m_uGreen;
			pOutput->m_uBlue = pInput->m_uBlue;
			++pInput;
			++pOutput;
		} while (--uEntries);
	}
}

/*! ************************************

	\brief Copy an array of RGBAWord8_t entries from an array of RGBWord8_t entries

	Given a pointer to an array of RGBWord8_t color entries, copy the colors into
	another array of RGBAWord8_t entries and set the alpha value to zero.

	\param pOutput Pointer to the palette to copy to
	\param pInput Pointer to the palette to copy from
	\param uEntries Number of color entries in the palette
	\sa CopyPalette(RGBAWord8_t *,const RGBAWord8_t *,uintptr_t)

***************************************/

void BURGER_API Burger::CopyPalette(RGBAWord8_t *pOutput,const RGBWord8_t *pInput,uintptr_t uEntries)
{
	if (uEntries) {
		do {
			pOutput->m_uRed = pInput->m_uRed;
			pOutput->m_uGreen = pInput->m_uGreen;
			pOutput->m_uBlue = pInput->m_uBlue;
			pOutput->m_uAlpha = 0;
			++pInput;
			++pOutput;
		} while (--uEntries);
	}
}

/*! ************************************

	\fn void Burger::CopyPalette(RGBAWord8_t *pOutput,const RGBAWord8_t *pInput,uintptr_t uEntries)
	\brief Copy an array of RGBAWord8_t entries from an array of RGBAWord8_t entries

	Given a pointer to an array of RGBAWord8_t color entries, copy the colors into
	another array of RGBAWord8_t entries

	\param pOutput Pointer to the palette to copy to
	\param pInput Pointer to the palette to copy from
	\param uEntries Number of color entries in the palette
	\sa CopyPalette(RGBAWord8_t *,const RGBWord8_t *,uintptr_t)

***************************************/

/*! ************************************

	\brief Copy an array of RGBWord8_t entries from an array of 16 bit XRGB entries

	Given a pointer to an array of 16 bit XRGB color entries, copy the colors into
	another array of RGBWord8_t entries.

	Each color is 4 bits, however it's expanded to 8 bits in the output palette

	Red = (Color>>8)&0xF
	Green = (Color>>4)&0xF
	Blue = Color&0xF

	\param pOutput Pointer to the palette to copy to
	\param pInput Pointer to the palette to copy from
	\param uEntries Number of color entries in the palette
	\sa CopyPalette444(RGBAWord8_t *,const uint16_t *,uintptr_t) or Renderer::RGB4ToRGB8Table

***************************************/

void BURGER_API Burger::CopyPalette444(RGBWord8_t *pOutput,const uint16_t *pInput,uintptr_t uEntries)
{
	if (uEntries) {
		do {
			uint_t uColor = pInput[0];
			pOutput->m_uRed = Renderer::RGB4ToRGB8Table[(uColor>>8U)&0xFU];
			pOutput->m_uGreen = Renderer::RGB4ToRGB8Table[(uColor>>4U)&0xFU];
			pOutput->m_uBlue = Renderer::RGB4ToRGB8Table[uColor&0xFU];
			++pOutput;
			++pInput;
		} while (--uEntries);
	}
}

/*! ************************************

	\brief Copy an array of RGBAWord8_t entries from an array of 16 bit XRGB entries

	Given a pointer to an array of 16 bit XRGB color entries, copy the colors into
	another array of RGBAWord8_t entries. Set Alpha to 255

	Each color is 4 bits, however it's expanded to 8 bits in the output palette

	Red = (Color>>8)&0xF
	Green = (Color>>4)&0xF
	Blue = Color&0xF

	\param pOutput Pointer to the palette to copy to
	\param pInput Pointer to the palette to copy from
	\param uEntries Number of color entries in the palette
	\sa CopyPalette444(RGBWord8_t *,const uint16_t *,uintptr_t) or Renderer::RGB4ToRGB8Table

***************************************/

void BURGER_API Burger::CopyPalette444(RGBAWord8_t *pOutput,const uint16_t *pInput,uintptr_t uEntries)
{
	if (uEntries) {
		do {
			uint_t uColor = pInput[0];
			pOutput->m_uRed = Renderer::RGB4ToRGB8Table[(uColor>>8U)&0xFU];
			pOutput->m_uGreen = Renderer::RGB4ToRGB8Table[(uColor>>4U)&0xFU];
			pOutput->m_uBlue = Renderer::RGB4ToRGB8Table[uColor&0xFU];
			// Force alpha to maximum
			pOutput->m_uAlpha = 0xFF;
			++pOutput;
			++pInput;
		} while (--uEntries);
	}
}

/*! ************************************

	\brief Copy an array of RGBWord8_t entries from an array of 16 bit ARGB entries

	Given a pointer to an array of 16 bit ARGB color entries, copy the colors into
	another array of RGBWord8_t entries and discard the alpha

	Each color is 4 bits, however it's expanded to 8 bits in the output palette

	Alpha = (Color>>12)&0xF
	Red = (Color>>8)&0xF
	Green = (Color>>4)&0xF
	Blue = Color&0xF

	\note Alpha is discarded in this function

	\param pOutput Pointer to the palette to copy to
	\param pInput Pointer to the palette to copy from
	\param uEntries Number of color entries in the palette
	\sa CopyPalette4444(RGBAWord8_t *,const uint16_t *,uintptr_t) or Renderer::RGB4ToRGB8Table

***************************************/

void BURGER_API Burger::CopyPalette4444(RGBWord8_t *pOutput,const uint16_t *pInput,uintptr_t uEntries)
{
	if (uEntries) {
		do {
			uint_t uColor = pInput[0];
			pOutput->m_uRed = Renderer::RGB4ToRGB8Table[(uColor>>8U)&0xFU];
			pOutput->m_uGreen = Renderer::RGB4ToRGB8Table[(uColor>>4U)&0xFU];
			pOutput->m_uBlue = Renderer::RGB4ToRGB8Table[uColor&0xFU];
			++pOutput;
			++pInput;
		} while (--uEntries);
	}
}

/*! ************************************

	\brief Copy an array of RGBAWord8_t entries from an array of 16 bit ARGB entries

	Given a pointer to an array of 16 bit ARGB color entries, copy the colors into
	another array of RGBAWord8_t entries.

	Each color is 4 bits, however it's expanded to 8 bits in the output palette

	Alpha = (Color>>12)&0xF
	Red = (Color>>8)&0xF
	Green = (Color>>4)&0xF
	Blue = Color&0xF

	\param pOutput Pointer to the palette to copy to
	\param pInput Pointer to the palette to copy from
	\param uEntries Number of color entries in the palette
	\sa CopyPalette4444(RGBWord8_t *,const uint16_t *,uintptr_t) or Renderer::RGB4ToRGB8Table

***************************************/

void BURGER_API Burger::CopyPalette4444(RGBAWord8_t *pOutput,const uint16_t *pInput,uintptr_t uEntries)
{
	if (uEntries) {
		do {
			uint_t uColor = pInput[0];
			pOutput->m_uRed = Renderer::RGB4ToRGB8Table[(uColor>>8U)&0xFU];
			pOutput->m_uGreen = Renderer::RGB4ToRGB8Table[(uColor>>4U)&0xFU];
			pOutput->m_uBlue = Renderer::RGB4ToRGB8Table[uColor&0xFU];
			pOutput->m_uAlpha = Renderer::RGB4ToRGB8Table[(uColor>>12U)&0xFU];
			++pOutput;
			++pInput;
		} while (--uEntries);
	}
}



/*! ************************************

	\brief Convert an RGB color to HSL color

	Perform a conversion of RGB color space to Hue, Saturation and
	Intensity color space

	\param pOutput Pointer to the structure to get the converted HSL color
	\param pInput Pointer to an RGB color in the range for 0.0f to 1.0f per element
	\sa Convert(RGBFloat_t *,const HSL_t*)

***************************************/

void BURGER_API Burger::Convert(HSL_t *pOutput,const RGBFloat_t *pInput)
{

	float fRed = pInput->m_fRed;			// Load into registers
	float fGreen = pInput->m_fGreen;
	float fBlue = pInput->m_fBlue;

	// Which color is the brightest?
	// It will also pick the luminance
	// Note the 6 patterns
	
	float fHue;
	float fSaturation;
	float fLuminance;
	if (fRed >= fGreen) {
		fLuminance = fRed;					// Use red

		if (fBlue > fRed) {					// B>R>G
			fLuminance = fBlue;				// Use blue
			fSaturation = 1.0f - fGreen;	// Green is saturation
			fHue = (fRed * (1.0f / 6.0f)) + 0.6666666667f;
			
		} else if (fBlue > fGreen) {		// R>B>G
		
			fSaturation = 1.0f - fGreen;	// Green is saturation
			fHue = ((1.0f - fBlue) * (1.0f/ 6.0f)) + 0.8333333333f;
		
		} else {							// R>G>B
			fSaturation = 1.0f - fBlue;		// Blue is saturation
			fHue = fGreen * (1.0f / 6.0f);
		}
	} else {
		fLuminance = fGreen;				// Use green
		if (fBlue > fGreen) {				// B>G>R
			fLuminance = fBlue;				// Use blue
			fSaturation = 1.0f - fRed;		// Red is saturation
			fHue = (fBlue * (1.0f / 6.0f)) + 0.3333333333f;

		} else if (fBlue > fRed) {			// G>B>R
			fSaturation = 1.0f - fRed;		// Red is saturation
			fHue = ((1.0f - fGreen) * (1.0f / 6.0f)) + 0.5f;
		} else {							// G>R>B */
			fSaturation = 1.0f - fBlue;		// Blue is saturation
			fHue = ((1.0f - fRed) * (1.0f / 6.0f)) + 0.16666667f;
		}
	}

	// Save the result

	pOutput->m_fHue = fHue;
	pOutput->m_fSaturation = fSaturation;
	pOutput->m_fLuminance = fLuminance;
}

/*! ************************************

	\brief Convert a HSL color to an RGB color

	Perform a conversion of Hue, Saturation and
	Intensity color space to Red, Green and
	Blue color space

	\param pOutput Pointer to the structure to get the converted RGB color
	\param pInput Pointer to a HSL color in the range for 0.0f to 1.0f per element
	\sa Convert(HSL_t *,const RGBFloat_t*)

***************************************/

void BURGER_API Burger::Convert(RGBFloat_t *pOutput,const HSL_t *pInput)
{
	float fHue = pInput->m_fHue;
	float fSaturation = pInput->m_fSaturation;
	float fLuminance = pInput->m_fLuminance;

	// There are 6 hue parts
	
	float fRed;
	float fGreen;
	float fBlue;
	if (fHue < 0.5f) {					// First half of the color wheel
		if (fHue < 0.166667f) { 
			fRed = 1.0f;				// R = 1, B = 0, G = H
			fGreen = fHue * 6.0f;		// Green section
			fBlue = 0.0f;
		} else {
			fGreen = 1.0f;
			if (fHue < 0.33333333f) {	// R = -H, G = 1, B = 0
				fRed = 1.0f - ((fHue - 0.166667f) * 6.0f);
				fBlue = 0.0f;
			} else { 					// R = 0, G = 1, B = H
				fRed = 0.0f;
				fBlue = (fHue - 0.3333333f) * 6.0f;
			}
		}
	} else {							// Second half of the color wheel
		if (fHue >= 0.833333f) {		// R = 1, G = 0, B = -H
			fRed = 1.0f;
			fGreen = 0.0f;
			fBlue = 1.0f - ((fHue - 0.833333f) * 6.0f);
		} else {
			fBlue = 1.0f;
			if (fHue < 0.666667f) {		// R = 0, G = -H, B = 1
				fRed = 0.0f;
				fGreen = 1.0f - ((fHue - 0.5f) * 6.0f);
			} else {					// R = H, G = 0, B = 1
				fRed = (fHue - 0.666667f) * 6.0f;
				fGreen = 0.0f;
			}
		}
	}

	// Apply saturation
	
	fRed = 1.0f - (fSaturation * (1.0f - fRed));
	fGreen = 1.0f - (fSaturation * (1.0f - fGreen));
	fBlue = 1.0f - (fSaturation * (1.0f - fBlue));

	// Apply luminosity and save the result

	pOutput->m_fRed = fRed * fLuminance;
	pOutput->m_fGreen = fGreen * fLuminance;
	pOutput->m_fBlue = fBlue * fLuminance;
}

/*! ************************************

	\brief Set a range of RGB color entries to a 256 entry palette

	Given a pointer to a 256 color palette, copy the color 
	entries passed in the input array and store them in the
	output array. Since the output array is assumed to be 256
	entries, the starting index must be less than 256 and
	the number of entries copied will be clamped to
	prevent a buffer overrun.

	Any entries that would result in a buffer overrun will be ignored and
	if the starting index is greater than 255, this function will do nothing.

	\param pOutput Pointer to the 256 color palette to store the new colors
	\param pInput Pointer to the color entries to copy from
	\param uStartIndex Color index of the 256 color output palette to start modification (Maximum 255)
	\param uPaletteSize Number of color entries to copy (Maximum 256)
	\sa CopyPalette256(RGBWord8_t *,const RGBAWord8_t *,uint_t,uint_t)

***************************************/

void BURGER_API Burger::CopyPalette256(RGBWord8_t *pOutput,const RGBWord8_t *pInput,uint_t uStartIndex,uint_t uPaletteSize)
{
	// Invalid start index?
	if (uStartIndex<256) {
		// Is the size in range?
		if ((uPaletteSize+uStartIndex)>256U) {
			// Set the size to the maximum (Clamp)
			uPaletteSize = 256U-uStartIndex;
		}
		CopyPalette(pOutput+uStartIndex,pInput,uPaletteSize);
	}
}

/*! ************************************

	\brief Set a range of RGB color entries to a 256 entry palette

	Given a pointer to a 256 color palette, copy the color 
	entries passed in the input array and store them in the
	output array. Since the output array is assumed to be 256
	entries, the starting index must be less than 256 and
	the number of entries copied will be clamped to
	prevent a buffer overrun.

	Any entries that would result in a buffer overrun will be ignored and
	if the starting index is greater than 255, this function will do nothing.

	\note The alpha in the input palette will be ignored.

	\param pOutput Pointer to the 256 color palette to store the new colors
	\param pInput Pointer to the color entries to copy from
	\param uStartIndex Color index of the 256 color output palette to start modification (Maximum 255)
	\param uPaletteSize Number of color entries to copy (Maximum 256)
	\sa CopyPalette256(RGBWord8_t *,const RGBWord8_t *,uint_t,uint_t)

***************************************/

void BURGER_API Burger::CopyPalette256(RGBWord8_t *pOutput,const RGBAWord8_t *pInput,uint_t uStartIndex,uint_t uPaletteSize)
{
	// Invalid start index?
	if (uStartIndex<256) {
		// Is the size in range?
		if ((uPaletteSize+uStartIndex)>256U) {
			// Set the size to the maximum (Clamp)
			uPaletteSize = 256U-uStartIndex;
		}
		CopyPalette(pOutput+uStartIndex,pInput,uPaletteSize);
	}
}

/*! ************************************

	\brief Set a range of RGBA color entries to a 256 entry palette

	Given a pointer to a 256 color palette, copy the color 
	entries passed in the input array and store them in the
	output array. Since the output array is assumed to be 256
	entries, the starting index must be less than 256 and
	the number of entries copied will be clamped to
	prevent a buffer overrun.

	Any entries that would result in a buffer overrun will be ignored and
	if the starting index is greater than 255, this function will do nothing.

	\note The Alpha of each copied entry will be set to a default of 255 (Maximum / opaque)

	\param pOutput Pointer to the 256 color palette to store the new colors
	\param pInput Pointer to the color entries to copy from
	\param uStartIndex Color index of the 256 color output palette to start modification (Maximum 255)
	\param uPaletteSize Number of color entries to copy (Maximum 256)
	\sa CopyPalette256(RGBAWord8_t *,const RGBAWord8_t *,uint_t,uint_t)

***************************************/

void BURGER_API Burger::CopyPalette256(RGBAWord8_t *pOutput,const RGBWord8_t *pInput,uint_t uStartIndex,uint_t uPaletteSize)
{
	// Invalid start index?
	if (uStartIndex<256) {
		// Is the size in range?
		if ((uPaletteSize+uStartIndex)>256U) {
			// Set the size to the maximum (Clamp)
			uPaletteSize = 256U-uStartIndex;
		}
		CopyPalette(pOutput+uStartIndex,pInput,uPaletteSize);
	}
}

/*! ************************************

	\brief Set a range of RGBA color entries to a 256 entry palette

	Given a pointer to a 256 color palette, copy the color 
	entries passed in the input array and store them in the
	output array. Since the output array is assumed to be 256
	entries, the starting index must be less than 256 and
	the number of entries copied will be clamped to
	prevent a buffer overrun.

	Any entries that would result in a buffer overrun will be ignored and
	if the starting index is greater than 255, this function will do nothing.

	\param pOutput Pointer to the 256 color palette to store the new colors
	\param pInput Pointer to the color entries to copy from
	\param uStartIndex Color index of the 256 color output palette to start modification (Maximum 255)
	\param uPaletteSize Number of color entries to copy (Maximum 256)
	\sa CopyPalette256(RGBAWord8_t *,const RGBWord8_t *,uint_t,uint_t)

***************************************/

void BURGER_API Burger::CopyPalette256(RGBAWord8_t *pOutput,const RGBAWord8_t *pInput,uint_t uStartIndex,uint_t uPaletteSize)
{
	// Invalid start index?
	if (uStartIndex<256) {
		// Is the size in range?
		if ((uPaletteSize+uStartIndex)>256U) {
			// Set the size to the maximum (Clamp)
			uPaletteSize = 256U-uStartIndex;
		}
		CopyPalette(pOutput+uStartIndex,pInput,uPaletteSize);
	}
}

/*! ************************************

	\brief Clear an RGB palette

	Set all values to zero

	\param pOutput Pointer to the palette to set to Black
	\param uPaletteSize Number of color entries to clear
	\sa ClearPalette(RGBAWord8_t *,uintptr_t)

***************************************/

void BURGER_API Burger::ClearPalette(RGBWord8_t *pOutput,uintptr_t uPaletteSize)
{
	// Just set everything to zero
	MemoryClear(pOutput,sizeof(pOutput[0])*uPaletteSize);
}

/*! ************************************

	\brief Clear an RGBA palette

	Set the red, green and blue values to zero and the alpha to 255

	\param pOutput Pointer to the palette to set to Black
	\param uPaletteSize Number of color entries to clear
	\sa ClearPalette(RGBWord8_t *,uintptr_t)

***************************************/

void BURGER_API Burger::ClearPalette(RGBAWord8_t *pOutput,uintptr_t uPaletteSize)
{
	if (uPaletteSize) {
		do {
			pOutput->m_uRed = 0;		// Clear out the palette
			pOutput->m_uGreen = 0;
			pOutput->m_uBlue = 0;
			pOutput->m_uAlpha = 255;	// Set alpha to opaque
			++pOutput;
		} while (--uPaletteSize);
	}
}

/*! ************************************

	\brief Fill an RGB palette with a specific color

	Set all values to a specific color

	\param pOutput Pointer to the palette to fill
	\param pInput Pointer to the color to fill with
	\param uPaletteSize Number of color entries to set
	\sa FillPalette(RGBAWord8_t *,const RGBAWord8_t *,uintptr_t)

***************************************/

void BURGER_API Burger::FillPalette(RGBWord8_t *pOutput,const RGBWord8_t *pInput,uintptr_t uPaletteSize)
{
	if (uPaletteSize) {
		uint8_t uRed = pInput->m_uRed;
		uint8_t uGreen = pInput->m_uGreen;
		uint8_t uBlue = pInput->m_uBlue;
		do {
			pOutput->m_uRed = uRed;		// Set the color
			pOutput->m_uGreen = uGreen;
			pOutput->m_uBlue = uBlue;
			++pOutput;
		} while (--uPaletteSize);
	}
}

/*! ************************************

	\brief Fill an RGBA palette with a specific color

	Set all values to a specific color

	\param pOutput Pointer to the palette to fill
	\param pInput Pointer to the color to fill with
	\param uPaletteSize Number of color entries to set
	\sa FillPalette(RGBWord8_t *,const RGBWord8_t *,uintptr_t)

***************************************/

void BURGER_API Burger::FillPalette(RGBAWord8_t *pOutput,const RGBAWord8_t *pInput,uintptr_t uPaletteSize)
{
	if (uPaletteSize) {
		uint8_t uRed = pInput->m_uRed;
		uint8_t uGreen = pInput->m_uGreen;
		uint8_t uBlue = pInput->m_uBlue;
		uint8_t uAlpha = pInput->m_uAlpha;
		do {
			pOutput->m_uRed = uRed;		// Set the color
			pOutput->m_uGreen = uGreen;
			pOutput->m_uBlue = uBlue;
			pOutput->m_uAlpha = uAlpha;
			++pOutput;
		} while (--uPaletteSize);
	}
}
