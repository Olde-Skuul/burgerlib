/***************************************

	File Manager Class
	MSDOS Target version

	Copyright (c) 1995-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

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
#include "brutf8.h"
#include "brwin437.h"
#include <dos.h>

#if !defined(DOXYGEN)

// DTA for long filenames
struct WinDosData_t {
	uint32_t m_uAttrib;
	uint32_t m_CreationTimeLow;
	uint32_t m_CreationTimeHigh;
	uint32_t m_AccessTimeLow;
	uint32_t m_AccessTimeHigh;
	uint32_t m_WriteTimeLow;
	uint32_t m_WriteTimeHigh;
	uint32_t m_uSizeLow;
	uint32_t m_uSizeHigh;
	uint32_t m_ReservedLow;
	uint32_t m_ReservedHigh;
	char m_FileName[260];
	char m_ShortName[14];
};

// DTA for DOS 2.0 (Vintage)
struct DosData_t {
	uint8_t m_Reserved[21];
	uint8_t m_uAttrib;
	uint16_t m_WriteTimeLow;
	uint16_t m_WriteTimeHigh;
	uint16_t m_uSizeLow;
	uint16_t m_uSizeHigh;
	char m_FileName[13];
};
#endif

/***************************************

	Open a directory for scanning
	Return an error if the directory doesn't exist

***************************************/

Burger::eError Burger::DirectorySearch::Open(Filename* pName) BURGER_NOEXCEPT
{
	// Assume failure
	m_bHandleOk = 0;

	Regs16 Regs;

	// Get the DOS buffers
	uint32_t uRealBuffer = GetRealBufferPtr();
	char* pProtected = static_cast<char*>(GetRealBufferProtectedPtr());

	// Get the real pathname and convert to DOS encoding
	const char* pPath = pName->GetNative();
	uintptr_t i = Win437::TranslateFromUTF8(pProtected + 512, 512, pPath);

	if (i && reinterpret_cast<const uint8_t*>(pPath)[i - 1] != '\\') {
		StringConcatenate(pProtected + 512, "\\");
	}
	StringConcatenate(pProtected + 512, "*.*");

	// If under Windows 95 or higher, use the long filename form.
	if (FileManager::MSDOS_HasLongFilenames()) {

		// http://www.ctyme.com/intr/rb-3203.htm
		Regs.ax = 0x714E;
		// All directories and files
		Regs.cx = 0x0010;
		Regs.dx = static_cast<uint16_t>(uRealBuffer + 512);
		Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16);
		Regs.di = static_cast<uint16_t>(uRealBuffer);
		Regs.es = static_cast<uint16_t>(uRealBuffer >> 16);

		// Return DOS time
		Regs.si = 1;
		Int86x(0x21, &Regs, &Regs);
		if (!(Regs.flags & 1)) {
			m_bHandleOk = 10;
			m_sFileHandle = Regs.ax;
			return kErrorNone;
		}
		return kErrorAccessDenied;
	}

	// Use the old method

	// Get the Disk Transfer address and make a copy
	// http://www.ctyme.com/intr/rb-2710.htm
	Regs.ax = 0x2F00;
	Int86x(0x21, &Regs, &Regs);
	uint16_t uOldOffset = Regs.bx;
	uint16_t uOldSegment = Regs.es;

	// Set the disk transfer address to my buffer
	// http://www.ctyme.com/intr/rb-2589.htm
	Regs.ax = 0x1A00;
	Regs.dx = static_cast<uint16_t>(uRealBuffer);
	Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16);
	Int86x(0x21, &Regs, &Regs);

	// Open the file
	// http://www.ctyme.com/intr/rb-2977.htm
	Regs.ax = 0x4E00;
	// All directories and files
	Regs.cx = 0x0010;
	Regs.dx = static_cast<uint16_t>(uRealBuffer + 512);
	Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16);
	Int86x(0x21, &Regs, &Regs);

	eError uResult = kErrorAccessDenied;
	if (!(Regs.flags & 1)) {
		m_bHandleOk = 1;
		uResult = kErrorNone;
	}

	// Restore the disk transfer address address to the old value
	// http://www.ctyme.com/intr/rb-2589.htm
	Regs.ax = 0x1A00;
	Regs.dx = uOldOffset;
	Regs.ds = uOldSegment;
	Int86x(0x21, &Regs, &Regs);

	return uResult;
}

/***************************************

	Get the next directory entry

	Return FALSE if the entry is valid, TRUE if an error occurs.

***************************************/

