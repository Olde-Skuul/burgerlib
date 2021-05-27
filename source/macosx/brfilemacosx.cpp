/***************************************

    MacOS version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"

#if defined(BURGER_MACOSX) || defined(DOXYGEN)

/***************************************

	MacOS version of Burger::File
	Copyright Rebecca Ann Heineman

***************************************/

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>

/***************************************

	\brief Open a file using a Burger::Filename

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a Burger::Filename object
	\param eAccess Enumeration on permissions requested on the opened file
	\return File::OKAY if no error, error code if not.
	\sa Open(const char *, eFileAccess) and File(const char *,eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::Open(Filename* pFileName, eFileAccess eAccess) BURGER_NOEXCEPT
{
	Close();
	eAccess = static_cast<eFileAccess>(eAccess & 3);

	static const int g_Permissions[4] = {O_RDONLY, O_WRONLY | O_CREAT | O_TRUNC,
		O_WRONLY | O_CREAT, O_RDWR | O_CREAT};
	int fp = open(pFileName->GetNative(), g_Permissions[eAccess], 0666);
	uint_t uResult = FILENOTFOUND;
	if (fp != -1) {
		m_pFile = reinterpret_cast<void*>(fp);
		uResult = OKAY;
		if (eAccess == APPEND) {
			uResult = SetMarkAtEOF();
		}
	}
	return static_cast<Burger::eError>(uResult);
}

/***************************************

	\brief Close any open file

	Close any previously opened file

	\return File::OKAY if no error, error code if not.
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::Close(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
		int eClose = close(fp);
		if (eClose == -1) {
			uResult = kErrorIO;
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
	\return 0 if error or an empty file. Non-zero is the size of the file in
		bytes.
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

uintptr_t BURGER_API Burger::File::GetSize(void)
{
	uintptr_t uSize = 0;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
		struct stat MyStat;
		int iError = fstat(fp, &MyStat);
		if (iError != -1) {
#if defined(BURGER_64BITCPU)
			uSize = MyStat.st_size;
#else
			if (static_cast<uint64_t>(MyStat.st_size) <=
				static_cast<uint64_t>(0xFFFFFFFFU)) {
				uSize = static_cast<uintptr_t>(MyStat.st_size);
			} else {
				uSize = 0xFFFFFFFFU;
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
	\return Number of bytes read (Can be less than what was requested due to EOF
		or read errors)
	\sa Write(const void *,uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::Read(void* pOutput, uintptr_t uSize)
{
	uintptr_t uResult = 0;
	if (uSize && pOutput) {
		int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp) {
			size_t uRead =
				static_cast<size_t>(read(fp, pOutput, uSize)); // Read data
			if (uRead == -1) {
				uRead = 0;
			}
			uResult = static_cast<uintptr_t>(uRead);
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
	\return Number of bytes written (Can be less than what was requested due to
		EOF or write errors)
	\sa Read(void *,uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::Write(const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uSize && pInput) {
		int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp) {
			size_t uWrite =
				static_cast<size_t>(write(fp, pInput, uSize)); // Write data
			if (uWrite == -1) {
				uWrite = 0;
			}
			uResult = static_cast<uintptr_t>(uWrite);
		}
	}
	return uResult;
}

/***************************************

	\brief Get the current file mark

	If a file is open, query the operating system for the location of the file
	mark for future reads or writes.

	\return Current file mark or zero if an error occurred
	\sa Write(const void *,uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::GetMark(void)
{
	uintptr_t uMark = 0;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
		off_t lCurrentMark = lseek(fp, 0, SEEK_CUR);
		if (lCurrentMark != -1) {
			uMark = static_cast<uintptr_t>(lCurrentMark);
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

Burger::eError BURGER_API Burger::File::SetMark(uintptr_t uMark)
{
	eError uResult = kErrorNotInitialized;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
		off_t lCurrentMark = lseek(fp, uMark, SEEK_SET);
		if (lCurrentMark != -1) {
			uResult = kErrorNone;
		} else {
			uResult = kErrorOutOfBounds;
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

uint_t BURGER_API Burger::File::SetMarkAtEOF(void)
{
	uint_t uResult = INVALID_MARK;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
		off_t lCurrentMark = lseek(fp, 0, SEEK_END);
		if (lCurrentMark != -1) {
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was last modified

	If a file is open, query the operating system for the last time
	the file was modified.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file
		modification time

	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or
		other codes for errors
	\sa GetCreationTime() or SetModificationTime()

***************************************/

