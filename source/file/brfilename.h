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
	/** Size of the filename buffer(Maximum) */
	static const uint_t kBufferSize = 256;

	/** Pointer to the burgerlib filename */
	char* m_pFilename;
	/** Pointer to the native filename */
	char* m_pNativeFilename;

#if !defined(BURGER_MAC) && !defined(DOXYGEN)
	/** Local buffer containing the filename (If it can fit in this buffer) */
	char m_Filename[kBufferSize - (sizeof(char*))];
	/** Local buffer containing the native filename */
	char m_NativeFilename[kBufferSize - (sizeof(char*))];
#else
	/** Opaque FSRef used by (MacOS 9 only) */
	uint8_t m_FSRef[80];
	/** Directory reference (MacOS 9 Only) */
	long m_lDirID;
	/** Volume reference used by copy and rename (MacOS 9 Only) */
	short m_sVRefNum;
	/** Local buffer containing the filename (If it can fit in this buffer) */
	char m_Filename[kBufferSize -
		(sizeof(char*) + sizeof(long) + sizeof(short))];
	/** Local buffer containing the native filename */
	char m_NativeFilename[kBufferSize - (sizeof(char*) + 80)];

	/** Number of cache entries (MacOS 9 only) */
	static const uint_t kDirectoryCacheSize = 8;

	struct ExpandCache_t {
		const char* m_pName;  ///< Pointer to the original directory name
		uint32_t m_uHitTick;  ///< Last time hit (For purging)
		uint_t m_uNameLength; ///< Length of the string
		uint8_t m_FSRef[80];  ///< Opaque FSRef
	};

	/** Directory cache (MAC Classic/Carbon Only) */
	static ExpandCache_t m_DirectoryCache[kDirectoryCacheSize];

public:
	static void BURGER_API InitDirectoryCache(void);
	static void BURGER_API PurgeDirectoryCache(void);
#endif

public:
	Filename() BURGER_NOEXCEPT: m_pFilename(m_Filename),
								m_pNativeFilename(m_NativeFilename)
#if defined(BURGER_MAC)
		,
								m_lDirID(0),
								m_sVRefNum(0)
#endif
	{
		m_Filename[0] = 0;
		m_NativeFilename[0] = 0;
	}
	Filename(const char* pFilename) BURGER_NOEXCEPT;
	Filename(Filename const& rInput) BURGER_NOEXCEPT;
	Filename& operator=(Filename const& rInput);
	~Filename();
	BURGER_INLINE const char* c_str(void) const BURGER_NOEXCEPT
	{
		return m_pFilename;
	}
	BURGER_INLINE char* c_str(void) BURGER_NOEXCEPT
	{
		return m_pFilename;
	}
	eError BURGER_API Set(const char* pInput) BURGER_NOEXCEPT;
	eError BURGER_API Set(const uint16_t* pInput) BURGER_NOEXCEPT;
	void BURGER_API Clear(void) BURGER_NOEXCEPT;
	eError BURGER_API Append(const char* pInput);
	void BURGER_API GetFileName(char* pOutput, uintptr_t uOutputLength) const;
	void BURGER_API GetFileExtension(
		char* pOutput, uintptr_t uOutputLength) const BURGER_NOEXCEPT;
	void BURGER_API SetFileExtension(const char* pExtension) BURGER_NOEXCEPT;
	eError BURGER_API DirName(void) BURGER_NOEXCEPT;
	eError BURGER_API DirName(String* pOutput) const BURGER_NOEXCEPT;
	void BURGER_API BaseName(String* pOutput) const BURGER_NOEXCEPT;
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
	eError BURGER_API SetFromNative(
		const char* pInput, long lDirID = 0, short sVRefNum = 0) BURGER_NOEXCEPT;
	BURGER_INLINE FSRef* GetFSRef(void)
	{
		return reinterpret_cast<FSRef*>(m_FSRef);
	}
	BURGER_INLINE long GetDirID(void) const
	{
		return m_lDirID;
	}
	BURGER_INLINE short GetVRefNum(void) const
	{
		return m_sVRefNum;
	}
	BURGER_INLINE void SetDirID(long lDirID)
	{
		m_lDirID = lDirID;
	}
	BURGER_INLINE void SetVRefNum(short sVRefNum)
	{
		m_sVRefNum = sVRefNum;
	}
	eError BURGER_API SetFromDirectoryID(
		long lDirID, short sVolRefNum) BURGER_NOEXCEPT;
#endif

	static Filename* BURGER_API New(void) BURGER_NOEXCEPT;
	static Filename* BURGER_API New(const char* pFilename) BURGER_NOEXCEPT;
	static Filename* BURGER_API New(Filename const& rInput) BURGER_NOEXCEPT;
};
}
/* END */

#endif
