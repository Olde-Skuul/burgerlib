/***************************************

	Grid Index iterator

	Based on source written by Thatcher Ulrich

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brgridindex.h"
#include "brfixedpoint.h"

/*! ************************************

	\brief Determine the optimum size of a grid
	
	Utility to find good values for x width and y height
	for a grid index, based on the expected number of
	items to index and the bounding box.

	A bias for the values up or down using grid_scale
	can be applied

	\note The cell count is clamped between 1 and 256 inclusive.

	\param pXCellCount Pointer to receive the width of the new grid
	\param pYCellCount Pointer to receive the height of the new grid
	\param pBoundingRect Bounding rect of the grid
	\param uItemCountEstimate Number of items likely to occupy the grid
	\param fGridScale Scale for precision

***************************************/

void BURGER_API Burger::ChooseGridSize(Word* pXCellCount,Word* pYCellCount,const Vector4D_t *pBoundingRect,WordPtr uItemCountEstimate,float fGridScale)
{
	// Set the default
	Word uXCellCount = 1;
	Word uYCellCount = 1;
	// Any cells?
	if (uItemCountEstimate) {
		// Get the area to map
		float fEstimate = static_cast<float>(static_cast<int>(uItemCountEstimate));
		float fWidth = pBoundingRect->GetWidth();
		float fHeight = pBoundingRect->GetHeight();
		float fArea = fWidth * fHeight;
		// Valid area?
		if (fArea > 0) {
			// Scale the area to the grid (Convert to reciprocal)
			fArea = 1.0f / (fArea * fGridScale);
			float fSqrt = Sqrt(fEstimate);
			fWidth = (fWidth * fWidth) * fArea;
			fHeight = (fHeight * fHeight) * fArea;
			uXCellCount = static_cast<Word>(static_cast<int>(fWidth * fSqrt));
			uYCellCount = static_cast<Word>(static_cast<int>(fHeight * fSqrt));
		} else {
			// No area? Width of height was zero
			// Convert the scale to the cell count
			Word uTemp = static_cast<Word>(static_cast<int>(fGridScale*fGridScale*fEstimate));
			if (fWidth > 0) {
				uXCellCount = uTemp;
			} else {
				uYCellCount = uTemp;
			}
		}
		// Force bounds to 1 through 256
		uXCellCount = Clamp(static_cast<Word32>(uXCellCount),1U,256U);
		uYCellCount = Clamp(static_cast<Word32>(uYCellCount),1U,256U);
	}
	pXCellCount[0] = uXCellCount;
	pYCellCount[0] = uYCellCount;
}

/*! ************************************

	\class Burger::GridIndexPoints
	\brief Manager to handle a grid with singly linked lists with values
	
	This class will create a 2D grid where data can be quickly added and
	removed at will.

	\sa GridIndexBox

***************************************/

/*! ************************************

	\struct Burger::GridIndexPoints::Entry_t
	\brief Singly linked data chunk for GridIndexPoints
	
	This is a single entry in the grid managed by GridIndexPoints

	\sa GridIndexPoints

***************************************/

/*! ************************************

	\fn Burger::GridIndexPoints::Entry_t Burger::GridIndexPoints::GetCell(Word uX,Word uY) const
	\brief Get the entry at a location in the grid
	
	Retrieve the root entry, if any, at the requested location in the grid

	\param uX X coordinate in the grid
	\param uY Y coordinate in the grid
	\return Pointer to a data entry or \ref NULL if no data is present in the location.

	\sa GetCellIndex(const Vector2D_t *) const or GridIndexPoints::Entry_t

***************************************/

/*! ************************************

	\fn WordPtr Burger::GridIndexPoints::GetCellIndex(const Vector2D_t *pInput) const
	\brief Get the offset into the grid from a coordinate
	
	Given a high precision location in the grid, get the 1 dimensional
	index into the 2 dimensional array.

	\param pInput Pointer to the high resolution location
	\return One dimensional index into the array

	\sa GetCell(Word,Word) const

***************************************/

/*! ************************************

	\brief Get the high precision location that contains a specific point.
	
	Given a high precision location in the grid, clamp it
	onto the grid

	\param pOutput Pointer to store the adjusted location.
	\param pInput Pointer to the high resolution location

	\sa GetCell(Word,Word) const

***************************************/

