/***************************************

    Font base class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfont.h"
#include "brutf8.h"
#include "brutf32.h"
#include "brnumberto.h"

// RTTI data
#if !defined(DOXYGEN)
BURGER_CREATE_STATICRTTI_PARENT(Burger::Font,Burger::Base);
#endif

/*! ************************************

	\class Burger::Font
	\brief Base class for font manager
	
	This is the base class for rendering with a font
	onto a video display. 

	When a font is created, it needs to be attached to a Renderer context
	for actual low level pixel manipulation

***************************************/

/*! ************************************

	\brief Constructor.
	
	Initializes all shared variables to defaults.
	
	\param pDisplay Pointer to a display context to use for
		low level APIs

***************************************/

Burger::Font::Font(Display *pDisplay) :
	m_pDisplay(pDisplay),
	m_iX(0),
	m_iY(0),
	m_uHeight(0),
	m_uFirst(0),
	m_uCount(0)
{
}

/*! ************************************

	\brief Destructor.
	
	Does absolutely nothing
	
***************************************/

Burger::Font::~Font()
{
}

/*! ************************************

	\brief Draws a string of UTF8 text onto the
	screen.

	Render a string of text from the X/Y origin.
	No attempt is made to perform word wrap. The string will be clipped
	to the Renderer class's clip boundaries

	\param pInput Pointer to a UTF8 string. If \ref NULL or an empty string, perform nothing
	\param uLength Length of the string in bytes
	\sa GetPixelWidth(const char *,uintptr_t)

***************************************/

void Burger::Font::Draw(const char *pInput,uintptr_t uLength)
{
	if (uLength) {
		const char *pEnd = pInput+uLength;
		do {
			const uint32_t uChar = UTF32::translate_from_UTF8(pInput);
			// Bad string?
			if (uChar == UTF32::kInvalid) {
				break;
			}
			DrawChar(uChar);						// Draw the character
			pInput = UTF8::next_token(pInput);		// Next
		} while (pInput<pEnd);
	}
}

/*! ************************************

	\brief Determine the number of pixels the string will require to render onto the display

	Given a string of UTF8 text, determine the number of pixels will
	be needed to render the string. Return the value.
	Clipping is not considered in the calculation.
	Any invalid UTF8 characters or characters that are not present in the font
	are skipped.

	\param pInput Pointer to a UTF8 string. If \ref NULL or an empty string, return zero
	\param uLength Length of the string in bytes
	\sa Draw(const char *,uintptr_t)

***************************************/

uint_t Burger::Font::GetPixelWidth(const char * /* pInput */ ,uintptr_t /* uLength */)
{
	return 0;
}

/*! ************************************

	\brief Draw a single UTF32 character

	Given a valid UTF32 code, draw the single character

	\param uLetter UTF32 character code to draw
	\sa Draw(const char *,uintptr_t)

***************************************/

void Burger::Font::DrawChar(uint_t /* uLetter */)
{
}

/*! ************************************

	\fn Burger::Font::SetDisplay(Display *pDisplay)
	\brief Set the Display to use

	\param pDisplay Pointer to a Display
	\sa GetDisplay(void) const

***************************************/

/*! ************************************

	\fn Burger::Font::GetDisplay(void) const
	\brief Get the Display currently in use

	\return Pointer to a Display, can be \ref NULL if no Display has been set
	\sa SetDisplay(Display *)

***************************************/

/*! ************************************

	\fn Burger::Font::SetX(int iX)
	\brief Set the draw cursor X coordinate

	Sets the x coordinate where the next draw call will operate at

	\param iX New X coordinate to draw at
	\sa GetX(void) const, SetY(int) or SetXY(int,int)

***************************************/

/*! ************************************

	\fn Burger::Font::SetY(int iY)
	\brief Set the draw cursor Y coordinate

	Sets the y coordinate where the next draw call will operate at

	\param iY New Y coordinate to draw at
	\sa GetY(void) const, SetX(int) or SetXY(int,int)

***************************************/

/*! ************************************

	\fn Burger::Font::SetXY(int iX,int iY)
	\brief Set the draw cursor X and Y coordinate

	Sets the x and y coordinates where the next draw call will operate at

	\param iX New X coordinate to draw at
	\param iY New Y coordinate to draw at
	\sa SetX(int) or SetY(int)

***************************************/

