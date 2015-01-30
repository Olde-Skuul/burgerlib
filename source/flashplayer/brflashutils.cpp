/***************************************

	Flash player utility functions
	
	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#include "brflashutils.h"
#include "brstringfunctions.h"

/*! ************************************

	\enum Burger::Flash::eFileType
	\brief Supported file type enumerations

	Enumeration of data files that are supported by the Flash
	player

***************************************/

/*! ************************************

	\enum Burger::Flash::eActionScriptStandardMember
	\brief Supported file type enumerations

	Enumeration of data files that are supported by the Flash
	player

***************************************/


/*! ************************************

	\brief Return the version of the flash player

	The version is in Adobe format and it's currently
	"WIN 9.0.0.0".

	\return "C" string with the current version of the Flash player

***************************************/

const char * BURGER_API Burger::Flash::GetVersion(void)
{
	return "WIN 9.0.0.0";
}

/*! ************************************

	\fn float Burger::Flash::TwipsToPixels(float fInput)
	\brief Convert a value from Flash Twips to Pixels

	Flash uses a coordinate system of 20 twips per pixel.
	This function converts from twips to pixels by a
	multiplication of 1.0f/20.0f
	
	\param fInput Value in Flash twips
	\return Value converted to pixels
	\sa TwipsToPixels(Vector2D_t *) or PixelsToTwips(float)

***************************************/

/*! ************************************

	\fn float Burger::Flash::PixelsToTwips(float fInput)
	\brief Convert a value from Pixels to Flash Twips

	Flash uses a coordinate system of 20 twips per pixel.
	This function converts from pixels to twips by a
	multiplication of 20.0f
	
	\param fInput Value in pixels
	\return Value converted to Flash twips
	\sa PixelsToTwips(Vector2D_t *) or TwipsToPixels(float)

***************************************/

/*! ************************************

	\brief Convert a 2D point from Flash Twips to Pixels

	Flash uses a coordinate system of 20 twips per pixel.
	This function converts from twips to pixels by a
	multiplication of 1.0f/20.0f
	
	\param pInput Pointer to a Vector2D_t to convert
	\sa TwipsToPixels(float) or PixelsToTwips(Vector2D_t *)

***************************************/

void BURGER_API Burger::Flash::TwipsToPixels(Vector2D_t *pInput)
{
	pInput->x = TwipsToPixels(pInput->x);
	pInput->y = TwipsToPixels(pInput->y);
}

/*! ************************************

	\brief Convert a 2D point from Pixels to Flash Twips

	Flash uses a coordinate system of 20 twips per pixel.
	This function converts from pixels to twips by a
	multiplication of 20.0f
	
	\param pInput Pointer to a Vector2D_t to convert
	\sa PixelsToTwips(float) or TwipsToPixels(Vector2D_t *)

***************************************/

void BURGER_API Burger::Flash::PixelsToTwips(Vector2D_t *pInput)
{
	pInput->x = PixelsToTwips(pInput->x);
	pInput->y = PixelsToTwips(pInput->y);

}

/*! ************************************

	\brief Convert a texture width/height to a power of 2

	Textures may need to be scaled to a power of two, this function
	will take a coordinate and scale it up to the nearest power of 2.

	In the case where the scaling up would result in an excess of
	stretched pixels (60% stretching), it will be scaled to the next lower
	power of two.
	
	\param uInput Value to convert
	\sa PowerOf2(Word32)

***************************************/

Word32 BURGER_API Burger::Flash::TextureSizePower2(Word32 uInput)
{
	// Convert to the next power of 2
	Word32 uResult = PowerOf2(uInput);
	// If the ratio of uResult/uInput is greater than 3/5, scale down one shift
	if ((uInput*5U) < (uResult*3U)) {
		uResult>>=1;
	}
	return uResult;
}

/*! ************************************

	\brief Extract a 32 bit unsigned integer from a byte stream

	Adobe flash uses a byte stream encoding to save space when
	recording 32 bit unsigned integers. This function will decode
	up to 5 bytes from the stream and output the 32 bit
	unsigned value decoded.

	This code was lovingly ripped off from the file
	http://wwwimages.adobe.com/www.adobe.com/content/dam/Adobe/en/devnet/swf/pdf/swf-file-format-spec.pdf
	at page 17.
	
	\param pOutput Pointer to store the decoded Word32
	\param pInput Pointer to the byte stream
	\return Number of bytes read in from the byte stream (1 through 5)

***************************************/

