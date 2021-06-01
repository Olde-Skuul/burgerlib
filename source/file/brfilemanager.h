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
	enum {
		/** 8: Current working directory at application launch */
		PREFIXCURRENT = 8,
		/** 9: Directory where the application executable resides */
		PREFIXAPPLICATION = 9,
		/** "*:" Boot volume prefix number */
		PREFIXBOOT = 32,
		/** "@:" User preferences folder prefix number */
		PREFIXPREFS = 33,
		/** "$:" System volume folder prefix number */
		PREFIXSYSTEM = 34,
		/** Maximum number of pathname prefixes */
		PREFIXMAX = 35,
		/** Illegal prefix number */
		PREFIXINVALID = 999
	};
	enum eIOCommand {
		IOCOMMAND_INVALID,  ///< Invalid command (Must be zero)
		IOCOMMAND_OPEN,     ///< Pending open file command
		IOCOMMAND_CLOSE,    ///< Pending close file command
		IOCOMMAND_READ,     ///< Pending read file command
		IOCOMMAND_WRITE,    ///< Pending write file command
		IOCOMMAND_SEEK,     ///< Pending seek file command
		IOCOMMAND_SEEKEOF,  ///< Pending seek to end of file command
		IOCOMMAND_CALLBACK, ///< Pending completion callback command
		IOCOMMAND_SYNC,     ///< Pending sync command
		IOCOMMAND_ENDTHREAD ///< Pending end thread command
	};
	struct Queue_t {
		File* m_pFile;   ///< Pointer to the file object that this Queue_t is
						 ///< attached to
		void* m_pBuffer; ///< Pointer to the I/O buffer or callback pointer
		uintptr_t m_uLength;     ///< Value to attach to the command
		eIOCommand m_uIOCommand; ///< IO Command
	};
	typedef void(BURGER_API* ProcCallback)(Queue_t* pQueue);

private:
	FileManager();
	~FileManager();
	void BURGER_API FlushIO(void) BURGER_NOEXCEPT {}
	void BURGER_API WaitUntilQueueHasSpace(void) BURGER_NOEXCEPT {}
	void BURGER_API AddQueue(
		File* pFile, eIOCommand uIOCommand, void* pBuffer, uintptr_t uLength);
	void BURGER_API Sync(File* pFile);
	static uintptr_t BURGER_API QueueHandler(void* pData);

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
	const char* m_pPrefix[PREFIXMAX];
	/** Queue of IO events */
	Queue_t m_IOQueue[kMaxQueue];

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
	/** True if MSDOS has long filename support (MSDOS Only) */
	uint8_t m_bAllowed;
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
	static void BURGER_API Init(void);
	static void BURGER_API Shutdown(void);
#if defined(BURGER_MSDOS)
	static uint_t BURGER_API AreLongFilenamesAllowed(void) BURGER_NOEXCEPT;
#else
	static BURGER_INLINE uint_t AreLongFilenamesAllowed(void) BURGER_NOEXCEPT
	{
		return TRUE;
	}
#endif
	static uint_t BURGER_API GetVolumeName(
		Filename* pOutput, uint_t uVolumeNum) BURGER_NOEXCEPT;
	static uint_t BURGER_API GetVolumeNumber(const char* pInput);
	static void BURGER_API DefaultPrefixes(void);
	static uint_t BURGER_API GetPrefix(
		Filename* pOutput, uint_t uPrefixNum) BURGER_NOEXCEPT;
	static uint_t BURGER_API SetPrefix(
		uint_t uPrefixNum, const char* pPrefixName) BURGER_NOEXCEPT;
	static uint_t BURGER_API SetPrefix(
		uint_t uPrefixNum, Filename* pPrefixName) BURGER_NOEXCEPT;
	static void BURGER_API PopPrefix(uint_t uPrefixNum) BURGER_NOEXCEPT;
	static uint_t BURGER_API GetModificationTime(
		const char* pFileName, TimeDate_t* pOutput);
	static uint_t BURGER_API GetModificationTime(
		Filename* pFileName, TimeDate_t* pOutput);
	static uint_t BURGER_API GetCreationTime(
		const char* pFileName, TimeDate_t* pOutput);
	static uint_t BURGER_API GetCreationTime(
		Filename* pFileName, TimeDate_t* pOutput);
	static uint_t BURGER_API DoesFileExist(
		const char* pFileName) BURGER_NOEXCEPT;
	static uint_t BURGER_API DoesFileExist(Filename* pFileName) BURGER_NOEXCEPT;
	static uint32_t BURGER_API GetAuxType(const char* pFileName);
	static uint32_t BURGER_API GetAuxType(Filename* pFileName);
	static uint32_t BURGER_API GetFileType(const char* pFileName);
	static uint32_t BURGER_API GetFileType(Filename* pFileName);
	static uint_t BURGER_API GetFileAndAuxType(
		const char* pFileName, uint32_t* pFileType, uint32_t* pAuxType);
	static uint_t BURGER_API GetFileAndAuxType(
		Filename* pFileName, uint32_t* pFileType, uint32_t* pAuxType);
	static uint_t BURGER_API SetAuxType(
		const char* pFileName, uint32_t uAuxType);
	static uint_t BURGER_API SetAuxType(Filename* pFileName, uint32_t uAuxType);
	static uint_t BURGER_API SetFileType(
		const char* pFileName, uint32_t uFileType);
	static uint_t BURGER_API SetFileType(
		Filename* pFileName, uint32_t uFileType);
	static uint_t BURGER_API SetFileAndAuxType(
		const char* pFileName, uint32_t uFileType, uint32_t uAuxType);
	static uint_t BURGER_API SetFileAndAuxType(
		Filename* pFileName, uint32_t uFileType, uint32_t uAuxType);
	static uint_t BURGER_API CreateDirectoryPath(const char* pFileName);
	static uint_t BURGER_API CreateDirectoryPath(Filename* pFileName);
	static uint_t BURGER_API CreateDirectoryPathDirName(const char* pFileName);
	static uint_t BURGER_API CreateDirectoryPathDirName(Filename* pFileName);
	static uint_t BURGER_API DeleteFile(const char* pFileName) BURGER_NOEXCEPT;
	static uint_t BURGER_API DeleteFile(Filename* pFileName) BURGER_NOEXCEPT;
	static uint_t BURGER_API RenameFile(
		const char* pNewName, const char* pOldName);
	static uint_t BURGER_API RenameFile(Filename* pNewName, Filename* pOldName);
	static uint_t BURGER_API ChangeOSDirectory(const char* pDirName);
	static uint_t BURGER_API ChangeOSDirectory(Filename* pDirName);
	static FILE* BURGER_API OpenFile(const char* pFileName, const char* pType);
	static FILE* BURGER_API OpenFile(Filename* pFileName, const char* pType);
	static uint_t BURGER_API CopyFile(
		const char* pDestName, const char* pSourceName);
	static uint_t BURGER_API CopyFile(
		Filename* pDestName, Filename* pSourceName);
	static uint_t BURGER_API SaveFile(
		const char* pFileName, const void* pData, uintptr_t uLength);
	static uint_t BURGER_API SaveFile(
		Filename* pFileName, const void* pData, uintptr_t uLength);
	static uint_t BURGER_API SaveTextFile(
		const char* pFileName, const void* pData, uintptr_t uLength);
	static uint_t BURGER_API SaveTextFile(
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
