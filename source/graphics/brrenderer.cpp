/***************************************

	Base display context class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brrenderer.h"
#include "brshape8bit.h"

/*! ************************************

	\class Burger::Renderer
	\brief Base graphics renderer class

	The Display class manages a video display, however, the actual
	method of rendering graphics is handled by the Renderer class.
	This disconnect was needed since Display classes are tied to
	the underlying API and software rendering is not.

	Renderer classes are directly tied to the API they use
	to inform the operating system (if any) on how to 
	draw the requested graphics. Some examples are DirectX 9,
	DirectX 10, OpenGL and software rendering. APIs can
	be chained where a software rendering context sits
	on top of an OpenGL context so the final result is
	drawn using OpenGL while the application renders
	exclusively in software.

	\sa Display, DisplayOpenGL, RendererOpenGL, DisplayOpenGLSoftware8 and RendererSoftware8

***************************************/

/*! ************************************

	Used for some routines to create a square to quickly
	generate a color match. Can be used by other routines
	for their own purposes. Only used for CPUs where integer
	multiplies are slow such as the PowerPC.

	\code
		int Color = 0;		// Value from -255 to 255
		Word uSquare = Renderer::ByteSquareTable[Color+255];	// Square
	\endcode

	\sa Burger::Palette::FindColorIndex()

***************************************/

const Word Burger::Renderer::ByteSquareTable[255+256] = {
	65025,64516,64009,63504,63001,62500,62001,61504,	// -255 to -248
	61009,60516,60025,59536,59049,58564,58081,57600,	// -247 to -240
	57121,56644,56169,55696,55225,54756,54289,53824,	// -239 to -232
	53361,52900,52441,51984,51529,51076,50625,50176,	// -231 to -224
	49729,49284,48841,48400,47961,47524,47089,46656,	// -223 to -216
	46225,45796,45369,44944,44521,44100,43681,43264,	// -215 to -208
	42849,42436,42025,41616,41209,40804,40401,40000,	// -207 to -200
	39601,39204,38809,38416,38025,37636,37249,36864,	// -199 to -192
	36481,36100,35721,35344,34969,34596,34225,33856,	// -191 to -184
	33489,33124,32761,32400,32041,31684,31329,30976,	// -183 to -176
	30625,30276,29929,29584,29241,28900,28561,28224,	// -175 to -168
	27889,27556,27225,26896,26569,26244,25921,25600,	// -167 to -160
	25281,24964,24649,24336,24025,23716,23409,23104,	// -159 to -152
	22801,22500,22201,21904,21609,21316,21025,20736,	// -151 to -144
	20449,20164,19881,19600,19321,19044,18769,18496,	// -143 to -136
	18225,17956,17689,17424,17161,16900,16641,16384,	// -135 to -128
	16129,15876,15625,15376,15129,14884,14641,14400,	// -127 to -120
	14161,13924,13689,13456,13225,12996,12769,12544,	// -119 to -112
	12321,12100,11881,11664,11449,11236,11025,10816,	// -111 to -104
	10609,10404,10201,10000, 9801, 9604, 9409, 9216,	// -103 to -96
	 9025, 8836, 8649, 8464, 8281, 8100, 7921, 7744,	// -95 to -88
	 7569, 7396, 7225, 7056, 6889, 6724, 6561, 6400,	// -87 to -80
	 6241, 6084, 5929, 5776, 5625, 5476, 5329, 5184,	// -79 to -72
	 5041, 4900, 4761, 4624, 4489, 4356, 4225, 4096,	// -71 to -64
	 3969, 3844, 3721, 3600, 3481, 3364, 3249, 3136,	// -63 to -56
	 3025, 2916, 2809, 2704, 2601, 2500, 2401, 2304,	// -55 to -48
	 2209, 2116, 2025, 1936, 1849, 1764, 1681, 1600,	// -47 to -40
	 1521, 1444, 1369, 1296, 1225, 1156, 1089, 1024,	// -39 to -32
	  961,  900,  841,  784,  729,  676,  625,  576,	// -31 to -24
	  529,  484,  441,  400,  361,  324,  289,  256,	// -23 to -16
	  225,  196,  169,  144,  121,  100,   81,   64,	// -15 to -8
	   49,   36,   25,   16,    9,    4,    1,    0,	// -7 to 0
	    1,    4,    9,   16,   25,   36,   49,   64,	// 1 to 8
	   81,  100,  121,  144,  169,  196,  225,  256,	// 9 to 16
	  289,  324,  361,  400,  441,  484,  529,  576,	// 17 to 24
	  625,  676,  729,  784,  841,  900,  961, 1024,	// 25 to 32
	 1089, 1156, 1225, 1296, 1369, 1444, 1521, 1600,	// 33 to 40
	 1681, 1764, 1849, 1936, 2025, 2116, 2209, 2304,	// 41 to 48
	 2401, 2500, 2601, 2704, 2809, 2916, 3025, 3136,	// 49 to 56
	 3249, 3364, 3481, 3600, 3721, 3844, 3969, 4096,	// 57 to 64
	 4225, 4356, 4489, 4624, 4761, 4900, 5041, 5184,	// 65 to 72
	 5329, 5476, 5625, 5776, 5929, 6084, 6241, 6400,	// 73 to 80
	 6561, 6724, 6889, 7056, 7225, 7396, 7569, 7744,	// 81 to 88
	 7921, 8100, 8281, 8464, 8649, 8836, 9025, 9216,	// 89 to 96
	 9409, 9604, 9801,10000,10201,10404,10609,10816,	// 97 to 104
	11025,11236,11449,11664,11881,12100,12321,12544,	// 105 to 112
	12769,12996,13225,13456,13689,13924,14161,14400,	// 113 to 120
	14641,14884,15129,15376,15625,15876,16129,16384,	// 121 to 128
	16641,16900,17161,17424,17689,17956,18225,18496,	// 129 to 136
	18769,19044,19321,19600,19881,20164,20449,20736,	// 137 to 144
	21025,21316,21609,21904,22201,22500,22801,23104,	// 145 to 152
	23409,23716,24025,24336,24649,24964,25281,25600,	// 153 to 160
	25921,26244,26569,26896,27225,27556,27889,28224,	// 161 to 168
	28561,28900,29241,29584,29929,30276,30625,30976,	// 169 to 176
	31329,31684,32041,32400,32761,33124,33489,33856,	// 177 to 184
	34225,34596,34969,35344,35721,36100,36481,36864,	// 185 to 192
	37249,37636,38025,38416,38809,39204,39601,40000,	// 193 to 200
	40401,40804,41209,41616,42025,42436,42849,43264,	// 201 to 208
	43681,44100,44521,44944,45369,45796,46225,46656,	// 209 to 216
	47089,47524,47961,48400,48841,49284,49729,50176,	// 217 to 224
	50625,51076,51529,51984,52441,52900,53361,53824,	// 225 to 232
	54289,54756,55225,55696,56169,56644,57121,57600,	// 233 to 240
	58081,58564,59049,59536,60025,60516,61009,61504,	// 241 to 248
	62001,62500,63001,63504,64009,64516,65025};			// 249 to 255

