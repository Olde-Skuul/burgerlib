/***************************************

	File Manager Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEMANAGER_H__
#define __BRFILEMANAGER_H__

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

#ifndef __BRMUTEX_H__
#include "brmutex.h"
#endif

#ifndef __BRTHREAD_H__
#include "brthread.h"
#endif

#ifndef __BRSEMAPHORE_H__
#include "brsemaphore.h"
#endif

#ifndef __BRARRAY_H__
#include "brarray.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

#ifndef _INC_STDIO
#include <stdio.h>
#endif

/* BEGIN */
namespace Burger {
class File;

class FileManager {
	BURGER_DISABLE_COPY(FileManager);

public:
	/** Maximum number of pending IO events(Power of 2) */
	static const uint_t kMaxQueue = 128;

	enum ePrefix {
		/** 8: Current working directory at application launch */
		kPrefixCurrent = 8,
		/** 9: Directory where the application executable resides */
		kPrefixApplication = 9,
		/** "*:" Boot volume prefix number */
		kPrefixBoot = 32,
		/** "@:" User preferences folder prefix number */
		kPrefixPrefs = 33,
		/** "$:" System volume folder prefix number */
		kPrefixSystem = 34,
		/** Maximum number of pathname prefixes */
		kPrefixCount = 35,
		/** Illegal prefix number */
		kPrefixInvalid = 999
	};

	enum eIOCommand {
		/** Invalid command (Must be zero) */
		kIOCommandInvalid,
		/** Pending open file command */
		kIOCommandOpen,
		/** Pending close file command */
		kIOCommandClose,
		/** Pending read file command */
		kIOCommandRead,
		/** Pending write file command */
		kIOCommandWrite,
		/** Pending seek file command */
		kIOCommandSeek,
		/** Pending seek to end of file command */
		kIOCommandSeekEOF,
		/** Pending completion callback command */
		kIOCommandCallback,
		/** Pending sync command */
		kIOCommandSync,
		/** Pending end thread command */
		kIOCommandEndThread
	};

	/** Asynchronous file I/O message */
	struct Queue_t {
		/** Pointer to the file object that this Queue_t is attached to */
		File* m_pFile;
		/** Pointer to the I/O buffer or callback pointer */
		void* m_pBuffer;
		/** Value to attach to the command */
		uintptr_t m_uLength;
		/** IO Command */
		eIOCommand m_uIOCommand;
	};

	/** Function prototype for asynchronous I/O callbacks */
	typedef void(BURGER_API* ProcCallback)(Queue_t* pQueue);

protected:
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
	/** Semaphore to ping the IO thread */
	Semaphore m_PingIOThread;
	/** Semaphore to get a reply from the IO thread for syncing */
	Semaphore m_IOThreadSync;
	/** Worker thread record pointer */
	Thread m_Thread;
#endif

	/** Index to the queue start */
	volatile uint32_t m_uQueueStart;
	/** Index to the end of the queue */
	volatile uint32_t m_uQueueEnd;

	/** Array of prefix strings */
	String m_Prefixes[kPrefixCount];

