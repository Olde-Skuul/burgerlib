/***************************************

	MacOS GetVolParmsInfoBuffer handlers

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "mac_volparms.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include <Files.h>

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === GetVolParmsInfoBuffer accessors ===
#endif

/*! ************************************

	\brief Return the version of a GetVolParmsInfoBuffer

	Depending on the version of MacOS the application is running, and the device
	driver found for the mounted drive, it will support levels of information.
	This accessor will return the version of the GetVolParmsInfoBuffer buffer so
	the application will know what levels of data are present.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Version of the GetVolParmsInfoBuffer (0 through 3)

***************************************/

short BURGER_API Burger::MacOS::get_version(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	// Get the 16 bit signed version
	return pVolParamsInfoBuffer->vMVersion;
}

/*! ************************************

	\brief Return the volume attributes of a GetVolParmsInfoBuffer

	Obtain the volume attribute bits.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Volume attribute bits.

***************************************/

long BURGER_API Burger::MacOS::get_attributes(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	// Get the 32 bit field
	return pVolParamsInfoBuffer->vMAttrib;
}

/*! ************************************

	\brief Return the volume Handle

	Obtain the Handle of the volume driver data. USE AT YOUR OWN RISK!

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Volume driver local data Handle.

***************************************/

Handle BURGER_API Burger::MacOS::get_local_handle(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return pVolParamsInfoBuffer->vMLocalHand;
}

/*! ************************************

	\brief Return the server IPv4 address

	If the volume is a mounted network server, the IPv4 address can be obtained
	from this function. If the value is zero, then this is a locally mounted
	block device. Firewire, Floppy, USB and IDE drives will return zero.

	\note If it's an AppleTalk volume, it may also be an AppleTalk address

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return IPv4 address of network shared volume, or zero.

***************************************/

uint32_t BURGER_API Burger::MacOS::get_server_address(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	// Convert the IP address to unsigned
	return static_cast<uint32_t>(pVolParamsInfoBuffer->vMServerAdr);
}

/*! ************************************

	\brief Return the volume speed grade.

	If the speed class of a volume is known, return that value. Higher is
	better. If the speed is unknown or if the version is less than 2, return 0.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Speed grade of volume, or zero if unknown.

***************************************/

long BURGER_API Burger::MacOS::get_volume_grade(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMVersion < 2) ?
		0 :
		pVolParamsInfoBuffer->vMVolumeGrade;
}

/*! ************************************

	\brief Return the foreign privilege model

	Volumes may have additional privileges models, this value is 0 for HFS
	volumes which may or may not support AFP, fsUnixPriv if it supports A/UX. If
	the version is less than 2, return 0.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Volume privilege ID, or zero if unknown.

***************************************/

short BURGER_API Burger::MacOS::get_foreign_PrivID(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMVersion < 2) ?
		static_cast<short>(0) :
		pVolParamsInfoBuffer->vMForeignPrivID;
}

/*! ************************************

	\brief Return the extended volume attributes.

	Most volumes support extended attributes that denote if the volume can be
	ejected, max size, and other details. If the GetVolParmsInfoBuffer record is
	less than version 3, this function will return 0.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Volume extended attributes or 0 if not supported.

***************************************/

long BURGER_API Burger::MacOS::get_extended_attributes(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMVersion < 3) ?
		0 :
		pVolParamsInfoBuffer->vMExtendedAttributes;
}

/*! ************************************

	\brief Return the device ID

	Device name for use with IOKit. If the GetVolParmsInfoBuffer record is
	less than version 4, this function will return \ref nullptr.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return Device ID or \ref nullptr if not supported.

***************************************/

void* BURGER_API Burger::MacOS::get_device_ID(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMVersion < 4) ?
		nullptr :
		pVolParamsInfoBuffer->vMDeviceID;
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === GetVolParmsInfoBuffer flag and value testers ===
#endif

/*! ************************************

	\brief Is the volume a network drive?

	Test if the volume is a shared drive accessed by a network.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if it's a remote drive, \ref FALSE if local.

***************************************/

uint_t BURGER_API Burger::MacOS::is_network_volume(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	// Test the IPv4 address
	return pVolParamsInfoBuffer->vMServerAdr != 0;
}

/*! ************************************

	\brief Is the File Control Blocks limited to 8?

	Test if the volume uses only 8 control blocks instead of 16.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if it limits FCBs, \ref FALSE if local.

***************************************/

uint_t BURGER_API Burger::MacOS::has_limit_FCBs(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bLimitFCBs)) != 0;
}