// Docs in the header

BURGER_ALIGN(const Word8 Burger::Renderer::RGB2ToRGB3Table[4],4) = {0x00,0x02,0x05,0x07};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB2ToRGB4Table[4],4) = {0x00,0x05,0x0A,0x0F};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB2ToRGB5Table[4],4) = {0x00,0x0A,0x15,0x1F};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB2ToRGB6Table[4],4) = {0x00,0x15,0x2A,0x3F};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB2ToRGB8Table[4],4) = {0x00,0x55,0xAA,0xFF};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB3ToRGB4Table[8],8) = {0x00,0x02,0x04,0x06,0x09,0x0B,0x0D,0x0F};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB3ToRGB5Table[8],8) = {0x00,0x04,0x09,0x0D,0x12,0x16,0x1B,0x1F};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB3ToRGB6Table[8],8) = {0x00,0x09,0x12,0x1B,0x24,0x2D,0x36,0x3F};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB3ToRGB8Table[8],8) = {0x00,0x24,0x49,0x6D,0x92,0xB6,0xDB,0xFF};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB4ToRGB5Table[16],16) = {
	0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,
	0x11,0x13,0x15,0x17,0x19,0x1B,0x1D,0x1F};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB4ToRGB6Table[16],16) = {
	0x00,0x04,0x08,0x0D,0x11,0x15,0x19,0x1D,
	0x22,0x26,0x2A,0x2E,0x32,0x37,0x3B,0x3F};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB4ToRGB8Table[16],16) = {
	0x00,0x11,0x22,0x33,0x44,0x55,0x66,0x77,
	0x88,0x99,0xAA,0xBB,0xCC,0xDD,0xEE,0xFF};
