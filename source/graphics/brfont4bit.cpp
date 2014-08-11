/***************************************

	4 bit font class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfont4bit.h"
#include "brutf8.h"
#include "brendian.h"
#include "brpalette.h"

// The data is stored in little endian format

#if !defined(DOXYGEN)
struct Font4BitImage_t {
	Word16 m_usHeight;		// Height of the font in pixels
	Word16 m_usCount;		// Number of font cells
	Word16 m_usFirst;		// First ASCII char
	Word8 m_Widths[1];		// Width table
	// Word16 m_usOffsets[1];	// Offsets to the font strikes
	// RGBColorList_t m_DefaultColors;	// Default color list
};
#endif

/*! ************************************

	\class Burger::Font4Bit
	\brief Class to render a tiny space 4 bit per pixel font
	
	For systems with tight memory requirements, this font renderer
	store the font strike at 4 bits per pixel to allow color rendering while occupying
	only a fraction of the space a full texture based font would take.

	Use the font creation tool in MakeRez to create font data for this class

	This font type is optimized for software rendering

***************************************/

/*! ************************************

	\brief Constructor.
	
	Initializes all shared variables to defaults.
	
	\param pRenderer Pointer to a rendering context to use for
		low level APIs

***************************************/

Burger::Font4Bit::Font4Bit(Renderer *pRenderer) :
	Font(pRenderer),
	m_pRezFile(NULL),
	m_ppData(NULL),
	m_uPixelOffset(0),
	m_uRezNum(0),
	m_uInvisibleColor(0)
{
	MemoryClear(m_ColorTable.Words,sizeof(m_ColorTable.Words));
}

/*! ************************************

	\brief Destructor.
	
	Release all resources allocated by the font manager

	\sa Shutdown()
	
***************************************/

Burger::Font4Bit::~Font4Bit()
{
	Shutdown();
}

/***************************************

	\brief Determine the number of pixels the string will require to render onto the display

	Given a string of UTF8 text, determine the number of pixels will
	be needed to render the string. Return the value.
	Clipping is not considered in the calculation.
	Any invalid UTF8 characters or characters that are not present in the font
	are skipped.

	\param pInput Pointer to a UTF8 string. If \ref NULL or an empty string, return zero
	\param uLength Length of the string in bytes
	\sa Draw(const char *,WordPtr)

***************************************/

Word Burger::Font4Bit::GetPixelWidth(const char *pInput,WordPtr uLength)
{
	Word uResult = 0;				// Assume no width
	if (pInput && uLength) {
		void **pHandle = m_ppData;	// Init tables
		if (pHandle) {				// Data? And valid font?
			// Pointer to the table
			const Word8 *pWidths = static_cast<const Font4BitImage_t *>(pHandle[0])->m_Widths;
			Word uCount = m_uCount;
			Word uFirst = m_uFirst;
			do {
				// Get the char
				Word uTemp = reinterpret_cast<const Word8 *>(pInput)[0]-uFirst;
				if (uTemp<uCount) {					// Valid?
					uResult+=pWidths[uTemp];		// Add to the width
				}
				++pInput;
			} while (--uLength);
		}
	}
	return uResult;
}

/***************************************

	Draw a char on the screen
	It can draw to an 8 or 16 bit screen

	You may ask yourself, why in hell do I use goto's like I do,
	I use goto's for the road least traveled by so the CPU pipeline is
	not broken in most cases. This way the code runs faster in the most
	likely code cases.

***************************************/

