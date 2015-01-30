/***************************************

	Texture for rendering class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRTEXTURE_H__
#define __BRTEXTURE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

#ifndef __BRIMAGE_H__
#include "brimage.h"
#endif

#ifndef __BRREZFILE_H__
#include "brrezfile.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {
class Display;
class Texture : public ReferenceCounter {
	BURGER_RTTI_IN_CLASS();
public:
	enum eWrapping {
		WRAP_REPEAT,		///< Wrap the texture around
		WRAP_CLAMP			///< Clamp the texture edge
	};
	enum eFilter {
		FILTER_NEAREST,		///< Faster, use nearest pixel
		FILTER_LINEAR		///< Use weighted average for smoothing
	};
	enum eLoader {
		LOADER_LOAD,		///< Command to load a texture into the Image
		LOADER_UNLOAD,		///< Command to release the memory in the Image if allowed
		LOADER_SHUTDOWN		///< Force all memory to be released
	};
	typedef Word (BURGER_API *LoaderProc)(Texture *pThis,eLoader uCommand);	///< Function prototype for loading callback
protected:
	enum eDirtyFlags {
		DIRTY_WRAPPING_S=0x01,	///< Set if the S value was changed
		DIRTY_WRAPPING_T=0x02,	///< Set if the T value was changed
		DIRTY_MIN=0x04,			///< Set if the MIN value was changed
		DIRTY_MAG=0x08,			///< Set if the MAG value was changed
		DIRTY_IMAGE=0x10,		///< Set if the image bitmap was changed
		DIRTY_EXTRA=0x20		///< Flags for derived classes
	};
	LoaderProc m_pLoader;	///< Function to load the image in case of texture purge
	void *m_pUserData;		///< Pointer to user data for the loader
	Texture *m_pNext;		///< Pointer to the next entry
	Texture *m_pPrev;		///< Pointer to the previous entry
#if defined(BURGER_XBOX360)
	D3DTexture *m_pD3DTexture;
#endif
#if defined(BURGER_OPENGL_SUPPORTED) && !defined(BURGER_WINDOWS)
	Word m_uTextureID;		///< OpenGL Texture ID
#endif
	Image m_Image;			///< Description of the texture data
	eWrapping m_eWrappingS;	///< Wrapping setting for the S value
	eWrapping m_eWrappingT;	///< Wrapping setting for the T value
	eFilter m_eMinFilter;	///< Filter for shrinkage
	eFilter m_eMagFilter;	///< Filter for expansion
	Word m_uDirty;			///< Values needed to be updated
	static Texture *g_pHead;	///< Global pointer to the linked list of textures
	void AddToGlobalList(void);
	void RemoveFromGlobalList(void);
public:
	Texture();
	Texture(eWrapping uWrapping,eFilter uFilter);
	virtual ~Texture();
#if defined(BURGER_WINDOWS)
	virtual Word Bind(Display *pDisplay) = 0;
	virtual void Release(Display *pDisplay) = 0;
#else
	Word Bind(Display *pDisplay);
	void Release(Display *pDisplay);
#endif
	Word BURGER_API LoadImageMemory(void);
	void BURGER_API UnloadImageMemory(void);
	void BURGER_API ShutdownImageMemory(void);
	BURGER_INLINE const Image *GetImage(void) const { return &m_Image; }
	BURGER_INLINE Image *GetImage(void) { return &m_Image; }
	BURGER_INLINE const Word8 *GetImage(Word uMipMap) const { return m_Image.GetImage(uMipMap); }
	BURGER_INLINE Word GetWidth(void) const { return m_Image.GetWidth(); }
	BURGER_INLINE Word GetWidth(Word uMipMap) const { return m_Image.GetWidth(uMipMap); }
	BURGER_INLINE Word GetHeight(void) const { return m_Image.GetHeight(); }
	BURGER_INLINE Word GetHeight(Word uMipMap) const { return m_Image.GetHeight(uMipMap); }
	BURGER_INLINE WordPtr GetStride(void) const { return m_Image.GetStride(); }
	BURGER_INLINE WordPtr GetStride(Word uMipMap) const { return m_Image.GetStride(uMipMap); }
	BURGER_INLINE Image::ePixelTypes GetType(void) const { return m_Image.GetType(); }
	BURGER_INLINE Word GetMipMapCount(void) const { return m_Image.GetMipMapCount(); }
	BURGER_INLINE eWrapping GetWrappingS(void) const { return m_eWrappingS; }
	BURGER_INLINE void SetWrappingS(eWrapping uWrapping) { m_eWrappingS = uWrapping; m_uDirty |= DIRTY_WRAPPING_S; }
	BURGER_INLINE eWrapping GetWrappingT(void) const { return m_eWrappingT; }
	BURGER_INLINE void SetWrappingT(eWrapping uWrapping) { m_eWrappingT = uWrapping; m_uDirty |= DIRTY_WRAPPING_T; }
	BURGER_INLINE void SetWrapping(eWrapping uWrapping) { m_eWrappingS = uWrapping; m_eWrappingT = uWrapping; m_uDirty |= (DIRTY_WRAPPING_S|DIRTY_WRAPPING_T);}
	BURGER_INLINE eFilter GetMinFilter(void) const { return m_eMinFilter; }
	BURGER_INLINE void SetMinFilter(eFilter uFilter) { m_eMinFilter = uFilter; m_uDirty |= DIRTY_MIN; }
	BURGER_INLINE eFilter GetMagFilter(void) const { return m_eMagFilter; }
	BURGER_INLINE void SetMagFilter(eFilter uFilter) { m_eMagFilter = uFilter; m_uDirty |= DIRTY_MAG; }
	BURGER_INLINE void SetFilter(eFilter uFilter) { m_eMinFilter = uFilter; m_eMagFilter = uFilter; m_uDirty |= (DIRTY_MIN|DIRTY_MAG); }
	BURGER_INLINE LoaderProc GetLoader(void) const { return m_pLoader; }
	BURGER_INLINE void SetLoader(LoaderProc pCallback) { m_pLoader = pCallback; }
	BURGER_INLINE static Texture *GetFirstTexture(void) { return g_pHead; }
	BURGER_INLINE Texture *GetNext(void) const { return m_pNext; }
	BURGER_INLINE Texture *GetPrevious(void) const { return m_pPrev; }
	void BURGER_API LoadTGA(RezFile *pRezFile,Word uRezNum);
	void BURGER_API LoadTGA(const char *pFilename);
	void BURGER_API LoadPNG(RezFile *pRezFile,Word uRezNum);
	void BURGER_API LoadPNG(const char *pFilename);
	void BURGER_API LoadBMP(RezFile *pRezFile,Word uRezNum);
	void BURGER_API LoadBMP(const char *pFilename);
	static void BURGER_API ReleaseAll(Display *pDisplay);
#if defined(BURGER_XBOX360)
	Word BURGER_API GetD3DFormat(void) const;
	BURGER_INLINE D3DTexture *GetTexture(void) const { return m_pD3DTexture; }
#endif
#if defined(BURGER_OPENGL_SUPPORTED) && !defined(BURGER_WINDOWS)
	static int GetWrapping(eWrapping uWrapping);
	static int GetFilter(eFilter uFilter);
	BURGER_INLINE Word GetTextureID(void) const { return m_uTextureID; }
#endif
private:
	static Word BURGER_API CallbackRezFileTGA(Texture *pTexture,eLoader uLoader);
	static Word BURGER_API CallbackFileTGA(Texture *pTexture,eLoader uLoader);
	static Word BURGER_API CallbackRezFilePNG(Texture *pTexture,eLoader uLoader);
	static Word BURGER_API CallbackFilePNG(Texture *pTexture,eLoader uLoader);
	static Word BURGER_API CallbackRezFileBMP(Texture *pTexture,eLoader uLoader);
	static Word BURGER_API CallbackFileBMP(Texture *pTexture,eLoader uLoader);
};
}
/* END */
#endif