void BURGER_API Burger::GridIndexPoints::GetContainingCellClamped(Vector2D_t *pOutput,const Vector2D_t *pInput) const
{
	float fX = ((pInput->x - m_BoundsRect.GetTopLeft()->x) * m_fXCells) / m_BoundsRect.GetWidth();
	float fY = ((pInput->y - m_BoundsRect.GetTopLeft()->y) * m_fYCells) / m_BoundsRect.GetHeight();
	fX = Clamp(fX,0.0f,m_fXCells-1);
	fY = Clamp(fY,0.0f,m_fYCells-1);
	pOutput->x = fX;
	pOutput->y = fY;
}

/*! ************************************

	\brief Construct a grid of a specific size
	
	Given a grid size and a bounds rect that encompasses it,
	allocate memory for the grid and initialize it to \ref
	NULL entries.

	\param pBounds Pointer to the bounds rectangle
	\param uXCells Width of the grid in cells
	\param uYCells Height of the grid in cells

	\sa GridIndexPoints(const Vector4D_t *,WordPtr,float)

***************************************/

Burger::GridIndexPoints::GridIndexPoints(const Vector4D_t *pBounds,Word uXCells,Word uYCells) :
	m_BoundsRect(pBounds[0]),
	m_uXCells(uXCells),
	m_uYCells(uYCells),
	m_fXCells(static_cast<float>(static_cast<int>(uXCells))),
	m_fYCells(static_cast<float>(static_cast<int>(uYCells)))
{
	BURGER_ASSERT(uXCells > 0 && uYCells > 0);
	BURGER_ASSERT(pBounds->GetTopLeft()->x <= pBounds->GetBottomRight()->x);
	BURGER_ASSERT(pBounds->GetTopLeft()->y <= pBounds->GetBottomRight()->y);

	// Allocate the grid.
	m_ppGrid = static_cast<Entry_t **>(AllocClear(sizeof(Entry_t*)*uXCells * uYCells));
}

/*! ************************************

	\brief Construct a grid of an optimum size
	
	Given a grid size in elements and a bounds rect that encompasses it,
	allocate memory for the grid and initialize it to \ref
	NULL entries.

	The actual size will be generated to be as close to square
	that will hold as many elements as uItemCountEstimate requests.

	\param pBounds Pointer to the bounds rectangle
	\param uItemCountEstimate Number of total elements in the grid
	\param fGridScale Scale value to increase precision on the grid for the bounds rect

	\sa GridIndexPoints(const Vector4D_t *,Word,Word)

***************************************/

Burger::GridIndexPoints::GridIndexPoints(const Vector4D_t *pBounds,WordPtr uItemCountEstimate,float fGridScale) :
	m_BoundsRect(pBounds[0])
{
	// Figure out the optimum size
	ChooseGridSize(&m_uXCells,&m_uYCells,pBounds,uItemCountEstimate,fGridScale);
	BURGER_ASSERT(m_uXCells > 0 && m_uYCells > 0);
	BURGER_ASSERT(pBounds->GetTopLeft()->x <= pBounds->GetBottomRight()->x);
	BURGER_ASSERT(pBounds->GetTopLeft()->y <= pBounds->GetBottomRight()->y);
	m_fXCells = static_cast<float>(static_cast<int>(m_uXCells));
	m_fYCells = static_cast<float>(static_cast<int>(m_uYCells));

	// Allocate the grid.
	m_ppGrid = static_cast<Entry_t **>(AllocClear(sizeof(Entry_t*)*m_uXCells * m_uYCells));
}

/*! ************************************

	\brief Dispose of all allocated memory
	
	Standard destructor

***************************************/

Burger::GridIndexPoints::~GridIndexPoints()
{
	// Any data in the grid?
	WordPtr uCount = m_uXCells * m_uYCells;
	if (uCount) {
		// Iterate over the grid and dispose of any entries
		Entry_t **ppEntry = m_ppGrid;
		do {
			Entry_t *pEntry = ppEntry[0];
			if (pEntry) {
				do {
					Entry_t *pNext = pEntry->m_pNext;
					Free(pEntry);
					pEntry = pNext;
				} while (pEntry);
			}
			++ppEntry;
		} while (--uCount);
	}
	Free(m_ppGrid);
}

/*! ************************************

	\fn const Vector4D_t * Burger::GridIndexPoints::GetBoundsRect(void) const
	\brief Return a pointer to the bounds rectangle
	
	Retrieve a pointer to the bounds rectangle used by the class.

	\return Constant pointer to the rectangle that encompasses the grid

***************************************/

/*! ************************************

	\struct Burger::GridIndexPoints::iterator
	\brief STL compatible iterator
	
	Iterator used to traverse the grid using STL or STL like code.
	
	\sa GridIndexPoints

***************************************/

