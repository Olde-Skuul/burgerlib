/***************************************

	File Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"
#include "brendian.h"
#include "brfilemanager.h"
#include "brmemoryfunctions.h"

#include <stdio.h>

/*! ************************************

	\class Burger::File
	\brief System file reference class

	A functional equivalent to FILE *, except files are all considered binary
	and pathnames are only Burgerlib format.

	\sa FileManager or Filename

***************************************/

/*! ************************************

	\enum Burger::File::eFileAccess
	\brief Enumeration to open a file.

	Used by open(const char*, eFileAccess) to determine the method of opening a
	file.

	\sa open(const char*, eFileAccess) or open(Filename*, eFileAccess)

***************************************/

/*! ************************************

	\brief Create a \ref File class

	Initialize variables, however no file is opened so all file access functions
	will fail until open() is called and it succeeds.

	\sa open(const char*, eFileAccess), open(Filename*, eFileAccess), close(),
		~File()

***************************************/

Burger::File::File() BURGER_NOEXCEPT: m_pFile(nullptr),
									  m_Filename()
#if defined(BURGER_MAC)
	,
									  m_bUsingFSRef(FALSE)
#endif
{
}

/*! ************************************

	\brief Create a \ref File class with a file

	Open a file and initialize the variables. If the file open operation fails,
	all file access functions will fail until a new file is opened.

	Use is_opened() const to test if the file successfully opened.

	\param pFileName Pointer to a "C" string containing a Burgerlib pathname
	\param uAccess Enumeration on permissions requested on the opened file

	\sa open(const char*, eFileAccess), File(Filename*, eFileAccess), close()

***************************************/

Burger::File::File(const char* pFileName, eFileAccess uAccess) BURGER_NOEXCEPT
	: m_pFile(nullptr),
	  m_Filename(pFileName)
#if defined(BURGER_MAC)
	,
	  m_bUsingFSRef(FALSE)
#endif
{
	open(pFileName, uAccess);
}

/*! ************************************

	\brief Create a \ref File class with a \ref Filename

	Open a file and initialize the variables. If the file open operation fails,
	all file access functions will fail until a new file is opened.

	Use is_opened() const to test if the file successfully opened.

	\param pFileName Pointer to a \ref Filename object
	\param uAccess Enumeration on permissions requested on the opened file

	\sa File(const char*, eFileAccess), open(Filename*, eFileAccess), close()

***************************************/

Burger::File::File(Filename* pFileName, eFileAccess uAccess) BURGER_NOEXCEPT
	: m_pFile(nullptr),
	  m_Filename(pFileName[0])
#if defined(BURGER_MAC)
	,
	  m_bUsingFSRef(FALSE)
#endif
{
	open(pFileName, uAccess);
}

/*! ************************************

	\brief Close any open file

	Shut down the \ref File object and close the open file if it was still open.

	\sa close()

***************************************/

Burger::File::~File()
{
	close();
}

/*! ************************************

	\brief Create a new File instance

	Allocate memory using Burger::allocate_memory() and initialize a File with it.

	\param pFileName Pointer to a "C" string containing a Burgerlib pathname
	\param uAccess Enumeration on permissions requested on the opened file

	\return \ref nullptr if out of memory or the file didn't successfully open

	\sa \ref delete_object(const T*)

***************************************/

Burger::File* BURGER_API Burger::File::new_object(
	const char* pFileName, eFileAccess uAccess) BURGER_NOEXCEPT
{
	// Manually allocate the memory
	File* pThis = new (allocate_memory(sizeof(File))) File();
	if (pThis) {

		// Load up the data
		if (pThis->open(pFileName, uAccess) != kErrorNone) {

			// Kill the malformed class
			delete_object(pThis);
			pThis = nullptr;
		}
	}
	return pThis;
}

/*! ************************************

	\brief Create a new File instance

	Allocate memory using Burger::allocate_memory() and initialize a File with it.

	\param pFileName Pointer to a Burger::Filename object
	\param uAccess Enumeration on permissions requested on the opened file

	\return \ref nullptr if out of memory or the file didn't successfully open

	\sa \ref delete_object(const T*)

***************************************/

Burger::File* BURGER_API Burger::File::new_object(
	Filename* pFileName, eFileAccess uAccess) BURGER_NOEXCEPT
{
	// Manually allocate the memory
	File* pThis = new (allocate_memory(sizeof(File))) File();
	if (pThis) {

		// Load up the data
		if (pThis->open(pFileName, uAccess) != kErrorNone) {

			// Kill the malformed class
			delete_object(pThis);
			pThis = nullptr;
		}
	}
	return pThis;
}