/*! ************************************

	\fn Burger::Font::GetX(void) const
	\brief Get the draw cursor X coordinate

	Returns the x coordinate where the next draw call will operate at

	\return Current X coordinate to draw at
	\sa GetY(void) const, SetX(int)

***************************************/

/*! ************************************

	\fn Burger::Font::GetY(void) const
	\brief Get the draw cursor Y coordinate

	Returns the y coordinate where the next draw call will operate at

	\return Current Y coordinate to draw at
	\sa GetX(void) const, SetY(int)

***************************************/

/*! ************************************

	\fn Burger::Font::GetHeight(void) const
	\brief Get the height of the font in pixels

	Returns the height in pixels of the currently installed font typeface.

	\return Height in pixels of the font typeface.
	\sa GetFirstChar(void) const and GetCount(void) const

***************************************/

/*! ************************************

	\fn Burger::Font::GetFirstChar(void) const
	\brief Get the lowest valid character in UTF32 format that resides in the font.

	Returns the lowest character code in the font. If both the first
	and last character are zero, then no characters are valid

	\return First valid UTF32 character code in the font typeface
	\sa GetHeight(void) const and GetCount(void) const

***************************************/

/*! ************************************

	\fn Burger::Font::GetCount(void) const
	\brief Get the highest valid character in UTF32 format that resides in the font.

	Returns the highest character code in the font. If both the first
	and last character are zero, then no characters are valid

	\return Last valid UTF32 character code in the font typeface
	\sa GetFirstChar(void) const and GetHeight(void) const

***************************************/

/*! ************************************

	\brief Given a UTF32 character code, return the width in pixels the typeface
	will render to.

	Invalid UTF32 character codes will return a width of zero

	\param uLetter UTF32 character code
	\return Number of pixels wide this character strike
	\sa GetPixelWidth(const char *,uintptr_t)

***************************************/

uint_t BURGER_API Burger::Font::GetPixelWidthChar(uint_t uLetter)
{
	char Ascii[8];
	const uintptr_t uLength = UTF8::from_UTF32(Ascii,uLetter);
	return GetPixelWidth(Ascii,uLength);
}

/*! ************************************

	\brief Determine the number of pixels needed to draw a numeric string

	Convert the input value into a string and determine the number of pixels
	needed to render the string

	\param iInput Signed input value to convert into a numeric string
	\sa GetPixelWidth(const char *,uintptr_t)

***************************************/

uint_t BURGER_API Burger::Font::GetPixelWidthNumber(int32_t iInput)
{
	char Ascii[16];

	NumberToAscii(Ascii,iInput);
	return GetPixelWidth(Ascii,string_length(Ascii));
}

/*! ************************************

	\brief Determine the number of pixels needed to draw a numeric string

	Convert the input value into a string and determine the number of pixels
	needed to render the string

	\param uInput Unsigned input value to convert into a numeric string
	\sa GetPixelWidth(const char *,uintptr_t)

***************************************/

uint_t BURGER_API Burger::Font::GetPixelWidthNumber(uint32_t uInput)
{
	char Ascii[16];

	NumberToAscii(Ascii,uInput);
	return GetPixelWidth(Ascii,string_length(Ascii));
}

/*! ************************************

	\brief Determine the number of pixels needed to draw a UTF8 "C" string

	\param pInput Pointer to a UTF8 "C" string or \ref NULL to get zero
	\return The string's width in pixels
	\sa GetPixelWidth(const char *,uintptr_t)

***************************************/

uint_t BURGER_API Burger::Font::GetPixelWidthString(const char *pInput)
{
	if (pInput) {
		return GetPixelWidth(pInput,string_length(pInput));		// Get the text width
	}
	return 0;
}

/*! ************************************

	\brief Draw a signed numeric string

	Convert the input value into a string and draw it at the current
	x and y coordinates

	\param iInput Signed input value to convert into a numeric string
	\sa Draw(const char *,uintptr_t)

***************************************/

void BURGER_API Burger::Font::DrawNumber(int32_t iInput)
{
	char Ascii[16];

	NumberToAscii(Ascii,iInput);
	Draw(Ascii,string_length(Ascii));
}

/*! ************************************

	\brief Draw an unsigned numeric string

	Convert the input value into a string and draw it at the current
	x and y coordinates

	\param uInput Unsigned input value to convert into a numeric string
	\sa Draw(const char *,uintptr_t)

***************************************/

