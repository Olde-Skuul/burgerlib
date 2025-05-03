/***************************************

	File Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

/* BEGIN */
namespace Burger {
class File {
	BURGER_DISABLE_COPY(File);

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

protected:
	/** Open file reference */
	void* m_pFile;

	/** Name of the file that was opened */
	Filename m_Filename;

#if defined(BURGER_MAC) || defined(DOXYGEN)
	/** True if using MacOS 9 or higher APIs (MacOS Only) */
	uint_t m_bUsingFSRef;
#endif

public:
	File() BURGER_NOEXCEPT;
	File(
		const char* pFileName, eFileAccess uAccess = kReadOnly) BURGER_NOEXCEPT;
	File(Filename* pFileName, eFileAccess uAccess = kReadOnly) BURGER_NOEXCEPT;
	~File();

	static File* BURGER_API new_object(
		const char* pFileName, eFileAccess uAccess = kReadOnly) BURGER_NOEXCEPT;
	static File* BURGER_API new_object(
		Filename* pFileName, eFileAccess uAccess = kReadOnly) BURGER_NOEXCEPT;

	BURGER_INLINE Filename* get_filename(void) BURGER_NOEXCEPT
	{
		return &m_Filename;
	}

	BURGER_INLINE void* get_file_pointer(void) const BURGER_NOEXCEPT
	{
		return m_pFile;
	}

	BURGER_INLINE void set_file_pointer(void* pFile) BURGER_NOEXCEPT
	{
		m_pFile = pFile;
	}

	BURGER_INLINE uint_t is_opened(void) const BURGER_NOEXCEPT
	{
		return m_pFile != nullptr;
	}

	eError BURGER_API open(
		const char* pFileName, eFileAccess uAccess = kReadOnly) BURGER_NOEXCEPT;
	eError BURGER_API open(
		Filename* pFileName, eFileAccess uAccess = kReadOnly) BURGER_NOEXCEPT;
	eError BURGER_API open_async(
		const char* pFileName, eFileAccess uAccess = kReadOnly) BURGER_NOEXCEPT;
	eError BURGER_API open_async(
		Filename* pFileName, eFileAccess uAccess = kReadOnly) BURGER_NOEXCEPT;
	eError BURGER_API close(void) BURGER_NOEXCEPT;
	eError BURGER_API close_async(void) BURGER_NOEXCEPT;
	uint64_t BURGER_API get_file_size(void) BURGER_NOEXCEPT;
	uintptr_t BURGER_API read(void* pOutput, uintptr_t uSize) BURGER_NOEXCEPT;
	eError BURGER_API read_async(
		void* pOutput, uintptr_t uSize) BURGER_NOEXCEPT;
	uintptr_t BURGER_API write(
		const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT;
	uint64_t BURGER_API get_mark(void) BURGER_NOEXCEPT;
	eError BURGER_API set_mark(uint64_t uMark) BURGER_NOEXCEPT;
	eError BURGER_API set_mark_at_EOF(void) BURGER_NOEXCEPT;
	eError BURGER_API get_modification_time(
		TimeDate_t* pOutput) BURGER_NOEXCEPT;
	eError BURGER_API get_creation_time(TimeDate_t* pOutput) BURGER_NOEXCEPT;
	eError BURGER_API set_modification_time(
		const TimeDate_t* pInput) BURGER_NOEXCEPT;
	eError BURGER_API set_creation_time(
		const TimeDate_t* pInput) BURGER_NOEXCEPT;

	eError BURGER_API set_creator_type(uint32_t uCreatorType) BURGER_NOEXCEPT;
	eError BURGER_API set_file_type(uint32_t uFileType) BURGER_NOEXCEPT;
	uint32_t BURGER_API get_creator_type(void) BURGER_NOEXCEPT;
	uint32_t BURGER_API get_file_type(void) BURGER_NOEXCEPT;
	eError BURGER_API set_creator_and_file_type(
		uint32_t uCreatorType, uint32_t uFileType) BURGER_NOEXCEPT;

	eError BURGER_API read_c_string(
		char* pOutput, uintptr_t uLength) BURGER_NOEXCEPT;
	uint32_t BURGER_API read_big_uint32(void) BURGER_NOEXCEPT;
	uint16_t BURGER_API read_big_uint16(void) BURGER_NOEXCEPT;
	uint32_t BURGER_API read_little_uint32(void) BURGER_NOEXCEPT;
	uint16_t BURGER_API read_little_uint16(void) BURGER_NOEXCEPT;
};
}
/* END */

#endif
