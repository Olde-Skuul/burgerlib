/***************************************

	Filename Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilename.h"
#include "brfilemanager.h"
#include "brmemoryfunctions.h"

/*! ************************************

	\class Burger::Filename
	\brief File name container

	This container class is a high speed conversion method to convert a
	BurgerLib formatted filename into a filename suitable for use with the
	native file system.

	Some operating systems require more information than just the filename. Due
	to this, it's highly recommended that a Burgerlib path is generated, then a
	call to get_native() is invoked to do the proper conversion and generate the
	extra data. Currently, only the MacOS Carbon and Classic targets require
	this extra data, however for future compatibility or performance upgrades,
	don't assume that creating a filename by hand will be a cross platform
	solution.

	When writing code intended for a single platform, it's encouraged to load
	and store the extra data. Also, you can use the set_native() call to
	convert a native filename into BurgerLib format without having to do
	anything special.

	\sa struct Burger::FileManager or class Burger::DirectorySearch

***************************************/

/*! ************************************

	\brief Ensure the filename ends with a colon.

	If the filename is not empty, check if the last character is a colon. If it
	is not, one will be appended.

	\return Zero if no error, non-zero if out of memory.

	\sa join(const char *)

***************************************/

Burger::eError BURGER_API Burger::Filename::end_with_colon(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	if (!m_Filename.empty()) {
		// Ensure it ends with a colon
		if (!m_Filename.ends_with(':')) {
			uResult = m_Filename.push_back(':');
		}
	}
	return uResult;
}

/*! ************************************

	\fn Burger::Filename::Filename()
	\brief Default constructor

	Simple inline initializer, Designed for high speed and does not call any
	other function

	\sa Burger::Filename::assign(const char *) or Burger::Filename::~Filename()

***************************************/

/*! ************************************

	\brief Initialize the pathname to the input string

	Given a string passed by pInput, set the contained string to match. In a
	majority of cases, no memory is allocated. If in the rare case the string is
	of a large length, it will allocate a buffer and store the string within the
	allocated memory and ignore the internal buffer..

	\note This function does not alter the string in any way. It is stored in
		the internal buffer as is. It also will not modify any platform specific
		variables.

	\param pFilename Pointer to a valid "C" string or \ref nullptr to force the
		class to empty.

	\sa clear() and assign(const char *)

***************************************/

Burger::Filename::Filename(const char* pFilename) BURGER_NOEXCEPT
	: m_Filename(pFilename),
	  m_bNativeValid(FALSE)
#if defined(BURGER_MAC)
	,
	  m_lDirID(0),
	  m_sVRefNum(0)
#endif
{
}

/*! ************************************

	\brief Initialize the pathname from another pathname

	Given a Burger::Filename passed by reference, set the contained string to
	match. In a majority of cases, no memory is allocated. If in the rare case
	the string is of a large length, it will allocate a buffer and store the
	string within the allocated memory and ignore the internal buffer..

	\note This function does not alter the string in any way. It is stored
	in the internal buffer as is. It also will
	not modify any platform specific variables.

	\param rInput Reference to another Filename to copy from
	\sa clear() and assign(const char *)

***************************************/

Burger::Filename::Filename(Filename const& rInput) BURGER_NOEXCEPT
	: m_Filename(rInput.m_Filename),
	  m_NativeFilename(rInput.m_NativeFilename),
	  m_bNativeValid(rInput.m_bNativeValid)
{
	// Copy the mac specific data
#if defined(BURGER_MAC)
	MemoryCopy(&m_FSRef, &rInput.m_FSRef, sizeof(m_FSRef));
	m_lDirID = rInput.m_lDirID;
	m_sVRefNum = rInput.m_sVRefNum;
#endif
}

/*! ************************************

	\brief Copy a Burger::Filename

	\param rInput Reference to a Filename to copy from
	\sa assign()

***************************************/

Burger::Filename& Burger::Filename::operator=(
	Filename const& rInput) BURGER_NOEXCEPT
{
	m_Filename = rInput.m_Filename;
	m_NativeFilename = rInput.m_NativeFilename;
	m_bNativeValid = rInput.m_bNativeValid;

#if defined(BURGER_MAC)
	MemoryCopy(&m_FSRef, &rInput.m_FSRef, sizeof(m_FSRef));
	m_lDirID = rInput.m_lDirID;
	m_sVRefNum = rInput.m_sVRefNum;
#endif

	return *this;
}

/*! ************************************

	\brief Move a Burger::Filename

	\param rInput Reference to a Filename to move from
	\sa assign(const char *)

***************************************/

#if defined(BURGER_RVALUE_REFERENCES) || defined(DOXYGEN)
Burger::Filename::Filename(Filename&& rInput) BURGER_NOEXCEPT
	: m_Filename(static_cast<String&&>(rInput.m_Filename)),
	  m_NativeFilename(static_cast<String&&>(rInput.m_NativeFilename)),
	  m_bNativeValid(rInput.m_bNativeValid)