void BURGER_API Burger::Font::DrawNumber(uint32_t uInput)
{
	char Ascii[16];

	NumberToAscii(Ascii,uInput);
	Draw(Ascii,string_length(Ascii));
}

/*! ************************************

	\brief Draw a UTF8 "C" string

	Take a valid UTF8 coded "C" string and draw it at the current
	x and y coordinates

	\param pInput Pointer to a valid UTF8 encoded "C" string. \ref NULL performs no action.
	\sa Draw(const char *,uintptr_t)

***************************************/

void BURGER_API Burger::Font::DrawString(const char *pInput)
{
	if (pInput) {
		Draw(pInput,string_length(pInput));		// Draw the text
	}
}

/*! ************************************

	\brief Draw a centered string

	Given a UTF8 encoded "C" string and an X/Y coordinate, determine
	the width in pixels the string will occupy and render the string
	with it centered on the x coordinate.

	\param iX X coordinate to center the string on
	\param iY Y coordinate to draw the string at
	\param pInput Pointer to a valid "C" string. \ref NULL performs no action.
	\sa Draw(const char *,uintptr_t)

***************************************/

void BURGER_API Burger::Font::DrawStringCenterX(int iX,int iY,const char *pInput)
{
	if (pInput) {				// Failsafe
		uintptr_t uLength = string_length(pInput);			// Get the string length
		uint_t uWidth = GetPixelWidth(pInput,uLength);	// Get the string width
		m_iX = iX - static_cast<int>(uWidth>>1U);
		m_iY = iY;
		Draw(pInput,uLength);
	}
}

/*! ************************************

	\brief Draw a string at a specific location

	Draw a UTF8 "C" string at a specific X/Y coordinate.

	\param iX X coordinate to draw at
	\param iY Y coordinate to draw at
	\param pInput Pointer to a valid UTF8 encoded "C" string
	\sa Draw(const char *,uintptr_t)

***************************************/

void BURGER_API Burger::Font::DrawStringAtXY(int iX,int iY,const char *pInput)
{
	m_iX = iX;		// Set the location
	m_iY = iY;
	if (pInput) {	// Failsafe
		Draw(pInput,string_length(pInput));
	}
}

/*! ************************************

	\brief Return the number of characters in a string that would fit in a pixel width.
	
	To perform word wrap, this function will scan each "word" by using
	spaces as delimiters and CR, LF and zero as an end of string.
	It will perform a test after each word until the clip width is hit.
	Zero can be returned if the string cannot fit due to word wrap or if the
	string is empty.

	\param pInput Pointer to a UTF8 encoded "C" string to parse.
	\param uWidth Maximum number of pixels allowed to draw
	\return Number of bytes of the string that can be drawn without clipping

***************************************/

uintptr_t BURGER_API Burger::Font::CharsForPixelWidth(const char *pInput,uint_t uWidth)
{
	uintptr_t uResult = 0;			// Number of characters that are valid
	const char *pMark = pInput;		// Save in a temp
	const char *pWork = pInput;		// Get the current mark

	for (;;) {
		uint_t uChar;					// Temp
			
		// Scan to the next space, CR/LF or the end of the string
		
		do {
			uChar = reinterpret_cast<const uint8_t *>(pWork)[0];		// Get a character
			++pWork;												// Next char
		} while (uChar && (uChar!=' ') && (uChar != 13) && (uChar!=10));	// End here?
		--pWork;
		uintptr_t uTemp = static_cast<uintptr_t>(pWork-pMark);			// Number of characters to test for

		uint_t uPixelWidth = GetPixelWidth(pInput,uResult+uTemp);		// Test the pixel width
		if (uPixelWidth>uWidth) {			// Is this too long?
			if (!uResult) {					// Nothing parsed before
				uResult = uTemp;			// Accept the line as is... (Bad but it will have to do)
			}
			break;
		}
		uResult+=uTemp;						// Accept this length
		if (uChar!=' ') {					// End of the line
			break;							// Return this NOW!!
		}
		pMark = pWork;						// My new starting point
		do {
			++pWork;						// Skip past the space
			uChar = reinterpret_cast<const uint8_t *>(pWork)[0];
		} while (uChar==' ');				// Wait until a non-space
	}
	return uResult;							// No data to print
}

