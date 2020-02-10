/***************************************

	Base display context class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRRENDERER_H__
#define __BRRENDERER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRRECT_H__
#include "brrect.h"
#endif

#ifndef __BRREZFILE_H__
#include "brrezfile.h"
#endif

/* BEGIN */
namespace Burger {
class Renderer {
public:
	static const Word ByteSquareTable[255+256];					///< Table of squares from -255 to 255
	BURGER_ALIGN(static const Word8 RGB2ToRGB3Table[4],4);		///< Table to convert 2 bit color to 3 bit color
	BURGER_ALIGN(static const Word8 RGB2ToRGB4Table[4],4);		///< Table to convert 2 bit color to 4 bit color
	BURGER_ALIGN(static const Word8 RGB2ToRGB5Table[4],4);		///< Table to convert 2 bit color to 5 bit color
	BURGER_ALIGN(static const Word8 RGB2ToRGB6Table[4],4);		///< Table to convert 2 bit color to 6 bit color
	BURGER_ALIGN(static const Word8 RGB2ToRGB8Table[4],4);		///< Table to convert 2 bit color to 8 bit color
	BURGER_ALIGN(static const Word8 RGB3ToRGB4Table[8],8);		///< Table to convert 3 bit color to 4 bit color
	BURGER_ALIGN(static const Word8 RGB3ToRGB5Table[8],8);		///< Table to convert 3 bit color to 5 bit color
	BURGER_ALIGN(static const Word8 RGB3ToRGB6Table[8],8);		///< Table to convert 3 bit color to 6 bit color
	BURGER_ALIGN(static const Word8 RGB3ToRGB8Table[8],8);		///< Table to convert 3 bit color to 8 bit color
	BURGER_ALIGN(static const Word8 RGB4ToRGB5Table[16],16);	///< Table to convert 4 bit color to 5 bit color
	BURGER_ALIGN(static const Word8 RGB4ToRGB6Table[16],16);	///< Table to convert 4 bit color to 6 bit color
	BURGER_ALIGN(static const Word8 RGB4ToRGB8Table[16],16);	///< Table to convert 4 bit color to 8 bit color
	BURGER_ALIGN(static const Word8 RGB5ToRGB6Table[32],16);	///< Table to convert 5 bit color to 6 bit color
	BURGER_ALIGN(static const Word8 RGB5ToRGB8Table[32],16);	///< Table to convert 5 bit color to 8 bit color
	BURGER_ALIGN(static const Word8 RGB6ToRGB8Table[64],16);	///< Table to convert 6 bit color to 8 bit color
	Renderer(void);
	virtual ~Renderer();
	virtual Word Init(Word uWidth,Word uHeight,Word uDepth,Word uFlags);
    virtual void Shutdown(void);
	virtual void BeginScene(void);
	virtual void EndScene(void);
	virtual void Draw8BitPixels(int iX,int iY,Word uWidth,Word uHeight,WordPtr uStride,const Word8 *pPixels);
	virtual void Draw8BitPixelsMasked(int iX,int iY,Word uWidth,Word uHeight,WordPtr uStride,const Word8 *pPixels);
	virtual void DrawPixel(int iX,int iY,Word uColorIndex);
	virtual void DrawRect(int iX,int iY,Word uWidth,Word uHeight,Word uColorIndex);
	virtual void Clear(Word uColorIndex);
protected:
	virtual void UpdateClip(void);
	Rect_t m_Clip;			///< Bounds rectangle for clipping
	Display *m_pDisplay;	///< Attached display
	void *m_pFrameBuffer;	///< Pointer to the software frame buffer (If supported)
	WordPtr m_uStride;		///< Width in BYTES of the display buffer (For software rendering)
	Word m_uWidth;			///< Width in pixels of the display buffer
	Word m_uHeight;			///< Height in pixels of the display buffer
	Word m_uDepth;			///< Depth in bits of the display buffer
	Word m_uFlags;			///< Flags for describing available features
public:
	BURGER_INLINE Display *GetDisplay(void) const { return m_pDisplay; }
	BURGER_INLINE void SetDisplay(Display *pDisplay) { m_pDisplay = pDisplay; }
	BURGER_INLINE void *GetFrameBuffer(void) const { return m_pFrameBuffer; }
	BURGER_INLINE void SetFrameBuffer(void *pFrameBuffer) { m_pFrameBuffer = pFrameBuffer; }
	BURGER_INLINE WordPtr GetStride(void) const { return m_uStride; }
	BURGER_INLINE void SetStride(WordPtr uStride) { m_uStride = uStride; }
	BURGER_INLINE Word GetWidth(void) const { return m_uWidth; }
	BURGER_INLINE Word GetHeight(void) const { return m_uHeight; }
	BURGER_INLINE Word GetDepth(void) const { return m_uDepth; }
	BURGER_INLINE Word GetFlags(void) const { return m_uFlags; }
	BURGER_INLINE const Rect_t *GetClip(void) const { return &m_Clip; }
	void GetClip(Rect_t *pOutput) const;
	void SetClip(const Rect_t *pInput);
	void SetClip(int iLeft,int iTop,int iRight,int iBottom);
	void DrawShape8Bit(int iX,int iY,RezFile *pRezFile,Word uRezNum);
	void DrawShape8BitCentered(RezFile *pRezFile,Word uRezNum);
	void DrawShape8BitMasked(int iX,int iY,RezFile *pRezFile,Word uRezNum);
	void DrawShape8BitMaskedCentered(RezFile *pRezFile,Word uRezNum);
};
}
/* END */

#endif