WordPtr BURGER_API Burger::Flash::GetEncodedU32Length(Word32 *pOutput,const Word8 *pInput)
{
	// Get the first byte from the stream
	Word32 uResult = pInput[0];
	WordPtr uLength = 1;
	if (uResult & 0x80) {
		uLength = 2;
		// Use an exclusive or to clear off the flag bit
		uResult = (uResult ^ 0x80) | (static_cast<Word32>(pInput[1]) << 7U);
		if (uResult & 0x4000) {
			uLength = 3;
			uResult = (uResult ^ 0x4000) | (static_cast<Word32>(pInput[2]) << 14U);
			if (uResult & 0x200000) {
				uLength = 4;
				uResult = (uResult ^ 0x200000) | (static_cast<Word32>(pInput[3]) << 21U);
				if (uResult & 0x10000000) {
					uLength = 5;
					uResult = (uResult ^ 0x10000000) | (static_cast<Word32>(pInput[4]) << 28U);
				}
			}
		}
	}
	// Store the decoded 32 bit value
	pOutput[0] = uResult;
	// Return the number of bytes decoded
	return uLength;
}

/*! ************************************

	\brief Enumerate common class names
	
	Perform a case insensitive comparison to a list of
	class names and return an enumeration.

	\param pName Pointer to "C" string of name to enumerate
	\return Enumeration of the string or \ref M_INVALID_MEMBER if not found

***************************************/

#if !defined(DOXYGEN)
static const char *StandardMemberNames[Burger::Flash::ACTION_SCRIPT_STANDARD_MEMBER_COUNT] = {
	"_x",				// 0 M_X
	"_y",				// 1 M_Y
	"_xscale",			// 2 M_XSCALE
	"_yscale",			// 3 M_YSCALE
	"_currentframe",	// 4 M_CURRENTFRAME
	"_totalframes",		// 5 M_TOTALFRAMES
	"_alpha",			// 6 M_ALPHA
	"_visible",			// 7 M_VISIBLE
	"_width",			// 8 M_WIDTH
	"_height",			// 9 M_HEIGHT
	"_rotation",		// 10 M_ROTATION
	"_target",			// 11 M_TARGET
	"_framesloaded",	// 12 M_FRAMESLOADED
	"_name",			// 13 M_NAME
	"_droptarget",		// 14 M_DROPTARGET
	"_url",				// 15 M_URL
	"_highquality",		// 16 M_HIGHQUALITY
	"_focusrect",		// 17 M_FOCUSRECT
	"_soundbuftime",	// 18 M_SOUNDBUFTIME
	"_xmouse",			// 19 M_XMOUSE
	"_ymouse",			// 20 M_YMOUSE
	"_parent",			// 21 M_PARENT
	"text",				// 22 M_TEXT
	"textWidth",		// 23 M_TEXTWIDTH
	"textColor",		// 24 M_TEXTCOLOR
	"border",			// 25 M_BORDER
	"multiline",		// 26 M_MULTILINE
	"wordWrap",			// 27 M_WORDWRAP
	"type",				// 28 M_TYPE
	"backgroundColor",	// 29 M_BACKGROUNDCOLOR
	"_this",			// 30 M_THIS
	"this",				// 31 MTHIS
	"_root",			// 32 M_ROOT
	".",				// 33 MDOT
	"..",				// 34 MDOT2
	"_level0",			// 35 M_LEVEL0
	"_global",			// 36 M_GLOBAL
	"enabled",			// 37 M_ENABLED
	"password",			// 38 M_PASSWORD
	"onMouseMove"		// 39 M_MOUSE_MOVE
};
#endif

Burger::Flash::eActionScriptStandardMember BURGER_API Burger::Flash::EnumerateStandardMember(const char *pName)
{
	WordPtr uCount = BURGER_ARRAYSIZE(StandardMemberNames);
	eActionScriptStandardMember eResult = M_INVALID_MEMBER;
	const char **ppArray = StandardMemberNames;
	do {
		if (!StringCaseCompare(pName,ppArray[0])) {
			eResult = static_cast<eActionScriptStandardMember>(BURGER_ARRAYSIZE(StandardMemberNames)-uCount);
			break;
		}
		++ppArray;
	} while (--uCount);
	return eResult;
}

/*! ************************************

	\brief Enumerate support file extensions

	Check the filename and determine what type of
	data is contained.

	\param pFilename Pointer to "C" string of the filename to test
	\return Enumeration of the filetype or \ref UNKNOWN if not found

***************************************/

Burger::Flash::eFileType BURGER_API Burger::Flash::GetFileType(const char *pFilename)
{
	eFileType eResult = UNKNOWN;
	if (!StringCaseCompare(pFilename,"http://",7)) {
		eResult = URL;
	} else {
		const char *pExtension = GetFileExtension(pFilename);
		if (pExtension) {
			if (!StringCaseCompare(pExtension,"swf")) {
				eResult = SWF;
			} else if (!StringCaseCompare(pExtension,"jpg")) {
				eResult = JPG;
			} else if (!StringCaseCompare(pExtension,"3ds")) {		
				eResult = X3DS;
			} else if (!StringCaseCompare(pExtension,"txt")) {		
				eResult = TXT;
			}
		}
	}
	return eResult;
}