#if defined(BURGER_MAC)
	,
	  m_lDirID(rInput.m_lDirID),
	  m_sVRefNum(rInput.m_sVRefNum)
#endif
{
#if defined(BURGER_MAC)
	MemoryCopy(m_FSRef, rInput.m_FSRef, sizeof(m_FSRef));
#endif
}

/*! ************************************

	\brief Move a Burger::Filename

	\param rInput Reference to a Filename to move from

	\sa assign(const char *)

***************************************/

Burger::Filename& Burger::Filename::operator=(Filename&& rInput) BURGER_NOEXCEPT
{
	m_Filename = static_cast<String&&>(rInput.m_Filename);
	m_NativeFilename = static_cast<String&&>(rInput.m_NativeFilename);
	m_bNativeValid = rInput.m_bNativeValid;

	// Copy the macOS specific data
#if defined(BURGER_MAC)
	m_lDirID = rInput.m_lDirID;
	m_sVRefNum = rInput.m_sVRefNum;
	MemoryCopy(m_FSRef, rInput.m_FSRef, sizeof(m_FSRef));
#endif
	return *this;
}
#endif

/*! ************************************

	\brief Release any memory allocated

	If in the course of setting a pathname to an excessive length, release any
	extra allocated memory. In most cases, this does nothing.

	\note Can call \ref Burger::Free() in some cases

***************************************/

Burger::Filename::~Filename() {}

/*! ************************************

	\fn Burger::Filename::c_str(void) const
	\brief Retrieve a pointer to the Burgerlib filename.

	Return a pointer to a valid const "C" string,

	String may be an empty string if it wasn't set via a previous call. This
	function will never return a \ref NULL pointer.

	\return A pointer to an unmodifiable "C" string. Make no attempt to write to
		it.

	\sa Filename::Set(const char *) or Filename::GetPtr(void)

***************************************/

/*! ************************************

	\fn Burger::Filename::c_str(void)
	\brief Retrieve a pointer to the filename.

	Return a pointer to a valid "C" string,

	String may be an empty string if it wasn't set via a previous call. This
	function will never return a \ref NULL pointer.

	\return A pointer to an modifiable "C" string. Make no attempt to write into
		the string since it may interfere with the operation of the class.

	\sa Set(const char *) or Filename::GetPtr(void) const

***************************************/

/*! ************************************

	\brief Sets the pathname to the input string

	Given a string passed by pInput, set the contained string to match. In a
	majority of cases, no memory is allocated. If in the rare case the string is
	of a large length, it will allocate a buffer and store the string within the
	allocated memory and ignore the internal buffer..

	\note This function does not alter the string in any way. It is stored in
		the internal buffer as is. It also will not modify any platform specific
		variables.

	\param pInput Pointer to a valid "C" string or \ref NULL to force the class
		to empty.

	\sa Burger::Filename::Clear()

***************************************/

Burger::eError BURGER_API Burger::Filename::assign(
	const char* pInput) BURGER_NOEXCEPT
{
	// Need to rebuild when getting native pathname
	m_bNativeValid = FALSE;
	return m_Filename.assign(pInput);
}

/*! ************************************

	\brief Sets the pathname to a UTF-16 input string

	Given a UTF-16 string passed by pInput, set the contained string to match.
	In a majority of cases, no memory is allocated. If in the rare case the
	string is of a large length, it will allocate a buffer and store the string
	within the allocated memory and ignore the internal buffer.

	\note This function does not alter the string in any way. It is stored in
	the internal buffer as is. It also will not modify any platform specific
	variables.

	\param pInput Pointer to a valid "C" string or \ref NULL to force the class
		to empty.

	\sa Clear() or Set(const char *)

***************************************/

Burger::eError BURGER_API Burger::Filename::assign(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	// Need to rebuild when getting native pathname
	m_bNativeValid = FALSE;
	return m_Filename.assign(pInput);
}

/*! ************************************

	\brief Set the filename to an empty string.

	If in the course of setting a pathname to an excessive length, release any
	extra allocated memory. In most cases, this simply resets the cached buffer
	to an empty string.

***************************************/

void BURGER_API Burger::Filename::clear(void) BURGER_NOEXCEPT
{
	// Clear the buffers
	m_bNativeValid = FALSE;
	m_Filename.clear();
	m_NativeFilename.clear();
}

/*! ************************************

	\brief Append a filename to the end of a path

	Given a filename, append the filename to the end of the path and add a
	trailing colon. A colon will be inserted between the existing path and the
	filename being appended.

	\param pInput Pointer to "C" string of the filename to append to the path.

	\return Zero if no error, non-zero if out of memory.

***************************************/

Burger::eError BURGER_API Burger::Filename::join(
	const char* pInput) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	if (pInput) {
		// Perform the actual work
		uResult = join(pInput, string_length(pInput));
	}
	return uResult;
}

/*! ************************************

	\brief Append a filename to the end of a path

	Given a filename, append the filename to the end of the path and add a
	trailing colon. A colon will be inserted between the existing path and the
	filename being appended.

	\param pInput Pointer to buffer with the filename to append to the path.
	\param uInputLength Length of the buffer to append

	\return Zero if no error, non-zero if out of memory.

***************************************/