/*! ************************************

	\fn Burger::GridIndexPoints::iterator::iterator(const GridIndexPoints *pParent)
	\brief Default constructor
	
	Create an iterator for GridIndexPoints
	
	\param pParent Pointer to the parent GridIndexPoints class

***************************************/

/*! ************************************

	\fn Word Burger::GridIndexPoints::iterator::at_end(void) const
	\brief \ref TRUE if the iterator is at the end of the data
	
	\return \ref FALSE if the is more data remaining, \ref TRUE if data is exhausted.

***************************************/

/*! ************************************

	\fn void Burger::GridIndexPoints::iterator::operator++()
	\brief Step to the next data entry

	Iterate over the data until the next valid data chunk is found.
	If the iterator is already at the end of the data, no action
	is performed.

	\sa advance(void)

***************************************/

/*! ************************************

	\brief Step to the next data entry
	
	Point at next element in the iteration. Will assert
	if it's called when it's already reached the end of the data.

	\sa operator++()

***************************************/

void BURGER_API Burger::GridIndexPoints::iterator::advance(void)
	
{
	Entry_t *pEntry = m_pEntry;
	if (pEntry) {
		// Continue through current cell.
		pEntry = pEntry->m_pNext;
	}
	if (!pEntry) {
		// Done with current cell; go to next cell.
		++m_uCurrentCellX;
		while (m_uCurrentCellY <= static_cast<Word>(m_QueryCellsRect.GetBottom())) {
			while (m_uCurrentCellX <= static_cast<Word>(m_QueryCellsRect.GetRight())) {
				pEntry = m_pParent->GetCell(m_uCurrentCellX,m_uCurrentCellY);
				if (pEntry) {
					// Found a valid cell.
					break;
				}
				++m_uCurrentCellX;
			}
			if (pEntry) {
				break;
			}
			m_uCurrentCellX = static_cast<Word>(m_QueryCellsRect.GetLeft());
			++m_uCurrentCellY;
		}
		if (!pEntry) {
			BURGER_ASSERT(m_uCurrentCellX == static_cast<Word>(m_QueryCellsRect.GetLeft()));
			BURGER_ASSERT(m_uCurrentCellY == static_cast<Word>(m_QueryCellsRect.GetBottom() + 1));
			// No more valid cells.
			BURGER_ASSERT(at_end());
		}
	}
	m_pEntry = pEntry;
}

/*! ************************************

	\fn Burger::GridIndexPoints::Entry_t & Burger::GridIndexPoints::iterator::operator*() const
	\brief Access the entry with a reference

	Allows the used of the * operator to dereference the underlying data
	via a reference.

	\return Reference to the data entry.
	\sa operator->() const 

***************************************/

/*! ************************************

	\fn Burger::GridIndexPoints::Entry_t * Burger::GridIndexPoints::iterator::operator->() const
	\brief Access the entry with a reference

	Allows the used of the * operator to dereference the underlying data
	via a pointer.

	\return Pointer to the data entry.
	\sa operator*() const

***************************************/

/*! ************************************

	\brief Create an iterator for a sub-rectangle of the grid

	Clip a high resolution rectangle from the grid and set up an
	iterator that will traverse only the requested data.

	\return An iterator set up to the requested data.
	\sa end(void) const

***************************************/

Burger::GridIndexPoints::iterator Burger::GridIndexPoints::begin(const Vector4D_t *pInput) const
{
	Vector2D_t vTemp;
	iterator it(this);
	// Set up the high resolution rectangle
	it.m_QueryRect.Set(pInput);
	// Get the rectangle in cells
	GetContainingCellClamped(&vTemp,pInput->GetTopLeft());
	it.m_QueryCellsRect.SetLeft(static_cast<int>(vTemp.x));
	it.m_QueryCellsRect.SetTop(static_cast<int>(vTemp.y));
	GetContainingCellClamped(&vTemp,pInput->GetBottomRight());
	it.m_QueryCellsRect.SetRight(static_cast<int>(vTemp.x));
	it.m_QueryCellsRect.SetBottom(static_cast<int>(vTemp.y));
	BURGER_ASSERT(it.m_QueryCellsRect.GetLeft() <= it.m_QueryCellsRect.GetRight());
	BURGER_ASSERT(it.m_QueryCellsRect.GetTop() <= it.m_QueryCellsRect.GetBottom());

	// Find the first cell
	it.m_uCurrentCellX = static_cast<Word>(it.m_QueryCellsRect.GetLeft());
	it.m_uCurrentCellY = static_cast<Word>(it.m_QueryCellsRect.GetTop());
	it.m_pEntry = GetCell(it.m_uCurrentCellX,it.m_uCurrentCellY);

	// Make sure iterator starts valid.
	if (it.m_pEntry == NULL) {
		it.advance();
	}
	return it;
}

