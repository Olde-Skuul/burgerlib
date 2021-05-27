/***************************************

    Resource manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRREZFILE_H__
#define __BRREZFILE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRMEMORYHANDLE_H__
#include "brmemoryhandle.h"
#endif

#ifndef __BRFILE_H__
#include "brfile.h"
#endif

#ifndef __BRDECOMPRESS_H__
#include "brdecompress.h"
#endif

/* BEGIN */
namespace Burger {
class RezFile {
    BURGER_DISABLE_COPY(RezFile);
public:
	static const char g_RezFileSignature[4];		///< 'BRGR'
	enum {
		MAXCODECS=3,			///< Maximum number of compression codecs available
		ROOTHEADERSIZE = static_cast<int>(sizeof(uint32_t)*2)+(MAXCODECS*4)+4,	///< Size of RootHeader_t on disk
		// Defines for the bits in m_uFileOffset on disk
		ENTRYFLAGSNAMEOFFSETMASK=0x0007FFFF,	///< Filename offset
		ENTRYFLAGSDECOMPMASK=0x00180000,	///< Mask for decompressors (2 bits)
		ENTRYFLAGSHIGHMEMORY=0x00200000,	///< True if load in fixed memory
		ENTRYFLAGSREFCOUNT=0xFF000000,	///< Refcount mask
		ENTRYFLAGSREFSHIFT=24,				///< Bits to shift for the refcount
		ENTRYFLAGSREFADD=(1<<ENTRYFLAGSREFSHIFT),	///< 1 in refcount format
		ENTRYFLAGSDECOMPSHIFT=19			///< Shift value to get the decompression type index (2 bits)
	};
	static const uint_t INVALIDREZNUM = static_cast<uint_t>(-1);	///< Illegal resource number, used as an error code

	struct RootHeader_t {
		char m_Name[4];			///< 'BRGR' 
		uint32_t m_uGroupCount;	///< Number of entries
		uint32_t m_uMemSize;		///< Amount of memory the entries take up
		char m_CodecID[MAXCODECS][4];	///< Compression codecs used
	};

	struct FileRezEntry_t {
		uint32_t m_uFileOffset;	///< Place in the rez file for the data
		uint32_t m_uLength;		///< Size of the data in the rez file (Uncompressed)
		uint32_t m_uNameOffset;	///< Offset to the filename
		uint32_t m_uCompressedLength;	///< Size of the data COMPRESSED, in the rez file, with the upper 3 bits as flags
	};
	
	struct FileRezGroup_t {
		uint32_t m_uBaseRezNum;	///< Resource base ID
		uint32_t m_uCount;		///< Number of resources in the group
		FileRezEntry_t m_Array[1];	///< Array of resources
	};

private:
	enum {
		MAXBUFFER = 65536,					///< Size of decompression buffer
		// Flags on for data records in the Rez File
		REZOFFSETFIXED=0x80000000,			///< True if load in fixed memory
		REZOFFSETDECOMPMASK=0x60000000,		///< Mask for decompressors
		REZOFFSETMASK=0x1FFFFFFF,			///< Big enough for 1 Gig file (GD-ROM or CDRom)
		REZOFFSETDECOMPSHIFT=29,			///< Decompressor mask shift value (Matches REZOFFSETDECOMPMASK)
		// Defines for the bits in m_uFileOffset on disk
		ENTRYFLAGSTESTED=0x00000001,		///< True if the filename was checked
		ENTRYFLAGSFILEFOUND=0x0000002,		///< True if a file was found
		// Used by the rez file parser
		SWAPENDIAN=0x01,					///< Manually swap endian
		OLDFORMAT=0x02						///< Parsing an old rez file format
	};

	struct RezEntry_t {
		void **m_ppData;		///< Handle to data in memory
		const char *m_pRezName;	///< Pointer to the resource name, or \ref NULL if none
		uint32_t m_uFileOffset;	///< Offset into the rez file
		uint32_t m_uLength;		///< Length of the data when decompressed in memory
		uint32_t m_uFlags;		///< Offset to the filename, Flags and Refcount
		uint32_t m_uCompressedLength;	///< Length of the data compressed
	};

	struct RezGroup_t {
		uint_t m_uBaseRezNum;		///< Base Resource number
		uint_t m_uCount;			///< Number of entries
		RezEntry_t m_Array[1];	///< First entry
	};
public:
	struct FilenameToRezNum_t {
		const char *m_pRezName;	///< Pointer to the filename
		uint_t m_uRezNum;			///< Resource number associated with this entry
	};
private:
	Decompress *m_Decompressors[MAXCODECS];	///< Decompressor functions
	File m_File;						///< Open file reference
	MemoryManagerHandle *m_pMemoryManager;	///< Pointer to the handle based memory manager to use
	uint32_t m_uGroupCount;				///< Number of resource groups
	uint32_t m_uRezNameCount;				///< Number of resource names in m_pRezNames
	RezGroup_t *m_pGroups;				///< Array of resource groups
	FilenameToRezNum_t *m_pRezNames;	///< Pointer to sorted resource names if present
	uint_t m_bExternalFileEnabled;		///< \ref TRUE if external file access is enabled

