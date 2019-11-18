/***************************************

	Library to handle mp3 files (MP3)

	Copyright (c) 2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEMP3_H__
#define __BRFILEMP3_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
extern Word32 GetID3V2TagLength(const void *pInput);

class FileMP3 {
	struct ID3V2Tag_t {
		const static Word ID3_TAG_UNSYNCHRONISATION = 0x80;
		const static Word ID3_TAG_EXTENDED = 0x40;
		const static Word ID3_TAG_EXPERIMENTAL = 0x20;
		const static Word ID3_TAG_FOOTERPRESENT = 0x10;

		char m_ID3[3];			///< "ID3"
		Word8 m_uVersionMajor;	///< ID3 major version
		Word8 m_uVersionMinor;	///< ID3 minor version
		Word8 m_uFlags;			///< ID3 flags
		Word32 m_uLength;		///< Length of the ID3 chunk
	};

public:
	FileMP3();
	~FileMP3();
};

}
/* END */

#endif