/*! ************************************

	\fn Burger::GridIndexPoints::iterator Burger::GridIndexPoints::end(void) const
	\brief Create an iterator with no data

	This iterator is used for comparisons to determine if the iterator
	returned by begin() has reached the end of the data.

	\return Iterator that is pointing past the end of the data.
	\sa begin(const Vector4D_t *) const

***************************************/

/*! ************************************

	\brief Insert data into the grid

	At the location indicated by pPoint, create an entry with the data
	and insert it into the grid.

	\param pPoint Pointer to the location in the grid to insert uValue
	\param uValue Data to place in the location in the grid.
	\sa remove(Entry_t *)

***************************************/

void BURGER_API Burger::GridIndexPoints::add(const Vector2D_t *pPoint,Word uValue)
{
	Vector2D_t vCellIndex;
	GetContainingCellClamped(&vCellIndex,pPoint);

	Entry_t *pEntry = static_cast<Entry_t *>(Alloc(sizeof(Entry_t)));
	pEntry->m_Point.Set(pPoint);
	pEntry->m_uValue = uValue;

	// Link it into the containing cell.
	WordPtr uIndex = GetCellIndex(&vCellIndex);
	pEntry->m_pNext = m_ppGrid[uIndex];
	m_ppGrid[uIndex] = pEntry;
}

/*! ************************************

	\brief Remove an entry from the grid

	Check if the entry is in the grid, and if so, removes
	it and then deletes the memory for the entry.

	\param pEntry Pointer to the Entry_t to dispose of.
	\sa add(const Vector2D_t *,Word)

***************************************/

void BURGER_API Burger::GridIndexPoints::remove(Entry_t *pEntry)
{
	BURGER_ASSERT(pEntry);

	Vector2D_t vCellIndex;
	GetContainingCellClamped(&vCellIndex,&pEntry->m_Point);
	WordPtr uIndex = GetCellIndex(&vCellIndex);

	// Track the previous pointer for singly linked list removal
	Entry_t **ppUnlink = &m_ppGrid[uIndex];
	Entry_t *pPrevious = ppUnlink[0];
	while (pPrevious) {
		if (pPrevious == pEntry) {
			// This is the one; unlink it.
			*ppUnlink = pPrevious->m_pNext;
			Free(pEntry);
			return;
		}
		// Go to the next entry.
		ppUnlink = &pPrevious->m_pNext;
		pPrevious = ppUnlink[0];
	}
	// Didn't find entry!  Something is wrong.
	BURGER_ASSERT(0);
}

/*! ************************************

	\brief Find an entry on the grid

	Scan the grid if an entry if present, if so, return 
	an iterator pointing to the requested entry. Otherwise,
	the iterator is set to point to the end of the data.

	\param pPoint Pointer to the location in the grid to start searching.
	\param uValue Value to match.
	\sa add(const Vector2D_t *,Word) or remove(Entry_t *)

***************************************/

Burger::GridIndexPoints::iterator Burger::GridIndexPoints::find(const Vector2D_t *pPoint,Word uValue)
{
	Vector4D_t Temp;
	Temp.SetRect(pPoint);
	iterator it = begin(&Temp);
	if (!it.at_end()) {
		do {
			if (it->m_Point == pPoint[0] && it->m_uValue == uValue) {
				// Found it.
				return it;
			}
			++it;
		} while (!it.at_end());
	}
	// Didn't find it.
	BURGER_ASSERT(it.at_end());
	return it;
}




/*! ************************************

	\class Burger::GridIndexBox
	\brief Class for managing a grid with objects on overlapping areas

	For some algorithms, it may be needed to insert data into overlapping
	regions onto a grid so when a location on a grid is queried, all objects
	that are in that location on the grid can be found.

	\sa GridIndexPoints

***************************************/

/*! ************************************

	\struct Burger::GridIndexBox::Entry
	\brief Data chunk for GridIndexBox
	
	This is a single entry in the grid managed by GridIndexBox

	\sa GridIndexBox

***************************************/

/*! ************************************

	\fn Burger::GridIndexBox::Entry::Entry()
	\brief Default constructor
	
	Initializes an Entry in the grid managed by GridIndexBox

	\sa GridIndexBox or Entry

***************************************/

/*! ************************************

	\fn SimpleArray<Entry*>* Burger::GridIndexBox::GetCell(Word uX,Word uY) const
	\brief Get the entry at a location in the grid
	
	Retrieve the root entry, if any, at the requested location in the grid

	\param uX X coordinate in the grid
	\param uY Y coordinate in the grid
	\return Pointer to a data entry or \ref NULL if no data is present in the location.

	\sa GetContainingCellClamped(Vector2D_t *,const Vector2D_t *) const or GridIndexBox::Entry

***************************************/

