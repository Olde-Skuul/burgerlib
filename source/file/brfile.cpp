/***************************************

	File Class

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"
#include "brfilemanager.h"
#include "brendian.h"
#include "brmemoryfunctions.h"
#include <stdio.h>

/*! ************************************

	\class Burger::File
	\brief System file reference class

	A functional equivalent to FILE *, except files
	are all considered binary and pathnames are
	only Burgerlib format

***************************************/

/*! ************************************

	\brief Create a Burger::File class

	Initialize variables, however no file is opened
	so all file access functions will fail until
	Open() is called and it succeeds

	\sa Open(const char *,eFileAccess), Open(Filename *,eFileAccess), Close(), ~File()

***************************************/

Burger::File::File() :
	m_pFile(NULL),
	m_uPosition(0),
	m_Filename(),
	m_Semaphore()
{
#if defined(BURGER_MAC)
	MemoryClear(m_FSRef,sizeof(m_FSRef));
#endif
}

/*! ************************************

	\brief Create a Burger::File class with a file

	Open a file and initialize the variables. If the file
	open operation fails, all file access functions will fail
	until a new file is opened.

	\param pFileName Pointer to a "C" string containing a Burgerlib pathname
	\param eAccess Enumeration on permissions requested on the opened file
	\sa Open(const char *,eFileAccess), File(Filename *,eFileAccess), Close()

***************************************/

Burger::File::File(const char *pFileName,eFileAccess eAccess) :
	m_pFile(NULL),
	m_uPosition(0),
	m_Filename(pFileName),
	m_Semaphore()
{
#if defined(BURGER_MAC)
	MemoryClear(m_FSRef,sizeof(m_FSRef));
#endif
	Open(pFileName,eAccess);
}

/*! ************************************

	\brief Create a Burger::File class with a Burger::Filename

	Open a file and initialize the variables. If the file
	open operation fails, all file access functions will fail
	until a new file is opened.

	\param pFileName Pointer to a Burger::Filename object
	\param eAccess Enumeration on permissions requested on the opened file
	\sa File(const char *,eFileAccess), Open(Filename *,eFileAccess), Close()

***************************************/

Burger::File::File(Filename *pFileName,eFileAccess eAccess) :
	m_pFile(NULL),
	m_uPosition(0),
	m_Filename(pFileName[0]),
	m_Semaphore()
{
#if defined(BURGER_MAC)
	MemoryClear(m_FSRef,sizeof(m_FSRef));
#endif
	Open(pFileName,eAccess);
}

/*! ************************************

	\brief Close any open file

	Shut down the \ref File and close any open file.

	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

Burger::File::~File()
{
	Close();
}

/*! ************************************

	\brief Create a new File instance

	Allocate memory using Burger::Alloc() and 
	initialize a File with it.

	\param pFileName Pointer to a "C" string containing a Burgerlib pathname
	\param eAccess Enumeration on permissions requested on the opened file
	\return \ref NULL if out of memory or the file didn't successfully open
	\sa Burger::Delete(const File *)

***************************************/

Burger::File * BURGER_API Burger::File::New(const char *pFileName,eFileAccess eAccess)
{
	// Manually allocate the memory
	File *pThis = new (Alloc(sizeof(File))) File();
	if (pThis) {
	// Load up the data
		if (pThis->Open(pFileName,eAccess)==kErrorNone) {
			// We're good!
			return pThis;
		}
		// Kill the malformed class
		Delete(pThis);
	}
	// Sorry Charlie!
	return nullptr;
}

/*! ************************************

	\brief Create a new File instance

	Allocate memory using Burger::Alloc() and 
	initialize a File with it.

	\param pFileName Pointer to a Burger::Filename object
	\param eAccess Enumeration on permissions requested on the opened file
	\return \ref NULL if out of memory or the file didn't successfully open
	\sa Burger::Delete(const File *)

***************************************/

Burger::File * BURGER_API Burger::File::New(Filename *pFileName,eFileAccess eAccess)
{
	// Manually allocate the memory
	File *pThis = new (Alloc(sizeof(File))) File();
	if (pThis) {
	// Load up the data
		if (pThis->Open(pFileName,eAccess)==kErrorNone) {
			// We're good!
			return pThis;
		}
		// Kill the malformed class
		Delete(pThis);
	}
	// Sorry Charlie!
	return nullptr;
}

