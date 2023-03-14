/***************************************

	Unix version of the File class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#if !defined(DOXYGEN)

// Permission and access for ::open()
static const int g_Permissions[4] = {O_RDONLY, O_WRONLY | O_CREAT | O_TRUNC,
	O_WRONLY | O_CREAT, O_RDWR | O_CREAT};

// iOS doesn't have stat64, but stat performs the same action
#if (defined(BURGER_DARWIN) && defined(BURGER_ARM)) || defined(BURGER_IOS)
#define stat64 stat
#define fstat64 fstat
#endif


#if defined(BURGER_DARWIN)
#undef st_mtime
#undef st_atime
#define off64_t off_t
#define lseek64 lseek
#define st_mtime st_mtimespec
#define st_atime st_atimespec
#endif

#endif

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

	uAccess = static_cast<eFileAccess>(uAccess & 3);
	int fp = ::open(pFileName->get_native(), g_Permissions[uAccess], 0666);
	uint_t uResult = kErrorFileNotFound;
	if (fp != -1) {
		m_pFile = reinterpret_cast<void*>(fp);
		uResult = kErrorNone;
		if (uAccess == kAppend) {
			uResult = set_mark_at_EOF();
		}
	}
	return static_cast<eError>(uResult);
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
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
		int iClose = ::close(fp);
		if (iClose == -1) {
			uResult = kErrorIO;
		}
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
	uint64_t uSize = 0;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {

		// Get the data
		struct stat64 MyStat;
		int iError = fstat64(fp, &MyStat);

		// Sanity check for 64 bit support
		BURGER_STATIC_ASSERT(sizeof(MyStat.st_size) == 8);

		if (iError != -1) {
			uSize = static_cast<uint64_t>(MyStat.st_size);
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

uintptr_t BURGER_API Burger::File::read(
	void* pOutput, uintptr_t uSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uSize && pOutput) {
		int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp) {
			// Read data
			ssize_t uRead = ::read(fp, pOutput, uSize);
			if (uRead == static_cast<ssize_t>(-1)) {
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

	\sa read(void *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::write(
	const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uSize && pInput) {
		int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp) {

			// Write data
			ssize_t uWrite = ::write(fp, pInput, uSize);
			if (uWrite == static_cast<ssize_t>(-1)) {
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

	\sa write(const void*, uintptr_t) or set_mark(uint64_t)

***************************************/

