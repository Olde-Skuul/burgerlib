/***************************************

	MacOS version of Burger::File

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"
#if defined(BURGER_MAC)
#include "brstring16.h"
#include <Files.h>

/***************************************

	\brief Open a file using a Burger::Filename

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a Burger::Filename object
	\param eAccess Enumeration on permissions requested on the opened file
	\return File::OKAY if no error, error code if not.
	\sa Open(const char *, eFileAccess) and File(const char *,eFileAccess)

***************************************/

Word BURGER_API Burger::File::Open(Filename* pFileName, eFileAccess eAccess)
{
	Close();
	eAccess = static_cast<eFileAccess>(eAccess & 3);

	static const SInt8 g_Permissions[4] = {
		fsRdPerm, fsWrPerm, fsWrPerm, fsRdWrPerm};
	
	HFSUniStr255 ForkName;
	FSGetDataForkName(&ForkName);
	
	// Convert the filename to unicode
	String16 MyName(pFileName->GetNative());
	// Create a UFT16 FSRef
	OSErr eError = FSMakeFSRefUnicode(pFileName->GetFSRef(), MyName.GetLength(),
		MyName.GetPtr(), kUnicode16BitFormat,
		reinterpret_cast<FSRef*>(m_FSRef));
	Word uResult = FILENOTFOUND;

	// Error? File not found?
	// See if it should be created
	if ((eAccess != READONLY) && ((eError == fnfErr) || (eError == nsvErr))) {
		FSCatalogInfo MyInfo;
		((FileInfo*)(&MyInfo.finderInfo))->fileType = 'BINA';
		((FileInfo*)(&MyInfo.finderInfo))->fileCreator = '????';
		((FileInfo*)(&MyInfo.finderInfo))->finderFlags = 0;
		((FileInfo*)(&MyInfo.finderInfo))->location.h = 0;
		((FileInfo*)(&MyInfo.finderInfo))->location.v = 0;
		((FileInfo*)(&MyInfo.finderInfo))->reservedField = 0;
		MyInfo.textEncodingHint = kUnicode16BitFormat;
		eError = FSCreateFileUnicode(pFileName->GetFSRef(), MyName.GetLength(),
			MyName.GetPtr(), kFSCatInfoTextEncoding + kFSCatInfoFinderInfo,
			&MyInfo, reinterpret_cast<FSRef*>(m_FSRef), NULL);
	}
	if (!eError) {
		short fp;
		eError = FSOpenFork(reinterpret_cast<FSRef*>(m_FSRef), ForkName.length,
			ForkName.unicode, g_Permissions[eAccess], &fp);
		if (!eError) {
			m_pFile = reinterpret_cast<void*>(fp);
			uResult = OKAY;
			if (eAccess == APPEND) {
				uResult = SetMarkAtEOF();
			}
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

Word BURGER_API Burger::File::Close(void)
{
	Word uResult = OKAY;
	short fp = static_cast<short>(reinterpret_cast<WordPtr>(m_pFile));
	if (fp) {
		OSErr eClose = FSCloseFork(fp);
		if (eClose) {
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
	\return 0 if error or an empty file. Non-zero is the size of the file in
bytes. \sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

WordPtr BURGER_API Burger::File::GetSize(void)
{
	WordPtr uSize = 0;
	short fp = static_cast<short>(reinterpret_cast<WordPtr>(m_pFile));
	if (fp) {
		SInt64 lFileSize;
		OSErr eSize = FSGetForkSize(fp, &lFileSize);
		if (!eSize) {
			if (static_cast<Word64>(lFileSize)
				<= static_cast<Word64>(0xFFFFFFFFU)) {
				uSize = static_cast<WordPtr>(lFileSize);
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
	\return Number of bytes read (Can be less than what was requested due to EOF
or read errors) \sa Write(const void *,WordPtr)

***************************************/

WordPtr BURGER_API Burger::File::Read(void* pOutput, WordPtr uSize)
{
	WordPtr uResult = 0;
	if (uSize && pOutput) {
		short fp = static_cast<short>(reinterpret_cast<WordPtr>(m_pFile));
		if (fp) {
			ByteCount lDataRead = 0;
			OSErr eRead = FSReadFork(
				fp, fsAtMark, 0, uSize, pOutput, &lDataRead); // Read data
			uResult = static_cast<WordPtr>(lDataRead);
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
EOF or write errors) \sa Read(void *,WordPtr)

***************************************/

WordPtr BURGER_API Burger::File::Write(const void* pInput, WordPtr uSize)
{
	WordPtr uResult = 0;
	if (uSize && pInput) {
		short fp = static_cast<short>(reinterpret_cast<WordPtr>(m_pFile));
		if (fp) {
			ByteCount lDataWritten = 0;
			OSErr eWrite = FSWriteFork(
				fp, fsAtMark, 0, uSize, (void *)pInput, &lDataWritten); // Write data
			uResult = static_cast<WordPtr>(lDataWritten);
		}
	}
	return uResult;
}

/***************************************

	\brief Get the current file mark

	If a file is open, query the operating system for the location
	of the file mark for future reads or writes.

	\return Current file mark or zero if an error occurred
	\sa Write(const void *,WordPtr)

***************************************/

WordPtr BURGER_API Burger::File::GetMark(void)
{
	WordPtr uMark = 0;
	short fp = static_cast<short>(reinterpret_cast<WordPtr>(m_pFile));
	if (fp) {
		SInt64 lCurrentMark;
		OSErr eErr = FSGetForkPosition(fp, &lCurrentMark);
		if (!eErr) {
			if (static_cast<Word64>(lCurrentMark)
				<= static_cast<Word64>(0xFFFFFFFFU)) {
				uMark = static_cast<WordPtr>(lCurrentMark);
			} else {
				uMark = 0xFFFFFFFFU;
			}
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

Word BURGER_API Burger::File::SetMark(WordPtr uMark)
{
	Word uResult = INVALID_MARK;
	short fp = static_cast<short>(reinterpret_cast<WordPtr>(m_pFile));
	if (fp) {
		OSErr eErr = FSSetForkPosition(fp, fsFromStart, uMark);
		if (!eErr) {
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

Word BURGER_API Burger::File::SetMarkAtEOF(void)
{
	Word uResult = INVALID_MARK;
	short fp = static_cast<short>(reinterpret_cast<WordPtr>(m_pFile));
	if (fp) {
		OSErr eErr = FSSetForkPosition(fp, fsFromLEOF, 0);
		if (!eErr) {
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
modification time \return File::OKAY if successful, File::NOT_IMPLEMENTED if not
available or other codes for errors \sa GetCreationTime() or
SetModificationTime()

***************************************/

Word BURGER_API Burger::File::GetModificationTime(TimeDate_t* pOutput)
{
	Word uResult = FILENOTFOUND;
	short fp = static_cast<short>(reinterpret_cast<WordPtr>(m_pFile));
	if (fp) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = reinterpret_cast<FSRef*>(m_FSRef);
		Block.whichInfo = kFSCatInfoContentMod;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		OSErr eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyInfo.contentModDate);
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was created

	If a file is open, query the operating system for the time
	the file was created.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file creation
time \return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or
other codes for errors \sa GetModificationTime() or SetCreationTime()

***************************************/

Word BURGER_API Burger::File::GetCreationTime(TimeDate_t* pOutput)
{
	Word uResult = FILENOTFOUND;
	short fp = static_cast<short>(reinterpret_cast<WordPtr>(m_pFile));
	if (fp) {
		FSRefParam Block;
		FSCatalogInfo MyInfo;
		Block.ref = reinterpret_cast<FSRef*>(m_FSRef);
		Block.whichInfo = kFSCatInfoContentMod;
		Block.catInfo = &MyInfo;
		Block.spec = NULL;
		Block.parentRef = NULL;
		Block.outName = NULL;
		OSErr eError = PBGetCatalogInfoSync(&Block);
		if (!eError) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyInfo.createDate);
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Set the time the file was last modified

	If a file is open, call the operating system to set the file
	modification time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file
modification time \return File::OKAY if successful, File::NOT_IMPLEMENTED if not
available or other codes for errors \sa SetCreationTime() or
GetModificationTime()

***************************************/

Word BURGER_API Burger::File::SetModificationTime(const TimeDate_t* pInput)
{
	Word uResult = FILENOTFOUND;
#if 0
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
#else
	BURGER_UNUSED(pInput);
#endif
	return uResult;
}

/***************************************

	\brief Set the time the file was created

	If a file is open, call the operating system to set the file
	creation time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file
creation time \return File::OKAY if successful, File::NOT_IMPLEMENTED if not
available or other codes for errors \sa SetModificationTime() or
GetCreationTime()

***************************************/

Word BURGER_API Burger::File::SetCreationTime(const TimeDate_t* pInput)
{
	Word uResult = FILENOTFOUND;
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
	BURGER_UNUSED(pInput);
#endif
	return uResult;
}

#endif