/*! ************************************

	\fn Word Burger::File::IsOpened(void) const
	\brief Return \ref TRUE if a file is open

	Test if a file is currently open. If there's an active file, return
	\ref TRUE, otherwise return \ref FALSE

	\return \ref TRUE if there's an open file
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

/*! ************************************

	\brief Open a file using a Burgerlib pathname

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a "C" string containing a Burgerlib pathname
	\param eAccess Enumeration on permissions requested on the opened file
	\return File::OKAY if no error, error code if not.
	\sa Open(Filename *, eFileAccess) and File(const char *,eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::Open(const char *pFileName,eFileAccess eAccess) BURGER_NOEXCEPT
{
	Filename MyFilename(pFileName);
	return Open(&MyFilename,eAccess);
}

/*! ************************************

	\brief Open a file using a Burger::Filename

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a Burger::Filename object
	\param eAccess Enumeration on permissions requested on the opened file
	\return File::OKAY if no error, error code if not.
	\sa Open(const char *, eFileAccess) and File(const char *,eFileAccess)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || defined(BURGER_VITA)) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::File::Open(Filename *pFileName,eFileAccess eAccess) BURGER_NOEXCEPT
{
	static const char *g_OpenFlags[4] = {
		"rb","wb","ab","r+b"
	};
	Close();
	FILE *fp = fopen(pFileName->GetNative(),g_OpenFlags[eAccess&3]);
	Word uResult = FILENOTFOUND;
	if (fp) {
		m_pFile = fp;
		uResult = OKAY;
	}
	return static_cast<Burger::eError>(uResult);
}

/*! ************************************

	\brief Close any open file

	Close any previously opened file

	\return File::OKAY if no error, error code if not.
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

Word BURGER_API Burger::File::Close(void) BURGER_NOEXCEPT
{
	Word uResult = OKAY;
	FILE *fp = static_cast<FILE *>(m_pFile);
	if (fp) {
		if (fclose(static_cast<FILE *>(fp))) {
			uResult = IOERROR;
		}
		m_pFile = NULL;
	}
	return uResult;
}

/*! ************************************

	\brief Return the size of a file in bytes

	If a file is open, query the operating system for the size of the file
	in bytes.

	\note The return value is 32 bits wide on a 32 bit operating system, 64 bits
		wide on 64 bit operating systems
	\return 0 if error or an empty file. Non-zero is the size of the file in bytes. 
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

WordPtr BURGER_API Burger::File::GetSize(void)
{
	WordPtr uSize = 0;
	FILE *fp = static_cast<FILE *>(m_pFile);
	if (fp) {
		// Save the current file mark
		long Temp = ftell(fp);
		// Seek to the end of file
		if (!fseek(fp,0,SEEK_END)) {
			// Get the file size
			uSize = static_cast<WordPtr>(ftell(fp));
		}
		// If no error, restore the old file mark
		if (Temp!=-1) {
			fseek(fp,Temp,SEEK_SET);
		}
	}
	return uSize;
}

/*! ************************************

	\brief Read data from an open file

	If a file is open, perform a read operation. This function will fail
	if the file was not opened for read access.

	\param pOutput Pointer to a buffer of data to read from a file
	\param uSize Number of bytes to read
	\return Number of bytes read (Can be less than what was requested due to EOF or read errors)
	\sa Write(const void *,WordPtr)

***************************************/