Burger::eError BURGER_API Burger::Filename::join(
	const char* pInput, uintptr_t uInputLength) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;

	if (uInputLength) {

		// Ensure this path ends with a colon for a seamless merge
		uResult = end_with_colon();
		if (!uResult) {

			// Need to rebuild when getting native pathname
			m_bNativeValid = FALSE;
			uResult = m_Filename.append(pInput, uInputLength);

			// Failsafe to ensure the result ends with a colon
			if (!uResult) {
				uResult = end_with_colon();
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Obtain the filename in a path

	Given a pathname, return the filename at the end of the path. The output is
	guaranteed to have a terminating zero unless uOutputLength is zero.

	\note If the filename is too big to fit in the output buffer, it will be
		truncated.

	\param pOutput Buffer to receive the filename at the end of the path
	\param uOutputLength Size of the buffer to obtain the filename.

***************************************/

Burger::eError BURGER_API Burger::Filename::get_basename(
	char* pOutput, uintptr_t uOutputLength) const BURGER_NOEXCEPT
{

	// Get the length of the path
	const char* pInput = m_Filename.c_str();
	uintptr_t uLength = m_Filename.length();

	// Anything?
	if (uLength) {
		// If there is an ending colon?
		if (pInput[uLength - 1] == ':') {
			// Ignore the ending colon
			--uLength;
		}
		// More than just the colon?
		if (uLength) {

			// Scan backwards until a colon is found, or use the whole
			// string
			uintptr_t uIndex = uLength;
			do {
				--uIndex;
				if (pInput[uIndex] == ':') {
					++uIndex;
					// Remove the prefix
					uLength -= uIndex;
					pInput += uIndex;
					break;
				}
			} while (uIndex);
			// Either the entire string or the text to the right of the
			// colon remains at this point via pInput and uLength
		}
	}

	// Copy as much as possible
	string_copy(pOutput, uOutputLength, pInput, uLength);

	// Was there a buffer issue?
	eError uError = kErrorNone;
	if (uLength >= uOutputLength) {
		uError = kErrorBufferTooSmall;
	}
	return uError;
}

/*! ************************************

	\brief Extract the base name from a pathname

	Given a pathname, remove the directory from the beginning of the path,
	leaving only the file name remaining. The resulting filename will not have
	leading or trailing colons

	\param pOutput Pointer to a valid Burger::String instance to receive the new
		string

***************************************/

Burger::eError BURGER_API Burger::Filename::get_basename(
	String* pOutput) const BURGER_NOEXCEPT
{

	// Get the length of the path
	const char* pInput = m_Filename.c_str();
	uintptr_t uLength = m_Filename.length();

	// Anything?
	if (uLength) {
		// If there is an ending colon?
		if (pInput[uLength - 1] == ':') {
			// Ignore the ending colon
			--uLength;
		}
		// More than just the colon?
		if (uLength) {

			// Scan backwards until a colon is found, or use the whole
			// string
			uintptr_t uIndex = uLength;
			do {
				--uIndex;
				if (pInput[uIndex] == ':') {
					++uIndex;
					// Remove the prefix
					uLength -= uIndex;
					pInput += uIndex;
					break;
				}
			} while (uIndex);
			// Either the entire string or the text to the right of the
			// colon remains at this point via pInput and uLength
		}
	}
	return pOutput->assign(pInput, uLength);
}

/*! ************************************

	\brief Obtain the filename extension

	Given a pathname, return the file extension for the filename at the end of
	the path

	\note If the filename extension is too big to fit in the output buffer, it
		will be truncated.

	\param pOutput Buffer to receive the filename at the end of the path
	\param uOutputLength Size of the buffer to obtain the filename.

***************************************/

Burger::eError BURGER_API Burger::Filename::get_file_extension(
	char* pOutput, uintptr_t uOutputLength) const BURGER_NOEXCEPT
{
	// Get the length of the path
	const char* pInput = m_Filename.c_str();
	uintptr_t uLength = m_Filename.length();

	// Anything?
	if (uLength) {
		// If there is an ending colon?
		if (pInput[uLength - 1] == ':') {
			// Ignore the ending colon
			--uLength;
		}
		// More than just the colon?
		if (uLength) {
			// Scan backwards until a period is found, or a colon meaning
			// no extension exists
			uintptr_t uIndex = uLength;
			do {
				--uIndex;
				if (pInput[uIndex] == ':') {
					uLength = 0;
					break;
				}
				if (pInput[uIndex] == '.') {
					// The first character cannot be '.'
					if (!uIndex || (pInput[uIndex - 1] == ':')) {
						uLength = 0;
					} else {
						++uIndex;
						// Remove the prefix
						uLength -= uIndex;
						pInput += uIndex;
					}
					break;
				}
			} while (uIndex);
			// Either the entire string or the text to the right of the colon
			// remains at this point via pInput and uLength
		}
	}
	string_copy(pOutput, uOutputLength, pInput, uLength);

	// Was there a buffer issue?
	eError uError = kErrorNone;
	if (uLength >= uOutputLength) {
		uError = kErrorBufferTooSmall;
	}
	return uError;
}

/*! ************************************

	\brief Set the filename extension

	Given a filename extension, set the filename to this extension

	\param pExtension Pointer to a "C" string that contains the new filename
		extension

	\sa Burger::Filename::GetFileExtension(char *,uintptr_t) const

***************************************/

Burger::eError BURGER_API Burger::Filename::set_file_extension(
	const char* pExtension) BURGER_NOEXCEPT
{
	// Remove trailing colon if any
	if (m_Filename.ends_with(':')) {
		m_Filename.pop_back();
	}

	// Get the length of the path
	const char* pInput = m_Filename.c_str();
	uintptr_t uLength = m_Filename.length();

	// Remove the current extension
	if (uLength) {

		// Scan backwards until a period is found, or a colon meaning
		// no extension exists
		uintptr_t uIndex = uLength;
		do {
			// Found a colon before '.', abort
			--uIndex;
			if (pInput[uIndex] == ':') {
				break;
			}

			// Found dot? Check if this is the first character
			if (pInput[uIndex] == '.') {
				// The first character cannot be '.'
				if (uIndex && (pInput[uIndex - 1] != ':')) {
					// Remove the prefix
					uLength = uIndex;
				}
				break;
			}
		} while (uIndex);
		// Remove the '.txt' extension if one was found.
		m_Filename.resize(uLength);
	}

	// Note, it's possible if the filename is empty, that only the extension
	// will be appended, which will create a filename that's only the extension.

	uintptr_t uExtensionLength = 0;
	if (pExtension) {
		// Don't perform a double period insert
		if (pExtension[0] == '.') {
			++pExtension;
		}
		uExtensionLength = string_length(pExtension);
	}

	eError uResult = kErrorNone;

	// If there is no extension, end here.
	if (uExtensionLength) {

		// Insert the period and then the extension
		m_Filename.push_back('.');
		uResult = m_Filename.append(pExtension, uExtensionLength);
	}

	end_with_colon();
	m_bNativeValid = FALSE;
	return uResult;
}

/*! ************************************

	\brief Extract the directory from a pathname

	Given a pathname, remove the filename from the end of the path, leaving
	only the directory name remaining.

	\note If there is only the label name remaining, return the label name.

***************************************/

Burger::eError BURGER_API Burger::Filename::dirname(void) BURGER_NOEXCEPT
{
	const char* pFilename = m_Filename.c_str();

	// Get a character from the filename
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pFilename)[0];

	// Skip the first colon
	if (uTemp == ':') {
		uTemp = reinterpret_cast<const uint8_t*>(pFilename)[1];
		++pFilename;
	}

	// Is there a string?
	if (uTemp) {

		// Don't truncate
		const char* pLastColon = nullptr;
		do {
			// Is this the last colon?
			if (uTemp == ':') {
				// If it's the last character of the string?
				// Ignore it
				if (!pFilename[1]) {
					break;
				}
				// Consider this the last colon
				pLastColon = pFilename;
			}
			// Fetch a character
			uTemp = reinterpret_cast<const uint8_t*>(pFilename)[1];
			++pFilename;
		} while (uTemp);

		// Do I have a "last colon"?
		if (pLastColon) {

			// Truncate the string after the colon
			m_Filename.resize(
				static_cast<uintptr_t>(pLastColon - m_Filename.c_str()) + 1);
			m_bNativeValid = FALSE;
		}
	}

	// Since this function only shortens the filename, there is no
	// chance of a memory error.
	return kErrorNone;
}

/*! ************************************

	\brief Extract the directory from a pathname

	Given a pathname, remove the filename from the end of the path, leaving only
	the directory name remaining.

	\param pOutput Buffer to receive the filename at the end of the path
	\param uOutputLength Size of the buffer to obtain the filename.

***************************************/

Burger::eError BURGER_API Burger::Filename::get_dirname(
	char* pOutput, uintptr_t uOutputLength) const BURGER_NOEXCEPT
{
	const char* pFilename = m_Filename.c_str();

	// Get a character from the filename
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pFilename)[0];

	// Skip the first colon
	if (uTemp == ':') {
		uTemp = reinterpret_cast<const uint8_t*>(pFilename)[1];
		++pFilename;
	}
	const char* pLastColon = nullptr;
	// Is there a string?
	if (uTemp) {
		// Don't truncate
		do {
			// Is this the last colon?
			if (uTemp == ':') {
				// If it's the last character of the string?
				// Ignore it
				if (!pFilename[1]) {
					break;
				}
				// Consider this the last colon
				pLastColon = pFilename;
			}
			// Fetch a character
			uTemp = reinterpret_cast<const uint8_t*>(pFilename)[1];
			++pFilename;
		} while (uTemp);
		++pFilename;
	}

	// Do I have a "last colon"?
	if (pLastColon) {
		pFilename = pLastColon + 1;
	}
	pLastColon = m_Filename.c_str();
	uintptr_t uLength = static_cast<uintptr_t>(pFilename - pLastColon);

	// Save the string
	string_copy(pOutput, uOutputLength, pLastColon, uLength);

	// Was there a buffer issue?
	eError uError = kErrorNone;
	if (uLength >= uOutputLength) {
		uError = kErrorBufferTooSmall;
	}
	return uError;
}

/*! ************************************

	\brief Extract the directory from a pathname

	Given a pathname, remove the filename from the end of the path, leaving only
	the directory name remaining.

	\param pOutput Pointer to a valid Burger::String instance to receive the new
		string

***************************************/

Burger::eError BURGER_API Burger::Filename::get_dirname(
	String* pOutput) const BURGER_NOEXCEPT
{
	const char* pFilename = m_Filename.c_str();

	// Get a character from the filename
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pFilename)[0];

	// Skip the first colon
	if (uTemp == ':') {
		uTemp = reinterpret_cast<const uint8_t*>(pFilename)[1];
		++pFilename;
	}
	const char* pLastColon = nullptr;
	// Is there a string?
	if (uTemp) {
		// Don't truncate
		do {
			// Is this the last colon?
			if (uTemp == ':') {
				// If it's the last character of the string?
				// Ignore it
				if (!pFilename[1]) {
					break;
				}
				// Consider this the last colon
				pLastColon = pFilename;
			}
			// Fetch a character
			uTemp = reinterpret_cast<const uint8_t*>(pFilename)[1];
			++pFilename;
		} while (uTemp);
		++pFilename;
	}

	// Do I have a "last colon"?
	if (pLastColon) {
		pFilename = pLastColon + 1;
	}
	pLastColon = m_Filename.c_str();

	// Truncate the string after the colon
	return pOutput->assign(
		pLastColon, static_cast<uintptr_t>(pFilename - pLastColon));
}

