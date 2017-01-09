/***************************************

	Palette manager class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRPALETTE_H__
#define __BRPALETTE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRDISPLAY_H__
#include "brdisplay.h"
#endif

#ifndef __BRVECTOR3D_H__
#include "brvector3d.h"
#endif

#ifndef __BRVECTOR4D_H__
#include "brvector4d.h"
#endif

/* BEGIN */
namespace Burger {
struct RGBWord8_t {
	Word8 m_uRed;	///< 8 bit red value
	Word8 m_uGreen;	///< 8 bit green value
	Word8 m_uBlue;	///< 8 bit blue value
	static const RGBWord8_t Aqua;
	static const RGBWord8_t Black;
	static const RGBWord8_t Blue;
	static const RGBWord8_t Fuchsia;
	static const RGBWord8_t Gray;
	static const RGBWord8_t Green;
	static const RGBWord8_t Lime;
	static const RGBWord8_t Maroon;
	static const RGBWord8_t Navy;
	static const RGBWord8_t Olive;
	static const RGBWord8_t Orange;
	static const RGBWord8_t Purple;
	static const RGBWord8_t Red;
	static const RGBWord8_t Silver;
	static const RGBWord8_t Teal;
	static const RGBWord8_t White;
	static const RGBWord8_t Yellow;
	BURGER_INLINE Word32 ToRGB(void) const { return (static_cast<Word32>(m_uRed)<<16U) + (static_cast<Word32>(m_uGreen)<<8U) + m_uBlue; }
	BURGER_INLINE Word32 ToBGR(void) const { return (static_cast<Word32>(m_uBlue)<<16U) + (static_cast<Word32>(m_uGreen)<<8U) + m_uRed; }
	BURGER_INLINE void FromRGB(Word32 uColor) { m_uRed = static_cast<Word8>(uColor>>16U); m_uGreen = static_cast<Word8>(uColor>>8U); m_uBlue = static_cast<Word8>(uColor); }
	BURGER_INLINE void FromBGR(Word32 uColor) { m_uRed = static_cast<Word8>(uColor); m_uGreen = static_cast<Word8>(uColor>>8U); m_uBlue = static_cast<Word8>(uColor>>16U); }
	void BURGER_API Interpolate(const RGBWord8_t *pFrom,const RGBWord8_t *pTo,float fFactor);
};
struct RGBAWord8_t {
	Word8 m_uRed;	///< 8 bit red value
	Word8 m_uGreen;	///< 8 bit green value
	Word8 m_uBlue;	///< 8 bit blue value
	Word8 m_uAlpha;	///< 8 bit alpha value
	static const RGBAWord8_t Aqua;
	static const RGBAWord8_t Black;
	static const RGBAWord8_t Blue;
	static const RGBAWord8_t Fuchsia;
	static const RGBAWord8_t Gray;
	static const RGBAWord8_t Green;
	static const RGBAWord8_t Lime;
	static const RGBAWord8_t Maroon;
	static const RGBAWord8_t Navy;
	static const RGBAWord8_t Olive;
	static const RGBAWord8_t Orange;
	static const RGBAWord8_t Purple;
	static const RGBAWord8_t Red;
	static const RGBAWord8_t Silver;
	static const RGBAWord8_t Teal;
	static const RGBAWord8_t White;
	static const RGBAWord8_t Yellow;
	BURGER_INLINE Word32 ToARGB(void) const { return (static_cast<Word32>(m_uAlpha)<<24U) + (static_cast<Word32>(m_uRed)<<16U) + (static_cast<Word32>(m_uGreen)<<8U) + m_uBlue; }
	BURGER_INLINE Word32 ToABGR(void) const { return (static_cast<Word32>(m_uAlpha)<<24U) + (static_cast<Word32>(m_uBlue)<<16U) + (static_cast<Word32>(m_uGreen)<<8U) + m_uRed; }
	BURGER_INLINE void FromARGB(Word32 uColor) { m_uRed = static_cast<Word8>(uColor>>16U); m_uGreen = static_cast<Word8>(uColor>>8U); m_uBlue = static_cast<Word8>(uColor); m_uAlpha = static_cast<Word8>(uColor>>24U); }
	BURGER_INLINE void FromABGR(Word32 uColor) { m_uRed = static_cast<Word8>(uColor); m_uGreen = static_cast<Word8>(uColor>>8U); m_uBlue = static_cast<Word8>(uColor>>16U); m_uAlpha = static_cast<Word8>(uColor>>24U); }
	void BURGER_API Interpolate(const RGBAWord8_t *pFrom,const RGBAWord8_t *pTo,float fFactor);
};
struct RGBWord16_t {
	Word16 m_uRed;		///< 16 bit red value
	Word16 m_uGreen;	///< 16 bit green value
	Word16 m_uBlue;		///< 16 bit blue value
};
struct RGBAWord16_t {
	Word16 m_uRed;		///< 16 bit red value
	Word16 m_uGreen;	///< 16 bit green value
	Word16 m_uBlue;		///< 16 bit blue value
	Word16 m_uAlpha;	///< 16 bit alpha value
};
struct RGBFloat_t {
	float m_fRed;	///< 32 bit float red value
	float m_fGreen;	///< 32 bit float green value
	float m_fBlue;	///< 32 bit float blue value
	static const RGBFloat_t Aqua;
	static const RGBFloat_t Black;
	static const RGBFloat_t Blue;
	static const RGBFloat_t Fuchsia;
	static const RGBFloat_t Gray;
	static const RGBFloat_t Green;
	static const RGBFloat_t Lime;
	static const RGBFloat_t Maroon;
	static const RGBFloat_t Navy;
	static const RGBFloat_t Olive;
	static const RGBFloat_t Orange;
	static const RGBFloat_t Purple;
	static const RGBFloat_t Red;
	static const RGBFloat_t Silver;
	static const RGBFloat_t Teal;
	static const RGBFloat_t White;
	static const RGBFloat_t Yellow;
	BURGER_INLINE operator Vector3D_t &() { return *reinterpret_cast<Vector3D_t *>(this); }
	BURGER_INLINE operator const Vector3D_t &() const { return *reinterpret_cast<const Vector3D_t *>(this); }
	BURGER_INLINE RGBFloat_t &operator = (const Vector3D_t &rInput) { *this = *reinterpret_cast<const RGBFloat_t *>(&rInput); return *this; }
	void BURGER_API HSLInterpolate(const RGBFloat_t *pInput1,const RGBFloat_t *pInput2,float fFactor,Word bDirection=FALSE);
};
struct RGBAFloat_t {
	float m_fRed;	///< 32 bit float red value
	float m_fGreen;	///< 32 bit float green value
	float m_fBlue;	///< 32 bit float blue value
	float m_fAlpha;	///< 32 bit float alpha value
	static const RGBAFloat_t Aqua;
	static const RGBAFloat_t Black;
	static const RGBAFloat_t Blue;
	static const RGBAFloat_t Fuchsia;
	static const RGBAFloat_t Gray;
	static const RGBAFloat_t Green;
	static const RGBAFloat_t Lime;
	static const RGBAFloat_t Maroon;
	static const RGBAFloat_t Navy;
	static const RGBAFloat_t Olive;
	static const RGBAFloat_t Orange;
	static const RGBAFloat_t Purple;
	static const RGBAFloat_t Red;
	static const RGBAFloat_t Silver;
	static const RGBAFloat_t Teal;
	static const RGBAFloat_t White;
	static const RGBAFloat_t Yellow;
	BURGER_INLINE operator Vector4D_t &() { return *reinterpret_cast<Vector4D_t *>(this); }
	BURGER_INLINE operator const Vector4D_t &() const { return *reinterpret_cast<const Vector4D_t *>(this); }
	BURGER_INLINE RGBAFloat_t &operator = (const Vector4D_t &rInput) { *this = *reinterpret_cast<const RGBAFloat_t *>(&rInput); return *this; }
};
struct HSL_t {
	float m_fHue;			///< 32 bit float hue 0 to 1.0f
	float m_fSaturation;	///< 32 bit float saturation 0 to 1.0f
	float m_fLuminance;		///< 32 bit float luminance 0 to 1.0f
	void BURGER_API Interpolate(const HSL_t *pInput1,const HSL_t *pInput2,float fFactor,Word bDirection=FALSE);
};
class Palette {
public:
	struct RGBColorList {
		Word8 m_uCount;			///< Number of entries
		RGBWord8_t Colors[1];	///< Actual data to set
	};