/*! ************************************

	\fn Burger::File::get_filename(void)
	\brief Return \ref Filename object.

	If a file was opened, the filename is cached in the File object. This call
	will return a pointer to that cached filename.

	\note Do not assign a new filename to the object. It's only for information
		purposes and not for modification.

	\return Pointer to the contained \ref Filename object.
	\sa is_opened(void) const or get_file_pointer(void) const

***************************************/

/*! ************************************

	\fn Burger::File::get_file_pointer(void) const
	\brief Return the native file reference.

	If a file was opened, the underlying operating system will create a file
	reference and it is stored within the File object.

	\note This value is platform specific. This is only to be used by platform
	specific code for use with operating system calls beyond the scope of the
	Burgerlib API.

	\return Native file system file reference or \ref nullptr if invalid.
	\sa get_filename() or set_file_pointer(void *)

***************************************/

/*! ************************************

	\fn Burger::File::set_file_pointer(void *)
	\brief Set the native file reference.

	If using platform specific code to open a file using special privileges
	beyond the Burgerlib API, this call will allow code to manually set the file
	reference.

	\note This call is platform specific and great care is required for its use.

	\param pFile Native file reference.

	\sa get_file_pointer() const or close()

***************************************/

/*! ************************************

	\fn Burger::File::is_opened(void) const
	\brief Return \ref TRUE if a file is open.

	Test if a file is currently open. If there's an active file, return
	\ref TRUE, otherwise return \ref FALSE

	\return \ref TRUE if there's an open file
	\sa open(const char*, eFileAccess) and open(Filename*, eFileAccess)

***************************************/

/*! ************************************

	\brief Open a file using a Burgerlib pathname

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a "C" string containing a Burgerlib pathname
	\param uAccess Enumeration on permissions requested on the opened file

	\return \ref kErrorNone if no error, error code if not.

	\sa open(Filename*, eFileAccess) and File(const char*, eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::open(
	const char* pFileName, eFileAccess uAccess) BURGER_NOEXCEPT
{
	Filename MyFilename(pFileName);
	return open(&MyFilename, uAccess);
}

/*! ************************************

	\brief Open a file using a \ref Filename

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a \ref Filename object
	\param uAccess Enumeration on permissions requested on the opened file

	\return \ref kErrorNone if no error, error code if not.

	\sa open(const char*, eFileAccess) and File(const char*, eFileAccess)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_UNIX) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)

#if !defined(DOXYGEN)
static const char* g_OpenFlags[4] = {"rb", "wb", "ab", "r+b"};
#endif

Burger::eError BURGER_API Burger::File::open(
	Filename* pFileName, eFileAccess uAccess) BURGER_NOEXCEPT
{
	// Make sure the previous file is closed
	close();

	FILE* fp = fopen(pFileName->get_native(), g_OpenFlags[uAccess & 3U]);
	uint_t uResult = kErrorFileNotFound;
	if (fp) {
		m_pFile = fp;
		uResult = kErrorNone;
	}
	return static_cast<eError>(uResult);
}

/*! ************************************

	\brief Close open file

	Close previously opened file. If there was no open file, this function
	performs no operation and returns \ref kErrorNone

	\return \ref kErrorNone if no error, error code if not.

	\sa open(const char*, eFileAccess) and open(Filename*, eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::close(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	FILE* fp = static_cast<FILE*>(m_pFile);
	if (fp) {
		if (fclose(static_cast<FILE*>(fp))) {
			uResult = kErrorIO;
		}
		m_pFile = nullptr;
	}
	return uResult;
}

/*! ************************************

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
	uint64_t uSize = 0;
	FILE* fp = static_cast<FILE*>(m_pFile);
	if (fp) {

		// Save the current file mark
		long Temp = ftell(fp);

		// Seek to the end of file
		if (!fseek(fp, 0, SEEK_END)) {

			// Get the file size
			uSize = static_cast<uint64_t>(ftell(fp));
		}

		// If no error, restore the old file mark
		if (Temp != -1) {
			fseek(fp, Temp, SEEK_SET);
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

	\return Number of bytes read (Can be less than what was requested due to EOF
		or read errors)

	\sa write(const void*, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::read(
	void* pOutput, uintptr_t uSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uSize && pOutput) {
		FILE* fp = static_cast<FILE*>(m_pFile);
		if (fp) {
			uResult = fread(pOutput, 1, uSize, fp);
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

	\return Number of bytes written (Can be less than what was requested due to
		EOF or write errors)

	\sa read(void *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::write(
	const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uSize && pInput) {
		FILE* fp = static_cast<FILE*>(m_pFile);
		if (fp) {
			uResult = fwrite(pInput, 1, uSize, fp);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Get the current file mark

	If a file is open, query the operating system for the location
	of the file mark for future reads or writes.

	\return Current file mark or zero if an error occurred

	\sa write(const void*, uintptr_t) or set_mark(uint64_t)

***************************************/

