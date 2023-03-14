/***************************************

	MacOS version of Burger::File

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

	\brief Open a file using a \ref Filename

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a \ref Filename object
	\param uAccess Enumeration on permissions requested on the opened file

	\return \ref kErrorNone if no error, error code if not.

	\sa open(const char*, eFileAccess) and File(const char*, eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::open(
	Filename* pFileName, eFileAccess uAccess) BURGER_NOEXCEPT
{
	// Make sure the previous file is closed
	close();

	// Update the filename
	m_Filename = *pFileName;

	// Ensure the file access is in bounds
	uAccess = static_cast<eFileAccess>(uAccess & 3);

	// Map the access to macOS
	static const SInt8 g_Permissions[4] = {fsRdPerm | fsWrDenyPerm,
		fsWrPerm | fsRdDenyPerm, fsWrPerm | fsRdDenyPerm, fsRdWrPerm};

	// Assume file not found
	eError uResult = kErrorNone;
	int iMacError;
	short fp;

#if !(defined(BURGER_CFM) && defined(BURGER_68K))
	// Try to use MacOS 9 or higher to create the file
	const char* pNative = m_Filename.get_native();

	// Test if the MacOS 9 APIs are present
	FSRef* pFSRef = m_Filename.get_FSRef();
	if (pFSRef) {
		// Do it the MacOS 9 way
		FSRef MyRef;

		// Incomplete FSRef?
		if (pNative[0]) {
			// Error? File not found?
			// See if it should be created
			if (uAccess != kReadOnly) {

				String16 MyName(pNative);
				FSCatalogInfo MyInfo;
				InitFileInfo(reinterpret_cast<FileInfo*>(&MyInfo.finderInfo));
				MyInfo.textEncodingHint = kUnicode16BitFormat;
				uResult = platform_convert_to_error(
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

			iMacError = FSOpenFork(pFSRef, 0, nullptr,
				static_cast<SInt8>(g_Permissions[uAccess] & 3), &fp);
			if (!iMacError) {
				m_pFile = reinterpret_cast<void*>(fp);
				m_bUsingFSRef = TRUE;
				if (uAccess == kAppend) {
					uResult = set_mark_at_EOF();
				}
			}
			return platform_convert_to_error(iMacError);
		}
	}
#endif

	// Do it the classic way

	// Get the FSSpec for the new file
	FSSpec MySpec;
	m_Filename.get_FSSpec(&MySpec);

	// Try to open it
	iMacError = OpenAware(&MySpec, g_Permissions[uAccess], &fp);

	// No file? If so, see if needs to be created.
	if ((uAccess != kReadOnly) && (iMacError == fnfErr)) {

		// Create the file (No data of any kind)
		iMacError = CreateEmptyFile(&MySpec);
		if (!iMacError) {

			// Try opening it again.
			iMacError = OpenAware(&MySpec, g_Permissions[uAccess], &fp);
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
		if (uAccess == kAppend) {

			// Error?
			uResult = set_mark_at_EOF();
		}
	}
	return uResult;
}

/***************************************

	\brief Close open file

	Close previously opened file. If there was no open file, this function
	performs no operation and returns \ref kErrorNone

	\return \ref kErrorNone if no error, error code if not.

	\sa open(const char*, eFileAccess) and open(Filename*, eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::close(void) BURGER_NOEXCEPT
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

	\note The return value is 64 bits wide, which may be larger than the
		amount of available memory on some systems.

	\return 0 if error or an empty file. Non-zero is the size of the file in
		bytes.

	\sa read(void*, uintptr_t) and write(const void*, uintptr_t)

***************************************/

