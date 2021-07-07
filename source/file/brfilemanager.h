/***************************************

	File Manager Class

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

#ifndef __BRCRITICALSECTION_H__
#include "brcriticalsection.h"
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
	friend class File;

public:
	/** Maximum number of pending IO events(Power of 2) */
	static const uint_t kMaxQueue = 128;

	/** Predefined pathname prefixes */
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

	/** Asynchronous file I/O commands */
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

private:
	FileManager() BURGER_NOEXCEPT;
	~FileManager();
	void BURGER_API FlushIO(void) BURGER_NOEXCEPT {}
	void BURGER_API WaitUntilQueueHasSpace(void) BURGER_NOEXCEPT {}
	void BURGER_API AddQueue(File* pFile, eIOCommand uIOCommand, void* pBuffer,
		uintptr_t uLength) BURGER_NOEXCEPT;
	void BURGER_API Sync(File* pFile);
	static uintptr_t BURGER_API QueueHandler(void* pData);

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
	void BURGER_API DetectMSDOSVersion(void) BURGER_NOEXCEPT;
#endif

	/** Semaphore to ping the IO thread */
	Semaphore m_PingIOThread;
	/** Semaphore to get a reply from the IO thread for syncing */
	Semaphore m_IOThreadSync;
	/** Worker thread record pointer */
	Thread m_Thread;
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

#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	/** Length of the boot volume name (MACOSX or iOS only) */
	uint_t m_uBootNameSize;
	/** Boot volume name in the format ":FooBar" (Zero terminated) (MACOSX or
	 * iOS only) */
	char* m_pBootName;
#endif

	/** Global instance of the file manager */
	static FileManager* g_pFileManager;

public:
	static eError BURGER_API Init(void);
	static void BURGER_API Shutdown(void) BURGER_NOEXCEPT;

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
	static uint_t BURGER_API MSDOS_HasLongFilenames(void) BURGER_NOEXCEPT;
	static uint_t BURGER_API MSDOS_GetOSVersion(void) BURGER_NOEXCEPT;
	static uint_t BURGER_API MSDos_GetOSTrueVersion(void) BURGER_NOEXCEPT;
	static const char* BURGER_API MSDOS_GetName(void) BURGER_NOEXCEPT;
	static uint_t BURGER_API MSDos_GetFlavor(void) BURGER_NOEXCEPT;
	static eError BURGER_API MSDos_Expand8_3Filename(String* pInput) BURGER_NOEXCEPT;
	static eError BURGER_API MSDos_ConvertTo8_3Filename(
		String* pInput) BURGER_NOEXCEPT;
