/***************************************

	MSDOS version of Burger::File

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"

#if defined(BURGER_MSDOS)
#include "brfilemanager.h"
#include "brmemoryfunctions.h"
#include "brwin437.h"

#include "msdos_structs.h"
#include "msdos_memory.h"

/***************************************

	\brief Open a file using a \ref Filename

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a \ref Filename object
	\param uAccess Enumeration on permissions requested on the opened file

	\return \ref kErrorNone if no error, error code if not.

	\sa open(const char*, eFileAccess) and File(const char*, eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::open(
	Filename* pFileName, eFileAccess uAccess) BURGER_NOEXCEPT
{
	// Make sure the previous file is closed
	close();

	// 0 = Read only, 1 write only, 2 R/W
	static const uint16_t g_OpenAccess[4] = {0x00, 0x01, 0x01, 0x02};
	static const uint16_t g_CreateAction[4] = {1, 2 + 16, 1 + 16, 1 + 16};

	// Copy the filename to "Real" memory
	Win437::translate_from_UTF8(static_cast<char*>(MSDos::get_temp_protected_buffer()),
		512, pFileName->get_native());

	uAccess = static_cast<eFileAccess>(uAccess & 3);
	MSDos::Regs16_t Regs;
	uint32_t uTemp = MSDos::get_temp_real_buffer(); // Local buffer
	uint_t uResult = kErrorFileNotFound;
	// Are long filenames supported?
	if (!FileManager::MSDOS_has_long_filenames()) {
		// Use the old dos commands
		// Open preexisting file?

		uint16_t uCommand;
		if (uAccess == kWriteOnly) {
			// Create file
			uCommand = 0x3C00;
		} else {
			// Open file
			uCommand = 0x3D00 + g_OpenAccess[uAccess];
		}
		Regs.ax = uCommand;
		Regs.cx = 0;

		// Pass the filename buffer
		Regs.dx = static_cast<uint16_t>(uTemp);
		Regs.ds = static_cast<uint16_t>(uTemp >> 16);
		// Int 0x21,0x3D Open
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
		if (!(Regs.flags & 1)) {
			// Preexisting file was opened
			m_pFile = reinterpret_cast<void*>(Regs.ax);
			uResult = kErrorNone;
			if (uAccess == kAppend) {
				uResult = set_mark_at_EOF();
			}
		} else {
			if (uAccess != kReadOnly) {
				// Attempt to create the file Int 0x21,0x3C Create
				Regs.ax = 0x3C00;
				Regs.cx = 0;
				// Pass the filename buffer
				Regs.dx = static_cast<uint16_t>(uTemp);
				Regs.ds = static_cast<uint16_t>(uTemp >> 16);

				MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
				if (!(Regs.flags & 1)) {
					// File was created
					m_pFile = reinterpret_cast<void*>(Regs.ax);
					uResult = kErrorNone;
				}
			}
		}
	} else {
		Regs.ax = 0x716C;

		// 0x2000 = Return error code instead of Int 24h
		Regs.bx = g_OpenAccess[uAccess] + 0x2000;
		Regs.cx = 0;
		Regs.dx = g_CreateAction[uAccess];
		Regs.si = static_cast<uint16_t>(uTemp); // Pass the filename buffer
		Regs.ds = static_cast<uint16_t>(uTemp >> 16); // Get the segment
		Regs.di = 0;
		// Int 0x21,0x716C Open
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
		if (!(Regs.flags & 1)) {
			// File was opened
			m_pFile = reinterpret_cast<void*>(Regs.ax);
			uResult = kErrorNone;
			if (uAccess == kAppend) {
				uResult = set_mark_at_EOF();
			}
		}
	}
	return static_cast<eError>(uResult);
}

/***************************************

	\brief Close open file

	Close previously opened file. If there was no open file, this function
	performs no operation and returns \ref kErrorNone

	\return \ref kErrorNone if no error, error code if not.

	\sa open(const char*, eFileAccess) and open(Filename*, eFileAccess)

***************************************/

Burger::eError BURGER_API Burger::File::close(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x3E Close
		MSDos::Regs16_t Regs;
		Regs.ax = 0x3E00;
		Regs.bx = fp;
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
		if (Regs.flags & 1) {
			uResult = kErrorIO;
		}
		m_pFile = NULL;
	}
	return uResult;
}

/***************************************

	\brief Return the size of a file in bytes

	If a file is open, query the operating system for the size of the file
	in bytes.

	\note The return value is 64 bits wide, which may be larger than the
		amount of available memory on some systems.

	\return 0 if error or an empty file. Non-zero is the size of the file in
		bytes.

	\sa read(void*, uintptr_t) and write(const void*, uintptr_t)

***************************************/

