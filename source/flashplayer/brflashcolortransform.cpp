/***************************************

    Flash player color transform manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brflashcolortransform.h"
#include "brdebug.h"
#include "brflashutils.h"

/*! ************************************

	\class Burger::Flash::ColorTransform
	\brief Color transform for Adobe Flash

	This class implements the color transformation
	values for Adobe Flash

	Look to page 23 of swf-file-format-spec.pdf for full documentation

	\sa RGBAWord8_t

***************************************/

/*! ************************************

	\brief Default constructor

	Set all the multiplication values to 1.0f and the
	color adders to 0.0f

	\sa Identity(void)

***************************************/

Burger::Flash::ColorTransform::ColorTransform() :
	m_fRedMultTerm(1.0f),
	m_fGreenMultTerm(1.0f),
	m_fBlueMultTerm(1.0f),
	m_fAlphaMultTerm(1.0f),
	m_fRedAddTerm(0.0f),
	m_fGreenAddTerm(0.0f),
	m_fBlueAddTerm(0.0f),
	m_fAlphaAddTerm(0.0f)
{
}

/*! ************************************

	\brief Set the transform to an inert state

	Set all the multiplication values to 1.0f and the
	color adders to 0.0f

	\sa ColorTransform()

***************************************/

void BURGER_API Burger::Flash::ColorTransform::Identity(void)
{
	m_fRedMultTerm = 1.0f;
	m_fGreenMultTerm = 1.0f;
	m_fBlueMultTerm = 1.0f;
	m_fAlphaMultTerm = 1.0f;
	m_fRedAddTerm = 0.0f;
	m_fGreenAddTerm = 0.0f;
	m_fBlueAddTerm = 0.0f;
	m_fAlphaAddTerm = 0.0f;
}

/*! ************************************

	\brief Concatenate a color transform

	Transform this instance with another so that it will
	act on a color if the input transform was done first and
	then this one.

	\param pInput Pointer to a color transform to apply to this one.
	\sa Transform(RGBAWord8_t *,const RGBAWord8_t*) const

***************************************/

void BURGER_API Burger::Flash::ColorTransform::Concatenate(const ColorTransform *pInput)
{
	// Transform the color and then apply
	m_fRedAddTerm += m_fRedMultTerm * pInput->m_fRedAddTerm;
	m_fGreenAddTerm += m_fGreenMultTerm * pInput->m_fGreenAddTerm;
	m_fBlueAddTerm += m_fBlueMultTerm * pInput->m_fBlueAddTerm;
	m_fAlphaAddTerm += m_fAlphaMultTerm * pInput->m_fAlphaAddTerm;
	// Multiply by the alphas
	m_fRedMultTerm *= pInput->m_fRedMultTerm;
	m_fGreenMultTerm *= pInput->m_fGreenMultTerm;
	m_fBlueMultTerm *= pInput->m_fBlueMultTerm;
	m_fAlphaMultTerm *= pInput->m_fAlphaMultTerm;
}

/*! ************************************

	\brief Transform a color

	Transform the color and clamp the values to prevent overflow
	and underflow.

	\param pOutput Pointer to an RGBA color to receive the transformed color
	\param pInput Pointer to an RGBA color to transform
	\sa Concatenate(const ColorTransform *)

***************************************/

void BURGER_API Burger::Flash::ColorTransform::Transform(RGBAWord8_t *pOutput,const RGBAWord8_t *pInput) const
{
	pOutput->m_uRed = static_cast<uint8_t>(static_cast<int>(Burger::Clamp(static_cast<float>(pInput->m_uRed) * m_fRedMultTerm + m_fRedAddTerm,0.0f,255.0f)));
	pOutput->m_uGreen = static_cast<uint8_t>(static_cast<int>(Burger::Clamp(static_cast<float>(pInput->m_uGreen) * m_fGreenMultTerm + m_fGreenAddTerm,0.0f,255.0f)));
	pOutput->m_uBlue = static_cast<uint8_t>(static_cast<int>(Burger::Clamp(static_cast<float>(pInput->m_uBlue) * m_fBlueMultTerm + m_fBlueAddTerm,0.0f,255.0f)));
	pOutput->m_uAlpha = static_cast<uint8_t>(static_cast<int>(Burger::Clamp(static_cast<float>(pInput->m_uAlpha) * m_fAlphaMultTerm + m_fAlphaAddTerm,0.0f,255.0f)));
}

/*! ************************************

	\brief Read in an RGB color transform from the input stream

	Look to page 24 of swf-file-format-spec.pdf for full documentation

	\param pStream Pointer to data stream to read from
	\sa ReadRGBA(Stream*)

***************************************/

