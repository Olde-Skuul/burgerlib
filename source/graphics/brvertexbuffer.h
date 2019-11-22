/***************************************

	Vertex buffer class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRDISPLAYOBJECT_H__
#include "brdisplayobject.h"
#endif

#if defined(BURGER_MACOSX) && !defined(__BRMACOSXTYPES_H__)
#include "brmacosxtypes.h"
#endif

#if defined(BURGER_SHIELD) && !defined(__BRSHIELDTYPES_H__)
#include "brshieldtypes.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

#if defined(BURGER_XBOX360) && !defined(__BRXBOX360TYPES_H__)
#include "brxbox360types.h"
#endif

/* BEGIN */
namespace Burger {
class Display;
class VertexBuffer : public DisplayObject {

    BURGER_DISABLE_COPY(VertexBuffer);
	BURGER_RTTI_IN_CLASS();

public:
	enum eUsage {
		USAGE_CHUNKMASK = 0xFF0,	///< Mask for the chunk type
		USAGE_CHUNKMASKSHIFT = 4,	///< Shift for the chunk type
		USAGE_TYPEMASK=0xF000,		///< Mask for the data type
		USAGE_TYPEMASKSHIFT = 12,	///< Shift for the data type
		
		USAGE_FLOAT1 = 0<<USAGE_CHUNKMASKSHIFT,		///< Data chunks is a single float
		USAGE_FLOAT2 = 1<<USAGE_CHUNKMASKSHIFT,		///< Data chunks is 2 floats
		USAGE_FLOAT3 = 2<<USAGE_CHUNKMASKSHIFT,		///< Data chunks is 3 floats
		USAGE_FLOAT4 = 3<<USAGE_CHUNKMASKSHIFT,		///< Data chunks is 4 floats
		USAGE_COLOR4 = 4<<USAGE_CHUNKMASKSHIFT,		///< Data chunks are color bytes (0-255 is 0.0f to 1.0f)

		USAGE_POSITION=0<<USAGE_TYPEMASKSHIFT,		///< Data type is a position
		USAGE_BLENDWEIGHT=1<<USAGE_TYPEMASKSHIFT,	///< Data type is a blend weight
		USAGE_BLENDINDICES=2<<USAGE_TYPEMASKSHIFT,	///< Data type is a blend index
		USAGE_NORMAL=3<<USAGE_TYPEMASKSHIFT,		///< Data type is a vertex normal
		USAGE_PSIZE=4<<USAGE_TYPEMASKSHIFT,			///< Data type is a point size
		USAGE_TEXCOORD=5<<USAGE_TYPEMASKSHIFT,		///< Data type is a texture coordinate
		USAGE_TANGENT=6<<USAGE_TYPEMASKSHIFT,		///< Data type is a vertex tangent
		USAGE_BINORMAL=7<<USAGE_TYPEMASKSHIFT,		///< Data type is a vertex bi-normal
		USAGE_TESSFACTOR=8<<USAGE_TYPEMASKSHIFT,	///< Data type is a tessellation factor
		USAGE_POSITIONT=9<<USAGE_TYPEMASKSHIFT,		///< Data type is a transformed position
		USAGE_COLOR=10<<USAGE_TYPEMASKSHIFT,		///< Data type is a vertex color
		USAGE_FOG=11<<USAGE_TYPEMASKSHIFT,			///< Data type is a fog constant
		USAGE_DEPTH=12<<USAGE_TYPEMASKSHIFT,		///< Data type is a depth value
		USAGE_SAMPLE=13<<USAGE_TYPEMASKSHIFT,		///< Data type is a texture value

		FLAGAOS_DONTCOPY_VERTICES = 1<<0,			///< For VertexAoS_t, the m_pVertexArray entry is persistent, don't copy it
		FLAGAOS_DONTCOPY_ELEMENTS = 1<<1,			///< For VertexAoS_t, the m_pElements entry is persistent, don't copy it
		FLAGAOS_DONTCOPY_MEMBERS = 1<<2,			///< For VertexAoS_t, the m_pMembers entry is persistent, don't copy it
		FLAGAOS_VERTICES_DYNAMIC = 1<<3,			///< For VertexAoS_t, hint that the vertices will be modifed in future calls
		FLAGAOS_ELEMENTS_32BIT = 1<<4,				///< For VertexAoS_t, elements are 16 bit, unless this flag is set, and then they are 32 bits per entry
		FLAG_VERTEXDIRTY = 1<<30,					///< Internal flag, set when vertex data was changed and an upload is queued
		USAGE_END=0x7FFFFFFFU						///< End of the data
	};

	struct VertexAoS_t {
		const void *m_pVertexArray;	///< Pointer to the vertex data to upload, if \ref NULL don't upload data
		WordPtr m_uVertexArraySize;	///< Size in bytes of the entire vertex buffer, if 0, don't generate vertex data
		const void *m_pElements;	///< Pointer to the element index data to upload, \ref NULL don't upload data
		WordPtr m_uElementSize;		///< Size in bytes of the entire element buffer, if 0, don't generate an element index array
		const Word *m_pMembers;		///< Pointer to array of array description enumerations
		Word m_uFlags;				///< Look at FLAGAOS_* values for description
	};

