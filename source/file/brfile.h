/***************************************

	File Class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRFILENAME_H__
#include "brfilename.h"
#endif

#ifndef __BRTIMEDATE_H__
#include "brtimedate.h"
#endif

#ifndef __BRCRITICALSECTION_H__
#include "brcriticalsection.h"
#endif

/* BEGIN */
namespace Burger {
class File {
	friend class FileManager;
public:
	enum eFileAccess {
		READONLY=0x00,		///< Open file with read access only
		WRITEONLY=0x01,		///< Create and open empty file with write access only
		APPEND=0x02,		///< Open file with write access and set the mark at the end
		READANDWRITE=0x3	///< Open file for read and write access
	};
	enum eError {
		OKAY=0,				///< No error
		NOT_IMPLEMENTED=1,	///< Requested task not available on this platform
		IOERROR=2,			///< Device error
		WRITEERROR=3,		///< Can't write (Writing to a DVD?)
		INVALID_MARK=4,		///< File mark beyond the range of the file
		OUTOFRANGE=5,		///< Parameter out of range
		FILENOTFOUND=6		///< File not found
	};
private:
	void *m_pFile;				///< Open file reference
	uintptr_t m_uPosition;		///< Seek position
	Filename m_Filename;		///< Name of the file that was opened
	Semaphore m_Semaphore;		///< Semaphore for syncing file operations
#if defined(BURGER_MAC) || defined(DOXYGEN)
	Word8 m_FSRef[80];			///< File reference (MacOS Only)
#endif

public:
	File();
	File(const char *pFileName,eFileAccess eAccess=READONLY);
	File(Filename *pFileName,eFileAccess eAccess=READONLY);
	~File();
	static File * BURGER_API New(const char *pFileName,eFileAccess eAccess=READONLY);
	static File * BURGER_API New(Filename *pFileName,eFileAccess eAccess=READONLY);
	BURGER_INLINE Word IsOpened(void) const { return m_pFile!= nullptr; }
	Burger::eError BURGER_API Open(const char *pFileName,eFileAccess eAccess=READONLY) BURGER_NOEXCEPT;
    Burger::eError BURGER_API Open(Filename *pFileName,eFileAccess eAccess=READONLY) BURGER_NOEXCEPT;
	Word BURGER_API OpenAsync(const char *pFileName,eFileAccess eAccess=READONLY);
	Word BURGER_API OpenAsync(Filename *pFileName,eFileAccess eAccess=READONLY);
	Word BURGER_API Close(void) BURGER_NOEXCEPT;
	Word BURGER_API CloseAsync(void);
	WordPtr BURGER_API GetSize(void);
	WordPtr BURGER_API Read(void *pOutput,WordPtr uSize);
	Word BURGER_API ReadAsync(void *pOutput,WordPtr uSize);
	WordPtr BURGER_API Write(const void *pInput,WordPtr uSize) BURGER_NOEXCEPT;
	WordPtr BURGER_API GetMark(void);
	Word BURGER_API SetMark(WordPtr uMark);
	Word BURGER_API SetMarkAtEOF(void);
	Word BURGER_API GetModificationTime(TimeDate_t *pOutput);
	Word BURGER_API GetCreationTime(TimeDate_t *pOutput);
	Word BURGER_API SetModificationTime(const TimeDate_t *pInput);
	Word BURGER_API SetCreationTime(const TimeDate_t *pInput);
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
	Word BURGER_API ReadCString(char *pOutput,WordPtr uLength);
	Word32 BURGER_API ReadBigWord32(void);
	Word16 BURGER_API ReadBigWord16(void);
	Word32 BURGER_API ReadLittleWord32(void);
	Word16 BURGER_API ReadLittleWord16(void);
};
}
/* END */

#endif

