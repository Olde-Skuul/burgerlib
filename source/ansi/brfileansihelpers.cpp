/***************************************

	File Manager ANSI helper functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#define __useAppleExts__

#include "brfileansihelpers.h"
#include "brendian.h"
#include "brfloatingpoint.h"
#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"
#include <errno.h>

//
// Used for GetSize() functions
//

#if defined(BURGER_WINDOWS) || defined(BURGER_MSDOS) || defined(BURGER_XBOX360)
#include <fcntl.h>
#include <io.h>
#if defined(BURGER_METROWERKS)
#include <crtl.h>
#include <windows.h>
#endif
#endif

#if defined(BURGER_MAC)
#include <Files.h>
#if defined(__MSL__)
#if !defined(_MSL_CANT_THROW)
#define _MSL_CANT_THROW
#endif
extern "C" char __msl_system_has_new_file_apis(void) _MSL_CANT_THROW;
#else
#include <fcntl.h>
#include <stdio.h>
#endif
#endif

/*! ************************************

	\brief Write a big endian 16-bit integer to a file.

	Given an ANSI FILE *, write a 16-bit integer in big endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param uInput Data to write.
	\return Zero if no error, "errno" on error

	\sa WriteLittle(FILE *,uint16_t), WriteBig(FILE *,uint32_t) and
WriteBig(FILE
*,uint64_t)

***************************************/