/*! ************************************

	\brief Get the high precision location that contains a specific point.
	
	Given a high precision location in the grid, clamp it
	onto the grid

	\param pOutput Pointer to store the adjusted location.
	\param pInput Pointer to the high resolution location

	\sa GetCell(Word,Word) const or GetContainingCellsClamped(Vector4D_t *,const Vector4D_t *) const

***************************************/

void BURGER_API Burger::GridIndexBox::GetContainingCellClamped(Vector2D_t *pOutput,const Vector2D_t *pInput) const
{
	float fX = ((pInput->x - m_BoundsRect.GetTopLeft()->x) * m_fXCells) / m_BoundsRect.GetWidth();
	float fY = ((pInput->y - m_BoundsRect.GetTopLeft()->y) * m_fYCells) / m_BoundsRect.GetHeight();
	fX = Clamp(fX,0.0f,m_fXCells-1);
	fY = Clamp(fY,0.0f,m_fYCells-1);
	pOutput->x = fX;
	pOutput->y = fY;
}

/*! ************************************

	\brief Get the high precision location that contains a specific rectangle of cells.
	
	Given a high precision area in the grid, clamp it
	onto the grid and return in cells.

	\param pOutput Pointer to store the adjusted rectangle.
	\param pInput Pointer to the high resolution rectangle

	\sa GetCell(Word,Word) const or GetContainingCellClamped(Vector2D_t *,const Vector2D_t *) const

***************************************/

void BURGER_API Burger::GridIndexBox::GetContainingCellsClamped(Vector4D_t *pOutput,const Vector4D_t *pInput) const
{
	GetContainingCellClamped(pOutput->GetTopLeft(),pInput->GetTopLeft());
	GetContainingCellClamped(pOutput->GetBottomRight(),pInput->GetBottomRight());
}

/*! ************************************

	\brief Construct a grid of a specific size
	
	Given a grid size and a bounds rect that encompasses it,
	allocate memory for the grid and initialize it to
	empty arrays.

	\param pBounds Pointer to the bounds rectangle
	\param uXCells Width of the grid in cells
	\param uYCells Height of the grid in cells

	\sa GridIndexBox(const Vector4D_t *,WordPtr,float)

***************************************/

Burger::GridIndexBox::GridIndexBox(const Vector4D_t *pBounds,Word uXCells,Word uYCells) :
	m_BoundsRect(pBounds[0]),
	m_uXCells(uXCells),
	m_uYCells(uYCells),
	m_fXCells(static_cast<float>(static_cast<int>(uXCells))),
	m_fYCells(static_cast<float>(static_cast<int>(uYCells))),
	m_uQueryCount(0)
{
	BURGER_ASSERT(uXCells > 0 && uYCells > 0);
	BURGER_ASSERT(pBounds->GetTopLeft()->x <= pBounds->GetBottomRight()->x);
	BURGER_ASSERT(pBounds->GetTopLeft()->y <= pBounds->GetBottomRight()->y);

	// Allocate the grid.
	m_pGrid = new SimpleArray<Entry*>[uXCells * uYCells];
}

/*! ************************************

	\brief Construct a grid of an optimum size
	
	Given a grid size in elements and a bounds rect that encompasses it,
	allocate memory for the grid and initialize it to \ref
	NULL entries.

	The actual size will be generated to be as close to square
	that will hold as many elements as uItemCountEstimate requests.

	\param pBounds Pointer to the bounds rectangle
	\param uItemCountEstimate Number of total elements in the grid
	\param fGridScale Scale value to increase precision on the grid for the bounds rect

	\sa GridIndexBox(const Vector4D_t *,Word,Word)

***************************************/

Burger::GridIndexBox::GridIndexBox(const Vector4D_t *pBounds,WordPtr uItemCountEstimate,float fGridScale) :
	m_BoundsRect(pBounds[0]),
	m_uQueryCount(0)
{
	ChooseGridSize(&m_uXCells,&m_uYCells,pBounds,uItemCountEstimate,fGridScale);

	BURGER_ASSERT(m_uXCells > 0 && m_uYCells > 0);
	BURGER_ASSERT(pBounds->GetTopLeft()->x <= pBounds->GetBottomRight()->x);
	BURGER_ASSERT(pBounds->GetTopLeft()->y <= pBounds->GetBottomRight()->y);
	m_fXCells = static_cast<float>(static_cast<int>(m_uXCells));
	m_fYCells = static_cast<float>(static_cast<int>(m_uYCells));
	m_pGrid = new SimpleArray<Entry*>[m_uXCells * m_uYCells];
}