uint64_t BURGER_API Burger::File::get_mark(void) BURGER_NOEXCEPT
{
	uint64_t uMark = 0;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {

		off64_t lCurrentMark = lseek64(fp, 0, SEEK_CUR);

		// Sanity check for 64 bit support
		BURGER_STATIC_ASSERT(sizeof(lCurrentMark) == 8);

		if (lCurrentMark != -1) {
			uMark = static_cast<uint64_t>(lCurrentMark);
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
	eError uResult = kErrorNotInitialized;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {

		off64_t lCurrentMark = lseek64(fp, uMark, SEEK_SET);

		// Sanity check for 64 bit support
		BURGER_STATIC_ASSERT(sizeof(lCurrentMark) == 8);

		if (lCurrentMark == -1) {
			uResult = kErrorOutOfBounds;
		} else {
			uResult = kErrorNone;
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
	eError uResult = kErrorNotInitialized;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {

		off64_t lCurrentMark = lseek64(fp, 0, SEEK_END);

		// Sanity check for 64 bit support
		BURGER_STATIC_ASSERT(sizeof(lCurrentMark) == 8);

		if (lCurrentMark == -1) {
			uResult = kErrorOutOfBounds;
		} else {
			uResult = kErrorNone;
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
	eError uResult = kErrorFileNotFound;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {
		struct stat MyStat;
		int iError = fstat(fp, &MyStat);
		if (iError != -1) {
			// Return the modification time
			pOutput->Load(reinterpret_cast<const timespec*>(&MyStat.st_mtime));
			uResult = kErrorNone;
		}
	}
	if (uResult != kErrorNone) {
		pOutput->Clear();
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
// Darwin has creation time in stat64
#if defined(BURGER_DARWIN)

	eError uResult = kErrorFileNotFound;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {

		// Get the data
		struct stat64 MyStat;
		int iError = fstat64(fp, &MyStat);

		if (iError != -1) {
			// If it succeeded, the file must exist
			pOutput->Load(&MyStat.st_birthtimespec);
			uResult = kErrorNone;
		}
	}
	if (uResult != kErrorNone) {
		pOutput->Clear();
	}
	return uResult;

// Linux and Android 11 or higher support statx
#elif defined(BURGER_LINUX) || \
	(defined(BURGER_ANDROID) && (__ANDROID_API__ >= 30))

	// If the filesystem supports it, get the creation time
	// Note: Usually this is for files on the network. Most
	// Linux filesystems do not support file creation time

	eError uResult = kErrorFileNotFound;
	int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
	if (fp) {

		// Use statx
		struct statx MyStat;
		int iError = statx(AT_FDCWD, m_Filename.get_native(),
			AT_SYMLINK_NOFOLLOW | AT_STATX_FORCE_SYNC,
			STATX_BTIME | STATX_BASIC_STATS, &MyStat);

		// Successful call?
		if (iError != -1) {

			// stx_btime is a statx_timestamp, translate to timespec
			struct timespec Spec;

			// If the time returned is zero, it means the file system
			// didn't support creation time
			if ((Spec.tv_sec = MyStat.stx_btime.tv_sec) == 0) {
				uResult = kErrorNotSupportedOnThisPlatform;
			} else {
				Spec.tv_nsec = MyStat.stx_btime.tv_nsec;
				pOutput->Load(&Spec);
				uResult = kErrorNone;
			}
		}
	}

	// Clear on error
	if (uResult != kErrorNone) {
		pOutput->Clear();
	}
	return uResult;

#else
	// Not on this platform
	pOutput->Clear();
	return kErrorNotSupportedOnThisPlatform;
#endif
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
	eError uResult = kErrorFileNotFound;
	struct timespec NewTime;

	// Convert time to timespec
	if (!pInput->Store(&NewTime)) {

		// See if the file exists
		int fp = static_cast<int>(reinterpret_cast<uintptr_t>(m_pFile));
		if (fp) {

			// Get the access time
			struct stat MyStat;
			int iError = fstat(fp, &MyStat);
			if (iError != -1) {
				timeval Array[2];

				// Access time from file
				TIMESPEC_TO_TIMEVAL(&Array[0],
					reinterpret_cast<const struct timespec*>(&MyStat.st_atime));

				// New modification time
				TIMEVAL_TO_TIMESPEC(&Array[1], &NewTime);

				// If using an old version of android, futimes() isn't available
#if defined(BURGER_ANDROID) && defined(__ANDROID_API__) && \
	(__ANDROID_API__ < 26)
				iError = utimes(m_Filename.get_native(), Array);
#else
				// Update the modification time
				iError = futimes(fp, Array);
#endif
				if (iError != -1) {
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

	\param pInput Pointer to a \ref TimeDate_t to use for the new file
		creation time

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa get_creation_time() or set_modification_time()

***************************************/

Burger::eError BURGER_API Burger::File::set_creation_time(
	const TimeDate_t* pInput) BURGER_NOEXCEPT
{
	BURGER_UNUSED(pInput);
	return kErrorNotSupportedOnThisPlatform;
}

// Clean up defines
#if defined(BURGER_IOS) && !defined(DOXYGEN)
#undef stat64
#undef fstat64
#endif

#if defined(BURGER_DARWIN) && !defined(DOXYGEN)
#undef st_mtime
#undef st_atime
#undef off64_t
#undef lseek64
#endif

#endif
