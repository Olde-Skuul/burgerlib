/***************************************

	ASCII to number functions

	Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

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

extern uint32_t BURGER_API AsciiToInteger(
	const char* pInput, const char** pDest = nullptr) BURGER_NOEXCEPT;
extern int_t BURGER_API AsciiToInteger(const char* pInput, int_t iDefault,
	int_t iMin = INT32_MIN, int_t iMax = INT32_MAX) BURGER_NOEXCEPT;
extern uint_t BURGER_API AsciiToInteger(
	uint32_t* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern uint_t BURGER_API AsciiToWord(const char* pInput, uint_t uDefault,
	uint_t uMin = 0, uint_t uMax = UINT32_MAX) BURGER_NOEXCEPT;
extern uint64_t BURGER_API AsciiToInteger64(
	const char* pInput, const char** pDest = nullptr) BURGER_NOEXCEPT;
extern int64_t BURGER_API AsciiToInteger64(const char* pInput, int64_t iDefault,
	int64_t iMin = INT64_MIN, int64_t iMax = INT64_MAX) BURGER_NOEXCEPT;
extern uint_t BURGER_API AsciiToInteger64(
	uint64_t* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern uint64_t BURGER_API AsciiToWord64(const char* pInput, uint64_t uDefault,
	uint64_t uMin = 0, uint64_t uMax = UINT64_MAX) BURGER_NOEXCEPT;
extern float BURGER_API AsciiToFloat(
	const char* pInput, const char** pDest = nullptr) BURGER_NOEXCEPT;
extern double BURGER_API AsciiToDouble(
	const char* pInput, const char** pDest = nullptr) BURGER_NOEXCEPT;
extern uint_t BURGER_API AsciiToBoolean(
	const char* pInput, const char** pDest = nullptr) BURGER_NOEXCEPT;
extern uint_t BURGER_API AsciiToBoolean(
	const char* pInput, uint_t bDefault) BURGER_NOEXCEPT;
extern uint_t BURGER_API AsciiToBoolean(
	uint_t* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern float BURGER_API AsciiToFloat(
	const char* pInput, float fDefault) BURGER_NOEXCEPT;
extern float BURGER_API AsciiToFloat(
	const char* pInput, float fDefault, float fMin, float fMax) BURGER_NOEXCEPT;
extern uint_t BURGER_API AsciiToFloat(
	float* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern double BURGER_API AsciiToDouble(
	const char* pInput, double dDefault) BURGER_NOEXCEPT;
extern double BURGER_API AsciiToDouble(const char* pInput, double dDefault,
	double dMin, double dMax) BURGER_NOEXCEPT;
extern uint_t BURGER_API AsciiToDouble(
	double* pOutput, const char* pInput) BURGER_NOEXCEPT;
extern uint32_t BURGER_API AsciiHexToInteger(
	const char* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
extern uint32_t BURGER_API AsciiHexToInteger(
	const uint16_t* pInput, uintptr_t uLength) BURGER_NOEXCEPT;
}

/* END */

#endif
