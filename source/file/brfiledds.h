/***************************************

    DDS File handler class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEDDS_H__
#define __BRFILEDDS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRIMAGE_H__
#include "brimage.h"
#endif

#ifndef __BRINPUTMEMORYSTREAM_H__
#include "brinputmemorystream.h"
#endif

#ifndef __BROUTPUTMEMORYSTREAM_H__
#include "broutputmemorystream.h"
#endif

#ifndef __BRPALETTE_H__
#include "brpalette.h"
#endif

/* BEGIN */
namespace Burger {
class FileDDS {
	uint32_t m_uReserved[11];
public:
#if defined(BURGER_BIGENDIAN) || defined(DOXYGEN)
	static const uint32_t FILETYPE = 0x44445320;		///< 'DDS ' Mac File type for DDS files (Byte swapped on little endian machines)
	static const uint32_t AUXTYPE = 0x474B4F4E;		///< 'GKON' Mac creator code for DDS files (Byte swapped on little endian machines)
#else
	static const uint32_t FILETYPE = 0x20534444;		///< 'DDS '
	static const uint32_t AUXTYPE = 0x4E4F4B47;		///< 'GKON'
#endif
	FileDDS();
	Image *Load(InputMemoryStream *pInput);
	uint_t Save(OutputMemoryStream *pOutput,const Image *pImage);
	BURGER_INLINE const uint32_t *GetReserved(void) const { return m_uReserved; }
	BURGER_INLINE uint32_t *GetReserved(void) { return m_uReserved; }
};
}
/* END */

#endif