/*! ************************************

	\brief Does the Finder uses the shared volume handle.

	Test if the Finder uses the shared volume handle for its local window list.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the Finder uses the handle, \ref FALSE if local.

***************************************/

uint_t BURGER_API Burger::MacOS::has_local_WList(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bLocalWList)) != 0;
}

/*! ************************************

	\brief Is the mini finder disabled?

	Burgerlib requires a minimum of MacOS 7.5 or higher, so this will always
	return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the mini Finder is disabled, \ref FALSE if available.

***************************************/

uint_t BURGER_API Burger::MacOS::has_no_mini_finder(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoMiniFndr)) != 0;
}

/*! ************************************

	\brief Is the volume name locked?

	Test if the volume name can be changed. Locked and network volumes cannot be
	altered, so they will return \ref TRUE. Local drives almost always return
	\ref FALSE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume name is immutable, \ref FALSE if is can be
		changed.

***************************************/

uint_t BURGER_API Burger::MacOS::has_no_volume_name_edit(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoVNEdit)) != 0;
}

/*! ************************************

	\brief Is the modification time controlled by the volume?

	Test if the volume modification times is managed by the volume. Network
	drives tend to manage the file creation/modification times, so if this
	returns \ref TRUE, all requests to alter the time codes will fail.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume manages time codes, \ref FALSE if the codes
		can be locally changed.

***************************************/

uint_t BURGER_API Burger::MacOS::has_no_local_sync(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoLclSync)) != 0;
}

/*! ************************************

	\brief Does the volume zoom to the trash?

	If this volume is unmounted, should the Finder show a "zoom" to the trash?

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume should make a visual effect, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_trash_offline(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bTrshOffLine)) != 0;
}

/*! ************************************

	\brief Don't switch to launching applications on this volume?

	If the volume forbids launching applications from it, this accessor will
	return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if applications shouldn't launch from this volume,
		\ref FALSE if permitted.

***************************************/

uint_t BURGER_API Burger::MacOS::has_no_switch_to(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoSwitchTo)) != 0;
}

/*! ************************************

	\brief Forbid desktop items on this volume?

	If the volume forbids placing items on the desktop, this accessor will
	return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the desktop folder is not supported on this volume,
		\ref FALSE if permitted.

***************************************/

uint_t BURGER_API Burger::MacOS::has_no_desk_items(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoDeskItems)) != 0;
}

/*! ************************************

	\brief Is booting disabled on this volume?

	If the volume does not have boot blocks, return \ref TRUE. Boot blocks are
	required to boot into classic MacOS. The Control Panel will dim the icon for
	this drive for startup if this bit is set.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume cannot be booted, \ref FALSE if bootable.

***************************************/

uint_t BURGER_API Burger::MacOS::has_no_boot_blocks(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoBootBlks)) != 0;
}

/*! ************************************

	\brief Is AppleTalk File Protocol access interfaces supported?

	If the volume supports AppleTalk File Protocol, this accessor will return
	\ref TRUE. Calls to ``PBHGetLoginInfo``, ``PBHGetDirAccess``,
	``PBHSetDirAccess``,``PBHMapID``, and ``PBHMapName`` will succeed.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supported AFP, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_access_control(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bAccessCntl)) != 0;
}

/*! ************************************

	\brief Is the volume missing a System directory?

	This volume doesn't support a System directory. Do not switch launch to this
	volume if \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume don't support a System directory, \ref FALSE
		if it does.

***************************************/

uint_t BURGER_API Burger::MacOS::has_no_system_directory(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bNoSysDir)) != 0;
}

/*! ************************************

	\brief Is the volume using an external file system?

	This volume is managed remotely or within a device and not from the local
	file manager if this returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume file system is controlled by an external
		device, \ref FALSE if it's managed locally.

***************************************/

uint_t BURGER_API Burger::MacOS::has_external_file_system_volume(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasExtFSVol)) != 0;
}