BURGER_ALIGN(const Word8 Burger::Renderer::RGB5ToRGB6Table[32],16) = {
	0x00,0x02,0x04,0x06,0x08,0x0A,0x0C,0x0E,
	0x10,0x12,0x14,0x16,0x18,0x1A,0x1C,0x1E,
	0x21,0x23,0x25,0x27,0x29,0x2B,0x2D,0x2F,
	0x31,0x33,0x35,0x37,0x39,0x3B,0x3D,0x3F};

/*! ************************************

	\var Burger::Renderer::RGB5ToRGB8Table[32]

	Scale 0-31 into 0-255 in a linear fashion.
	The formula is Result = ((i*0xFFFF)/31)>>8

	\sa Burger::Palette::FromRGB15()

***************************************/

BURGER_ALIGN(const Word8 Burger::Renderer::RGB5ToRGB8Table[32],16) = {
	0x00,0x08,0x10,0x18,0x21,0x29,0x31,0x39,
	0x42,0x4A,0x52,0x5A,0x63,0x6B,0x73,0x7B,
	0x84,0x8C,0x94,0x9C,0xA5,0xAD,0xB5,0xBD,
	0xC6,0xCE,0xD6,0xDE,0xE7,0xEF,0xF7,0xFF};

/*! ************************************

	\var Burger::Renderer::RGB6ToRGB8Table[64]

	Scale 0-63 into 0-255 in a linear fashion.
	The formula is Result = ((i*0xFFFF)/63)>>8

	\sa Burger::Palette::RGB16ToRGB24()

***************************************/

BURGER_ALIGN(const Word8 Burger::Renderer::RGB6ToRGB8Table[64],16) = {
	0x00,0x04,0x08,0x0C,0x10,0x14,0x18,0x1C,
	0x20,0x24,0x28,0x2C,0x30,0x34,0x38,0x3C,
	0x41,0x45,0x49,0x4D,0x51,0x55,0x59,0x5D,
	0x61,0x65,0x69,0x6D,0x71,0x75,0x79,0x7D,
	0x82,0x86,0x8A,0x8E,0x92,0x96,0x9A,0x9E,
	0xA2,0xA6,0xAA,0xAE,0xB2,0xB6,0xBA,0xBE,
	0xC3,0xC7,0xCB,0xCF,0xD3,0xD7,0xDB,0xDF,
	0xE3,0xE7,0xEB,0xEF,0xF3,0xF7,0xFB,0xFF};

/*! ************************************

	\brief Default constructor.
	
	Initializes all of the shared variables
	
***************************************/

Burger::Renderer::Renderer(void) :
	m_pDisplay(NULL),
	m_pFrameBuffer(NULL),
	m_uStride(0),
	m_uWidth(0),
	m_uHeight(0),
	m_uDepth(0),
	m_uFlags(0)
{
	m_Clip.Clear();
}

/*! ************************************

	\brief Default destructor.
	
	Clears out the shared variables and calls the
	chained function Renderer::Shutdown() to 
	release all resources.

	\sa Renderer::Shutdown()
	
***************************************/

Burger::Renderer::~Renderer()
{
	Shutdown();
}

/*! ************************************

	\brief Initialize a renderer
	
	This is called when a Display is given a
	Renderer class to set up internal
	variables describing the parameters of the
	current display
	
	\param uWidth Width in pixels of the display to render to
	\param uHeight Height in pixels of the display to render to
	\param uDepth Depth in bits of the display to render to
	\param uFlags Bit flags to describe specific display features
	\return Zero if no error occurred, non-zero if the renderer could not initialize

***************************************/

Word Burger::Renderer::Init(Word uWidth,Word uHeight,Word uDepth,Word uFlags)
{
	m_uWidth = uWidth;
	m_uHeight = uHeight;
	m_uDepth = uDepth;
	m_uFlags = uFlags;
	m_Clip.Set(0,0,static_cast<int>(uWidth),static_cast<int>(uHeight));
	return 0;
}

/*! ************************************

	\brief Shut down a renderer

	Release all resources allocated by the renderer

***************************************/

void Burger::Renderer::Shutdown(void)
{
}

/*! ************************************

	\brief Start up a renderer for a frame

	Before a frame is started, the renderer may need to initialize
	internal states for the underlying API or hardware. This function 
	will perform these actions

***************************************/

void Burger::Renderer::BeginScene(void)
{
}

