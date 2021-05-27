/***************************************

    Filename Class

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
	static const uint_t cBufferSize = 256;	///< Size of the filename buffer (Maximum)

	char *m_pFilename;				///< Pointer to the burgerlib filename
	char *m_pNativeFilename;		///< Pointer to the native filename

#if !defined(BURGER_MAC) && !defined(DOXYGEN)
	char m_Filename[cBufferSize-(sizeof(char *))];	///< Local buffer containing the filename (If it can fit in this buffer)
	char m_NativeFilename[cBufferSize-(sizeof(char *))];	///< Local buffer containing the native filename
#else
	uint8_t m_FSRef[80];				///< Opaque FSRef used by (MacOS 9 only)
	long m_lDirID;					///< Directory reference (MacOS 9 Only)
	short m_sVRefNum;				///< Volume reference used by copy and rename (MacOS 9 Only)
	char m_Filename[cBufferSize-(sizeof(char *)+sizeof(long)+sizeof(short))];	///< Local buffer containing the filename (If it can fit in this buffer)
	char m_NativeFilename[cBufferSize-(sizeof(char *)+80)];	///< Local buffer containing the native filename

	static const uint_t DIRCACHESIZE = 8;	///< Number of cache entries (MacOS 9 only)
	struct ExpandCache_t {
		const char *m_pName;		///< Pointer to the original directory name
		uint32_t m_uHitTick;			///< Last time hit (For purging)
		uint_t m_uNameLength;			///< Length of the string
		uint8_t m_FSRef[80];			///< Opaque FSRef
	};
	static ExpandCache_t m_DirectoryCache[DIRCACHESIZE];	///< Directory cache (MAC Classic/Carbon Only)
public:
	static void BURGER_API InitDirectoryCache(void);
	static void BURGER_API PurgeDirectoryCache(void);
#endif

public:
	Filename() BURGER_NOEXCEPT : m_pFilename(m_Filename), m_pNativeFilename(m_NativeFilename)
#if defined(BURGER_MAC)
	,m_lDirID(0),m_sVRefNum(0)
#endif
	{ m_Filename[0] = 0; m_NativeFilename[0] = 0; }
	Filename(const char *pFilename) BURGER_NOEXCEPT;
	Filename(Filename const &rInput) BURGER_NOEXCEPT;
	Filename & operator = (Filename const &rInput);
	~Filename();
	BURGER_INLINE const char *GetPtr(void) const BURGER_NOEXCEPT { return m_pFilename; }
	BURGER_INLINE char *GetPtr(void) BURGER_NOEXCEPT { return m_pFilename; }
	void BURGER_API Set(const char *pInput) BURGER_NOEXCEPT;
	void BURGER_API Set(const uint16_t *pInput) BURGER_NOEXCEPT;
	void BURGER_API Clear(void) BURGER_NOEXCEPT;
	void BURGER_API Append(const char *pInput);
	void BURGER_API GetFileName(char *pOutput,uintptr_t uOutputLength) const;
	void BURGER_API GetFileExtension(char *pOutput,uintptr_t uOutputLength) const;
	void BURGER_API SetFileExtension(const char *pExtension);
	void BURGER_API DirName(void);
	void BURGER_API DirName(String *pOutput) const;
	void BURGER_API BaseName(String *pOutput) const;
	uint_t BURGER_API IsFullPathname(void) const;
	uint_t BURGER_API IsFilenameOnly(void) const;
	uint_t BURGER_API ParsePrefixNumber(void) const;
	void BURGER_API Expand(void) BURGER_NOEXCEPT;
	void BURGER_API Expand(const char *pInput) BURGER_NOEXCEPT;
	void BURGER_API SetSystemWorkingDirectory(void);
	void BURGER_API SetApplicationDirectory(void);
	void BURGER_API SetMachinePrefsDirectory(void);
	void BURGER_API SetUserPrefsDirectory(void);
	const char * BURGER_API GetNative(void);

#if !defined(BURGER_MAC) || defined(DOXYGEN)
	void BURGER_API SetFromNative(const char *pInput);
	void BURGER_API SetFromNative(const uint16_t *pInput);
#endif

#if defined(BURGER_MAC) || defined(DOXYGEN)
	void BURGER_API SetFromNative(const char *pInput,long lDirID=0,short sVRefNum=0);
	BURGER_INLINE FSRef *GetFSRef(void) { return reinterpret_cast<FSRef *>(m_FSRef); }
	BURGER_INLINE long GetDirID(void) const { return m_lDirID; }
	BURGER_INLINE short GetVRefNum(void) const { return m_sVRefNum; }
	BURGER_INLINE void SetDirID(long lDirID) { m_lDirID = lDirID; }
	BURGER_INLINE void SetVRefNum(short sVRefNum) { m_sVRefNum = sVRefNum; }
	uint_t BURGER_API SetFromDirectoryID(long lDirID,short sVolRefNum);
#endif

	static Filename * BURGER_API New(void);
	static Filename * BURGER_API New(const char *pFilename);
	static Filename * BURGER_API New(Filename const &rInput);
};
}
/* END */

#endif

