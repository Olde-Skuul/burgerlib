/***************************************

	Windows version of Burger::File

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"
#if defined(BURGER_WINDOWS)
#include "brstring16.h"
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <Windows.h>

/***************************************

	\brief Open a file using a Burger::Filename

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a Burger::Filename object
	\param eAccess Enumeration on permissions requested on the opened file
	\return File::OKAY if no error, error code if not.
	\sa Open(const char *, eFileAccess) and File(const char *,eFileAccess)

***************************************/

Word Burger::File::Open(Filename *pFileName,eFileAccess eAccess)
{
	static const DWORD g_Access[4] = { GENERIC_READ,GENERIC_WRITE,GENERIC_WRITE,GENERIC_READ|GENERIC_WRITE };
	static const DWORD g_Share[4] = { FILE_SHARE_READ,0,0,0 };
	static const DWORD g_Creation[4] = { OPEN_EXISTING,CREATE_ALWAYS,OPEN_ALWAYS, OPEN_ALWAYS };
	Close();
	String16 FinalName(pFileName->GetNative());
	eAccess = static_cast<eFileAccess>(eAccess&3);
	HANDLE fp = CreateFileW(reinterpret_cast<const WCHAR *>(FinalName.GetPtr()),g_Access[eAccess],g_Share[eAccess],NULL,g_Creation[eAccess],/* FILE_ATTRIBUTE_NORMAL| */ FILE_FLAG_SEQUENTIAL_SCAN,NULL);
	Word uResult = FILENOTFOUND;
	if (fp!=INVALID_HANDLE_VALUE) {
		m_pFile = fp;
		uResult = OKAY;
		if (eAccess==APPEND) {
			uResult = SetMarkAtEOF();
		}
	}
	return uResult;
}

/***************************************

	\brief Close any open file

	Close any previously opened file

	\return File::OKAY if no error, error code if not.
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

Word Burger::File::Close(void)
{
	Word uResult = OKAY;
	HANDLE fp = m_pFile;
	if (fp) {
		BOOL bCloseResult = CloseHandle(fp);
		if (!bCloseResult) {
			uResult = IOERROR;
		}
		m_pFile = NULL;
	}
	return uResult;
}

/***************************************

	\brief Return the size of a file in bytes

	If a file is open, query the operating system for the size of the file
	in bytes.

	\note The return value is 32 bits wide on a 32 bit operating system, 64 bits
		wide on 64 bit operating systems
	\return 0 if error or an empty file. Non-zero is the size of the file in bytes.
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

WordPtr Burger::File::GetSize(void)
{
	WordPtr uSize = 0;
	HANDLE fp = m_pFile;
	if (fp) {
		LARGE_INTEGER uLongSize;
		BOOL bGetSizeResult = GetFileSizeEx(fp,&uLongSize);
		if (bGetSizeResult) {
#if defined(BURGER_64BITCPU)
			// 64 bit CPUs can handle really large files
			uSize = uLongSize.QuadPart;
#else
			// 32 bit CPUs can overflow
			if (uLongSize.HighPart) {
				uSize = 0xFFFFFFFFUL;
			} else {
				uSize = uLongSize.LowPart;
			}
#endif
		}
	}
	return uSize;
}

/***************************************

	\brief Read data from an open file

	If a file is open, perform a read operation. This function will fail
	if the file was not opened for read access.

	\param pOutput Pointer to a buffer of data to read from a file
	\param uSize Number of bytes to read
	\return Number of bytes read (Can be less than what was requested due to EOF or read errors)
	\sa Write(const void *,WordPtr)

***************************************/

WordPtr Burger::File::Read(void *pOutput,WordPtr uSize)
{
	WordPtr uResult = 0;
	if (uSize && pOutput) {
		HANDLE fp = m_pFile;
		if (fp) {
			DWORD uRead;
			// The function only allows 32 bits of data to be sent in a single call.
			// It's unlikely that 4GB of data will be read, however, the function can handle it.
			// :)
#if defined(BURGER_64BITCPU)
			do {
				// Size of the chunk must fit in a 32 bit value
				DWORD uChunk = (uSize<0xFFFF0000UL) ? static_cast<DWORD>(uSize) : 0xFFFF0000UL;
				BOOL bReadResult = ReadFile(fp,pOutput,uChunk,&uRead,NULL);
				if (!bReadResult) {
					// Error! Abort!
					break;
				}
				pOutput = static_cast<Word8 *>(pOutput)+uRead;
				uResult += uRead;
				uSize -= uRead;
				// Was the read smaller than the read request?
				if (uRead!=uChunk) {
					break;
				}
			} while (uSize);
#else
			// 32 bit CPUs have it easy. ;-)
			BOOL bReadResult = ReadFile(fp,pOutput,uSize,&uRead,NULL);
			if (bReadResult) {
				uResult = uRead;
			}
#endif
		}
	}
	return uResult;
}


/***************************************

	\brief Write data into an open file

	If a file is open, perform a write operation. This function will fail
	if the file was not opened for write access.

	\param pInput Pointer to a buffer of data to write to a file
	\param uSize Number of bytes to write
	\return Number of bytes written (Can be less than what was requested due to EOF or write errors)
	\sa Read(void *,WordPtr)

***************************************/