uint64_t BURGER_API Burger::File::get_file_size(void) BURGER_NOEXCEPT
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

	\sa write(const void*, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::read(void* pOutput, uintptr_t uSize)
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

	\sa read(void *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::write(
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

	\sa write(const void*, uintptr_t) or set_mark(uint64_t)

***************************************/

uint64_t BURGER_API Burger::File::get_mark(void) BURGER_NOEXCEPT
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

	\return \ref kErrorNone if successful, \ref kErrorOutOfBounds if not.

	\sa get_mark() or set_mark_at_EOF()

***************************************/

Burger::eError BURGER_API Burger::File::set_mark(uint64_t uMark) BURGER_NOEXCEPT
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

	\return \ref kErrorNone if successful, \ref kErrorOutOfBounds if not.

	\sa get_mark() or set_mark(uint64_t)

***************************************/

Burger::eError BURGER_API Burger::File::set_mark_at_EOF(void) BURGER_NOEXCEPT
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

	\param pOutput Pointer to a \ref TimeDate_t to receive the file
		modification time

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa get_creation_time() or set_modification_time()

***************************************/

Burger::eError BURGER_API Burger::File::get_modification_time(
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

	If a file is open, query the operating system for the time the file was
	created.

	\param pOutput Pointer to a \ref TimeDate_t to receive the file creation
		time

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa get_modification_time() or set_creation_time()

***************************************/

Burger::eError BURGER_API Burger::File::get_creation_time(
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

	\param pInput Pointer to a \ref TimeDate_t to use for the new file
		modification time

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa get_modification_time() or set_creation_time()

***************************************/

Burger::eError BURGER_API Burger::File::set_modification_time(
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

	\param pInput Pointer to a \ref TimeDate_t to use for the new file
		creation time

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa get_creation_time() or set_modification_time()

***************************************/

Burger::eError BURGER_API Burger::File::set_creation_time(
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

/***************************************

	\brief Set the file's auxiliary type

	If a file is open, call the MacOS operating system to set the file's
	auxiliary type to the passed value.

	The file's auxiliary type is usually set to the application ID code.

	\note This is a MacOS and Darwin exclusive feature. If the application is
		not running on Apple operating system, it will fail with a code of
		\ref kErrorNotSupportedOnThisPlatform.

	\param uCreatorType Value to set the file's creator type

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa set_creator_and_file_type(), set_file_type() or get_creator_type()

***************************************/

Burger::eError BURGER_API Burger::File::set_creator_type(
	uint32_t uCreatorType) BURGER_NOEXCEPT
{
	int iMacError;
	eError uResult = kErrorFileNotFound;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened the new way?
		if (m_bUsingFSRef) {
			FSRef MyRef;
			iMacError = GetFileLocation(&MyRef, fp);
			if (!iMacError) {
				// Use the Carbon version?
				FSRefParam Block;
				FSCatalogInfo MyInfo;
				iMacError =
					DoGetCatInfo(&MyInfo, &Block, &MyRef, kFSCatInfoFinderInfo);
				if (!iMacError) {
					// If it succeeded, modify data
					reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)
						->fileCreator = uCreatorType;
					iMacError = PBSetCatalogInfoSync(&Block);
				}
				uResult = platform_convert_to_error(iMacError);
			}

		} else
#endif
		{
			CInfoPBRec InfoPBRec;
			FSSpec MySpec;
			iMacError = GetFileLocation(&MySpec, fp);
			if (!iMacError) {
				// Use a local name buffer for reading and writing to the file
				uint8_t TempBuffer[256];
				iMacError = DoGetCatInfo(&InfoPBRec, TempBuffer, &MySpec);

				// No error? File was found
				if (!iMacError) {

					// Only files have creator types
					if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {

						// Set the new type
						InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator =
							uCreatorType;

						// Restore the Directory ID (Since it was modified)
						InfoPBRec.hFileInfo.ioDirID = MySpec.parID;

						// Save out the new state
						iMacError = PBSetCatInfoSync(&InfoPBRec);

						if (!iMacError) {
							uResult = kErrorNone;
						}
					}
				}
				uResult = platform_convert_to_error(iMacError);
			}
		}
	}

	// Return error code
	return uResult;
}

/***************************************

	\brief Set the file's type code

	If a file is open, call the MacOS operating system to set the file's
	type to the passed value.

	\note This is a MacOS and Darwin exclusive feature. If the application is
		not running on Apple operating system, it will fail with a code of
		\ref kErrorNotSupportedOnThisPlatform.

	\param uFileType Value to set the file's type

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa set_creator_and_file_type(), set_creator_type() or get_file_type()

***************************************/

Burger::eError BURGER_API Burger::File::set_file_type(
	uint32_t uFileType) BURGER_NOEXCEPT
{
	int iMacError;
	eError uResult = kErrorFileNotFound;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened the new way?
		if (m_bUsingFSRef) {
			FSRef MyRef;
			iMacError = GetFileLocation(&MyRef, fp);
			if (!iMacError) {
				// Use the Carbon version?
				FSRefParam Block;
				FSCatalogInfo MyInfo;
				iMacError =
					DoGetCatInfo(&MyInfo, &Block, &MyRef, kFSCatInfoFinderInfo);
				if (!iMacError) {
					// If it succeeded, modify data
					reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType =
						uFileType;
					iMacError = PBSetCatalogInfoSync(&Block);
				}
				uResult = platform_convert_to_error(iMacError);
			}

		} else
#endif
		{
			CInfoPBRec InfoPBRec;
			FSSpec MySpec;
			iMacError = GetFileLocation(&MySpec, fp);
			if (!iMacError) {
				// Use a local name buffer for reading and writing to the file
				uint8_t TempBuffer[256];
				iMacError = DoGetCatInfo(&InfoPBRec, TempBuffer, &MySpec);

				// No error? File was found
				if (!iMacError) {

					// Only files have creator types
					if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {

						// Set the new type
						InfoPBRec.hFileInfo.ioFlFndrInfo.fdType = uFileType;

						// Restore the Directory ID (Since it was modified)
						InfoPBRec.hFileInfo.ioDirID = MySpec.parID;

						// Save out the new state
						iMacError = PBSetCatInfoSync(&InfoPBRec);

						if (!iMacError) {
							uResult = kErrorNone;
						}
					}
				}
				uResult = platform_convert_to_error(iMacError);
			}
		}
	}

	// Return error code
	return uResult;
}

/***************************************

	\brief Get the file's auxiliary type

	If a file is open, call the MacOS operating system to get the file's
	auxiliary type.

	The file's auxiliary type is usually set to the application ID code.

	\note This is a MacOS exclusive feature. If the application is not running
	on MacOS, it will fail by returning zero.

	\return The four byte code or zero on failure

	\sa set_creator_type() or get_file_type()

***************************************/

uint32_t BURGER_API Burger::File::get_creator_type(void) BURGER_NOEXCEPT
{
	int iMacError;
	uint32_t uResult = 0;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened the new way?
		if (m_bUsingFSRef) {
			FSRef MyRef;
			iMacError = GetFileLocation(&MyRef, fp);
			if (!iMacError) {
				// Use the Carbon version?
				FSRefParam Block;
				FSCatalogInfo MyInfo;
				iMacError =
					DoGetCatInfo(&MyInfo, &Block, &MyRef, kFSCatInfoFinderInfo);
				if (!iMacError) {
					// If it succeeded, modify data
					uResult = reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)
								  ->fileCreator;
				}
			}

		} else
#endif
		{
			CInfoPBRec InfoPBRec;
			FSSpec MySpec;
			iMacError = GetFileLocation(&MySpec, fp);
			if (!iMacError) {
				// Use a local name buffer for reading and writing to the file
				uint8_t TempBuffer[256];
				iMacError = DoGetCatInfo(&InfoPBRec, TempBuffer, &MySpec);

				// No error? File was found
				if (!iMacError) {

					// Only files have creator types
					if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {

						// Get the new type
						uResult = InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator;
					}
				}
			}
		}
	}

	// Return error code
	return uResult;
}

/***************************************

	\brief Get the file's type code

	If a file is open, call the MacOS operating system to get the file's
	type code.

	\note This is a MacOS exclusive feature. If the application is not running
	on MacOS, it will fail by returning zero.

	\return The four byte code or zero on failure

	\sa set_file_type() or get_creator_type()

***************************************/

uint32_t BURGER_API Burger::File::get_file_type(void) BURGER_NOEXCEPT
{
	int iMacError;
	uint32_t uResult = 0;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened the new way?
		if (m_bUsingFSRef) {
			FSRef MyRef;
			iMacError = GetFileLocation(&MyRef, fp);
			if (!iMacError) {
				// Use the Carbon version?
				FSRefParam Block;
				FSCatalogInfo MyInfo;
				iMacError =
					DoGetCatInfo(&MyInfo, &Block, &MyRef, kFSCatInfoFinderInfo);
				if (!iMacError) {
					// If it succeeded, modify data
					uResult = reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)
								  ->fileType;
				}
			}

		} else
#endif
		{
			CInfoPBRec InfoPBRec;
			FSSpec MySpec;
			iMacError = GetFileLocation(&MySpec, fp);
			if (!iMacError) {
				// Use a local name buffer for reading and writing to the file
				uint8_t TempBuffer[256];
				iMacError = DoGetCatInfo(&InfoPBRec, TempBuffer, &MySpec);

				// No error? File was found
				if (!iMacError) {

					// Only files have creator types
					if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {

						// Get the new type
						uResult = InfoPBRec.hFileInfo.ioFlFndrInfo.fdType;
					}
				}
			}
		}
	}

	// Return error code
	return uResult;
}

