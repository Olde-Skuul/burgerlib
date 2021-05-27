/***************************************

    Flash player data stream reader

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brflashrgba.h"
#include "brdebug.h"

/*! ************************************

	\class Burger::Flash::RGBAWord8
	\brief RGBA color class for Flash

	Converts the Burgerlib RGBAWord8_t structure into a class
	that has constructors. Will default to white (255,255,255,255).

	\sa RGBAWord8_t

***************************************/

/*! ************************************

	\fn Burger::Flash::RGBAWord8::RGBAWord8()
	\brief Default constructor

	Sets the color to white (255,255,255,255).

	\sa RGBAWord8(uint8_t,uint8_t,uint8_t,uint8_t) or RGBAWord8(double)

***************************************/

/*! ************************************

	\fn Burger::Flash::RGBAWord8::RGBAWord8(uint8_t bRed,uint8_t bGreen,uint8_t bBlue,uint8_t bAlpha)
	\brief Default constructor

	Sets the color a specific color

	\param bRed Default red component
	\param bGreen Default green component
	\param bBlue Default blue component
	\param bAlpha Default alpha component

	\sa RGBAWord8() or RGBAWord8(double)

***************************************/

/*! ************************************

	\fn Burger::Flash::RGBAWord8::RGBAWord8(double dColor)
	\brief Default constructor

	Sets the color a specific color where Red is dColor/65536, Green
	is dColor/256 and Blue is dColor and all values are clamped to 8 bit.

	\param dColor Encoded color number
	\sa Set(double), RGBAWord8(uint8_t,uint8_t,uint8_t,uint8_t) or RGBAWord8()

***************************************/

/*! ************************************

	\brief Read an RGB(A) color from an input stream based on a shape type

	Given a shape type of type 1, 2 or 3, read in the RGB color
	into the record. Type 1 and 2 reads in R,G, and B and sets the alpha
	to 255 where type 3 reads in all four components.

	The shape type is documented on page 132 of swf-file-format-spec.pdf

	\param pStream Pointer to the data stream
	\param uTagType Shape type.
	\sa ReadRGBA(Stream*) or ReadRGB(Stream*)

***************************************/

void BURGER_API Burger::Flash::RGBAWord8::Read(Stream* pStream,uint_t uTagType)
{
	if (uTagType <= 22) {		// Shape 1 (2) or Shape 2 (22) is RGB
		ReadRGB(pStream);
	} else {
		ReadRGBA(pStream);		// Shape 3 (32) is RGBA
	}
}

/*! ************************************

	\brief Read an RGBA color from an input stream

	Read in four bytes, Red, Green, Blue and Alpha from the
	input stream.

	\param pStream Pointer to the data stream
	\sa Read(Stream*,uint_t) or ReadRGB(Stream*)

***************************************/

void BURGER_API Burger::Flash::RGBAWord8::ReadRGBA(Stream* pStream)
{
	m_uRed = pStream->GetByte();
	m_uGreen = pStream->GetByte();
	m_uBlue = pStream->GetByte();
	m_uAlpha = pStream->GetByte();
}

/*! ************************************

	\brief Read an RGB color from an input stream

	Read in three bytes, Red, Green, and Blue from the
	input stream. Set Alpha to 255

	\param pStream Pointer to the data stream
	\sa Read(Stream*,uint_t) or ReadRGBA(Stream*)

***************************************/

void BURGER_API Burger::Flash::RGBAWord8::ReadRGB(Stream* pStream)
{
	m_uRed = pStream->GetByte();
	m_uGreen = pStream->GetByte();
	m_uBlue = pStream->GetByte();
	m_uAlpha = 255;
}

/*! ************************************

	\fn Burger::Flash::RGBAWord8::Set(uint8_t bRed,uint8_t bGreen,uint8_t bBlue,uint8_t bAlpha)
	\brief Sets the color

	Sets the color a specific color

	\param bRed New red component
	\param bGreen New green component
	\param bBlue New blue component
	\param bAlpha New alpha component

	\sa Set(double) or Set(uint32_t)

***************************************/

/*! ************************************

	\brief Convert a 24 bit color to RGB

	Shift the color 16 bits right to obtain red, 8 bits
	to the right to obtain green and clamp to 8 bits to
	obtain blue.

	Red = (uColor>>16)&0xFF
	Green = (uColor>>8)&0xFF
	Blue = uColor&0xFF

	\param uColor Color encoded into a 24 bit number
	\sa Set(double)

***************************************/

void BURGER_API Burger::Flash::RGBAWord8::Set(uint32_t uColor)
{
	m_uRed = static_cast<uint8_t>(uColor >> 16U);
	m_uGreen = static_cast<uint8_t>(uColor >> 8U);
	m_uBlue = static_cast<uint8_t>(uColor);
	m_uAlpha = 255;
}

/*! ************************************

	\fn Burger::Flash::RGBAWord8::Set(double dColor)
	\brief Convert a 24 bit (double) color to RGB

	Convert the 64 bit floating point number into
	a 24 bit integer and then shift the color 16
	bits right to obtain red, 8 bits
	to the right to obtain green and clamp to 8 bits to
	obtain blue.

	Red = (uColor>>16)&0xFF
	Green = (uColor>>8)&0xFF
	Blue = uColor&0xFF

	\param dColor 24 bit color encoded into a 64 bit floating point number
	\sa Set(uint32_t)

***************************************/

/*! ************************************

	\brief Prints the color to the debugger

	Prints the color to Debug::Message()

***************************************/

void BURGER_API Burger::Flash::RGBAWord8::Print(void) const
{
	Debug::Message("RGBAWord8: %u %u %u %u\n",static_cast<uint_t>(m_uRed),static_cast<uint_t>(m_uGreen),static_cast<uint_t>(m_uBlue),static_cast<uint_t>(m_uAlpha));
}
