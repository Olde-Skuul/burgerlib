/***************************************

	ASCII to number functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRASCIITO_H__
#define __BRASCIITO_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {

extern Word32 BURGER_API AsciiToInteger(
	const char* pInput, const char** pDest = nullptr) BURGER_NOEXCEPT;
extern Int BURGER_API AsciiToInteger(const char* pInput, Int iDefault,
	Int iMin = INT32_MIN, Int iMax = INT32_MAX);
extern Word BURGER_API AsciiToInteger(Word32* pOutput, const char* pInput);
extern Word BURGER_API AsciiToWord(const char* pInput, Word uDefault,
	Word uMin = 0, Word uMax = BURGER_MAXUINT);
extern Word64 BURGER_API AsciiToInteger64(
	const char* pInput, const char** pDest = NULL);
extern Int64 BURGER_API AsciiToInteger64(const char* pInput, Int64 iDefault,
	Int64 iMin = INT64_MIN, Int64 iMax = INT64_MAX);
extern Word BURGER_API AsciiToInteger64(Word64* pOutput, const char* pInput);
extern Word64 BURGER_API AsciiToWord64(const char* pInput, Word64 uDefault,
	Word64 uMin = 0, Word64 uMax = BURGER_MAXUINT64);
extern float BURGER_API AsciiToFloat(
	const char* pInput, const char** pDest = NULL);
extern double BURGER_API AsciiToDouble(
	const char* pInput, const char** pDest = NULL);
extern Word BURGER_API AsciiToBoolean(
	const char* pInput, const char** pDest = NULL);
extern Word BURGER_API AsciiToBoolean(const char* pInput, Word bDefault);
extern Word BURGER_API AsciiToBoolean(Word* pOutput, const char* pInput);
extern float BURGER_API AsciiToFloat(const char* pInput, float fDefault);
extern float BURGER_API AsciiToFloat(
	const char* pInput, float fDefault, float fMin, float fMax);
extern Word BURGER_API AsciiToFloat(float* pOutput, const char* pInput);
extern double BURGER_API AsciiToDouble(const char* pInput, double dDefault);
extern double BURGER_API AsciiToDouble(
	const char* pInput, double dDefault, double dMin, double dMax);
extern Word BURGER_API AsciiToDouble(double* pOutput, const char* pInput);
extern Word32 BURGER_API AsciiHexToInteger(const char* pInput, WordPtr uLength);
extern Word32 BURGER_API AsciiHexToInteger(
	const Word16* pInput, WordPtr uLength);
}

/* END */

#endif
