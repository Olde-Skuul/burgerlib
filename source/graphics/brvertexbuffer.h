/***************************************

	Vertex buffer class

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRVERTEXBUFFER_H__
#define __BRVERTEXBUFFER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

#if defined(BURGER_SHIELD) && !defined(__BRSHIELDTYPES_H__)
#include "brshieldtypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {
class Display;
class VertexBuffer : public ReferenceCounter {
	BURGER_RTTI_IN_CLASS();
public:
	enum eUsage {
		USAGE_COUNTMASK=0x0F,		///< Mask for the element count
		USAGE_CHUNKMASK=0x0F0,		///< Mask for the chunk type
		USAGE_TYPEMASK=0xF00,		///< Mask for the data type
		USAGE_FLOAT=0<<4,			///< Data chunks are floats
		USAGE_USHORT=1<<4,			///< Data chunks are unsigned shorts
		USAGE_POSITION=0<<8,		///< Data type is a position
		USAGE_BLENDWEIGHT=1<<8,		///< Data type is a blend weight
		USAGE_BLENDINDICES=2<<8,	///< Data type is a blend index
		USAGE_NORMAL=3<<8,			///< Data type is a vertex normal
		USAGE_PSIZE=4<<8,			///< Data type is a point size
		USAGE_TEXCOORD=5<<8,		///< Data type is a texture coordinate
		USAGE_TANGENT=6<<8,			///< Data type is a vertex tangent
		USAGE_BINORMAL=7<<8,		///< Data type is a vertex bi-normal
		USAGE_TESSFACTOR=8<<8,		///< Data type is a tessellation factor
		USAGE_POSITIONT=9<<8,		///< Data type is a transformed position
		USAGE_COLOR=10<<8,			///< Data type is a vertex color
		USAGE_FOG=11<<8,			///< Data type is a fog constant
		USAGE_DEPTH=12<<8,			///< Data type is a depth value
		USAGE_SAMPLE=13<<8			///< Data type is a texture value
	};
	struct VertexAoS_t {
		const void *m_pVertexArray;	///< Pointer to the vertex data to upload, if \ref NULL don't upload data
		WordPtr m_uVertexArraySize;	///< Size in bytes of the entire vertex buffer, if 0, don't generate vertex data
		const Word16 *m_pElements;	///< Pointer to the element index data to upload, \ref NULL don't upload data
		WordPtr m_uElementSize;		///< Size in bytes of the entire element buffer, if 0, don't generate an element index array
		const Word *m_pMembers;		///< Pointer to array of array description enumerations
	};
#if defined(BURGER_XBOX360) || defined(DOXYGEN)
protected:
	D3DVertexBuffer * m_pD3DVertexBuffer;			///< (XBox 360 Only) DirectX vertex buffer
	D3DVertexDeclaration *m_pD3DVertexDeclaration;	///< (XBox 360 Only) DirectX vertex array description
	Word m_uStride;									///< (XBox 360 Only) Size in bytes of each entry in the array
	Word m_uArrayEntryCount;						///< (XBox 360 Only) Number of entries in the array
#endif
#if defined(BURGER_OPENGL_SUPPORTED) && !defined(BURGER_WINDOWS)
	Word m_uVertexArrayObjectID;	///< (OpenGL only) Main Vertex Object Array
	Word m_uElementID;				///< (OpenGL only) ID to the index array
	Word m_uBufferID;				///< (OpenGL only) ID for the buffer
	Word m_uArrayEntryCount;		///< (OpenGL only) Number of entries in the array
	Word m_uElementEntryCount;		///< (OpenGL only) Number of entries in the element array
#endif
public:
	VertexBuffer();
	virtual ~VertexBuffer();
#if defined(BURGER_WINDOWS)
	virtual Word LoadData(Display *pDisplay,const VertexAoS_t *pDescription) = 0;
	virtual void ReleaseData(void) = 0;
#else
	Word LoadData(Display *pDisplay,const VertexAoS_t *pDescription);
	void ReleaseData(void);
#endif
#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	BURGER_INLINE D3DVertexBuffer *GetD3DVertexBuffer(void) const { return m_pD3DVertexBuffer; }
	BURGER_INLINE D3DVertexDeclaration *GetD3DVertexDescription(void) const { return m_pD3DVertexDeclaration; }
	BURGER_INLINE Word GetStride(void) const { return m_uStride; }
	BURGER_INLINE Word GetArrayEntryCount(void) const { return m_uArrayEntryCount; }
#endif
#if defined(BURGER_OPENGL_SUPPORTED) && !defined(BURGER_WINDOWS)
	BURGER_INLINE Word GetVertexArrayObject(void) const { return m_uVertexArrayObjectID; }
	BURGER_INLINE Word GetElementID(void) const { return m_uElementID; }
	BURGER_INLINE Word GetBufferID(void) const { return m_uBufferID; }
	BURGER_INLINE Word GetArrayEntryCount(void) const { return m_uArrayEntryCount; }
	BURGER_INLINE Word GetElementEntryCount(void) const { return m_uElementEntryCount; }
#endif
};
}
/* END */
#endif
