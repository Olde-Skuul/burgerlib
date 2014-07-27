/***************************************

	Filename Class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
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
public:
	enum {
		MAXBUFFERSIZE = 256,	///< Size of the filename buffer (Maximum)
#if defined(BURGER_MAC)
		BUFFERSIZE = MAXBUFFERSIZE-static_cast<int>(sizeof(char *)+sizeof(long)+sizeof(short))	///< Size of the filename buffer (Adjusted to to keep the class size at exactly 512 bytes in length)
#else
		BUFFERSIZE = static_cast<int>(MAXBUFFERSIZE-sizeof(char *))		///< Size of the filename buffer (Adjusted to to keep the class size at exactly 512 bytes in length)
#endif
	};
	Filename() : m_pFilename(m_Filename), m_pNativeFilename(m_NativeFilename)
#if defined(BURGER_MAC)
	,m_lDirID(0),m_sVRefNum(0)
#endif
	{ m_Filename[0] = 0; }
	Filename(const char *pFilename);
	Filename(Filename const &rInput);
	Filename & operator = (Filename const &rInput);
	~Filename();
	BURGER_INLINE const char *GetPtr(void) const { return m_pFilename; }
	BURGER_INLINE char *GetPtr(void) { return m_pFilename; }
	void Set(const char *pInput);
	void Clear(void);
	void Append(const char *pInput);
	void GetFileName(char *pOutput,WordPtr uOutputLength) const;
	void GetFileExtension(char *pOutput,WordPtr uOutputLength) const;
	void SetFileExtension(const char *pExtension);
	void DirName(void);
	void DirName(String *pOutput) const;
	void BaseName(String *pOutput) const;
	Word IsFullPathname(void) const;
	Word IsFilenameOnly(void) const;
	Word ParsePrefixNumber(void) const;
	void Expand(void);
	void Expand(const char *pInput);
	const char *GetNative(void);
#if !defined(BURGER_MAC) || defined(DOXYGEN)
	void SetFromNative(const char *pInput);
#endif
#if defined(BURGER_MAC) || defined(DOXYGEN)
	void SetFromNative(const char *pInput,long lDirID=0,short sVRefNum=0);
	BURGER_INLINE FSRef *GetFSRef(void) { return reinterpret_cast<FSRef *>(m_FSRef); }
	BURGER_INLINE long GetDirID(void) const { return m_lDirID; }
	BURGER_INLINE short GetVRefNum(void) const { return m_sVRefNum; }
	BURGER_INLINE void SetDirID(long lDirID) { m_lDirID = lDirID; }
	BURGER_INLINE void SetVRefNum(short sVRefNum) { m_sVRefNum = sVRefNum; }
	Word SetFromDirectoryID(long lDirID,short sVolRefNum);
#endif 
public:
	char *m_pFilename;				///< Pointer to the burgerlib filename
	char *m_pNativeFilename;		///< Pointer to the native filename
#if defined(BURGER_MAC)
	Word8 m_FSRef[80];				///< Opaque FSRef used by MacOS
	long m_lDirID;					///< Directory reference (MacOS Only)
	short m_sVRefNum;				///< Volume reference used by copy and rename  (MacOS Only)
#endif
	char m_Filename[BUFFERSIZE];	///< Local buffer containing the filename (If it can fit in this buffer)
#if !defined(BURGER_MAC) && !defined(DOXYGEN)
	char m_NativeFilename[MAXBUFFERSIZE-(sizeof(char *))];	///< Local buffer containing the native filename
#else
	char m_NativeFilename[MAXBUFFERSIZE-(sizeof(char *)+80)];	///< Local buffer containing the native filename
	static const Word DIRCACHESIZE = 8;	///<  Number of cache entries
	struct ExpandCache_t {
		const char *m_pName;		///< Pointer to the original directory name
		Word32 m_uHitTick;			///< Last time hit (For purging)
		Word m_uNameLength;			///< Length of the string
		Word8 m_FSRef[80];			///< Opaque FSRef
	};
	static ExpandCache_t m_DirectoryCache[DIRCACHESIZE];	///< Directory cache (MAC Classic/Carbon Only)
public:
	static void BURGER_API InitDirectoryCache(void);
	static void BURGER_API PurgeDirectoryCache(void);
#endif
};
}
/* END */

#endif

