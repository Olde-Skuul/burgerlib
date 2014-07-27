/***************************************

	File Manager ANSI helper functions

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brfileansihelpers.h"
#include "brendian.h"
#include "brstringfunctions.h"
#include "brglobalmemorymanager.h"

#if defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || defined(BURGER_XBOX360)
#include <io.h>
#endif

#if defined(BURGER_MAC)
#include <Files.h>
#if defined(__MSL__)
#if !defined(_MSL_CANT_THROW)
#define _MSL_CANT_THROW
#endif
extern "C" char __msl_system_has_new_file_apis(void) _MSL_CANT_THROW;
#endif
#endif

/*! ************************************

	\brief Write a big endian 32-bit value to a file.

	Given an ANSI FILE *, write a 32-bit value in big endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param uInput Data to write.

	\sa Burger::WriteBig(FILE *,Word16) and Burger::WriteLittle(FILE *,Word32)

***************************************/

void BURGER_API Burger::WriteBig(FILE *fp,Word32 uInput)
{
	Word32 mValue = BigEndian::Load(uInput);
	fwrite(&mValue,4,1,fp);		// Save the long word
}

/*! ************************************

	\brief Write a big endian 16-bit value to a file.

	Given an ANSI FILE *, write a 16-bit value in big endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param uInput Data to write.

	\sa Burger::WriteBig(FILE *,Word32) and Burger::WriteLittle(FILE *,Word16)

***************************************/

void BURGER_API Burger::WriteBig(FILE *fp,Word16 uInput)
{
	Word16 mValue = BigEndian::Load(uInput);
	fwrite(&mValue,2,1,fp);		// Save the short word
}

/*! ************************************

	\brief Write a little endian 32-bit value to a file.

	Given an ANSI FILE *, write a 32-bit value in little endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param uInput Data to write.

	\sa Burger::WriteLittle(FILE *,Word16) and Burger::WriteBig(FILE *,Word32)

***************************************/

void BURGER_API Burger::WriteLittle(FILE *fp,Word32 uInput)
{
	Word32 mValue = LittleEndian::Load(uInput);
	fwrite(&mValue,4,1,fp);		// Save the long word
}

/*! ************************************

	\brief Write a little endian 16-bit value to a file.

	Given an ANSI FILE *, write a 16-bit value in little endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param uInput Data to write.

	\sa Burger::WriteLittle(FILE *,Word32) and Burger::WriteBig(FILE *,Word16)

***************************************/

void BURGER_API Burger::WriteLittle(FILE *fp,Word16 uInput)
{
	Word16 mValue = LittleEndian::Load(uInput);
	fwrite(&mValue,2,1,fp);		// Save the short word
}

/*! ************************************

	\brief Write a "C" string with the terminating zero to a file stream

	Given an ANSI FILE *, write a "C" string with a terminating zero
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param pInput Pointer to a "C" string to write.

	\sa Burger::ReadCString(FILE *,char *,WordPtr)

***************************************/

void BURGER_API Burger::WriteCString(FILE *fp,const char *pInput)
{
	// Write the string with the terminating zero.
	fwrite(pInput,StringLength(pInput)+1,1,fp);
}

/*! ************************************

	\brief Read a big endian 32-bit value from a file.

	Given an ANSI FILE *, read a 32-bit value in big endian format
	from the file stream.

	\param fp FILE * to the stream to read from.

	\sa Burger::ReadBigWord16(FILE *) and Burger::ReadLittleWord32(FILE *)

***************************************/

Word32 BURGER_API Burger::ReadBigWord32(FILE *fp)
{
	Word32 mValue;
	fread(&mValue,4,1,fp);		// Save the long word
	return BigEndian::Load(&mValue);
}

/*! ************************************

	\brief Read a big endian 16-bit value from a file.

	Given an ANSI FILE *, read a 16-bit value in big endian format
	from the file stream.

	\param fp FILE * to the stream to read from.

	\sa Burger::ReadBigWord32(FILE *) and Burger::ReadLittleWord16(FILE *)

***************************************/

Word16 BURGER_API Burger::ReadBigWord16(FILE *fp)
{
	Word16 mValue;
	fread(&mValue,2,1,fp);		// Save the short word
	return BigEndian::Load(&mValue);
}

/*! ************************************

	\brief Read a little endian 32-bit value from a file.

	Given an ANSI FILE *, read a 32-bit value in little endian format
	from the file stream.

	\param fp FILE * to the stream to read from.

	\sa Burger::ReadLittleWord16(FILE *) and Burger::ReadBigWord32(FILE *)

***************************************/