uint64_t BURGER_API Burger::File::get_mark(void) BURGER_NOEXCEPT
{
	uint64_t uMark = 0;
	FILE* fp = static_cast<FILE*>(m_pFile);
	if (fp) {

		// Save the current file mark
		long Temp = ftell(fp);

		// If no error, restore the old file mark
		if (Temp != -1) {
			uMark = static_cast<uint64_t>(Temp);
		}
	}
	return uMark;
}

/*! ************************************

	\brief Set the current file mark

	If a file is open, set the read/write mark at the location passed.

	\param uMark Value to set the new file mark to.

	\return \ref kErrorNone if successful, \ref kErrorOutOfBounds if not.

	\sa get_mark() or set_mark_at_EOF()

***************************************/

Burger::eError BURGER_API Burger::File::set_mark(uint64_t uMark) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotInitialized;
	FILE* fp = static_cast<FILE*>(m_pFile);
	if (fp) {
		// Seek to the end of file
		if (!fseek(fp, static_cast<long>(uMark), SEEK_SET)) {
			uResult = kErrorNone;
		} else {
			uResult = kErrorOutOfBounds;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Set the current file mark at the end of the file

	If a file is open, set the read/write mark to the end of the file.

	\return \ref kErrorNone if successful, \ref kErrorOutOfBounds if not.

	\sa get_mark() or set_mark(uint64_t)

***************************************/

Burger::eError BURGER_API Burger::File::set_mark_at_EOF(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorOutOfBounds;
	FILE* fp = static_cast<FILE*>(m_pFile);
	if (fp) {
		if (!fseek(fp, 0, SEEK_END)) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/*! ************************************

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
	pOutput->Clear();
	return kErrorNotSupportedOnThisPlatform;
}

/*! ************************************

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
	pOutput->Clear();
	return kErrorNotSupportedOnThisPlatform;
}

/*! ************************************

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
	const TimeDate_t* /* pInput */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}

/*! ************************************

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
	const TimeDate_t* /* pInput */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Open a file asynchronously.

	Create an event in the FileManager queue to open this file.

	\param pFileName Pointer to a "C" string containing a Burgerlib pathname
	\param uAccess Enumeration on permissions requested on the opened file

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors.

	\sa close_async()

***************************************/

Burger::eError BURGER_API Burger::File::open_async(
	const char* pFileName, eFileAccess uAccess) BURGER_NOEXCEPT
{
	m_Filename.assign(pFileName);
	FileManager::g_pFileManager->add_queue(
		this, FileManager::kIOCommandOpen, nullptr, uAccess);
	return kErrorNone;
}

/*! ************************************

	\brief Open a file asynchronously.

	Create an event in the FileManager queue to open this file.

	\param pFileName Pointer to a Filename object
	\param uAccess Enumeration on permissions requested on the opened file

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors.

	\sa close_async()

***************************************/

Burger::eError BURGER_API Burger::File::open_async(
	Filename* pFileName, eFileAccess uAccess) BURGER_NOEXCEPT
{
	m_Filename = *pFileName;
	FileManager::g_pFileManager->add_queue(
		this, FileManager::kIOCommandOpen, nullptr, uAccess);
	return kErrorNone;
}

/*! ************************************

	\brief Close open file asynchronously

	Queue up a close file event, if needed.

	\return \ref kErrorNone if no error, error code if not.

	\sa open(const char*, eFileAccess) and open(Filename*, eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::close_async(void) BURGER_NOEXCEPT
{
	FileManager::g_pFileManager->add_queue(
		this, FileManager::kIOCommandClose, nullptr, 0);
	return kErrorNone;
}

/*! ************************************

	\brief Read open file asynchronously

	Queue up a read file event.

	\param pOutput Pointer to buffer to receive the data.
	\param uSize Size in byte of the read operation.

	\return \ref kErrorNone if no error, error code if not.

	\sa open(const char*, eFileAccess) and open(Filename*, eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::read_async(
	void* pOutput, uintptr_t uSize) BURGER_NOEXCEPT
{
	FileManager::g_pFileManager->add_queue(
		this, FileManager::kIOCommandRead, pOutput, uSize);
	return kErrorNone;
}

#if !(defined(BURGER_MAC) || defined(BURGER_DARWIN)) || defined(DOXYGEN)

/*! ************************************

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
	uint32_t /* uCreatorType */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}

/*! ************************************

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
	uint32_t /* uFileType */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}

/*! ************************************

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
	return 0;
}

/*! ************************************

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
	return 0;
}

/*! ************************************

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
	uint32_t /* uCreatorType */, uint32_t /* uFileType */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}

