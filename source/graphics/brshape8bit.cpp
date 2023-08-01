/***************************************

    Display base class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brshape8bit.h"
#include "brrezfile.h"
#include "brendian.h"

/*! ************************************

	Load a Burgerlib shape assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

Burger::LWShape_t * BURGER_API Burger::LWShape_t::Load(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	LWShape_t *Result = static_cast<LWShape_t *>(pRezFile->Load(uRezNum,&bLoaded));
	if (Result && bLoaded) {
		Result->Width = SwapEndian::load(Result->Width);
		Result->Height = SwapEndian::load(Result->Height);
	}
	return Result;
#else
	return static_cast<LWShape_t *>(pRezFile->Load(uRezNum));
#endif
}

/*! ************************************

	Load a Burgerlib shape assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

void BURGER_API Burger::LWShape_t::Preload(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	LWShape_t *pResult = static_cast<LWShape_t *>(pRezFile->Load(uRezNum,&bLoaded));
	if (pResult) {
		if (bLoaded) {
			SwapEndian::fixup(&pResult->Width);
			SwapEndian::fixup(&pResult->Height);
		}
		pRezFile->Release(uRezNum);
	}
#else
	pRezFile->Preload(uRezNum);
#endif
}

/*! ************************************

	Load a Burgerlib shape assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

Burger::LWXShape_t * BURGER_API Burger::LWXShape_t::Load(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	LWXShape_t *pResult = static_cast<LWXShape_t *>(pRezFile->Load(uRezNum,&bLoaded));
	if (pResult && bLoaded) {
		SwapEndian::fixup(&pResult->XOffset);
		SwapEndian::fixup(&pResult->YOffset);
		SwapEndian::fixup(&pResult->Shape.Width);
		SwapEndian::fixup(&pResult->Shape.Height);
	}
	return pResult;
#else
	return static_cast<LWXShape_t *>(pRezFile->Load(uRezNum));
#endif
}

/*! ************************************

	Load a Burgerlib shape assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

void BURGER_API Burger::LWXShape_t::Preload(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	LWXShape_t *pResult = static_cast<LWXShape_t *>(pRezFile->Load(uRezNum,&bLoaded));
	if (pResult) {
		if (bLoaded) {
			SwapEndian::fixup(&pResult->XOffset);
			SwapEndian::fixup(&pResult->YOffset);
			SwapEndian::fixup(&pResult->Shape.Width);
			SwapEndian::fixup(&pResult->Shape.Height);
		}
		pRezFile->Release(uRezNum);
	}
#else
	pRezFile->Preload(uRezNum);
#endif
}

/*! ************************************

	Load a Burgerlib shape assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

Burger::Shape8Bit_t * BURGER_API Burger::Shape8Bit_t::Load(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	Shape8Bit_t *pShape = static_cast<Shape8Bit_t *>(pRezFile->Load(uRezNum,&bLoaded));
	if (pShape && bLoaded) {
		SwapEndian::fixup(&pShape->m_usWidth);
		SwapEndian::fixup(&pShape->m_usHeight);
	}
	return pShape;
#else
	return static_cast<Shape8Bit_t *>(pRezFile->Load(uRezNum));
#endif
}

/*! ************************************

	Load a Burgerlib shape assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

void BURGER_API Burger::Shape8Bit_t::Preload(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	Shape8Bit_t *pShape = static_cast<Shape8Bit_t *>(pRezFile->Load(uRezNum,&bLoaded));
	if (pShape) {
		if (bLoaded) {
			SwapEndian::fixup(&pShape->m_usWidth);
			SwapEndian::fixup(&pShape->m_usHeight);
		}
		pRezFile->Release(uRezNum);
	}
#else
	pRezFile->Preload(uRezNum);
#endif
}

/*! ************************************

	Load a Burgerlib shape assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

Burger::Shape8BitOffset_t * BURGER_API Burger::Shape8BitOffset_t::Load(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	Shape8BitOffset_t *pShape = static_cast<Shape8BitOffset_t *>(pRezFile->Load(uRezNum,&bLoaded));
	if (pShape && bLoaded) {
		SwapEndian::fixup(&pShape->m_sXOffset);
		SwapEndian::fixup(&pShape->m_sYOffset);
		SwapEndian::fixup(&pShape->m_Shape8Bit.m_usWidth);
		SwapEndian::fixup(&pShape->m_Shape8Bit.m_usHeight);
	}
	return pShape;
#else
	return static_cast<Shape8BitOffset_t *>(pRezFile->Load(uRezNum));
#endif
}

/*! ************************************

	Load a Burgerlib shape assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

void BURGER_API Burger::Shape8BitOffset_t::Preload(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	Shape8BitOffset_t *pShape = static_cast<Shape8BitOffset_t *>(pRezFile->Load(uRezNum,&bLoaded));
	if (pShape) {
		if (bLoaded) {
			SwapEndian::fixup(&pShape->m_sXOffset);
			SwapEndian::fixup(&pShape->m_sYOffset);
			SwapEndian::fixup(&pShape->m_Shape8Bit.m_usWidth);
			SwapEndian::fixup(&pShape->m_Shape8Bit.m_usHeight);
		}
		pRezFile->Release(uRezNum);
	}
#else
	pRezFile->Preload(uRezNum);
#endif
}

/*! ************************************

	Load a Burgerlib shape array assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

Burger::Shape8BitArray_t * BURGER_API Burger::Shape8BitArray_t::Load(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	Shape8BitArray_t *pResult = static_cast<Shape8BitArray_t *>(pRezFile->Load(uRezNum,&bLoaded));	/* Load in the resource */
	if (pResult && bLoaded) {		/* Just loaded in? */
		uint32_t uCount = SwapEndian::load(pResult->m_Index);	/* Get the count */
		if (uCount) {		/* Any entries? */
			uCount >>=2;	/* Get the TRUE count */
			uint_t i = 0;
			do {
				uint32_t uOffset = SwapEndian::load(&pResult->m_Index[i]);
				pResult->m_Index[i] = uOffset;	/* Save the NEW offset */
				Shape8Bit_t *TempPtr = reinterpret_cast<Shape8Bit_t *>(reinterpret_cast<uint8_t *>(pResult)+uOffset);
				SwapEndian::fixup(&TempPtr->m_usWidth);
				SwapEndian::fixup(&TempPtr->m_usHeight);
			} while (++i<uCount);
		}
	}
	return pResult;
