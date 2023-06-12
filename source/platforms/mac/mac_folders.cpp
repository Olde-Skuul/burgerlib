/***************************************

	MacOS specific directory functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "mac_folders.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include <Files.h>

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === ioACUser flag accessors ===
#endif

/*! ************************************

	\brief Does the logged in use have ownership of this directory?

	The directory is owned by the currently logged in user, return \ref TRUE.

	\maconly

	\param ioACUser ioACUser value from a valid DirInfo structure.

	\return \ref TRUE if the directory is owned by the user, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::is_user_owner(int8_t ioACUser) BURGER_NOEXCEPT
{
	return !(ioACUser & kioACUserNotOwnerMask);
}

/*! ************************************

	\brief Does the logged in use have full access to this directory?

	If the user has full access to this directory, return \ref TRUE.

	\maconly

	\param ioACUser ioACUser value from a valid DirInfo structure.

	\return \ref TRUE if user has full directory access, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::user_has_full_access(
	int8_t ioACUser) BURGER_NOEXCEPT
{
	return !(ioACUser &
		(kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
			kioACUserNoMakeChangesMask));
}

/*! ************************************

	\brief Is the directory a drop box?

	Drop box folders allow operations like copying into the directory. But it
	doesn't allow viewing of the files and folders within. Check the directory
	attributes if the directory is a drop box. Return \ref TRUE if so.

	\maconly

	\param ioACUser ioACUser value from a valid DirInfo structure.

	\return \ref TRUE if the directory is a drop box, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::user_has_drop_box_access(
	int8_t ioACUser) BURGER_NOEXCEPT
{
	return (ioACUser &
			   (kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
				   kioACUserNoMakeChangesMask)) ==
		(kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask);
}

/*! ************************************

	\brief Can the contents of the directory be changed?

	If the user has access to make changes in the directory, return \ref TRUE.

	\maconly

	\param ioACUser ioACUser value from a valid DirInfo structure.

	\return \ref TRUE if the user has modification access to a directory,
		\ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::user_has_change_access(
	int8_t ioACUser) BURGER_NOEXCEPT
{
	return (ioACUser &
			   (kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
				   kioACUserNoMakeChangesMask)) == kioACUserNoMakeChangesMask;
}

/*! ************************************

	\brief Is the user denied access to this directory?

	If the user has no access to this directory, return \ref TRUE.

	\maconly

	\param ioACUser ioACUser value from a valid DirInfo structure.

	\return \ref TRUE if the user has no access to a directory,
		\ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::user_has_no_access(
	int8_t ioACUser) BURGER_NOEXCEPT
{
	return (ioACUser &
			   (kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
				   kioACUserNoMakeChangesMask)) ==
		(kioACUserNoSeeFolderMask | kioACUserNoSeeFilesMask |
			kioACUserNoMakeChangesMask);
}

#endif