/*! ************************************

	\brief Wrap up frame rendering

	After a frame is started, the renderer may need to wrap up
	some internal states and present cache or queued render
	events to the hardware. This function will perform these
	actions.

***************************************/

void Burger::Renderer::EndScene(void)
{
}

/*! ************************************

	\brief Draw 8 bit data onto the software buffer

	Draw an 8 bit per pixel image to the screen clipped
	to the current clip rect.

	\param iX X coordinate to draw the image
	\param iY Y coordinate to draw the image
	\param uWidth Width of the image in bytes
	\param uHeight Height of the image in bytes
	\param uStride Bytes per scan line of the image
	\param pPixels Base pointer to the bit map to draw
	
	\sa Renderer::Draw8BitPixelsMasked()

***************************************/

void Burger::Renderer::Draw8BitPixels(int /* iX */,int /* iY */,Word /* uWidth */,Word /* uHeight */,WordPtr /* uStride */,const Word8 * /* pPixels */)
{
}

/*! ************************************

	\brief Draw 8 bit data onto the software buffer

	Draw an 8 bit per pixel image to the screen without 
	drawing any pixels whose value are zero.
	The image is clipped to the current clip rect.

	\param iX X coordinate to draw the image
	\param iY Y coordinate to draw the image
	\param uWidth Width of the image in bytes
	\param uHeight Height of the image in bytes
	\param uStride Bytes per scan line of the image
	\param pPixels Base pointer to the bit map to draw

	\sa Renderer::Draw8BitPixels()
	
***************************************/

void Burger::Renderer::Draw8BitPixelsMasked(int /* iX */,int /* iY */,Word /* uWidth */,Word /* uHeight */,WordPtr /* uStride */,const Word8 * /* pPixels */)
{
}


/*! ************************************

	\brief Draw an 8 bit pixel onto the software buffer

	Draw an 8 bit pixel to the screen by taking the 8 bit index and
	looking it up in the current palette.

	The pixel is clipped to the current clip rect.

	\param iX X coordinate to draw the pixel
	\param iY Y coordinate to draw the pixel
	\param uColorIndex Color to draw (Assumed to be an index to the active palette)
	
***************************************/

void Burger::Renderer::DrawPixel(int /* iX */,int /* iY */,Word /* uColorIndex */)
{
}

/*! ************************************

	\brief Draw 8 bit color rectangle

	Draw a solid 8 bit colored rectangle to the screen.
	The pixel is clipped to the current clip rect.

	\param iX X coordinate of the leftmost line
	\param iY Y coordinate of the topmost line
	\param uWidth Width of the rectangle in pixels
	\param uHeight Height of the rectangle in pixels
	\param uColorIndex Color to draw (Assumed to be an index to the active palette)
	
***************************************/

void Burger::Renderer::DrawRect(int /* iX */,int /* iY */,Word /* uWidth */,Word /* uHeight */,Word /* uColorIndex */)
{
}

/*! ************************************

	\brief Clear the entire screen to a solid color

	Using a color lookup index, get the color from the current palette
	and fill the screen with this color.

	\param uColorIndex Color to draw (Assumed to be an index to the active palette)

***************************************/

void Burger::Renderer::Clear(Word uColorIndex)
{
	DrawRect(0,0,m_uWidth,m_uHeight,uColorIndex);
}

/*! ************************************

	\brief Update the clip rect to the operating system API

	Some render targets need to have the local clip rect passed to the
	operating system API, this function will perform this update.

	\note This is an internal function that is called whenever
	the clip rect is updated.

	\sa SetClip(const Rect_t *) or SetClip(int,int,int,int)

***************************************/

void Burger::Renderer::UpdateClip(void)
{
}

/*! ************************************

	\fn Burger::Renderer::GetDisplay(void) const
	\brief Get the pointer to the attached video display.

	A renderer needs a connection to a video display to obtain context information
	of the display for some graphics APIs.

	\note This value can be \ref NULL on if a Display was never attached
	
	\return Pointer to the attached Display
	\sa SetDisplay()

***************************************/

/*! ************************************

	\fn Burger::Renderer::SetDisplay(Display *)
	\brief Set the pointer to the attached display

	A renderer needs a connection to a video display to obtain context information
	of the display for some graphics APIs.
	
	\param pDisplay Pointer to a Display to attach to this Renderer or \ref NULL to disconnect
	\sa GetDisplay()

***************************************/