void Burger::Font4Bit::DrawChar(Word uLetter)
{
	uLetter-=m_uFirst;			// Adjust to first VALID char
	if (uLetter>=m_uCount) {	// Can't draw this character?
		return;					// Exit now
	}
	const Font4BitImage_t *pFont = static_cast<const Font4BitImage_t *>(m_ppData[0]);

	// Bounds check the X coord
	// Width of the font strike
	Word uCharacterWidth = pFont->m_Widths[uLetter];
	int iTempX = m_iX;			// Get the X coord
	int iMaxX = iTempX+static_cast<int>(uCharacterWidth);

	// Set the new X Coordinate for next call
	m_iX = iMaxX;

	const Rect_t *pClip = m_pRenderer->GetClip();
	// Off the right or left side?
	if ((iTempX>=pClip->m_iRight) || (iMaxX<=pClip->m_iLeft)) {
		return;
	}

	// Bounds check the Y coord

	int iTempY = m_iY;				// Get the Y coord
	Word uHeight = m_uHeight;		// Get the height of the font char
	int iMaxY = static_cast<int>(uHeight)+iTempY;		// Get the bottommost Y coord
	// Off the top or bottom?
	if ((iTempY>=pClip->m_iBottom) || (iMaxY<=pClip->m_iTop)) {
		return;
	}
	Word uByteWidth = (uCharacterWidth+1)>>1;	// Convert to bytes per cel
	// Calculate the location of the font strike
	const Word8 *pFontIndexes = &pFont->m_Widths[m_uCount];
	Word uRGBListOffset = LittleEndian::LoadAny(&reinterpret_cast<const Word16 *>(pFontIndexes)[uLetter]);
	const Word8 *pStrike = pFontIndexes+uRGBListOffset;

	// Clip the font data
	// First clip the Y coord

	// Clip the top?
	if (iTempY<pClip->m_iTop) {
		// Clip the top of the font
		uHeight = static_cast<Word>(iMaxY-pClip->m_iTop);	// Crop the height (Bottom Y = height)
		pStrike = pStrike-((iTempY-pClip->m_iTop)*static_cast<int>(uByteWidth));	// (I'm subtracting a negative number)
		// Remove the upper pixels
		iTempY = pClip->m_iTop;		// Zap the top Y coord
	}

	// Clip the bottom?
	if (static_cast<Word>(iMaxY)>static_cast<Word>(pClip->m_iBottom)) {
		// Crop the height (iTempY is already >=0)
		uHeight = static_cast<Word>(pClip->m_iBottom-iTempY);
	}

	// Now, let's clip the X coord

	// I assume I don't need to begin INSIDE the font

	Word uSkipFlags = FALSE;			// Clear flags for 4 bit trimming (1 Skip first 4 bit pixel, 2 skip last 4 bit pixel)
	// Off the left side?
	if (iTempX<pClip->m_iLeft) {			
		// Rightmost X is the width
		uCharacterWidth = static_cast<Word>(iMaxX-pClip->m_iLeft);		
		iTempX -= pClip->m_iLeft;
		uSkipFlags = iTempX&1U;						// Even / Odd
		pStrike = pStrike - (iTempX>>1)-uSkipFlags;	// Subtracting a negative number
		iTempX = pClip->m_iLeft;					// Reset the dest X
	}
	// Clip to the right
	if (static_cast<Word>(iMaxX)>static_cast<Word>(pClip->m_iRight)) {
		uCharacterWidth = static_cast<Word>(pClip->m_iRight-iTempX);
	}

	// Let's finally draw the font
	// uHeight = clipped height of font
	// uCharacterWidth = clipped pixel width of font
	// uByteWidth = Bytes to skip per line of font data
	// pStrike = Pointer to font
	// pDest = Pointer to screen
	// uSkipFlags = True if I start drawing on odd pixel in font data
	// uLinePadding = Number of pad bytes after each scan line

	WordPtr uLinePadding;			// Video screen adjust against clipped font width
	Word uInvisibleColor;			// Color index to ignore

	// Draw here!
	register Word8 *pDest = (m_pRenderer->GetStride()*iTempY)+static_cast<Word8*>(m_pRenderer->GetFrameBuffer());

	// 8 bit renderer?
	if (m_pRenderer->GetDepth()<9) {

		// This is the 8 bit blitter

		// Make the screen coord
		pDest += iTempX;
		// Create the dest font skip
		uLinePadding = m_pRenderer->GetStride()-uCharacterWidth;

		// Will I draw a leading char?
		if (uSkipFlags) {
			// Remove 1 from width pair loop
			--uCharacterWidth;
			// There is a ++ to the source pointer for this
			--uByteWidth;
		}
        // Will I have an ending char?
	    if (uCharacterWidth&1) {
            // Set a flag
	    	uSkipFlags|=2;
            // +1 to the dest screen address
		    ++uLinePadding;
    	}
	    uCharacterWidth>>=1;			// Convert to pairs
    	uByteWidth-=uCharacterWidth;	// Remove adder
	    uInvisibleColor = m_uInvisibleColor;		// Get the mask color
    	do {
            // Skip first character?
	    	if (uSkipFlags&1U) {
                // Get font data
		    	Word uStrike = pStrike[0]&0x0FU;
                // Valid?
			    if (uStrike!=uInvisibleColor) {
                    // Store to screen
				    pDest[0] = m_ColorTable.Bytes[uStrike];
	    		}
    			++pDest;		// Next screen byte
		    	++pStrike;		// Next source pixel
	    	}

            // Any center part?
    		if (uCharacterWidth) {
                // Save font width in temp
    			Word uLoop = uCharacterWidth;
	    		do {
		    		Word uStrike2 = pStrike[0];	            // Get font data
			    	Word uStrike1 = uStrike2>>4U;	        // Split it
				    uStrike2 = uStrike2&0x0F;
    				if (uInvisibleColor!=uStrike1) {	    // Valid?
	    				pDest[0] = m_ColorTable.Bytes[uStrike1];	// Store to screen
		    		}
			    	if (uInvisibleColor!=uStrike2) {		// Ok?
				    	pDest[1] = m_ColorTable.Bytes[uStrike2];	// Store to screen
    				}
	    			pDest+=2;		// Add the data
		    		++pStrike;		// Next source pixel
			    } while (--uLoop);
    		}
            // Is there a trailing pixel?
	    	if (uSkipFlags&2U) {
		    	Word uStrike = pStrike[0];	// Get font data
		    	uStrike >>= 4U;
			    if (uStrike!=uInvisibleColor) {	// Valid?
				    pDest[0] = m_ColorTable.Bytes[uStrike];	        // Store to screen
    			}
	    	}
		    pDest+=uLinePadding;		// Adjust the screen pointer
    		pStrike += uByteWidth;  	// Adjust the font pointer
	    } while (--uHeight);
	} else {

    	// This routine will draw the font using 16 bit routines

        // Make the screen coord
	    pDest += iTempX<<1U;
        // Create the dest font skip
	    uLinePadding = m_pRenderer->GetStride()-(uCharacterWidth<<1);

	    if (uSkipFlags) {	            // Will I draw a leading char?
		    --uCharacterWidth;		    // Remove 1 from width pair loop
		    --uByteWidth;	            // There is a ++ to the source pointer for this
	    }
	    if (uCharacterWidth&1) {		// Will I have an ending char?
		    uSkipFlags|=2;	            // Set a flag
		    uLinePadding+=2;			// +1 to the dest screen address
	    }
    	uCharacterWidth>>=1U;			// Convert to pairs
	    uByteWidth-=uCharacterWidth;	// Remove adder
	    uInvisibleColor = m_uInvisibleColor;		// Get the mask color
    	do {
	    	if (uSkipFlags&1U) {	                // Skip first character?
		    	Word uStrike = pStrike[0]&0x0FU;	// Get font data
			    if (uStrike!=uInvisibleColor) {	    // Valid?
				    reinterpret_cast<Word16 *>(pDest)[0] = m_ColorTable.Shorts[uStrike];	// Store to screen
    			}
	    		pDest+=2;		                        // Next screen byte
		    	++pStrike;			                    // Next source pixel
    		}
	    	if (uCharacterWidth) {		                // Any center part?
		    	Word uLoop = uCharacterWidth;		    // Save font width in temp
			    do {
				    Word uStrike2 = pStrike[0];	        // Get font data
    				Word uStrike1 = uStrike2>>4U;	    // Split it
	    			uStrike2 = uStrike2&0x0FU;
		    		if (uInvisibleColor!=uStrike1) {	// Valid?
			    		reinterpret_cast<Word16 *>(pDest)[0] = m_ColorTable.Shorts[uStrike1];	// Store to screen
				    }
    				if (uInvisibleColor!=uStrike2) {    // Ok?
	    				reinterpret_cast<Word16 *>(pDest)[1] = m_ColorTable.Shorts[uStrike2];	// Store to screen
		    		}
			    	pDest+=4;		    // Add the data
				    ++pStrike;			// Next source pixel
    			} while (--uLoop);
	    	}
		    if (uSkipFlags&2U) {	                // Is there a trailing pixel?
			    Word uStrike = pStrike[0];	    	// Get font data
			    uStrike >>= 4U;
    			if (uStrike!=uInvisibleColor) {	    // Valid?
	    			reinterpret_cast<Word16 *>(pDest)[0] = m_ColorTable.Shorts[uStrike];	// Store to screen
		    	}
	    	}
    		pDest+=uLinePadding;		// Adjust the screen pointer
    		pStrike += uByteWidth;	    // Adjust the font pointer
	    } while (--uHeight);
    }
}

