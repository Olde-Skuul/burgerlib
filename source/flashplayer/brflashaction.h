/***************************************

    Flash player action buffer

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFLASHACTION_H__
#define __BRFLASHACTION_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSIMPLEARRAY_H__
#include "brsimplearray.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
class SharedBuffer : public ReferenceCounter {
	SimpleArray<uint8_t> m_Buffer;		///< Internal data buffer
	BURGER_RTTI_IN_CLASS();
public:
	SharedBuffer();
	virtual ~SharedBuffer();
	BURGER_INLINE uintptr_t GetDataSize(void) const { return m_Buffer.size();}
	BURGER_INLINE void Append(uint8_t uInput) { m_Buffer.push_back(uInput); }
	BURGER_INLINE const uint8_t *GetData(void) const { return &m_Buffer[0]; }
	BURGER_INLINE SimpleArray<uint8_t> *GetBuffer(void) { return &m_Buffer; }
};
}}
/* END */

#endif