/*! ************************************

	\brief Dispose of all allocated memory
	
	Standard destructor

***************************************/

Burger::GridIndexBox::~GridIndexBox()
{
	// Care has to be exercised here. Pointers
	// are duplicated, so use remove() on
	// every pointer found so
	// the duplicates can be purged as
	// well as the original

	WordPtr uCount = m_uXCells * m_uYCells;
	if (uCount) {
		SimpleArray<Entry*> *pArray = m_pGrid;
		do {
			// Get the entry array pointer
			WordPtr uCellCount = pArray->size();
			if (uCellCount) {
				do {
					if (uCellCount) {
						// Dispose of the entry and all duplicate pointers
						remove((*pArray)[0]);
					}
					uCellCount = pArray->size();
				} while (uCellCount);
			}
			++pArray;
		} while (--uCount);
	}
	delete [] m_pGrid;
}

/*! ************************************

	\fn const Vector4D_t *Burger::GridIndexBox::GetBounds(void) const
	\brief Get the bounds rectangle

	\return Pointer to the bounds rectangle

***************************************/

/*! ************************************

	\fn Word Burger::GridIndexBox::GetQueryCount(void) const
	\brief Get the number of times begin(const Vector4D_t *) has been called.
	
	\return Counter that has the number of times a search was performed.

***************************************/




/*! ************************************

	\struct Burger::GridIndexBox::iterator
	\brief STL compatible iterator
	
	Iterator used to traverse the grid using STL or STL like code.
	
	\sa GridIndexBox

***************************************/

/*! ************************************

	\fn Burger::GridIndexBox::iterator::iterator(const GridIndexBox *pParent)
	\brief Default constructor
	
	Create an iterator for GridIndexPoints
	
	\param pParent Pointer to the parent GridIndexPoints class

***************************************/

/*! ************************************

	\fn Word Burger::GridIndexBox::iterator::at_end(void) const
	\brief \ref TRUE if the iterator is at the end of the data
	
	\return \ref FALSE if the is more data remaining, \ref TRUE if data is exhausted.

***************************************/

/*! ************************************

	\fn void Burger::GridIndexBox::iterator::operator++()
	\brief Step to the next data entry

	Iterate over the data until the next valid data chunk is found.
	If the iterator is already at the end of the data, no action
	is performed.

	\sa advance(void)

***************************************/

/*! ************************************

	\brief Step to the next data entry
	
	Point at next element in the iteration. Will assert
	if it's called when it's already reached the end of the data.

	\sa operator++() or AdvanceInCell(void)

***************************************/

void BURGER_API Burger::GridIndexBox::iterator::advance(void)
	// Point at next element in the iteration.
{
	if (!AdvanceInCell()) {

		// Done with current cell; go to next cell.
		++m_uCurrentCellX;
		while (m_uCurrentCellY <= static_cast<Word>(m_QueryCellsRect.GetBottom())) {
			while (m_uCurrentCellX <= static_cast<Word>(m_QueryCellsRect.GetRight())) {
				if (AdvanceInCell()) {
					// We're good.
					return;
				}
				++m_uCurrentCellX;
			}
			m_uCurrentCellX = static_cast<Word>(m_QueryCellsRect.GetLeft());
			++m_uCurrentCellY;
		}
		BURGER_ASSERT(m_uCurrentCellX == static_cast<Word>(m_QueryCellsRect.GetLeft()));
		BURGER_ASSERT(m_uCurrentCellY == static_cast<Word>(m_QueryCellsRect.GetBottom() + 1));
		// No more valid cells.
		BURGER_ASSERT(at_end());
	}
}

/*! ************************************

	\brief Step to the next data entry in the current array
	
	Go to the next valid element in the current cell.
	If the end of the cell is reached, set
	m_uCurrentCellArrayIndex to \ref BURGER_MAXWORDPTR and return \ref FALSE.
	Otherwise point m_uCurrentCellArrayIndex at the valid
	element, and return \ref TRUE.

	\return \ref TRUE if data was found, \ref FALSE if the end of the array was reached.
	\sa operator++() or advance(void)

***************************************/

