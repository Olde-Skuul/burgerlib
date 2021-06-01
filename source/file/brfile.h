/***************************************

	File Class

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

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
		/** Open file with read access only */
		kReadOnly,
		/** Create and open empty file with write access only */
		kWriteOnly,
		/** Open file with write access and set the mark at the end */
		kAppend,
		/** Open file for read and write access */
		kReadWrite
	};

private:
	void* m_pFile;         ///< Open file reference
	uintptr_t m_uPosition; ///< Seek position
	Filename m_Filename;   ///< Name of the file that was opened
	Semaphore m_Semaphore; ///< Semaphore for syncing file operations
#if defined(BURGER_MAC) || defined(DOXYGEN)
	uint8_t m_FSRef[80]; ///< File reference (MacOS Only)
#endif

public:
	File();
	File(const char* pFileName, eFileAccess eAccess = kReadOnly);
	File(Filename* pFileName, eFileAccess eAccess = kReadOnly);
	~File();
	static File* BURGER_API New(
		const char* pFileName, eFileAccess eAccess = kReadOnly);
	static File* BURGER_API New(
		Filename* pFileName, eFileAccess eAccess = kReadOnly);
	BURGER_INLINE uint_t IsOpened(void) const BURGER_NOEXCEPT
	{
		return m_pFile != nullptr;
	}
	eError BURGER_API Open(
		const char* pFileName, eFileAccess eAccess = kReadOnly) BURGER_NOEXCEPT;
	eError BURGER_API Open(
		Filename* pFileName, eFileAccess eAccess = kReadOnly) BURGER_NOEXCEPT;
	uint_t BURGER_API OpenAsync(
		const char* pFileName, eFileAccess eAccess = kReadOnly);
	uint_t BURGER_API OpenAsync(
		Filename* pFileName, eFileAccess eAccess = kReadOnly);
	eError BURGER_API Close(void) BURGER_NOEXCEPT;
	uint_t BURGER_API CloseAsync(void);
	uintptr_t BURGER_API GetSize(void);
	uintptr_t BURGER_API Read(void* pOutput, uintptr_t uSize);
	uint_t BURGER_API ReadAsync(void* pOutput, uintptr_t uSize);
	uintptr_t BURGER_API Write(
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT;
	uintptr_t BURGER_API GetMark(void);
	eError BURGER_API SetMark(uintptr_t uMark);
	uint_t BURGER_API SetMarkAtEOF(void);
	uint_t BURGER_API GetModificationTime(TimeDate_t* pOutput);
	uint_t BURGER_API GetCreationTime(TimeDate_t* pOutput);
	uint_t BURGER_API SetModificationTime(const TimeDate_t* pInput);
	uint_t BURGER_API SetCreationTime(const TimeDate_t* pInput);
#if (defined(BURGER_MACOS) || defined(BURGER_IOS)) || defined(DOXYGEN)
	uint_t SetAuxType(uint32_t uAuxType);
	uint_t SetFileType(uint32_t uFileType);
	uint32_t GetAuxType(void);
	uint32_t GetFileType(void);
	uint_t SetAuxAndFileType(uint32_t uAuxType, uint32_t uFileType);
#else
	BURGER_INLINE uint_t SetAuxType(uint32_t /* uAuxType */)
	{
		return kErrorNotSupportedOnThisPlatform;
	}
	BURGER_INLINE uint_t SetFileType(uint32_t /* uFileType */)
	{
		return kErrorNotSupportedOnThisPlatform;
	}
	BURGER_INLINE uint32_t GetAuxType(void) const
	{
		return 0;
	}
	BURGER_INLINE uint32_t GetFileType(void) const
	{
		return 0;
	}
	BURGER_INLINE uint_t SetAuxAndFileType(
		uint32_t /* uAuxType */, uint32_t /* uFileType */)
	{
		return kErrorNotSupportedOnThisPlatform;
	}
#endif
	uint_t BURGER_API ReadCString(char* pOutput, uintptr_t uLength);
	uint32_t BURGER_API ReadBigWord32(void);
	uint16_t BURGER_API ReadBigWord16(void);
	uint32_t BURGER_API ReadLittleWord32(void);
	uint16_t BURGER_API ReadLittleWord16(void);
};
}
/* END */

#endif