/*! ************************************

	\brief Determine if a filename starts with a "drive number"

	A Burgerlib pathname can use drive letters. They are in the form of ".D2:"
	as a volume name where the period and D denote the path is a device and the
	number is the device number in the chain. While technically there is no
	limit to devices, it's considered an error to have a device number higher
	than 99.

	This function will check if the Burgerlib pathname is a device and return
	the device number if so. If not, UINT32_MAX will be returned as an
	error.

	\return Volume number if the path starts with a device number,
		UINT32_MAX if not.

	\sa is_abs()

***************************************/

uint_t BURGER_API Burger::Filename::is_drive_number(void) const BURGER_NOEXCEPT
{
	// Assume failure
	uint_t uDeviceNum = UINT32_MAX;

	const char* pFilename = m_Filename.c_str();
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pFilename)[0];
	if (uTemp == '.') {

		uint_t uTestChar2 = reinterpret_cast<const uint8_t*>(pFilename)[1];
		if ((uTestChar2 & 0xDFU) == 'D') {
			// Validate the input to only allow the form of
			// ".D22:" where it starts with ".D", followed by ascii digits, then
			// terminated with a colon.

			// Init numeric index
			uintptr_t uLength = 2;

			// Init drive number
			uDeviceNum = 0;
			do {
				// Get an ASCII char
				uTemp = pFilename[uLength];
				++uLength;

				// Proper end of string?
				if (uTemp == ':') {
					// If nothing was parsed, abort
					if (uLength == 3) {
						uDeviceNum = UINT32_MAX;
					}
					break;
				}
				uTemp -= '0';

				// Numeric value?
				if (uTemp >= 10) {
					// Force using the CWD
					uDeviceNum = UINT32_MAX;
					break;
				}

				// Adjust previous value
				uDeviceNum = uDeviceNum * 10;

				// Make full decimal result
				uDeviceNum = uDeviceNum + uTemp;
				// Loop until done
			} while (uDeviceNum < 100U);
		}
	}
	return uDeviceNum;
}

