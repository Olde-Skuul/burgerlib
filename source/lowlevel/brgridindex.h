/***************************************

    Grid Index iterator

    Based on source written by Thatcher Ulrich

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRGRIDINDEX_H__
#define __BRGRIDINDEX_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRASSERT_H__
#include "brassert.h"
#endif

#ifndef __BRFLOATINGPOINT_H__
#include "brfloatingpoint.h"
#endif

#ifndef __BRVECTOR2D_H__
#include "brvector2d.h"
#endif

#ifndef __BRVECTOR3D_H__
#include "brvector3d.h"
#endif

#ifndef __BRVECTOR4D_H__
#include "brvector4d.h"
#endif

#ifndef __BRRECT_H__
#include "brrect.h"
#endif

#ifndef __BRSIMPLEARRAY_H__
#include "brsimplearray.h"
#endif

/* BEGIN */
namespace Burger {
extern void BURGER_API ChooseGridSize(uint_t* pXCellCount,uint_t* pYCellCount,const Vector4D_t *pBoundingRect,uintptr_t uItemCountEstimate,float fGridScale = 0.707f);

class GridIndexPoints {
public:
	struct Entry_t {
		Vector2D_t m_Point;	///< X,Y coordinate of the grid location
		Entry_t *m_pNext;	///< Pointer to the next item at this location
		uint_t m_uValue;		///< Value at this location
	};
private:
	Vector4D_t m_BoundsRect;	///< Rectangle encompassing the area of the grid
	Entry_t **m_ppGrid;			///< Array of pointers for grid objects
	uint_t m_uXCells;				///< Number of cells wide
	uint_t m_uYCells;				///< Number of cells high
	float m_fXCells;			///< Number of cells wide as a float
	float m_fYCells;			///< Number of cells high as a float

public:
	BURGER_INLINE Entry_t *GetCell(uint_t uX,uint_t uY) const
	{
		BURGER_ASSERT(uX < m_uXCells);
		BURGER_ASSERT(uY < m_uYCells);
		return m_ppGrid[(uY * m_uXCells) + uX];
	}

	BURGER_INLINE uintptr_t GetCellIndex(const Vector2D_t *pInput) const
	{
		BURGER_ASSERT(pInput->x < m_fXCells);
		BURGER_ASSERT(pInput->y < m_fYCells);
		uint_t uX = static_cast<uint_t>(static_cast<int>(pInput->x));
		uint_t uY = static_cast<uint_t>(static_cast<int>(pInput->y));
		return static_cast<uintptr_t>(uY * m_uXCells) + uX;
	}
	void BURGER_API GetContainingCellClamped(Vector2D_t *pOutput,const Vector2D_t *pInput) const;
public:
	GridIndexPoints(const Vector4D_t *pBounds,uint_t uXCells,uint_t uYCells);
	GridIndexPoints(const Vector4D_t *pBounds,uintptr_t uItemCountEstimate,float fGridScale = 0.707f);
	~GridIndexPoints();
	BURGER_INLINE const Vector4D_t *GetBoundsRect(void) const { return &m_BoundsRect; }

	struct iterator {
		const GridIndexPoints *m_pParent;	///< Pointer to parent class
		Entry_t *m_pEntry;					///< Current object
		Vector4D_t m_QueryRect;				///< Rect of interest
		Rect_t m_QueryCellsRect;			///< Bounds rectangle in cells
		uint_t m_uCurrentCellX;				///< Last accessed cell X coordinate
		uint_t m_uCurrentCellY;				///< Last accessed cell Y coordinate

		iterator(const GridIndexPoints *pParent) :
			m_pParent(pParent),
			m_pEntry(NULL),
			m_uCurrentCellX(0),
			m_uCurrentCellY(0) {
			m_QueryRect.Zero();
			m_QueryCellsRect.Clear();
		}