/*! ************************************

	\brief Initialize a font structure

	Load a font from a resource file and prepare it for
	drawing. This will set the font to consider
	color #0 to be invisible

	\param pRezFile Pointer to the resource file that the font resides in
	\param uRezNum Resource number of the font file
	\param pPalette Palette to use for drawing
	\param pRenderer Rendering context to use for drawing
	\sa InstallToPalette()

***************************************/

void Burger::Font4Bit::Init(RezFile *pRezFile,Word uRezNum,const Word8 *pPalette,Renderer *pRenderer)
{
	if (pRenderer) {
		m_pRenderer = pRenderer;
	}
	// Install the font
	InstallToPalette(pRezFile,uRezNum,pPalette);
	// Set up for masked shapes
	UseMask();
}

/*! ************************************

	\brief Release a font back to the resource manager

	Release all resource allocated by the font

***************************************/

void Burger::Font4Bit::Shutdown(void)
{
	Word uRezNum = m_uRezNum;
	if (uRezNum) {		// Was a font loaded?
		// Release the resource
		m_pRezFile->Release(uRezNum);
		m_uRezNum = 0;	// Kill the variables
		m_pRezFile = NULL;
		m_ppData = NULL;
		m_uFirst = 0;
		m_uCount = 0;
	}
}

/*! ************************************

	\brief Save the current state of the font manager

	Capture the current state of font manager so it can be restored later
	Values include the location of the draw cursor and the color rendering modes

	\param pOutput Pointer to a uninitialized structure to contain the state.
	\sa RestoreState()

***************************************/