/*! ************************************

	\fn Burger::Renderer::GetFrameBuffer(void) const
	\brief Get the base pointer to the frame buffer.

	When software rendering is requested, a frame buffer is 
	allocated/created and a pointer and stride are
	created to allow software rendering functions to
	manipulate the buffer to update the game screen.

	\note This value can be \ref NULL on renderers that don't use a frame buffer like OpenGL
	
	\return Base pointer to a buffer that is Renderer::m_uStride * Renderer::m_uHeight in bytes
	\sa SetFrameBuffer()

***************************************/

/*! ************************************

	\fn Burger::Renderer::SetFrameBuffer(void *)
	\brief Set the base pointer to the frame buffer.

	When software rendering is requested, a frame buffer is 
	allocated/created and a pointer and stride are
	created to allow software rendering functions to
	manipulate the buffer to update the game screen.
	
	\param pFrameBuffer Base pointer to a buffer that is Renderer::m_uStride * Renderer::m_uHeight in bytes
	\sa GetFrameBuffer()

***************************************/

/*! ************************************

	\fn Burger::Renderer::GetStride(void) const
	\brief Get the width in bytes of the display buffer scan line

	\note This value can be zero on renderers that don't use a frame buffer like OpenGL
	
	\return Width in bytes of a single display buffer scan line
	\sa SetStride()

***************************************/

/*! ************************************

	\fn Burger::Renderer::SetStride(WordPtr)
	\brief Set the width in bytes of the display buffer scan line
	
	\note This value is only used in software renderers

	\param uStride Width in bytes of a single display buffer scan line
	\sa GetStride()
	
***************************************/

/*! ************************************

	\fn Burger::Renderer::GetWidth(void) const
	\brief Get the width in pixels for the rendering context

	\note This is not used for clipping, see SetClip(const Rect_t *) for clipping
	
	\return Width in pixels of a single display buffer scan line
	\sa GetHeight(), GetDepth(), GetFlags() or GetStride()

***************************************/

/*! ************************************

	\fn Burger::Renderer::GetHeight(void) const
	\brief Get the height in pixels for the rendering context

	\note This is not used for clipping, see SetClip(const Rect_t *) for clipping
	
	\return Height in pixels of a single display buffer
	\sa SetClip(const Rect_t *), GetWidth(), GetDepth(), GetFlags() or GetStride()

***************************************/

/*! ************************************

	\fn Burger::Renderer::GetDepth(void) const
	\brief Get the depth in bits for this renderer
	
	\return Depth in bits for the renderer, usually 8, 15, 16, 24 or 32
	\sa GetWidth(), GetHeight(), GetFlags() or GetStride()

***************************************/

/*! ************************************

	\fn Burger::Renderer::GetFlags(void) const
	\brief Get the state flags for the renderer
	
	\return Bit flags from the Display class determining modes available for rendering
	\sa GetWidth(), GetHeight(), GetDepth() or GetStride()

***************************************/

/*! ************************************

	\fn const Rect_t * Burger::Renderer::GetClip(void) const
	\brief Return the current clip rectangle

	Drawing is clipped to this bounds rectangle. On screen
	initialization, it's set to the bounds of the entire screen.
	At runtime, the bounds rectangle can be changed to all draw
	functions that allow clipping will draw to whatever
	rectangle is used at the time.

	Do not modify the rectangle directly. Call SetClip()
	so that internal variables can be updated if needed
	when changing the clip rectangle.

	\return Pointer to the read-only current clip rectangle.

	\sa SetClip(const Rect_t *) or SetClip(int,int,int,int)

***************************************/

/*! ************************************

	\brief Return the current clip rectangle

	Drawing is clipped to this bounds rectangle. On screen
	initialization, it's set to the bounds of the entire screen.
	At runtime, the bounds rectangle can be changed to all draw
	functions that allow clipping will draw to whatever
	rectangle is used at the time.

	Do not modify the rectangle directly. Call SetClip()
	so that internal variables can be updated if needed
	when changing the clip rectangle.

	\param pOutput Pointer to the Rect_t to store a copy of the current clip rectangle.

	\sa SetClip(const Rect_t *) or SetClip(int,int,int,int)

***************************************/

void Burger::Renderer::GetClip(Rect_t *pOutput) const
{
	pOutput[0] = m_Clip;
}

/*! ************************************

	\brief Set the current clip rectangle

	Drawing is clipped to this bounds rectangle. On screen
	initialization, it's set to the bounds of the entire screen.
	At runtime, the bounds rectangle can be changed to all draw
	functions that allow clipping will draw to whatever
	rectangle is used at the time.

	\param pInput Pointer to the new clip rectangle

	\sa GetClip() or SetClip(int,int,int,int)

***************************************/