/*! ************************************

	\brief Does the volume support PBHOpenDeny?

	Does the volume support the calls ``PBHOpenDeny`` and ``PBHOpenRFDeny``? If
	so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports PBHOpenDeny, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_open_deny(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasOpenDeny)) != 0;
}

/*! ************************************

	\brief Does the volume support PBHCopyFile?

	Does the volume support the call ``PBHCopyFile``? If so, it returns
	\ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports PBHCopyFile, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_copy_file(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasCopyFile)) != 0;
}

/*! ************************************

	\brief Does the volume support PBHMoveRename?

	Does the volume support the call ``PBHMoveRename``? If so, it returns
	\ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports PBHMoveRename, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_move_rename(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasMoveRename)) != 0;
}

/*! ************************************

	\brief Does the volume support desktop functions?

	Does the volume support all of the desktop functions for the Desktop
	Manager? If so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports Desktop Manager, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_desktop_manager(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasDesktopMgr)) != 0;
}

/*! ************************************

	\brief Does the volume support AFP short names?

	Does the volume support AppleTalk File Protocol short names? If so, it
	returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports AFP short names, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_short_name(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasShortName)) != 0;
}

/*! ************************************

	\brief Does the volume support locked folders?

	Does the volume support attribute bits on folders so they can be locked? If
	so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports folder locking, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_folder_lock(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasFolderLock)) != 0;
}

/*! ************************************

	\brief Is local file sharing enabled on this volume?

	If this volume is being shared, return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if file sharing is enabled on this volume, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_personal_access(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib &
			   (1L << bHasPersonalAccessPrivileges)) != 0;
}

/*! ************************************

	\brief Does this volume support "Users and Groups"?

	If this volume is either on MacOS X, or on a network volume that supports
	user level privilege access, return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume support user level access \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_user_group_list(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasUserGroupList)) != 0;
}

/*! ************************************

	\brief Does the volume support PBCatSearch?

	Does the volume support the call ``PBCatSearch``? If so, it returns
	\ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports PBCatSearch, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_cat_search(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasCatSearch)) != 0;
}

/*! ************************************

	\brief Does the volume support PBExchangeFiles?

	Does the volume support the call ``PBExchangeFiles`` and file ID functions?
	If so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports PBExchangeFiles, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_file_IDs(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasFileIDs)) != 0;
}

/*! ************************************

	\brief Does the volume use BTrees?

	Does the volume use BTrees? This is \ref TRUE on local volume formatted with
	HFS. If the volume is formatted with a different file system, this will
	return \ref FALSE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume uses BTrees, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_BTree_manager(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bHasBTreeMgr)) != 0;
}

/*! ************************************

	\brief Can folders inherit privileges?

	Does the volume support privilege inheritance? If so, return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume allows access privilege inheritance,
		\ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_blank_access(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib &
			   (1L << bHasBlankAccessPrivileges)) != 0;
}

/*! ************************************

	\brief Can async file operations be performed on this volume?

	Does the volume support asynchronous file operations properly? Return
	\ref TRUE if true.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports asynchronous file operations,
		\ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_support_async_requests(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib & (1L << bSupportsAsyncRequests)) !=
		0;
}

/*! ************************************

	\brief Does the volume use a Trash cache?

	Mac OS X doesn't support Trash attributes and simulates the behavior by
	using a hidden folder. If this is being used, return \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume uses a trash cache, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_trash_volume_cache(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (pVolParamsInfoBuffer->vMAttrib &
			   (1L << bSupportsTrashVolumeCache)) != 0;
}

#if defined(BURGER_METROWERKS)
#pragma mark -
#pragma mark === Volume extended attribute accessors ===
#endif

/*! ************************************

	\brief Can the volume be ejected?

	If the volume can be unmounted, return \ref TRUE. This is especially true
	for USB and CD/DVD/BluRay drives.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume can be ejected, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_ejectable(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (get_extended_attributes(pVolParamsInfoBuffer) &
			   (1L << bIsEjectable)) != 0;
}

/*! ************************************

	\brief Does the volume driver directly support HFS+?

	If the volume driver supports the HFS+ API, return \ref TRUE. Otherwise the
	Mac File Manager will emulate them. If a volume supports the HFS+ APIs
	doesn't mean it's formatted with HFS+.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume support HFS+ APIs, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_HFSPlus_APIs(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (get_extended_attributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsHFSPlusAPIs)) != 0;
}

/*! ************************************

	\brief Does the volume support FSCatalogSearch?

	Does the volume support the call ``FSCatalogSearch`` and file ID functions?
	If so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports FSCatalogSearch, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_FSCatalogSearch(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (get_extended_attributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsFSCatalogSearch)) != 0;
}

/*! ************************************

	\brief Does the volume support FSExchangeObjects?

	Does the volume support the call ``FSExchangeObjects`` and file ID
	functions? If so, it returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports FSExchangeObjects, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_FSExchangeObjects(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (get_extended_attributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsFSExchangeObjects)) != 0;
}

/*! ************************************

	\brief Does the volume support 2 terabyte files?

	Does the volume support files as large as 2 terabytes? Return \ref TRUE if
	true.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports 2 TB files, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_2TB_file_support(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (get_extended_attributes(pVolParamsInfoBuffer) &
			   (1L << bSupports2TBFiles)) != 0;
}

/*! ************************************

	\brief Does the volume support filenames longer than 31 characters?

	Does the volume support filenames longer than 31 characters? Return
	\ref TRUE if true.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports long filenames, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_long_filenames(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (get_extended_attributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsLongNames)) != 0;
}

/*! ************************************

	\brief Does the volume support Unicode?

	Does the volume support Unicode so it can handle multiple languages? Return
	\ref TRUE if true.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports Unicode, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_multiple_script_names(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (get_extended_attributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsMultiScriptNames)) != 0;
}

/*! ************************************

	\brief Does the volume support named forks?

	The mac uses a file system with two forks, a data fork and a resource fork.
	If the volume supports forks beyond that, they have names. The default is
	none for the data fork, and "RESOURCE_FORK" for the resource fork. If
	alternate forks are supported, this accessor will return \ref TRUE.

	Does the volume support Unicode so it can handle multiple languages? Return
	\ref TRUE if true.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports more than two forks, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_named_forks(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (get_extended_attributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsNamedForks)) != 0;
}

/*! ************************************

	\brief Does the volume support recursive iterators?

	The volume supports recursive iterators, not at the volume root if this
	returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports recursive iterators, \ref FALSE if
		not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_subtree_iterators(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (get_extended_attributes(pVolParamsInfoBuffer) &
			   (1L << bSupportsSubtreeIterators)) != 0;
}

/*! ************************************

	\brief Does the volume support Lg2Phys SPI correctly.

	The volume supports Lg2Phys correctly if this returns \ref TRUE.

	\maconly

	\param pVolParamsInfoBuffer Pointer to a valid GetVolParmsInfoBuffer

	\return \ref TRUE if the volume supports Lg2Phys, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::has_large_to_physical_blocks(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT
{
	return (get_extended_attributes(pVolParamsInfoBuffer) &
			   (1L << bL2PCanMapFileBlocks)) != 0;
}

/*! ************************************

	\brief Obtain information from a drive volume.

	Call PBHGetVolParmsSync() and fill in a supplied GetVolParmsInfoBuffer
	record. Pass in a pointer to a uint32_t that has the size, in bytes, of the
	GetVolParmsInfoBuffer structure and the function will replace the value with
	the actual transferred buffer size.

	\maconly

	\param pOutput Pointer to an uninitialized GetVolParmsInfoBuffer buffer
	\param pOutputLength Pointer to a value that contains the size, in bytes, of
		the GetVolParmsInfoBuffer. Will return the actual size filled in.
	\param sVRefNum Volume reference number of the volume to get information
		from.

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::MacOS::get_volume_parms(
	GetVolParmsInfoBuffer* pOutput, uint32_t* pOutputLength, short sVRefNum)
{
	// Set up the parameter block
	HParamBlockRec hpbr;

	hpbr.ioParam.ioNamePtr = nullptr;
	hpbr.ioParam.ioVRefNum = sVRefNum;
	hpbr.ioParam.ioBuffer = reinterpret_cast<Ptr>(pOutput);
	hpbr.ioParam.ioReqCount = static_cast<long>(*pOutputLength);

	// Get the volume information using the MacOS 7.1 API
	int iMacError = PBHGetVolParmsSync(&hpbr);
	if (!iMacError) {

		// Return the actual number of bytes transferred
		*pOutputLength = static_cast<uint32_t>(hpbr.ioParam.ioActCount);
	}
	return iMacError;
}

/*! ************************************

	\brief Obtain information about a volume.

	Given a volume reference number, fill in a HParamBlockRec record with
	information about the volume. The ioNamePtr entry is zeroed out so
	retrieving a volume name is not possible with this call.

	\maconly

	\param pOutput Pointer to an uninitialized HParamBlockRec buffer
	\param sVRefNum Volume reference number

	\return Zero if no error, or mac error code

***************************************/

int BURGER_API Burger::MacOS::get_volume_info(
	HParamBlockRec* pOutput, short sVRefNum) BURGER_NOEXCEPT
{
	// Set the volume number
	pOutput->volumeParam.ioVRefNum = sVRefNum;

	// Don't worry about the name
	pOutput->volumeParam.ioNamePtr = nullptr;

	// Use only the volume reference number only
	pOutput->volumeParam.ioVolIndex = 0;

	// Get the information
	return PBHGetVInfoSync(pOutput);
}

#endif