/*! ************************************

	\brief Determine if a filename is a fully qualified pathname

	Burgerlib pathnames can be prefixed, partially or fully qualified.
	A fully qualified pathname means that the entire pathname exists
	and a prefix will not be prefixed and there would be no need to
	"Expand" the pathname before converting it to a native format.

	There are two ways a pathname is fully qualified, firstly if
	the pathname starts with a ":", it's assumed that the first
	name is the name of the volume. If it's ".D2:", then the number between
	the "D" and the ":" is the volume number (Or mapped to a drive letter
	in the case of Windows or MSDOS)

	\return \ref TRUE if the pathname is fully qualified, \ref FALSE if not.
	\sa is_filename_only()

***************************************/

uint_t BURGER_API Burger::Filename::is_abs(void) const BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	const char* pFilename = m_Filename.c_str();
	const uint_t uTemp = reinterpret_cast<const uint8_t*>(pFilename)[0];

	// Volume by name?
	if (uTemp == ':') {
		uResult = TRUE;

		// Volume by number?
	} else if (is_drive_number() != UINT32_MAX) {
		// It's valid!!!
		uResult = TRUE;
	}
	return uResult;
}

/*! ************************************

	\brief Determine if a filename has no prefix and is not fully qualified

	There are two ways a pathname not qualified, firstly if the pathname is a
	full pathname, it's qualified. If the pathname has a prefix start, it's
	qualified.

	Filenames of this nature will be automatically assumed to be offset from
	directory Burger::FileManager::kPrefixCurrent (the current working
	directory)

	\return \ref TRUE if the pathname is not qualified, \ref FALSE if not.
	\sa is_abs() const

***************************************/