		BURGER_INLINE uint_t at_end(void) const { return !m_pEntry; }
		BURGER_INLINE void operator++() { if (m_pEntry) { advance(); }}
		void BURGER_API advance(void);
		BURGER_INLINE Entry_t &operator*() const { BURGER_ASSERT(m_pEntry != NULL); return *m_pEntry; }
		BURGER_INLINE Entry_t *operator->() const { BURGER_ASSERT(m_pEntry != NULL); return m_pEntry; }
	};
	iterator begin(const Vector4D_t *pInput) const;
	BURGER_INLINE iterator end(void) const { return iterator(this); }
	void BURGER_API add(const Vector2D_t *pPoint,uint_t uValue);
	void BURGER_API remove(Entry_t *pEntry);
	iterator find(const Vector2D_t *pPoint,uint_t uValue);
};

class GridIndexBox {
public:
	struct Entry {
		Vector4D_t m_BoundsRect;		///< Bounds rect for testing
		uint_t m_uValue;					///< Payload in the object
		uint_t m_uQueryCount;				///< Search ID number
		Entry() : m_uQueryCount(0) {}
	};
private:
	Vector4D_t m_BoundsRect;			///< Bounds rectangle for the grid
	SimpleArray<Entry* > *m_pGrid;		///< Array of pointers to arrays in the grid
	uint_t m_uXCells;						///< Number of cells wide
	uint_t m_uYCells;						///< Number of cells high
	float m_fXCells;					///< Number of cells wide as a float
	float m_fYCells;					///< Number of cells high as a float
	uint_t m_uQueryCount;					///< Token for iterating over the grid

public:
	BURGER_INLINE SimpleArray<Entry*>* GetCell(uint_t uX,uint_t uY) const {
		BURGER_ASSERT(uX < m_uXCells);
		BURGER_ASSERT(uY < m_uYCells);
		return &m_pGrid[(uY * m_uXCells) + uX];
	}
	void BURGER_API GetContainingCellClamped(Vector2D_t *pOutput,const Vector2D_t *pInput) const;
	void BURGER_API GetContainingCellsClamped(Vector4D_t *pOutput,const Vector4D_t *pInput) const;
public:
    GridIndexBox(const Vector4D_t *pBounds, uint_t uXCells, uint_t uYCells);
    GridIndexBox(const Vector4D_t *pBounds, uintptr_t uItemCountEstimate, float fGridScale = 0.707f);
	~GridIndexBox();
	BURGER_INLINE const Vector4D_t *GetBounds(void) const { return &m_BoundsRect; }
	BURGER_INLINE uint_t GetQueryCount(void) const { return m_uQueryCount; }
	struct iterator {
		const GridIndexBox *m_pParent;		///< Pointer to parent class
		Entry *m_pEntry;					///< Current object
		Vector4D_t m_QueryRect;				///< Rect of interest
		Rect_t m_QueryCellsRect;			///< Bounds rectangle in cells
		uint_t m_uCurrentCellX;				///< Last accessed cell X coordinate
		uint_t m_uCurrentCellY;				///< Last accessed cell Y coordinate
		uintptr_t m_uCurrentCellArrayIndex;	///< Index to the current entry's array

		iterator(const GridIndexBox *pParent) :
			m_pParent(pParent),
			m_pEntry(NULL),
			m_uCurrentCellX(0),
			m_uCurrentCellY(0),
			m_uCurrentCellArrayIndex(UINTPTR_MAX) {
			m_QueryRect.Zero();
			m_QueryCellsRect.Clear();
		}
		BURGER_INLINE uint_t at_end(void) const { return !m_pEntry; }
		BURGER_INLINE void operator++() { if (m_pEntry) { advance(); }}
		void BURGER_API advance(void);
		uint_t BURGER_API AdvanceInCell(void);
		BURGER_INLINE Entry &operator*() const { BURGER_ASSERT(m_pEntry != NULL); return *m_pEntry; }
		BURGER_INLINE Entry *operator->() const { BURGER_ASSERT(m_pEntry != NULL); return m_pEntry; }
	};
	iterator begin(const Vector4D_t *pBounds);
	BURGER_INLINE iterator begin_all(void) { return begin(&m_BoundsRect); }
	BURGER_INLINE iterator end(void) const { return iterator(this); }
	void BURGER_API add(const Vector4D_t *pBounds,uint_t uValue);
	void BURGER_API remove(Entry *pEntry);
	iterator find(const Vector4D_t *pBounds,uint_t uValue);
	Entry * BURGER_API FindData(const Vector2D_t *pPoint,uint_t uValue) const;
};
}
/* END */

#endif
