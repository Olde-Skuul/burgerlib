/***************************************

    File Manager Class
    MSDOS Target version

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_MSDOS)
#include "brdosextender.h"
#include "brfilemanager.h"
#include "brfilename.h"
#include "brmemoryfunctions.h"
#include <dos.h>

/***************************************

	Open a directory for scanning
	Return an error if the directory doesn't exist

***************************************/

uint_t Burger::DirectorySearch::Open(Filename *pName) BURGER_NOEXCEPT
{
	Regs16 MyRegs;
	uint32_t TempPath = GetRealBufferPtr();
	uint8_t *DataPtr = static_cast<uint8_t *>(GetRealBufferProtectedPtr());

	const char *pPath = pName->GetNative();
	StringCopy((char *)DataPtr+512,pPath);
	uintptr_t i = StringLength(pPath);
	if (i && pPath[i-1]!='\\') {
		StringConcatenate((char *)DataPtr+512,"\\");
	}
	StringConcatenate((char *)DataPtr+512,"*.*");

	if (FileManager::AreLongFilenamesAllowed()) {
		MyRegs.ax = 0x714E;			/* Read first */
		MyRegs.cx = 0x0010;			/* All directories and files */
		MyRegs.dx = static_cast<uint16_t>(TempPath+512);
		MyRegs.ds = static_cast<uint16_t>(TempPath>>16);
		MyRegs.di = static_cast<uint16_t>(TempPath);
		MyRegs.es = static_cast<uint16_t>(TempPath>>16);
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
	uint32_t Attrib;
	uint32_t CreateLo;
	uint32_t CreateHi;
	uint32_t AccessLo;
	uint32_t AccessHi;
	uint32_t WriteLo;
	uint32_t WriteHi;
	uint32_t SizeLo;
	uint32_t SizeHi;
	uint32_t ReservedLo;
	uint32_t ReservedHi;
	char FileName[260];
	char ShortName[13];
};

uint_t Burger::DirectorySearch::GetNextEntry(void) BURGER_NOEXCEPT
{
	if (!m_bHandleOk) {
		return TRUE;
	}
	Regs16 MyRegs;
	uint_t Flags;

	uint32_t TempPath = GetRealBufferPtr();
	uint8_t *DataPtr = static_cast<uint8_t*>(GetRealBufferProtectedPtr());
Again:
	if (FileManager::AreLongFilenamesAllowed()) {
		if (m_bHandleOk==11) {
			MyRegs.ax = 0x714F;
			MyRegs.bx = m_sFileHandle;
			MyRegs.di = static_cast<uint16_t>(TempPath);
			MyRegs.es = static_cast<uint16_t>(TempPath>>16);
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
	m_CreationDate.LoadMSDOS(((uint32_t *)&((struct find_t *)m_MyFindT)->wr_time)[0]);
	MemoryCopy(&m_ModificatonDate,&m_CreationDate,sizeof(TimeDate_t));
	m_uFileSize = ((struct find_t *)m_MyFindT)->size;
	return FALSE;
}

/***************************************

	Close a directory that's being scanned

***************************************/

void Burger::DirectorySearch::Close(void) BURGER_NOEXCEPT
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