uint_t BURGER_API Burger::Filename::is_filename_only(void) const BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	if (!is_abs()) {
		if (has_prefix_number() == FileManager::kPrefixInvalid) {
			uResult = TRUE;
		}
	}
	return uResult;
}

/*! ************************************

	\brief Determine if a filename has a prefix.

	Is the pathname starts with "8:" or "*:" or any other valid prefix
	values, then it's considered prefixed and it will have that directory
	prefixed when converting it to a fully qualified pathname.

	\return FileManager::kPrefixInvalid if there is no prefix or the prefix
		number if it does.

	\sa is_filename_only()

***************************************/

uint_t BURGER_API Burger::Filename::has_prefix_number(
	void) const BURGER_NOEXCEPT
{
	// Assume failure
	uint_t uPrefixNum = FileManager::kPrefixInvalid;

	const char* pFilename = m_Filename.c_str();
	uint_t uTestChar = reinterpret_cast<const uint8_t*>(pFilename)[0];

	// Has to start with number
	uTestChar -= '0';
	if (uTestChar < 10U) {

		// Is it a number? If it's a valid prefix number followed by a colon
		// yank it out and set uPrefixNum to the value

		uintptr_t uIndex = 1;
		do {
			// I found an ending colon, accept it, maybe
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pFilename)[uIndex];
			++uIndex;                   // Next value
			if (uTemp == ':') {         // End the prefix here?
				uPrefixNum = uTestChar; // This is the prefix I want
				break;                  // It's good!
			}

			// 0-9?
			uTemp -= '0';

			// Valid digit?
			if (uTemp >= 10) {
				// It's a filename that starts with a number like 2Temp
				break;
			}

			// Shift up the previous number
			uTestChar = uTestChar * 10;
			// Total prefix number
			uTestChar = uTestChar + uTemp;

			// Test for overflow
		} while (uTestChar < FileManager::kPrefixCount);

		// Test for special prefixes
	} else if (pFilename[1] == ':') {

		// Check for the special case prefixes of "$:", "*:" and "@:"
		if (uTestChar == static_cast<uint_t>('$' - '0')) {
			// System folder?
			uPrefixNum = FileManager::kPrefixSystem;

		} else if (uTestChar == static_cast<uint_t>('*' - '0')) {
			// Boot volume?
			uPrefixNum = FileManager::kPrefixBoot;

		} else if (uTestChar == static_cast<uint_t>('@' - '0')) {
			// Prefs folder?
			uPrefixNum = FileManager::kPrefixPrefs;
		}
	}
	return uPrefixNum;
}

/*! ************************************

	\brief Expand a filename by using prefix mapping.

	Using the rules for a Burgerlib type pathname, expand a path
	into a FULL pathname native to the Burgerlib file system.

	Directory delimiters are colons only.

	If the path starts with a colon, then it is a full pathname starting with a
	volume name. If the path starts with ".D2:" then it is a full pathname
	starting with a drive number. If the path starts with a "$:","*:" or "@:"
	then use special prefix numbers 32-34 If the path starts with 0: through 31:
	then use prefix 0-31. Otherwise prefix the pathname with the contents of
	prefix 8 ("Default")

	If the path after the prefix is removed is a period then POP the number of
	directories from the pathname for each period present after the first.
	Example "..:PrevDir:File:" will go down one directory and up the directory
	PrevDir

	All returned pathnames will have a trailing colon

	\param pInput Pointer to a pathname string

***************************************/

Burger::eError BURGER_API Burger::Filename::abs_path(
	const char* pInput) BURGER_NOEXCEPT
{
	if (!pInput || !pInput[0]) {
		// Assign to the current prefix and be done with it.
		m_bNativeValid = FALSE;
		return FileManager::get_prefix(&m_Filename, FileManager::kPrefixCurrent);
	}
	eError uResult = assign(pInput);
	if (!uResult) {
		uResult = abs_path();
	}
	return uResult;
}

/*! ************************************

	\brief Expand a filename by using prefix mapping.

	Using the rules for a Burgerlib type pathname, expand a path
	into a FULL pathname native to the Burgerlib file system.

	Directory delimiters are colons only.

	If the path starts with a colon, then it is a full pathname starting with a
	volume name. If the path starts with ".D2:" then it is a full pathname
	starting with a drive number. If the path starts with a "$:","*:" or "@:"
	then use special prefix numbers 32-34 If the path starts with 0: through 31:
	then use prefix 0-31. Otherwise prefix the pathname with the contents of
	prefix 8 ("Default")

	If the path after the prefix is removed is a period then POP the number of
	directories from the pathname for each period present after the first.
	Example "..:PrevDir:File:" will go down one directory and up the directory
	PrevDir

	All returned pathnames will have a trailing colon

***************************************/