#else
	return static_cast<Shape8BitArray_t *>(pRezFile->Load(uRezNum));	/* Just load it */
#endif
}

/*! ************************************

	Load a Burgerlib shape array assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

void BURGER_API Burger::Shape8BitArray_t::Preload(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	Shape8BitArray_t *pResult = static_cast<Shape8BitArray_t *>(pRezFile->Load(uRezNum,&bLoaded));	/* Load in the resource */
	if (pResult) {
		if (bLoaded) {		/* Just loaded in? */
			uint32_t uCount = SwapEndian::load(pResult->m_Index);	/* Get the count */
			if (uCount) {		/* Any entries? */
				uCount >>=2;	/* Get the TRUE count */
				uint_t i = 0;
				do {
					uint32_t uOffset = SwapEndian::load(&pResult->m_Index[i]);
					pResult->m_Index[i] = uOffset;	/* Save the NEW offset */
					Shape8Bit_t *TempPtr = reinterpret_cast<Shape8Bit_t *>(reinterpret_cast<uint8_t *>(pResult)+uOffset);
					SwapEndian::fixup(&TempPtr->m_usWidth);
					SwapEndian::fixup(&TempPtr->m_usHeight);
				} while (++i<uCount);
			}
		}
		pRezFile->Release(uRezNum);
	}
#else
	pRezFile->Preload(uRezNum);	/* Just load it */
#endif
}

/*! ************************************

	Load a Burgerlib shape array assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

Burger::Shape8BitOffsetArray_t * BURGER_API Burger::Shape8BitOffsetArray_t::Load(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	Shape8BitOffsetArray_t *pResult = static_cast<Shape8BitOffsetArray_t *>(pRezFile->Load(uRezNum,&bLoaded));	/* Load in the resource */
	if (pResult && bLoaded) {		/* Just loaded in? */
		uint32_t uCount = SwapEndian::load(pResult->m_Index);	/* Get the count */
		if (uCount) {		/* Any entries? */
			uCount >>=2;	/* Get the TRUE count */
			uint_t i = 0;
			do {
				uint32_t uOffset = SwapEndian::load(&pResult->m_Index[i]);
				pResult->m_Index[i] = uOffset;	/* Save the NEW offset */
				Shape8BitOffset_t *TempPtr = reinterpret_cast<Shape8BitOffset_t *>(reinterpret_cast<uint8_t *>(pResult)+uOffset);
				SwapEndian::fixup(&TempPtr->m_sXOffset);
				SwapEndian::fixup(&TempPtr->m_sYOffset);
				SwapEndian::fixup(&TempPtr->m_Shape8Bit.m_usWidth);
				SwapEndian::fixup(&TempPtr->m_Shape8Bit.m_usHeight);
			} while (++i<uCount);
		}
	}
	return pResult;
