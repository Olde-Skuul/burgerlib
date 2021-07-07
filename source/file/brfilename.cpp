/***************************************

	Filename Class

	Copyright (c) 1995-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

	This container class is a high speed conversion method
	to convert a BurgerLib formatted filename into a
	filename suitable for use with the native file system.

	Some operating systems require more information than
	just the filename. Due to this, it's highly recommended
	that a Burgerlib path is generated, then a call to
	GetNative() is invoked
	to do the proper conversion and generate the extra
	data. Currently, only the MacOS Carbon and Classic
	targets require this extra data, however for future
	compatibility or performance upgrades, don't assume
	that creating a filename by hand will be a cross platform
	solution.

	When writing code intended for a single platform, it's
	encouraged to load and store the extra data. Also,
	you can use the SetFromNative()
	call to convert a native filename into BurgerLib
	format without having to do anything special.

	\note This class will occupy 512 bytes per instance. No more, no less.

	\sa struct Burger::FileManager or class Burger::DirectorySearch

***************************************/

/*! ************************************

	\fn Burger::Filename::Filename()
	\brief Default constructor

	Simple inline initializer, Designed for high speed and
	does not call any other function

	\sa Burger::Filename::Set(const char *) or Burger::Filename::~Filename()

***************************************/

/*! ************************************

	\brief Initialize the pathname to the input string

	Given a string passed by pInput, set the contained string to match.
	In a majority of cases, no memory is allocated. If in the
	rare case the string is of a large length, it will allocate a
	buffer and store the string within the allocated memory and ignore
	the internal buffer..

	\note This function does not alter the string in any way. It is stored
	in the internal buffer as is. It also will
	not modify any platform specific variables.

	\param pFilename Pointer to a valid "C" string or \ref NULL to force the
class to empty. \sa Clear() and Set()

***************************************/

Burger::Filename::Filename(const char* pFilename) BURGER_NOEXCEPT
	: m_pFilename(m_Filename),
	  m_pNativeFilename(m_NativeFilename)
#if defined(BURGER_MAC)
	,
	  m_lDirID(0),
	  m_sVRefNum(0)
#endif
{
	m_NativeFilename[0] = 0;
	// Was there an input string?
	if (!pFilename) {
		// All initialized in case of an empty input string
		m_Filename[0] = 0;
	} else {
		uintptr_t uLength = StringLength(pFilename);
		//
		// Can the string fit in the local buffer?
		if (uLength < sizeof(m_Filename)) {
			// Yep! Copy and get lost
			MemoryCopy(m_Filename, pFilename, uLength + 1);
		} else {
			// Make a copy into an allocated buffer
			char* pNew = StringDuplicate(pFilename);
			// Are we boned?
			if (pNew) {
				// We're good, otherwise, return an empty class
				m_pFilename = pNew;
			} else {
				m_Filename[0] = 0;
			}
		}
	}
}

/*! ************************************

	\brief Initialize the pathname from another pathname

	Given a Burger::Filename passed by reference, set the contained string to
match. In a majority of cases, no memory is allocated. If in the rare case the
string is of a large length, it will allocate a buffer and store the string
within the allocated memory and ignore the internal buffer..

	\note This function does not alter the string in any way. It is stored
	in the internal buffer as is. It also will
	not modify any platform specific variables.

	\param rInput Reference to another Filename to copy from
	\sa Clear() and Set()

***************************************/

Burger::Filename::Filename(Filename const& rInput) BURGER_NOEXCEPT
{
	const char* pFilename = rInput.m_pFilename;
	uintptr_t uLength = StringLength(pFilename);
	char* pNew = m_Filename;

	// Can the string fit in the local buffer?
	if (uLength < sizeof(m_Filename)) {
		// Yep! Copy and get lost
		MemoryCopy(pNew, pFilename, uLength + 1);
	} else {
		// Make a copy into an allocated buffer
		pNew = StringDuplicate(pFilename);
		// Are we boned?
		if (!pNew) {
			// We're boned
			pNew = m_Filename;
			m_Filename[0] = 0;
		}
	}
	m_pFilename = pNew;

	// Get the translated pathname
	pFilename = rInput.m_pNativeFilename;
	uLength = StringLength(pFilename);
	pNew = m_NativeFilename;

	// Can the string fit in the local buffer?
	if (uLength < sizeof(m_NativeFilename)) {
		// Yep! Copy and get lost
		MemoryCopy(pNew, pFilename, uLength + 1);
	} else {
		// Make a copy into an allocated buffer
		pNew = StringDuplicate(pFilename);
		// Are we boned?
		if (!pNew) {
			// We're boned
			pNew = m_NativeFilename;
			m_NativeFilename[0] = 0;
		}
	}
	m_pNativeFilename = pNew;

	// Copy the mac specific data

#if defined(BURGER_MAC)
	m_lDirID = rInput.m_lDirID;
	m_sVRefNum = rInput.m_sVRefNum;
#endif
}