Word BURGER_API Burger::GridIndexBox::iterator::AdvanceInCell(void)
{
	Word uQueryCount = m_pParent->GetQueryCount();
	SimpleArray<Entry*> *pArray = m_pParent->GetCell(m_uCurrentCellX,m_uCurrentCellY);

	while (++m_uCurrentCellArrayIndex < pArray->size()) {
		// Continue through the current cell.
		Entry *pEntry = (*pArray)[m_uCurrentCellArrayIndex];
		// Skip this one; we already iterated over it.
		if (pEntry->m_uQueryCount != uQueryCount) {
			// Update query id.
			pEntry->m_uQueryCount = uQueryCount;
			m_pEntry = pEntry;
			return TRUE;
		}
	}
	// No more valid entries in this cell.
	m_pEntry = NULL;
	m_uCurrentCellArrayIndex = BURGER_MAXWORDPTR;
	return FALSE;
}

/*! ************************************

	\fn Burger::GridIndexBox::Entry & Burger::GridIndexBox::iterator::operator*() const
	\brief Access the entry with a reference

	Allows the used of the * operator to dereference the underlying data
	via a reference.

	\return Reference to the data entry.
	\sa operator->() const 

***************************************/

/*! ************************************

	\fn Burger::GridIndexBox::Entry *Burger::GridIndexBox::iterator::operator->() const
	\brief Access the entry with a reference

	Allows the used of the * operator to dereference the underlying data
	via a pointer.

	\return Pointer to the data entry.
	\sa operator*() const

***************************************/

/*! ************************************

	\brief Create an iterator for a sub-rectangle of the grid

	Clip a high resolution rectangle from the grid and set up an
	iterator that will traverse only the requested data.

	\return An iterator set up to the requested data.
	\sa end(void) const or begin_all(void)

***************************************/

Burger::GridIndexBox::iterator Burger::GridIndexBox::begin(const Vector4D_t *pBounds)
{
	if (++m_uQueryCount == 0) {
		// Query id wrapped around!  Clear m_uQueryCount in all entries in our
		// array, to avoid aliasing from old queries.
		WordPtr uCount = m_uXCells * m_uYCells;
		if (uCount) {
			SimpleArray<Entry*> *pArray = m_pGrid;
			do {
				WordPtr uCellCount = pArray->size();
				if (uCellCount) {
					WordPtr i = 0;
					do {
						Entry *pEntry = (*pArray)[i];
						pEntry->m_uQueryCount = 0;
						++pEntry;
					} while (++i<uCellCount);
				}
				++pArray;
			} while (--uCount);
		}
		m_uQueryCount = 1;
	}

	// Create the iterator
	iterator it(this);
	it.m_QueryRect.Set(pBounds);
	Vector2D_t vTemp;
	GetContainingCellClamped(&vTemp,pBounds->GetTopLeft());
	it.m_QueryCellsRect.SetLeft(static_cast<int>(vTemp.x));
	it.m_QueryCellsRect.SetTop(static_cast<int>(vTemp.y));
	GetContainingCellClamped(&vTemp,pBounds->GetBottomRight());
	it.m_QueryCellsRect.SetRight(static_cast<int>(vTemp.x));
	it.m_QueryCellsRect.SetBottom(static_cast<int>(vTemp.y));

	BURGER_ASSERT(it.m_QueryCellsRect.GetLeft() <= it.m_QueryCellsRect.GetRight());
	BURGER_ASSERT(it.m_QueryCellsRect.GetTop() <= it.m_QueryCellsRect.GetBottom());
	it.m_uCurrentCellX = static_cast<Word>(it.m_QueryCellsRect.GetLeft());
	it.m_uCurrentCellY = static_cast<Word>(it.m_QueryCellsRect.GetTop());
	it.advance();	// find first valid entry.
	return it;
}

/*! ************************************

	\fn Burger::GridIndexBox::iterator Burger::GridIndexBox::begin_all(void)
	\brief Create an iterator for all of the data in the grid

	Set up an iterator that will traverse all of the data.

	\return An iterator set up to traverse the entire grid.
	\sa end(void) const or begin(const Vector4D_t *)

***************************************/

/*! ************************************

	\fn Burger::GridIndexBox::iterator Burger::GridIndexBox::end(void) const
	\brief Create an iterator with no data

	This iterator is used for comparisons to determine if the iterator
	returned by begin() or begin_all() has reached the end of the data.

	\return Iterator that is pointing past the end of the data.
	\sa begin(const Vector4D_t *) or begin_all(void)

***************************************/

/*! ************************************

	\brief Insert data into the grid

	In the area indicated by pBounds, create an entry with the data
	and insert it into the grid.

	\param pBounds Pointer to the area of the grid to insert uValue
	\param uValue Data to place in the area in the grid.
	\sa remove(Entry *)

***************************************/