#else
	return static_cast<Shape8BitOffsetArray_t *>(pRezFile->Load(uRezNum));	/* Just load it */
#endif
}

/*! ************************************

	Load a Burgerlib shape array assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

void BURGER_API Burger::Shape8BitOffsetArray_t::Preload(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	Shape8BitOffsetArray_t *pResult = static_cast<Shape8BitOffsetArray_t *>(pRezFile->Load(uRezNum,&bLoaded));	/* Load in the resource */
	if (pResult) {
		if (bLoaded) {		/* Just loaded in? */
			uint32_t uCount = SwapEndian::load(pResult->m_Index);	/* Get the count */
			if (uCount) {		/* Any entries? */
				uCount >>=2;	/* Get the TRUE count */
				uint_t i = 0;
				do {
					uint32_t uOffset = SwapEndian::load(&pResult->m_Index[i]);
					pResult->m_Index[i] = uOffset;	/* Save the NEW offset */
					Shape8BitOffset_t *TempPtr = reinterpret_cast<Shape8BitOffset_t *>(reinterpret_cast<uint8_t *>(pResult)+uOffset);
					SwapEndian::fixup(&TempPtr->m_sXOffset);
					SwapEndian::fixup(&TempPtr->m_sYOffset);
					SwapEndian::fixup(&TempPtr->m_Shape8Bit.m_usWidth);
					SwapEndian::fixup(&TempPtr->m_Shape8Bit.m_usHeight);
				} while (++i<uCount);
			}
		}
		pRezFile->Release(uRezNum);
	}
#else
	pRezFile->Load(uRezNum);	/* Just load it */
#endif
}


/*! ************************************

	Load a Burgerlib shape array assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

#if 0
void * BURGER_API Burger::RezFile::LoadShapeArray(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	void *pResult = Load(uRezNum);	/* Load in the resource */
	if (pResult && m_bResourceLoaded) {		/* Just loaded in? */
		uint32_t uCount = SwapEndian::load(static_cast<const uint32_t *>(pResult));	/* Get the count */
		if (uCount) {		/* Any entries? */
			uCount >>=2;	/* Get the TRUE count */
			uint_t i = 0;
			do {
				uint32_t uOffset = SwapEndian::load(&static_cast<uint32_t *>(pResult)[i]);
				static_cast<uint32_t *>(pResult)[i] = uOffset;	/* Save the NEW offset */
				LWShape_t *TempPtr = reinterpret_cast<LWShape_t *>(static_cast<uint8_t *>(pResult)+uOffset);
				SwapEndian::fixup(&TempPtr->Width);
				SwapEndian::fixup(&TempPtr->Height);
			} while (++i<uCount);
		}
	}
	return pResult;
#else
	return pRezFile->Load(uRezNum);	/* Just load it */
#endif
}

/*! ************************************

	Load a Burgerlib shape array assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

void BURGER_API Burger::RezFile::PreloadShapeArray(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	void *pResult = Load(uRezNum);	/* Load in the resource */
	if (pResult) {
		if (m_bResourceLoaded) {		/* Just loaded in? */
			uint_t uCount = SwapEndian::load(static_cast<uint32_t *>(pResult));	/* Get the count */
			if (uCount) {		/* Any entries? */
				uCount >>=2;	/* Get the TRUE count */
				uint_t i = 0;
				do {
					uint32_t uOffset = SwapEndian::load(&static_cast<uint32_t *>(pResult)[i]);
					static_cast<uint32_t *>(pResult)[i] = uOffset;	/* Save the NEW offset */
					LWShape_t *TempPtr = reinterpret_cast<LWShape_t *>(static_cast<uint8_t *>(pResult)+uOffset);
					SwapEndian::fixup(&TempPtr->Width);
					SwapEndian::fixup(&TempPtr->Height);
				} while (++i<uCount);
			}
		}
		Release(uRezNum);
	}