void BURGER_API Burger::Flash::ColorTransform::ReadRGB(Stream* pStream)
{
	// Align the bit stream
	pStream->ByteAlign();

	// Get the flags
	uint_t bHasAddTerms = pStream->GetWord(1);
	uint_t bHasMultTerms = pStream->GetWord(1);
	uint_t uNbits = pStream->GetWord(4);

	if (bHasMultTerms) {
		// 8.8 fixed point
		m_fRedMultTerm = static_cast<float>(pStream->GetInt(uNbits)) * (1.0f/256.0f);
		m_fGreenMultTerm = static_cast<float>(pStream->GetInt(uNbits)) * (1.0f/256.0f);
		m_fBlueMultTerm = static_cast<float>(pStream->GetInt(uNbits)) * (1.0f/256.0f);
	} else {
		m_fRedMultTerm = 1.0f;
		m_fGreenMultTerm = 1.0f;
		m_fBlueMultTerm = 1.0f;
	}
	if (bHasAddTerms) {
		m_fRedAddTerm = static_cast<float>(pStream->GetInt(uNbits));
		m_fGreenAddTerm = static_cast<float>(pStream->GetInt(uNbits));
		m_fBlueAddTerm = static_cast<float>(pStream->GetInt(uNbits));
	} else {
		m_fRedAddTerm = 0.0f;
		m_fGreenAddTerm = 0.0f;
		m_fBlueAddTerm = 0.0f;
	}
	// Set alpha to inert values
	m_fAlphaMultTerm = 1.0f;
	m_fAlphaAddTerm = 0.0f;
}

/*! ************************************

	\brief Read in an RGBA color transform from the input stream

	Look to page 25 of swf-file-format-spec.pdf for full documentation

	\param pStream Pointer to data stream to read from
	\sa ReadRGB(Stream*)

***************************************/

void BURGER_API Burger::Flash::ColorTransform::ReadRGBA(Stream* pStream)
{
	// Align the bit stream
	pStream->ByteAlign();

	// Get the flags
	uint_t bHasAddTerms = pStream->GetWord(1);
	uint_t bHasMultTerms = pStream->GetWord(1);
	uint_t uNbits = pStream->GetWord(4);

	if (bHasMultTerms) {
		m_fRedMultTerm = static_cast<float>(pStream->GetInt(uNbits)) * (1.0f/256.0f);
		m_fGreenMultTerm = static_cast<float>(pStream->GetInt(uNbits)) * (1.0f/256.0f);
		m_fBlueMultTerm = static_cast<float>(pStream->GetInt(uNbits)) * (1.0f/256.0f);
		m_fAlphaMultTerm = static_cast<float>(pStream->GetInt(uNbits)) * (1.0f/256.0f);
	} else {
		m_fRedMultTerm = 1.0f;
		m_fGreenMultTerm = 1.0f;
		m_fBlueMultTerm = 1.0f;
		m_fAlphaMultTerm = 1.0f;
	}
	if (bHasAddTerms) {
		m_fRedAddTerm = static_cast<float>(pStream->GetInt(uNbits));
		m_fGreenAddTerm = static_cast<float>(pStream->GetInt(uNbits));
		m_fBlueAddTerm = static_cast<float>(pStream->GetInt(uNbits));
		m_fAlphaAddTerm = static_cast<float>(pStream->GetInt(uNbits));
	} else {
		m_fRedAddTerm = 0.0f;
		m_fGreenAddTerm = 0.0f;
		m_fBlueAddTerm = 0.0f;
		m_fAlphaAddTerm = 0.0f;
	}
}

/*! ************************************

	\brief Clamp all variables to legal values

	Clamp the multiplier to 0.0f to 1.0f and the color to -255.0f
	to 255.0f

	\sa Identity(void)

***************************************/

void BURGER_API Burger::Flash::ColorTransform::Clamp(void)
{
	m_fRedMultTerm = Burger::Clamp(m_fRedMultTerm,0.0f,1.0f);
	m_fGreenMultTerm = Burger::Clamp(m_fGreenMultTerm,0.0f,1.0f);
	m_fBlueMultTerm = Burger::Clamp(m_fBlueMultTerm,0.0f,1.0f);
	m_fAlphaMultTerm = Burger::Clamp(m_fAlphaMultTerm,0.0f,1.0f);

	m_fRedAddTerm = Burger::Clamp(m_fRedAddTerm,-255.0f,255.0f);
	m_fGreenAddTerm = Burger::Clamp(m_fGreenAddTerm,-255.0f,255.0f);
	m_fBlueAddTerm = Burger::Clamp(m_fBlueAddTerm,-255.0f,255.0f);
	m_fAlphaAddTerm = Burger::Clamp(m_fAlphaAddTerm,-255.0f,255.0f);
}

/*! ************************************

	\brief Prints the color transform to the debugger

	Prints the color to Debug::Message()

***************************************/

void BURGER_API Burger::Flash::ColorTransform::Print(void) const
{
	Debug::Message("Red transform %4.4f %4.4f\n",m_fRedMultTerm,m_fRedAddTerm);
	Debug::Message("Green transform %4.4f %4.4f\n",m_fGreenMultTerm,m_fGreenAddTerm);
	Debug::Message("Blue transform %4.4f %4.4f\n",m_fBlueMultTerm,m_fBlueAddTerm);
	Debug::Message("Alpha transform %4.4f %4.4f\n",m_fAlphaMultTerm,m_fAlphaAddTerm);
}