	/** Queue of IO events */
	Queue_t m_IOQueue[kMaxQueue];

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
	/** Name of the MSDos flavor being run */
	const char* m_pDOSName;
	/** MSDOS version (MSDOS Only) */
	uint16_t m_uMSDOSVersion;
	/** MSDOS TRUE version (MSDOS Only) */
	uint16_t m_uMSDOSTrueVersion;
	/** True if MSDOS has long filename support (MSDOS Only) */
	uint8_t m_bLongNamesAllowed;
	/** OEM number (MSDOS Only) */
	uint8_t m_uOEMFlavor;
#endif

#if defined(BURGER_DARWIN) || defined(DOXYGEN)
	/** Length of the boot volume name (Darwin only) */
	uintptr_t m_uBootNameSize;
	/** Boot volume name in the format ":FooBar" (Zero terminated) (Darwin only)
	 */
	char* m_pBootName;
#endif

#if (defined(BURGER_UNIX) && !defined(BURGER_DARWIN)) || defined(DOXYGEN)
	/** Pointer to the /proc pathname or nullptr is no permission exists to
	 * access it */
	String m_ProcPath;
	/** Pointer to the /etc/mtab file */
	String m_EtcMtab;
	/** Volume prefix for paths */
	String m_VolumePrefix;
	/** Array of strings of file system types */
	ClassArray<String> m_FSTypes;
	/** Array of strings from fstab */
	ClassArray<String> m_FSTabs;
	/** Array of string of mounted volume paths */
	ClassArray<String> m_VolumePaths;
#endif

public:
	/** Global instance of the file manager */
	static FileManager* g_pFileManager;

protected:
	FileManager() BURGER_NOEXCEPT;
	~FileManager();

public:
	void BURGER_API flush_IO(void) BURGER_NOEXCEPT;
	void BURGER_API wait_until_queue_has_space(void) BURGER_NOEXCEPT;
	void BURGER_API add_queue(File* pFile, eIOCommand uIOCommand, void* pBuffer,
		uintptr_t uLength) BURGER_NOEXCEPT;
	void BURGER_API sync(File* pFile) BURGER_NOEXCEPT;
	static uintptr_t BURGER_API queue_handler(void* pData) BURGER_NOEXCEPT;

protected:
	void BURGER_API platform_setup(void) BURGER_NOEXCEPT;
	void BURGER_API platform_shutdown(void) BURGER_NOEXCEPT;

public:
	static eError BURGER_API initialize(void) BURGER_NOEXCEPT;
	static void BURGER_API shut_down(void) BURGER_NOEXCEPT;

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
	static uint_t BURGER_API MSDOS_has_long_filenames(void) BURGER_NOEXCEPT;
	static uint_t BURGER_API MSDOS_get_os_version(void) BURGER_NOEXCEPT;
	static uint_t BURGER_API MSDOS_get_os_true_version(void) BURGER_NOEXCEPT;
	static const char* BURGER_API MSDOS_get_name(void) BURGER_NOEXCEPT;
	static uint_t BURGER_API MSDOS_get_flavor(void) BURGER_NOEXCEPT;
	static eError BURGER_API MSDOS_expand_8_3_filename(
		String* pInput) BURGER_NOEXCEPT;
	static eError BURGER_API MSDOS_convert_to_8_3_filename(
		String* pInput) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
	static BURGER_API uint_t is_UTF8_file_system(void) BURGER_NOEXCEPT;
#else
	static BURGER_INLINE BURGER_CONSTEXPR uint_t is_UTF8_file_system(
		void) BURGER_NOEXCEPT
	{
#if defined(BURGER_MSDOS) || defined(BURGER_XBOX) || defined(BURGER_XBOX360)
		return FALSE;
#else
		return TRUE;
#endif
	}
#endif