Word32 BURGER_API Burger::ReadLittleWord32(FILE *fp)
{
	Word32 mValue;
	fread(&mValue,4,1,fp);		// Save the long word
	return LittleEndian::Load(&mValue);
}

/*! ************************************

	\brief Read a little endian 16-bit value from a file.

	Given an ANSI FILE *, read a 16-bit value in little endian format
	from the file stream.

	\param fp FILE * to the stream to read from.

	\sa Burger::ReadLittleWord32(FILE *) and Burger::ReadBigWord16(FILE *)

***************************************/

Word16 BURGER_API Burger::ReadLittleWord16(FILE *fp)
{
	Word16 mValue;
	fread(&mValue,2,1,fp);		// Save the long word
	return LittleEndian::Load(&mValue);
}

/*! ************************************

	\brief Read a "C" string with the terminating zero to a file stream

	Given an ANSI FILE *, read a "C" string with a terminating zero
	into the file stream. If the string read is larger than the buffer,
	it is truncated. The buffer will have an ending zero on valid 
	read or a truncated read. If uLength was zero, then pInput can be \ref NULL

	\param fp FILE * to the stream to write to.
	\param pInput Pointer to a "C" string to write.
	\param uLength Size of the buffer (To prevent overruns)

	\return TRUE if the string was read, FALSE, hit EOF

	\sa Burger::WriteCString(FILE *,const char *)

***************************************/

Word BURGER_API Burger::ReadCString(FILE *fp,char *pInput,WordPtr uLength)
{
	// Set the maximum buffer size
	// and remove 1 to make space or the ending zero
	char *pEnd = (pInput+uLength)-1;
	Word uTemp;
	for (;;) {		// Stay until either zero or EOF
		uTemp = static_cast<Word>(fgetc(fp));			// Get the char from the file
		if (!uTemp || uTemp==static_cast<Word>(EOF)) {	// Exit due to end of string?
			break;
		}
		// Can I store it?
		if (pInput<pEnd) {
			pInput[0] = static_cast<char>(uTemp);
			++pInput;		// Inc the input pointer
		}
	}
	if (uLength) {		// Any space in buffer?
		pInput[0] = 0;	// Add ending zero
	}
	if (uTemp) {		// EOF?
		return FALSE;	// Hit the EOF.
	}
	return TRUE;	// Hit the end of string (More data may be present)
}

/*! ************************************

	\brief Return the size of a file.

	Given an ANSI FILE *, return the size of the file.

	\param fp FILE * to the stream to write to.
	
	\note On 32 bit platforms, if the file is larger than 4GB,
	the size will be returned as 0xFFFFFFFF. On 64 bit platforms
	the size will be returned with 64 bit precision.

	This function is faster than the fseek(), ftell(seek),
	fseek() method since this uses custom code tailored for each
	compiler to directly access records in the FILE structure in
	most cases.

	\return The size of the file in bytes, or zero on error.

***************************************/

#if defined(BURGER_XBOX360) || defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || defined(DOXYGEN)

WordPtr BURGER_API Burger::GetSize(FILE *fp)
{
#if defined(BURGER_WIN64) || defined(BURGER_XBOX360)
	// 64 bit windows allows a full 64 bit length
	Int64 lLength = _filelengthi64(fileno(fp));		// 64 bit version
	if (lLength==-1LL) {							// Error?
		lLength = 0;
	}
	return static_cast<WordPtr>(lLength);			// I got it.
#else
	// DOS/Windows allows a direct query to the data stream
	long lLength = filelength(fileno(fp));	// Fast version for INTEL
	if (lLength==-1) {						// Error?
		lLength = 0;
	}
	return static_cast<Word32>(lLength);	// I got it.
#endif
}

//
// MetroWerks for MacOS version (PPC and 68K)
// Goes straight into the MSL structure and then called MacOS
// with 64 bit support.
//
// Note: The APIS flags are defined in ansi_prefix.mac.h
//

#elif defined(BURGER_MAC) && defined(__MSL__)

