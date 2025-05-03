/***************************************

	FileManager Class: Darwin

	Copyright (c) 2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfilemanager.h"

#if defined(BURGER_DARWIN) || defined(DOXYGEN)
#include "brglobalmemorymanager.h"
#include "brmemoryfunctions.h"
#include "brstringfunctions.h"

#include <sys/attr.h>
#include <unistd.h>

/***************************************

	\brief Handle platform specific startup code

	Calls system functions to determine the version, state and several platform
	specific variables to allow the FileManager to run better by pre-caching
	relevant data.

	For MS-DOS, it will determine the version and flavor of MS/DOS this
	application is running. It will also detect DosBox.

	For Linux, it will scan all the mounted volumes for quick access to shared
	volumes.

	For Android and consoles, it will check if there are mounted SD Cards or
	other external data storage devices so the application can be aware of them.

	\sa platform_shutdown() or \ref FileManager

***************************************/

void BURGER_API Burger::FileManager::platform_setup(void) BURGER_NOEXCEPT
{
	Filename MyFilename;

	// Get the boot volume name
	eError uResult = get_volume_name(&MyFilename, 0);
	if (!uResult) {
		// Set the initial prefix
		const char* pBootName = MyFilename.c_str();
		m_uBootNameSize = string_length(pBootName);
		m_pBootName = StringDuplicate(pBootName);
	}
}

/***************************************

	\brief Handle platform specific shutdown code

	Calls system functions to release cached values obtained from the platform's
	operating system.

	\sa platform_setup() or \ref FileManager

***************************************/

void BURGER_API Burger::FileManager::platform_shutdown(void) BURGER_NOEXCEPT
{
	free_memory(m_pBootName);
	m_pBootName = nullptr;
	m_uBootNameSize = 0;
}

/***************************************

	\brief Return a file's type code using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will retrieve the 4 byte code
	from the file.

	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing.

	\param pFileName Pointer to a \ref Filename of a native OS pathname

	\return 0 if the file doesn't exist, the function isn't implemented of if
		the file doesn't have extended information. A 32 bit value if the data
		can be retrieved.

	\sa get_file_type(const char*), get_creator_type(const char*) or
		get_creator_and_file_type(const char*, uint32_t*, uint32_t*)

***************************************/

uint32_t BURGER_API Burger::FileManager::get_creator_type(Filename* pFileName)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};

	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	memory_clear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->get_native(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	uint_t uResult;
	if (iError < 0) {
		uResult = 0;
	} else {
		// It's parsed!
		uResult = reinterpret_cast<const uint32_t*>(Entry)[2U];
	}
	return uResult;
}

/***************************************

	\brief Return a file's creator code using a native filename.

	On MacOS, a file has extended data which includes the application code that
	owns the file. This function will retrieve the 4 byte code from the file.

	On non MacOS platforms, this function only return 0 (An error condition) and
	perform nothing..

	\param pFileName Pointer to a \ref Filename of a native OS pathname

	\return 0 if the file doesn't exist, the function isn't implemented of if
		the file doesn't have extended information. A 32 bit value if the data
		can be retrieved.

	\sa get_creator_type(const char*), get_file_type(const char*) or
		get_creator_and_file_type(const char*, uint32_t*, uint32_t*)

***************************************/

uint32_t BURGER_API Burger::FileManager::get_file_type(Filename* pFileName)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};

	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	memory_clear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->get_native(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	uint_t uResult;
	if (iError < 0) {
		uResult = 0;
	} else {
		// It's parsed!
		uResult = reinterpret_cast<const uint32_t*>(Entry)[1];
	}
	return uResult;
}

/***************************************

	\brief Return a file's type and creator codes using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename, and the creator code of the application that
	owns this file. This function will retrieve the 4 byte codes from the file.

	On non MacOS platforms, this function only return non-zero (An error
	condition) and perform store zeros in the result values..

	\param pFileName Pointer to a \ref Filename of a native OS pathname
	\param pCreatorType Pointer to a uint32_t that will receive the file creator
		code.
	\param pFileType Pointer to a uint32_t that will receive the file type code.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero is returned on
		successful completion.

	\sa get_creator_and_file_type(Filename*, uint32_t*, uint32_t*),
		get_file_type(const char*) or get_creator_type(const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::get_creator_and_file_type(
	Filename* pFileName, uint32_t* pCreatorType, uint32_t* pFileType)
{
	*pFileType = 0;
	*pCreatorType = 0;

	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};

	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	memory_clear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->get_native(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	eError uResult;
	if (iError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		pFileType[0] = reinterpret_cast<const uint32_t*>(Entry)[1];
		pCreatorType[0] = reinterpret_cast<const uint32_t*>(Entry)[2];
		uResult = kErrorNone;
	}
	return uResult;
}

/***************************************

	\brief Set a file's type code using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will set the 4 byte code for the
	file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a \ref Filename of a native OS pathname
	\param uCreatorType Four byte character code to set.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero if successful.

	\sa set_file_type(const char*, uint32_t), set_creator_type(
		const char*, uint32_t) or set_creator_and_file_type(
		const char*, uint32_t, uint32_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::set_creator_type(
	Filename* pFileName, uint32_t uCreatorType)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};
	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	memory_clear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->get_native(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	eError uResult;
	if (iError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		reinterpret_cast<uint32_t*>(Entry)[2] = uCreatorType;
		iError = setattrlist(pFileName->get_native(), &AttributesList,
			Entry + sizeof(uint32_t), 32, 0);
		if (iError < 0) {
			uResult = kErrorIO;
		} else {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	\brief Set a file's type code using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename. This function will set the 4 byte code for the
	file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a \ref Filename of a native OS pathname,
	\param uFileType Four byte character code to set.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero if successful.

	\sa set_file_type(const char*, uint32_t), set_creator_type(
		const char *,uint32_t) or set_creator_and_file_type(
		const char*, uint32_t, uint32_t)

***************************************/