/*! ************************************

	\brief Create a full pathname

	Check if the input is a fully qualified pathname
	or a web page URL and if it is, return as is.
	Otherwise, prefix the directory to the pathname.

	\param pOutput Pointer to the Filename to receive the output
	\param pDirectory Pointer to the home directory
	\param pInput Pointer to "C" string of the filename to test

***************************************/

void BURGER_API Burger::Flash::GetFullURL(Filename *pOutput,const Filename *pDirectory,const char *pInput)
{
	// is path relative ?
	if ((pInput[1] == ':') ||
		(pInput[0] == '/') ||
		(!StringCaseCompare(pInput,"http://",7))) {
			pOutput->Set(pInput);
			// path like c:\my.swf or /home/my.swf or URL is absolute
	} else {
		pOutput[0] = pDirectory[0];
		pOutput->Append(pInput);
	}
}

/*! ************************************

	\class Burger::Flash::Multiname
	\brief Data packet for Flash Multiname support

	Adobe flash virtual machine uses name spaces to share
	code and data. This structure encapsulates the data
	
	Documentation is found starting at page 23.

	http://www.adobe.com/content/dam/Adobe/en/devnet/actionscript/articles/avm2overview.pdf
	
	\sa Namespace

***************************************/

/*! ************************************

	\fn Burger::Flash::Multiname::Multiname()
	\brief Default constructor

	Sets the object to \ref CONSTANT_UNDEFINED

***************************************/

/*! ************************************

	\fn Word Burger::Flash::Multiname::IsQName(void) const
	\brief Test if the object is a QName

	\return \ref TRUE if the object is QName type.
	\sa \ref CONSTANT_QName

***************************************/


/*! ************************************

	\class Burger::Flash::Namespace
	\brief Data packet for Flash Multiname support

	Adobe flash virtual machine uses name spaces to share
	code and data. This structure encapsulates the data
	to differentiate differing behaviors of name spaces
	
	Documentation is found starting at page 26.

	http://www.adobe.com/content/dam/Adobe/en/devnet/actionscript/articles/avm2overview.pdf
	
	\sa Multiname

***************************************/

/*! ************************************

	\fn Burger::Flash::Namespace::Namespace()
	\brief Default constructor

	Sets the object to \ref CONSTANT_Undefined

***************************************/



BURGER_CREATE_STATICRTTI_PARENT(Burger::Flash::BaseExecute,Burger::Base);

/*! ************************************

	\class Burger::Flash::BaseExecute
	\brief Execution base class

	Execute tags include things that control the operation of
	the Flash movie. Essentially, these are the events associated
	with a frame.

***************************************/

/*! ************************************

	\fn Burger::Flash::BaseExecute::BaseExecute()
	\brief Default constructor

	Assigns default functions for everything that wasn't
	taken control of in a derived class.

***************************************/


/*! ************************************

	\brief Default destructor

	Doesn't do anything. Look at the derived classes
	for any actions

***************************************/

Burger::Flash::BaseExecute::~BaseExecute()
{
}

/*! ************************************

	\brief Execute action

	Perform the action
	\param pObject Pointer to an object to perform the action on

***************************************/

void Burger::Flash::BaseExecute::Execute(Flash::CharacterObject* /* pObject */)
{
}

/*! ************************************

	\brief Execute state change

	Perform a state change
	\param pObject Pointer to an object to perform the state change on

***************************************/

void Burger::Flash::BaseExecute::ExecuteState(Flash::CharacterObject* /* pObject */)
{
}

/*! ************************************

	\brief Execute state change based in frame number

	Revert a state change that was a specific frame in the movie

	\param pObject Pointer to an object to perform the state change on
	\param uFrame Frame number to base the state change on

***************************************/

void Burger::Flash::BaseExecute::ExecuteStateReverse(Flash::CharacterObject *pObject,Word /* uFrame */)
{
	// Default behavior is apply the generic state change
	ExecuteState(pObject); 
}

/*! ************************************

	\brief Get the depth value and ID

	If there is an ID, return the depth of recursion in the upper
	16 bit and the ID in the lower 16 bits.

	Return \ref BURGER_MAXUINT this value is not applicable.

	\return Depth in the upper 16 bits, ID in the lower 16 bits or \ref BURGER_MAXUINT

***************************************/

Word32 Burger::Flash::BaseExecute::GetDepthIDOfReplaceOrAddTag(void) const 
{
	return BURGER_MAXUINT; 
}

/*! ************************************

	\var const Burger::StaticRTTI Burger::Flash::BaseExecute::g_StaticRTTI
	\brief The global description of the class

	This record contains the name of this class and a
	reference to the parent

***************************************/