Burger::eError Burger::DirectorySearch::GetNextEntry(void) BURGER_NOEXCEPT
{
	// Not opened?
	if (!m_bHandleOk) {
		return kErrorNotEnumerating;
	}

	Regs16 Regs;
	uint_t uFlags;

	uint32_t uRealBuffer = GetRealBufferPtr();
	uint8_t* pProtected = static_cast<uint8_t*>(GetRealBufferProtectedPtr());
	for (;;) {

		// Is this using the Windows 95 or higher code?
		if (m_bHandleOk >= 10) {

			// The open command already filled in the data, so it's already
			// ready.

			// Second call?
			if (m_bHandleOk == 11) {

				// Read the next entry
				// http://www.ctyme.com/intr/rb-3204.htm
				Regs.ax = 0x714F;
				Regs.bx = m_sFileHandle;
				Regs.di = static_cast<uint16_t>(uRealBuffer);
				Regs.es = static_cast<uint16_t>(uRealBuffer >> 16);

				// Return DOS time
				Regs.si = 1;
				Int86x(0x21, &Regs, &Regs);
				if (Regs.flags & 1) {
					// Close immediately
					Close();
					return kErrorNotEnumerating;
				}
			}

			// Entry is ready for parsing.
			m_bHandleOk = 11;

			const WinDosData_t* pWinDOS =
				reinterpret_cast<const WinDosData_t*>(pProtected);

			// Convert to UTF8
			UTF8::FromWin437(m_Name, sizeof(m_Name), pWinDOS->m_FileName);

			// If it's the directory entries, skip them
			uFlags = pWinDOS->m_uAttrib;
			if (uFlags & 0x10) {
				if (!StringCompare(".", m_Name) ||
					!StringCompare("..", m_Name)) {
					continue;
				}
			}

			m_CreationDate.LoadMSDOS(pWinDOS->m_CreationTimeLow);
			m_ModificatonDate.LoadMSDOS(pWinDOS->m_WriteTimeLow);
			if (pWinDOS->m_uSizeHigh) {
				m_uFileSize = 0xFFFFFFFFUL;
			} else {
				m_uFileSize = pWinDOS->m_uSizeLow;
			}
			break;
		}

		// Vintage? DOS 2.0
		if (m_bHandleOk == 2) {
			// Get the Disk Transfer address and make a copy
			// http://www.ctyme.com/intr/rb-2710.htm
			Regs.ax = 0x2F00;
			Int86x(0x21, &Regs, &Regs);
			uint16_t uOldOffset = Regs.bx;
			uint16_t uOldSegment = Regs.es;

			// Set the disk transfer address to my buffer
			// http://www.ctyme.com/intr/rb-2589.htm
			Regs.ax = 0x1A00;
			Regs.dx = static_cast<uint16_t>(uRealBuffer);
			Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16);
			Int86x(0x21, &Regs, &Regs);

			// Find next matching file
			// http://www.ctyme.com/intr/rb-2979.htm
			Regs.ax = 0x4F00;
			Regs.dx = static_cast<uint16_t>(uRealBuffer + 512);
			Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16);
			Int86x(0x21, &Regs, &Regs);

			uint_t uResult = TRUE;
			if (!(Regs.flags & 1)) {
				uResult = FALSE;
			}
			// Restore the disk transfer address address to the old value
			// http://www.ctyme.com/intr/rb-2589.htm
			Regs.ax = 0x1A00;
			Regs.dx = uOldOffset;
			Regs.ds = uOldSegment;
			Int86x(0x21, &Regs, &Regs);

			if (uResult) {
				m_bHandleOk = 0;
				return kErrorNotEnumerating;
			}
		}

		m_bHandleOk = 2;

		const DosData_t* pDOSData =
			reinterpret_cast<const DosData_t*>(pProtected);

		// Convert to UTF8
		UTF8::FromWin437(m_Name, sizeof(m_Name), pDOSData->m_FileName);

		// If it's the directory entries, skip them
		uFlags = pDOSData->m_uAttrib;
		if (uFlags & 0x10) {
			if (!StringCompare(".", m_Name) || !StringCompare("..", m_Name)) {
				continue;
			}
		}

		m_CreationDate.LoadMSDOS(
			reinterpret_cast<const uint32_t*>(&pDOSData->m_WriteTimeLow)[0]);
		MemoryCopy(&m_ModificatonDate, &m_CreationDate, sizeof(TimeDate_t));
		m_uFileSize = pDOSData->m_uSizeLow +
			(static_cast<uint32_t>(pDOSData->m_uSizeHigh) << 16);
		break;
	}

	// Set the attributes and exit without an error
	m_bLocked = static_cast<uint8_t>(uFlags & 0x01);
	m_bHidden = static_cast<uint8_t>((uFlags & 0x02) >> 1U);
	m_bSystem = static_cast<uint8_t>((uFlags & 0x04) >> 2U);
	m_bDir = static_cast<uint8_t>((uFlags & 0x10) >> 4U);
	return kErrorNone;
}

/***************************************

	Close a directory that's being scanned

***************************************/

void Burger::DirectorySearch::Close(void) BURGER_NOEXCEPT
{
	// Note: DOS 2.0 doesn't "close" the directory.

	// This can only be true if an extended directory
	if (m_bHandleOk >= 10) {

		// Properly close the directory
		// http://www.ctyme.com/intr/rb-3211.htm
		Regs16 MyRegs;
		MyRegs.ax = 0x71A1;
		MyRegs.bx = m_sFileHandle;
		Int86x(0x21, &MyRegs, &MyRegs);
		m_sFileHandle = 0;
	}
	m_bHandleOk = 0;
}

#endif