Burger::eError BURGER_API Burger::FileManager::set_file_type(
	Filename* pFileName, uint32_t uFileType)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};
	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	memory_clear(&AttributesList, sizeof(AttributesList));
	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->get_native(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	eError uResult;
	if (iError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		reinterpret_cast<uint32_t*>(Entry)[1] = uFileType;
		iError = setattrlist(pFileName->get_native(), &AttributesList,
			Entry + sizeof(uint32_t), 32, 0);
		if (iError < 0) {
			uResult = kErrorIO;
		} else {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	\brief Set a file's type and creator codes using a native filename.

	On MacOS, a file has extended data which includes the file's type, which is
	independent of the filename, and the creator code of the application that
	owns this file. This function will set the 4 byte codes for the file.

	On non MacOS platforms, this function will perform nothing.

	\param pFileName Pointer to a \ref Filename of a native OS pathname
	\param uCreatorType A uint32_t of the new file creator code.
	\param uFileType A uint32_t of the new file type code.

	\return Non-zero if the file doesn't exist, the function isn't implemented
		of if the file doesn't have extended information. Zero is returned on
		successful completion.

	\sa get_creator_and_file_type(const char*, uint32_t*, uint32_t*),
		get_file_type(const char*) or get_creator_type(const char*)

***************************************/

Burger::eError BURGER_API Burger::FileManager::set_creator_and_file_type(
	Filename* pFileName, uint32_t uCreatorType, uint32_t uFileType)
{
	// Structure declaration of data coming from getdirentriesattr()
	struct FInfoAttrBuf {
		uint32_t m_uLength;  // Length of this data structure
		char finderInfo[32]; // Aux/File type are the first 8 bytes
	};

	// Buffer to hold the attributes and the filename
	uint8_t Entry[sizeof(FInfoAttrBuf)];

	// Attributes requested
	attrlist AttributesList;

	// Initialize the attributes list
	memory_clear(&AttributesList, sizeof(AttributesList));

	// "sizeof" for the structure
	AttributesList.bitmapcount = ATTR_BIT_MAP_COUNT;

	// Note: If these flags are changed, the FInfoAttrBuf MUST be
	// adjusted to reflect the request or weird stuff will happen
	AttributesList.commonattr = ATTR_CMN_FNDRINFO;

	// Get the directory entry
	int iError = getattrlist(
		pFileName->get_native(), &AttributesList, Entry, sizeof(Entry), 0);

	// No errors?

	eError uResult;
	if (iError < 0) {
		uResult = kErrorFileNotFound;
	} else {
		// It's parsed!
		reinterpret_cast<uint32_t*>(Entry)[1] = uFileType;
		reinterpret_cast<uint32_t*>(Entry)[2] = uCreatorType;
		iError = setattrlist(pFileName->get_native(), &AttributesList,
			Entry + sizeof(uint32_t), 32, 0);
		if (iError < 0) {
			uResult = kErrorIO;
		} else {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/*! ************************************

	\fn Burger::FileManager::get_boot_name()
	\brief Return the boot volume name

	The boot volume is a UTF8 "C" string in the format of ":Foobar" (A leading
	colon and no ending colon)

	\note Available only on MacOSX

	\return const char * to the boot volume name.
	\sa Burger::FileManager::get_boot_name_length()

***************************************/

/*! ************************************

	\fn Burger::FileManager::get_boot_name_length()
	\brief Return the length of the boot volume name

	The boot volume is a UTF8 "C" string in the format of ":Foobar" (A leading
	colon and no ending colon).

	\note Available only on MacOSX

	\return Number of bytes in length of the boot volume name
	\sa Burger::FileManager::get_boot_name()

***************************************/

#endif