	static int BURGER_ANSIAPI QSortNames(const void *pFirst,const void *pSecond);
	uintptr_t BURGER_API GetRezGroupBytes(void) const;
	void BURGER_API AdjustNamePointers(uintptr_t uAdjust);
	RezEntry_t * BURGER_API Find(uint_t uRezNum) const;
	uint_t BURGER_API FindName(const char *pRezName,FilenameToRezNum_t **ppOutput) const;
	static RezGroup_t * BURGER_API ParseRezFileHeader(const uint8_t *pData,const RootHeader_t *pHeader,uint_t uSwapFlag,uint32_t uStartOffset);
	void BURGER_API ProcessRezNames(void);
	void BURGER_API FixupFilenames(char *pText);
public:
	RezFile(MemoryManagerHandle *pMemoryManager);
	~RezFile();
	static RezFile * BURGER_API New(MemoryManagerHandle *pMemoryManager,const char *pFileName,uint32_t uStartOffset=0);
	uint_t BURGER_API Init(const char *pFileName,uint32_t uStartOffset=0);
	void BURGER_API Shutdown(void);
	void BURGER_API PurgeCache(void);
	uint_t BURGER_API SetExternalFlag(uint_t bEnable);
	uint_t BURGER_INLINE GetExternalFlag(void) const { return m_bExternalFileEnabled; }
	void BURGER_API LogDecompressor(uint_t uCompressID,Decompress *pProc);
	uint_t BURGER_API GetRezNum(const char *pRezName) const;
	uint_t BURGER_API GetName(uint_t uRezNum,char *pBuffer,uintptr_t uBufferSize) const;
	uint_t BURGER_API AddName(const char *pRezName);
	void BURGER_API Remove(uint_t uRezNum);
	void BURGER_API Remove(const char *pRezName);
	BURGER_INLINE const FilenameToRezNum_t * GetNameArray(void) const { return m_pRezNames; }
	BURGER_INLINE uint_t GetNameArraySize(void) const { return m_uRezNameCount; }
	uint_t BURGER_API GetLowestRezNum(void) const;
	uint_t BURGER_API GetHighestRezNum(void) const;
	uintptr_t BURGER_API GetSize(uint_t uRezNum) const;
	uintptr_t BURGER_API GetCompressedSize(uint_t uRezNum) const;
	uint_t BURGER_API GetIDFromHandle(const void **ppRez,uint_t *pRezNum,char *pBuffer,uintptr_t uBufferSize) const;
	uint_t BURGER_API GetIDFromPointer(const void *pRez,uint_t *pRezNum,char *pBuffer,uintptr_t uBufferSize) const;
	void ** BURGER_API LoadHandle(uint_t uRezNum,uint_t *pLoadedFlag=NULL);
	void ** BURGER_API LoadHandle(const char *pRezName,uint_t *pLoadedFlag=NULL);
	void * BURGER_API Load(uint_t uRezNum,uint_t *pLoadedFlag=NULL);
	void * BURGER_API Load(const char *pRezName,uint_t *pLoadedFlag=NULL);
	uint_t BURGER_API Read(uint_t uRezNum,void *pBuffer,uintptr_t uBufferSize);
	uint_t BURGER_API Read(const char *pRezName,void *pBuffer,uintptr_t uBufferSize);
	void BURGER_API Release(uint_t uRezNum);
	void BURGER_API Release(const char *pRezName);
	void BURGER_API Kill(uint_t uRezNum);
	void BURGER_API Kill(const char *pRezName);
	void BURGER_API Detach(uint_t uRezNum);
	void BURGER_API Detach(const char *pRezName);
	void BURGER_API Preload(uint_t uRezNum);
	void BURGER_API Preload(const char *pRezName);
};
class InputRezStream : public InputMemoryStream {
protected:
	RezFile *m_pRezFile;		///< Pointer to the resource file this stream came from
	uint_t m_uRezNum;				///< Which resource entry is being streamed?
public:
	InputRezStream();
	InputRezStream(RezFile *pRezFile,uint_t uRezNum);
	~InputRezStream();
	uint_t BURGER_API Open(RezFile *pRezFile,uint_t uRezNum);
	void BURGER_API Release(void);
	BURGER_INLINE RezFile *GetRezFile(void) const { return m_pRezFile; }
	BURGER_INLINE uint_t GetRezNum(void) const { return m_uRezNum; }
};
}
/* END */

#endif