#endif

/*! ************************************

	\brief Read a "C" string with the terminating zero to a file stream

	Read a "C" string with a terminating zero from the file stream. If the
	string read is larger than the buffer, it is truncated. The buffer will have
	an ending zero on valid read or a trucated read. If uLength was zero, then
	pInput can be \ref nullptr

	\param pOutput Pointer to a "C" string to write.
	\param uLength Size of the buffer (To prevent overruns)

	\return Zero if the string was read, \ref kErrorEndOfFile hit EOF

	\sa WriteCString(FILE *, const char *)

***************************************/

Burger::eError BURGER_API Burger::File::read_c_string(
	char* pOutput, uintptr_t uLength) BURGER_NOEXCEPT
{
	// Set the maximum buffer size
	// and remove 1 to make space or the ending zero
	const char* pEnd = (pOutput + uLength) - 1;
	uint_t uTemp;

	// Stay until either zero or EOF
	for (;;) {
		uint8_t Buffer;
		if (read(&Buffer, 1) != 1) {
			// EOF reached
			uTemp = 666;
			break;
		}

		// Exit due to end of string?
		uTemp = Buffer;
		if (!uTemp) {
			break;
		}
		// Can I store it?
		if (pOutput < pEnd) {
			// Inc the input pointer
			pOutput[0] = static_cast<char>(uTemp);
			++pOutput;
		}
	}

	// Any space in buffer?
	if (uLength) {
		// Add ending zero
		pOutput[0] = 0;
	}

	// Hit the EOF.
	if (uTemp) {
		return kErrorEndOfFile;
	}

	// Hit the end of string (More data may be present)
	return kErrorNone;
}

/*! ************************************

	\brief Read a big endian 32-bit value from a file.

	Given a file opened for reading, read a 32-bit value in big endian format
	from the file stream.

	\return A 32 bit big endian int converted to native endian

	\sa read_big_uint16() and read_little_uint32()

***************************************/

uint32_t BURGER_API Burger::File::read_big_uint32(void) BURGER_NOEXCEPT
{
	// Save the long word
	uint32_t mValue;
	read(&mValue, 4);
	return BigEndian::load(&mValue);
}

/*! ************************************

	\brief Read a big endian 16-bit value from a file.

	Given a file opened for reading, read a 16-bit value in big endian format
	from the file stream.

	\return A 16 bit big endian short converted to native endian

	\sa read_big_uint32() and read_little_uint16()

***************************************/

uint16_t BURGER_API Burger::File::read_big_uint16(void) BURGER_NOEXCEPT
{
	// Save the short word
	uint16_t mValue;
	read(&mValue, 2);
	return BigEndian::load(&mValue);
}

/*! ************************************

	\brief Read a little endian 32-bit value from a file.

	Given a file opened for reading, read a 32-bit value in little endian format
	from the file stream.

	\return A 32 bit little endian int converted to native endian

	\sa read_little_uint16() and read_big_uint32()

***************************************/

uint32_t BURGER_API Burger::File::read_little_uint32(void) BURGER_NOEXCEPT
{
	// Save the long word
	uint32_t mValue;
	read(&mValue, 4);
	return LittleEndian::load(&mValue);
}

/*! ************************************

	\brief Read a little endian 16-bit value from a file.

	Given a file opened for reading, read a 16-bit value in little endian format
	from the file stream.

	\return A 16 bit little endian short converted to native endian

	\sa read_little_uint32() and read_big_uint16()

***************************************/

uint16_t BURGER_API Burger::File::read_little_uint16(void) BURGER_NOEXCEPT
{
	// Save the long word
	uint16_t mValue;
	read(&mValue, 2);
	return LittleEndian::load(&mValue);
}
