/***************************************

	MacOS version of Burger::File

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"
#if defined(BURGER_MAC)
#include "brstring16.h"
#include <Devices.h>
#include <Files.h>

/***************************************

	\brief Open a file using a Burger::Filename

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a Burger::Filename object
	\param eAccess Enumeration on permissions requested on the opened file
	\return kErrorNone if no error, error code if not.
	\sa Open(const char *, eFileAccess) and File(const char *,eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::Open(
	Filename* pFileName, eFileAccess eAccess) BURGER_NOEXCEPT
{
	// Ignore any errors.
	Close();

	// Update the filename
	m_Filename = *pFileName;

	// Ensure the file access is in bounds
	eAccess = static_cast<eFileAccess>(eAccess & 3);

	// Map the access to macOS
	static const SInt8 g_Permissions[4] = {fsRdPerm | fsWrDenyPerm,
		fsWrPerm | fsRdDenyPerm,
		fsWrPerm | fsRdDenyPerm, fsRdWrPerm};

	// Assume file not found
	eError uResult = kErrorNone;
	int iMacError;
	short fp;

#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Try to use MacOS 9 or higher to create the file
	const char* pNative = m_Filename.GetNative();

	// Test if the MacOS 9 APIs are present
	FSRef* pFSRef = m_Filename.GetFSRef();
	if (pFSRef) {
		// Do it the MacOS 9 way
		FSRef MyRef;

		// Incomplete FSRef?
		if (pNative[0]) {
			// Error? File not found?
			// See if it should be created
			if (eAccess != kReadOnly) {

				String16 MyName(pNative);
				FSCatalogInfo MyInfo;
				InitFileInfo(reinterpret_cast<FileInfo*>(&MyInfo.finderInfo));
				MyInfo.textEncodingHint = kUnicode16BitFormat;
				uResult = MacConvertError(
					FSCreateFileUnicode(pFSRef, MyName.length(), MyName.c_str(),
						kFSCatInfoTextEncoding + kFSCatInfoFinderInfo, &MyInfo,
						&MyRef, nullptr));

				// Success, open the file using the new FSRef
				if (!uResult) {
					pFSRef = &MyRef;
				}
			}
		}

		// No errors so far?
		if (!uResult) {

			iMacError = FSOpenFork(pFSRef, 0, nullptr, static_cast<SInt8>(g_Permissions[eAccess]&3), &fp);
			if (!iMacError) {
				m_pFile = reinterpret_cast<void*>(fp);
				m_bUsingFSRef = TRUE;
				if (eAccess == kAppend) {
					uResult = SetMarkAtEOF();
				}
			}
			return MacConvertError(iMacError);
		}
	}
#endif

	// Do it the classic way

	// Get the FSSpec for the new file
	FSSpec MySpec;
	m_Filename.GetFSSpec(&MySpec);

	// Try to open it
	iMacError = OpenAware(&MySpec, g_Permissions[eAccess], &fp);

	// No file? If so, see if needs to be created.
	if ((eAccess != kReadOnly) && (iMacError == fnfErr)) {

		// Create the file (No data of any kind)
		iMacError = CreateEmptyFile(&MySpec);
		if (!iMacError) {

			// Try opening it again.
			iMacError = OpenAware(&MySpec, g_Permissions[eAccess], &fp);
		}
	}

	// Assume access is denied
	uResult = kErrorAccessDenied;
	if (!iMacError) {

		// The file was opened fine, capture it and mark as old style
		m_pFile = reinterpret_cast<void*>(fp);
		m_bUsingFSRef = FALSE;
		uResult = kErrorNone;

		// If appending, set the file mark at the end
		if (eAccess == kAppend) {

			// Error?
			uResult = SetMarkAtEOF();
		}
	}
	return uResult;
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
	OSErr iMacError;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {

#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened with the new way?
		if (m_bUsingFSRef) {
			iMacError = FSCloseFork(fp);
		} else
#endif
		{
			// It was opened the old way.
			iMacError = FSClose(fp);
		}
		// Record the error
		if (iMacError) {
			uResult = kErrorIO;
		}
		// Clear out the file reference
		m_pFile = nullptr;
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

uint64_t BURGER_API Burger::File::GetSize(void) BURGER_NOEXCEPT
{
	int iMacError;
	uint64_t uSize = 0;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened with the new way?
		if (m_bUsingFSRef) {
			SInt64 lFileSize;
			iMacError = FSGetForkSize(fp, &lFileSize);
			if (!iMacError) {
				uSize = static_cast<uint64_t>(lFileSize);
			}
		} else
#endif
		{
			// Do it the old fashioned way
			ParamBlockRec PBR;
			PBR.ioParam.ioRefNum = fp;
			iMacError = PBGetEOFSync(&PBR);
			if (!iMacError) {
				uSize = reinterpret_cast<uint64_t>(PBR.ioParam.ioMisc);
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
	\return Number of bytes read (Can be less than what was requested due to EOF
		or read errors)

	\sa Write(const void *,uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::Read(void* pOutput, uintptr_t uSize)
{
	uintptr_t uResult = 0;
	if (uSize && pOutput) {
		int iMacError;
		short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
			// Was it opened with the new way?
			if (m_bUsingFSRef) {
				// Read data from the open fork
				ByteCount lDataRead = 0;
				iMacError =
					FSReadFork(fp, fsAtMark, 0, uSize, pOutput, &lDataRead);
				uResult = static_cast<uintptr_t>(lDataRead);
			} else
#endif
			{
				ParamBlockRec PBR;
				PBR.ioParam.ioRefNum = fp;
				PBR.ioParam.ioBuffer = static_cast<Ptr>(pOutput);
				PBR.ioParam.ioReqCount = static_cast<long>(uSize);
				PBR.ioParam.ioPosMode = fsAtMark;
				iMacError = PBReadSync(&PBR);
				uResult = static_cast<uintptr_t>(PBR.ioParam.ioActCount);
			}
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

uintptr_t BURGER_API Burger::File::Write(
	const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uSize && pInput) {
		int iMacError;
		short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
			// Was it opened with the new way?
			if (m_bUsingFSRef) {
				// Write data to the open fork
				ByteCount lDataWritten = 0;
				iMacError = FSWriteFork(fp, fsAtMark, 0, uSize,
					const_cast<void*>(pInput), &lDataWritten);
				uResult = static_cast<uintptr_t>(lDataWritten);
			} else
#endif
			{
				ParamBlockRec PBR;
				PBR.ioParam.ioRefNum = fp;
				PBR.ioParam.ioBuffer =
					static_cast<Ptr>(const_cast<void*>(pInput));
				PBR.ioParam.ioReqCount = static_cast<long>(uSize);
				PBR.ioParam.ioPosMode = fsAtMark;
				iMacError = PBWriteSync(&PBR);
				uResult = static_cast<uintptr_t>(PBR.ioParam.ioActCount);
			}
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
	int iMacError;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened with the new way?
		if (m_bUsingFSRef) {
			SInt64 lCurrentMark;
			iMacError = FSGetForkPosition(fp, &lCurrentMark);
			if (!iMacError) {
				uMark = static_cast<uint64_t>(lCurrentMark);
			}
		} else
#endif
		{
			// Do it the old fashioned way
			ParamBlockRec PBR;
			PBR.ioParam.ioRefNum = fp;
			iMacError = PBGetFPosSync(&PBR);
			if (!iMacError) {
				uMark = static_cast<uint32_t>(PBR.ioParam.ioPosOffset);
			}
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
	int iMacError;
	eError uResult = kErrorNotInitialized;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened with the new way?
		if (m_bUsingFSRef) {
			iMacError =
				FSSetForkPosition(fp, fsFromStart, static_cast<SInt64>(uMark));
			if (!iMacError) {
				uResult = kErrorNone;
			} else {
				uResult = kErrorOutOfBounds;
			}
		} else
#endif
		{
			// Do it the old fashioned way
			uResult = kErrorOutOfBounds;
			if (uMark <= 0xFFFFFFFFU) {
				ParamBlockRec PBR;
				PBR.ioParam.ioRefNum = fp;
				PBR.ioParam.ioPosMode = fsFromStart;
				PBR.ioParam.ioPosOffset = static_cast<long>(uMark);
				iMacError = PBSetFPosSync(&PBR);
				if (!iMacError) {
					uResult = kErrorNone;
				}
			}
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
	int iMacError;
	eError uResult = kErrorOutOfBounds;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened with the new way?
		if (m_bUsingFSRef) {
			iMacError = FSSetForkPosition(fp, fsFromLEOF, 0);
			if (!iMacError) {
				uResult = kErrorNone;
			}
		} else
#endif
		{
			// Do it the old fashioned way
			uResult = kErrorOutOfBounds;
			ParamBlockRec PBR;
			PBR.ioParam.ioRefNum = fp;
			PBR.ioParam.ioPosMode = fsFromLEOF;
			PBR.ioParam.ioPosOffset = 0;
			iMacError = PBSetFPosSync(&PBR);
			if (!iMacError) {
				uResult = kErrorNone;
			}
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

	\return kErrorNone if successful, kErrorNotSupportedOnThisPlatform if not
		available or other codes for errors

	\sa GetCreationTime() or SetModificationTime()

***************************************/