uint_t BURGER_API Burger::WriteBig(FILE* fp, uint16_t uInput)
{
	uint16_t Value;
	// Store as big endian
	BigEndian::Store(&Value, uInput);
	uint_t uResult = 0;
	if (fwrite(&Value, 1, 2, fp) != 2) { // Save the short word
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Write a big endian 32-bit integer to a file.

	Given an ANSI FILE *, write a 32-bit integer in big endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param uInput Data to write.
	\return Zero if no error, "errno" on error

	\sa WriteLittle(FILE *,uint32_t), WriteBig(FILE *,uint16_t) and
WriteBig(FILE
*,uint64_t)

***************************************/

uint_t BURGER_API Burger::WriteBig(FILE* fp, uint32_t uInput)
{
	uint32_t Value;
	// Store as big endian
	BigEndian::Store(&Value, uInput);
	uint_t uResult = 0;
	if (fwrite(&Value, 1, 4, fp) != 4) { // Save the long word
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Write a big endian 64-bit integer to a file.

	Given an ANSI FILE *, write a 64-bit integer in big endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param uInput Data to write.
	\return Zero if no error, "errno" on error

	\sa WriteLittle(FILE *,uint64_t), WriteBig(FILE *,uint16_t) and
WriteBig(FILE
*,uint32_t)

***************************************/

uint_t BURGER_API Burger::WriteBig(FILE* fp, uint64_t uInput)
{
	uint64_t Value;
	// Store as big endian
	BigEndian::Store(&Value, uInput);
	uint_t uResult = 0;
	if (fwrite(&Value, 1, 8, fp) != 8) { // Save the 64 bit word
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Write a big endian 32-bit float to a file.

	Given an ANSI FILE *, write a 32-bit float in big endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param fInput Data to write.
	\return Zero if no error, "errno" on error

	\sa WriteLittle(FILE *,float) or WriteBig(FILE *,double)

***************************************/

uint_t BURGER_API Burger::WriteBig(FILE* fp, float fInput)
{
	float Value;
	// Store as big endian
	BigEndian::Store(&Value, fInput);
	uint_t uResult = 0;
	if (fwrite(&Value, 1, 4, fp) != 4) { // Save the 32 bit float
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Write a big endian 64-bit float to a file.

	Given an ANSI FILE *, write a 64-bit float in big endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param dInput Data to write.
	\return Zero if no error, "errno" on error

	\sa WriteLittle(FILE *,double) or WriteBig(FILE *,float)

***************************************/

uint_t BURGER_API Burger::WriteBig(FILE* fp, double dInput)
{
	double Value;
	// Store as big endian
	BigEndian::Store(&Value, dInput);
	uint_t uResult = 0;
	if (fwrite(&Value, 1, 8, fp) != 8) { // Save the 64 bit float
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Write a little endian 16-bit integer to a file.

	Given an ANSI FILE *, write a 16-bit integer in little endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param uInput Data to write.
	\return Zero if no error, "errno" on error

	\sa WriteBig(FILE *,uint16_t), WriteLittle(FILE *,uint32_t) and
WriteLittle(FILE
*,uint64_t)

***************************************/

uint_t BURGER_API Burger::WriteLittle(FILE* fp, uint16_t uInput)
{
	uint16_t Value;
	// Store as little endian
	LittleEndian::Store(&Value, uInput);
	uint_t uResult = 0;
	if (fwrite(&Value, 1, 2, fp) != 2) { // Save the short word
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Write a little endian 32-bit integer to a file.

	Given an ANSI FILE *, write a 32-bit integer in little endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param uInput Data to write.
	\return Zero if no error, "errno" on error

	\sa WriteBig(FILE *,uint32_t), WriteLittle(FILE *,uint16_t) and
WriteLittle(FILE
*,uint64_t)

***************************************/

uint_t BURGER_API Burger::WriteLittle(FILE* fp, uint32_t uInput)
{
	uint32_t Value;
	// Store as little endian
	LittleEndian::Store(&Value, uInput);
	uint_t uResult = 0;
	if (fwrite(&Value, 1, 4, fp) != 4) { // Save the long word
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Write a little endian 64-bit integer to a file.

	Given an ANSI FILE *, write a 64-bit integer in little endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param uInput Data to write.
	\return Zero if no error, "errno" on error

	\sa WriteBig(FILE *,uint64_t), WriteLittle(FILE *,uint16_t) and
WriteLittle(FILE
*,uint32_t)

***************************************/

uint_t BURGER_API Burger::WriteLittle(FILE* fp, uint64_t uInput)
{
	uint64_t Value;
	// Store as little endian
	LittleEndian::Store(&Value, uInput);
	uint_t uResult = 0;
	if (fwrite(&Value, 1, 8, fp) != 8) { // Save the 64 bit word
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Write a little endian 32-bit float to a file.

	Given an ANSI FILE *, write a 32-bit float in little endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param fInput Data to write.
	\return Zero if no error, "errno" on error

	\sa WriteBig(FILE *,float) or WriteLittle(FILE *,double)

***************************************/

uint_t BURGER_API Burger::WriteLittle(FILE* fp, float fInput)
{
	float Value;
	// Store as little endian
	LittleEndian::Store(&Value, fInput);
	uint_t uResult = 0;
	if (fwrite(&Value, 1, 4, fp) != 4) { // Save the 32 bit float
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Write a little endian 64-bit float to a file.

	Given an ANSI FILE *, write a 64-bit float in little endian format
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param dInput Data to write.
	\return Zero if no error, "errno" on error

	\sa WriteBig(FILE *,double) or WriteLittle(FILE *,float)

***************************************/

uint_t BURGER_API Burger::WriteLittle(FILE* fp, double dInput)
{
	double Value;
	// Store as little endian
	LittleEndian::Store(&Value, dInput);
	uint_t uResult = 0;
	if (fwrite(&Value, 1, 8, fp) != 8) { // Save the 64 bit float
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Write a "C" string with the terminating zero to a file stream

	Given an ANSI FILE *, write a "C" string with a terminating zero
	into the file stream.

	\param fp FILE * to the stream to write to.
	\param pInput Pointer to a "C" string to write.
	\return Zero if no error, "errno" on error

	\sa read_c_string(FILE *,char *,uintptr_t)

***************************************/

uint_t BURGER_API Burger::WriteCString(FILE* fp, const char* pInput)
{
	// Write the string with the terminating zero.
	uint_t uResult = 0;
	uintptr_t uLength = StringLength(pInput) + 1;
	if (fwrite(pInput, 1, uLength, fp) != uLength) {
		uResult = static_cast<uint_t>(errno);
	}
	return uResult;
}

/*! ************************************

	\brief Read a big endian 16-bit integer from a file.

	Given an ANSI FILE *, read a 16-bit integer in big endian format
	from the file stream.

	\param fp FILE * to the stream to read from.
	\return Zero on error or integer from the data stream

	\sa read_little_uint16(FILE *), read_big_uint32(FILE *) or ReadBigWord64(FILE *)

***************************************/

uint16_t BURGER_API Burger::read_big_uint16(FILE* fp)
{
	uint16_t uResult = 0;
	uint16_t Value;
	if (fread(&Value, 1, 2, fp) == 2) {
		uResult = BigEndian::Load(&Value);
	}
	return uResult;
}

/*! ************************************

	\brief Read a big endian 32-bit integer from a file.

	Given an ANSI FILE *, read a 32-bit integer in big endian format
	from the file stream.

	\param fp FILE * to the stream to read from.
	\return Zero on error or integer from the data stream

	\sa read_little_uint32(FILE *), read_big_uint16(FILE *) or ReadBigWord64(FILE *)

***************************************/

uint32_t BURGER_API Burger::read_big_uint32(FILE* fp)
{
	uint32_t uResult = 0;
	uint32_t Value;
	if (fread(&Value, 1, 4, fp) == 4) {
		uResult = BigEndian::Load(&Value);
	}
	return uResult;
}

/*! ************************************

	\brief Read a big endian 64-bit integer from a file.

	Given an ANSI FILE *, read a 64-bit integer in big endian format
	from the file stream.

	\param fp FILE * to the stream to read from.
	\return Zero on error or integer from the data stream

	\sa ReadLittleWord64(FILE *), read_big_uint16(FILE *) or ReadBigWord64(FILE *)

***************************************/

uint64_t BURGER_API Burger::ReadBigWord64(FILE* fp)
{
	uint64_t uResult = 0;
	uint64_t Value;
	if (fread(&Value, 1, 8, fp) == 8) {
		uResult = BigEndian::Load(&Value);
	}
	return uResult;
}

/*! ************************************

	\brief Read a big endian 32-bit float from a file.

	Given an ANSI FILE *, read a 32-bit float in big endian format
	from the file stream.

	\param fp FILE * to the stream to read from.
	\return NaN on error or float from the data stream

	\sa ReadLittleFloat(FILE *) or ReadBigDouble(FILE *)

***************************************/

float BURGER_API Burger::ReadBigFloat(FILE* fp)
{
	float fResult;
	float Value;
	if (fread(&Value, 1, 4, fp) != 4) {
		fResult = g_fNan;
	} else {
		fResult = BigEndian::Load(&Value);
	}
	return fResult;
}

/*! ************************************

	\brief Read a big endian 64-bit float from a file.

	Given an ANSI FILE *, read a 64-bit float in big endian format
	from the file stream.

	\param fp FILE * to the stream to read from.
	\return NaN on error or double from the data stream

	\sa ReadLittleDouble(FILE *), ReadBigFloat(FILE *)

***************************************/

double BURGER_API Burger::ReadBigDouble(FILE* fp)
{
	double dResult;
	double Value;
	if (fread(&Value, 1, 8, fp) != 8) {
		dResult = g_dNan;
	} else {
		dResult = BigEndian::Load(&Value);
	}
	return dResult;
}

/*! ************************************

	\brief Read a little endian 16-bit integer from a file.

	Given an ANSI FILE *, read a 16-bit integer in little endian format
	from the file stream.

	\param fp FILE * to the stream to read from.
	\return Zero on error or integer from the data stream

	\sa read_big_uint16(FILE *), read_little_uint32(FILE *) or 
		ReadLittleWord64(FILE *)

***************************************/

uint16_t BURGER_API Burger::read_little_uint16(FILE* fp)
{
	uint16_t uResult = 0;
	uint16_t Value;
	if (fread(&Value, 1, 2, fp) == 2) {
		uResult = LittleEndian::Load(&Value);
	}
	return uResult;
}

/*! ************************************

	\brief Read a little endian 32-bit integer from a file.

	Given an ANSI FILE *, read a 32-bit integer in little endian format
	from the file stream.

	\param fp FILE * to the stream to read from.
	\return Zero on error or integer from the data stream

	\sa read_big_uint32(FILE *), read_little_uint16(FILE *) or ReadLittleWord64(FILE
*)

***************************************/

uint32_t BURGER_API Burger::read_little_uint32(FILE* fp)
{
	uint32_t uResult = 0;
	uint32_t Value;
	if (fread(&Value, 1, 4, fp) == 4) {
		uResult = LittleEndian::Load(&Value);
	}
	return uResult;
}

/*! ************************************

	\brief Read a little endian 64-bit integer from a file.

	Given an ANSI FILE *, read a 64-bit integer in little endian format
	from the file stream.

	\param fp FILE * to the stream to read from.
	\return Zero on error or integer from the data stream

	\sa ReadBigWord64(FILE *), read_little_uint16(FILE *) or 
		read_little_uint32(FILE *)

***************************************/

uint64_t BURGER_API Burger::ReadLittleWord64(FILE* fp)
{
	uint64_t uResult = 0;
	uint64_t Value;
	if (fread(&Value, 1, 8, fp) == 8) {
		uResult = LittleEndian::Load(&Value);
	}
	return uResult;
}

/*! ************************************

	\brief Read a little endian 32-bit float from a file.

	Given an ANSI FILE *, read a 32-bit float in little endian format
	from the file stream.

	\param fp FILE * to the stream to read from.
	\return NaN on error or float from the data stream

	\sa ReadBigFloat(FILE *) or ReadLittleDouble(FILE *)

***************************************/

float BURGER_API Burger::ReadLittleFloat(FILE* fp)
{
	float fResult;
	float Value;
	if (fread(&Value, 1, 4, fp) != 4) {
		fResult = g_fNan;
	} else {
		fResult = LittleEndian::Load(&Value);
	}
	return fResult;
}

/*! ************************************

	\brief Read a little endian 64-bit float from a file.

	Given an ANSI FILE *, read a 64-bit float in little endian format
	from the file stream.

	\param fp FILE * to the stream to read from.
	\return NaN on error or double from the data stream

	\sa ReadBigDouble(FILE *) or ReadLittleFloat(FILE *)

***************************************/

double BURGER_API Burger::ReadLittleDouble(FILE* fp)
{
	double dResult;
	double Value;
	if (fread(&Value, 1, 8, fp) != 8) {
		dResult = g_dNan;
	} else {
		dResult = LittleEndian::Load(&Value);
	}
	return dResult;
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

	\return \ref TRUE if the string was read, \ref FALSE, hit EOF

	\sa WriteCString(FILE *,const char *)

***************************************/

uint_t BURGER_API Burger::read_c_string(FILE* fp, char* pInput, uintptr_t uLength)
{
	// Set the maximum buffer size
	// and remove 1 to make space or the ending zero
	int iTemp;
	for (;;) {                        // Stay until either zero or EOF
		iTemp = fgetc(fp);            // Get the char from the file
		if (!iTemp || iTemp == EOF) { // Exit due to end of string?
			break;
		}
		// Can I store it?
		if (uLength >= 2) {
			pInput[0] = static_cast<char>(iTemp);
			--uLength;
			++pInput; // Inc the input pointer
		}
	}
	if (uLength) {     // Any space in buffer for the terminating zero?
		pInput[0] = 0; // Add ending zero
	}
	if (iTemp) {      // EOF?
		return FALSE; // Hit the EOF.
	}
	return TRUE; // Hit the end of string (More data may be present)
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
	\sa GetSize64(FILE *)

***************************************/

#if defined(BURGER_XBOX360) || defined(BURGER_WINDOWS) || \
	defined(BURGER_MSDOS) || defined(DOXYGEN)

uintptr_t BURGER_API Burger::GetSize(FILE* fp)
{
#if defined(BURGER_WIN64) || defined(BURGER_XBOX360)
	// 64 bit windows allows a full 64 bit length
	int64_t lLength = _filelengthi64(fileno(fp)); // 64 bit version
	if (lLength == -1LL) {                        // Error?
		lLength = 0;
	}
	return static_cast<uintptr_t>(lLength); // I got it.
#else
	// DOS/Windows allows a direct query to the data stream
	long lLength = filelength(fileno(fp)); // Fast version for INTEL
	if (lLength == -1) {                   // Error?
		lLength = 0;
	}
	return static_cast<uint32_t>(lLength); // I got it.
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

uintptr_t BURGER_API Burger::GetSize(FILE* fp)
{
	uintptr_t uResult = 0;
	BURGER_UNUSED(fp);

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
			if (ParmBlock.positionOffset >= 0x100000000LL) {
				--uResult; // -1
			} else {
				// Return the file size
				uResult = static_cast<uintptr_t>(ParmBlock.positionOffset);
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
			uResult = reinterpret_cast<uintptr_t>(IoBlock.ioParam.ioMisc);
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

#elif defined(BURGER_MACOSX) || defined(BURGER_IOS)

uintptr_t BURGER_API Burger::GetSize(FILE* fp)
{
	off_t Mark = ftello(fp); // Get the current position
	fseeko(fp, 0, SEEK_END); // Seek to the end of the file
	off_t Length = ftello(fp);
	uintptr_t uResult = 0;
	if (Length == -1) {
		// If 64 bit, use as is
#if defined(BURGER_64BITCPU)
		uResult = static_cast<uintptr_t>(Length);
#else
		// If 32 bit, max the value out
		if (static_cast<uint64_t>(Length) < 0x100000000ULL) {
			uResult = static_cast<uintptr_t>(Length);
		} else {
			uResult = 0xFFFFFFFFU;
		}
#endif
	}
	fseeko(fp, Mark, SEEK_SET);
	return uResult;
}

#else

//
// Generic operating system version
//

uintptr_t BURGER_API Burger::GetSize(FILE* fp)
{
	long Mark = ftell(fp);                 // Get the current position
	fseek(fp, 0, SEEK_END);                // Seek to the end of the file
	long Length = ftell(fp);               // Where's the EOF?
	fseek(fp, Mark, SEEK_SET);             // Restore the file position
	return static_cast<uintptr_t>(Length); // Return the file size
}

#endif

/*! ************************************

	\brief Return the size of a file with 64 bits

	Given an ANSI FILE *, return the size of the file.

	\param fp FILE * to the stream to write to.

	This function is faster than the fseek(), ftell(seek),
	fseek() method since this uses custom code tailored for each
	compiler to directly access records in the FILE structure in
	most cases.

	\return The size of the file in bytes, or zero on error.
	\sa GetSize(FILE *)

***************************************/

#if defined(BURGER_XBOX360) || defined(BURGER_WINDOWS) || \
	defined(BURGER_MSDOS) || defined(DOXYGEN)

uint64_t BURGER_API Burger::GetSize64(FILE* fp)
{

	// Go directly into MSL's library to get the windows
	// handle so a 64 bit file size can be obtained
	// on 32 bit systems

#if defined(BURGER_METROWERKS)
	int ifp = fileno(fp);
	uint64_t lLength = 0;
	if (static_cast<uint_t>(ifp) < NUM_HANDLES) {
		FileStruct* pStruct = _HandleTable[ifp];
		if (pStruct) {
			HANDLE hHandle = pStruct->handle;
			LARGE_INTEGER uLongSize;
			BOOL bGetSizeResult = GetFileSizeEx(hHandle, &uLongSize);
			if (bGetSizeResult) {
				lLength = static_cast<uint64_t>(uLongSize.QuadPart);
			}
		}
	}
	return lLength; // I got it.
#else
	// 64 bit windows allows a full 64 bit length
	int64_t lLength = _filelengthi64(fileno(fp)); // 64 bit version
	if (lLength == -1LL) {                        // Error?
		lLength = 0;
	}
	return static_cast<uint64_t>(lLength); // I got it.
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

uint64_t BURGER_API Burger::GetSize64(FILE* fp)
{
	uint64_t uResult = 0;
	BURGER_UNUSED(fp);

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
			// Return the file size
			uResult = static_cast<uint64_t>(ParmBlock.positionOffset);
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
			uResult = reinterpret_cast<uint64_t>(IoBlock.ioParam.ioMisc);
		}
#endif
#if _MSL_USE_OLD_AND_NEW_FILE_APIS
	}
#endif
	return uResult;
}

//
// MacOSX version (Cocoa) or iOS
//

#elif defined(BURGER_MACOSX) || defined(BURGER_IOS)

uint64_t BURGER_API Burger::GetSize64(FILE* fp)
{
	off_t Mark = ftello(fp); // Get the current position
	fseeko(fp, 0, SEEK_END); // Seek to the end of the file
	off_t Length = ftello(fp);
	uint64_t uResult = 0;
	if (Length == -1LL) {
		uResult = static_cast<uint64_t>(Length);
	}
	fseeko(fp, Mark, SEEK_SET);
	return uResult;
}

#else

//
// Generic operating system version
//

uint64_t BURGER_API Burger::GetSize64(FILE* fp)
{
	long Mark = ftell(fp);                // Get the current position
	fseek(fp, 0, SEEK_END);               // Seek to the end of the file
	long Length = ftell(fp);              // Where's the EOF?
	fseek(fp, Mark, SEEK_SET);            // Restore the file position
	return static_cast<uint64_t>(Length); // Return the file size
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
	\sa LoadFile(FILE *,uintptr_t *)

***************************************/

Burger::eError BURGER_API Burger::SaveFile(
	FILE* fp, const void* pInput, uintptr_t uLength)
{
	eError uResult = kErrorWriteFailure; // Assume failure
	if (fp) {                            // Valid file structure?
		uintptr_t uNewLength =
			fwrite(pInput, 1, uLength, fp);      // Write the file contents
		int Err = fclose(fp);                    // Close the file
		if ((uNewLength == uLength) && (!Err)) { // Error?
			uResult = kErrorNone;                // It's ok!
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

	Dispose of the memory returned with a call to
	\ref Burger::Free(const void *)

	\param fp FILE * to the file to read from
	\param pLength to a uintptr_t to receive the size of the file in bytes.

	\return \ref NULL if failure, a pointer on success

	\note If the file is empty, it's considered a failure and a \ref NULL is
		returned.

	\sa SaveFile(FILE *,const void *,uintptr_t)

***************************************/

void* BURGER_API Burger::LoadFile(FILE* fp, uintptr_t* pLength)
{
	void* pResult = NULL;      // Init result
	uintptr_t uFileLength = 0; // Init length

	if (fp) {                      // Valid file reference?
		uFileLength = GetSize(fp); // Get the file length
		if (uFileLength) {
			pResult = Alloc(uFileLength); // Get memory
			if (pResult) {
				if (fread(pResult, 1, uFileLength, fp) !=
					uFileLength) { // Read the file contents
					// Oh. Crap.
					Free(pResult);  // Release the memory
					pResult = NULL; // Zap the pointer
					uFileLength = 0;
				}
			}
		}
		fclose(fp); // Close the file
	}
	if (pLength) {                // Shall I return the file length? */
		pLength[0] = uFileLength; // Ok
	}
	return pResult; // Return the result
}

/*! ************************************

	\brief Force an ANSI FILE * to binary mode

	If the "C" standard library supports it, change the mode for an
	open FILE * to binary

	\note This is only relevant to operating systems that
	require remapping of text files on to and from a non-linux
	line feed only line endings.

	\param fp FILE * to the file to read from

***************************************/

void BURGER_API Burger::SetBinaryMode(FILE* fp)
{
#if defined(__EMX__)
	_fsetmode(fp, "b");
#elif defined(__BORLANDC__)
	setmode(_fileno(fp), O_BINARY);
#elif defined(__CYGWIN__)
	setmode(fileno(fp), _O_BINARY);
#elif defined(BURGER_WINDOWS) || defined(BURGER_WATCOM) || \
	defined(BURGER_XBOX360)
	_setmode(_fileno(fp), _O_BINARY);
	// Metrowerks MSL library
#elif defined(BURGER_MAC) && defined(__MSL__)
	fp->mode.binary_io = 1;
#else
	// Linux/Android/MacOSX/BSD
	BURGER_UNUSED(fp);
#endif
}