void Burger::Font4Bit::SaveState(State_t *pOutput)
{
	pOutput->m_iX = m_iX;				// Save the current pen position
	pOutput->m_iY = m_iY;
	pOutput->m_uRezNum = m_uRezNum;		// Save the font number
	pOutput->m_pRezFile = m_pRezFile;	// Save the resource file
	pOutput->m_uInvisibleColor = m_uInvisibleColor;
	MemoryCopy(pOutput->m_ColorTable,m_ColorTable.Words,sizeof(pOutput->m_ColorTable));
}

/*! ************************************

	\brief Restore the previous state of the font manager
	\param pInput Pointer to a initialized structure that contains a previous state.
	\sa SaveState()

***************************************/

void Burger::Font4Bit::RestoreState(const State_t *pInput)
{
	m_iX = pInput->m_iX;		// Set the X and Y coords
	m_iY = pInput->m_iY;
	if (pInput->m_uRezNum) {		// Valid font number?
		InstallToPalette(pInput->m_pRezFile,pInput->m_uRezNum,NULL);	// Set the new font
	}
	m_uInvisibleColor=pInput->m_uInvisibleColor;
	MemoryCopy(m_ColorTable.Words,pInput->m_ColorTable,sizeof(m_ColorTable.Words));
}

/*! ************************************

	\brief Set a color for a font

	\param uColorIndex Index into the color table, maximum value is 15
	\param uColor Color in the current color mode to draw with

***************************************/

void Burger::Font4Bit::SetColor(Word uColorIndex,Word uColor)
{
	if (uColorIndex<16) {
		switch (m_pRenderer->GetDepth()) {		// Color mode?
		case 8:
			m_ColorTable.Bytes[uColorIndex] = static_cast<Word8>(uColor);	// Set the color
			break;
		case 15:
		case 16:
			m_ColorTable.Shorts[uColorIndex] = static_cast<Word16>(uColor);	// Set as 16 bit
			break;
		case 24:
		case 32:
			m_ColorTable.Words[uColorIndex] = static_cast<Word32>(uColor);	// Set as true color
			break;
		}
	}
}

/*! ************************************

	\fn Burger::Font4Bit::UseZero()
	\brief Make color index #0 visible

***************************************/

