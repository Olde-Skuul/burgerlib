/***************************************

	MacOS GetVolParmsInfoBuffer handlers

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MAC_VOLPARMS_H__
#define __MAC_VOLPARMS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __MAC_TYPES_H__
#include "mac_types.h"
#endif

/* BEGIN */
namespace Burger { namespace MacOS {

extern short BURGER_API get_version(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern long BURGER_API get_attributes(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern char** BURGER_API get_local_handle(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint32_t BURGER_API get_server_address(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern long BURGER_API get_volume_grade(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern short BURGER_API get_foreign_PrivID(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern long BURGER_API get_extended_attributes(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern void* BURGER_API get_device_ID(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;

extern uint_t BURGER_API is_network_volume(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_limit_FCBs(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_local_WList(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_no_mini_finder(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_no_volume_name_edit(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_no_local_sync(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_trash_offline(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_no_switch_to(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_no_desk_items(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_no_boot_blocks(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_access_control(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_no_system_directory(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_external_file_system_volume(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_open_deny(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_copy_file(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_move_rename(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_desktop_manager(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_short_name(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_folder_lock(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_personal_access(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_user_group_list(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_cat_search(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_file_IDs(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_BTree_manager(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_blank_access(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_support_async_requests(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_trash_volume_cache(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;

extern uint_t BURGER_API has_ejectable(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_HFSPlus_APIs(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_FSCatalogSearch(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_FSExchangeObjects(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_2TB_file_support(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_long_filenames(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_multiple_script_names(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_named_forks(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_subtree_iterators(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;
extern uint_t BURGER_API has_large_to_physical_blocks(
	const GetVolParmsInfoBuffer* pVolParamsInfoBuffer) BURGER_NOEXCEPT;

extern int BURGER_API get_volume_parms(GetVolParmsInfoBuffer* pOutput,
	uint32_t* pOutputLength, short sVRefNum) BURGER_NOEXCEPT;

extern int BURGER_API get_volume_info(
	HParamBlockRec* pOutput, short sVRefNum) BURGER_NOEXCEPT;
}}

/* END */

#endif