uint_t BURGER_API Burger::File::GetModificationTime(TimeDate_t* pOutput)
{
	uint_t uResult = FILENOTFOUND;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
		struct stat MyStat;
		int iError = fstat(fp, &MyStat);
		if (iError != -1) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyStat.st_mtimespec);
			uResult = OKAY;
		}
	}
	if (uResult != OKAY) {
		pOutput->Clear();
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was created

	If a file is open, query the operating system for the time the file was
	created.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file creation
		time

	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or
		other codes for errors

	\sa GetModificationTime() or SetCreationTime()

***************************************/

uint_t BURGER_API Burger::File::GetCreationTime(TimeDate_t* pOutput)
{
	uint_t uResult = FILENOTFOUND;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
		// Is fstat64 supported?
#if _POSIX_VERSION >= 200112L
		struct stat64 MyStat;
		int iError = fstat64(fp, &MyStat);
		if (iError != -1) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyStat.st_birthtimespec);
			uResult = OKAY;
		}
#endif
	}
	if (uResult != OKAY) {
		pOutput->Clear();
	}
	return uResult;
}

/***************************************

	\brief Set the time the file was last modified

	If a file is open, call the operating system to set the file modification
	time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file
		modification time

	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or
		other codes for errors

	\sa SetCreationTime() or GetModificationTime()

***************************************/

uint_t BURGER_API Burger::File::SetModificationTime(const TimeDate_t* pInput)
{
	uint_t uResult = FILENOTFOUND;
	uintptr_t NewTime;
	if (!pInput->StoreTimeT(&NewTime)) {
		int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp) {
			struct stat MyStat;
			int iError = fstat(fp, &MyStat);
			if (iError != -1) {
				timeval Array[2];
				Array[0].tv_sec = MyStat.st_atimespec.tv_sec; // Access time
				Array[0].tv_usec =
					static_cast<uint32_t>(MyStat.st_atimespec.tv_nsec / 1000);
				Array[1].tv_sec =
					static_cast<time_t>(NewTime); // Modification time
				Array[1].tv_usec = pInput->m_usMilliseconds * 1000;
				iError = futimes(fp, Array);
				if (iError != -1) {
					uResult = File::OKAY;
				}
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Set the time the file was created

	If a file is open, call the operating system to set the file creation time
	to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file
		creation time

	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or
		other codes for errors

	\sa SetModificationTime() or GetCreationTime()

***************************************/

uint_t BURGER_API Burger::File::SetCreationTime(const TimeDate_t* pInput)
{
	uint_t uResult = NOT_IMPLEMENTED;
#if 0
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
#else
	pInput = NULL;
#endif
	return uResult;
}

/*! ************************************

	\fn Burger::FileManager::GetBootName()
	\brief Return the boot volume name

	The boot volume is a UTF8 "C" string in the format of ":Foobar" (A leading
	colon and no ending colon)

	\note Available only on MacOSX

	\return const char * to the boot volume name.
	\sa Burger::FileManager::GetBootNameSize()

***************************************/

/*! ************************************

	\fn Burger::FileManager::GetBootNameSize()
	\brief Return the length of the boot volume name

	The boot volume is a UTF8 "C" string in the format of ":Foobar" (A leading
	colon and no ending colon).

	\note Available only on MacOSX

	\return Number of bytes in length of the boot volume name
	\sa Burger::FileManager::GetBootName()

***************************************/

#endif