/*! ************************************

	\fn Burger::Font4Bit::UseMask()
	\brief Make color index #0 transparent

***************************************/

/*! ************************************

	\brief Install a font

	Load a font from the resource file and use the currently attached
	Display to determine how to render the image

	\param pRezFile Pointer to the resource file that the font resides in
	\param uRezNum Resource number of the font file
	\param pPalette Palette to use for drawing

***************************************/

void Burger::Font4Bit::InstallToPalette(Burger::RezFile *pRezFile,Word uRezNum,const Word8 *pPalette)
{
	if (m_uRezNum!=uRezNum) {		// Already in memory?
		Shutdown();					// Release the previous font
		if (uRezNum) {				// Valid font #?
			void **ppFont = pRezFile->LoadHandle(uRezNum);	// Load it in
			if (ppFont) {			// Did it load?
				m_ppData = ppFont;	// Save the handle
				const Font4BitImage_t *pFontImage = static_cast<Font4BitImage_t *>(ppFont[0]);
				m_uHeight = LittleEndian::Load(&pFontImage->m_usHeight);
				m_uCount = LittleEndian::Load(&pFontImage->m_usCount);
				m_uFirst = LittleEndian::Load(&pFontImage->m_usFirst);
				m_uPixelOffset = m_uCount+BURGER_OFFSETOF(Font4BitImage_t,m_Widths);
				m_uRezNum = uRezNum;		// Set the new font
				m_pRezFile = pRezFile;
				if (pPalette) {				// Normal mode?
					const Word8 *pFontIndexes = &pFontImage->m_Widths[m_uCount];
					// After the indexes, is a default color scheme, index to it and
					// use it to draw
					Word uRGBListOffset = LittleEndian::LoadAny(&reinterpret_cast<const Word16 *>(pFontIndexes)[m_uCount]);
					SetColorRGBListToPalette(reinterpret_cast<const RGBColorList_t *>(pFontIndexes+uRGBListOffset),pPalette);
				}
			}
		}
	}
}

/*! ************************************

	\brief Set the colors to a font

	Using a RGB color list, set all the colors to a mounted font.
	\note The palette provided must be the colors needed for rendering

***************************************/

void Burger::Font4Bit::SetColorRGBListToPalette(const RGBColorList_t *pRGBList,const Word8 *pPalette)
{
	Word uCount = pRGBList->m_uCount;	// Get the number of colors
	if (uCount) {						// No colors?!?!?
		if (uCount>=17) {
			uCount = 16;				// Failsafe, never use more than 16
		}
		const RGBWord8_t *pRGB = pRGBList->m_Colors;		// Point to the tripletts
		Word i = 0;						// Init color index
		if (m_pRenderer->GetDepth()<9) {
			pPalette=pPalette+3;		// Never remap to use color 0
			do {
				SetColor(i,Palette::FindColorIndex(pPalette,pRGB->m_uRed,pRGB->m_uGreen,pRGB->m_uBlue,254)+1);
				++pRGB;					// Some compilers pad!!
			} while (++i<uCount);		// Next index
		} else {
			Display *pDisplay = m_pRenderer->GetDisplay();
			do {
				SetColor(i,Palette::ToDisplay(pRGB,pDisplay));
				++pRGB;					// Some compilers pad!!
			} while (++i<uCount);
		}
	}
}

/*! ************************************

	\brief Set the font to use a different palette

	When a palette is changed, call this function to alert the font manager
	of the new color scheme. Needed when rendering to 15 bit or higher
	bit depths since palette changes are manually performed

	\param pPalette Pointer to a 256 color RGB palette

***************************************/

void Burger::Font4Bit::SetToPalette(const Word8 *pPalette)
{
	// Is there a handle?
	if (m_ppData && pPalette) {
        const Word8 *pFontIndexes = &static_cast<const Font4BitImage_t *>(m_ppData[0])->m_Widths[m_uCount];
		// After the indexes, is a default color scheme, index to it and
		// use it to draw
		Word uRGBListOffset = LittleEndian::LoadAny(&reinterpret_cast<const Word16 *>(pFontIndexes)[m_uCount]);
		SetColorRGBListToPalette(reinterpret_cast<const RGBColorList_t *>(pFontIndexes+uRGBListOffset),pPalette);
	}
}