/*! ************************************

	\brief Copy a Burger::Filename

	\param rInput Reference to a Filename to copy from
	\sa Set()

***************************************/

Burger::Filename& Burger::Filename::operator=(Filename const& rInput)
{
	Clear();

	const char* pFilename = rInput.m_pFilename;
	uintptr_t uLength = StringLength(pFilename);
	char* pNew = m_Filename;

	// Can the string fit in the local buffer?
	if (uLength < sizeof(m_Filename)) {
		// Yep! Copy and get lost
		MemoryCopy(pNew, pFilename, uLength + 1);
	} else {
		// Make a copy into an allocated buffer
		pNew = StringDuplicate(pFilename);
		// Are we boned?
		if (!pNew) {
			// We're boned
			pNew = m_Filename;
			m_Filename[0] = 0;
		}
	}
	m_pFilename = pNew;

	// Get the translated pathname
	pFilename = rInput.m_pNativeFilename;
	uLength = StringLength(pFilename);
	pNew = m_NativeFilename;

	// Can the string fit in the local buffer?
	if (uLength < sizeof(m_NativeFilename)) {
		// Yep! Copy and get lost
		MemoryCopy(pNew, pFilename, uLength + 1);
	} else {
		// Make a copy into an allocated buffer
		pNew = StringDuplicate(pFilename);
		// Are we boned?
		if (!pNew) {
			// We're boned
			pNew = m_NativeFilename;
			m_NativeFilename[0] = 0;
		}
	}
	m_pNativeFilename = pNew;

#if defined(BURGER_MAC)
	m_lDirID = rInput.m_lDirID;
	m_sVRefNum = rInput.m_sVRefNum;
#endif

	return *this;
}

/*! ************************************

	\brief Release any memory allocated

	If in the course of setting a pathname to an excessive length,
	release any extra allocated memory. In most cases, this does nothing.

	\note Can call \ref Burger::Free() in some cases

***************************************/

Burger::Filename::~Filename()
{
	if (m_pFilename != m_Filename) {
		Free(m_pFilename);
	}
	if (m_pNativeFilename != m_NativeFilename) {
		Free(m_pNativeFilename);
	}
}

/*! ************************************

	\fn Burger::Filename::GetPtr(void) const
	\brief Retrieve a pointer to the Burgerlib filename.

	Return a pointer to a valid const "C" string,

	String may be an empty string if it wasn't set
	via a previous call. This function will never
	return a \ref NULL pointer.

	\return A pointer to an unmodifiable "C" string.
	Make no attempt to write to it,

	\sa Filename::Set(const char *) or Filename::GetPtr(void)

***************************************/

/*! ************************************

	\fn Burger::Filename::GetPtr(void)
	\brief Retrieve a pointer to the filename.

	Return a pointer to a valid "C" string,

	String may be an empty string if it wasn't set
	via a previous call. This function will never
	return a \ref NULL pointer.

	\return A pointer to an modifiable "C" string.
	Make no attempt to write into the string since
	it may interfere with the operation of the class.

	\sa Set(const char *) or Filename::GetPtr(void) const

***************************************/

/*! ************************************

	\brief Sets the pathname to the input string

	Given a string passed by pInput, set the contained string to match.
	In a majority of cases, no memory is allocated. If in the
	rare case the string is of a large length, it will allocate a
	buffer and store the string within the allocated memory and ignore
	the internal buffer..

	\note This function does not alter the string in any way. It is stored
	in the internal buffer as is. It also will
	not modify any platform specific variables.

	\param pInput Pointer to a valid "C" string or \ref NULL to force the class
to empty. \sa Burger::Filename::Clear()

***************************************/