void Burger::Renderer::SetClip(const Rect_t *pInput)
{
	m_Clip = pInput[0];
	UpdateClip();
}

/*! ************************************

	\brief Set the current clip rectangle

	Drawing is clipped to this bounds rectangle. On screen
	initialization, it's set to the bounds of the entire screen.
	At runtime, the bounds rectangle can be changed to all draw
	functions that allow clipping will draw to whatever
	rectangle is used at the time.

	\param iLeft Left most point of the clip rectangle
	\param iTop Top most point of the clip rectangle
	\param iRight Right most point of the clip rectangle
	\param iBottom Bottom most point of the clip rectangle

	\sa GetClip() or SetClip(const Rect_t *)

***************************************/

void Burger::Renderer::SetClip(int iLeft,int iTop,int iRight,int iBottom)
{
	m_Clip.Set(iLeft,iTop,iRight,iBottom);
	UpdateClip();
}

/*! ************************************

	\brief Draw a Shape8Bit_t image from a resource file

	Given a Shape8Bit_t data image stored in a resource file, draw it
	to the screen

	\param iX X coordinate of the leftmost line
	\param iY Y coordinate of the topmost line
	\param pRezFile Pointer to a valid rez file
	\param uRezNum Resource number to the data chunk in the rez file

***************************************/

void Burger::Renderer::DrawShape8Bit(int iX,int iY,RezFile *pRezFile,Word uRezNum)
{
	// Load the resource
	const Shape8Bit_t *pShape = Shape8Bit_t::Load(pRezFile,uRezNum);
	if (pShape) {
		pShape->Draw(this,iX,iY);
		pRezFile->Release(uRezNum);
	}
}

/*! ************************************

	\brief Draw a Shape8Bit_t image from a resource file centered on the screen

	Given a Shape8Bit_t data image stored in a resource file, draw it
	to the center of the screen

	\param pRezFile Pointer to a valid rez file
	\param uRezNum Resource number to the data chunk in the rez file

***************************************/

void Burger::Renderer::DrawShape8BitCentered(RezFile *pRezFile,Word uRezNum)
{
	// Load the resource
	const Shape8Bit_t *pShape = Shape8Bit_t::Load(pRezFile,uRezNum);	
	if (pShape) {
		// Center X
		int iX = static_cast<int>((m_uWidth-pShape->GetHeight())/2U);
		// Center Y
		int iY = static_cast<int>((m_uHeight-pShape->GetHeight())/2U);
		pShape->Draw(this,iX,iY);
		pRezFile->Release(uRezNum);
	}
}

/*! ************************************

	\brief Draw a masked Shape8Bit_t image from a resource file

	Given a masked Shape8Bit_t data image stored in a resource file, draw it
	to the screen

	\param iX X coordinate of the leftmost line
	\param iY Y coordinate of the topmost line
	\param pRezFile Pointer to a valid rez file
	\param uRezNum Resource number to the data chunk in the rez file

***************************************/

void Burger::Renderer::DrawShape8BitMasked(int iX,int iY,RezFile *pRezFile,Word uRezNum)
{
	// Load the resource
	const Shape8Bit_t *pShape = Shape8Bit_t::Load(pRezFile,uRezNum);
	if (pShape) {
		pShape->DrawMasked(this,iX,iY);
		pRezFile->Release(uRezNum);
	}
}

/*! ************************************

	\brief Draw a masked Shape8Bit_t image from a resource file centered on the screen

	Given a masked Shape8Bit_t data image stored in a resource file, draw it
	to the center of the screen

	\param pRezFile Pointer to a valid rez file
	\param uRezNum Resource number to the data chunk in the rez file

***************************************/

void Burger::Renderer::DrawShape8BitMaskedCentered(RezFile *pRezFile,Word uRezNum)
{
	// Load the resource
	const Shape8Bit_t *pShape = Shape8Bit_t::Load(pRezFile,uRezNum);
	if (pShape) {
		// Center X
		int iX = static_cast<int>((m_uWidth-pShape->GetHeight())/2U);
		// Center Y
		int iY = static_cast<int>((m_uHeight-pShape->GetHeight())/2U);
		pShape->DrawMasked(this,iX,iY);
		pRezFile->Release(uRezNum);
	}
}
