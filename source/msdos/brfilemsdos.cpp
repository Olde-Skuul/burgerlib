/***************************************

	MSDOS version of Burger::File

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfile.h"

#if defined(BURGER_MSDOS)
#include "brdosextender.h"
#include "brfilemanager.h"

/***************************************

	\brief Open a file using a Burger::Filename

	Close any previously opened file and open a new file.

	\param pFileName Pointer to a Burger::Filename object
	\param eAccess Enumeration on permissions requested on the opened file
	\return File::OKAY if no error, error code if not.
	\sa Open(const char *, eFileAccess) and File(const char *,eFileAccess)

***************************************/

Word BURGER_API Burger::File::Open(Filename *pFileName,eFileAccess eAccess)
{
	static const Word16 g_OpenAccess[4] = { 0x00,0x01,0x01,0x02 };
	static const Word16 g_CreateAction[4] = {1,2+16,1+16,1+16};
	Close();
	// Copy the filename to "Real" memory
	StringCopy(static_cast<char *>(GetRealBufferProtectedPtr()),pFileName->GetNative());

	eAccess = static_cast<eFileAccess>(eAccess&3);
	Regs16 Regs;
	Word32 uTemp = GetRealBufferPtr();			// Local buffer
	Word uResult = FILENOTFOUND;
	// Are long filenames supported?
	if (!FileManager::AreLongFilenamesAllowed()) {
		// Use the old dos commands
		// Open preexisting file?
		Regs.ax = g_OpenAccess[eAccess]+0x3D00;
		Regs.dx = static_cast<Word16>(uTemp);		// Pass the filename buffer
		Regs.ds = static_cast<Word16>(uTemp>>16);	// Get the segment
		// Int 0x21,0x3D Open
		Int86x(0x21,&Regs,&Regs);
		if (!(Regs.flags&1)) {
			// Preexisting file was opened
			m_pFile = reinterpret_cast<void *>(Regs.ax);
			uResult = OKAY;
			if (eAccess==APPEND) {
				uResult= SetMarkAtEOF();
			}
		} else {
			if (eAccess!=READONLY) {
				// Attempt to create the file Int 0x21,0x3C Create
				Regs.ax = 0x3C00;
				Regs.cx = 0;
				Regs.dx = static_cast<Word16>(uTemp);		// Pass the filename buffer
				Regs.ds = static_cast<Word16>(uTemp>>16);	// Get the segment
				Int86x(0x21,&Regs,&Regs);
				if (!(Regs.flags&1)) {
					// File was created
					m_pFile = reinterpret_cast<void *>(Regs.ax);
					uResult = OKAY;
				}
			}
		}
	} else {
		Regs.ax = 0x716C;
		Regs.bx = g_OpenAccess[eAccess]+0x2000;	// 0x2000 = Return error code instead of Int 24h
		Regs.cx = 0;
		Regs.dx = g_CreateAction[eAccess];
		Regs.si = static_cast<Word16>(uTemp);		// Pass the filename buffer
		Regs.ds = static_cast<Word16>(uTemp>>16);	// Get the segment
		Regs.di = 0;
		// Int 0x21,0x716C Open
		Int86x(0x21,&Regs,&Regs);
		if (!(Regs.flags&1)) {
			// File was opened
			m_pFile = reinterpret_cast<void *>(Regs.ax);
			uResult = OKAY;
			if (eAccess==APPEND) {
				uResult= SetMarkAtEOF();
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Close any open file

	Close any previously opened file

	\return File::OKAY if no error, error code if not.
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

Word BURGER_API Burger::File::Close(void)
{
	Word uResult = OKAY;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x3E Close
		Regs16 Regs;
		Regs.ax = 0x3E00;
		Regs.bx = fp;
		Int86x(0x21,&Regs,&Regs);
		if (Regs.flags&1) {
			uResult = IOERROR;
		}
		m_pFile = NULL;
	}
	return uResult;
}

/***************************************

	\brief Return the size of a file in bytes

	If a file is open, query the operating system for the size of the file
	in bytes.

	\note The return value is 32 bits wide on a 32 bit operating system, 64 bits
		wide on 64 bit operating systems
	\return 0 if error or an empty file. Non-zero is the size of the file in bytes. 
	\sa Open(const char *, eFileAccess) and Open(Filename *,eFileAccess)

***************************************/

WordPtr BURGER_API Burger::File::GetSize(void)
{
	Regs16 MyRegs;
	Regs16 MyRegsStore;
	Regs16 MyRegsSeek;
	WordPtr uSize = 0;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x4201 Seek/Current
		MyRegs.ax = 0x4201;
		MyRegs.bx = fp;
		MyRegs.cx = 0;		// Offset
		MyRegs.dx = 0;
		Int86x(0x21,&MyRegs,&MyRegsStore);
		if (!(MyRegsStore.flags&1)) {
			// Seek to the end to get the file size
			MyRegs.ax = 0x4202;
			Int86x(0x21,&MyRegs,&MyRegsSeek);
			if (!(MyRegsSeek.flags&1)) {
				// Seek/Set
				MyRegs.ax = 0x4200;
				MyRegs.cx = MyRegsStore.ax;
				MyRegs.dx = MyRegsStore.dx;
				Int86x(0x21,&MyRegs,&MyRegs);
				if (!(MyRegs.flags&1)) {
					// Get the file size
					uSize = (MyRegsSeek.dx<<16U)+MyRegsSeek.ax;
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
	\return Number of bytes read (Can be less than what was requested due to EOF or read errors)
	\sa Write(const void *,WordPtr)

***************************************/

WordPtr BURGER_API Burger::File::Read(void *pOutput,WordPtr uSize)
{
	WordPtr uResult = 0;
	if (uSize && pOutput) {
		int fp = reinterpret_cast<int>(m_pFile);
		if (fp) {
			Word32 uTemp = GetRealBufferPtr();			// Local buffer
			do {
				Word uChunk = uSize<8192 ? uSize : 8192;
				Regs16 Regs;
				Regs.ax = 0x3F00;
				Regs.bx = fp;
				Regs.cx = static_cast<Word16>(uChunk);
				Regs.dx = static_cast<Word16>(uTemp);		// Pass the filename buffer
				Regs.ds = static_cast<Word16>(uTemp>>16);	// Get the segment
				Int86x(0x21,&Regs,&Regs);
				if (Regs.flags&1) {
					break;
				}
				MemoryCopy(pOutput,GetRealBufferProtectedPtr(),uChunk);
				uResult += Regs.ax;
				uSize -= Regs.ax;
				pOutput = static_cast<char *>(pOutput)+Regs.ax;
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
	\return Number of bytes written (Can be less than what was requested due to EOF or write errors)
	\sa Read(void *,WordPtr)

***************************************/

WordPtr BURGER_API Burger::File::Write(const void *pInput,WordPtr uSize)
{
	WordPtr uResult = 0;
	if (uSize && pInput) {
		int fp = reinterpret_cast<int>(m_pFile);
		if (fp) {
			Word32 uTemp = GetRealBufferPtr();			// Local buffer
			do {
				Word uChunk = uSize<8192 ? uSize : 8192;
				Regs16 Regs;
				Regs.ax = 0x4000;
				Regs.bx = fp;
				Regs.cx = static_cast<Word16>(uChunk);
				Regs.dx = static_cast<Word16>(uTemp);		// Pass the filename buffer
				Regs.ds = static_cast<Word16>(uTemp>>16);	// Get the segment
				MemoryCopy(GetRealBufferProtectedPtr(),pInput,uChunk);
				Int86x(0x21,&Regs,&Regs);
				if (Regs.flags&1) {
					break;
				}
				uResult += Regs.ax;
				uSize -= Regs.ax;
				pInput = static_cast<const char *>(pInput)+Regs.ax;
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
	\sa Write(const void *,WordPtr)

***************************************/

WordPtr BURGER_API Burger::File::GetMark(void)
{
	Regs16 MyRegs;
	WordPtr uMark = 0;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x4201 Seek/Current
		MyRegs.ax = 0x4201;
		MyRegs.bx = fp;
		MyRegs.cx = 0;		// Offset
		MyRegs.dx = 0;
		Int86x(0x21,&MyRegs,&MyRegs);
		if (!(MyRegs.flags&1)) {
			// Get the file size
			uMark = (MyRegs.dx<<16U)+MyRegs.ax;
		}
	}
	return uMark;
}

/***************************************

	\brief Set the current file mark

	If a file is open, set the read/write mark at the location passed.

	\param uMark Value to set the new file mark to.
	\return File::OKAY if successful, File::INVALID_MARK if not.
	\sa GetMark() or SetMarkAtEOF()

***************************************/

Word BURGER_API Burger::File::SetMark(WordPtr uMark)
{
	Word uResult = INVALID_MARK;
	Regs16 MyRegs;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x4200 Seek/Set
		MyRegs.ax = 0x4200;
		MyRegs.bx = fp;
		MyRegs.cx = static_cast<Word16>(uMark>>16);		// Offset
		MyRegs.dx = static_cast<Word16>(uMark);
		Int86x(0x21,&MyRegs,&MyRegs);
		if (!(MyRegs.flags&1)) {
			// Get the file size
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Set the current file mark at the end of the file

	If a file is open, set the read/write mark to the end of the file.

	\return File::OKAY if successful, File::INVALID_MARK if not.
	\sa GetMark() or SetMark()

***************************************/

Word BURGER_API Burger::File::SetMarkAtEOF(void)
{
	Word uResult = INVALID_MARK;
	Regs16 MyRegs;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x4202 Seek/End
		MyRegs.ax = 0x4202;
		MyRegs.bx = fp;
		MyRegs.cx = 0;		// Offset
		MyRegs.dx = 0;
		Int86x(0x21,&MyRegs,&MyRegs);
		if (!(MyRegs.flags&1)) {
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was last modified

	If a file is open, query the operating system for the last time
	the file was modified.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file modification time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa GetCreationTime() or SetModificationTime()

***************************************/

Word BURGER_API Burger::File::GetModificationTime(TimeDate_t *pOutput)
{
	Word uResult = FILENOTFOUND;
	Regs16 Regs;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		// Int 0x21,0x5700 Get File date time
		Regs.ax = 0x5700;
		Regs.bx = fp;
		Int86x(0x21,&Regs,&Regs);
		if (!(Regs.flags&1)) {
			pOutput->LoadMSDOS((Regs.dx<<16)+Regs.cx);
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Get the time the file was created

	If a file is open, query the operating system for the time
	the file was created.

	\param pOutput Pointer to a Burger::TimeDate_t to receive the file creation time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa GetModificationTime() or SetCreationTime()

***************************************/

Word BURGER_API Burger::File::GetCreationTime(TimeDate_t *pOutput)
{
	pOutput->Clear();
	return NOT_IMPLEMENTED;
}

/***************************************

	\brief Set the time the file was last modified

	If a file is open, call the operating system to set the file
	modification time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file modification time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa SetCreationTime() or GetModificationTime()

***************************************/

Word BURGER_API Burger::File::SetModificationTime(const TimeDate_t *pInput)
{
	Word uResult = FILENOTFOUND;
	Regs16 Regs;
	int fp = reinterpret_cast<int>(m_pFile);
	if (fp) {
		Word32 uTime = pInput->StoreMSDOS();
		// Int 0x21,0x5701 Set File date time
		Regs.ax = 0x5701;
		Regs.bx = fp;
		Regs.cx = static_cast<Word16>(uTime);
		Regs.dx = static_cast<Word16>(uTime>>16);
		Int86x(0x21,&Regs,&Regs);
		if (!(Regs.flags&1)) {
			uResult = OKAY;
		}
	}
	return uResult;
}

/***************************************

	\brief Set the time the file was created

	If a file is open, call the operating system to set the file
	creation time to the passed value.

	\param pInput Pointer to a Burger::TimeDate_t to use for the new file creation time
	\return File::OKAY if successful, File::NOT_IMPLEMENTED if not available or other codes for errors
	\sa SetModificationTime() or GetCreationTime()

***************************************/

Word BURGER_API Burger::File::SetCreationTime(const TimeDate_t * /* pInput */)
{
	return NOT_IMPLEMENTED;
}

#endif