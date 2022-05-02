/***************************************

    Playstation Vita version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"

#if defined(BURGER_VITA) || defined(DOXYGEN)
#include "brmemoryfunctions.h"
#include <kernel.h>

/***************************************
 
	\brief Open a file using a Burger::Filename
 
	Close any previously opened file and open a new file.
 
	\param pFileName Pointer to a Burger::Filename object
	\param eAccess Enumeration on permissions requested on the opened file
	\return kErrorNone if no error, error code if not.
	\sa Open(const char *, eFileAccess) and File(const char *,eFileAccess)
 
***************************************/

Burger::eError BURGER_API Burger::File::Open(Filename *pFileName,eFileAccess eAccess) BURGER_NOEXCEPT
{
	Close();
	eAccess = static_cast<eFileAccess>(eAccess&3);
	
	static const int g_Permissions[4] = { SCE_O_RDONLY,SCE_O_WRONLY|SCE_O_CREAT|SCE_O_TRUNC,SCE_O_WRONLY|SCE_O_CREAT,SCE_O_RDWR|SCE_O_CREAT };
	int fp = sceIoOpen(pFileName->GetNative(),g_Permissions[eAccess],SCE_STM_RWU);
	uint_t uResult = kErrorFileNotFound;
	if (fp>SCE_OK) {
		m_pFile = reinterpret_cast<void *>(fp);	
		uResult = kErrorNone;
		if (eAccess==kAppend) {
			uResult = SetMarkAtEOF();
		}
	}
	return static_cast<Burger::eError>(uResult);
}

/***************************************

	\brief Close any open file

	Close any previously opened file

	\return kErrorNone if no error, error code if not.
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::Close(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp>SCE_OK) {
		int eClose = sceIoClose(fp);
		if (eClose==-1) {
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
	\return 0 if error or an empty file. Non-zero is the size of the file in bytes. 
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

uint64_t BURGER_API Burger::File::GetSize(void) BURGER_NOEXCEPT
{
	uint64_t uSize = 0;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp>SCE_OK) {
		SceIoStat MyStat;
		MemoryClear(&MyStat,sizeof(MyStat));
		int eError = sceIoGetstatByFd(fp,&MyStat);
		if (eError>=SCE_OK) {
			if (static_cast<uint64_t>(MyStat.st_size)<=static_cast<uint64_t>(0xFFFFFFFFU)) {
				uSize = static_cast<uintptr_t>(MyStat.st_size);
			} else {
				uSize = 0xFFFFFFFFU;
			}
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
	\sa Write(const void *,uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::Read(void *pOutput,uintptr_t uSize)
{
	uintptr_t uResult = 0;
	if (uSize && pOutput) {
		int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp>SCE_OK) {
			SceSSize uRead = sceIoRead(fp,pOutput,uSize);		// Read data
			if (uRead<0) {
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
	\return Number of bytes written (Can be less than what was requested due to EOF or write errors)
	\sa Read(void *,uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::Write(const void *pInput,uintptr_t uSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uSize && pInput) {
		int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp>SCE_OK) {
			SceSSize uWrite = sceIoWrite(fp,pInput,uSize);		// Write data
			if (uWrite<0) {
				uWrite = 0;
			}
			uResult = static_cast<uintptr_t>(uWrite);
		}
	}
	return uResult;
}


/***************************************

	\brief Get the current file mark

	If a file is open, query the operating system for the location
	of the file mark for future reads or writes.

	\return Current file mark or zero if an error occurred
	\sa Write(const void *,uintptr_t)

***************************************/

uint64_t BURGER_API Burger::File::GetMark(void) BURGER_NOEXCEPT
{
	uint64_t uMark = 0;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp>SCE_OK) {
		long lCurrentMark = sceIoLseek32(fp,0,SCE_SEEK_CUR);
		if (lCurrentMark>=0) {
			uMark = static_cast<uint64_t>(lCurrentMark);
		}
	}
	return uMark;
}

/***************************************

	\brief Set the current file mark

	If a file is open, set the read/write mark at the location passed.

	\param uMark Value to set the new file mark to.
	\return kErrorNone if successful, kErrorOutOfBounds if not.
	\sa GetMark() or SetMarkAtEOF()

***************************************/

