/***************************************

	Filename Class

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	static void BURGER_API InitDirectoryCache(void);
	static void BURGER_API PurgeDirectoryCache(void);
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
	void BURGER_API get_basename(
		char* pOutput, uintptr_t uOutputLength) const BURGER_NOEXCEPT;
	void BURGER_API get_basename(String* pOutput) const BURGER_NOEXCEPT;
	void BURGER_API get_file_extension(
		char* pOutput, uintptr_t uOutputLength) const BURGER_NOEXCEPT;
	eError BURGER_API set_file_extension(
		const char* pExtension) BURGER_NOEXCEPT;
	eError BURGER_API dirname(void) BURGER_NOEXCEPT;
	eError BURGER_API get_dirname(String* pOutput) const BURGER_NOEXCEPT;

	uint_t BURGER_API IsFullPathname(void) const BURGER_NOEXCEPT;
	uint_t BURGER_API IsFilenameOnly(void) const BURGER_NOEXCEPT;
	uint_t BURGER_API ParsePrefixNumber(void) const BURGER_NOEXCEPT;
	eError BURGER_API Expand(void) BURGER_NOEXCEPT;
	eError BURGER_API Expand(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API SetSystemWorkingDirectory(void) BURGER_NOEXCEPT;
	eError BURGER_API SetApplicationDirectory(void) BURGER_NOEXCEPT;
	eError BURGER_API SetBootVolumeDirectory(void) BURGER_NOEXCEPT;
	eError BURGER_API SetMachinePrefsDirectory(void) BURGER_NOEXCEPT;
	eError BURGER_API SetUserPrefsDirectory(void) BURGER_NOEXCEPT;
	const char* BURGER_API GetNative(void) BURGER_NOEXCEPT;

#if !defined(BURGER_MAC) || defined(DOXYGEN)
	eError BURGER_API SetFromNative(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API SetFromNative(const uint16_t* pInput) BURGER_NOEXCEPT;
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
	eError BURGER_API SetFromNative(const char* pInput, long lDirID = 0,
		short sVRefNum = 0) BURGER_NOEXCEPT;
	eError BURGER_API GetFSSpec(FSSpec* pFSSpec) BURGER_NOEXCEPT;

	BURGER_INLINE FSRef* GetFSRefOld(void) BURGER_NOEXCEPT
	{
		return reinterpret_cast<FSRef*>(m_FSRef);
	}
	FSRef* BURGER_API GetFSRef(void) BURGER_NOEXCEPT;
	BURGER_INLINE long GetDirID(void) const BURGER_NOEXCEPT
	{
		return m_lDirID;
	}
	BURGER_INLINE short GetVRefNum(void) const BURGER_NOEXCEPT
	{
		return m_sVRefNum;
	}
	BURGER_INLINE void SetDirID(long lDirID) BURGER_NOEXCEPT
	{
		m_lDirID = lDirID;
	}
	BURGER_INLINE void SetVRefNum(short sVRefNum) BURGER_NOEXCEPT
	{
		m_sVRefNum = sVRefNum;
	}
	eError BURGER_API SetFromDirectoryID(
		long lDirID, short sVolRefNum) BURGER_NOEXCEPT;
	eError BURGER_API SetFromDirectoryIDClassic(void) BURGER_NOEXCEPT;
	eError BURGER_API GetNativeClassic(
		const char* pInput, long lDirID, short sVolRefNum) BURGER_NOEXCEPT;
#if !(defined(BURGER_CFM) && defined(BURGER_68K)) || defined(DOXYGEN)
	eError BURGER_API SetFromDirectoryIDCarbon(void) BURGER_NOEXCEPT;
	eError BURGER_API GetNativeCarbon(
		const char* pInput, long lDirID, short sVolRefNum) BURGER_NOEXCEPT;
#endif
#endif

	static Filename* BURGER_API New(void) BURGER_NOEXCEPT;
	static Filename* BURGER_API New(const char* pFilename) BURGER_NOEXCEPT;
	static Filename* BURGER_API New(Filename const& rInput) BURGER_NOEXCEPT;
};
}
/* END */

#endif
