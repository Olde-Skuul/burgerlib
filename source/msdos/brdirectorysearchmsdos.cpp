/***************************************

	File Manager Class
	MSDOS Target version

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_MSDOS)
#include "brdosextender.h"
#include "brfilemanager.h"
#include "brfilename.h"
#include <dos.h>

/***************************************

	Open a directory for scanning
	Return an error if the directory doesn't exist

***************************************/

Word Burger::DirectorySearch::Open(Filename *pName)
{
	Regs16 MyRegs;
	Word32 TempPath = GetRealBufferPtr();
	Word8 *DataPtr = static_cast<Word8 *>(GetRealBufferProtectedPtr());

	const char *pPath = pName->GetNative();
	StringCopy((char *)DataPtr+512,pPath);
	WordPtr i = StringLength(pPath);
	if (i && pPath[i-1]!='\\') {
		StringConcatenate((char *)DataPtr+512,"\\");
	}
	StringConcatenate((char *)DataPtr+512,"*.*");

	if (FileManager::AreLongFilenamesAllowed()) {
		MyRegs.ax = 0x714E;			/* Read first */
		MyRegs.cx = 0x0010;			/* All directories and files */
		MyRegs.dx = static_cast<Word16>(TempPath+512);
		MyRegs.ds = static_cast<Word16>(TempPath>>16);
		MyRegs.di = static_cast<Word16>(TempPath);
		MyRegs.es = static_cast<Word16>(TempPath>>16);
		MyRegs.si = 1;				/* Return DOS time */
		Int86x(0x21,&MyRegs,&MyRegs);
		if (!(MyRegs.flags&1)) {
			m_bHandleOk = 10;
			m_sFileHandle = MyRegs.ax;
			return FALSE;
		}
		return TRUE;
	}
	if (!_dos_findfirst((char *)DataPtr+512,0x10,(struct find_t *)m_MyFindT)) {
		m_bHandleOk = 1;
		return FALSE;
	}
	return TRUE;
}

/***************************************

	Get the next directory entry
	Return FALSE if the entry is valid, TRUE if
	an error occurs.

***************************************/

struct WinDosData_t {
	Word32 Attrib;
	Word32 CreateLo;
	Word32 CreateHi;
	Word32 AccessLo;
	Word32 AccessHi;
	Word32 WriteLo;
	Word32 WriteHi;
	Word32 SizeLo;
	Word32 SizeHi;
	Word32 ReservedLo;
	Word32 ReservedHi;
	char FileName[260];
	char ShortName[13];
};

Word Burger::DirectorySearch::GetNextEntry(void)
{
	if (!m_bHandleOk) {
		return TRUE;
	}
	Regs16 MyRegs;
	Word Flags;

	Word32 TempPath = GetRealBufferPtr();
	Word8 *DataPtr = static_cast<Word8*>(GetRealBufferProtectedPtr());
Again:
	if (FileManager::AreLongFilenamesAllowed()) {
		if (m_bHandleOk==11) {
			MyRegs.ax = 0x714F;
			MyRegs.bx = m_sFileHandle;
			MyRegs.di = static_cast<Word16>(TempPath);
			MyRegs.es = static_cast<Word16>(TempPath>>16);
			MyRegs.si = 1;			/* Return DOS time */
			Int86x(0x21,&MyRegs,&MyRegs);
			if (MyRegs.flags&1) {
				Close();
				return TRUE;
			}
		}
		m_bHandleOk = 11;
		StringCopy(m_Name,((WinDosData_t *)DataPtr)->FileName);
		m_bDir = FALSE;
		m_bHidden = FALSE;
		m_bSystem = FALSE;
		m_bLocked = FALSE;
		Flags = ((WinDosData_t *)DataPtr)->Attrib;
		if (Flags & 0x10) {
			m_bDir = TRUE;
			if (!StringCompare(".",m_Name) || !StringCompare("..",m_Name)) {
				goto Again;
			}
		}
		if (Flags & 0x01) {
			m_bLocked = TRUE;
		}
		if (Flags & 0x02) {
			m_bHidden = TRUE;
		}
		if (Flags & 0x04) {
			m_bSystem = TRUE;
		}
		m_CreationDate.LoadMSDOS(((WinDosData_t *)DataPtr)->CreateLo);
		m_ModificatonDate.LoadMSDOS(((WinDosData_t *)DataPtr)->WriteLo);
		if (((WinDosData_t *)DataPtr)->SizeHi) {
			m_uFileSize = 0xFFFFFFFFUL;
		} else {
			m_uFileSize = ((WinDosData_t *)DataPtr)->SizeLo;
		}
		return FALSE;
	}
	if (m_bHandleOk==2) {
		if (_dos_findnext((struct find_t *)m_MyFindT)) {
			return TRUE;
		}
	}
	m_bHandleOk = 2;
	StringCopy(m_Name,((struct find_t *)m_MyFindT)->name);

	m_bDir = FALSE;
	m_bHidden = FALSE;
	m_bSystem = FALSE;
	m_bLocked = FALSE;
	Flags = ((struct find_t *)m_MyFindT)->attrib;
	if (Flags & 0x10) {
		m_bDir = TRUE;
		if (!StringCompare(".",m_Name) || !StringCompare("..",m_Name)) {
			goto Again;
		}
	}
	if (Flags & 0x01) {
		m_bLocked = TRUE;
	}
	if (Flags & 0x02) {
		m_bHidden = TRUE;
	}
	if (Flags & 0x04) {
		m_bSystem = TRUE;
	}
	m_CreationDate.LoadMSDOS(((Word32 *)&((struct find_t *)m_MyFindT)->wr_time)[0]);
	MemoryCopy(&m_ModificatonDate,&m_CreationDate,sizeof(TimeDate_t));
	m_uFileSize = ((struct find_t *)m_MyFindT)->size;
	return FALSE;
}

/***************************************

	Close a directory that's being scanned

***************************************/

void Burger::DirectorySearch::Close(void)
{
	Regs16 MyRegs;
	if (m_bHandleOk>=10) {		/* This can only be true if an extended directory */
		MyRegs.ax = 0x71A1;
		MyRegs.bx = m_sFileHandle;
		Int86x(0x21,&MyRegs,&MyRegs);
		m_bHandleOk = FALSE;
	} else if (m_bHandleOk) {	/* Dos mode? */
		_dos_findclose((struct find_t *)m_MyFindT);
		m_bHandleOk = FALSE;
	}
}

#endif
