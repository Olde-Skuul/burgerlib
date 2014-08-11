/***************************************

	File Class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILE_H__
#define __BRFILE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRFILENAME_H__
#include "brfilename.h"
#endif

#ifndef __BRTIMEDATE_H__
#include "brtimedate.h"
#endif

/* BEGIN */
namespace Burger {
class File {
public:
	enum eFileAccess {
		READONLY=0x00,		///< Open file with read access only
		WRITEONLY=0x01,		///< Create and open empty file with write access only
		APPEND=0x02,		///< Open file with write access and set the mark at the end
		READANDWRITE=0x3	///< Open file for read and write access
	};
	enum {
		OKAY=0,				///< No error
		NOT_IMPLEMENTED=1,	///< Requested task not available on this platform
		IOERROR=2,			///< Device error
		WRITEERROR=3,		///< Can't write (Writing to a DVD?)
		INVALID_MARK=4,		///< File mark beyond the range of the file
		OUTOFRANGE=5,		///< Parameter out of range
		FILENOTFOUND=6		///< File not found
	};
	File();
	File(const char *pFileName,eFileAccess eAccess=READONLY);
	File(Filename *pFileName,eFileAccess eAccess=READONLY);
	~File();
	static File * BURGER_API New(const char *pFileName,eFileAccess eAccess=READONLY);
	static File * BURGER_API New(Filename *pFileName,eFileAccess eAccess=READONLY);
	BURGER_INLINE Word IsOpened(void) const { return m_pFile!=NULL; }
	Word Open(const char *pFileName,eFileAccess eAccess=READONLY);
	Word Open(Filename *pFileName,eFileAccess eAccess=READONLY);
	Word Close(void);
	WordPtr GetSize(void);
	WordPtr Read(void *pOutput,WordPtr uSize);
	WordPtr Write(const void *pInput,WordPtr uSize);
	WordPtr GetMark(void);
	Word SetMark(WordPtr uMark);
	Word SetMarkAtEOF(void);
	Word GetModificationTime(TimeDate_t *pOutput);
	Word GetCreationTime(TimeDate_t *pOutput);
	Word SetModificationTime(const TimeDate_t *pInput);
	Word SetCreationTime(const TimeDate_t *pInput);
#if (defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
	Word SetAuxType(Word32 uAuxType);
	Word SetFileType(Word32 uFileType);
	Word32 GetAuxType(void);
	Word32 GetFileType(void);
	Word SetAuxAndFileType(Word32 uAuxType,Word32 uFileType);
#else
	BURGER_INLINE Word SetAuxType(Word32 /* uAuxType */) { return NOT_IMPLEMENTED; }
	BURGER_INLINE Word SetFileType(Word32 /* uFileType */) { return NOT_IMPLEMENTED; }
	BURGER_INLINE Word32 GetAuxType(void) const { return 0; }
	BURGER_INLINE Word32 GetFileType(void) const { return 0; }
	BURGER_INLINE Word SetAuxAndFileType(Word32 /* uAuxType */,Word32 /* uFileType */) { return NOT_IMPLEMENTED; }
#endif
	Word ReadCString(char *pOutput,WordPtr uLength);
	Word32 ReadBigWord32(void);
	Word16 ReadBigWord16(void);
	Word32 ReadLittleWord32(void);
	Word16 ReadLittleWord16(void);
private:
	void *m_pFile;				///< Open file reference
#if defined(BURGER_MAC) || defined(DOXYGEN)
	Word8 m_FSRef[80];			///< File reference (MacOS Only)
#endif
};
}
/* END */

#endif