#else
	pRezFile->Preload(uRezNum);	/* Just load it */
#endif
}

/*! ************************************

	Load a Burgerlib shape array assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

void * BURGER_API Burger::RezFile::LoadXShapeArray(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	void *pResult = Load(uRezNum);	/* Load in the resource */
	if (pResult && m_bResourceLoaded) {		/* Just loaded in? */
		uint_t uCount = SwapEndian::load(static_cast<uint32_t *>(pResult));	/* Get the count */
		if (uCount) {		/* Any entries? */
			uCount >>=2;	/* Get the TRUE count */
			uint_t i = 0;
			do {
				uint32_t uOffset = SwapEndian::load(static_cast<uint32_t *>(pResult)[i]);
				static_cast<uint32_t *>(pResult)[i] = uOffset;	/* Save the NEW offset */
				LWXShape_t *TempPtr = reinterpret_cast<LWXShape_t *>(static_cast<uint8_t *>(pResult)+uOffset);
				SwapEndian::fixup(&TempPtr->XOffset);
				SwapEndian::fixup(&TempPtr->YOffset);
				SwapEndian::fixup(&TempPtr->Shape.Width);
				SwapEndian::fixup(&TempPtr->Shape.Height);
			} while (++i<uCount);
		}
	}
	return pResult;
#else
	return pRezFile->Load(uRezNum);	/* Just load it */
#endif
}

/*! ************************************

	Load a Burgerlib shape array assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

void BURGER_API Burger::RezFile::PreloadXShapeArray(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	void *pResult = Load(uRezNum);	/* Load in the resource */
	if (pResult) {
		if (m_bResourceLoaded) {		/* Just loaded in? */
			uint32_t uCount = SwapEndian::load(static_cast<uint32_t *>(pResult));	/* Get the count */
			if (uCount) {		/* Any entries? */
				uCount >>=2;	/* Get the TRUE count */
				uint_t i = 0;
				do {
					uint32_t uOffset = SwapEndian::load(static_cast<uint32_t *>(pResult)[i]);
					static_cast<uint32_t *>(pResult)[i] = uOffset;	/* Save the NEW offset */
					LWXShape_t *TempPtr = reinterpret_cast<LWXShape_t *>(static_cast<uint8_t *>(pResult)+uOffset);
					SwapEndian::fixup(&TempPtr->XOffset);
					SwapEndian::fixup(&TempPtr->YOffset);
					SwapEndian::fixup(&TempPtr->Shape.Width);
					SwapEndian::fixup(&TempPtr->Shape.Height);
				} while (++i<uCount);
			}
		}
		Release(uRezNum);
	}
#else
	pRezFile->Load(uRezNum);	/* Just load it */
#endif
}
#endif

/*! ************************************

	Load a Burgerlib shape assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

Burger::GfxShape_t * BURGER_API Burger::GfxShape_t::Load(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	GfxShape_t *pResult = static_cast<GfxShape_t *>(pRezFile->Load(uRezNum,&bLoaded));
	if (pResult && bLoaded) {
		SwapEndian::fixup(&pResult->XShape.XOffset);
		SwapEndian::fixup(&pResult->XShape.YOffset);
		SwapEndian::fixup(&pResult->XShape.Shape.Width);
		SwapEndian::fixup(&pResult->XShape.Shape.Height);
	}
	return pResult;
#else
	return static_cast<GfxShape_t *>(pRezFile->Load(uRezNum));
#endif
}

/*! ************************************

	Load a Burgerlib shape assuming it's
	little endian. I will swap the endian on
	big endian machines

***************************************/

void BURGER_API Burger::GfxShape_t::Preload(Burger::RezFile *pRezFile,uint_t uRezNum)
{
#if defined(BURGER_BIGENDIAN)
	uint_t bLoaded;
	GfxShape_t *pResult = static_cast<GfxShape_t *>(pRezFile->Load(uRezNum,&bLoaded));
	if (pResult) {
		if (bLoaded) {
			SwapEndian::fixup(&pResult->XShape.XOffset);
			SwapEndian::fixup(&pResult->XShape.YOffset);
			SwapEndian::fixup(&pResult->XShape.Shape.Width);
			SwapEndian::fixup(&pResult->XShape.Shape.Height);
		}
		pRezFile->Release(uRezNum);
	}
#else
	pRezFile->Preload(uRezNum);
#endif
}

