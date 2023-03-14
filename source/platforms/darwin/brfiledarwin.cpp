/***************************************

	Darwin version of the File class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"

#if defined(BURGER_DARWIN) || defined(DOXYGEN)
#include "brfilemanager.h"

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
	return FileManager::set_creator_type(get_filename(), uCreatorType);
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
	return FileManager::set_file_type(get_filename(), uFileType);
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
	return FileManager::get_creator_type(get_filename());
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
	return FileManager::get_file_type(get_filename());
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
	return FileManager::set_creator_and_file_type(
		get_filename(), uCreatorType, uFileType);
}

#endif