#endif

	static BURGER_INLINE uint_t IsUTF8FileSystem(void) BURGER_NOEXCEPT
	{
#if defined(BURGER_MSDOS) || defined(BURGER_XBOX) || \
	defined(BURGER_XBOX360) || defined(BURGER_MAC)
		return FALSE;
#else
		return TRUE;
#endif
	}
	static eError BURGER_API GetVolumeName(
		Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT;
	static uint_t BURGER_API GetVolumeNumber(
		const char* pInput) BURGER_NOEXCEPT;
	static eError BURGER_API DefaultPrefixes(void) BURGER_NOEXCEPT;
	static eError BURGER_API GetPrefix(
		Filename* pOutput, uint_t uPrefixNum) BURGER_NOEXCEPT;
	static eError BURGER_API SetPrefix(
		uint_t uPrefixNum, const char* pPrefixName) BURGER_NOEXCEPT;
	static eError BURGER_API SetPrefix(
		uint_t uPrefixNum, const Filename* pPrefixName) BURGER_NOEXCEPT;
	static eError BURGER_API PopPrefix(uint_t uPrefixNum) BURGER_NOEXCEPT;
	static eError BURGER_API GetModificationTime(
		const char* pFileName, TimeDate_t* pOutput);
	static eError BURGER_API GetModificationTime(
		Filename* pFileName, TimeDate_t* pOutput);
	static eError BURGER_API GetCreationTime(
		const char* pFileName, TimeDate_t* pOutput);
	static eError BURGER_API GetCreationTime(
		Filename* pFileName, TimeDate_t* pOutput);
	static uint_t BURGER_API DoesFileExist(
		const char* pFileName) BURGER_NOEXCEPT;
	static uint_t BURGER_API DoesFileExist(Filename* pFileName) BURGER_NOEXCEPT;
	static uint32_t BURGER_API GetAuxType(const char* pFileName);
	static uint32_t BURGER_API GetAuxType(Filename* pFileName);
	static uint32_t BURGER_API GetFileType(const char* pFileName);
	static uint32_t BURGER_API GetFileType(Filename* pFileName);
	static eError BURGER_API GetFileAndAuxType(
		const char* pFileName, uint32_t* pFileType, uint32_t* pAuxType);
	static eError BURGER_API GetFileAndAuxType(
		Filename* pFileName, uint32_t* pFileType, uint32_t* pAuxType);
	static eError BURGER_API SetAuxType(
		const char* pFileName, uint32_t uAuxType);
	static eError BURGER_API SetAuxType(Filename* pFileName, uint32_t uAuxType);
	static eError BURGER_API SetFileType(
		const char* pFileName, uint32_t uFileType);
	static eError BURGER_API SetFileType(
		Filename* pFileName, uint32_t uFileType);
	static eError BURGER_API SetFileAndAuxType(
		const char* pFileName, uint32_t uFileType, uint32_t uAuxType);
	static eError BURGER_API SetFileAndAuxType(
		Filename* pFileName, uint32_t uFileType, uint32_t uAuxType);
	static eError BURGER_API CreateDirectoryPath(const char* pFileName);
	static eError BURGER_API CreateDirectoryPath(Filename* pFileName);
	static eError BURGER_API CreateDirectoryPathDirName(const char* pFileName);
	static eError BURGER_API CreateDirectoryPathDirName(Filename* pFileName);
	static eError BURGER_API DeleteFile(const char* pFileName) BURGER_NOEXCEPT;
	static eError BURGER_API DeleteFile(Filename* pFileName) BURGER_NOEXCEPT;
	static eError BURGER_API RenameFile(
		const char* pNewName, const char* pOldName);
	static eError BURGER_API RenameFile(Filename* pNewName, Filename* pOldName);
	static eError BURGER_API ChangeOSDirectory(const char* pDirName);
	static eError BURGER_API ChangeOSDirectory(Filename* pDirName);
	static FILE* BURGER_API OpenFile(const char* pFileName, const char* pType);
	static FILE* BURGER_API OpenFile(Filename* pFileName, const char* pType);
	static eError BURGER_API CopyFile(
		const char* pDestName, const char* pSourceName);
	static eError BURGER_API CopyFile(
		Filename* pDestName, Filename* pSourceName);
	static eError BURGER_API SaveFile(
		const char* pFileName, const void* pData, uintptr_t uLength);
	static eError BURGER_API SaveFile(
		Filename* pFileName, const void* pData, uintptr_t uLength);
	static eError BURGER_API SaveTextFile(
		const char* pFileName, const void* pData, uintptr_t uLength);
	static eError BURGER_API SaveTextFile(
		Filename* pFileName, const void* pData, uintptr_t uLength);
	static void* BURGER_API LoadFile(
		const char* pFileName, uintptr_t* pLength) BURGER_NOEXCEPT;
	static void* BURGER_API LoadFile(
		Filename* pFileName, uintptr_t* pLength) BURGER_NOEXCEPT;

#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	static BURGER_INLINE const char* GetBootName(void) BURGER_NOEXCEPT
	{
		return g_pFileManager->m_pBootName;
	}
	static BURGER_INLINE uint_t GetBootNameSize(void) BURGER_NOEXCEPT
	{
		return g_pFileManager->m_uBootNameSize;
	}
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
	static short BURGER_API OpenResourceFile(
		const char* pFileName, char uPermission);
	static uint_t BURGER_API CreateResourceFIle(const char* pFileName);
#endif
};

class FileManagerSimple {
public:
	FileManagerSimple() BURGER_NOEXCEPT
	{
		FileManager::Init();
	}
	~FileManagerSimple()
	{
		FileManager::Shutdown();
	}
};
}
/* END */

#endif