/***************************************

	\brief Set the file's auxiliary and file type

	If a file is open, call the MacOS operating system to set the file's
	auxiliary and file types to the passed values.

	The file's auxiliary type is usually set to the application ID code.

	\note This is a MacOS exclusive feature. If the application is not running
	on MacOS, it will fail with a code of \ref kErrorNotSupportedOnThisPlatform.

	\param uCreatorType Value to set the file's auxiliary type
	\param uFileType Value to set the file's type

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa set_file_type() or set_creator_type()

***************************************/

Burger::eError BURGER_API Burger::File::set_creator_and_file_type(
	uint32_t uCreatorType, uint32_t uFileType) BURGER_NOEXCEPT
{
	int iMacError;
	eError uResult = kErrorFileNotFound;
	short fp = static_cast<short>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
#if !(defined(BURGER_CFM) && defined(BURGER_68K))
		// Was it opened the new way?
		if (m_bUsingFSRef) {
			FSRef MyRef;
			iMacError = GetFileLocation(&MyRef, fp);
			if (!iMacError) {
				// Use the Carbon version?
				FSRefParam Block;
				FSCatalogInfo MyInfo;
				iMacError =
					DoGetCatInfo(&MyInfo, &Block, &MyRef, kFSCatInfoFinderInfo);
				if (!iMacError) {
					// If it succeeded, modify data
					reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)->fileType =
						uFileType;
					reinterpret_cast<FileInfo*>(&MyInfo.finderInfo)
						->fileCreator = uCreatorType;
					iMacError = PBSetCatalogInfoSync(&Block);
				}
				uResult = platform_convert_to_error(iMacError);
			}

		} else
#endif
		{
			CInfoPBRec InfoPBRec;
			FSSpec MySpec;
			iMacError = GetFileLocation(&MySpec, fp);
			if (!iMacError) {
				// Use a local name buffer for reading and writing to the file
				uint8_t TempBuffer[256];
				iMacError = DoGetCatInfo(&InfoPBRec, TempBuffer, &MySpec);

				// No error? File was found
				if (!iMacError) {

					// Only files have creator types
					if (!(InfoPBRec.dirInfo.ioFlAttrib & kioFlAttribDirMask)) {

						// Set the new type
						InfoPBRec.hFileInfo.ioFlFndrInfo.fdType = uFileType;
						InfoPBRec.hFileInfo.ioFlFndrInfo.fdCreator =
							uCreatorType;

						// Restore the Directory ID (Since it was modified)
						InfoPBRec.hFileInfo.ioDirID = MySpec.parID;

						// Save out the new state
						iMacError = PBSetCatInfoSync(&InfoPBRec);

						if (!iMacError) {
							uResult = kErrorNone;
						}
					}
				}
				uResult = platform_convert_to_error(iMacError);
			}
		}
	}

	// Return error code
	return uResult;
}

#endif