WordPtr BURGER_API Burger::File::Read(void *pOutput,WordPtr uSize)
{
	WordPtr uResult = 0;
	if (uSize && pOutput) {
		FILE *fp = static_cast<FILE *>(m_pFile);
		if (fp) {
			uResult = fread(pOutput,1,uSize,fp);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Write data into an open file

	If a file is open, perform a write operation. This function will fail
	if the file was not opened for write access.

	\param pInput Pointer to a buffer of data to write to a file
	\param uSize Number of bytes to write
	\return Number of bytes written (Can be less than what was requested due to EOF or write errors)
	\sa Read(void *,WordPtr)

***************************************/

WordPtr BURGER_API Burger::File::Write(const void *pInput,WordPtr uSize) BURGER_NOEXCEPT
{
	WordPtr uResult = 0;
	if (uSize && pInput) {
		FILE *fp = static_cast<FILE *>(m_pFile);
		if (fp) {
			uResult = fwrite(pInput,1,uSize,fp);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Get the current file mark

	If a file is open, query the operating system for the location
	of the file mark for future reads or writes.

	\return Current file mark or zero if an error occurred
	\sa Write(const void *,WordPtr)

***************************************/

WordPtr BURGER_API Burger::File::GetMark(void)
{
	WordPtr uMark = 0;
	FILE *fp = static_cast<FILE *>(m_pFile);
	if (fp) {
		// Save the current file mark
		long Temp = ftell(fp);
		// If no error, restore the old file mark
		if (Temp!=-1) {
			uMark = static_cast<WordPtr>(Temp);
		}
	}
	return uMark;
}

/*! ************************************

	\brief Set the current file mark

	If a file is open, set the read/write mark at the location passed.

	\param uMark Value to set the new file mark to.
	\return File::OKAY if successful, File::INVALID_MARK if not.
	\sa GetMark() or SetMarkAtEOF()

***************************************/

Word BURGER_API Burger::File::SetMark(WordPtr uMark)
{
	Word uResult = INVALID_MARK;
	FILE *fp = static_cast<FILE *>(m_pFile);
	if (fp) {
		// Seek to the end of file
		if (!fseek(fp,static_cast<long>(uMark),SEEK_SET)) {
			uResult = OKAY;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Set the current file mark at the end of the file

	If a file is open, set the read/write mark to the end of the file.

	\return File::OKAY if successful, File::INVALID_MARK if not.
	\sa GetMark() or SetMark()

***************************************/

Word BURGER_API Burger::File::SetMarkAtEOF(void)
{
	Word uResult = INVALID_MARK;
	FILE *fp = static_cast<FILE *>(m_pFile);
	if (fp) {
		if (!fseek(fp,0,SEEK_END)) {
			uResult = OKAY;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Get the time the file was last modified

	If a file is open, query the operating system for the last time
	the file was modified.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file modification time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa GetCreationTime() or SetModificationTime()

***************************************/

Word BURGER_API Burger::File::GetModificationTime(TimeDate_t *pOutput)
{
	pOutput->Clear();
	return NOT_IMPLEMENTED;
}

/*! ************************************

	\brief Get the time the file was created

	If a file is open, query the operating system for the time
	the file was created.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file creation time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa GetModificationTime() or SetCreationTime()

***************************************/

Word BURGER_API Burger::File::GetCreationTime(TimeDate_t *pOutput)
{
	pOutput->Clear();
	return NOT_IMPLEMENTED;
}

/*! ************************************

	\brief Set the time the file was last modified

	If a file is open, call the operating system to set the file
	modification time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file modification time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa SetCreationTime() or GetModificationTime()

***************************************/

Word BURGER_API Burger::File::SetModificationTime(const TimeDate_t * /* pInput */)
{
	return NOT_IMPLEMENTED;
}

/*! ************************************

	\brief Set the time the file was created

	If a file is open, call the operating system to set the file
	creation time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file creation time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa SetModificationTime() or GetCreationTime()

***************************************/

Word BURGER_API Burger::File::SetCreationTime(const TimeDate_t * /* pInput */)
{
	return NOT_IMPLEMENTED;
}
#endif

Word BURGER_API Burger::File::OpenAsync(const char *pFileName,eFileAccess eAccess)
{
	m_Filename.Set(pFileName);
	FileManager::g_pFileManager->AddQueue(this,FileManager::IOCOMMAND_OPEN,NULL,eAccess);
	return 0;
}

Word BURGER_API Burger::File::OpenAsync(Filename *pFileName,eFileAccess eAccess)
{
	m_Filename = pFileName[0];
	FileManager::g_pFileManager->AddQueue(this,FileManager::IOCOMMAND_OPEN,NULL,eAccess);
	return 0;
}

Word BURGER_API Burger::File::CloseAsync(void)
{
	FileManager::g_pFileManager->AddQueue(this,FileManager::IOCOMMAND_CLOSE,NULL,0);
	return 0;
}

Word BURGER_API Burger::File::ReadAsync(void *pOutput,WordPtr uSize)
{
	FileManager::g_pFileManager->AddQueue(this,FileManager::IOCOMMAND_READ,pOutput,uSize);
	return 0;
}


/*! ************************************

	\fn Burger::File::SetAuxType(Word32 uAuxType)
	\brief Set the file's auxiliary type

	If a file is open, call the MacOS operating system to set the file's
	auxiliary type to the passed value.

	The file's auxiliary type is usually set to the application ID code.

	\note This is a MacOS exclusive feature. If the application is not running
	on MacOS, it will fail with a code of File::NOT_IMPLEMENTED.

	\param uAuxType Value to set the file's auxiliary type
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa SetAuxAndFileType(), SetFileType() or GetAuxType()

***************************************/

/*! ************************************

	\fn Burger::File::SetFileType(Word32 uFileType)
	\brief Set the file's type code

	If a file is open, call the MacOS operating system to set the file's
	type to the passed value.

	\note This is a MacOS exclusive feature. If the application is not running
	on MacOS, it will fail with a code of File::NOT_IMPLEMENTED.

	\param uFileType Value to set the file's type
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa SetAuxAndFileType(), SetAuxType() or GetFileType()

***************************************/

/*! ************************************

	\fn Burger::File::GetAuxType(void)
	\brief Get the file's auxiliary type

	If a file is open, call the MacOS operating system to get the file's
	auxiliary type.

	The file's auxiliary type is usually set to the application ID code.

	\note This is a MacOS exclusive feature. If the application is not running
	on MacOS, it will fail by returning zero.

	\return The four byte code or zero on failure
	\sa SetAuxType() or GetFileType()

***************************************/

/*! ************************************

	\fn Burger::File::GetFileType(void)
	\brief Get the file's type code

	If a file is open, call the MacOS operating system to get the file's
	type code.

	\note This is a MacOS exclusive feature. If the application is not running
	on MacOS, it will fail by returning zero.

	\return The four byte code or zero on failure
	\sa SetFileType() or GetAuxType()

***************************************/

/*! ************************************

	\fn Burger::File::SetAuxAndFileType(Word32 uAuxType,Word32 uFileType)
	\brief Set the file's auxiliary and file type

	If a file is open, call the MacOS operating system to set the file's
	auxiliary and file types to the passed values.

	The file's auxiliary type is usually set to the application ID code.

	\note This is a MacOS exclusive feature. If the application is not running
	on MacOS, it will fail with a code of File::NOT_IMPLEMENTED.

	\param uAuxType Value to set the file's auxiliary type
	\param uFileType Value to set the file's type
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa SetFileType() or SetAuxType()

***************************************/

/*! ************************************

	\brief Read a "C" string with the terminating zero to a file stream

	Read a "C" string with a terminating zero
	from the file stream. If the string read is larger than the buffer,
	it is truncated. The buffer will have an ending zero on valid 
	read or a trucated read. If uLength was zero, then pInput can be \ref NULL

	\param pOutput Pointer to a "C" string to write.
	\param uLength Size of the buffer (To prevent overruns)

	\return \ref TRUE if the string was read, \ref FALSE, hit EOF

	\sa Burger::WriteCString(FILE *,const char *)

***************************************/

Word BURGER_API Burger::File::ReadCString(char *pOutput,WordPtr uLength)
{
	// Set the maximum buffer size
	// and remove 1 to make space or the ending zero
	char *pEnd = (pOutput+uLength)-1;
	Word uTemp;
	for (;;) {		// Stay until either zero or EOF
		Word8 Buffer;
		if (Read(&Buffer,1)!=1) {
			uTemp = 666;	// EOF reached
			break;
		}
		uTemp = Buffer;
		if (!uTemp) {		// Exit due to end of string?
			break;
		}
		// Can I store it?
		if (pOutput<pEnd) {
			pOutput[0] = static_cast<char>(uTemp);
			++pOutput;		// Inc the input pointer
		}
	}
	if (uLength) {		// Any space in buffer?
		pOutput[0] = 0;	// Add ending zero
	}
	if (uTemp) {		// EOF?
		return FALSE;	// Hit the EOF.
	}
	return TRUE;	// Hit the end of string (More data may be present)
}

/*! ************************************

	\brief Read a big endian 32-bit value from a file.

	Given a file opened for reading, read a 32-bit value in big endian format
	from the file stream.

	\sa Burger::File::ReadBigWord16() and Burger::File::ReadLittleWord32()
	\return A 32 bit big endian int converted to native endian

***************************************/

Word32 BURGER_API Burger::File::ReadBigWord32(void)
{
	Word32 mValue;
	Read(&mValue,4);		// Save the long word
	return BigEndian::Load(&mValue);
}

/*! ************************************

	\brief Read a big endian 16-bit value from a file.

	Given a file opened for reading, read a 16-bit value in big endian format
	from the file stream.

	\sa Burger::File::ReadBigWord32() and Burger::File::ReadLittleWord16()
	\return A 16 bit big endian short converted to native endian

***************************************/

Word16 BURGER_API Burger::File::ReadBigWord16(void)
{
	Word16 mValue;
	Read(&mValue,2);		// Save the short word
	return BigEndian::Load(&mValue);
}

/*! ************************************

	\brief Read a little endian 32-bit value from a file.

	Given a file opened for reading, read a 32-bit value in little endian format
	from the file stream.

	\sa Burger::File::ReadLittleWord16() and Burger::File::ReadBigWord32()
	\return A 32 bit little endian int converted to native endian

***************************************/

Word32 BURGER_API Burger::File::ReadLittleWord32(void)
{
	Word32 mValue;
	Read(&mValue,4);		// Save the long word
	return LittleEndian::Load(&mValue);
}

/*! ************************************

	\brief Read a little endian 16-bit value from a file.

	Given a file opened for reading, read a 16-bit value in little endian format
	from the file stream.

	\sa Burger::File::ReadLittleWord32() and Burger::File::ReadBigWord16()
	\return A 16 bit little endian short converted to native endian

***************************************/

Word16 BURGER_API Burger::File::ReadLittleWord16(void)
{
	Word16 mValue;
	Read(&mValue,2);		// Save the long word
	return LittleEndian::Load(&mValue);
}