Burger::eError BURGER_API Burger::Filename::Set(
	const char* pInput) BURGER_NOEXCEPT
{
	Clear();

	// Was there an input string?
	if (pInput) {
		const uintptr_t uLength = StringLength(pInput);
		//
		// Can the string fit in the local buffer?
		if (uLength < sizeof(m_Filename)) {
			// Yep! Copy and get lost
			MemoryCopy(m_Filename, pInput, uLength + 1);
		} else {
			// Make a copy into an allocated buffer
			char* pNew = StringDuplicate(pInput);
			// Are we boned?
			if (pNew) {
				// We're good, otherwise, return an empty class
				m_pFilename = pNew;
			}
		}
	}
	return kErrorNone;
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

Burger::eError BURGER_API Burger::Filename::Set(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	// Convert from UTF-16 to UTF-8
	String Temp(pInput);
	// Set the string
	return Set(Temp.c_str());
}

/*! ************************************

	\brief Set the filename to an empty string.

	If in the course of setting a pathname to an excessive length,
	release any extra allocated memory. In most cases, this simply
	resets the cached buffer to an empty string.

***************************************/

void BURGER_API Burger::Filename::Clear(void) BURGER_NOEXCEPT
{
	// Anything allocated?
	if (m_pFilename != m_Filename) {
		// It's toast
		Free(m_pFilename);
		// Reset my buffer pointer
		m_pFilename = m_Filename;
	}
	// Clear the string
	m_Filename[0] = 0;

	// Anything allocated?
	if (m_pNativeFilename != m_NativeFilename) {
		// It's toast
		Free(m_pNativeFilename);
		// Reset my buffer pointer
		m_pNativeFilename = m_NativeFilename;
	}
}

/*! ************************************

	\brief Append a filename to the end of a path

	Given a filename, append the filename to the end of the path and add a
	trailing colon

***************************************/

Burger::eError BURGER_API Burger::Filename::Append(const char* pInput)
{
	if (pInput) {
		// Get the length of the string to append
		const uintptr_t uInputLength = StringLength(pInput);
		if (uInputLength) {
			// Get the original string
			const char* pFilename = m_pFilename;
			uintptr_t uFilenameLength = StringLength(pFilename);
			// Size of the proposed final string (And space for the separating
			// colon) the '2' is for the possible 2 colons that could be
			// inserted into the pathname
			uintptr_t uTotal = uInputLength + uFilenameLength + 2;
			// Will it fit in the local buffer?
			char* pOutput = m_Filename;
			if (uTotal >= sizeof(m_Filename)) {
				// Allocate the new space
				pOutput = StringDuplicate(pFilename, uInputLength + 2);
			} else {
				MemoryCopy(pOutput, pFilename, uFilenameLength);
			}
			// Make sure it ends with a colon
			if (uFilenameLength && pOutput[uFilenameLength - 1] != ':') {
				pOutput[uFilenameLength] = ':';
				++uFilenameLength;
			}
			// Append the string
			MemoryCopy(pOutput + uFilenameLength, pInput, uInputLength);
			// Ensure it ends with a colon
			uTotal = uInputLength + uFilenameLength;
			if (uTotal && pOutput[uTotal - 1] != ':') {
				pOutput[uTotal] = ':';
				++uTotal;
			}
			pOutput[uTotal] = 0;
			// Was the old string allocated?
			if (pFilename != m_Filename) {
				Free(pFilename);
			}
			// Store my new string
			m_pFilename = pOutput;
		}
	}
	return kErrorNone;
}

/*! ************************************

	\brief Obtain the filename in a path

	Given a pathname, return the filename at the end of the path

	\note If the filename is too big to fit in the output buffer, it
	will be truncated.

	\param pOutput Buffer to receive the filename at the end of the path
	\param uOutputLength Size of the buffer to obtain the filename.

***************************************/

void BURGER_API Burger::Filename::GetFileName(
	char* pOutput, uintptr_t uOutputLength) const
{
	// Get the length of the path
	const char* pInput = m_pFilename;
	uintptr_t uLength = StringLength(pInput);
	// Anything?
	if (uLength) {
		// If there is an ending colon?
		if (pInput[uLength - 1] == ':') {
			// Ignore the ending colon
			--uLength;
		}
		// More than just the colon?
		if (uLength) {
			// Scan backwards until a colon is found, or use the whole string
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
			// Either the entire string or the text to the right of the colon
			// remains at this point via pInput and uLength
		}
	}
	StringCopy(pOutput, uOutputLength, pInput, uLength);
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

void BURGER_API Burger::Filename::GetFileExtension(
	char* pOutput, uintptr_t uOutputLength) const BURGER_NOEXCEPT
{
	// Get the length of the path
	const char* pInput = m_pFilename;
	uintptr_t uLength = StringLength(pInput);
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
	StringCopy(pOutput, uOutputLength, pInput, uLength);
}

/*! ************************************

	\brief Set the filename extension

	Given a filename extension, set the filename to this extension

	\param pExtension Pointer to a "C" string that contains the new filename
extension \sa Burger::Filename::GetFileExtension(char *,uintptr_t) const

***************************************/

void BURGER_API Burger::Filename::SetFileExtension(
	const char* pExtension) BURGER_NOEXCEPT
{
	// Get the length of the path
	const char* pInput = m_pFilename;
	uintptr_t uLength = StringLength(pInput);
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
					break;
				}
				if (pInput[uIndex] == '.') {
					// The first character cannot be '.'
					if (uIndex && (pInput[uIndex - 1] != ':')) {
						// Remove the prefix
						uLength = uIndex;
					}
					break;
				}
			} while (uIndex);
			// Either the entire string or the text to the right of the colon
			// remains at this point via pInput and uLength
		}
	}
	uintptr_t uExtensionLength = 0;
	if (pExtension) {
		// Don't perform a double period insert
		if (pExtension[0] == '.') {
			++pExtension;
		}
		uExtensionLength = StringLength(pExtension);
	}
	uintptr_t uTotal = uLength + 1; // Add for the ending colon
	if (uExtensionLength) {
		uTotal += uExtensionLength + 1; /// Add for the '.' separator
	}
	// Will it fit in the local buffer?
	char* pOutput = m_Filename;
	if (uTotal >= sizeof(m_Filename)) {
		// Allocate the new space
		pOutput = StringDuplicate(pInput, uExtensionLength + 4);
	} else {
		MemoryCopy(pOutput, pInput, uLength);
	}
	if (uExtensionLength) {
		pOutput[uLength] = '.';
		MemoryCopy(pOutput + uLength + 1, pExtension, uExtensionLength);
		uLength += uExtensionLength + 1;
	}
	pOutput[uLength] = ':';
	pOutput[uLength + 1] = 0;
	// Was the old string allocated?
	if (pInput != m_Filename) {
		Free(pInput);
	}
	// Store my new string
	m_pFilename = pOutput;
}

