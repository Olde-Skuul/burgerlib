/***************************************

	Filename Class

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILENAME_H__
#define __BRFILENAME_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

#if defined(BURGER_MAC) && !defined(__BRMACTYPES_H__)
#include "brmactypes.h"
#endif

/* BEGIN */
namespace Burger {
class Filename {

	/** Pointer to the burgerlib filename */
	String m_Filename;
	/** Pointer to the native filename */
	String m_NativeFilename;

	/** TRUE if the native path is valid */
	uint_t m_bNativeValid;

#if defined(BURGER_MAC) || defined(DOXYGEN)
	/** Opaque FSRef used by (MacOS 9 only) */
	uint8_t m_FSRef[80];
	/** Directory reference (MacOS 9 Only) */
	long m_lDirID;
	/** Volume reference used by copy and rename (MacOS 9 Only) */
	short m_sVRefNum;

	/** Number of cache entries (MacOS 9 only) */
	static const uint_t kDirectoryCacheSize = 8;

	struct ExpandCache_t {
		/** Pointer to the original directory name */
		const char* m_pName;
		/**  Last time hit (For purging) */
		uint32_t m_uHitTick;
		/** Length of the string */
		uint_t m_uNameLength;
		/** Opaque FSRef */
		uint8_t m_FSRef[80];
		/** Directory reference (MacOS 9 Only) */
		long m_lDirID;
		/** Volume reference used by copy and rename (MacOS 9 Only) */
		short m_sVRefNum;
	};

	/** Directory cache (MAC Classic/Carbon Only) */
	static ExpandCache_t m_DirectoryCache[kDirectoryCacheSize];

public:
	static void BURGER_API init_directory_cache(void);
	static void BURGER_API purge_directory_cache(void);
#endif
protected:
	eError BURGER_API end_with_colon(void) BURGER_NOEXCEPT;

public:
	Filename() BURGER_NOEXCEPT: m_bNativeValid(FALSE)
#if defined(BURGER_MAC)
		,
								m_lDirID(0),
								m_sVRefNum(0)
#endif
	{
	}
	Filename(const char* pFilename) BURGER_NOEXCEPT;
	Filename(Filename const& rInput) BURGER_NOEXCEPT;
	Filename& operator=(Filename const& rInput) BURGER_NOEXCEPT;
#if defined(BURGER_RVALUE_REFERENCES) || defined(DOXYGEN)
	Filename(Filename&& rInput) BURGER_NOEXCEPT;
	Filename& operator=(Filename&& rInput) BURGER_NOEXCEPT;
#endif
	~Filename();
	BURGER_INLINE const char* c_str(void) const BURGER_NOEXCEPT
	{
		return m_Filename.c_str();
	}
	BURGER_INLINE char* c_str(void) BURGER_NOEXCEPT
	{
		return m_Filename.c_str();
	}
	eError BURGER_API assign(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API assign(const uint16_t* pInput) BURGER_NOEXCEPT;
	void BURGER_API clear(void) BURGER_NOEXCEPT;
	eError BURGER_API join(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API join(
		const char* pInput, uintptr_t uInputLength) BURGER_NOEXCEPT;
	eError BURGER_API get_basename(
		char* pOutput, uintptr_t uOutputLength) const BURGER_NOEXCEPT;
	eError BURGER_API get_basename(String* pOutput) const BURGER_NOEXCEPT;
	eError BURGER_API get_file_extension(
		char* pOutput, uintptr_t uOutputLength) const BURGER_NOEXCEPT;
	eError BURGER_API set_file_extension(
		const char* pExtension) BURGER_NOEXCEPT;
	eError BURGER_API dirname(void) BURGER_NOEXCEPT;
	eError BURGER_API get_dirname(
		char* pOutput, uintptr_t uOutputLength) const BURGER_NOEXCEPT;
	eError BURGER_API get_dirname(String* pOutput) const BURGER_NOEXCEPT;
	uint_t BURGER_API is_drive_number(void) const BURGER_NOEXCEPT;
	uint_t BURGER_API is_abs(void) const BURGER_NOEXCEPT;
	uint_t BURGER_API is_filename_only(void) const BURGER_NOEXCEPT;
	uint_t BURGER_API has_prefix_number(void) const BURGER_NOEXCEPT;
	eError BURGER_API abs_path(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API abs_path(void) BURGER_NOEXCEPT;
	const char* BURGER_API get_native(void) BURGER_NOEXCEPT;

#if !defined(BURGER_MAC) || defined(DOXYGEN)
	eError BURGER_API set_native(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API set_native(const uint16_t* pInput) BURGER_NOEXCEPT;
#endif

	eError BURGER_API set_system_working_directory(void) BURGER_NOEXCEPT;
	eError BURGER_API set_application_directory(void) BURGER_NOEXCEPT;
	eError BURGER_API set_boot_volume(void) BURGER_NOEXCEPT;
	eError BURGER_API set_system_prefs_directory(void) BURGER_NOEXCEPT;
	eError BURGER_API set_user_prefs_directory(void) BURGER_NOEXCEPT;

#if defined(BURGER_MAC) || defined(DOXYGEN)
	eError BURGER_API set_native(const char* pInput, long lDirID = 0,
		short sVRefNum = 0) BURGER_NOEXCEPT;
	eError BURGER_API get_FSSpec(FSSpec* pFSSpec) BURGER_NOEXCEPT;
	FSRef* BURGER_API get_FSRef(void) BURGER_NOEXCEPT;
	BURGER_INLINE long get_DirID(void) const BURGER_NOEXCEPT
	{
		return m_lDirID;
	}
	BURGER_INLINE short get_VRefNum(void) const BURGER_NOEXCEPT
	{
		return m_sVRefNum;
	}
	BURGER_INLINE void set_DirID(long lDirID) BURGER_NOEXCEPT
	{
		m_lDirID = lDirID;
	}
	BURGER_INLINE void set_VRefNum(short sVRefNum) BURGER_NOEXCEPT
	{
		m_sVRefNum = sVRefNum;
	}
	eError BURGER_API set_native(long lDirID, short sVolRefNum) BURGER_NOEXCEPT;
	eError BURGER_API set_native_internal(void) BURGER_NOEXCEPT;
	eError BURGER_API get_native_internal(
		const char* pInput, long lDirID, short sVolRefNum) BURGER_NOEXCEPT;
#if !(defined(BURGER_CFM) && defined(BURGER_68K)) || defined(DOXYGEN)
	eError BURGER_API set_native_Carbon(void) BURGER_NOEXCEPT;
	eError BURGER_API get_native_Carbon(
		const char* pInput, long lDirID, short sVolRefNum) BURGER_NOEXCEPT;
#endif
#endif

	static Filename* BURGER_API new_object(void) BURGER_NOEXCEPT;
	static Filename* BURGER_API new_object(const char* pFilename) BURGER_NOEXCEPT;
	static Filename* BURGER_API new_object(Filename const& rInput) BURGER_NOEXCEPT;
};
}
/* END */

#endif