	static eError BURGER_API get_volume_name(
		Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT;
	static uint_t BURGER_API get_volume_number(
		const char* pInput) BURGER_NOEXCEPT;
	static eError BURGER_API set_default_prefixes(void) BURGER_NOEXCEPT;
	static eError BURGER_API get_prefix(
		Filename* pOutput, uint_t uPrefixNum) BURGER_NOEXCEPT;
	static eError BURGER_API get_prefix(
		String* pOutput, uint_t uPrefixNum) BURGER_NOEXCEPT;
	static eError BURGER_API set_prefix(
		uint_t uPrefixNum, const char* pPrefixName) BURGER_NOEXCEPT;
	static eError BURGER_API set_prefix(
		uint_t uPrefixNum, const Filename* pPrefixName) BURGER_NOEXCEPT;
	static eError BURGER_API pop_prefix(uint_t uPrefixNum) BURGER_NOEXCEPT;

	static eError BURGER_API get_modification_time(
		const char* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT;
	static eError BURGER_API get_modification_time(
		Filename* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT;
	static eError BURGER_API get_creation_time(
		const char* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT;
	static eError BURGER_API get_creation_time(
		Filename* pFileName, TimeDate_t* pOutput) BURGER_NOEXCEPT;
	static uint_t BURGER_API does_file_exist(
		const char* pFileName) BURGER_NOEXCEPT;
	static uint_t BURGER_API does_file_exist(
		Filename* pFileName) BURGER_NOEXCEPT;

	static uint32_t BURGER_API get_creator_type(const char* pFileName);
	static uint32_t BURGER_API get_creator_type(Filename* pFileName);
	static uint32_t BURGER_API get_file_type(const char* pFileName);
	static uint32_t BURGER_API get_file_type(Filename* pFileName);
	static eError BURGER_API get_creator_and_file_type(
		const char* pFileName, uint32_t* pCreatorType, uint32_t* pFileType);
	static eError BURGER_API get_creator_and_file_type(
		Filename* pFileName, uint32_t* pCreatorType, uint32_t* pFileType);
	static eError BURGER_API set_creator_type(
		const char* pFileName, uint32_t uCreatorType);
	static eError BURGER_API set_creator_type(
		Filename* pFileName, uint32_t uCreatorType);
	static eError BURGER_API set_file_type(
		const char* pFileName, uint32_t uFileType);
	static eError BURGER_API set_file_type(
		Filename* pFileName, uint32_t uFileType);
	static eError BURGER_API set_creator_and_file_type(
		const char* pFileName, uint32_t uCreatorType, uint32_t uFileType);
	static eError BURGER_API set_creator_and_file_type(
		Filename* pFileName, uint32_t uCreatorType, uint32_t uFileType);

	static eError BURGER_API create_directory_path(
		const char* pFileName) BURGER_NOEXCEPT;
	static eError BURGER_API create_directory_path(
		Filename* pFileName) BURGER_NOEXCEPT;
	static eError BURGER_API create_directory_path_dirname(
		const char* pFileName);
	static eError BURGER_API create_directory_path_dirname(
		const Filename* pFileName);
	static eError BURGER_API delete_file(const char* pFileName) BURGER_NOEXCEPT;
	static eError BURGER_API delete_file(Filename* pFileName) BURGER_NOEXCEPT;
	static eError BURGER_API rename_file(
		const char* pNewName, const char* pOldName) BURGER_NOEXCEPT;
	static eError BURGER_API rename_file(
		Filename* pNewName, Filename* pOldName) BURGER_NOEXCEPT;
	static eError BURGER_API change_OS_directory(const char* pDirName);
	static eError BURGER_API change_OS_directory(Filename* pDirName);
	static FILE* BURGER_API open_file(
		const char* pFileName, const char* pType) BURGER_NOEXCEPT;
	static FILE* BURGER_API open_file(
		Filename* pFileName, const char* pType) BURGER_NOEXCEPT;
	static eError BURGER_API copy_file(
		const char* pDestName, const char* pSourceName) BURGER_NOEXCEPT;
	static eError BURGER_API copy_file(
		Filename* pDestName, Filename* pSourceName) BURGER_NOEXCEPT;
	static eError BURGER_API save_file(const char* pFileName, const void* pData,
		uintptr_t uLength) BURGER_NOEXCEPT;
	static eError BURGER_API save_file(Filename* pFileName, const void* pData,
		uintptr_t uLength) BURGER_NOEXCEPT;
	static eError BURGER_API save_text_file(const char* pFileName,
		const void* pData, uintptr_t uLength) BURGER_NOEXCEPT;
	static eError BURGER_API save_text_file(Filename* pFileName,
		const void* pData, uintptr_t uLength) BURGER_NOEXCEPT;
	static void* BURGER_API load_file(
		const char* pFileName, uintptr_t* pLength) BURGER_NOEXCEPT;
	static void* BURGER_API load_file(
		Filename* pFileName, uintptr_t* pLength) BURGER_NOEXCEPT;

#if defined(BURGER_DARWIN) || defined(DOXYGEN)
	static BURGER_INLINE const char* get_boot_name(void) BURGER_NOEXCEPT
	{
		return g_pFileManager->m_pBootName;
	}
	static BURGER_INLINE uintptr_t get_boot_name_length(void) BURGER_NOEXCEPT
	{
		return g_pFileManager->m_uBootNameSize;
	}
#endif

#if (defined(BURGER_UNIX) && !defined(BURGER_DARWIN)) || defined(DOXYGEN)
	static BURGER_INLINE const String* get_volume_prefix(void) BURGER_NOEXCEPT
	{
		return &g_pFileManager->m_VolumePrefix;
	}
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
	static short BURGER_API OpenResourceFile(
		const char* pFileName, char uPermission);
	static uint_t BURGER_API CreateResourceFIle(const char* pFileName);
#endif

#if (defined(BURGER_UNIX) && !defined(BURGER_DARWIN)) || defined(DOXYGEN)
	static const char* BURGER_API UnixFindProcFolder(void) BURGER_NOEXCEPT;
	static void BURGER_API UnixFind_etc_mtab(
		String& rOutput, const char* pProcPath) BURGER_NOEXCEPT;
	static void BURGER_API UnixParseFSTypes(
		ClassArray<String>& rFSTypes, const char* pProcPath) BURGER_NOEXCEPT;
	static void BURGER_API UnixParseFStab(
		ClassArray<String>& rFSTabs) BURGER_NOEXCEPT;
	static void BURGER_API UnixParseMTab(ClassArray<String>& rVolumePaths,
		const char* pEtcMtab,
		const ClassArray<String>& rFSTypes) BURGER_NOEXCEPT;
#endif
#if defined(BURGER_UNIX) || defined(DOXYGEN)
	static void BURGER_API unix_find_volumes_folder(
		String* pOutput) BURGER_NOEXCEPT;
#endif
};

class FileManagerSimple {
	BURGER_DISABLE_COPY(FileManagerSimple);

public:
	FileManagerSimple() BURGER_NOEXCEPT
	{
		FileManager::initialize();
	}
	~FileManagerSimple()
	{
		FileManager::shut_down();
	}
};
}
/* END */

#endif