/*! ************************************

	\brief Extract the directory from a pathname

	Given a pathname, remove the filename from the end of the path, leaving
	only the directory name remaining.

***************************************/

Burger::eError BURGER_API Burger::Filename::DirName(void) BURGER_NOEXCEPT
{
	char* pFilename = m_pFilename;
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
		char* pLastColon = NULL;
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
			pLastColon[1] = 0;
		}
	}
	return kErrorNone;
}

/*! ************************************

	\brief Extract the directory from a pathname

	Given a pathname, remove the filename from the end of the path, leaving
	only the directory name remaining.
	\param pOutput Pointer to a valid Burger::String instance to receive the new
		string

***************************************/

Burger::eError BURGER_API Burger::Filename::DirName(
	String* pOutput) const BURGER_NOEXCEPT
{
	const char* pFilename = m_pFilename;
	// Get a character from the filename
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pFilename)[0];
	// Skip the first colon
	if (uTemp == ':') {
		uTemp = reinterpret_cast<const uint8_t*>(pFilename)[1];
		++pFilename;
	}
	const char* pLastColon = NULL;
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
	pLastColon = m_pFilename;
	// Truncate the string after the colon
	pOutput->Set(pLastColon, static_cast<uintptr_t>(pFilename - pLastColon));
	return kErrorNone;
}

/*! ************************************

	\brief Extract the base name from a pathname

	Given a pathname, remove the directory from the beginning of the path,
leaving only the file name remaining. The resulting filename will not have
	leading or trailing colons

	\param pOutput Pointer to a valid Burger::String instance to receive the new
string

***************************************/

void BURGER_API Burger::Filename::BaseName(
	String* pOutput) const BURGER_NOEXCEPT
{
	const char* pFilename = m_pFilename;
	// Get a character from the filename
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pFilename)[0];
	// Skip the first colon
	if (uTemp == ':') {
		uTemp = reinterpret_cast<const uint8_t*>(pFilename)[1];
		++pFilename;
	}
	const char* pLastColon = NULL;
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
		++pLastColon;
		if (pFilename[-1] == ':') {
			--pFilename;
		}
	} else {
		pLastColon = m_pFilename;
	}
	// Truncate the string after the colon
	pOutput->Set(pLastColon, static_cast<uintptr_t>(pFilename - pLastColon));
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
	\sa IsFilenameOnly()