uint64_t BURGER_API Burger::File::get_file_size(void) BURGER_NOEXCEPT
{
	MSDos::Regs16_t MyRegs;
	MSDos::Regs16_t MyRegsStore;
	MSDos::Regs16_t MyRegsSeek;
	uint64_t uSize = 0;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x4201 Seek/Current
		MyRegs.ax = 0x4201;
		MyRegs.bx = fp;
		MyRegs.cx = 0; // Offset
		MyRegs.dx = 0;
		MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegsStore);
		if (!(MyRegsStore.flags & 1)) {
			// Seek to the end to get the file size
			MyRegs.ax = 0x4202;
			MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegsSeek);
			if (!(MyRegsSeek.flags & 1)) {
				// Seek/Set
				MyRegs.ax = 0x4200;
				MyRegs.cx = MyRegsStore.ax;
				MyRegs.dx = MyRegsStore.dx;
				MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs);
				if (!(MyRegs.flags & 1)) {
					// Get the file size
					uSize = (MyRegsSeek.dx << 16U) + MyRegsSeek.ax;
				}
			}
		}
	}
	return uSize;
}

/***************************************

	\brief Read data from an open file

	If a file is open, perform a read operation. This function will fail
	if the file was not opened for read access.

	\param pOutput Pointer to a buffer of data to read from a file
	\param uSize Number of bytes to read

	\return Number of bytes read (Can be less than what was requested due to EOF
		or read errors)

	\sa write(const void*, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::read(void* pOutput, uintptr_t uSize)
{
	uintptr_t uResult = 0;
	if (uSize && pOutput) {
		int fp = reinterpret_cast<int>(m_pFile);
		if (fp) {
			uint32_t uTemp = MSDos::get_temp_real_buffer(); // Local buffer
			do {
				uint_t uChunk = uSize < 8192 ? uSize : 8192;
				MSDos::Regs16_t Regs;
				Regs.ax = 0x3F00;
				Regs.bx = fp;
				Regs.cx = static_cast<uint16_t>(uChunk);
				Regs.dx =
					static_cast<uint16_t>(uTemp); // Pass the filename buffer
				Regs.ds = static_cast<uint16_t>(uTemp >> 16); // Get the segment
				MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
				if (Regs.flags & 1) {
					break;
				}
				memory_copy(pOutput, MSDos::get_temp_protected_buffer(), uChunk);
				uResult += Regs.ax;
				uSize -= Regs.ax;
				pOutput = static_cast<char*>(pOutput) + Regs.ax;
			} while (uSize);
		}
	}
	return uResult;
}

/***************************************

	\brief Write data into an open file

	If a file is open, perform a write operation. This function will fail
	if the file was not opened for write access.

	\param pInput Pointer to a buffer of data to write to a file
	\param uSize Number of bytes to write

	\return Number of bytes written (Can be less than what was requested due to
		EOF or write errors)

	\sa read(void *, uintptr_t)

***************************************/

uintptr_t BURGER_API Burger::File::write(
	const void* pInput, uintptr_t uSize) BURGER_NOEXCEPT
{
	uintptr_t uResult = 0;
	if (uSize && pInput) {
		int fp = reinterpret_cast<int>(m_pFile);
		if (fp) {
			uint32_t uTemp = MSDos::get_temp_real_buffer(); // Local buffer
			do {
				uint_t uChunk = uSize < 8192 ? uSize : 8192;
				MSDos::Regs16_t Regs;
				Regs.ax = 0x4000;
				Regs.bx = fp;
				Regs.cx = static_cast<uint16_t>(uChunk);
				Regs.dx =
					static_cast<uint16_t>(uTemp); // Pass the filename buffer
				Regs.ds = static_cast<uint16_t>(uTemp >> 16); // Get the segment
				memory_copy(MSDos::get_temp_protected_buffer(), pInput, uChunk);
				MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
				if (Regs.flags & 1) {
					break;
				}
				uResult += Regs.ax;
				uSize -= Regs.ax;
				pInput = static_cast<const char*>(pInput) + Regs.ax;
			} while (uSize);
		}
	}
	return uResult;
}

/***************************************

	\brief Get the current file mark

	If a file is open, query the operating system for the location
	of the file mark for future reads or writes.

	\return Current file mark or zero if an error occurred

	\sa write(const void*, uintptr_t) or set_mark(uint64_t)

***************************************/

uint64_t BURGER_API Burger::File::get_mark(void) BURGER_NOEXCEPT
{
	MSDos::Regs16_t MyRegs;
	uint64_t uMark = 0;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x4201 Seek/Current
		MyRegs.ax = 0x4201;
		MyRegs.bx = fp;
		MyRegs.cx = 0; // Offset
		MyRegs.dx = 0;
		MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs);
		if (!(MyRegs.flags & 1)) {
			// Get the file size
			uMark = (MyRegs.dx << 16U) + MyRegs.ax;
		}
	}
	return uMark;
}