Burger::eError BURGER_API Burger::File::SetMark(uint64_t uMark) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotInitialized;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp>SCE_OK) {
		long lCurrentMark = sceIoLseek32(fp,uMark,SCE_SEEK_SET);
		if (lCurrentMark>=0) {
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

	\return kErrorNone if successful, kErrorOutOfBounds if not.
	\sa GetMark() or SetMark()

***************************************/

Burger::eError BURGER_API Burger::File::SetMarkAtEOF(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorOutOfBounds;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp>SCE_OK) {
		long lCurrentMark = sceIoLseek32(fp,0,SCE_SEEK_END);
		if (lCurrentMark>=0) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was last modified

	If a file is open, query the operating system for the last time
	the file was modified.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file modification time
	\return kErrorNone if successful, kErrorNotSupportedOnThisPlatform if not available or other codes for errors
	\sa GetCreationTime() or SetModificationTime()

***************************************/

Burger::eError BURGER_API Burger::File::GetModificationTime(TimeDate_t *pOutput) BURGER_NOEXCEPT
{
	eError uResult = kErrorFileNotFound;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp>SCE_OK) {
		SceIoStat MyStat;
		MemoryClear(&MyStat,sizeof(MyStat));
		int eError = sceIoGetstatByFd(fp,&MyStat);
		if (eError>=SCE_OK) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyStat.st_mtime);
			uResult = kErrorNone;
		}
	}
	if (uResult!=kErrorNone) {
		pOutput->Clear();
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was created

	If a file is open, query the operating system for the time
	the file was created.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file creation time
	\return kErrorNone if successful, kErrorNotSupportedOnThisPlatform if not available or other codes for errors
	\sa GetModificationTime() or SetCreationTime()

***************************************/

Burger::eError BURGER_API Burger::File::GetCreationTime(TimeDate_t *pOutput) BURGER_NOEXCEPT
{
	eError uResult = kErrorFileNotFound;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp>SCE_OK) {
		SceIoStat MyStat;
		MemoryClear(&MyStat,sizeof(MyStat));
		int eError = sceIoGetstatByFd(fp,&MyStat);
		if (eError>=SCE_OK) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyStat.st_ctime);
			uResult = kErrorNone;
		}
	}
	if (uResult!=kErrorNone) {
		pOutput->Clear();
	}
	return uResult;
}

/***************************************

	\brief Set the time the file was last modified

	If a file is open, call the operating system to set the file
	modification time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file modification time
	\return kErrorNone if successful, kErrorNotSupportedOnThisPlatform if not available or other codes for errors
	\sa SetCreationTime() or GetModificationTime()

***************************************/

Burger::eError BURGER_API Burger::File::SetModificationTime(const TimeDate_t *pInput) BURGER_NOEXCEPT
{
	eError uResult = kErrorFileNotFound;
	uintptr_t NewTime;
	if (!pInput->StoreTimeT(&NewTime)) {
		int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp>SCE_OK) {
			SceIoStat MyStat;
			MemoryClear(&MyStat,sizeof(MyStat));
			int eError = sceIoGetstatByFd(fp,&MyStat);
			if (eError>=SCE_OK) {
				pInput->Store(&MyStat.st_mtime);
				eError = sceIoChstatByFd(fp,&MyStat,SCE_CST_MT);
				if (eError>=SCE_OK) {
					uResult = kErrorNone;
				}
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Set the time the file was created

	If a file is open, call the operating system to set the file
	creation time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file creation time
	\return kErrorNone if successful, kErrorNotSupportedOnThisPlatform if not available or other codes for errors
	\sa SetModificationTime() or GetCreationTime()

***************************************/

Burger::eError BURGER_API Burger::File::SetCreationTime(const TimeDate_t *pInput) BURGER_NOEXCEPT
{
	eError uResult = kErrorFileNotFound;
	uintptr_t NewTime;
	if (!pInput->StoreTimeT(&NewTime)) {
		int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp>SCE_OK) {
			SceIoStat MyStat;
			MemoryClear(&MyStat,sizeof(MyStat));
			int eError = sceIoGetstatByFd(fp,&MyStat);
			if (eError>=SCE_OK) {
				pInput->Store(&MyStat.st_ctime);
				eError = sceIoChstatByFd(fp,&MyStat,SCE_CST_CT);
				if (eError>=SCE_OK) {
					uResult = kErrorNone;
				}
			}
		}
	}
	return uResult;
}

#endif
