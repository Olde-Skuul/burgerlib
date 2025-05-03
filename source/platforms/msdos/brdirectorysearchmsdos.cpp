/***************************************

	Directory search Class
	MSDOS version

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brdirectorysearch.h"

#if defined(BURGER_MSDOS)
#include "brfilemanager.h"
#include "brmemoryfunctions.h"
#include "brutf8.h"
#include "brwin437.h"
#include "msdos_structs.h"
#include "msdos_memory.h"

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

	\brief Parse directory using the old API

	\param pOutput Pointer to the DirectoryEntry_t array for output

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

***************************************/

static Burger::eError BURGER_API OldWay(
	Burger::SimpleArray<Burger::DirectoryEntry_t>* pOutput)
{
	// x86 registers for use with INT 021H
	Burger::MSDos::Regs16_t Regs;

	// Get the Disk Transfer address and make a copy
	// http://www.ctyme.com/intr/rb-2710.htm
	Regs.ax = 0x2F00U;
	Burger::MSDos::real_mode_interrupt(0x21U, &Regs, &Regs);
	uint16_t uOldOffset = Regs.bx;
	uint16_t uOldSegment = Regs.es;

	// Set the disk transfer address to my buffer
	// http://www.ctyme.com/intr/rb-2589.htm
	uint32_t uRealBuffer = Burger::MSDos::get_temp_real_buffer();
	Regs.ax = 0x1A00U;
	Regs.dx = static_cast<uint16_t>(uRealBuffer);
	Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16U);
	Burger::MSDos::real_mode_interrupt(0x21U, &Regs, &Regs);

	// Open the file
	// http://www.ctyme.com/intr/rb-2977.htm
	Regs.ax = 0x4E00U;

	// All directories and files
	Regs.cx = 0x0010U;
	Regs.dx = static_cast<uint16_t>(uRealBuffer + 512U);
	Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16U);
	Burger::MSDos::real_mode_interrupt(0x21U, &Regs, &Regs);

	Burger::eError uResult = Burger::kErrorNone;
	if (Regs.flags & 1U) {
		uResult = Burger::kErrorPathNotFound;

	} else {

		// It opened. Process!

		// Init the buffer
		pOutput->reserve(Burger::DirectorySearch::kDefaultReserve);
		char* pProtected = static_cast<char*>(Burger::MSDos::get_temp_protected_buffer());

		do {
			const DosData_t* pDOSData =
				reinterpret_cast<const DosData_t*>(pProtected);

			// If it's . or .. directories, skip them
			uint_t uFlags = pDOSData->m_uAttrib;
			if (!(uFlags & 0x10U) ||
				(Burger::StringCompare(".", pDOSData->m_FileName) &&
					Burger::StringCompare("..", pDOSData->m_FileName))) {

				// New directory entry
				Burger::DirectoryEntry_t Entry;

				// Set the attributes
				Entry.m_bLocked = static_cast<uint8_t>(uFlags & 0x01);
				Entry.m_bHidden = static_cast<uint8_t>((uFlags & 0x02) >> 1U);
				Entry.m_bSystem = static_cast<uint8_t>((uFlags & 0x04) >> 2U);
				Entry.m_bDir = static_cast<uint8_t>((uFlags & 0x10) >> 4U);

				// Get the file size (32 bit only)
				Entry.m_uFileSize = pDOSData->m_uSizeLow +
					(static_cast<uint32_t>(pDOSData->m_uSizeHigh) << 16U);

				// Get the modification time and copy as creation time
				Entry.m_CreationDate.LoadMSDOS(
					reinterpret_cast<const uint32_t*>(
						&pDOSData->m_WriteTimeLow)[0]);
				Burger::memory_copy(&Entry.m_ModificatonDate,
					&Entry.m_CreationDate, sizeof(Entry.m_CreationDate));

				// Convert filename to UTF8
				char TempName[128];
				Burger::UTF8::FromWin437(
					TempName, sizeof(TempName), pDOSData->m_FileName);

				// Make a copy of the string and store it
				const char* pName = Burger::StringDuplicate(TempName);
				if (!pName) {
					uResult = Burger::kErrorOutOfMemory;
					break;
				}
				Entry.m_pName = pName;

				// Add to the list
				uResult = pOutput->append(&Entry, 1);
			}

			// Find next matching file
			// http://www.ctyme.com/intr/rb-2979.htm
			Regs.ax = 0x4F00;
			Regs.dx = static_cast<uint16_t>(uRealBuffer + 512);
			Regs.ds = static_cast<uint16_t>(uRealBuffer >> 16);
			Burger::MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

			// Reached the end of the directory?
			if (Regs.flags & 1) {
				break;
			}

		} while (!uResult);
	}

	// Restore the disk transfer address address to the old value
	// http://www.ctyme.com/intr/rb-2589.htm
	Regs.ax = 0x1A00U;
	Regs.dx = uOldOffset;
	Regs.ds = uOldSegment;
	Burger::MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

	return uResult;
}