void BURGER_API Burger::GridIndexBox::add(const Vector4D_t *pBounds,Word uValue)
	// Insert a box, with the given payload, into our index.
{
	Entry *pEntry = New<Entry>();
	pEntry->m_BoundsRect.Set(pBounds);
	pEntry->m_uValue = uValue;

	Vector4D_t BoundsRect;
	GetContainingCellsClamped(&BoundsRect,pBounds);
	Word uLeft = static_cast<Word>(static_cast<int>(BoundsRect.x));
	Word uTop = static_cast<Word>(static_cast<int>(BoundsRect.y));
	Word uRight = static_cast<Word>(static_cast<int>(BoundsRect.z));
	Word uBottom = static_cast<Word>(static_cast<int>(BoundsRect.w));
	Word bPushed = FALSE;
	// Only insert into cells that matter
	for (Word iy = uTop; iy <= uBottom; iy++) {
		for (Word ix = uLeft; ix <= uRight; ix++) {
			SimpleArray<Entry*> *pArray = GetCell(ix,iy);
			pArray->push_back(pEntry);
			bPushed = TRUE;
		}
	}
	// To prevent memory leaks, if it wasn't pushed, delete it
	if (!bPushed) {
		Delete(pEntry);
	}
}

/*! ************************************

	\brief Remove an entry from the grid

	Check if the entry is in the grid, and if so, removes
	it and then deletes the memory for the entry.

	\param pEntry Pointer to the \ref Entry to dispose of.
	\sa add(const Vector4D_t *,Word)

***************************************/

void BURGER_API Burger::GridIndexBox::remove(Entry *pEntry)
{
	if (pEntry) {
		// Find and remove the entry from all cells that it overlaps with.
		Vector4D_t BoundsRect;
		GetContainingCellsClamped(&BoundsRect,&pEntry->m_BoundsRect);
		Word uLeft = static_cast<Word>(static_cast<int>(BoundsRect.x));
		Word uTop = static_cast<Word>(static_cast<int>(BoundsRect.y));
		Word uRight = static_cast<Word>(static_cast<int>(BoundsRect.z));
		Word uBottom = static_cast<Word>(static_cast<int>(BoundsRect.w));
		for (Word iy = uTop; iy <= uBottom; iy++) {
			for (Word ix = uLeft; ix <= uRight; ix++) {
				SimpleArray<Entry*> *pArray = GetCell(ix,iy);
				WordPtr uCount = pArray->size();
				if (uCount) {
					WordPtr i = 0;
					do {
						// Find entry, and remove it.
						if ((*pArray)[i] == pEntry) {
							pArray->remove_at(i);
							// No need to continue, it's gone
							break;
						}
					} while (++i<uCount);
				}
			}
		}
		Delete(pEntry);
	}
}

/*! ************************************

	\brief Find an entry on the grid

	Scan the grid if an entry if present, if so, return 
	an iterator pointing to the requested entry. Otherwise,
	the iterator is set to point to the end of the data.

	\param pBounds Pointer to the area in the grid to start searching.
	\param uValue Value to match.
	\sa FindData(const Vector2D_t *,Word) const

***************************************/

Burger::GridIndexBox::iterator Burger::GridIndexBox::find(const Vector4D_t *pBounds,Word uValue)
{
	iterator it = begin(pBounds);
	if (!it.at_end()) {
		do {
			if (it->m_BoundsRect == pBounds[0] && it->m_uValue == uValue) {
				// Found it.
				return it;
			}
			++it;
		} while (!it.at_end());
	}
	return it;
}

/*! ************************************

	\brief Find a single entry on the grid

	Scan the grid if an entry if present, if so, return 
	the pointer to the requested entry. Otherwise,
	return \ref NULL.

	\param pPoint Pointer to the point in the grid to search.
	\param uValue Value to match.
	\sa find(const Vector4D_t *,Word)

***************************************/

Burger::GridIndexBox::Entry * BURGER_API Burger::GridIndexBox::FindData(const Vector2D_t *pPoint,Word uValue) const
{
	Vector2D_t Clamped;
	GetContainingCellClamped(&Clamped,pPoint);
	SimpleArray<Entry*> *pCellArray = GetCell(static_cast<Word>(static_cast<int>(Clamped.x)),static_cast<Word>(static_cast<int>(Clamped.y)));
	WordPtr uCount = pCellArray->size();
	if (uCount) {
		WordPtr i = 0;
		do {
			Entry *pEntry = (*pCellArray)[i];
			if (pEntry->m_uValue == uValue) {
				// Found it.
				return pEntry;
			}
		} while (++i<uCount);
	}
	// Didn't find it.
	return NULL;
}