***************************************/

uint_t BURGER_API Burger::Filename::IsFullPathname(void) const BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	const char* pFilename = m_pFilename;
	uint_t uTemp = reinterpret_cast<const uint8_t*>(pFilename)[0];
	if (uTemp == ':') {
		uResult = TRUE;
	} else if (uTemp == '.') {
		uint_t uTestChar2 = reinterpret_cast<const uint8_t*>(pFilename)[1];
		if ((uTestChar2 & 0xDF) == 'D') {
			// Validate the input to only allow the form of
			// ".D22:" where it starts with ".D", followed by ascii digits, then
			// terminated with a colon. Any other form is assumed to be a valid
			// filename

			// Must be longer than 2 characters.
			uintptr_t i = 2;
			do {
				uTestChar2 = reinterpret_cast<const uint8_t*>(pFilename)[i];
				// Find terminating colon
				if (uTestChar2 == ':') {
					// Is it the form ".D:", if not, it's valid
					if (i != 2) {
						// It's valid!!!
						uResult = TRUE;
					}
					break;
				}
				++i;
				// Only ASCII digits are allowed. If a non ASCII is detected,
				// assume filename
			} while (static_cast<uint_t>(uTestChar2 - '0') < 10);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Determine if a filename has no prefix and is not fully qualified

	There are two ways a pathname not qualified, firstly if
	the pathname is a full pathname, it's qualified. If the pathname
	has a prefix start, it's qualified.

	Filenames of this nature will be automatically assumed to be offset
	from directory Burger::FileManager::kPrefixCurrent (the current working
	directory)

	\return \ref TRUE if the pathname is not qualified, \ref FALSE if not.
	\sa IsFullPathname() const

***************************************/

uint_t BURGER_API Burger::Filename::IsFilenameOnly(void) const BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;
	if (!IsFullPathname()) {
		if (ParsePrefixNumber() == FileManager::kPrefixInvalid) {
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

	\sa IsFilenameOnly()

***************************************/

uint_t BURGER_API Burger::Filename::ParsePrefixNumber(
	void) const BURGER_NOEXCEPT
{
	uint_t uPrefixNum = FileManager::kPrefixInvalid;
	const char* pFilename = m_pFilename;
	uint_t uTestChar = reinterpret_cast<const uint8_t*>(pFilename)[0];
	if ((uTestChar >= '0') && (uTestChar < ('9' + 1))) {
		// Is it a number? If it's a valid prefix number followed by a colon
		// yank it out and set uPrefixNum to the value
		uTestChar -= '0'; // Convert to 0-9
		uintptr_t uIndex = 1;
		do {
			// I found an ending colon, accept it, maybe
			uint_t uTemp = reinterpret_cast<const uint8_t*>(pFilename)[uIndex];
			++uIndex;                   // Next value
			if (uTemp == ':') {         // End the prefix here?
				uPrefixNum = uTestChar; // This is the prefix I want
				break;                  // It's good!
			}
			uTemp -= '0';      // 0-9?
			if (uTemp >= 10) { // Valid digit?
				break; // It's a filename that starts with a number like 2Temp
			}
			uTestChar = uTestChar * 10;    // Shift up the previous number
			uTestChar = uTestChar + uTemp; // Total prefix number
		} while (uTestChar < FileManager::kPrefixCount); // Test for overflow
	} else if (pFilename[1] == ':') {
		// Check for the special case prefixes of "$:", "*:" and "@:"
		if (uTestChar == '$') { // System folder?
			uPrefixNum = FileManager::kPrefixSystem;
		} else if (uTestChar == '*') { // Boot volume?
			uPrefixNum = FileManager::kPrefixBoot;
		} else if (uTestChar == '@') { // Prefs folder?
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
volume name. If the path starts with ".D2:" then it is a full pathname starting
with a drive number. If the path starts with a "$:","*:" or "@:" then use
special prefix numbers 32-34 If the path starts with 0: through 31: then use
prefix 0-31. Otherwise prefix the pathname with the contents of prefix 8
("Default")

	If the path after the prefix is removed is a period then POP the number of
	directories from the pathname for each period present after the first.
	Example "..:PrevDir:File:" will go down one directory and up the directory
PrevDir

	All returned pathnames will have a trailing colon

	\param pInput Pointer to a pathname string

***************************************/

Burger::eError BURGER_API Burger::Filename::Expand(const char* pInput) BURGER_NOEXCEPT
{
	Clear();

	// The first thing to do is determine what prefix # this is

	uintptr_t uLength; // Length of the input
	// Prefix found (Or kPrefixCount if bogus)
	uint_t uPrefixNum = FileManager::kPrefixCurrent;
	// Empty input (Assume prefix 8)
	// Prevent a crash by using a bogus pathname
	if (!pInput) {
		uLength = 0;
	} else {
		uLength = StringLength(pInput);
		if (uLength) {
			uint_t uTestChar = reinterpret_cast<const uint8_t*>(pInput)[0];
			// Is it a fully qualified pathname?
			if (uTestChar == ':') {
				uPrefixNum = FileManager::kPrefixCount;
			} else if (uLength >= 2) {
				uint_t uTestChar2 = reinterpret_cast<const uint8_t*>(pInput)[1];
				// Is this a drive number?
				if ((uTestChar == '.') && ((uTestChar2 & 0xDF) == 'D')) {
					// Validate the input to only allow the form of
					// ".D22:" where it starts with ".D", followed by ascii
					// digits, then terminated with a colon. Any other form is
					// assumed to be a valid filename

					// Must be longer than 2 characters.
					uintptr_t i = 2;
					if (i < uLength) {
						do {
							uTestChar2 =
								reinterpret_cast<const uint8_t*>(pInput)[i];
							// Find terminating colon
							if (uTestChar2 == ':') {
								// Is it the form ".D:", if not, it's valid
								if (i != 2) {
									// It's valid!!!
									uPrefixNum = FileManager::kPrefixCount;
								}
								break;
							}
							// Only ASCII digits are allowed. If a non ASCII is
							// detected, assume filename
							if (static_cast<uint_t>(uTestChar2 - '0') >= 10) {
								break;
							}
						} while (++i < uLength);
					}
				} else if ((uTestChar >= '0') && (uTestChar < ('9' + 1))) {
					// Is it a number? If it's a valid prefix number followed by
					// a colon yank it out and set uPrefixNum to the value
					uTestChar -= '0'; // Convert to 0-9
					uintptr_t uIndex = 1;
					do {
						// I found an ending colon, accept it, maybe
						uint_t uTemp =
							reinterpret_cast<const uint8_t*>(pInput)[uIndex];
						++uIndex;              // Next value
						if (uTemp == ':') {    // End the prefix here?
							uLength -= uIndex; // Hack off the number and colon
							pInput += uIndex;
							uPrefixNum = uTestChar; // This is the prefix I want
							break;                  // It's good!
						}
						uTemp -= '0';      // 0-9?
						if (uTemp >= 10) { // Valid digit?
							break; // It's a filename that starts with a number
								   // like 2Temp
						}
						uTestChar =
							uTestChar * 10; // Shift up the previous number
						uTestChar = uTestChar + uTemp; // Total prefix number
						// If I ran out of text, bad prefix
						if (uLength == uIndex) {
							break;
						}
					} while (uTestChar <
						FileManager::kPrefixCount); // Test for overflow
				} else if (uTestChar2 == ':') {
					// Check for the special case prefixes of "$:", "*:" and
					// "@:"
					if (uTestChar == '$') { // System folder?
						uPrefixNum = FileManager::kPrefixSystem;
					} else if (uTestChar == '*') { // Boot volume?
						uPrefixNum = FileManager::kPrefixBoot;
					} else if (uTestChar == '@') { // Prefs folder?
						uPrefixNum = FileManager::kPrefixPrefs;
					}
					// If a special prefix was found, or ".:" which means
					// current directory
					if ((uTestChar == '.') ||
						(uPrefixNum != FileManager::kPrefixCurrent)) {
						uLength -= 2;
						pInput += 2;
					}
				}
			}
		}
	}

	// Now I have a prefix number, let's prepend the prefix to the path
	// Requires uPrefixNum to be a valid prefix number

	Filename Prefix;
	const char* pPrefix = NULL; // Set to NULL to quiet the compiler
	uintptr_t uPrefixLen =
		0; // Assume only a zero byte ("C" end byte) (Empty prefix)
	if (uPrefixNum < FileManager::kPrefixCount) {
		FileManager::GetPrefix(
			&Prefix, uPrefixNum); // Is there a prefix attached?
		pPrefix = Prefix.GetPtr();
		if (pPrefix[0]) { // Nope...
			uPrefixLen =
				StringLength(pPrefix); // How many bytes for the prefix?

			// If the filename starts with a period, then remove it and pop the
			// prefix for each following period Note : I assume that a NULL
			// pPrefix is also joined by Index=0

			if (uLength >= 2) {
				uint_t uTestChar = reinterpret_cast<const uint8_t*>(pInput)[0];
				if (uTestChar == '.') {
					// Lets count the periods before a delimiting colon.
					// Only the form of "..:..:" is valid. "...Foo" is
					// considered a filename
					uintptr_t uPeriodCount = 0;
					uintptr_t uIndex = 1;
					do {
						uTestChar =
							reinterpret_cast<const uint8_t*>(pInput)[uIndex];
						++uIndex;
						if (uTestChar == '.') {
							continue;
						}
						// Not a ..: ?
						if (uTestChar != ':') {
							break;
						}
						// Accept the input
						pInput += uIndex;
						uLength -= uIndex;
						uPeriodCount += (uIndex -
							2); // Ignore the first period and the colon
						if (uLength < 2) {
							break;
						}
						// If not a period, the test below will kick me out.
						uTestChar = reinterpret_cast<const uint8_t*>(pInput)[0];
						uIndex = 1;
						if (uTestChar != '.') { // Starts with a period
							break;
						}
					} while (uIndex < uLength);

					if (uPeriodCount) {
						uintptr_t uPrefixLenCache = uPrefixLen;
						do {
							// Popped the entire prefix?
							if (!uPrefixLen) {
								// Use the root prefix
								uPrefixLen = uPrefixLenCache;
								break;
							}
							--uPrefixLen; // Index to the last char
							const uint8_t* pWork =
								reinterpret_cast<const uint8_t*>(pPrefix);
							do {
								--uPrefixLen; // Go to previous char
								if (pWork[uPrefixLen] == ':') {
									// Is this the root prefix?
									if (uPrefixLen) {
										uPrefixLenCache = uPrefixLen + 1;
									}
									break;
								}
							} while (
								(uPrefixLen != static_cast<uintptr_t>(-1)));
							++uPrefixLen;         // Grab the final colon
						} while (--uPeriodCount); // Discard the period

						// Edge case. If all the prefixes were reduced to
						// nothing but a single colon, keep the volume
						// name/number
						if (uPrefixLen < 2) {
							uPrefixLen = 0; // In case the prefix has no colons
							uintptr_t uTestIndex = 0;
							if (pPrefix[0] ==
								':') { // Skip the volume name colon
								uTestIndex = 1;
							}
							do {
								if (pPrefix[uTestIndex] == ':') {
									uPrefixLen = uTestIndex + 1;
									break;
								}
								++uTestIndex;
							} while (pPrefix[uTestIndex]);
						}
					}
				}
			}
		}
	}

	// At this point uLength == length of the filename string to append
	// uPrefixLen = length of the prefix

	char* pOutput = m_Filename;
	uintptr_t uTotal = uPrefixLen + uLength;
	if (uTotal >= (sizeof(m_Filename) - 2)) {
		pOutput = static_cast<char*>(Alloc(uTotal + 2));
		if (!pOutput) { // No memory? Return nothing
			uPrefixLen = 0;
			uLength = 0;
			uTotal = 0;
			pOutput = m_Filename;
		} else {
			m_pFilename = pOutput;
		}
	}

	// The buffer is allocated, store the resulting pathname

	if (uPrefixLen) {                             // Any ASCII?
		MemoryCopy(pOutput, pPrefix, uPrefixLen); // Copy the prefix
	}
	if (uLength) { // Any text for the path?
		MemoryCopy(&pOutput[uPrefixLen], pInput,
			uLength); // Copy the path AFTER the prefix
	}

	if (uTotal) { // Is there any text?
		if (reinterpret_cast<const uint8_t*>(pOutput)[uTotal - 1] !=
			':') {                 // Trailing colon?
			pOutput[uTotal] = ':'; // End the path with a colon
			++uTotal;              // +1 to the length
		}
	}
	pOutput[uTotal] = 0; // Add the terminating zero
	return kErrorNone;
}

/*! ************************************

	\brief Expand a filename by using prefix mapping.

	Using the rules for a Burgerlib type pathname, expand a path
	into a FULL pathname native to the Burgerlib file system.

	Directory delimiters are colons only.
	If the path starts with a colon, then it is a full pathname starting with a
volume name. If the path starts with ".D2:" then it is a full pathname starting
with a drive number. If the path starts with a "$:","*:" or "@:" then use
special prefix numbers 32-34 If the path starts with 0: through 31: then use
prefix 0-31. Otherwise prefix the pathname with the contents of prefix 8
("Default")

	If the path after the prefix is removed is a period then POP the number of
	directories from the pathname for each period present after the first.
	Example "..:PrevDir:File:" will go down one directory and up the directory
PrevDir

	All returned pathnames will have a trailing colon

***************************************/

Burger::eError BURGER_API Burger::Filename::Expand(void) BURGER_NOEXCEPT
{
	// Temp buffer
	char Buffer[sizeof(m_Filename)];
	// Get the string pointer
	const char* pInput = m_pFilename;
	// Was it an allocated buffer?
	if (pInput != m_Filename) {
		// Take ownership
		m_pFilename = m_Filename;
	} else {
		// Make a copy of the string and use the local copy
		StringCopy(Buffer, sizeof(Buffer), pInput);
		pInput = Buffer;
	}
	// Perform the expand operation using a safe input buffer
	const eError uResult = Expand(pInput);
	// If the string was allocated, release it
	if (pInput != Buffer) {
		Free(pInput);
	}
	return uResult;
}

/*! ************************************

	\brief Expand a filename from the BurgerLib format to the native OS format..

	For generic code, convert a BurgerLib path into one suitable for the
	current operating system. This function is mostly used when custom code
	for a specific platform is being written and the native path is required.

	\note If this function is invoked on a MacOS target, a Volume Reference
number and Directory ID are generated and can be accessed via calls to
Burger::Filename::GetDirID(void) const and Burger::Filename::GetVRefNum(void)
const

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA)) || \
	defined(DOXYGEN)
const char* BURGER_API Burger::Filename::GetNative(void) BURGER_NOEXCEPT
{
	return m_pFilename;
}
#endif

/*! ************************************

	\brief Set the filename to the current working directory

	Query the operating system for the current working directory and set the
	filename to that directory. The path is converted into UTF8 character
	encoding and stored in Burgerlib filename format

	On platforms where a current working directory doesn't make sense, like a
	ROM based system, the filename is cleared out.

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_XBOX360) || defined(BURGER_MACOS) || \
	defined(BURGER_LINUX) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)

Burger::eError BURGER_API Burger::Filename::SetSystemWorkingDirectory(
	void) BURGER_NOEXCEPT
{
	Clear();
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
	defined(BURGER_XBOX360) || defined(BURGER_MACOS) || \
	defined(BURGER_LINUX) || defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::SetApplicationDirectory(
	void) BURGER_NOEXCEPT
{
	Clear();
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
	defined(BURGER_XBOX360)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::SetBootVolumeDirectory(
	void) BURGER_NOEXCEPT
{
	Clear();
	return kErrorNotSupportedOnThisPlatform;
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
	defined(BURGER_XBOX360) || defined(BURGER_MACOS)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::SetMachinePrefsDirectory(
	void) BURGER_NOEXCEPT
{
	Clear();
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
	defined(BURGER_XBOX360) || defined(BURGER_MACOS)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::SetUserPrefsDirectory(
	void) BURGER_NOEXCEPT
{
	Clear();
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

	\sa SetFromNative(const uint16_t *)

***************************************/

#if !(defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || \
	defined(BURGER_MACOS) || defined(BURGER_IOS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA)) || \
	defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::SetFromNative(
	const char* pInput) BURGER_NOEXCEPT
{
	Set(pInput);
	return kErrorNotSupportedOnThisPlatform;
}
#endif

/*! ************************************

	\brief Expand a UTF-16 filename from the native format to Burgerlib.

	Copy the native pathname string into the internal native pathname buffer
	after converting the string to UTF-8.

	\param pInput Pointer to a pathname string

	\note This function does not exist on MacOS classic

	\sa SetFromNative(const char *) or SetFromNative(const char *,long,short)

***************************************/

#if !defined(BURGER_MAC) || defined(DOXYGEN)
Burger::eError BURGER_API Burger::Filename::SetFromNative(
	const uint16_t* pInput) BURGER_NOEXCEPT
{
	// Convert to UTF-8
	String Temp(pInput);
	// Set the path
	return SetFromNative(Temp.c_str());
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
