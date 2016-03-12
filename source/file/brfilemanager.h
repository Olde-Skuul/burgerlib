/***************************************

	File Manager Class

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
	static const Word cMaxQueue = 128;	///< Maximum number of pending IO events (Power of 2)
	enum {
		PREFIXCURRENT=8,		///< 8: Current working directory at application launch
		PREFIXAPPLICATION=9,	///< 9: Directory where the application executable resides
		PREFIXBOOT=32,			///< "*:" Boot volume prefix number
		PREFIXPREFS=33,			///< "@:" User preferences folder prefix number
		PREFIXSYSTEM=34,		///< "$:" System volume folder prefix number
		PREFIXMAX=35,			///< Maximum number of pathname prefixes
		PREFIXINVALID=999		///< Illegal prefix number
	};
	enum eIOCommand {
		IOCOMMAND_INVALID,	///< Invalid command (Must be zero)
		IOCOMMAND_OPEN,		///< Pending open file command
		IOCOMMAND_CLOSE,	///< Pending close file command
		IOCOMMAND_READ,		///< Pending read file command
		IOCOMMAND_WRITE,	///< Pending write file command
		IOCOMMAND_SEEK,		///< Pending seek file command
		IOCOMMAND_SEEKEOF,	///< Pending seek to end of file command
		IOCOMMAND_CALLBACK,	///< Pending completion callback command
		IOCOMMAND_SYNC,		///< Pending sync command
		IOCOMMAND_ENDTHREAD	///< Pending end thread command
	};
	struct Queue_t {
		File *m_pFile;				///< Pointer to the file object that this Queue_t is attached to
		void *m_pBuffer;			///< Pointer to the I/O buffer or callback pointer
		WordPtr m_uLength;			///< Value to attach to the command
		eIOCommand m_uIOCommand;	///< IO Command
	};
	typedef	void (BURGER_API *ProcCallback)(Queue_t *pQueue);

private:
	FileManager();
	~FileManager();
	void BURGER_API FlushIO(void) {}
	void BURGER_API WaitUntilQueueHasSpace(void) {}
	void BURGER_API AddQueue(File *pFile,eIOCommand uIOCommand,void *pBuffer,WordPtr uLength);
	void BURGER_API Sync(File *pFile);
	static WordPtr BURGER_API QueueHandler(void *pData);

	Semaphore m_PingIOThread;			///< Semaphore to ping the IO thread
	Semaphore m_IOThreadSync;			///< Semaphore to get a reply from the IO thread for syncing
	Thread m_Thread;					///< Worker thread record pointer
	volatile Word32 m_uQueueStart;		///< Index to the queue start
	volatile Word32 m_uQueueEnd;		///< Index to the end of the queue
	const char *m_pPrefix[PREFIXMAX];	///< Array of prefix strings
	Queue_t m_IOQueue[cMaxQueue];		///< Queue of IO events

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
	Word8 m_bAllowed;					///< True if MSDOS has long filename support (MSDOS Only)
#endif

#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	Word m_uBootNameSize;	///< Length of the boot volume name (MACOSX or iOS only)
	char *m_pBootName;		///< Boot volume name in the format ":FooBar" (Zero terminated) (MACOSX or iOS only)
#endif

	static FileManager *g_pFileManager;	///< Global instance of the file manager

public:
	static void BURGER_API Init(void);
	static void BURGER_API Shutdown(void);
#if defined(BURGER_MSDOS)
	static Word BURGER_API AreLongFilenamesAllowed(void);
#else
	static BURGER_INLINE Word AreLongFilenamesAllowed(void) { return TRUE; }
#endif
	static Word BURGER_API GetVolumeName(Filename *pOutput,Word uVolumeNum);
	static Word BURGER_API GetVolumeNumber(const char *pInput);
	static void BURGER_API DefaultPrefixes(void);
	static Word BURGER_API GetPrefix(Filename *pOutput,Word uPrefixNum);
	static Word BURGER_API SetPrefix(Word uPrefixNum,const char *pPrefixName);
	static Word BURGER_API SetPrefix(Word uPrefixNum,Filename *pPrefixName);
	static void BURGER_API PopPrefix(Word uPrefixNum);
	static Word BURGER_API GetModificationTime(const char *pFileName,TimeDate_t *pOutput);
	static Word BURGER_API GetModificationTime(Filename *pFileName,TimeDate_t *pOutput);
	static Word BURGER_API GetCreationTime(const char *pFileName,TimeDate_t *pOutput);
	static Word BURGER_API GetCreationTime(Filename *pFileName,TimeDate_t *pOutput);
	static Word BURGER_API DoesFileExist(const char *pFileName);
	static Word BURGER_API DoesFileExist(Filename *pFileName);
	static Word32 BURGER_API GetAuxType(const char *pFileName);
	static Word32 BURGER_API GetAuxType(Filename *pFileName);
	static Word32 BURGER_API GetFileType(const char *pFileName);
	static Word32 BURGER_API GetFileType(Filename *pFileName);
	static Word BURGER_API GetFileAndAuxType(const char *pFileName,Word32 *pFileType,Word32 *pAuxType);
	static Word BURGER_API GetFileAndAuxType(Filename *pFileName,Word32 *pFileType,Word32 *pAuxType);
	static Word BURGER_API SetAuxType(const char *pFileName,Word32 uAuxType);
	static Word BURGER_API SetAuxType(Filename *pFileName,Word32 uAuxType);
	static Word BURGER_API SetFileType(const char *pFileName,Word32 uFileType);
	static Word BURGER_API SetFileType(Filename *pFileName,Word32 uFileType);
	static Word BURGER_API SetFileAndAuxType(const char *pFileName,Word32 uFileType,Word32 uAuxType);
	static Word BURGER_API SetFileAndAuxType(Filename *pFileName,Word32 uFileType,Word32 uAuxType);
	static Word BURGER_API CreateDirectoryPath(const char *pFileName);
	static Word BURGER_API CreateDirectoryPath(Filename *pFileName);
	static Word BURGER_API CreateDirectoryPathDirName(const char *pFileName);
	static Word BURGER_API CreateDirectoryPathDirName(Filename *pFileName);
	static Word BURGER_API DeleteFile(const char *pFileName);
	static Word BURGER_API DeleteFile(Filename *pFileName);
	static Word BURGER_API RenameFile(const char *pNewName,const char *pOldName);
	static Word BURGER_API RenameFile(Filename *pNewName,Filename *pOldName);
	static Word BURGER_API ChangeOSDirectory(const char *pDirName);
	static Word BURGER_API ChangeOSDirectory(Filename *pDirName);
	static FILE *BURGER_API OpenFile(const char *pFileName,const char *pType);
	static FILE *BURGER_API OpenFile(Filename *pFileName,const char *pType);
	static Word BURGER_API CopyFile(const char *pDestName,const char *pSourceName);
	static Word BURGER_API CopyFile(Filename *pDestName,Filename *pSourceName);
	static Word BURGER_API SaveFile(const char *pFileName,const void *pData,WordPtr uLength);
	static Word BURGER_API SaveFile(Filename *pFileName,const void *pData,WordPtr uLength);
	static Word BURGER_API SaveTextFile(const char *pFileName,const void *pData,WordPtr uLength);
	static Word BURGER_API SaveTextFile(Filename *pFileName,const void *pData,WordPtr uLength);
	static void * BURGER_API LoadFile(const char *pFileName,WordPtr *pLength);
	static void * BURGER_API LoadFile(Filename *pFileName,WordPtr *pLength);

#if (defined(BURGER_MACOSX) || defined(BURGER_IOS)) || defined(DOXYGEN)
	static BURGER_INLINE const char *GetBootName(void) { return g_pFileManager->m_pBootName; }
	static BURGER_INLINE Word GetBootNameSize(void) { return g_pFileManager->m_uBootNameSize; }
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
	static short BURGER_API OpenResourceFile(const char *pFileName,char uPermission);
	static Word BURGER_API CreateResourceFIle(const char *pFileName);
#endif
};

class FileManagerSimple {
public:
	FileManagerSimple() { FileManager::Init(); }
	~FileManagerSimple() { FileManager::Shutdown(); }
};
}
/* END */

#endif