Burger::eError BURGER_API Burger::File::GetModificationTime(
	TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	int iMacError;
	eError uResult = kErrorFileNotFound;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened with the new way?
		if (m_bUsingFSRef) {
			FSRef MyRef;
			iMacError = GetFileLocation(&MyRef, fp);
			if (!iMacError) {
				FSCatalogInfo MyInfo;
				iMacError = DoGetCatInfo(&MyInfo, &MyRef, kFSCatInfoContentMod);
				if (!iMacError) {
					// If it succeeded, the file must exist
					pOutput->Load(&MyInfo.contentModDate);
					uResult = kErrorNone;
				}
			}
		} else
#endif
		{
			FSSpec MySpec;
			iMacError = GetFileLocation(&MySpec, fp);
			if (!iMacError) {
				CInfoPBRec InfoPBRec;
				InfoPBRec.dirInfo.ioVRefNum = MySpec.vRefNum;
				InfoPBRec.dirInfo.ioDrDirID = MySpec.parID;
				InfoPBRec.dirInfo.ioNamePtr = MySpec.name;
				InfoPBRec.dirInfo.ioFDirIndex = 0;

				iMacError = PBGetCatInfoSync(&InfoPBRec);
				if (!iMacError) {
					pOutput->LoadFileSeconds(InfoPBRec.hFileInfo.ioFlMdDat);
					uResult = kErrorNone;
				}
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was created

	If a file is open, query the operating system for the time
	the file was created.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file creation
		time

	\return kErrorNone if successful, kErrorNotSupportedOnThisPlatform if not
		available or other codes for errors

	\sa GetModificationTime() or SetCreationTime()

***************************************/

Burger::eError BURGER_API Burger::File::GetCreationTime(
	TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	int iMacError;
	eError uResult = kErrorFileNotFound;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened with the new way?
		if (m_bUsingFSRef) {
			FSRef MyRef;
			iMacError = GetFileLocation(&MyRef, fp);
			if (!iMacError) {
				FSCatalogInfo MyInfo;
				iMacError = DoGetCatInfo(&MyInfo, &MyRef, kFSCatInfoCreateDate);
				if (!iMacError) {
					// If it succeeded, the file must exist
					pOutput->Load(&MyInfo.createDate);
					uResult = kErrorNone;
				}
			}
		} else
#endif
		{
			FSSpec MySpec;
			iMacError = GetFileLocation(&MySpec, fp);
			if (!iMacError) {
				CInfoPBRec InfoPBRec;
				InfoPBRec.dirInfo.ioVRefNum = MySpec.vRefNum;
				InfoPBRec.dirInfo.ioDrDirID = MySpec.parID;
				InfoPBRec.dirInfo.ioNamePtr = MySpec.name;
				InfoPBRec.dirInfo.ioFDirIndex = 0;

				iMacError = PBGetCatInfoSync(&InfoPBRec);
				if (!iMacError) {
					pOutput->LoadFileSeconds(InfoPBRec.hFileInfo.ioFlCrDat);
					uResult = kErrorNone;
				}
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Set the time the file was last modified

	If a file is open, call the operating system to set the file
	modification time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file
		modification time

	\return kErrorNone if successful, kErrorNotSupportedOnThisPlatform if not
		available or other codes for errors

	\sa SetCreationTime() or GetModificationTime()

***************************************/

Burger::eError BURGER_API Burger::File::SetModificationTime(
	const TimeDate_t* pInput) BURGER_NOEXCEPT
{
	int iMacError;
	eError uResult = kErrorFileNotFound;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened with the new way?
		if (m_bUsingFSRef) {
			FSRef MyRef;
			iMacError = GetFileLocation(&MyRef, fp);
			if (!iMacError) {
				FSRefParam Block;
				FSCatalogInfo MyInfo;
				iMacError =
					DoGetCatInfo(&MyInfo, &Block, &MyRef, kFSCatInfoContentMod);
				if (!iMacError) {
					// If it succeeded, the file must exist
					pInput->Store(&MyInfo.contentModDate);
					iMacError = PBSetCatalogInfoSync(&Block);
					uResult = kErrorNone;
				}
			}
		} else
#endif
		{
			FSSpec MySpec;
			iMacError = GetFileLocation(&MySpec, fp);
			if (!iMacError) {
				CInfoPBRec InfoPBRec;
				InfoPBRec.dirInfo.ioVRefNum = MySpec.vRefNum;
				InfoPBRec.dirInfo.ioDrDirID = MySpec.parID;
				InfoPBRec.dirInfo.ioNamePtr = MySpec.name;
				InfoPBRec.dirInfo.ioFDirIndex = 0;

				iMacError = PBGetCatInfoSync(&InfoPBRec);
				if (!iMacError) {
					InfoPBRec.hFileInfo.ioFlMdDat = pInput->GetFileSeconds();
					InfoPBRec.dirInfo.ioDrDirID = MySpec.parID;
					iMacError = PBSetCatInfoSync(&InfoPBRec);
					if (!iMacError) {
						uResult = kErrorNone;
					}
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

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file
		creation time

	\return kErrorNone if successful, kErrorNotSupportedOnThisPlatform if not
		available or other codes for errors

	\sa SetModificationTime() or GetCreationTime()

***************************************/

Burger::eError BURGER_API Burger::File::SetCreationTime(
	const TimeDate_t* pInput) BURGER_NOEXCEPT
{
	int iMacError;
	eError uResult = kErrorFileNotFound;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened with the new way?
		if (m_bUsingFSRef) {
			FSRef MyRef;
			iMacError = GetFileLocation(&MyRef, fp);
			if (!iMacError) {
				FSRefParam Block;
				FSCatalogInfo MyInfo;
				iMacError =
					DoGetCatInfo(&MyInfo, &Block, &MyRef, kFSCatInfoCreateDate);
				if (!iMacError) {
					// If it succeeded, the file must exist
					pInput->Store(&MyInfo.createDate);
					iMacError = PBSetCatalogInfoSync(&Block);
					uResult = kErrorNone;
				}
			}
		} else
#endif
		{
			FSSpec MySpec;
			iMacError = GetFileLocation(&MySpec, fp);
			if (!iMacError) {
				CInfoPBRec InfoPBRec;
				InfoPBRec.dirInfo.ioVRefNum = MySpec.vRefNum;
				InfoPBRec.dirInfo.ioDrDirID = MySpec.parID;
				InfoPBRec.dirInfo.ioNamePtr = MySpec.name;
				InfoPBRec.dirInfo.ioFDirIndex = 0;

				iMacError = PBGetCatInfoSync(&InfoPBRec);
				if (!iMacError) {
					InfoPBRec.hFileInfo.ioFlCrDat = pInput->GetFileSeconds();
					InfoPBRec.dirInfo.ioDrDirID = MySpec.parID;
					iMacError = PBSetCatInfoSync(&InfoPBRec);
					if (!iMacError) {
						uResult = kErrorNone;
					}
				}
			}
		}
	}
	return uResult;
}

#endif