/***************************************

	\brief Set the current file mark

	If a file is open, set the read/write mark at the location passed.

	\param uMark Value to set the new file mark to.

	\return \ref kErrorNone if successful, \ref kErrorOutOfBounds if not.

	\sa get_mark() or set_mark_at_EOF()

***************************************/

Burger::eError BURGER_API Burger::File::set_mark(uint64_t uMark) BURGER_NOEXCEPT
{
	eError uResult = kErrorNotInitialized;
	MSDos::Regs16_t MyRegs;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x4200 Seek/Set
		MyRegs.ax = 0x4200;
		MyRegs.bx = fp;
		MyRegs.cx = static_cast<uint16_t>(uMark >> 16); // Offset
		MyRegs.dx = static_cast<uint16_t>(uMark);
		MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs);
		if (!(MyRegs.flags & 1)) {
			// Get the file size
			uResult = kErrorNone;
		} else {
			uResult = kErrorOutOfBounds;
		}
	}
	return uResult;
}

/***************************************

	\brief Set the current file mark at the end of the file

	If a file is open, set the read/write mark to the end of the file.

	\return \ref kErrorNone if successful, \ref kErrorOutOfBounds if not.

	\sa get_mark() or set_mark(uint64_t)

***************************************/

Burger::eError BURGER_API Burger::File::set_mark_at_EOF(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorOutOfBounds;
	MSDos::Regs16_t MyRegs;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x4202 Seek/End
		MyRegs.ax = 0x4202;
		MyRegs.bx = fp;
		MyRegs.cx = 0; // Offset
		MyRegs.dx = 0;
		MSDos::real_mode_interrupt(0x21, &MyRegs, &MyRegs);
		if (!(MyRegs.flags & 1)) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was last modified

	If a file is open, query the operating system for the last time
	the file was modified.

	\param pOutput Pointer to a \ref TimeDate_t to receive the file
		modification time

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa get_creation_time() or set_modification_time()

***************************************/

Burger::eError BURGER_API Burger::File::get_modification_time(
	TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	eError uResult = kErrorFileNotFound;
	MSDos::Regs16_t Regs;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x5700 Get File date time
		Regs.ax = 0x5700;
		Regs.bx = fp;
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
		if (!(Regs.flags & 1)) {
			pOutput->LoadMSDOS((Regs.dx << 16) + Regs.cx);
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was created

	If a file is open, query the operating system for the time the file was
	created.

	\param pOutput Pointer to a \ref TimeDate_t to receive the file creation
		time

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa get_modification_time() or set_creation_time()

***************************************/

Burger::eError BURGER_API Burger::File::get_creation_time(
	TimeDate_t* pOutput) BURGER_NOEXCEPT
{
	pOutput->Clear();
	return kErrorNotSupportedOnThisPlatform;
}

/***************************************

	\brief Set the time the file was last modified

	If a file is open, call the operating system to set the file
	modification time to the passed value.

	\param pInput Pointer to a \ref TimeDate_t to use for the new file
		modification time

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa get_modification_time() or set_creation_time()

***************************************/

Burger::eError BURGER_API Burger::File::set_modification_time(
	const TimeDate_t* pInput) BURGER_NOEXCEPT
{
	eError uResult = kErrorFileNotFound;
	MSDos::Regs16_t Regs;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		uint32_t uTime = pInput->StoreMSDOS();
		// Int 0x21,0x5701 Set File date time
		Regs.ax = 0x5701;
		Regs.bx = fp;
		Regs.cx = static_cast<uint16_t>(uTime);
		Regs.dx = static_cast<uint16_t>(uTime >> 16);
		MSDos::real_mode_interrupt(0x21, &Regs, &Regs);
		if (!(Regs.flags & 1)) {
			uResult = kErrorNone;
		}
	}
	return uResult;
}

/***************************************

	\brief Set the time the file was created

	If a file is open, call the operating system to set the file
	creation time to the passed value.

	\param pInput Pointer to a \ref TimeDate_t to use for the new file
		creation time

	\return \ref kErrorNone if successful, \ref kErrorNotSupportedOnThisPlatform
		if not available or other codes for errors

	\sa get_creation_time() or set_modification_time()

***************************************/

Burger::eError BURGER_API Burger::File::set_creation_time(
	const TimeDate_t* /* pInput */) BURGER_NOEXCEPT
{
	return kErrorNotSupportedOnThisPlatform;
}

#endif