	static void BURGER_API FromRGB15(RGBWord8_t *pOutput,Word uInput);
	static void BURGER_API FromRGB15(RGBAWord8_t *pOutput,Word uInput);
	static void BURGER_API FromRGB1555(RGBAWord8_t *pOutput,Word uInput);
	static void BURGER_API FromRGB16(RGBWord8_t *pOutput,Word uInput);
	static void BURGER_API FromRGB16(RGBAWord8_t *pOutput,Word uInput);
	static Word BURGER_API ToRGB15(const RGBWord8_t *pInput);
	static Word BURGER_INLINE ToRGB15(const RGBAWord8_t *pInput) { return ToRGB15(reinterpret_cast<const RGBWord8_t*>(pInput)); }
	static Word BURGER_API ToRGB16(const RGBWord8_t *pInput);
	static Word BURGER_INLINE ToRGB16(const RGBAWord8_t *pInput) { return ToRGB16(reinterpret_cast<const RGBWord8_t*>(pInput)); }
	static Word BURGER_API ToDisplay(const RGBWord8_t *pInput,const Display *pDisplay);
	static Word BURGER_API ToDisplay(Word uRed,Word uGreen,Word uBlue,const Display *pDisplay);
	static Word BURGER_API ToDisplay(Word32 uRGBColor,const Display *pDisplay);
	static Word BURGER_API FindColorIndex(const Word8 *pPalette,Word uRed,Word uGreen,Word uBlue,Word uCount);
	static void BURGER_API Make8BitLookupTable(Word *pOutput,const Word8 *pInput,const Display *pDisplay);
	static void BURGER_API Make8BitLookupTable(Word *pOutput,RezFile *pInput,Word uRezNum,const Display *pDisplay);
	static void BURGER_API MakeRemapLookup(Word8 *pOutput,const Word8 *pNewPalette,const Word8 *pOldPalette);
	static void BURGER_API MakeRemapLookupMasked(Word8 *pOutput,const Word8 *pNewPalette,const Word8 *pOldPalette);
	static void BURGER_API MakeColorMasks(Word8 *pOutput,Word uMaskIndex);
	static void BURGER_API MakeFadeLookup(Word8 *pOutput,const Word8 *pInput,Word uRedAdjust,Word uGreenAdjust,Word uBlueAdjust);
};
BURGER_INLINE void CopyPalette(RGBWord8_t *pOutput,const RGBWord8_t *pInput,WordPtr uEntries=256) { MemoryCopy(pOutput,pInput,uEntries*sizeof(RGBWord8_t)); }
extern void BURGER_API CopyPalette(RGBWord8_t *pOutput,const RGBAWord8_t *pInput,WordPtr uEntries=256);
extern void BURGER_API CopyPalette(RGBAWord8_t *pOutput,const RGBWord8_t *pInput,WordPtr uEntries=256);
BURGER_INLINE void CopyPalette(RGBAWord8_t *pOutput,const RGBAWord8_t *pInput,WordPtr uEntries=256) { MemoryCopy(pOutput,pInput,uEntries*sizeof(RGBAWord8_t)); }
extern void BURGER_API CopyPalette444(RGBWord8_t *pOutput,const Word16 *pInput,WordPtr uEntries=256);
extern void BURGER_API CopyPalette444(RGBAWord8_t *pOutput,const Word16 *pInput,WordPtr uEntries=256);
extern void BURGER_API CopyPalette4444(RGBWord8_t *pOutput,const Word16 *pInput,WordPtr uEntries=256);
extern void BURGER_API CopyPalette4444(RGBAWord8_t *pOutput,const Word16 *pInput,WordPtr uEntries=256);
extern void BURGER_API Convert(RGBFloat_t *pOutput,const HSL_t *pInput);
extern void BURGER_API Convert(HSL_t *pOutput,const RGBFloat_t *pInput);
extern void BURGER_API CopyPalette256(RGBWord8_t *pOutput,const RGBWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256);
extern void BURGER_API CopyPalette256(RGBWord8_t *pOutput,const RGBAWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256);
extern void BURGER_API CopyPalette256(RGBAWord8_t *pOutput,const RGBWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256);
extern void BURGER_API CopyPalette256(RGBAWord8_t *pOutput,const RGBAWord8_t *pInput,Word uStartIndex=0,Word uPaletteSize=256);
extern void BURGER_API ClearPalette(RGBWord8_t *pOutput,WordPtr uPaletteSize=256);
extern void BURGER_API ClearPalette(RGBAWord8_t *pOutput,WordPtr uPaletteSize=256);
extern void BURGER_API FillPalette(RGBWord8_t *pOutput,const RGBWord8_t *pInput,WordPtr uPaletteSize=256);
extern void BURGER_API FillPalette(RGBAWord8_t *pOutput,const RGBAWord8_t *pInput,WordPtr uPaletteSize=256);

}
/* END */

#endif