	static const Word g_ChunkElementSizes[5];	///< Size of each element for the USAGE_* chunk sizes
	static const Word g_ChunkElementCounts[5];	///< Number of elements for the USAGE_* chunks
protected:
	const void *m_pVertexArray;	///< Pointer to the vertex data to upload, if \ref NULL don't upload data
	WordPtr m_uVertexArraySize;	///< Size in bytes of the entire vertex buffer, if 0, don't generate vertex data
	const void *m_pElements;	///< Pointer to the element index data to upload, \ref NULL don't upload data
	WordPtr m_uElementSize;		///< Size in bytes of the entire element buffer, if 0, don't generate an element index array
	const Word *m_pMembers;		///< Pointer to array of array description enumerations

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	D3DVertexBuffer * m_pD3DVertexBuffer;			///< (XBox 360 Only) DirectX vertex buffer
	D3DVertexDeclaration *m_pD3DVertexDeclaration;	///< (XBox 360 Only) DirectX vertex array description
	D3DIndexBuffer* m_pD3DIndexBuffer;				///< (XBox 360 Only) DirectX index buffer
#endif

#if defined(BURGER_WINDOWS) || defined(BURGER_OPENGL) || defined(DOXYGEN)
	union {

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
		struct {
			IDirect3DVertexBuffer9 * m_pVertexBuffer;		///< DirectX 9 vertex buffer
			IDirect3DVertexDeclaration9 *m_pDescription;	///< DirectX 9 vertex array description
			IDirect3DIndexBuffer9* m_pIndexBuffer;			///< DirectX 9 index buffer
		} m_DX9;							///< DirectX 9 specific data
#endif

		struct {
			Word m_uVertexArrayObjectID;	///< (OpenGL only) Main Vertex Object Array
			Word m_uElementID;				///< (OpenGL only) ID to the index array
			Word m_uBufferID;				///< (OpenGL only) ID for the buffer
		} m_GL;					///< OpenGL specific data

	} m_Platform;				///< Union of vertex buffer data for the platform
#endif

	Word m_uStride;				///< Size in bytes of each entry in the vertex array
	Word m_uArrayEntryCount;	///< Number of entries in the vertex array
	Word m_uElementEntryCount;	///< Number of entries in the element array
	Word m_uMemberCount;		///< Number of entries in the member array
	Word m_uFlags;				///< Flags describing how to treat the data

public:
	VertexBuffer();
	virtual ~VertexBuffer();

#if defined(BURGER_WINDOWS)
	Word CheckLoad(Display *pDisplay) BURGER_OVERRIDE = 0;
	void Release(Display *pDisplay) BURGER_OVERRIDE = 0;
#else
	Word CheckLoad(Display *pDisplay) BURGER_OVERRIDE;
	void Release(Display *pDisplay) BURGER_OVERRIDE;
#endif

	Word BURGER_API LoadData(Display *pDisplay,const VertexAoS_t *pDescription);
	void BURGER_API PurgeData(void);

#if defined(BURGER_XBOX360) || defined(DOXYGEN)
	BURGER_INLINE D3DVertexBuffer *GetD3DVertexBuffer(void) const { return m_pD3DVertexBuffer; }
	BURGER_INLINE D3DVertexDeclaration *GetD3DVertexDescription(void) const { return m_pD3DVertexDeclaration; }
	BURGER_INLINE D3DIndexBuffer* GetD3DIndexBuffer(void) const { return m_pD3DIndexBuffer; }	
#endif

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	BURGER_INLINE IDirect3DVertexBuffer9 *GetDX9VertexBuffer(void) const { return m_Platform.m_DX9.m_pVertexBuffer; }
	BURGER_INLINE IDirect3DVertexDeclaration9 *GetDX9VertexDescription(void) const { return m_Platform.m_DX9.m_pDescription; }
	BURGER_INLINE IDirect3DIndexBuffer9 *GetDX9IndexBuffer(void) const { return m_Platform.m_DX9.m_pIndexBuffer; }
#endif

#if defined(BURGER_OPENGL) || defined(DOXYGEN)
	BURGER_INLINE Word GetVertexArrayObject(void) const { return m_Platform.m_GL.m_uVertexArrayObjectID; }
	BURGER_INLINE Word GetElementID(void) const { return m_Platform.m_GL.m_uElementID; }
	BURGER_INLINE Word GetBufferID(void) const { return m_Platform.m_GL.m_uBufferID; }
#endif

	BURGER_INLINE Word GetStride(void) const { return m_uStride; }
	BURGER_INLINE Word GetArrayEntryCount(void) const { return m_uArrayEntryCount; }
	BURGER_INLINE Word GetElementEntryCount(void) const { return m_uElementEntryCount; }
};
}
/* END */
#endif