Burger::eError BURGER_API Burger::Filename::abs_path(void) BURGER_NOEXCEPT
{
	// Does the filename need a prefix?

	if (!is_abs()) {

		// Parse the prefix number
		uint_t uPrefix = has_prefix_number();

		// Found one, remove it.
		if (uPrefix != FileManager::kPrefixInvalid) {
			// Find the colon after the prefix
			const char* pFound = StringCharacter(m_Filename.c_str(), ':');
			const uintptr_t uIndex =
				static_cast<uintptr_t>(pFound - m_Filename.c_str());

			// Remove the prefix from the filename
			m_Filename.Remove(0, uIndex + 1);
		} else {

			// Use the current prefix
			uPrefix = FileManager::kPrefixCurrent;
		}

		// Insert the prefix from the prefix library
		String Prefix;
		FileManager::get_prefix(&Prefix, uPrefix);
		m_Filename.insert(0, Prefix.c_str(), Prefix.length());
		m_bNativeValid = FALSE;
	}

	// At this point, the path has the proper prefix, make sure this ends with a
	// colon
	end_with_colon();

	// Get rid of all of the :.: entries
	for (;;) {
		const char* pSinglePeriod = StringString(m_Filename.c_str(), ":.:");
		if (!pSinglePeriod) {
			break;
		}
		m_Filename.Remove(
			static_cast<uintptr_t>(pSinglePeriod - m_Filename.c_str()), 2);
		m_bNativeValid = FALSE;
	}

	// Now deal with the :..: directory entries by turning them into :.: entries
	// and removing the previous directory.

	// Make sure the volume name is never discarded.

	for (;;) {

		// Look for ":..", if not found, abort
		const char* pDoublePeriod = StringString(m_Filename.c_str(), ":..");
		if (!pDoublePeriod) {
			break;
		}

		// Locate the start of the previous directory
		const char* pDirName = MemoryCharacterReverse(m_Filename.c_str(),
			static_cast<uintptr_t>(pDoublePeriod - m_Filename.c_str()), ':');

		uintptr_t uRemoveStart;
		uintptr_t uRemoveLength;

		// Is the previous directory a label? Or nonexistent?
		if (!pDirName || (pDirName == m_Filename.c_str())) {

			// Only remove the period in the center of ":.."
			uRemoveStart =
				static_cast<uintptr_t>(pDoublePeriod - m_Filename.c_str()) + 1;
			uRemoveLength = 1;
			// Special case, if it turns a ":..:" into a ":.:", then get rid of
			// it.
			if (!MemoryCompare(pDoublePeriod, ":..:", 4)) {
				uRemoveLength = 3;
			}

		} else {
			// Remove the previous directory, starting after the colon, and
			// ending with the ":." at the end.
			uRemoveStart =
				static_cast<uintptr_t>(pDirName - m_Filename.c_str()) + 1;
			uRemoveLength =
				static_cast<uintptr_t>(pDoublePeriod - pDirName) + 1;

			// Special case. If the entry is ":..:", get rid of it too.
			if (pDoublePeriod[3] == ':') {
				uRemoveLength += 2;
			}
		}

		// Remove either just a dot from after the colon in ":.." or remove the
		// previous directory and a dot
		m_Filename.Remove(uRemoveStart, uRemoveLength);
		m_bNativeValid = FALSE;
	}

	return end_with_colon();
}