WordPtr Burger::File::Write(const void *pInput,WordPtr uSize)
{
	WordPtr uResult = 0;
	if (uSize && pInput) {
		HANDLE fp = m_pFile;
		if (fp) {
			DWORD uWrite;
			// The function only allows 32 bits of data to be sent in a single call.
			// It's unlikely that 4GB of data will be written, however, the function can handle it.
			// :)
#if defined(BURGER_64BITCPU)
			do {
				// Size of the chunk must fit in a 32 bit value
				DWORD uChunk = (uSize<0xFFFF0000UL) ? static_cast<DWORD>(uSize) : 0xFFFF0000UL;
				BOOL bWriteResult = WriteFile(fp,pInput,uChunk,&uWrite,NULL);
				if (!bWriteResult) {
					// Error! Abort!
					break;
				}
				pInput = static_cast<const Word8 *>(pInput)+uWrite;
				uResult += uWrite;
				uSize -= uWrite;
				// Was the write smaller than the write request?
				if (uWrite!=uChunk) {
					break;
				}
			} while (uSize);
#else
			// 32 bit CPUs have it easy. ;-)
			BOOL bWriteResult = WriteFile(fp,pInput,uSize,&uWrite,NULL);
			if (bWriteResult) {
				uResult = uWrite;
			}
#endif
		}
	}
	return uResult;
}


/***************************************

	\brief Get the current file mark

	If a file is open, query the operating system for the location
	of the file mark for future reads or writes.

	\return Current file mark or zero if an error occured
	\sa Write(const void *,WordPtr)

***************************************/

WordPtr Burger::File::GetMark(void)
{
	WordPtr uMark = 0;
	HANDLE fp = m_pFile;
	if (fp) {
		LARGE_INTEGER uNewPointer;
		LARGE_INTEGER uOldPointer;
		uNewPointer.QuadPart = 0;
		// Get the current file mark
		BOOL bPositionResult = SetFilePointerEx(fp,uNewPointer,&uOldPointer,FILE_CURRENT);
		// If no error, restore the old file mark
		if (bPositionResult) {
#if defined(BURGER_64BITCPU)
			uMark = uOldPointer.QuadPart;
#else
			if (uOldPointer.HighPart) {
				uMark = 0xFFFFFFFFUL;
			} else {
				uMark = uOldPointer.LowPart;
			}
#endif
		}
	}
	return uMark;
}

/***************************************

	\brief Set the current file mark

	If a file is open, set the read/write mark at the location passed.

	\param uMark Value to set the new file mark to.
	\return File::OKAY if successful, File::INVALID_MARK if not.
	\sa GetMark() or SetMarkAtEOF()

***************************************/

Word Burger::File::SetMark(WordPtr uMark)
{
	Word uResult = INVALID_MARK;
	HANDLE fp = m_pFile;
	if (fp) {
		LARGE_INTEGER uNewPointer;
		uNewPointer.QuadPart = uMark;
		// Get the current file mark
		BOOL bPositionResult = SetFilePointerEx(fp,uNewPointer,NULL,FILE_BEGIN);
		// If no error, restore the old file mark
		if (bPositionResult) {
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Set the current file mark at the end of the file

	If a file is open, set the read/write mark to the end of the file.

	\return File::OKAY if successful, File::INVALID_MARK if not.
	\sa GetMark() or SetMark()

***************************************/

Word Burger::File::SetMarkAtEOF(void)
{
	Word uResult = INVALID_MARK;
	HANDLE fp = m_pFile;
	if (fp) {
		LARGE_INTEGER uNewPointer;
		uNewPointer.QuadPart = 0;
		// Set the file mark to the end of the file
		BOOL bPositionResult = SetFilePointerEx(fp,uNewPointer,NULL,FILE_END);
		// Okay?
		if (bPositionResult) {
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was last modified

	If a file is open, query the operating system for the last time
	the file was modified.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file modification time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa GetCreationTime() or SetModificationTime()

***************************************/

Word Burger::File::GetModificationTime(TimeDate_t *pOutput)
{
	Word uResult = FILENOTFOUND;
	HANDLE fp = m_pFile;
	if (fp) {
		FILETIME ModificationTime;
		// Get the file modification time
		BOOL bFileInfoResult = GetFileTime(fp,NULL,&ModificationTime,NULL);
		// Okay?
		if (bFileInfoResult) {
			pOutput->Load(&ModificationTime);
			uResult = OKAY;
		}
	}
	return uResult;

}

/***************************************

	\brief Get the time the file was created

	If a file is open, query the operating system for the time
	the file was created.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file creation time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa GetModificationTime() or SetCreationTime()

***************************************/

Word Burger::File::GetCreationTime(TimeDate_t *pOutput)
{
	Word uResult = FILENOTFOUND;
	HANDLE fp = m_pFile;
	if (fp) {
		FILETIME CreationTime;
		// Get the file creation time
		BOOL bFileInfoResult = GetFileTime(fp,&CreationTime,NULL,NULL);
		// Okay?
		if (bFileInfoResult) {
			pOutput->Load(&CreationTime);
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Set the time the file was last modified

	If a file is open, call the operating system to set the file
	modification time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file modification time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa SetCreationTime() or GetModificationTime()

***************************************/

Word Burger::File::SetModificationTime(const TimeDate_t *pInput)
{
	Word uResult = FILENOTFOUND;
	HANDLE fp = m_pFile;
	if (fp) {
		FILETIME ModificationTime;
		pInput->Store(&ModificationTime);
		// Set the file modification time
		BOOL bFileInfoResult = SetFileTime(fp,NULL,&ModificationTime,NULL);
		if (bFileInfoResult) {
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Set the time the file was created

	If a file is open, call the operating system to set the file
	creation time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file creation time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa SetModificationTime() or GetCreationTime()

***************************************/

Word Burger::File::SetCreationTime(const TimeDate_t *pInput)
{
	Word uResult = FILENOTFOUND;
	HANDLE fp = m_pFile;
	if (fp) {
		FILETIME CreationTime;
		pInput->Store(&CreationTime);
		// Set the file creation time
		BOOL bFileInfoResult = SetFileTime(fp,&CreationTime,NULL,NULL);
		if (bFileInfoResult) {
			uResult = OKAY;
		}
	}
	return uResult;
}

#endif