WordPtr BURGER_API Burger::GetSize(FILE *fp)
{	
	WordPtr uResult = 0;

#if _MSL_USE_OLD_AND_NEW_FILE_APIS
	// Do I support old and new calls?
	if (__msl_system_has_new_file_apis()) {
#endif
#if _MSL_USE_NEW_FILE_APIS
		// Get the file size 64 bit aware.
		FSForkIOParam ParmBlock;
		ParmBlock.ioCompletion = 0;
		ParmBlock.forkRefNum = static_cast<short>(fp->handle);
		PBGetForkSizeSync(&ParmBlock);
		// Did I get it?
		if (ParmBlock.ioResult == noErr) {
			// Is the file larger that 32-bits?
			if (ParmBlock.positionOffset>=0x100000000LL) {
				--uResult;		// -1
			} else {
				// Return the file size
				uResult = static_cast<WordPtr>(ParmBlock.positionOffset);
			}
		}
#endif
#if _MSL_USE_OLD_AND_NEW_FILE_APIS
	} else {
#endif
#if _MSL_USE_OLD_FILE_APIS
	// The 68k and Pre-MacOS 9 systems
		ParamBlockRec IoBlock;
		IoBlock.ioParam.ioCompletion = 0;
		IoBlock.ioParam.ioRefNum = static_cast<short>(fp->handle);
	
		PBGetEOFSync(&IoBlock);
		if (IoBlock.ioParam.ioResult == noErr) {
			uResult = reinterpret_cast<WordPtr>(IoBlock.ioParam.ioMisc);
		}
#endif
#if _MSL_USE_OLD_AND_NEW_FILE_APIS
	}
#endif
	return uResult;
}

//
// MacOSX version (Cocoa)
//

#elif defined(BURGER_MACOSX)

WordPtr BURGER_API Burger::GetSize(FILE *fp)
{
	long Mark = ftell(fp);		// Get the current position
	fseek(fp,0,SEEK_END);		// Seek to the end of the file
	long Length = ftell(fp);
	WordPtr uResult=0;
	if (Length>=0) {
		uResult = static_cast<WordPtr>(Length);
	}
	fseek(fp,Mark,SEEK_SET);
	return uResult;
}

#else

//
// Generic operating system version
//

WordPtr BURGER_API Burger::GetSize(FILE *fp)
{
	long Mark = ftell(fp);		// Get the current position
	fseek(fp,0,SEEK_END);		// Seek to the end of the file
	long Length = ftell(fp);		// Where's the EOF?
	fseek(fp,Mark,SEEK_SET);	// Restore the file position
	return static_cast<WordPtr>(Length);	// Return the file size
}

#endif

/*! ************************************

	\brief Save memory to a file opened with fopen()

	Given an ANSI FILE * for a freshly opened file with write
	permissions, write the chunk of memory to the file 
	and close the file. The passed pointer is considered invalid
	and closed after this call is issued.

	\param fp FILE * to the file to write to
	\param pInput Pointer to the memory to write to
	\param uLength Number of bytes to write

	\return Zero if failure, non-zero on success
	\sa Burger::LoadFile(FILE *,WordPtr *)

***************************************/

Word BURGER_API Burger::SaveFile(FILE *fp,const void *pInput,WordPtr uLength)
{
	Word uResult = FALSE;		// Assume failure
	if (fp) {					// Valid file structure?
		WordPtr uNewLength = fwrite(pInput,1,uLength,fp);	// Write the file contents
		int Err = fclose(fp);	// Close the file
		if ((uNewLength==uLength) && (!Err)) {	// Error?
			uResult = TRUE;		// It's ok!
		}
	}
	return uResult;
}

/*! ************************************

	\brief Load a file opened with fopen() into memory

	Given an ANSI FILE * for a freshly opened file with read
	permissions, allocate enough memory to accommodate the entire
	file and read it into memory. The file is closed
	at the end of the function. The passed fp pointer is considered invalid
	and closed after this call is issued.

	Dispose of the memory returned with a call to \ref Burger::Free(const void *)

	\param fp FILE * to the file to read from
	\param pLength to a \ref WordPtr to receive the size of the file in bytes.

	\return \ref NULL if failure, a pointer on success

	\note If the file is empty, it's considered a failure and a \ref NULL is returned.

	\sa Burger::SaveFile(FILE *,const void *,WordPtr)
	
***************************************/

void * BURGER_API Burger::LoadFile(FILE *fp,WordPtr *pLength)
{
	void *pResult = NULL;		// Init result
	WordPtr uFileLength = 0;	// Init length

	if (fp) {		// Valid file reference?
		uFileLength = GetSize(fp);		// Get the file length
		if (uFileLength) {
			pResult = Alloc(uFileLength);	// Get memory
			if (pResult) {
				if (fread(pResult,1,uFileLength,fp)!=uFileLength) {		// Read the file contents
					// Oh. Crap.
					Free(pResult);			// Release the memory
					pResult = NULL;				// Zap the pointer
					uFileLength = 0;
				}
			}
		}
		fclose(fp);		// Close the file
	}
	if (pLength) {		// Shall I return the file length? */
		pLength[0] = uFileLength;	// Ok
	}
	return pResult;		// Return the result
}