/*! ************************************

	\brief Expand a filename from the BurgerLib format to the native OS format..

	For generic code, convert a BurgerLib path into one suitable for the
	current operating system. This function is mostly used when custom code
	for a specific platform is being written and the native path is required.

	\note If this function is invoked on a MacOS target, a Volume Reference
		number and Directory ID are generated and can be accessed via calls to
		Burger::Filename::get_DirID(void) const and
		Burger::Filename::get_VRefNum(void) const

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MAC) || defined(BURGER_DARWIN) || defined(BURGER_UNIX) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
const char* BURGER_API Burger::Filename::get_native(void) BURGER_NOEXCEPT
{
	// If already parsed, skip the conversion.
	if (!m_bNativeValid) {

		// Resolve prefixes and convert to absolute path
		abs_path();

		// Use the burgerlib path as a generic path
		m_NativeFilename = m_Filename;

		// It's valid
		m_bNativeValid = TRUE;
	}

	// Return the translated path
	return m_NativeFilename.c_str();
}
#endif

/*! ************************************

	\brief Set the filename to the current working directory

	Query the operating system for the current working directory and set the
	filename to that directory. The path is converted into UTF8 character
	encoding and stored in Burgerlib filename format

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

	\return \ref kErrorNone if successful, \ref kErrorPathNotFound if invalid,
		or \ref kErrorNotSupportedOnThisPlatform if not implemented on the
		platform.

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_XBOX360) || defined(BURGER_MAC) || defined(BURGER_UNIX) || \
	defined(BURGER_VITA)) || \
	defined(DOXYGEN)

Burger::eError BURGER_API Burger::Filename::set_system_working_directory(
	void) BURGER_NOEXCEPT
{
	clear();
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Set the filename to the application's directory

	Determine the directory where the application resides and set the filename
	to that directory. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_XBOX360) || defined(BURGER_MAC) || \
	defined(BURGER_DARWIN) || defined(BURGER_LINUX) || \
	defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::set_application_directory(
	void) BURGER_NOEXCEPT
{
	clear();
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Set the filename to the boot volume directory

	Determine the directory of the drive volume that the operating system was
	loaded from. The path is converted into UTF8 character encoding and stored
	in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_XBOX360) || defined(BURGER_LINUX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::set_boot_volume(
	void) BURGER_NOEXCEPT
{
	// Most non-windows platforms use the first volume as the boot volume
	return FileManager::get_volume_name(this, 0);
}
#endif

/*! ************************************

	\brief Set the filename to the local machine preferences directory

	Determine the directory where the user's preferences that are local to the
	machine is located. The path is converted into UTF8 character encoding and
	stored in Burgerlib filename format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_XBOX360) || defined(BURGER_MAC) || \
	defined(BURGER_DARWIN) || defined(BURGER_LINUX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::set_system_prefs_directory(
	void) BURGER_NOEXCEPT
{
	clear();
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Set the filename to the user's preferences directory

	Determine the directory where the user's preferences that could be shared
	among all machines the user has an account with is located. The path is
	converted into UTF8 character encoding and stored in Burgerlib filename
	format.

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_XBOX360) || defined(BURGER_MAC) || \
	defined(BURGER_DARWIN) || defined(BURGER_LINUX)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::set_user_prefs_directory(
	void) BURGER_NOEXCEPT
{
	clear();
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Expand a filename from the native format to Burgerlib.

	For generic code, obtain a filename (Usually from a command line) and
	convert it to a BurgerLib path. This function is an inline redirection to
	the proper low level function that will perform the actual conversion.

	\param pInput Pointer to a pathname string

	\note Due to MacOS requiring extra information such as a directory ID and
	volume number, ensure that the current volume and working directory are
	pointing to the directory the file is located. The MacOS call HSetVol() and
	HGetVol() can assist in those cases. It's preferred to use the \ref
	Burger::ConsoleApp class's built in file name management functions since
	they take this quirk into account.

	\sa set_native(const uint16_t *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MAC) || defined(BURGER_DARWIN) || defined(BURGER_UNIX) || \
	defined(BURGER_XBOX360) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::set_native(
	const char* pInput) BURGER_NOEXCEPT
{
	assign(pInput);
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Convert a native filename to Burgerlib format using UTF16 encoding.

	Copy the native pathname string into the internal native pathname buffer
	after converting the string to UTF-8. It's mostly used on Windows platforms
	since it's native character encoding is UTF16.

	\note This function does not exist on MacOS classic since MacOS classic only
	uses MacRomanUS 8 bit character encodings and a volume number and directory
	ID, so UTF16 support is not needed.

	\param pInput Pointer to a pathname string

	\sa set_native(const char *) or set_native(const char *,long,short)

***************************************/

#if !defined(BURGER_MAC) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::set_native(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	// Convert to UTF8
	String Temp(pInput);

	// Set the path using UTF8
	return set_native(Temp.c_str());
}
#endif

/*! ************************************

	\brief Allocate a new Filename

	Allocate memory using the Burgerlib memory manager and initialize it to a
	default Filename class instance.

	Delete with a call to Delete()

	\sa Delete(const T *)

***************************************/

Burger::Filename* BURGER_API Burger::Filename::New(void) BURGER_NOEXCEPT
{
	return new (Alloc(sizeof(Filename))) Filename();
}

/*! ************************************

	\brief Allocate a new Filename with a Burgerlib "C" string

	Allocate memory using the Burgerlib memory manager and initialize it to a
	default Filename class instance.

	Delete with a call to Delete()

	\param pFilename Pointer to a "C" string formatted as a Burgerlib pathname
	\sa Delete(const T *)

***************************************/

Burger::Filename* BURGER_API Burger::Filename::New(
	const char* pFilename) BURGER_NOEXCEPT
{
	return new (Alloc(sizeof(Filename))) Filename(pFilename);
}

/*! ************************************

	\brief Allocate a copy of a Filename

	Allocate memory using the Burgerlib memory manager and initialize it as a
	copy of another Filename class instance.

	Delete with a call to Delete()

	\param rInput Reference to a Filename to copy from
	\sa Delete(const T *)

***************************************/

Burger::Filename* BURGER_API Burger::Filename::New(
	Filename const& rInput) BURGER_NOEXCEPT
{
	return new (Alloc(sizeof(Filename))) Filename(rInput);
}