/***************************************

	\brief Parse directory using the new API

	\param pOutput Pointer to the DirectoryEntry_t array for output

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

***************************************/

static Burger::eError BURGER_API NewWay(
	Burger::SimpleArray<Burger::DirectoryEntry_t>* pOutput)
{
	// x86 registers for use with INT 021H
	Burger::MSDos::Regs16_t Regs;

	uint32_t uRealBuffer = Burger::MSDos::get_temp_real_buffer();

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
	Burger::MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

	Burger::eError uResult = Burger::kErrorNone;
	if (Regs.flags & 1U) {
		uResult = Burger::kErrorPathNotFound;

	} else {

		// It opened. Process!
		uint16_t sFileHandle = Regs.ax;

		// Init the buffer
		pOutput->reserve(Burger::DirectorySearch::kDefaultReserve);
		char* pProtected = static_cast<char*>(Burger::MSDos::get_temp_protected_buffer());

		do {
			const WinDosData_t* pDOSData =
				reinterpret_cast<const WinDosData_t*>(pProtected);

			// If it's . or .. directories, skip them
			uint_t uFlags = pDOSData->m_uAttrib;
			if (!(uFlags & 0x10U) ||
				(Burger::StringCompare(".", pDOSData->m_FileName) &&
					Burger::StringCompare("..", pDOSData->m_FileName))) {

				// New directory entry
				Burger::DirectoryEntry_t Entry;

				// Set the attributes
				Entry.m_bLocked = static_cast<uint8_t>(uFlags & 0x01);
				Entry.m_bHidden = static_cast<uint8_t>((uFlags & 0x02) >> 1U);
				Entry.m_bSystem = static_cast<uint8_t>((uFlags & 0x04) >> 2U);
				Entry.m_bDir = static_cast<uint8_t>((uFlags & 0x10) >> 4U);

				// Get the file size (32 bit only)
				Entry.m_uFileSize = pDOSData->m_uSizeLow +
					(static_cast<uint64_t>(pDOSData->m_uSizeHigh) << 32U);

				// Get the modification time and copy as creation time
				Entry.m_CreationDate.LoadMSDOS(
					reinterpret_cast<const uint32_t*>(
						&pDOSData->m_WriteTimeLow)[0]);
				Burger::memory_copy(&Entry.m_ModificatonDate,
					&Entry.m_CreationDate, sizeof(Entry.m_CreationDate));

				// Convert filename to UTF8
				char TempName[512];
				Burger::UTF8::FromWin437(
					TempName, sizeof(TempName), pDOSData->m_FileName);

				// Make a copy of the string and store it
				const char* pName = Burger::StringDuplicate(TempName);
				if (!pName) {
					uResult = Burger::kErrorOutOfMemory;
					break;
				}
				Entry.m_pName = pName;

				// Add to the list
				uResult = pOutput->append(&Entry, 1);
			}

			// Read the next entry
			// http://www.ctyme.com/intr/rb-3204.htm
			Regs.ax = 0x714F;
			Regs.bx = sFileHandle;
			Regs.di = static_cast<uint16_t>(uRealBuffer);
			Regs.es = static_cast<uint16_t>(uRealBuffer >> 16);

			// Return DOS time
			Regs.si = 1;
			Burger::MSDos::real_mode_interrupt(0x21, &Regs, &Regs);

			// Reached the end of the directory?
			if (Regs.flags & 1U) {
				break;
			}

		} while (!uResult);

		// Properly close the directory
		// http://www.ctyme.com/intr/rb-3211.htm
		Regs.ax = 0x71A1;
		Regs.bx = sFileHandle;
		Burger::MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
	}

	return uResult;
}

/***************************************

	\brief Open a directory for scanning

	\param pDirName Pointer to the "C" string filename in BurgerLib format.

	\return Zero on success, non-zero on an error or if the directory doesn't
		exist

	\sa close(void) or get_next_entry(void)

***************************************/

Burger::eError Burger::DirectorySearch::open(Filename* pName) BURGER_NOEXCEPT
{
	// Make sure the cache is cleared
	close();

	// Get the DOS buffers
	uint32_t uRealBuffer = MSDos::get_temp_real_buffer();
	char* pProtected = static_cast<char*>(MSDos::get_temp_protected_buffer());

	// Get the real pathname and convert to DOS encoding
	const char* pPath = pName->get_native();
	uintptr_t i = Win437::translate_from_UTF8(pProtected + 512, 512, pPath);

	if (i && reinterpret_cast<const uint8_t*>(pPath)[i - 1] != '\\') {
		StringConcatenate(pProtected + 512, "\\");
	}
	StringConcatenate(pProtected + 512, "*.*");

	// Perform the action using the old or new APIs
	eError uResult = kErrorNotEnumerating;
	if (!FileManager::MSDOS_has_long_filenames()) {
		uResult = OldWay(&m_Entries);
	} else {
		uResult = NewWay(&m_Entries);
	}
	return uResult;
}

#endif
