/***************************************

	Matt Pritchard's typesafe & enhanced c++ formatted output function
	replacements

	Note from Matt Pritchard: mprintf is dedicated to the ones I love: Jennifer,
	Nathan and Lauren.

	Copyright (c) 2012-2013 by Matt Pritchard <mattp_2006@yahoo.com>

	Copyright (c) 2017-2021 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

// ================================================================================================
// *************************************** mprintf's license
// **************************************
// ================================================================================================
//
// mprintf uses the (very permissive) open source ZLIB license:
//
// http://opensource.org/licenses/Zlib
//
// License text for mprintf.h:
//
// Copyright (c) 2012-2013 Matt Pritchard
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the
//   author(s) be held liable for any damages arising from the use of this
//   software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial
//   applications, and to alter it and redistribute it freely, subject to the
//   following restrictions:
//
//  1. The origin of this software must not be misrepresented; you must not
//  claim that you wrote the
//     original software. If you use this software in a product, an
//     acknowledgment in the product documentation would be appreciated but is
//     not required.
//
//  2. Altered source versions must be plainly marked as such, and must not be
//  misrepresented as
//     being the original software.
//
//  3. This notice may not be removed or altered from any source distribution.
//

#include "brprintf.h"
#include "brassert.h"
#include "brfixedpoint.h"
#include "brfloatingpoint.h"
#include "brfpinfo.h"
#include "brmemoryfunctions.h"
#include "brnumberto.h"
#include "brstring.h"
#include "brutf32.h"
#include "brutf8.h"

/*! ************************************

	\def BURGER_EXPAND_FORMATTING_FUNCTION
	\brief Function to generate SafePrint functions

	To invoke SafePrint via a parameter list, it's necessary
	to fake the ability to do the "C" ... style parameter passing.
	To handle this, generate a code fragment that will
	create the argument table and then call the SafePrint functions
	and this macro will create 24 functions that handle 1 through
	24 parameters.

***************************************/

/*! ************************************

	\class Burger::SafePrint
	\brief Container class for SafePrint functions

	This class contains all the static functions to handle the
	safe printf for Burgerlib.

***************************************/

/*! ************************************

	\struct Burger::SafePrint::ParamInfo_t
	\brief Container struct for SafePrint arguments

	Describes a single conversion to be done
	It can represent a block of literal text to be copied or
	it can describe and argument to be converted

	An array of ParamInfo_t objects is built during the parsing phase
	from the format string (with optional arguments) and contains the
	decoded and validated copy and formatted conversion commands that
	use the actual argument values to determine things like how many
	characters are outputted

	\note This is a structure, as such, it's the application's responsibility
	to zero out this structure on instantiation manually. Generally, this is
	created as an array, so it's encouraged to call
	\ref MemoryClear(void *,uintptr_t) on the entire array on creation.

***************************************/

/*! ************************************

	\fn eConversionCommand Burger::SafePrint::ParamInfo_t::GetCommand(void)
const \brief Get the \ref eConversionCommand

	This structure stores this as a byte for compactness, this accessor enforces
type safety

	\return The stored \ref eConversionCommand
	\sa SetCommand(eConversionCommand)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetCommand(eConversionCommand
uCommand) \brief Set the \ref eConversionCommand

	This structure stores this as a byte for compactness, this accessor enforces
type safety

	\param uCommand The \ref eConversionCommand to store in the structure
	\sa GetCommand(void) const

***************************************/

/*! ************************************

	\fn eConversionSpecifier Burger::SafePrint::ParamInfo_t::GetConversion(void)
const \brief Get the \ref eConversionSpecifier

	This structure stores this as a byte for compactness, this accessor enforces
type safety

	\return The stored \ref eConversionSpecifier
	\sa SetConversion(eConversionSpecifier) or GetConversionAsFlag(void) const

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetConversion(eConversionSpecifier
uOutputConversion) \brief Set the \ref eConversionSpecifier

	This structure stores this as a byte for compactness, this accessor enforces
type safety

	\param uOutputConversion The \ref eConversionSpecifier to store in the
structure \sa GetConversion(void) const or GetConversionAsFlag(void) const

***************************************/

/*! ************************************

	\fn uint32_t Burger::SafePrint::ParamInfo_t::GetConversionAsFlag(void) const
	\brief Get the \ref eConversionSpecifier as a shifted flag

	Get the value of 1 << \ref eConversionSpecifier

	\return The value of 1<< \ref eConversionSpecifier
	\sa SetConversion(eConversionSpecifier) or GetConversion(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::SafePrint::ParamInfo_t::GetArgIndex(void) const
	\brief Get the argument index

	This structure stores this as a byte for compactness, this accessor enforces
type safety

	\return The index into the argument array this structure refers to
	\sa SetArgIndex(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetArgIndex(uint_t uArgIndex)
	\brief Set the argument index

	This structure stores this as a byte for compactness, this accessor enforces
type safety

	\param uArgIndex The new index into the argument array this structure refers
to \sa GetArgIndex(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::SafePrint::ParamInfo_t::GetWarnings(void) const
	\brief Get the accumulated warnings

	This structure stores this as a byte for compactness, this accessor enforces
type safety

	\return The accumulated warning bits
	\sa SetWarning(eWarningFlags)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetWarning(eWarningFlags uWarning)
	\brief Accumulate a warning

	Take the warning flag and OR it into the existing warning state. This
function does not clear any previous bits.

	This structure stores this as a byte for compactness, this accessor enforces
type safety.

	\param uWarning The warning bit to set in the \ref m_uWarnings member
variable \sa GetWarnings(void) const

***************************************/

/*! ************************************

	\fn uint32_t Burger::SafePrint::ParamInfo_t::GetFlags(void) const
	\brief Get the accumulated \ref eConversionFlags bits

	The individual bits are defined in \ref eConversionFlags .

	\return All of the active conversion flags
	\sa SetFlags(uint32_t), SetFlag(eConversionFlags), or
IsFlagSet(eConversionFlags) const

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetFlags(uint32_t
uFormatOptionFlags) \brief Set the accumulated \ref eConversionFlags bits

	The individual bits are defined in \ref eConversionFlags.

	\param uFormatOptionFlags All of the \ref eConversionFlags to store in the
structure \sa GetFlags(void) const, SetFlag(eConversionFlags), or
ClearFlag(eConversionFlags)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetFlag(eConversionFlags
uFormatOptionFlags) \brief Set the specific \ref eConversionFlags bit

	Or the specific flag into the flags.

	\param uFormatOptionFlags The single type safe flag to accumulate into the
structure \sa IsFlagSet(eConversionFlags) const, GetFlags(void) const,
SetFlags(uint32_t), or ClearFlag(eConversionFlags)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::ClearFlag(eConversionFlags
uFormatOptionFlags) \brief Clear a specific \ref eConversionFlags bit

	Clear the specific flag stored in the structure.

	\sa ClearFlags(void), IsFlagSet(eConversionFlags) const, GetFlags(void)
const, SetFlags(uint32_t), or SetFlag(eConversionFlags)

***************************************/

/*! ************************************

	\fn uint_t Burger::SafePrint::ParamInfo_t::IsFlagSet(eConversionFlags
uFormatOptionFlags) \brief Test a specific \ref eConversionFlags bit

	Test if the specific flag is set and return \ref TRUE if so, otherwise
return \ref FALSE.

	\param uFormatOptionFlags The single type safe flag to test against
	\return \ref TRUE if set, \ref FALSE if not.
	\sa SetFlag(eConversionFlags), GetFlags(void) const, SetFlags(uint32_t), or
ClearFlag(eConversionFlags)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::ClearFlags(void)
	\brief Clear the \ref eConversionFlags bits

	Set the bits to zero.

	\sa ClearFlag(eConversionFlags), IsFlagSet(eConversionFlags) const,
GetFlags(void) const, SetFlags(uint32_t), or SetFlag(eConversionFlags)

***************************************/

/*! ************************************

	\fn uint_t Burger::SafePrint::ParamInfo_t::GetWidth(void) const
	\brief Get the width of the output for the argument

	This structure stores this as a uint16_t for compactness, this accessor
enforces type safety

	\return The width override parsed in the "C" string
	\sa SetWidth(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetWidth(uint_t uCharacterWidth)
	\brief Set the width of the output for the argument

	This structure stores this as a uint16_t for compactness, this accessor
enforces type safety

	\param uCharacterWidth The new width of the output for the argument
	\sa GetWidth(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::SafePrint::ParamInfo_t::GetPrecision(void) const
	\brief Get the desired precision of the argument

	This structure stores this as a uint16_t for compactness, this accessor
enforces type safety

	\return The precision requested for the argument
	\sa SetPrecision(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetPrecision(uint_t uPrecision)
	\brief Set the width of the output for the argument

	This structure stores this as a uint16_t for compactness, this accessor
	enforces type safety

	\param uPrecision The new precision requested for the argument
	\sa GetPrecision(void) const

***************************************/

/*! ************************************

	\fn uintptr_t Burger::SafePrint::ParamInfo_t::GetFormattedLength(void) const
	\brief Get the parsed length of the '%%' command

	This structure stores this as a uint16_t for compactness, this accessor
	enforces type safety

	\return The number of bytes in the parsed "C" string this format code
		occupies

	\sa SetFormattedLength(uintptr_t)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetFormattedLength(uintptr_t
		uFormattedLength)

	\brief Get the parsed length of the '%%' command

	This structure stores this as a uint16_t for compactness, this accessor
	enforces type safety

	\param uFormattedLength The new number of bytes in the parsed "C" string
		this format code occupies

	\sa GetFormattedLength(void) const

***************************************/

/*! ************************************

	\fn uintptr_t Burger::SafePrint::ParamInfo_t::GetOutputLength(void) const
	\brief Get the length of the processed argument

	This structure stores this as a uint16_t for compactness, this accessor
	enforces type safety

	\return The number of bytes the printed argument will occupy
	\sa SetOutputLength(uintptr_t)

***************************************/

/*! ************************************

	\fn void Burger::SafePrint::ParamInfo_t::SetOutputLength(uintptr_t
		uOutputLength)
	\brief Set the length of the processed argument

	This structure stores this as a uint16_t for compactness, this accessor
	enforces type safety

	\param uOutputLength The new number of bytes the printed argument will
		occupy

	\sa GetOutputLength(void) const

***************************************/

/*! ************************************

	\brief Set the parsed floating point information

	This structure stores these variables in a compact fashion, this accessor
enforces type safety

	\param uIntSpecialDigits Number of digits to the left of the decimal point
	\param uFracDigits Number of digits to the right of the decimal point
	\param bSign \ref TRUE if a sign flag should be displayed
	\param uDecimalPoint \ref TRUE if a decimal point should be displayed
	\param uPadDigits Number of digits to pad
	\param uExponentDigits Number of exponent digits

	\sa GetFloatInfo(uint_t *,uint_t *,uint_t *,uint_t *,uint_t *,uint_t *)
const

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::SetFloatInfo(
	uint_t uIntSpecialDigits, uint_t uFracDigits, uint_t bSign,
	uint_t uDecimalPoint, uint_t uPadDigits,
	uint_t uExponentDigits) BURGER_NOEXCEPT
{
	m_FloatInfo.m_uIntSpecialDigits = static_cast<uint16_t>(uIntSpecialDigits);
	m_FloatInfo.m_uFracDigits = static_cast<uint16_t>(uFracDigits);
	m_FloatInfo.m_uSignDecimalDigits =
		static_cast<uint8_t>((bSign << 4U) | (uDecimalPoint & 0x0fU));
	m_FloatInfo.m_uPadDigits = static_cast<uint16_t>(uPadDigits);
	m_FloatInfo.m_uExponentDigits = static_cast<uint8_t>(uExponentDigits);
}

/*! ************************************

	\brief Get the parsed floating point information

	This structure stores these variables in a compact fashion, this accessor
	enforces type safety

	\param pIntSpecialDigits Pointer to store the number of digits to the left
		of the decimal point
	\param pFracDigits Pointer to store the Number of digits to the right of the
		decimal point
	\param pSign Pointer to store \ref TRUE or \ref FALSE if a sign flag should
		be displayed
	\param pDecimalPoint Pointer to store \ref TRUE or \ref FALSE if a decimal
		point should be displayed
	\param pPadDigits Pointer to store the number of digits to pad
	\param pExponentDigits Pointer to store the number of exponent digits

	\sa SetFloatInfo(uint_t,uint_t,uint_t,uint_t,uint_t,uint_t)

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::GetFloatInfo(
	uint_t* pIntSpecialDigits, uint_t* pFracDigits, uint_t* pSign,
	uint_t* pDecimalPoint, uint_t* pPadDigits,
	uint_t* pExponentDigits) const BURGER_NOEXCEPT
{
	pIntSpecialDigits[0] = m_FloatInfo.m_uIntSpecialDigits;
	pFracDigits[0] = m_FloatInfo.m_uFracDigits;
	pSign[0] = static_cast<uint_t>(m_FloatInfo.m_uSignDecimalDigits >> 4U);
	pDecimalPoint[0] = (m_FloatInfo.m_uSignDecimalDigits & 0x0FU);
	pPadDigits[0] = m_FloatInfo.m_uPadDigits;
	pExponentDigits[0] = m_FloatInfo.m_uExponentDigits;
}

/*! ************************************

	\brief Set the analyzed floating point information

	This structure stores these variables in a compact fashion, this accessor
	enforces type safety

	\param uFPInfoResult Type of floating point number detected
	\param uPadIntZeros Number of characters to pad on the integer
	\param uPadFracZeros Number of character to pad on the fraction
	\param uFirstNonZeroIntPos Position of the first non-zero digit in the
		integer

	\sa GetFloatAnalysisInfo(FPPrintInfo::eResult *,uint_t *,uint_t *,uint_t *)
const

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::SetFloatAnalysisInfo(
	FPPrintInfo::eResult uFPInfoResult, uint_t uPadIntZeros,
	uint_t uPadFracZeros, uint_t uFirstNonZeroIntPos) BURGER_NOEXCEPT
{
	m_FloatInfo.m_uFPInfoResult = static_cast<uint16_t>(uFPInfoResult);
	m_FloatInfo.m_uPadIntZeros = static_cast<uint16_t>(uPadIntZeros);
	m_FloatInfo.m_uPadFracZeros = static_cast<uint16_t>(uPadFracZeros);
	m_FloatInfo.m_uFirstNonZeroIntPos =
		static_cast<uint16_t>(uFirstNonZeroIntPos);
}

/*! ************************************

	\brief Get the analyzed floating point information

	This structure stores these variables in a compact fashion, this accessor
	enforces type safety

	\param pFPInfoResult Pointer to the type of floating point number detected
	\param pPadIntZeros Pointer to the number of characters to pad on the
		integer
	\param pPadFracZeros Pointer to the number of character to pad on the
		fraction
	\param pFirstNonZeroIntPos Pointer to the position of the first non-zero
		digit in the integer

	\sa SetFloatAnalysisInfo(FPPrintInfo::eResult,uint_t,uint_t,uint_t)

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::GetFloatAnalysisInfo(
	FPPrintInfo::eResult* pFPInfoResult, uint_t* pPadIntZeros,
	uint_t* pPadFracZeros, uint_t* pFirstNonZeroIntPos) const BURGER_NOEXCEPT
{
	pFPInfoResult[0] =
		static_cast<FPPrintInfo::eResult>(m_FloatInfo.m_uFPInfoResult);
	pPadIntZeros[0] = m_FloatInfo.m_uPadIntZeros;
	pPadFracZeros[0] = m_FloatInfo.m_uPadFracZeros;
	pFirstNonZeroIntPos[0] = m_FloatInfo.m_uFirstNonZeroIntPos;
}

/*! ************************************

	\brief Set the special floating point information for NAN or INF

	This structure stores these variables in a compact fashion, this accessor
	enforces type safety

	\param uFPInfoResult Type of floating point number detected
	\param uIntSpecialDigits Number of characters in the string
	\param uPadIntZeros Number of characters to pad on the integer
	\param uIsNegative Set to non-zero if negative

	\sa SetFloatAnalysisInfo(FPPrintInfo::eResult,uint_t,uint_t,uint_t)

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::SetFloatSpecialResult(
	FPPrintInfo::eResult uFPInfoResult, uint_t uIntSpecialDigits,
	uint_t uPadIntZeros, uint_t uIsNegative) BURGER_NOEXCEPT
{
	m_FloatInfo.m_uFPInfoResult = static_cast<uint16_t>(uFPInfoResult);
	m_FloatInfo.m_uIntSpecialDigits = static_cast<uint16_t>(uIntSpecialDigits);
	m_FloatInfo.m_uPadIntZeros = static_cast<uint16_t>(uPadIntZeros);
	m_FloatInfo.m_uExponentDigits = static_cast<uint8_t>(uIsNegative);
}

/*! ************************************

	\fn uint_t Burger::SafePrint::ParamInfo_t::GetFloatSpecialChars(void) const
	\brief Get the number of special character for a special floating point
		number

	\return The number of bytes the special string will occupy
	\sa SetFloatSpecialResult(FPPrintInfo::eResult,uint_t,uint_t,uint_t)

***************************************/

/*! ************************************

	\fn uint_t Burger::SafePrint::ParamInfo_t::GetFloatSpecialLeadingZeros(void)
		const
	\brief Get the number leading zeros for a special floating point number

	\return The number of leading zeros
	\sa SetFloatSpecialResult(FPPrintInfo::eResult,uint_t,uint_t,uint_t)

***************************************/

/*! ************************************

	\fn uint_t Burger::SafePrint::ParamInfo_t::GetFloatSpecialIsNegative(void)
		const
	\brief Is the special floating pointer number negative?

	\return \ref TRUE if negative, \ref FALSE if not.
	\sa SetFloatSpecialResult(FPPrintInfo::eResult,uint_t,uint_t,uint_t)

***************************************/

/*! ************************************

	\brief Output padding for field width

	Handles the case when a field width is specified and and it's wider than the
	formatted argument output. Fills in the remainder of the field with the pad
	character and handles any justification

	\param pOutBuffer Pointer to the output buffer
	\return Number of bytes that was added via padding

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ParamInfo_t::PadFieldWidth(
	char* pOutBuffer) const BURGER_NOEXCEPT
{
	// if the width was not explicitly specified in the formatting command,
	// or it isn't wider than the formatted argument data,
	// then we won't be padding anything

	uintptr_t uFormattedLength = GetFormattedLength();
	const uintptr_t uOutputLength = GetOutputLength();
	uintptr_t uResult;
	if (!IsFlagSet(CONVFLAG_WIDTH_SPECIFIED) ||
		(GetWidth() <= uFormattedLength)) {
		BURGER_ASSERT(uFormattedLength == uOutputLength);
		uResult = 0;
	} else {
		BURGER_ASSERT(uFormattedLength < uOutputLength);

		const uint_t bLeftJustify = IsFlagSet(CONVFLAG_LEFT_JUSTIFY);
		const uintptr_t uPadLength = uOutputLength - uFormattedLength;
		if (uPadLength) {
			// Adjust to the output if left justified
			if (bLeftJustify) {
				pOutBuffer += uFormattedLength;
			}
			uintptr_t i = 0;
			do {
				pOutBuffer[i] = kWIDTH_PAD_CHARACTER;
			} while (++i < uPadLength);
		}

		// return number of characters into field to put argument data at
		uResult = (bLeftJustify) ? 0 : uPadLength;
	}
	return uResult;
}

/*! ************************************

	\brief Calculate the number of character that will be printed for this
		argument

	Gets the actual # of characters that will be output given the argument
	passed in and the format options

	\param pArg Pointer to a \ref ArgumentType to check

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::GetFormattedOutputLength(
	const ArgumentType* pArg) BURGER_NOEXCEPT
{
	const eConversionSpecifier uConversionSpecifier = GetConversion();
	uint_t bHasWidth = IsFlagSet(CONVFLAG_WIDTH_SPECIFIED);
	uint_t uWidth = (bHasWidth) ? GetWidth() : 0;

	// Init the final count
	uintptr_t uCharsCounted = 0;

	// display data in binary
	// flags that modify output length: CommaSeparate (insert space between
	// nibbles)
	if (uConversionSpecifier == CONVSPEC_BINNUMBER) {
		uintptr_t uArgBytes = pArg->GetDataLengthInBytes();
		uCharsCounted = 8 * uArgBytes; // 8 bits per byte

		// Account for commas
		uCharsCounted +=
			(IsFlagSet(CONVFLAG_COMMA_SEPARATE)) ? (uArgBytes * 2 - 1) : 0;

		// display as boolean text ("true" or "false")
		// flags that modify output length: none
	} else if (uConversionSpecifier == CONVSPEC_BOOLTEXT) {
		uCharsCounted = StringLength(
			g_pBoolStrings[pArg->GetBool()]); // length of "true" : "false"

		// Char is one character, 'nuff said
	} else if (uConversionSpecifier == CONVSPEC_CHAR) {
		if (IsFlagSet(CONVFLAG_INTERPRET_LONG) &&
			((pArg->GetType() == kArgumentTypeUInt16) ||
				(pArg->GetType() == kArgumentTypeInt16) ||
				(pArg->GetType() == kArgumentTypeUInt32) ||
				(pArg->GetType() == kArgumentTypeInt32))) {
			// Convert wide character to UTF-8
			char TempBufferUTF8[8];
			// The buffer is discard, only the length is of interest
			if (pArg->GetDataLengthInBytes() == 2) {
				uCharsCounted = UTF8::GetUTF16Size(
					static_cast<uint16_t>(pArg->GetUInt32()));
			} else {
				uCharsCounted = UTF8::from_UTF32(
					TempBufferUTF8, static_cast<uint32_t>(pArg->GetUInt32()));
			}
		} else {
			uCharsCounted = 1;
		}

		// Handle all the integer variations
	} else if ((uConversionSpecifier == CONVSPEC_DECIMAL_INT) ||
		(uConversionSpecifier == CONVSPEC_DECIMAL_WORD)) {
		// In relaxed mode, we allow signed ints with unsigned outputs
		// and unsigned ints with signed output and bools with either

		uint_t bSign = (pArg->IsSigned()) ? pArg->IsNegative() : 0;
		// Number of digits to print
		uint_t uNumericDigits = pArg->Is64Bit() ?
            NumberStringLength(pArg->GetUInt64()) :
            NumberStringLength(pArg->GetUInt32());
		uint_t uCommaCount = 0;

		// Precision can expand number of numeric chars output
		if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED)) {
			uint_t uPrecision = GetPrecision();
			// will we be adding leading zeros?
			if (uPrecision > uNumericDigits) {
				uNumericDigits = uPrecision;
			}

			// Special case: The result of converting a zero value with
			//  a precision of zero is no characters
			if (!uPrecision && pArg->IsZero()) {
				uNumericDigits = 0;
			}
		}

		// Ok, we got the number of numeric digits in the int... now what about
		// commas?
		if (IsFlagSet(CONVFLAG_COMMA_SEPARATE) && (uNumericDigits > 3)) {
			uCommaCount = (uNumericDigits - 1) / 3;
		}

		// check flags for displaying a sign character (signed int conversion
		// only)
		if ((uConversionSpecifier == CONVSPEC_DECIMAL_INT) &&
			(IsFlagSet(CONVFLAG_DISPLAY_SIGN) ||
				IsFlagSet(CONVFLAG_BLANK_SIGN))) {
			bSign = 1; // 1 digit for sign character
		}

		// If padding zeros and width specified, they will pad out the width
		// (we know there is no precision, comma or left justify)
		if (bHasWidth && IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
			if ((bSign + uNumericDigits) < uWidth) {
				uNumericDigits = uWidth - bSign;
			}
		}
		// total up characters to output
		uCharsCounted = bSign + uNumericDigits + uCommaCount;

		// Handle a floating point type conversion (regular, scientific
		// notation, or shorter of the two)

	} else if ((uConversionSpecifier == CONVSPEC_DECIMAL_FLOAT) ||
		(uConversionSpecifier == CONVSPEC_SCI_NOTE_FLOAT) ||
		(uConversionSpecifier == CONVSPEC_SHORTER_FLOAT)) {

		// Break the real number up into mantissa, exponent, etc
		FPInfo TheFPInfo;
		switch (pArg->GetType()) {
		case kArgumentTypeFloat16:
			TheFPInfo = pArg->m_Data.m_fHalf;
			break;
		case kArgumentTypeFloat32:
			TheFPInfo = pArg->m_Data.m_fFloat;
			break;
		case kArgumentTypeFloat64:
			TheFPInfo = pArg->m_Data.m_dDouble;
			break;
		default:
			// This is an error condition
			TheFPInfo = 0.0f;
			break;
		}

		// if no precision is specified, we are supposed to use 6 digits of
		// precision
		uint_t uFloatPrecision = 6;
		if (IsFlagSet(CONVFLAG_PRECISION_MARKER)) {
			if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED)) {
				uFloatPrecision = GetPrecision();
			} else {
				uFloatPrecision = 0; // "." only = means zero precision
			}
		}

		uint_t uSignLength = 0;
		// printing a sign character?
		if (TheFPInfo.is_negative() || IsFlagSet(CONVFLAG_DISPLAY_SIGN) ||
			IsFlagSet(CONVFLAG_BLANK_SIGN)) {
			uSignLength = 1;
		}

		// now run a digit check on it...
		const FPPrintInfo TheFPPrintInfo(&TheFPInfo, uFloatPrecision);

		// is it a "special" output such as NAN or infinity?
		if (TheFPPrintInfo.is_special()) {
			// save the result
			SetFloatSpecialResult(TheFPPrintInfo.get_result(),
				TheFPPrintInfo.get_special_form_digits(), 0,
				TheFPInfo.is_negative());
			uCharsCounted = TheFPPrintInfo.get_special_form_digits();

		} else {

			// Create the formatting defaults
			uint_t uExponentSpecialDigits = 0;
			uint_t uIntSpecialDigits = 0;
			uint_t uDecimalPoint = 0;
			uint_t uFracDigits = 0;
			uint_t uCommaCount = 0;
			uint_t uExponentDigits = 0;
			uint_t uPadDigits = 0;
			uint_t uExponentPadDigits = 0;

			// =========================================================
			// get the width for f conversion if f or g specified
			if (uConversionSpecifier == CONVSPEC_DECIMAL_FLOAT /* || (uConversionSpecifier == CONVSPECFLAG_SHORTER_FLOAT) */ ) {
				// Get the number of integer digits
				uIntSpecialDigits =
					TheFPPrintInfo.get_last_non_zero_int_digit_position();

				// zero point something?  then include the zero the left of the
				// decimal
				if (!uIntSpecialDigits) {
					uIntSpecialDigits = 1;
				}

				// how many digits to the right of the decimal point?
				uFracDigits = uFloatPrecision;

				// do we have a decimal point?
				if (uFloatPrecision || IsFlagSet(CONVFLAG_ALTERNATIVE_FORM)) {
					uDecimalPoint = 1;
				}

				// Ok, we got the number of digits in the int part... now what
				// about commas?
				if (IsFlagSet(CONVFLAG_COMMA_SEPARATE) &&
					(uIntSpecialDigits > 3)) {
					uCommaCount = (uIntSpecialDigits - 1) / 3;
				}

				// If padding zeros and width specified, they will pad out the
				// width (we know there is no comma or left justify flag)
				if (bHasWidth && IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
					const uint_t uTotalRealDigits = uSignLength +
						uIntSpecialDigits + uDecimalPoint + uFracDigits;
					if (uTotalRealDigits < uWidth) {
						uPadDigits = uWidth - uTotalRealDigits;
					}
				}
			}

			// =========================================================
			// get the width for e conversion if e or g specified
			if (uConversionSpecifier == CONVSPEC_SCI_NOTE_FLOAT /* || (uConversionSpecifier == CONVSPECFLAG_SHORTER_FLOAT) */ ) {
				// scientific notation, always 1 digit to the left of the
				// decimal
				uExponentSpecialDigits = 1;

				// do we have a decimal point?
				if (uFloatPrecision || IsFlagSet(CONVFLAG_ALTERNATIVE_FORM)) {
					uDecimalPoint = 1;
				}

				// how many digits to the right of the decimal point?
				uFracDigits = uFloatPrecision;

				const int32_t iExponent = static_cast<int32_t>(
					TheFPPrintInfo.has_integer() ?
                        TheFPPrintInfo.get_last_non_zero_int_digit_position() - 1 :
                        0 - TheFPPrintInfo.get_first_non_zero_frac_digit_position());

				// How many digits for the exponent?
				uint_t uExpDigits =
					NumberStringLength(static_cast<uint32_t>(absolute(iExponent)));

				// we support a configurable minimum number of exponent digits
				if (uExpDigits < kMIN_FLOAT_EXPONENT_DIGITS) {
					uExpDigits = kMIN_FLOAT_EXPONENT_DIGITS;
				}

				uExponentDigits =
					2 + uExpDigits; // add the 'e' and the sign character

				// If padding zeros and width specified, they will pad out the
				// width (we know there is no comma or left justify flag)
				if (bHasWidth && IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
					uint_t uTotalRealDigits = uSignLength +
						uExponentSpecialDigits + uDecimalPoint + uFracDigits +
						uExponentDigits;
					if (uTotalRealDigits < uWidth) {
						uExponentPadDigits = uWidth - uTotalRealDigits;
					}
				}
			}

			if (uConversionSpecifier == CONVSPEC_DECIMAL_FLOAT) {
				// tally up all the parts
				uCharsCounted = uSignLength + uIntSpecialDigits + uCommaCount +
					uDecimalPoint + uFracDigits + uPadDigits;

				// cache off the info from the analysis
				SetFloatInfo(uIntSpecialDigits, uFracDigits, uSignLength,
					uDecimalPoint, uPadDigits, 0);
				SetFloatAnalysisInfo(TheFPPrintInfo.get_result(), 0, 0,
					TheFPPrintInfo.get_first_non_zero_int_digit_position());

			} else if (uConversionSpecifier == CONVSPEC_SCI_NOTE_FLOAT) {
				// tally up all the parts
				uCharsCounted = uSignLength + uExponentSpecialDigits +
					uDecimalPoint + uFracDigits + uExponentPadDigits;

				// cache off the info
				SetFloatInfo(uExponentSpecialDigits, uFracDigits, uSignLength,
					uDecimalPoint, uExponentPadDigits, uExponentDigits);
			}
		}

		// Octal conversion takes unsigned ints and in relaxed mode signed ints
		// and bools
	} else if (uConversionSpecifier == CONVSPEC_OCTAL) {

		// A negative signed int is displayed as if it were interpreted (not
		// cast) to unsigned
		if (pArg->IsNegative()) {
			uCharsCounted = ((pArg->GetDataLengthInBytes() * 8) + 2) / 3;
		} else {
			uCharsCounted = (pArg->Is64Bit()) ?
                NumberOctalStringLength(pArg->GetUInt64()) :
                NumberOctalStringLength(pArg->GetUInt32());
		}
		// the only time we have a leading zero is when the value is 0
		uintptr_t uNonZeroDigits =
			(pArg->IsZero()) ? uCharsCounted - 1 : uCharsCounted;

		// Precision can expand number of octal digits output
		if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED)) {
			uint_t uPrecision = GetPrecision();
			// will we be adding leading zeros?
			if (uPrecision > uCharsCounted) {
				uCharsCounted = uPrecision;
			}

			// Special case: The result of converting a zero value with
			//  a precision of zero is no characters
			if (!uPrecision && pArg->IsZero()) {
				uCharsCounted = 0;
			}
		}

		// if alternate Form ('#') is specified and there isn't a leading zero
		//  already due to value = 0 or precision > digits then we have to add
		//  one
		if (IsFlagSet(CONVFLAG_ALTERNATIVE_FORM)) {
			if (uCharsCounted == uNonZeroDigits) {
				++uCharsCounted;
			}
		}

		// If padding zeros and width specified, they will pad out the width
		// (we know there is no precision, comma or left justify)
		if (bHasWidth && IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
			if (uCharsCounted < uWidth) {
				uCharsCounted = uWidth;
			}
		}

		// Hex conversion takes unsigned ints and in relaxed mode signed ints
		// and bools
		//
		// Per the C++ spec, "The value of the pointer is converted to a
		// sequence of
		//                    printing characters, in an implementation defined
		//                    manner."
		// So we are displaying pointers are as hex at full precision
		//  and allow for the alternative form to prefix a '0x'
	} else if ((uConversionSpecifier == CONVSPEC_HEX) ||
		(uConversionSpecifier == CONVSPEC_POINTER)) {

		// Adding the 0x?
		uint_t bAlternativeForm = IsFlagSet(CONVFLAG_ALTERNATIVE_FORM);
		uint_t uPrefixCount = (bAlternativeForm) ? 2U : 0;

		// for Hex (but not pointer), Alternative form (base prefix) is
		// suppressed by zero value
		if ((uConversionSpecifier == CONVSPEC_HEX) && bAlternativeForm &&
			pArg->IsZero()) {
			uPrefixCount = 0;
		}

		if (uConversionSpecifier == CONVSPEC_POINTER) {
			uCharsCounted = pArg->GetDataLengthInBytes() * 2;
		} else {
			// A negative signed int is displayed as if it were interpreted (not
			// cast) to unsigned
			if (pArg->IsNegative()) {
				uCharsCounted = pArg->GetDataLengthInBytes() * 2;
			} else {
				uCharsCounted = (pArg->Is64Bit()) ?
                    NumberHexStringLength(pArg->GetUInt64()) :
                    NumberHexStringLength(pArg->GetUInt32());
			}
		}

		// Precision can expand number of octal digits output
		if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED)) {
			uint_t uPrecision = GetPrecision();
			// will we be adding leading zeros?
			if (uPrecision > uCharsCounted) {
				uCharsCounted = uPrecision;
			}

			// Special case: The result of converting a zero value with
			//  a precision of zero is no characters
			if (!uPrecision && pArg->IsZero()) {
				uCharsCounted = 0;
			}
		}

		// If padding zeros and width specified, they will pad out the width
		// (we know there is no precision, comma or left justify)
		if (bHasWidth && IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
			if ((uCharsCounted + uPrefixCount) < uWidth) {
				uCharsCounted = uWidth - uPrefixCount;
			}
		}

		uCharsCounted += uPrefixCount;

		// "C" string support.

	} else if (uConversionSpecifier == CONVSPEC_TEXT_STRING) {

		// Get the length of the text being pointed to
		if (pArg->IsTextPointer()) {
			uCharsCounted = pArg->GetTextLength();
			// for strings, precision sets the *maximum number of bytes to be
			// written
			if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED)) {
				uint_t uPrecision = GetPrecision();
				if (uCharsCounted > uPrecision) {
					uCharsCounted = uPrecision;
				}
			}
		} else { // Expansion space for string classes, etc..
			uCharsCounted = 0;
		}

	} else {
		// Fell through and not found anything?
		BURGER_ASSERT(FALSE);
	}

	// fill out length of formatted argument
	SetFormattedLength(uCharsCounted);
	if (bHasWidth && (uWidth > uCharsCounted)) {
		uCharsCounted = uWidth;
	}

	// fill out total length of output
	SetOutputLength(uCharsCounted);
}

/*! ************************************

	\brief Sanity check the conversion flags

	Checks the flags specified in a given conversion
	for validity.  Compiler specific variations can be allowed here

	Warnings are set if issues were detected and corrected.

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::CheckConversionFlags(
	void) BURGER_NOEXCEPT
{
	// Don't bother if it's not a conversion
	if (GetCommand() != COMMAND_COPY_TEXT) {

		const eConversionSpecifier uConversionSpecifier = GetConversion();
		uint_t bHasPrecision = IsFlagSet(CONVFLAG_PRECISION_SPECIFIED);

		// Check to see if precision is valid for this conversion
		if (bHasPrecision) {
			if (!(g_ValidFlagsForConversion[uConversionSpecifier] &
					CONVFLAG_PRECISION_SPECIFIED)) {
				// Note warning that this doesn't support precision
				SetWarning(WARN_PRECISION_UNDEFINED);

				// Remove precision info from the conversion
				ClearFlag(CONVFLAG_PRECISION_SPECIFIED);
				ClearFlag(CONVFLAG_PRECISION_MARKER);
				SetPrecision(0);
				bHasPrecision = FALSE;
			}
		}

		// See if any output flags are not supported by this conversion
		BURGER_CONSTEXPR const uint32_t kFlagsToCheck = CONVFLAG_LEFT_JUSTIFY |
			CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN |
			CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS |
			CONVFLAG_COMMA_SEPARATE;
		const uint32_t uIsolatedFlags = GetFlags() & kFlagsToCheck;
		const uint32_t uValidFlags =
			g_ValidFlagsForConversion[uConversionSpecifier] & kFlagsToCheck;

		// Are there any flags set that aren't valid for this conversion type?
		if ((uIsolatedFlags & uValidFlags) != uIsolatedFlags) {
			// Note warning
			SetWarning(WARN_FLAG_UNDEFINED_FOR_CONVERSION);
			// Clear the unsupported flags
			const uint32_t uKeepMask = (~kFlagsToCheck) | uValidFlags;
			SetFlags(GetFlags() & uKeepMask);
		}

		const uint_t bFlagsdiouxXAllowed =
			((uConversionSpecifier == CONVSPEC_DECIMAL_INT) ||
				(uConversionSpecifier == CONVSPEC_DECIMAL_WORD) ||
				(uConversionSpecifier == CONVSPEC_OCTAL) ||
				(uConversionSpecifier == CONVSPEC_HEX));

		// Per C++ spec:  If a precision is specified, the 0 flag is ignored for
		// integer conversions Per C++ Spec:  If the 0 and - flags both appear,
		// the 0 flag is ignored. Our extension: If the 0 and , flags both
		// appear, the 0 flag is ignored.
		if (IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS) &&     // '0'
			(IsFlagSet(CONVFLAG_LEFT_JUSTIFY) ||      // '-'
				IsFlagSet(CONVFLAG_COMMA_SEPARATE) || // ','
				(bHasPrecision && bFlagsdiouxXAllowed))) {
			ClearFlag(CONVFLAG_LEFT_PAD_ZEROS);
			SetWarning(WARN_FLAG_SUPPRESSED);
		}

		// Per C++ Spec: if ' ' and + both appear, ' ' is ignored
		if (IsFlagSet(CONVFLAG_DISPLAY_SIGN) &&
			IsFlagSet(CONVFLAG_BLANK_SIGN)) {
			ClearFlag(CONVFLAG_BLANK_SIGN);
			SetWarning(WARN_FLAG_SUPPRESSED);
		}
	}
}

/*! ************************************

	\brief Sanity check the conversion modifier flags

	Checks the flags specified in a given conversion modifier
	for validity.  Compiler specific variations can be allowed here

	Warnings are set if issues were detected and corrected.

***************************************/

void BURGER_API Burger::SafePrint::ParamInfo_t::CheckConversionForWarnings(
	const ArgumentType* pArg) BURGER_NOEXCEPT
{
	const eConversionSpecifier uConversionSpecifier = GetConversion();

	// check for signed/unsigned mismatches
	if (pArg->IsNumeric()) {
		const uint_t bIsUnsigned = pArg->IsUnsigned();

		switch (uConversionSpecifier) {
		case CONVSPEC_DECIMAL_INT:
			if (bIsUnsigned) {
				SetWarning(WARN_SIGNED_UNSIGNED_MISMATCH);
			}
			break;

		case CONVSPEC_DECIMAL_WORD:
		case CONVSPEC_OCTAL:
		case CONVSPEC_HEX:
			if (!bIsUnsigned) {
				SetWarning(WARN_SIGNED_UNSIGNED_MISMATCH);
			}
			break;
		default:
			break;
		}
	}

	// check for length specifiers
	const uintptr_t uDataInBytes = pArg->GetDataLengthInBytes();
	uintptr_t uExpectedLength = g_ConversionArgumentSizes[uConversionSpecifier];

	// were any length flags given?

	if (IsFlagSet(CONVFLAG_INTERPRET_SPECIFIED)) {
		const uint_t bIntFlags_diouxX =
			(uConversionSpecifier == CONVSPEC_DECIMAL_INT ||
				uConversionSpecifier == CONVSPEC_DECIMAL_WORD ||
				uConversionSpecifier == CONVSPEC_OCTAL ||
				uConversionSpecifier == CONVSPEC_HEX);
		const uint_t bFloatFlags_aAeEfFgG =
			(uConversionSpecifier == CONVSPEC_DECIMAL_FLOAT ||
				uConversionSpecifier == CONVSPEC_SCI_NOTE_FLOAT ||
				uConversionSpecifier == CONVSPEC_SHORTER_FLOAT);

		// Need to implement all 'n' modifiers

		if (IsFlagSet(CONVFLAG_INTERPRET_SSHORT)) { // hh = char
			uExpectedLength = (bIntFlags_diouxX) ? 1 : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_SHORT)) { // h = short
			uExpectedLength = (bIntFlags_diouxX) ? 2 : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_LONG)) { // l = long int
			if (bIntFlags_diouxX) {
				uExpectedLength = sizeof(long);
			} else if (uConversionSpecifier == CONVSPEC_CHAR) {
				uExpectedLength = sizeof(uint16_t);
			} else if (uConversionSpecifier == CONVSPEC_TEXT_STRING) {
				// expected type = uint16_t *, however we're only checking the
				// sizeof( pointer )
				uExpectedLength = sizeof(void*);
			} else if (bFloatFlags_aAeEfFgG) {
				SetWarning(WARN_FLAG_NO_EFFECT);
			} else {
				uExpectedLength = UINTPTR_MAX;
			}
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_LLONG)) { // ll = long long int
			uExpectedLength =
				(bIntFlags_diouxX) ? sizeof(int64_t) : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_DOUBLE)) { // L = long double
			uExpectedLength =
				(bFloatFlags_aAeEfFgG) ? sizeof(long double) : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_INTMAX)) { // j = intmax_t or uintmax_t
			uExpectedLength =
				(bIntFlags_diouxX) ? sizeof(int32_t) : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_SIZET)) { // z = size_t
			uExpectedLength =
				(bIntFlags_diouxX) ? sizeof(uintptr_t) : UINTPTR_MAX;
		}

		if (IsFlagSet(CONVFLAG_INTERPRET_WORDPTR)) { // t = ptrdiff_t
			uExpectedLength =
				(bIntFlags_diouxX) ? sizeof(uintptr_t) : UINTPTR_MAX;
		}

		// is the length modifier not defined for the conversion?
		if (uExpectedLength == UINTPTR_MAX) {
			SetWarning(WARN_LENGTH_NOT_VALID_FOR_CONVERSION);
		}
	}

	// Special case for character conversion.  The conversion actually takes an
	// int, which chars are normally up-promoted to but because we capture the
	// actual type without up-promotion we want to not warn if it's smaller and
	// up-promoted
	if ((uConversionSpecifier == CONVSPEC_CHAR) &&
		!IsFlagSet(CONVFLAG_INTERPRET_SPECIFIED)) {
		if (uDataInBytes < uExpectedLength) {
			uExpectedLength = uDataInBytes;
		}
	}

	// is the expected size not the argument size?
	if (uDataInBytes != uExpectedLength) {
		SetWarning(WARN_LENGTH_SPECIFIER_MISMATCH);
	}
}

/*! ************************************

	\brief Store the precision or width value

	When a precision or width value is parsed, store it in the
	appropriate variable in the structure

	\param uStage Parsing stage to determine if parsing precision or width
	\param uCurrentNumValue Value to store
	\return Error code if the value is out of bounds.

***************************************/

Burger::eError BURGER_API Burger::SafePrint::ParamInfo_t::StoreOffNumericValue(
	eParseStage uStage, uint32_t uCurrentNumValue) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;
	if (uStage == PARSE_PRECISION) {
		if (uCurrentNumValue > kMaxPrintFPrecision) {
			uResult = kErrorInvalidParameter;
		} else {
			SetFlag(CONVFLAG_PRECISION_SPECIFIED);
			SetPrecision(uCurrentNumValue);
		}
	} else { // if ( stage	== PARSE_WIDTH) // store off the width parameter
		if (uCurrentNumValue > kMaxPrintFWidth) {
			uResult = kErrorOutOfBounds;
		} else {
			SetFlag(CONVFLAG_WIDTH_SPECIFIED);
			SetWidth(uCurrentNumValue);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Outputs an integer value

	Relevant flags and inputs:
	* * Width value	- if width > output length, fill with spaces
	* *	Left Justify flag - Left justifies the output
	* *	Display sign - forces a sign to be displayed
	* *	Blank sign - displays a blank in front of a positive number
	* *	Left Pad Zeros - fills zeros in front of the number
	* *	Comma flag - insert commas in front of every group of 3 digits

	Caveats:
	* *	Left Justify overrides padding with zeros
	* *	Comma Flag overrides padding with zeros
	* *	Display Sign overrides Blank Sign

	Composes numeric text from right to left (Except for padding)

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ParamInfo_t::FormatInteger(
	char* pOutBuffer, const ArgumentType* pArg) const BURGER_NOEXCEPT
{
	// sanity checks
	BURGER_ASSERT(pArg->IsInteger());

	// we can assume all inputs have been validated
	const uint_t bCommaFlag = IsFlagSet(CONVFLAG_COMMA_SEPARATE);
	const uint_t uPrecision = GetPrecision();

	// handle field width & left justify flags
	char* pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);

	// Point to last character - we're filling in backwards
	pOutText = pOutText + GetFormattedLength() - 1;

	const uint_t bIs64Bit = pArg->Is64Bit();
	const uint_t bIsNegative = pArg->IsNegative();
	const uint_t bHasSignChar = IsFlagSet(CONVFLAG_BLANK_SIGN) |
		IsFlagSet(CONVFLAG_DISPLAY_SIGN) | bIsNegative;

	uint32_t uNumber32 = 0;
	uint64_t uNumber64 = 0;
	if (!bIs64Bit) {
		uNumber32 = pArg->GetUInt32();
	} else {
		uNumber64 = pArg->GetUInt64();
	}

	uintptr_t uCharsWritten = 0;
	uint_t uDigitswritten = 0;
	uint_t uTotalDigitsWritten = 0;
	uint_t bEmitDigits = TRUE;

	// Special case: The result of converting a zero value with
	//  a precision of zero is no characters
	if (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED) && !uPrecision &&
		pArg->IsZero()) {
		bEmitDigits = FALSE;
	}

	// Loop to emit decimal digits, from right to left
	while (bEmitDigits) {
		uint_t uCurDigit;
		if (bIs64Bit) {
			uint64_t uRemainder64 = uNumber64 / 10ULL;
			uCurDigit = static_cast<uint_t>(uNumber64 - (uRemainder64 * 10ULL));
			uNumber64 = uRemainder64;
			bEmitDigits = (uNumber64 != 0);
		} else {
			uint32_t uRemainder32 = uNumber32 / 10U;
			uCurDigit = static_cast<uint_t>(uNumber32 - (uRemainder32 * 10U));
			uNumber32 = uRemainder32;
			bEmitDigits = (uNumber32 != 0);
		}

		// emit character
		*pOutText-- = static_cast<char>('0' + uCurDigit);
		++uCharsWritten;
		++uTotalDigitsWritten;
		++uDigitswritten;

		// Handle any
		if (bCommaFlag && (uDigitswritten == 3) && bEmitDigits) {
			*pOutText-- = kNUMERIC_GROUP_SEPARATOR;
			++uCharsWritten;
			uDigitswritten = 0;
		}
	}

	// take care of leading zeros caused by precision field
	uint_t bEmitLeadingZeros = (IsFlagSet(CONVFLAG_PRECISION_SPECIFIED) &&
		(uTotalDigitsWritten < uPrecision));
	while (bEmitLeadingZeros) {
		*pOutText-- = '0';
		++uCharsWritten;
		++uTotalDigitsWritten;
		++uDigitswritten;

		// update emit status
		bEmitLeadingZeros = (uTotalDigitsWritten < uPrecision);

		if (bCommaFlag && (uDigitswritten == 3) && bEmitLeadingZeros) {
			*pOutText-- = kNUMERIC_GROUP_SEPARATOR;
			++uCharsWritten;
			uDigitswritten = 0;
		}
	}

	// Take care of padding with zeros (no comma or left justify)
	if (IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
		char* pLastZero = pOutBuffer;
		if (bHasSignChar) {
			++pLastZero;
		}

		while (pOutText >= pLastZero) {
			*pOutText-- = '0';
			++uCharsWritten;
		}
	}

	// take care of writing out any sign character
	if (bHasSignChar) {
		*pOutText-- = static_cast<char>((bIsNegative) ? kNUMERIC_NEGATIVE_SIGN :
				(IsFlagSet(CONVFLAG_DISPLAY_SIGN))    ? kNUMERIC_POSITIVE_SIGN :
                                                        kNUMERIC_BLANK_SIGN);
		++uCharsWritten;
	}

	BURGER_ASSERT(uCharsWritten == GetFormattedLength());
	return GetOutputLength();
}

/*! ************************************

	\brief Outputs an integral value as octal or hex digits

	Outputs an integral value as octal or hex digits
	also handles pointer conversion

	Relevant flags and inputs:
	* * Width value - if width > output length, fill with spaces
	* * Left Justify flag - Left justifies the output
	* * Left Pad Zeros - fills zeros in front of the number
	* * Comma flag - insert commas? in front of every group of 3 digits
	* * AddBasePrefix - inserts base prefix in front of output

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ParamInfo_t::FormatHexOrOctal(
	char* pOutBuffer, const ArgumentType* pArg) const BURGER_NOEXCEPT
{
	// sanity checks
	BURGER_ASSERT(pArg->IsInteger());

	// we can assume all inputs have been validated
	const uint_t bCommaFlag = IsFlagSet(CONVFLAG_COMMA_SEPARATE);
	uint_t bAddBasePrefix = IsFlagSet(CONVFLAG_ALTERNATIVE_FORM);
	const uint_t bDisplayUpperCase = IsFlagSet(CONVFLAG_UPPERCASE);
	const uint_t bHasPrecision = IsFlagSet(CONVFLAG_PRECISION_SPECIFIED);
	const uint_t uPrecision = GetPrecision();

	// Special case for hex (but not pointer), zero value suppresses alternate
	// form prefix

	if (bAddBasePrefix && (GetConversion() == CONVSPEC_HEX) && pArg->IsZero()) {
		bAddBasePrefix = FALSE;
	}

	// handle field width & left justify flags
	char* pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);

	const uint_t bIsHex = (GetConversion() != CONVSPEC_OCTAL);

	uint_t uBitsToShift;
	uint32_t uDigitMask;
	uint_t uGroupBreakAt;

	if (bIsHex) {
		uBitsToShift = 4;
		uDigitMask = 0x0F;
		uGroupBreakAt = 4;
	} else {
		uBitsToShift = 3;
		uDigitMask = 0x07;
		uGroupBreakAt = 3;
	}

	const uint_t uPrefixDigits = (bAddBasePrefix && bIsHex) ? 2U : 0;

	const char* pOutputDigits = (bDisplayUpperCase) ? g_NibbleToAsciiUppercase :
                                                      g_NibbleToAsciiLowercase;

	// Point to last character - we're filling in backwards
	pOutText = pOutText + GetFormattedLength() - 1;

	const uint_t bIs64Bit = pArg->Is64Bit();
	uint32_t uNumber32 = 0;
	uint64_t uNumber64 = 0;
	if (!bIs64Bit) {
		uNumber32 = pArg->GetUInt32();
	} else {
		uNumber64 = pArg->GetUInt64();
	}

	uintptr_t uCharsWritten = 0;
	uint_t uDigitswritten = 0;
	uint_t uTotalDigitsWritten = 0;
	// for zero only, we'll emit a technically leading zero
	uint_t uLeadingZerosWritten = pArg->IsZero();
	uint_t bEmitDigits = TRUE;

	// Special case: The result of converting a zero value with
	//  a precision of zero is no characters
	if (bHasPrecision && !uPrecision && pArg->IsZero()) {
		bEmitDigits = FALSE;
		uLeadingZerosWritten = 0;
	}

	// Loop to emit decimal digits, from right to left
	while (bEmitDigits) {
		uint32_t uCurDigit;

		if (bIs64Bit) {
			uCurDigit = static_cast<uint32_t>(uNumber64) & uDigitMask;
			uNumber64 = uNumber64 >> uBitsToShift;
			bEmitDigits = (uNumber64 != 0);
		} else {
			uCurDigit = static_cast<uint32_t>(uNumber32) & uDigitMask;
			uNumber32 = uNumber32 >> uBitsToShift;
			bEmitDigits = (uNumber32 != 0);
		}

		// emit character
		*pOutText-- = pOutputDigits[uCurDigit];
		++uCharsWritten;
		++uDigitswritten;
		++uTotalDigitsWritten;

		// Handle any
		if (bCommaFlag && (uDigitswritten == uGroupBreakAt) && bEmitDigits) {
			*pOutText-- = kNUMERIC_GROUP_SEPARATOR;
			++uCharsWritten;
			uDigitswritten = 0;
		}
	}

	// take care of leading zeros caused by precision field
	uint_t bEmitLeadingZeros =
		(bHasPrecision && (uTotalDigitsWritten < uPrecision));
	while (bEmitLeadingZeros) {
		*pOutText-- = '0';
		++uCharsWritten;
		++uTotalDigitsWritten;
		++uDigitswritten;
		++uLeadingZerosWritten;

		// update emit status
		bEmitLeadingZeros = (uTotalDigitsWritten < uPrecision);

		if (bCommaFlag && (uDigitswritten == uGroupBreakAt) &&
			bEmitLeadingZeros) {
			*pOutText-- = kNUMERIC_GROUP_SEPARATOR;
			++uCharsWritten;
			uDigitswritten = 0;
		}
	}

	// Take care of padding with zeros (no comma or left justify)
	if (IsFlagSet(CONVFLAG_LEFT_PAD_ZEROS)) {
		while (pOutText >= (pOutBuffer + uPrefixDigits)) {
			*pOutText-- = '0';
			++uCharsWritten;
			++uLeadingZerosWritten;
		}
	}

	// Special case for octal: if Alternate Form ('#') is specified and no
	// leading zero has been written, then we need to add just one, so if
	// any zeros HAVE already been written, we turn off the zero prefix
	if (!bIsHex && bAddBasePrefix) {
		bAddBasePrefix = (!uLeadingZerosWritten);
	}

	// alternate form: add '0' for octal and '0x' for Hex/Pointer
	if (bAddBasePrefix) {
		if (bIsHex) {
			*pOutText-- = ((bDisplayUpperCase) ? 'X' : 'x');
			++uCharsWritten;
		}
		*pOutText-- = '0';
		++uCharsWritten;
	}
	BURGER_ASSERT(uCharsWritten == GetFormattedLength());
	return GetOutputLength();
}

/*! ************************************

	\brief Outputs an pointer as a hex address value

	Relevant flags and inputs:
	* * Width value - if width > output length, fill with spaces
	* * Left Justify flag - Left justifies the output
	* * DisplayUpperCase - modifies textual output

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ParamInfo_t::FormatPointer(
	char* pOutBuffer, const ArgumentType* pArg) const BURGER_NOEXCEPT
{
	// sanity checks
	BURGER_ASSERT(pArg->IsPointer());

	// make a copy of the output command as hex output with full precision
	ParamInfo_t TempParamInfo(*this);
	TempParamInfo.SetPrecision(static_cast<uint_t>(2U * sizeof(uintptr_t)));
	TempParamInfo.SetFlag(CONVFLAG_PRECISION_SPECIFIED);

	// make a copy of the argument as a Unsigned int
#if UINTPTR_MAX == 0xFFFFFFFFU
	ArgumentType TempArg(reinterpret_cast<uint32_t>(pArg->m_Data.m_pVoid));
	TempArg.SetType(kArgumentTypeUInt32);
#else
	ArgumentType TempArg(reinterpret_cast<uint64_t>(pArg->m_Data.m_pVoid));
	TempArg.SetType(kArgumentTypeUInt64);
#endif
	return TempParamInfo.FormatHexOrOctal(pOutBuffer, &TempArg);
}

/*! ************************************

	\brief Outputs a character value

	Relevant flags and inputs:
	* * Left Justify flag	- left justifies the output in the field
	* * Width value			- if width > output length, fill with spaces or
zeros
	* * Left Justify flag	- obvious flag is obvious

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ParamInfo_t::FormatChar(
	char* pOutBuffer, const Burger::ArgumentType* pArg) const BURGER_NOEXCEPT
{
	// sanity checks
	BURGER_ASSERT(pArg->IsCharacter());

	// special case check - we will have an output length of zero if
	// we have an invalid character conversion.  Width doesn't matter
	if (!GetOutputLength()) {
		return 0;
	}

	// handle field width & left justify flags
	char* pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);
	uintptr_t uCharsWritten = 0;

	// do we need to convert a wide char or a regular char?
	if (IsFlagSet(CONVFLAG_INTERPRET_LONG)) {
		// skip the wchar_t conversion if we already determined it's invalid
		uintptr_t uMaxLength = GetFormattedLength();
		if (uMaxLength) {

			// The function from_UTF16 adds a terminating zero, this is not
			// desired. So, parse into a temp buffer and then copy
			// the characters into the output to prevent a buffer
			// overrun by the terminating zero.

			char TempBufferUTF8[8];
			if (pArg->GetDataLengthInBytes() == 2) {
				uCharsWritten = UTF8::from_UTF16(
					TempBufferUTF8, static_cast<uint16_t>(pArg->GetUInt32()));
			} else {
				uCharsWritten = Burger::UTF8::from_UTF32(
					TempBufferUTF8, static_cast<uint32_t>(pArg->GetUInt32()));
			}
			if (uCharsWritten > uMaxLength) {
				uCharsWritten = uMaxLength;
			}
			if (uCharsWritten) {
				uintptr_t i = 0;
				do {
					pOutText[i] = TempBufferUTF8[i];
				} while (++i < uCharsWritten);
			}
		}
	} else {
		// Copy the single character
		*pOutText = pArg->GetChar();
		uCharsWritten = 1;
	}
	BURGER_ASSERT(uCharsWritten == GetFormattedLength());
	return GetOutputLength();
}

/*! ************************************

	\brief Outputs a text string

	Relevant flags and inputs:
	* * Width value - if width > output length, fill with spaces
	* * Left Justify flag - Left justifies the output

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ParamInfo_t::FormatTextString(
	char* pOutBuffer, const ArgumentType* pArg) const BURGER_NOEXCEPT
{
	// sanity checks
	BURGER_ASSERT(pArg->IsTextPointer());

	// handle field width & left justify flags
	char* pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);
	uintptr_t uCharsWritten = GetFormattedLength();

	// copy the text string (if we determined we have any text)
	if (uCharsWritten) {
		if (pArg->GetType() == kArgumentTypeUInt16Ptr) {
			if (pArg->m_Data.m_pUInt16) {
				const char Temp = pOutText[uCharsWritten];
				UTF8::from_UTF16(
					pOutText, uCharsWritten + 1, pArg->m_Data.m_pUInt16);
				pOutText[uCharsWritten] = Temp;
			} else {
				MemoryCopy(pOutText, g_NullString, uCharsWritten);
			}
		} else {
			const char* pSrcText = pArg->GetText();
			if (!pSrcText) {
				pSrcText = g_NullString;
			}
			MemoryCopy(pOutText, pSrcText, uCharsWritten);
		}
	}
	return GetOutputLength();
}

/*! ************************************

	\brief Outputs a boolean values value as "true"/"false"

	Relevant flags and inputs:
	* * Width value - if width > output length, fill with spaces
	* * Left Justify flag - obvious flag is obvious
	* * DisplayUpperCase - modifies textual output

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ParamInfo_t::FormatBool(
	char* pOutBuffer, const ArgumentType* pArg) const BURGER_NOEXCEPT
{
	// we can assume all inputs have been validated
	const uintptr_t bIsValueZero = pArg->IsZero();
	const uintptr_t uNumChars = 4 + bIsValueZero;

	// sanity checks
	BURGER_ASSERT(GetFormattedLength() == uNumChars);
	BURGER_ASSERT(pArg->IsNumeric());

	// handle field width & left justify flags
	char* pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);
	const char* pSrcText = (IsFlagSet(CONVFLAG_UPPERCASE)) ?
        (bIsValueZero) ? "FALSE" : "TRUE" :
		(bIsValueZero) ? "false" :
                         "true";
	MemoryCopy(pOutText, pSrcText, uNumChars);
	return GetOutputLength();
}

/*! ************************************

	\brief Output a data value as a binary bit pattern, MSB to LSB

	Relevant flags and inputs:
	* * Width value - if width > output length, fill with spaces
	* * Left Justify flag - obvious flag is obvious
	* * Comma flag - inserts spaces between each nibbles worth of digits
	* * Alternative form - display bits LSB to MSB

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ParamInfo_t::FormatBinary(
	char* pOutBuffer, const ArgumentType* pArg) const BURGER_NOEXCEPT
{
	// we can assume all inputs have been validated
	const uint_t bCommaFlag = IsFlagSet(CONVFLAG_COMMA_SEPARATE);

	// Display the bits normally, or reversed
	const uint_t bSwapBits = IsFlagSet(CONVFLAG_ALTERNATIVE_FORM);

	// Display the BYTES normally or reversed
	uint_t bBigEndian;

#if defined(BURGER_BIGENDIAN)
	bBigEndian = !bSwapBits;
#else
	bBigEndian = bSwapBits;
#endif

	// determine how many bytes to process and data characters we are to output
	uintptr_t uByteCount = pArg->GetDataLengthInBytes();
	const uint8_t* pBinaryData =
		static_cast<const uint8_t*>(pArg->GetDataAddress());

	// handle field width & left justify flags
	char* pBinDigits = pOutBuffer + PadFieldWidth(pOutBuffer);

	// sanity checks
	BURGER_ASSERT(GetFormattedLength() ==
		((bCommaFlag) ? ((10U * uByteCount) - 1) : 8U * uByteCount));

	// Assume little endian

	uintptr_t uOffset = uByteCount - 1U;
	intptr_t iDelta = -1;

	// alternative form, we start at the Least significant byte, going towards
	// the most
	if (bBigEndian) {
		uOffset = 0;
		iDelta = 1;
	}

	// convert data bytes into bits
	while (uByteCount) {
		const uint8_t uTheByte = pBinaryData[uOffset];
		uint8_t uBitMask = static_cast<uint8_t>((bSwapBits) ? 0x01U : 0x80U);
		uint_t uDigitsWritten = 0;

		do {
			*pBinDigits++ = (uTheByte & uBitMask) ? '1' : '0';
			++uDigitsWritten;

			if (bCommaFlag) {
				if ((uDigitsWritten == 4) ||
					((uDigitsWritten == 8) && (uByteCount > 1))) {
					*pBinDigits++ = kBINARY_GROUP_SEPARATOR;
				}
			}

			// slide the bitmask over one bit
			if (bSwapBits) {
				uBitMask <<= 1U;
			} else {
				uBitMask >>= 1U;
			}
		} while (uBitMask);
		// move to next byte according to endian-ness
		uOffset += iDelta;
		--uByteCount;
	}
	return GetOutputLength();
}

/*! ************************************

	\brief Print NaN's and Infinity

	Helper function to write out the text for special
	floating point values: NaN's and Infinity

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param uSpecialResult Special floating point number to print
	\return Number of characters printed

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ParamInfo_t::FormatSpecialReal(
	char* pOutBuffer, FPPrintInfo::eResult uSpecialResult) const BURGER_NOEXCEPT
{
	// ISO Spec Infinity/NaN - look up the correct string
	uintptr_t uIndex = static_cast<uintptr_t>(
		uSpecialResult - FPPrintInfo::kResultFirstSpecial);
	const char* pText = (IsFlagSet(CONVFLAG_UPPERCASE)) ?
        FPPrintInfo::g_SpecialFloatsTextUpper[uIndex] :
        FPPrintInfo::g_SpecialFloatsTextLower[uIndex];

	// copy out the special string to the output buffer
	uintptr_t uCharsWritten = 0;
	while (*pText) {
		*pOutBuffer++ = *pText++;
		++uCharsWritten;
	}
	BURGER_ASSERT(uCharsWritten == GetFormattedLength());
	return GetOutputLength();
}

/*! ************************************

	\brief Outputs a floating point value

	Relevant flags and inputs:
	* * Width value - if width > output length, fill with spaces
	* * Precision Value - minimum number of characters/digits to emit
	* * Left Justify flag - Left justifies the output
	* * Display sign - forces a sign to be displayed
	* * Blank sign - displays a blank in front of a positive number
	* * Left Pad Zeros - fills zeros in front of the number
	* * Comma flag - insert commas in front of every group of 3 digits

	Caveats:
	* * Left Justify overrides padding with zeros
	* * Comma Flag overrides padding with zeros

	\param pOutBuffer Pointer to the buffer to store the printed string
	\param pArg Pointer to the argument value
	\return Number of characters printed

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ParamInfo_t::FormatReal(
	char* pOutBuffer, const ArgumentType* pArg) const BURGER_NOEXCEPT
{
	// sanity checks
	BURGER_ASSERT(pArg->IsReal());

	// we can assume all inputs have been validated
	const uint_t bCommaFlag = IsFlagSet(CONVFLAG_COMMA_SEPARATE);
	const uint_t bBlankSign = IsFlagSet(CONVFLAG_BLANK_SIGN);

	// handle field width & left justify flags
	char* pOutText = pOutBuffer + PadFieldWidth(pOutBuffer);
	uintptr_t uCharsWritten = 0;

	// all these will be initialized with cached off values
	uint_t uIntDigits, uFracDigits, uSign, uDecimal, uPadDigits,
		uExponentDigits;
	uint_t uPadIntZeros, uPadFracZeros, uFirstNonZeroIntPos;
	FPPrintInfo::eResult uPrintResult;

	// get all the cached info on what we will be outputting
	GetFloatInfo(&uIntDigits, &uFracDigits, &uSign, &uDecimal, &uPadDigits,
		&uExponentDigits);
	GetFloatAnalysisInfo(
		&uPrintResult, &uPadIntZeros, &uPadFracZeros, &uFirstNonZeroIntPos);

	// Break the real number up into mantissa, exponent, etc
	FPInfo TheFPInfo;
	switch (pArg->GetType()) {
	case kArgumentTypeFloat16:
		TheFPInfo = pArg->m_Data.m_fHalf;
		break;
	case kArgumentTypeFloat32:
		TheFPInfo = pArg->m_Data.m_fFloat;
		break;
	case kArgumentTypeFloat64:
		TheFPInfo = pArg->m_Data.m_dDouble;
		break;
	default:
		BURGER_ASSERT(FALSE);
		return 0;
	}

	// We check for the special cases (infinity, NANs) first
	if ((uPrintResult >= FPPrintInfo::kResultFirstSpecial) &&
		(uPrintResult <= FPPrintInfo::kResultLastSpecial)) {
		FormatSpecialReal(pOutText, uPrintResult);
		return GetOutputLength();
	}

	// printing a sign character?
	if (TheFPInfo.is_negative() || IsFlagSet(CONVFLAG_DISPLAY_SIGN) ||
		bBlankSign) {
		uSign = 1;
	}

	// how many commas are we inserting?
	uint_t uCommaCount =
		(bCommaFlag && (uIntDigits > 3U)) ? (uIntDigits - 1U) / 3U : 0;

	// Is this a series of all '9's rounded up to '1' (plus zeros?)
	// Alternatively, is this a true zero value?
	if ((uPrintResult == FPPrintInfo::kResultFloatRoundedUpADigit) ||
		(uPrintResult == FPPrintInfo::kResultZero)) {
		char* pTextPtr = pOutText;
		if (uSign) {
			*pTextPtr++ = static_cast<char>((TheFPInfo.is_negative()) ?
                                   kNUMERIC_NEGATIVE_SIGN :
					(bBlankSign) ? kNUMERIC_BLANK_SIGN :
                                   kNUMERIC_POSITIVE_SIGN);
			++uCharsWritten;
		}

		// any leading zeros to pad the field?
		while (uPadDigits) {
			*pTextPtr++ = '0';
			++uCharsWritten;
			--uPadDigits;
		}

		// to do.. add numeric separator support
		BURGER_ASSERT(uIntDigits);
		uint_t uZerosToWrite = uIntDigits;

		if (uPrintResult == FPPrintInfo::kResultFloatRoundedUpADigit) {
			*pTextPtr++ = '1';
			--uZerosToWrite;
			++uCharsWritten;
		}

		while (uZerosToWrite) {
			*pTextPtr++ = '0';
			--uZerosToWrite;
			++uCharsWritten;
		}

		if (uDecimal) {
			*pTextPtr++ = kNUMERIC_DECIMAL_POINT;
			++uCharsWritten;
		}

		while (uFracDigits) {
			*pTextPtr++ = '0';
			--uFracDigits;
			++uCharsWritten;
		}

		BURGER_ASSERT(uCharsWritten == GetFormattedLength());
		return GetOutputLength();
	}

	// setup a high precision object big enough to hold the number
	const uint32_t bitsNeeded = (1U << (TheFPInfo.get_exponent_bit_count() - 1)) +
		TheFPInfo.get_mantissa_bit_count();
	FPLargeInt intPart(bitsNeeded);
	FPLargeInt fracPart(bitsNeeded);

	separate_integer_fraction(&intPart, &fracPart, &TheFPInfo);

	// we'll be writing this out right to left
	// either padDigits or commas will be zero
	char* pIntDigit =
		pOutText + uSign + uPadDigits + uIntDigits + uCommaCount - 1;
	char* pLastDigit = pIntDigit;

	// write out any sign character
	if (uSign) {
		*pOutText = static_cast<char>((TheFPInfo.is_negative()) ?
                               kNUMERIC_NEGATIVE_SIGN :
				(bBlankSign) ? kNUMERIC_BLANK_SIGN :
                               kNUMERIC_POSITIVE_SIGN);
		++uCharsWritten;
	}

	// do we have any leading zeros to write out?
	if (uPadDigits) {
		for (uint_t n = 0; n < uPadDigits; n++) {
			pOutText[n + uSign] = '0';
			++uCharsWritten;
		}
	}

	// Output the integer portion a digit at a time, right to left
	uint_t uZeroFirstDigits = uPadIntZeros; // needed for RTL cut-off emulation
	uint_t uIntDigitsReturned = 0;
	uint_t uDigitsInGroup = 0;
	uint_t uFracDigitsReturned = 0;

	do {
		// sanity check
		BURGER_ASSERT(pIntDigit >= pOutText);

		char theDigit = static_cast<char>(intPart.divide_return_remainder(10));
		++uIntDigitsReturned;

		if (uZeroFirstDigits) {
			theDigit = 0;
			--uZeroFirstDigits;
		}

		// need to put a group separator in before (after) the digit?
		if (bCommaFlag && (uDigitsInGroup == 3)) {
			*pIntDigit-- = kNUMERIC_GROUP_SEPARATOR;
			++uCharsWritten;
			uDigitsInGroup = 0;
		}

		// do we need to grab the pointer to this digit for later rounding up?
		if (uIntDigitsReturned == uFirstNonZeroIntPos) {
			pLastDigit = pIntDigit;
		}

		*pIntDigit = static_cast<char>('0' + theDigit);
		--pIntDigit;

		++uDigitsInGroup;
		++uCharsWritten;
	} while (intPart.is_not_zero());

	BURGER_ASSERT(uIntDigits == uIntDigitsReturned);

	// do we output a decimal point?   if we have fractional digits, we must
	if (uDecimal) {
		// compute the position of the decimal point, and write it out
		char* pDecimalPt =
			pOutText + uSign + uPadDigits + uIntDigits + uCommaCount;
		*pDecimalPt = kNUMERIC_DECIMAL_POINT;
		++uCharsWritten;

		// see how many fractional digits to actually calculate
		const uint_t uRealFracDigits = uFracDigits - uPadFracZeros;
		char* pFracDigit = ++pDecimalPt;

		// if we have fractional digits, then output them left to right
		if (uRealFracDigits) {
			while (uFracDigitsReturned < uRealFracDigits) {
				char theDigit =
					static_cast<char>(fracPart.multiply_return_overflow(10));
				++uFracDigitsReturned;

				// note last written digit position if we have to round up
				pLastDigit = pFracDigit;

				*pFracDigit++ = static_cast<char>('0' + theDigit);
				++uCharsWritten;
			}
		}

		// for RTL cutoff emulation, we pad zeros on the end after the cut-off
		while (uPadFracZeros) {
			*pFracDigit++ = '0';
			++uCharsWritten;
			--uPadFracZeros;
		}
	}

	// do we need to round the result up?  if so we have a pointer to
	// the rightmost significant digit.  We will increment that digit,
	// if there is a carry, move left, ignoring separators & decimals
	// until nothing carries.  We don't worry about it adding a digit to
	// the beginning of the number, as the 'all 9s' case has been
	// accounted for separately.
	if (uPrintResult == FPPrintInfo::kResultFloatRoundedUpAtEnd) {
		while (pLastDigit) { // suppress warning by checking pointer...
			uint8_t theDigit = static_cast<uint8_t>(*pLastDigit - '0');

			BURGER_ASSERT(theDigit <= 9U);

			// '0' through '8' means increment digit and we are done
			if (theDigit != 9) {
				// failsafe in case we find something that's not a number
				// (shouldn't happen)
				if (theDigit < 9U) {
					*pLastDigit = static_cast<char>('0' + theDigit + 1);
				}
				break;
			} else {
				// the digit is a '9', which means we increment, get '10', write
				// the '0' and carry over the 1 to the next digit to the left
				*pLastDigit = '0';
				--pLastDigit; // back up a digit

				// skip over the decimal point or any group separators
				if ((*pLastDigit ==
						static_cast<char>(kNUMERIC_DECIMAL_POINT)) ||
					(*pLastDigit ==
						static_cast<char>(kNUMERIC_GROUP_SEPARATOR))) {
					pLastDigit--;
				}
			}

			// sanity check
			BURGER_ASSERT(pLastDigit >= pOutText);
		}
	}

	BURGER_ASSERT(uCharsWritten == GetFormattedLength());
	return GetOutputLength();
}

/*! ************************************

	\struct Burger::SafePrint::ProcessResults_t

	\brief struct to hold the results of parsing.

	struct to hold the results of the parsing phase and
	output phases of a formatted output operation

***************************************/

/*! ************************************

	\brief Error handler

	Your opportunity to do additional error handling
	and reporting.  The format string and parameters that were passed in
	are provided to this function so that detailed information on exactly
	what the error was

	\param pFormatString Pointer to the original format string
	\param uArgCount Count of arguments in array
	\param ppArgs Array of parsed arguments
	\return Error code (Always negative)

***************************************/

intptr_t BURGER_API Burger::SafePrint::ProcessResults_t::ErrorHandler(
	const char* pFormatString, uintptr_t uArgCount,
	const ArgumentType** ppArgs) BURGER_NOEXCEPT
{
	BURGER_UNUSED(pFormatString);
	BURGER_UNUSED(uArgCount);
	BURGER_UNUSED(ppArgs);

	// Uncomment to get a text description of the error that occurred
	// const char* pErrorMsgText = GetErrorDescription(m_uPhaseResults);

	// return the error value to be returned to the original caller
	return static_cast<intptr_t>(m_uPhaseResults);
}

/*! ************************************

	\brief Output argument text

	Given an argument, the output buffer is sanity checked and then the argument
	is converted to text and stored in the output buffer.

	\param pOutputBuffer Pointer to the output buffer
	\param uOutputBufferSize Number of bytes in the output buffer
	\param pParamInfo Pointer to the structure with the information on how to
		print out the argument
	\param pArgument Pointer to the structure with the argument data

	\return Number of bytes outputted

***************************************/

uintptr_t BURGER_API Burger::SafePrint::ProcessResults_t::FormatArgument(
	char* pOutputBuffer, uintptr_t uOutputBufferSize,
	const ParamInfo_t* pParamInfo,
	const ArgumentType* pArgument) BURGER_NOEXCEPT
{
	uintptr_t uCharCount = 0;

	// Sanity check
	if (IsPointerInvalid(pOutputBuffer) || IsPointerInvalid(pParamInfo) ||
		IsPointerInvalid(pArgument) || !uOutputBufferSize) {
		m_uPhaseResults = kErrorInvalidParameter;

	} else {
		const eConversionSpecifier uConversionSpecifier =
			pParamInfo->GetConversion();
		const eArgumentType uArgumentType = pArgument->GetType();
		if ((uConversionSpecifier == CONVSPEC_INVALID) ||
			(uConversionSpecifier >= CONVSPEC_COUNT)) {
			m_uPhaseResults = kErrorInvalidArgument;

		} else {

			const uint32_t uConversionAsFlag =
				pParamInfo->GetConversionAsFlag();
			if (!(g_ValidOutputDisplayTypesStrict[uArgumentType] &
					uConversionAsFlag) &&
				!(g_ValidOutputDisplayTypesRelaxed[uArgumentType] &
					uConversionAsFlag)) {
				m_uPhaseResults = kErrorInvalidArgument;
			} else {

				// check remaining output width vs formatted argument length

				switch (uConversionSpecifier) {
				case CONVSPEC_BINNUMBER:
					uCharCount =
						pParamInfo->FormatBinary(pOutputBuffer, pArgument);
					break;

				case CONVSPEC_BOOLTEXT:
					uCharCount =
						pParamInfo->FormatBool(pOutputBuffer, pArgument);
					break;

				case CONVSPEC_CHAR:
					uCharCount =
						pParamInfo->FormatChar(pOutputBuffer, pArgument);
					break;

				case CONVSPEC_DECIMAL_INT:
				case CONVSPEC_DECIMAL_WORD:
					uCharCount =
						pParamInfo->FormatInteger(pOutputBuffer, pArgument);
					break;

				case CONVSPEC_DECIMAL_FLOAT:
				case CONVSPEC_SCI_NOTE_FLOAT:
				case CONVSPEC_SHORTER_FLOAT:
					uCharCount =
						pParamInfo->FormatReal(pOutputBuffer, pArgument);
					break;

				case CONVSPEC_OCTAL:
					uCharCount =
						pParamInfo->FormatHexOrOctal(pOutputBuffer, pArgument);
					break;

				case CONVSPEC_HEX:
					uCharCount =
						pParamInfo->FormatHexOrOctal(pOutputBuffer, pArgument);
					break;

				case CONVSPEC_POINTER:
					uCharCount =
						pParamInfo->FormatPointer(pOutputBuffer, pArgument);
					break;

				case CONVSPEC_TEXT_STRING:
					uCharCount =
						pParamInfo->FormatTextString(pOutputBuffer, pArgument);
					break;

				default:
					break;
				}
			}
		}
	}
	return uCharCount;
}

/*! ************************************

	\brief Output formatted text

	Given a format string and a list of processed arguments,
	output the final string into the output buffer. If there are issues
	with the output or if the buffer is exhausted, parsing terminates.

	\param pOutputBuffer Pointer to the output buffer
	\param uOutputBufferSize Number of bytes in the output buffer
	\param uArgCount Number of available arguments
	\param ppArgs Pointer to the array of pointers to arguments
	\param uParamInfoCount Number of parameter commands to process
	\param pParamInfos Pointer to an array of parameter commands

	\return Number of bytes outputted

***************************************/

uintptr_t BURGER_API
Burger::SafePrint::ProcessResults_t::GenerateFormatOutputToBuffer(
	char* pOutputBuffer, uintptr_t uOutputBufferSize, uintptr_t uArgCount,
	const ArgumentType** ppArgs, uintptr_t uParamInfoCount,
	const ParamInfo_t* pParamInfos) BURGER_NOEXCEPT
{
	BURGER_UNUSED(uArgCount);
	uintptr_t uCharCount = 0;

	// Sanity checks.  It should be impossible to call this function with any of
	// these bad
	BURGER_ASSERT(IsPointerValid(pOutputBuffer) && uOutputBufferSize);
	BURGER_ASSERT(uParamInfoCount && IsPointerValid(pParamInfos));
	BURGER_ASSERT(!(uArgCount && IsPointerInvalid(ppArgs)));

	// go through all the output commands we got from the preprocessing phase

	for (uintptr_t x = 0; x < uParamInfoCount; x++) {
		switch (pParamInfos->GetCommand()) {
		case COMMAND_COPY_TEXT: {
			uintptr_t uBytesToCopy = pParamInfos->GetOutputLength();
			const char* pSrcText = pParamInfos->m_pText;

			if ((uCharCount + uBytesToCopy) > uOutputBufferSize) {
				uBytesToCopy = uOutputBufferSize - uCharCount;
			}

			// can we do it fast?
			if (pParamInfos->IsFlagSet(CONVFLAG_HAS_SEQUENCE_CHAR)) {
				// nope.  copy a character at a time, looking for the '%%'
				// sequence
				char* pOutText = &pOutputBuffer[uCharCount];
				while (uBytesToCopy) {
					char c = *pSrcText++;
					if (c == '%') {
						pSrcText++; // skip second %
					}
					*pOutText++ = c;
					--uBytesToCopy;
					++uCharCount;
				}
			} else {
				// fast path, we don't have '%%' sequences to convert
				MemoryCopy(pOutputBuffer + uCharCount, pSrcText, uBytesToCopy);
				uCharCount += uBytesToCopy;
			}
			break;
		}

		case COMMAND_FORMAT_ARGUMENT: {
			const uintptr_t uCharsAdded = FormatArgument(
				&pOutputBuffer[uCharCount], uOutputBufferSize - uCharCount,
				pParamInfos, ppArgs[pParamInfos->GetArgIndex()]);
			if (m_uPhaseResults != kErrorNone) {
				return uCharCount;
			}
			uCharCount += uCharsAdded;
			break;
		}

		default:
			m_uPhaseResults = kErrorInvalidParameter;
			m_uErrorCharPos = uCharCount;
			return uCharCount;
		}

		// did we fill up the output buffer?
		if (uCharCount >= uOutputBufferSize) {
			break;
		}
		++pParamInfos;
	}
	return uCharCount;
}

/*! ************************************

	\brief Write data to ANSI FILE record

	Write a data buffer to a FILE

	\param fp Pointer to the FILE record
	\param pBuffer Pointer to the buffer to write to the FILE
	\param uBufferSize Number of bytes in the buffer to write
	\param uCharsSoFar Number of bytes written so far (Needed for error
		updating)

	\return \ref TRUE if successful, \ref FALSE on failure

***************************************/

uint_t BURGER_API Burger::SafePrint::ProcessResults_t::WriteBufferToFile(
	FILE* fp, const char* pBuffer, uintptr_t uBufferSize,
	uintptr_t uCharsSoFar) BURGER_NOEXCEPT
{
	uint_t uResult = TRUE;
	// somehow get asked to send nothing?
	if (uBufferSize) {

		BURGER_ASSERT(IsPointerValid(pBuffer));

		// We write the buffer using fwrite.  In the case of printf, outputFile
		// will be set to stdin
		const uintptr_t uCharsPassed = fwrite(pBuffer, 1, uBufferSize, fp);

		// did the RTL function report something other than expected success?
		if (uCharsPassed != uBufferSize) {
			m_uPhaseResults = kErrorWriteFailure;
			m_uErrorCharPos = uCharsSoFar + uCharsPassed;
			uResult = FALSE;
		}
	}
	// guess we're ok
	return uResult;
}

/*! ************************************

	\brief Output formatted text to a file

	Given a format string and a list of processed arguments, output the final
	string into a FILE. If there are issues with the output or if the buffer is
	exhausted, parsing terminates.

	\param fp Pointer to the FILE
	\param uArgCount Number of available arguments
	\param ppArgs Pointer to the array of pointers to arguments
	\param uParamInfoCount Number of parameter commands to process
	\param pParamInfos Pointer to an array of parameter commands

	\return Number of bytes outputted

***************************************/

uintptr_t BURGER_API
Burger::SafePrint::ProcessResults_t::GenerateFormattedOutputToFile(FILE* fp,
	uintptr_t uArgCount, const ArgumentType** ppArgs, uintptr_t uParamInfoCount,
	const ParamInfo_t* pParamInfos) BURGER_NOEXCEPT
{
	BURGER_UNUSED(uArgCount);
	uintptr_t uCharCount = 0;

	// Sanity checks.  It should be impossible to call this function with any of
	// these bad
	BURGER_ASSERT(uParamInfoCount && IsPointerInvalid(pParamInfos));
	BURGER_ASSERT(!(uArgCount && IsPointerInvalid(ppArgs)));

	// get a work buffer on the stack
	char Buffer[kMAX_CONVERSION_OUTPUT_SIZE + 1];

	// go through all the output commands we got from the preprocessing phase
	for (uintptr_t x = 0; x < uParamInfoCount; x++) {
		switch (pParamInfos->GetCommand()) {
		case COMMAND_COPY_TEXT: {
			uintptr_t uBytesToCopy = pParamInfos->GetOutputLength();
			const char* pSrcText = pParamInfos->m_pText;

			// can we do it fast?
			if (pParamInfos->IsFlagSet(CONVFLAG_HAS_SEQUENCE_CHAR)) {
				// nope.  we'll have to copy as much of the text to our work
				// buffer as we can, converting the '%%' to '%' as we go, then
				// sending it to the output device when done or when we fill up
				// the buffer

				uintptr_t uSrcBytesRemaining = uBytesToCopy;
				uintptr_t uBufferUsed = 0;

				while (uSrcBytesRemaining) {
					// copy a block of text to our work buffer
					while ((uBufferUsed < kMAX_CONVERSION_OUTPUT_SIZE) &&
						uSrcBytesRemaining) {
						const char c = *pSrcText++;
						if (c == '%') {
							++pSrcText;           // skip second %
							--uSrcBytesRemaining; // keep count correct
						}
						Buffer[uBufferUsed++] = c;
						--uSrcBytesRemaining;
					}

					// write out what we've got in the work buffer
					if (uBufferUsed) {
						if (!WriteBufferToFile(
								fp, pSrcText, uBufferUsed, uCharCount)) {
							return uCharCount;
						}

						uCharCount += uBufferUsed; // update stats
						uBufferUsed = 0;           // reset buffer
					}
				}
			} else { // fast path, we don't have '%%' sequences to convert
				if (!WriteBufferToFile(
						fp, pSrcText, uBytesToCopy, uCharCount)) {
					return uCharCount;
				}
				uCharCount += uBytesToCopy;
			}
			break;
		}

		case COMMAND_FORMAT_ARGUMENT: {
			// write the formatted argument to our work buffer
			uintptr_t uCharsAdded =
				FormatArgument(Buffer, kMAX_CONVERSION_OUTPUT_SIZE, pParamInfos,
					ppArgs[pParamInfos->GetArgIndex()]);

			if (m_uPhaseResults != kErrorNone) {
				return uCharCount;
			}

			// this sucks because if a string passed into '%s' is greater than
			// the work buffer size, we have to fail idea: maybe we could
			// special case it?
			if (uCharsAdded > kMAX_CONVERSION_OUTPUT_SIZE) {
				m_uPhaseResults = kErrorBufferTooSmall;
				return uCharCount;
			}

			Buffer[uCharsAdded] = 0;

			// Send the formatted output to stdout or the FILE*
			if (!WriteBufferToFile(fp, Buffer, uCharsAdded, uCharCount)) {
				return uCharCount;
			}
			uCharCount += uCharsAdded;
			break;
		}

		default:
			m_uPhaseResults = kErrorInvalidArgument;
			m_uErrorCharPos = uCharCount;
			return uCharCount;
		}
		++pParamInfos;
	}
	return uCharCount;
}

/*! ************************************

	\brief Analyzes a format string, and creates a list
		of output commands that would created the output using it

	Output commands indicated either a section of text to literally* copy,
	or an argument to evaluate and convert (format).

	Arguments and their corresponding conversion sequences are checked for
	syntactic errors and type compatibility

	It can optionally suppress all argument checking - useful for quick error
	checking externally supplied format strings at load time

	Returns results in \ref ProcessResults_t :
	* * Success in parsing the format string ( and arguments )
	* * Accurate total OR estimated length of formatted output in bytes
	* * Number of output commands created
	* * Info on the format sequence that caused an error (if any)

	\param uOptions
	\param pFormat
	\param uFormatLength
	\param uArgCount Number of available arguments
	\param ppArgs Pointer to the array of pointers to arguments
	\param uParamInfoCount Number of parameter commands to process
	\param pParamInfos Pointer to an array of parameter commands

	\return \ref TRUE if successful in parsing, \ref FALSE if there was an error

***************************************/

uint_t BURGER_API Burger::SafePrint::ProcessResults_t::FormatPreProcess(
	eParseOptions uOptions, const char* pFormat, uintptr_t uFormatLength,
	uintptr_t uArgCount, const ArgumentType** ppArgs, uintptr_t uParamInfoCount,
	ParamInfo_t* pParamInfos) BURGER_NOEXCEPT
{
	// Initialize return values

	m_uPhaseResults = kErrorGeneric;
	m_uFormattedOutputLength = 0;
	m_uNumOutputCommands = 0;
	m_uErrorFormatSequencePos = UINTPTR_MAX;
	m_uErrorCharPos = UINTPTR_MAX;

	// Check format string for validity. An empty string (just a null) is ok
	if (IsPointerInvalid(pFormat)) {
		m_uPhaseResults = kErrorInvalidParameter;
		return false;
	}

	// Check the format string for size problems
	if (uFormatLength >= kMAX_TOTAL_OUTPUT_SIZE) {
		m_uPhaseResults = kErrorBufferTooLarge;
		return false;
	}

	// check arguments for validity.  Should never see an error here, but this
	// catches new bugs introduced
	if ((uOptions & PARSEOPTION_CHECK_ARG_TYPES) && uArgCount) {
		// do we have have crap data passed in?  should be impossible to trigger
		// this error
		if (uArgCount && IsPointerInvalid(ppArgs)) {
			m_uPhaseResults = kErrorInvalidParameter;
			return false;
		}

		// check each argument to make sure the CprintfArg object looks ok
		for (uintptr_t i = 0; i < uArgCount; i++) {
			if (IsPointerInvalid(ppArgs[i]) ||
				(ppArgs[i]->GetType() == kArgumentTypeInvalid) ||
				(ppArgs[i]->GetType() >= kArgumentTypeCount)) {
				m_uPhaseResults = kErrorInvalidParameter;
				return false;
			}
		}
	}

	// Initialize the parsing engine

	// index of character currently being parsed
	uintptr_t uParsePosition = 0;

	// index of 1st character of the argument format section being parsed
	uintptr_t uFormatSequencePosition = 0;

	// index of the current output command being composed
	uintptr_t uCurrentCommandIndex = UINTPTR_MAX;

	// index of the next argument to use for formatting
	uint32_t uCurrentArgumentNext = 0;

	// parsing mode: decoding an argument format section
	uint_t bParsingParam = false;

	// parsing mode: scanning literal text to copy
	uint_t bParsingText = false;

	// arg parsing: has an explicit argument index been specified?
	uint_t bArgIndexSpecified = false;

	// arg parsing: are we parsing a numeric value?
	uint_t bParsingNumVal = false;

	// arg parsing: do we have a valid numeric value to use
	uint_t bCurNumValueValid = false;

	// arg parsing: value of the number begin parsed
	uint32_t uCurrentNumberValue = 0;

	uintptr_t uTotalLength = 0; // also here for the same reason as i

	eParseStage stage = PARSE_START;
	eError uErrorCode = kErrorNone;

	// loop through format string one character at a time
	while (uParsePosition < uFormatLength) {
		const uint8_t c = static_cast<const uint8_t*>(
			static_cast<const void*>(pFormat))[uParsePosition];

		// Are we evaluating an argument format section?
		if (!bParsingParam) {
			// Is it a normal text character?
			if (c != kFORMAT_ESCAPE_CHAR) {
				// do we need to start a new literal text copy command
				if (!bParsingText) {
					++uCurrentCommandIndex;
					if (uCurrentCommandIndex >= uParamInfoCount) {
						uErrorCode = kErrorInvalidArgument;
						goto parse_error;
					}

					pParamInfos[uCurrentCommandIndex].SetCommand(
						COMMAND_COPY_TEXT);
					pParamInfos[uCurrentCommandIndex].ClearFlags();
					pParamInfos[uCurrentCommandIndex].m_pText =
						&pFormat[uParsePosition];
					pParamInfos[uCurrentCommandIndex].SetOutputLength(0);
					bParsingText = true;
				}

				// add another char to copy
				pParamInfos[uCurrentCommandIndex].SetOutputLength(
					pParamInfos[uCurrentCommandIndex].GetOutputLength() + 1);
			} else {
				// hit the formatting escape char ( '%' )?
				// were we previously doing a literal text copy?
				// if so, (and it's not a '%%') we close out the previous
				// literal text copy..
				//
				// if this turns out to be the literal '%' character via '%%',
				// we flag the text copy so it know it has to convert the
				// sequence when copying

				// check for edge case:  a single '%' as the last character in
				// the format string
				if ((uParsePosition + 1) == uFormatLength) {
					uErrorCode = kErrorInvalidParameter;
					goto parse_error;
				}

				// We need to snoop ahead and see if we're doing the escape
				// sequence to output the literal text

				// do we have a literal '%' to output
				if (pFormat[uParsePosition + 1] ==
					static_cast<char>(kFORMAT_ESCAPE_CHAR)) {
					// first character in the format string? or right after a
					// conversion command? if so, we need to make a new command
					if ((uCurrentCommandIndex == UINTPTR_MAX) ||
						(pParamInfos[uCurrentCommandIndex].GetCommand() ==
							COMMAND_FORMAT_ARGUMENT)) {
						BURGER_ASSERT(!bParsingText);

						// emit text copy command
						++uCurrentCommandIndex;
						if (uCurrentCommandIndex >= uParamInfoCount) {
							uErrorCode = kErrorInvalidParameter;
							goto parse_error;
						}

						pParamInfos[uCurrentCommandIndex].SetCommand(
							COMMAND_COPY_TEXT);
						pParamInfos[uCurrentCommandIndex].ClearFlags();
						pParamInfos[uCurrentCommandIndex].m_pText =
							&pFormat[uParsePosition];
						pParamInfos[uCurrentCommandIndex].SetOutputLength(0);

						bParsingText = true;
					}

					// Mark the output command as needing to check for the '%%'
					// sequence
					pParamInfos[uCurrentCommandIndex].SetFlag(
						CONVFLAG_HAS_SEQUENCE_CHAR);
					// we'll only be outputting 1 char (but reading 2)
					pParamInfos[uCurrentCommandIndex].SetOutputLength(
						pParamInfos[uCurrentCommandIndex].GetOutputLength() +
						1);
					// skip over second '%' char in escape sequence
					uParsePosition++;
				} else {
					// ok, we have the start of an argument formatting /
					// conversion sequence begin a new argument formatting
					// command default it to the current arg which could be
					// invalid but also can be specified later
					++uCurrentCommandIndex;
					if (uCurrentCommandIndex >= uParamInfoCount) {
						uErrorCode = kErrorInvalidParameter;
						goto parse_error;
					}

					pParamInfos[uCurrentCommandIndex].SetCommand(
						COMMAND_FORMAT_ARGUMENT);
					pParamInfos[uCurrentCommandIndex].SetConversion(
						CONVSPEC_INVALID);
					pParamInfos[uCurrentCommandIndex].SetArgIndex(
						uCurrentArgumentNext);
					pParamInfos[uCurrentCommandIndex].ClearFlags();
					pParamInfos[uCurrentCommandIndex].SetWidth(0);
					pParamInfos[uCurrentCommandIndex].SetPrecision(0);
					pParamInfos[uCurrentCommandIndex].SetFormattedLength(0);
					pParamInfos[uCurrentCommandIndex].SetOutputLength(0);

					// initialize argument parsing state
					uFormatSequencePosition = uParsePosition;
					bParsingNumVal = false;
					bCurNumValueValid = false;
					bArgIndexSpecified = false;
					bParsingParam = true;
					uCurrentNumberValue = 0;

					// reset our state to not text parsing
					bParsingText = false;

					stage = PARSE_START;
				}
			}
		} else {
			// we are parsing an argument formatting sequence ( bParsingParam ==
			// true )
			eOpcode uOpCode = (c < 127) ? g_CodeMap[c] : OP_NOT_A_SYMBOL;

			// Did we get a character that is invalid in a format sequence?
			if ((uOpCode == OP_NOT_A_SYMBOL) ||
				(uOpCode == OP_LITERAL_PERCENT)) {
				uErrorCode = kErrorInvalidArgument;
				goto parse_error;
			}

			// are we parsing a number?
			if (uOpCode == OP_NUMERIC_VALUE) {
				// is this the first numeric character?
				if (!bParsingNumVal) {
					// is this a good time to be seeing numbers?
					if (stage >= PARSE_LENGTH) {
						uErrorCode = kErrorInvalidParameter;
						goto parse_error;
					}

					// Starting out with a zero?
					if ((c == '0') && stage < PARSE_WIDTH) {
						// we could check for a double zero here (but it is
						// valid) leading 0 means to left pad with zeros (we'll
						// undo this if this turns out to be the argument index)
						pParamInfos[uCurrentCommandIndex].SetFlag(
							CONVFLAG_LEFT_PAD_ZEROS);
					} else {
						// starting out with a non-zero number?
						// now we are extracting a numeric value
						bParsingNumVal = true;
						bCurNumValueValid = true;
						uCurrentNumberValue = 0;
					}
				}

				if (bParsingNumVal) {
					// if the numeric sequence has gotten too big to be valid
					// for arg #, width or precision, stop adding digits but
					// don't report error here - we let other code determine
					// which value is being parsed (we may not know yet) and let
					// that code provide the correctly detailed error code
					if (uCurrentNumberValue <= kMAX_TOTAL_OUTPUT_SIZE) {
						uCurrentNumberValue = (uCurrentNumberValue * 10U) +
							static_cast<int_t>(c - '0'); // add digit
					}
				}
			} else {
				bParsingNumVal = false; // it's not a number, so turn off
										// numeric value parsing mode

				// are we specifying the argument to use?
				if (uOpCode == OP_ARG_SPECIFIER) {

					if (stage > PARSE_ARGNUM) {
						uErrorCode = kErrorOutOfBounds;
						goto parse_error;
					}

					// already specified the index?
					if (bArgIndexSpecified) {
						uErrorCode = kErrorDuplicateEntries;
						goto parse_error;
					}

					// do we have a valid index value?
					if (!bCurNumValueValid) {
						uErrorCode = kErrorInvalidParameter;
						goto parse_error;
					}

					if ((uCurrentNumberValue < 1) ||
						(uCurrentNumberValue > uArgCount)) {
						uErrorCode = kErrorOutOfBounds;
						goto parse_error;
					}

					// change the current argument number to the one specified
					uCurrentArgumentNext = uCurrentNumberValue - 1;
					pParamInfos[uCurrentCommandIndex].SetArgIndex(
						uCurrentArgumentNext);

					bArgIndexSpecified =
						true; // note argument index as explicitly specified
					bCurNumValueValid = false; // we used up the numeric value

					// reset the zero padding flag if set by mistake
					pParamInfos[uCurrentCommandIndex].ClearFlags();

					// update the parsing stage
					stage = PARSE_ARGNUM;
				}

				// check for various formatting flags ( left justify, sign, base
				// prefix, etc)
				if ((uOpCode >= OP_FLAGS_BEGIN) && (uOpCode <= OP_FLAGS_END)) {
					// do we have numeric value preceding the flags or
					if ((stage > PARSE_FLAGS) || bCurNumValueValid) {
						uErrorCode = kErrorInvalidParameter;
						goto parse_error;
					}

					// Get the corresponding flag, see if we did it twice?
					const eConversionFlags optionFlag =
						g_PrintFlagMappings[uOpCode - OP_FLAGS_BEGIN];

					if (pParamInfos[uCurrentCommandIndex].IsFlagSet(
							optionFlag)) {
						uErrorCode = kErrorDuplicateEntries;
						goto parse_error;
					}

					// Record flag
					pParamInfos[uCurrentCommandIndex].SetFlag(optionFlag);
					stage = PARSE_FLAGS;
				}

				// did we hit the precision specifier?
				if (uOpCode == OP_PRECISION_SPECIFIER) {
					// seen it before?
					if (pParamInfos[uCurrentCommandIndex].IsFlagSet(
							CONVFLAG_PRECISION_MARKER)) {
						uErrorCode = kErrorDuplicateEntries;
						goto parse_error;
					}

					// is it out of proper sequence?
					if (stage >= PARSE_PRECISION) {
						uErrorCode = kErrorInvalidParameter;
						goto parse_error;
					}

					// do we have a width to save off?
					if (bCurNumValueValid) {
						uErrorCode = pParamInfos[uCurrentCommandIndex]
										 .StoreOffNumericValue(
											 stage, uCurrentNumberValue);
						if (uErrorCode != kErrorNone) {
							goto parse_error;
						}
						bCurNumValueValid = false;
					}

					// record that we found a "." (even if no number after it)
					pParamInfos[uCurrentCommandIndex].SetFlag(
						CONVFLAG_PRECISION_MARKER);
					stage = PARSE_PRECISION;
				}

				// Did we get a length specifier
				if (uOpCode >= OP_LENGTH_BEGIN && uOpCode <= OP_LENGTH_END) {
					// do we have a width or precision to store off?
					if (bCurNumValueValid) {
						uErrorCode = pParamInfos[uCurrentCommandIndex]
										 .StoreOffNumericValue(
											 stage, uCurrentNumberValue);
						if (uErrorCode != kErrorNone) {
							goto parse_error;
						}
						bCurNumValueValid = false;
					}

					//  'hh' and 'll' can be specified but no other double
					//  letter combo
					if (stage >= PARSE_LENGTH) {
						// 'hh' length specifier?
						if (stage == PARSE_LENGTH &&
							uOpCode == OP_LENGTH_SHORT &&
							pParamInfos[uCurrentCommandIndex].IsFlagSet(
								CONVFLAG_INTERPRET_SHORT)) {
							pParamInfos[uCurrentCommandIndex].ClearFlag(
								CONVFLAG_INTERPRET_SHORT);
							pParamInfos[uCurrentCommandIndex].SetFlag(
								CONVFLAG_INTERPRET_SSHORT);

							// 'll' length specifier?
						} else if (stage == PARSE_LENGTH &&
							uOpCode == OP_LENGTH_LONG &&
							pParamInfos[uCurrentCommandIndex].IsFlagSet(
								CONVFLAG_INTERPRET_LONG)) {
							pParamInfos[uCurrentCommandIndex].ClearFlag(
								CONVFLAG_INTERPRET_LONG);
							pParamInfos[uCurrentCommandIndex].SetFlag(
								CONVFLAG_INTERPRET_LLONG);
						} else {
							// must be an invalid length specifier combo
							uErrorCode = kErrorDuplicateEntries;
							goto parse_error;
						}
					} else {
						// found our first length specifier
						pParamInfos[uCurrentCommandIndex].SetFlag(
							g_PrintfLengthMappings[uOpCode - OP_LENGTH_BEGIN]);

						// Note that we found an explicit length modifier
						pParamInfos[uCurrentCommandIndex].SetFlag(
							CONVFLAG_INTERPRET_SPECIFIED);
						stage = PARSE_LENGTH;
					}
				}

				// Finally... did we get a conversion specifier?
				if ((uOpCode >= OP_OUTPUT_BEGIN) &&
					(uOpCode <= OP_OUTPUT_END)) {
					// are we checking the arguments? (or just the format
					// string)
					if (uOptions & PARSEOPTION_CHECK_ARG_TYPES) {
						// Is the argument # to use valid?
						if (uCurrentArgumentNext >= uArgCount) {
							uErrorCode = (bArgIndexSpecified) ?
                                kErrorOutOfBounds :
                                kErrorDataStarvation;
							goto parse_error;
						}
					}

					// do we have a width or precision to store off?
					if (bCurNumValueValid) {
						uErrorCode = pParamInfos[uCurrentCommandIndex]
										 .StoreOffNumericValue(
											 stage, uCurrentNumberValue);
						if (uErrorCode != kErrorNone) {
							goto parse_error;
						}
						bCurNumValueValid = false;
					}

					// are we checking the arguments? (or just the format
					// string)
					if (uOptions & PARSEOPTION_CHECK_ARG_TYPES) {
						// do we determine the type?
						if (uOpCode == OP_OUTPUT_DEFAULT) {
							uOpCode = GetDefaultArgumentType(
								ppArgs[uCurrentArgumentNext]);
							if (uOpCode == OP_NOT_A_SYMBOL) {
								// Should never get this unless someone forgot
								// to provide one in code
								uErrorCode = kErrorInvalidParameter;
								goto parse_error;
							}
						}

						// Get the display type, and any modifier flags
						pParamInfos[uCurrentCommandIndex].SetConversion(
							g_PrintfOutputType[uOpCode - OP_OUTPUT_BEGIN]
								.m_uOutputDisplayType);
						pParamInfos[uCurrentCommandIndex].SetFlag(
							g_PrintfOutputType[uOpCode - OP_OUTPUT_BEGIN]
								.m_uOutputFlagsModifier);

						// validate the output type is compatible with the
						// argument.

						// get the type of parameter we are going format
						// (ArgType::xxx)
						const eArgumentType uParameterType =
							ppArgs[uCurrentArgumentNext]->GetType();

						// get the output display type in bitfield form
						const uint32_t uDisplayTypeFlag =
							pParamInfos[uCurrentCommandIndex]
								.GetConversionAsFlag();

						// we have 2 different levels of format compatibility
						const uint_t bStrictOk =
							(uOptions & PARSEOPTION_STRICT_OUTPUT_TYPES_OK) ?
                            (g_ValidOutputDisplayTypesStrict[uParameterType] &
								uDisplayTypeFlag) != 0 :
                            false;
						const uint_t bRelaxedOk =
							(uOptions & PARSEOPTION_RELAXED_OUTPUT_TYPES_OK) ?
                            (g_ValidOutputDisplayTypesRelaxed[uParameterType] &
								uDisplayTypeFlag) != 0 :
                            false;

						if (!bStrictOk && !bRelaxedOk) {
							uErrorCode = kErrorInvalidArgument;
							goto parse_error;
						}

						// Now check the flags for this conversion
						pParamInfos[uCurrentCommandIndex]
							.CheckConversionFlags();

						// Check the conversion for any iffy things we want to
						// issue warnings for
						pParamInfos[uCurrentCommandIndex]
							.CheckConversionForWarnings(
								ppArgs[uCurrentArgumentNext]);

						// Might as well determine the output length of the
						// formatted argument here....
						pParamInfos[uCurrentCommandIndex]
							.GetFormattedOutputLength(
								ppArgs[uCurrentArgumentNext]);
					} else {
						// make the output command the best we can
						const eConversionSpecifier uConversionSpec =
							(uOpCode == OP_OUTPUT_DEFAULT) ?
                            CONVSPEC_NOT_SUPPLIED :
                            g_PrintfOutputType[uOpCode - OP_OUTPUT_BEGIN]
								.m_uOutputDisplayType;
						const eConversionFlags uConversionFlags =
							(uOpCode == OP_OUTPUT_DEFAULT) ?
                            CONVFLAG_NO_OPTIONS :
                            g_PrintfOutputType[uOpCode - OP_OUTPUT_BEGIN]
								.m_uOutputFlagsModifier;

						pParamInfos[uCurrentCommandIndex].SetConversion(
							uConversionSpec);
						pParamInfos[uCurrentCommandIndex].SetFlag(
							uConversionFlags);

						// todo:  Get default widths for known output types?

						// Get an estimated output size by checking width and
						// precision
						uint_t theLength = 8;
						if (theLength <
							pParamInfos[uCurrentCommandIndex].GetPrecision()) {
							theLength = pParamInfos[uCurrentCommandIndex]
											.GetPrecision();
						}
						if (theLength <
							pParamInfos[uCurrentCommandIndex].GetWidth()) {
							theLength =
								pParamInfos[uCurrentCommandIndex].GetWidth();
						}

						pParamInfos[uCurrentCommandIndex].SetFormattedLength(
							theLength);
						pParamInfos[uCurrentCommandIndex].SetOutputLength(
							theLength);

						// Now check the flags for this conversion
						pParamInfos[uCurrentCommandIndex]
							.CheckConversionFlags();
					}

					// Finished parsing the arg specifier, we're done with this
					// argument
					bParsingParam = false;

					// next format command will use the next argument unless it
					// specifies explicitly
					++uCurrentArgumentNext;
				}
			}

		} // ( bParsingParam == true )

		// Advance one character ( ASCII or UTF-8 )
		++uParsePosition;

	} // parsing format string

	// Sum up the results to report back
	m_uPhaseResults = kErrorNone;
	m_uNumOutputCommands = uCurrentCommandIndex + 1;

	// Get the total length of the formatted result
	for (uintptr_t uIndex2 = 0; uIndex2 <= uCurrentCommandIndex; uIndex2++) {
		uTotalLength += pParamInfos[uIndex2].GetOutputLength();
	}
	m_uFormattedOutputLength = uTotalLength;
	return true;

	// error handler needed here...
parse_error:

	m_uErrorCharPos = uErrorCode;
	m_uErrorFormatSequencePos = uFormatSequencePosition;
	m_uErrorCharPos = uParsePosition;
	return false;
}

/*! ************************************

	\brief Mapping table for UTF-8 characters to SafePrint opcodes

	Mapping table for ASCII/UTF-8 characters ranged 0-127
	used do decode/parse a character that is part of a
	conversion specification.

		!   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
	0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
	@   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
	P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
	`   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
	p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~  del

***************************************/

const Burger::SafePrint::eOpcode Burger::SafePrint::g_CodeMap[128] = {
	OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL,
	OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL,
	OP_NOT_A_SYMBOL, // 0-7	(0x00 - 0x07)
	OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL,
	OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL,
	OP_NOT_A_SYMBOL, // 8-15 (0x08 - 0x0f)
	OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL,
	OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL,
	OP_NOT_A_SYMBOL, // 16-23 (0x10 - 0x17)
	OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL,
	OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL, OP_NOT_A_SYMBOL,
	OP_NOT_A_SYMBOL,              // 24-31 (0x18 - 0x1f)
	OP_FLAG_LEAD_SPACE_POSITIVE,  // (space) 0x20
	OP_OUTPUT_BINARY,             // !
	OP_NOT_A_SYMBOL,              // "
	OP_FLAG_ALTERNATIVE_FORM,     // #
	OP_ARG_SPECIFIER,             // $ 0x24
	OP_LITERAL_PERCENT,           // %
	OP_NOT_A_SYMBOL,              // &
	OP_NOT_A_SYMBOL,              // '
	OP_NOT_A_SYMBOL,              // ( 0x28
	OP_NOT_A_SYMBOL,              // )
	OP_VALUE_FROM_ARG,            // *
	OP_FLAG_FORCESIGN,            // +
	OP_FLAG_COMMA_SEPERATE,       // , 0x2C
	OP_FLAG_LEFTJUSTIFY,          // -
	OP_PRECISION_SPECIFIER,       // .
	OP_NOT_A_SYMBOL,              // /
	OP_NUMERIC_VALUE,             // 0 0x30
	OP_NUMERIC_VALUE,             // 1
	OP_NUMERIC_VALUE,             // 2
	OP_NUMERIC_VALUE,             // 3
	OP_NUMERIC_VALUE,             // 4 0x34
	OP_NUMERIC_VALUE,             // 5
	OP_NUMERIC_VALUE,             // 6
	OP_NUMERIC_VALUE,             // 7
	OP_NUMERIC_VALUE,             // 8 0x38
	OP_NUMERIC_VALUE,             // 9
	OP_NOT_A_SYMBOL,              // :
	OP_NOT_A_SYMBOL,              // ;
	OP_NOT_A_SYMBOL,              // < 0x3C
	OP_NOT_A_SYMBOL,              // =
	OP_NOT_A_SYMBOL,              // >
	OP_NOT_A_SYMBOL,              // ?
	OP_OUTPUT_DEFAULT,            // @ 0x40
	OP_OUTPUT_HEXFLOATUPPER,      // A
	OP_OUTPUT_BOOLTEXTUPPER,      // B
	OP_OUTPUT_WIDECHARACTER,      // C
	OP_NOT_A_SYMBOL,              // D
	OP_OUTPUT_SCI_NOTEFLOATUPPER, // E
	OP_OUTPUT_DECIMALFLOATUPPER,  // F
	OP_OUTPUT_SCI_SHORTNOTEUPPER, // G
	OP_NOT_A_SYMBOL,              // H
	OP_NOT_A_SYMBOL,              // I
	OP_NOT_A_SYMBOL,              // J
	OP_NOT_A_SYMBOL,              // K
	OP_LENGTH_DOUBLE,             // L
	OP_NOT_A_SYMBOL,              // M
	OP_NOT_A_SYMBOL,              // N
	OP_NOT_A_SYMBOL,              // O
	OP_OUTPUT_POINTERUPPER,       // P 0x50
	OP_NOT_A_SYMBOL,              // Q
	OP_NOT_A_SYMBOL,              // R
	OP_OUTPUT_CSTRING16,          // S
	OP_NOT_A_SYMBOL,              // T
	OP_NOT_A_SYMBOL,              // U
	OP_NOT_A_SYMBOL,              // V
	OP_NOT_A_SYMBOL,              // W
	OP_OUTPUT_HEXUPPER,           // X
	OP_NOT_A_SYMBOL,              // Y
	OP_NOT_A_SYMBOL,              // Z
	OP_NOT_A_SYMBOL,              // [
	OP_NOT_A_SYMBOL,              // \ (line cont)
	OP_NOT_A_SYMBOL,              // ]
	OP_NOT_A_SYMBOL,              // ^
	OP_NOT_A_SYMBOL,              // _
	OP_NOT_A_SYMBOL,              // ` 0x60
	OP_OUTPUT_HEXFLOAT,           // a
	OP_OUTPUT_BOOLTEXT,           // b
	OP_OUTPUT_CHARACTER,          // c
	OP_OUTPUT_DECIMALINT,         // d
	OP_OUTPUT_SCI_NOTEFLOAT,      // e
	OP_OUTPUT_DECIMALFLOAT,       // f
	OP_OUTPUT_SCI_SHORTNOTE,      // g
	OP_LENGTH_SHORT,              // h
	OP_OUTPUT_DECIMALINT,         // i
	OP_LENGTH_INTMAX,             // j
	OP_NOT_A_SYMBOL,              // k
	OP_LENGTH_LONG,               // l
	OP_NOT_A_SYMBOL,              // m
	OP_OUTPUT_STORELENGTH,        // n
	OP_OUTPUT_OCTAL,              // o
	OP_OUTPUT_POINTER,            // p 0x70
	OP_NOT_A_SYMBOL,              // q
	OP_NOT_A_SYMBOL,              // r
	OP_OUTPUT_CSTRING,            // s
	OP_LENGTH_WORDPTR,            // t
	OP_OUTPUT_DECIMALWORD,        // u
	OP_NOT_A_SYMBOL,              // v
	OP_NOT_A_SYMBOL,              // w
	OP_OUTPUT_HEX,                // x
	OP_NOT_A_SYMBOL,              // y
	OP_LENGTH_SIZET,              // z
	OP_NOT_A_SYMBOL,              // {
	OP_NOT_A_SYMBOL,              // |
	OP_NOT_A_SYMBOL,              // }
	OP_NOT_A_SYMBOL,              // ~
	OP_NOT_A_SYMBOL               // (DEL)
};

/*! ************************************

	\brief Mapping table for OP_FLAG_* for printing

	Lookup table to convert \ref eOpcode in the OP_FLAG_* range
	to the appropriate conversion formatting flag

***************************************/

const Burger::SafePrint::eConversionFlags
	Burger::SafePrint::g_PrintFlagMappings[5] = {
		CONVFLAG_LEFT_JUSTIFY,     // OP_FLAG_LEFTJUSTIFY,
		CONVFLAG_DISPLAY_SIGN,     // OP_FLAG_FORCESIGN,
		CONVFLAG_BLANK_SIGN,       // OP_FLAG_LEAD_SPACE_POSITIVE,
		CONVFLAG_ALTERNATIVE_FORM, // OP_FLAG_ALTERNATIVE_FORM,
		CONVFLAG_COMMA_SEPARATE    // OP_FLAG_COMMA_SEPERATE
};

/*! ************************************

	\brief Mapping table for OP_LENGTH_* for printing

	Lookup table to convert \ref eOpcode in the OP_LENGTH_* range
	to the appropriate conversion argument length modifier

***************************************/

const Burger::SafePrint::eConversionFlags
	Burger::SafePrint::g_PrintfLengthMappings[6] = {
		CONVFLAG_INTERPRET_SHORT,   // OP_LENGTH_SHORT
		CONVFLAG_INTERPRET_LONG,    // OP_LENGTH_LONG
		CONVFLAG_INTERPRET_INTMAX,  // OP_LENGTH_INTMAX
		CONVFLAG_INTERPRET_SIZET,   // OP_LENGTH_SIZET
		CONVFLAG_INTERPRET_WORDPTR, // OP_LENGTH_WORDPTR
		CONVFLAG_INTERPRET_DOUBLE   // OP_LENGTH_DOUBLE
};

/*! ************************************

	\brief Mapping table for OP_OUTPUT_* for printing

	Lookup table to convert \ref eOpcode in the OP_OUTPUT_* range to the actual
	output conversion specification and includes any additional formatting
	options that are needed (such as using upper case characters) and info the
	expected argument type (which we only use to issue warnings)

***************************************/

const Burger::SafePrint::OutputTypeAndFlags_t
	Burger::SafePrint::g_PrintfOutputType[22] = {
		{CONVSPEC_BINNUMBER, CONVFLAG_NO_OPTIONS},    // OP_OUTPUT_BINARY
		{CONVSPEC_BOOLTEXT, CONVFLAG_NO_OPTIONS},     // OP_OUTPUT_BOOLTEXT
		{CONVSPEC_BOOLTEXT, CONVFLAG_UPPERCASE},      // OP_OUTPUT_BOOLTEXTUPPER
		{CONVSPEC_CHAR, CONVFLAG_NO_OPTIONS},         // OP_OUTPUT_CHARACTER
		{CONVSPEC_CHAR, CONVFLAG_OUTPUT_WCHAR},       // OP_OUTPUT_WIDECHARACTER
		{CONVSPEC_DECIMAL_INT, CONVFLAG_NO_OPTIONS},  // OP_OUTPUT_DECIMALINT
		{CONVSPEC_DECIMAL_WORD, CONVFLAG_NO_OPTIONS}, // OP_OUTPUT_DECIMALWORD
		{CONVSPEC_DECIMAL_FLOAT, CONVFLAG_NO_OPTIONS}, // OP_OUTPUT_DECIMALFLOAT
		{CONVSPEC_DECIMAL_FLOAT,
			CONVFLAG_UPPERCASE}, // OP_OUTPUT_DECIMALFLOATUPPER
		{CONVSPEC_HEX_FLOAT, CONVFLAG_NO_OPTIONS}, // OP_OUTPUT_HEXFLOAT
		{CONVSPEC_HEX_FLOAT, CONVFLAG_UPPERCASE},  // OP_OUTPUT_HEXFLOATUPPER
		{CONVSPEC_SCI_NOTE_FLOAT,
			CONVFLAG_NO_OPTIONS}, // OP_OUTPUT_SCI_NOTEFLOAT
		{CONVSPEC_SCI_NOTE_FLOAT,
			CONVFLAG_UPPERCASE}, // OP_OUTPUT_SCI_NOTEFLOATUPPER
		{CONVSPEC_SHORTER_FLOAT,
			CONVFLAG_NO_OPTIONS}, // OP_OUTPUT_SCI_SHORTNOTE
		{CONVSPEC_SHORTER_FLOAT,
			CONVFLAG_UPPERCASE},                 // OP_OUTPUT_SCI_SHORTNOTEUPPER
		{CONVSPEC_OCTAL, CONVFLAG_NO_OPTIONS},   // OP_OUTPUT_OCTAL
		{CONVSPEC_HEX, CONVFLAG_NO_OPTIONS},     // OP_OUTPUT_HEX
		{CONVSPEC_HEX, CONVFLAG_UPPERCASE},      // OP_OUTPUT_HEXUPPER
		{CONVSPEC_POINTER, CONVFLAG_NO_OPTIONS}, // OP_OUTPUT_POINTER
		{CONVSPEC_POINTER, CONVFLAG_UPPERCASE},  // OP_OUTPUT_POINTERUPPER
		{CONVSPEC_TEXT_STRING, CONVFLAG_NO_OPTIONS},  // OP_OUTPUT_CSTRING
		{CONVSPEC_TEXT_STRING, CONVFLAG_OUTPUT_WCHAR} // OP_OUTPUT_CSTRING16
};

/*! ************************************

	\brief CONVSPEC_* expected data sizes

	Table of expected argument sizes for each conversion

***************************************/

const uintptr_t Burger::SafePrint::g_ConversionArgumentSizes[14] = {
	UINTPTR_MAX,          // CONVSPEC_INVALID
	0,                    // CONVSPEC_BINNUMBER
	sizeof(bool),         // CONVSPEC_BOOLTEXT
	sizeof(int),          // CONVSPEC_CHAR
	sizeof(int),          // CONVSPEC_DECIMAL_INT
	sizeof(unsigned int), // CONVSPEC_DECIMAL_WORD
	sizeof(unsigned int), // CONVSPEC_OCTAL
	sizeof(unsigned int), // CONVSPEC_HEX
	sizeof(double),       // CONVSPEC_HEX_FLOAT
	sizeof(double),       // CONVSPEC_DECIMAL_FLOAT
	sizeof(double),       // CONVSPEC_SCI_NOTE_FLOAT
	sizeof(double),       // CONVSPEC_SHORTER_FLOAT
	sizeof(void*),        // CONVSPEC_POINTER
	sizeof(char*)         // CONVSPEC_TEXT_STRING
};

/*! ************************************

	\brief CONVSPEC_* valid output conversion flags

	Table of which flags are valid for what output conversions

***************************************/

const uint32_t Burger::SafePrint::g_ValidFlagsForConversion[14] = {
	CONVFLAG_NO_OPTIONS, // CONVSPEC_INVALID
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_ALTERNATIVE_FORM |
		CONVFLAG_COMMA_SEPARATE, // CONVSPEC_BINNUMBER
	CONVFLAG_LEFT_JUSTIFY,       // CONVSPEC_BOOLTEXT
	CONVFLAG_LEFT_JUSTIFY,       // CONVSPEC_CHAR
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN |
		CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_COMMA_SEPARATE |
		CONVFLAG_PRECISION_SPECIFIED, // CONVSPEC_DECIMAL_INT
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_COMMA_SEPARATE |
		CONVFLAG_PRECISION_SPECIFIED, // CONVSPEC_DECIMAL_WORD
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_ALTERNATIVE_FORM |
		CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_COMMA_SEPARATE |
		CONVFLAG_PRECISION_SPECIFIED, // CONVSPEC_OCTAL
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_ALTERNATIVE_FORM |
		CONVFLAG_LEFT_PAD_ZEROS | CONVFLAG_COMMA_SEPARATE |
		CONVFLAG_PRECISION_SPECIFIED, // CONVSPEC_HEX
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN |
		CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS |
		CONVFLAG_PRECISION_SPECIFIED, // CONVSPEC_HEX_FLOAT
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN |
		CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS |
		CONVFLAG_PRECISION_SPECIFIED, // CONVSPEC_DECIMAL_FLOAT
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN |
		CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS |
		CONVFLAG_PRECISION_SPECIFIED, // CONVSPEC_SCI_NOTE_FLOAT
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_DISPLAY_SIGN | CONVFLAG_BLANK_SIGN |
		CONVFLAG_ALTERNATIVE_FORM | CONVFLAG_LEFT_PAD_ZEROS |
		CONVFLAG_PRECISION_SPECIFIED,                  // CONVSPEC_SHORTER_FLOAT
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_ALTERNATIVE_FORM, // CONVSPEC_POINTER
	CONVFLAG_LEFT_JUSTIFY | CONVFLAG_PRECISION_SPECIFIED // CONVSPEC_TEXT_STRING
};

/*! ************************************

	\brief ARG_* valid output conversion flags

	Bit table of Valid output conversion specifiers for each argument type
	when applying strict argument conversion rules

***************************************/

const uint32_t
	Burger::SafePrint::g_ValidOutputDisplayTypesStrict[kArgumentTypeCount] = {
		0, // ARG_INVALID
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_INT |
			CONVSPECFLAG_CHARACTER, // ARG_INT8
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_INT |
			CONVSPECFLAG_CHARACTER, // ARG_INT16
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_INT |
			CONVSPECFLAG_CHARACTER, // ARG_INT32
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_INT |
			CONVSPECFLAG_CHARACTER, // ARG_INT64

		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_UNSIGNEDINT |
			CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX |
			CONVSPECFLAG_CHARACTER, // ARG_WORD8
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_UNSIGNEDINT |
			CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX |
			CONVSPECFLAG_CHARACTER, // ARG_WORD16
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_UNSIGNEDINT |
			CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX |
			CONVSPECFLAG_CHARACTER, // ARG_WORD32
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_UNSIGNEDINT |
			CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX |
			CONVSPECFLAG_CHARACTER, // ARG_WORD64

		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_FLOAT |
			CONVSPECFLAG_SCINOTE_FLOAT | CONVSPECFLAG_SHORTER_FLOAT |
			CONVSPECFLAG_HEX_FLOAT, // kTypeFloat16
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_FLOAT |
			CONVSPECFLAG_SCINOTE_FLOAT | CONVSPECFLAG_SHORTER_FLOAT |
			CONVSPECFLAG_HEX_FLOAT, // kTypeFloat32
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_DECIMAL_FLOAT |
			CONVSPECFLAG_SCINOTE_FLOAT | CONVSPECFLAG_SHORTER_FLOAT |
			CONVSPECFLAG_HEX_FLOAT, // kTypeFloat64

		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_BOOL_TEXT, // ARG_BOOL
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,   // ARG_SIMD64
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,   // ARG_SIMD128
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,   // ARG_SIMD128I
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER,   // ARG_SIMD128D

		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_TEXT_STRING |
			CONVSPECFLAG_POINTER, // ARG_PSTRING
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_TEXT_STRING |
			CONVSPECFLAG_POINTER, // ARG_PCHAR
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_TEXT_STRING |
			CONVSPECFLAG_POINTER, // ARG_PSCHAR
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_TEXT_STRING |
			CONVSPECFLAG_POINTER, // ARG_PUCHAR

		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PINT16
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER |
			CONVSPECFLAG_TEXT_STRING,                      // ARG_PWORD16
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PINT32
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PWORD32
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PINT64
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PWORD64
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PFLOAT
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PDOUBLE
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PSIMD64
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PSIMD128
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PSIMD128I
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER, // ARG_PSIMD128D
		CONVSPECFLAG_BINARY_NUMBER | CONVSPECFLAG_POINTER  // ARG_PVOID
};

/*! ************************************

	\brief ARG_* valid output relaxed conversion flags

	Bit table of Additional Valid output conversion specifiers for each argument
	type when allowing for "relaxed" conversions

***************************************/

const uint32_t
	Burger::SafePrint::g_ValidOutputDisplayTypesRelaxed[kArgumentTypeCount] = {
		0, // ARG_INVALID
		CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL |
			CONVSPECFLAG_HEX | CONVSPECFLAG_BOOL_TEXT, // ARG_INT8
		CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL |
			CONVSPECFLAG_HEX | CONVSPECFLAG_BOOL_TEXT, // ARG_INT16
		CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL |
			CONVSPECFLAG_HEX | CONVSPECFLAG_BOOL_TEXT, // ARG_INT32
		CONVSPECFLAG_DECIMAL_UNSIGNEDINT | CONVSPECFLAG_OCTAL |
			CONVSPECFLAG_HEX | CONVSPECFLAG_BOOL_TEXT, // ARG_INT64

		CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_BOOL_TEXT, // ARG_WORD8
		CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_BOOL_TEXT, // ARG_WORD16
		CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_BOOL_TEXT, // ARG_WORD32
		CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_BOOL_TEXT, // ARG_WORD64

		CONVSPECFLAG_HEX, // kTypeFloat16
		CONVSPECFLAG_HEX, // kTypeFloat32
		CONVSPECFLAG_HEX, // kTypeFloat64

		CONVSPECFLAG_DECIMAL_INT | CONVSPECFLAG_DECIMAL_UNSIGNEDINT |
			CONVSPECFLAG_OCTAL | CONVSPECFLAG_HEX, // ARG_BOOL

		CONVSPECFLAG_HEX, // ARG_SIMD64
		CONVSPECFLAG_HEX, // ARG_SIMD128
		CONVSPECFLAG_HEX, // ARG_SIMD128I
		CONVSPECFLAG_HEX, // ARG_SIMD128D

		CONVSPECFLAG_HEX, // ARG_PSTRING
		CONVSPECFLAG_HEX, // ARG_PCHAR
		CONVSPECFLAG_HEX, // ARG_PSCHAR
		CONVSPECFLAG_HEX, // ARG_PUCHAR

		CONVSPECFLAG_HEX, // ARG_PINT16
		CONVSPECFLAG_HEX, // ARG_PWORD16
		CONVSPECFLAG_HEX, // ARG_PINT32
		CONVSPECFLAG_HEX, // ARG_PWORD32
		CONVSPECFLAG_HEX, // ARG_PINT64
		CONVSPECFLAG_HEX, // ARG_PWORD64
		CONVSPECFLAG_HEX, // ARG_PFLOAT
		CONVSPECFLAG_HEX, // ARG_PDOUBLE
		CONVSPECFLAG_HEX, // ARG_PSIMD64
		CONVSPECFLAG_HEX, // ARG_PSIMD128
		CONVSPECFLAG_HEX, // ARG_PSIMD128I
		CONVSPECFLAG_HEX, // ARG_PSIMD128D
		CONVSPECFLAG_HEX  // ARG_PVOID
};

/*! ************************************

	\brief Convert error flag to message in English

	\param uFlags Error code
	\return Pointer to a "C" string of the warning message

***************************************/

const char* BURGER_API Burger::SafePrint::GetWarningDescription(
	eWarningFlags uFlags) BURGER_NOEXCEPT
{
	const char* pResult;
	switch (uFlags) {
	case WARN_NONE:
		pResult = "No Warning";
		break;
	case WARN_SIGNED_UNSIGNED_MISMATCH:
		pResult = "Argument signed mismatch with conversion";
		break;
	case WARN_FLAG_UNDEFINED_FOR_CONVERSION:
		pResult = "Flag undefined for conversion (ignored)";
		break;
	case WARN_FLAG_SUPPRESSED:
		pResult = "Flag suppressed by another flag";
		break;
	case WARN_PRECISION_UNDEFINED:
		pResult = "Precision is undefined for conversion";
		break;
	case WARN_LENGTH_SPECIFIER_MISMATCH:
		pResult = "Length modifier doesn't match argument";
		break;
	case WARN_LENGTH_NOT_VALID_FOR_CONVERSION:
		pResult = "Length modifier isn't valid for conversion";
		break;
	case WARN_FLAG_NO_EFFECT:
		pResult = "Flag had no effect";
		break;
	default:
		pResult = "Unspecified warning";
		break;
	}
	return pResult;
}

/*! ************************************

	\brief Get the default opcode from the argument data type

	\param pArg Error code
	\return Opcode that is best to process this argument or \ref OP_NOT_A_SYMBOL

***************************************/

Burger::SafePrint::eOpcode BURGER_API Burger::SafePrint::GetDefaultArgumentType(
	const ArgumentType* pArg) BURGER_NOEXCEPT
{
	eOpcode uCode = OP_NOT_A_SYMBOL;
	if (pArg) {
		switch (pArg->GetType()) {
		case kArgumentTypeInt8:
			uCode = OP_OUTPUT_DECIMALINT;
			break;
		case kArgumentTypeInt16:
			uCode = OP_OUTPUT_DECIMALINT;
			break;
		case kArgumentTypeInt32:
			uCode = OP_OUTPUT_DECIMALINT;
			break;
		case kArgumentTypeInt64:
			uCode = OP_OUTPUT_DECIMALINT;
			break;

		case kArgumentTypeUInt8:
			uCode = OP_OUTPUT_DECIMALWORD;
			break;
		case kArgumentTypeUInt16:
			uCode = OP_OUTPUT_DECIMALWORD;
			break;
		case kArgumentTypeUInt32:
			uCode = OP_OUTPUT_DECIMALWORD;
			break;
		case kArgumentTypeUInt64:
			uCode = OP_OUTPUT_DECIMALWORD;
			break;

		case kArgumentTypeFloat16:
			uCode = OP_OUTPUT_DECIMALFLOAT;
			break;
		case kArgumentTypeFloat32:
			uCode = OP_OUTPUT_DECIMALFLOAT;
			break;
		case kArgumentTypeFloat64:
			uCode = OP_OUTPUT_DECIMALFLOAT;
			break;

		case kArgumentTypeBool:
			uCode = OP_OUTPUT_BOOLTEXT;
			break;

		case kArgumentTypeBurgerStringPtr:
			uCode = OP_OUTPUT_CSTRING;
			break;
		case kArgumentTypeCharPtr:
			uCode = OP_OUTPUT_CSTRING;
			break;
		case kArgumentTypeInt8Ptr:
			uCode = OP_OUTPUT_CSTRING;
			break;
		case kArgumentTypeUInt8Ptr:
			uCode = OP_OUTPUT_CSTRING;
			break;

		case kArgumentTypeInt16Ptr:
			uCode = OP_OUTPUT_POINTER;
			break;
		case kArgumentTypeUInt16Ptr:
			uCode = OP_OUTPUT_POINTER;
			break;
		case kArgumentTypeInt32Ptr:
			uCode = OP_OUTPUT_POINTER;
			break;
		case kArgumentTypeUInt32Ptr:
			uCode = OP_OUTPUT_POINTER;
			break;
		case kArgumentTypeInt64Ptr:
			uCode = OP_OUTPUT_POINTER;
			break;
		case kArgumentTypeUInt64Ptr:
			uCode = OP_OUTPUT_POINTER;
			break;
		case kArgumentTypeFloat32Ptr:
			uCode = OP_OUTPUT_POINTER;
			break;
		case kArgumentTypeFloat64Ptr:
			uCode = OP_OUTPUT_POINTER;
			break;
		case kArgumentTypeVoidPtr:
			uCode = OP_OUTPUT_POINTER;
			break;
		default:
			break;
		}
	}
	return uCode;
}

/*! ************************************

	\brief Convert conversion specifier to message in English

	\param uType Specifier type to convert
	\return Pointer to a "C" string of the conversion specifier type

***************************************/

const char* BURGER_API Burger::SafePrint::GetOutputDisplayType(
	eConversionSpecifier uType) BURGER_NOEXCEPT
{
	const char* pResult;
	switch (uType) {
	case CONVSPEC_INVALID:
		pResult = "<INVALID>";
		break;
	case CONVSPEC_BINNUMBER:
		pResult = "binary";
		break;
	case CONVSPEC_BOOLTEXT:
		pResult = "bool as text";
		break;
	case CONVSPEC_CHAR:
		pResult = "char";
		break;
	case CONVSPEC_DECIMAL_INT:
		pResult = "decimal int";
		break;
	case CONVSPEC_DECIMAL_WORD:
		pResult = "decimal unsigned int";
		break;
	case CONVSPEC_OCTAL:
		pResult = "octal";
		break;
	case CONVSPEC_HEX:
		pResult = "hex";
		break;
	case CONVSPEC_HEX_FLOAT:
		pResult = "hex float";
		break;
	case CONVSPEC_DECIMAL_FLOAT:
		pResult = "decimal float";
		break;
	case CONVSPEC_SCI_NOTE_FLOAT:
		pResult = "scientific notation float";
		break;
	case CONVSPEC_SHORTER_FLOAT:
		pResult = "shorter of decimal or scientific float";
		break;
	case CONVSPEC_POINTER:
		pResult = "pointer";
		break;
	case CONVSPEC_TEXT_STRING:
		pResult = "text string";
		break;
	default:
		pResult = "<invalid>";
		break;
	}
	return pResult;
}

/*! ************************************

	\brief Convert conversion command to message in English

	\param uCommand Command to convert
	\return Pointer to a "C" string of the conversion command

***************************************/

const char* BURGER_API Burger::SafePrint::GetOutputCommandName(
	eConversionCommand uCommand) BURGER_NOEXCEPT
{
	const char* pResult;
	switch (uCommand) {
	case COMMAND_COPY_TEXT:
		pResult = "COPY TEXT";
		break;
	case COMMAND_FORMAT_ARGUMENT:
		pResult = "FORMAT ARGUMENT";
		break;
	default:
		pResult = "<invalid>";
		break;
	}
	return pResult;
}

/***************************************

	public SafePrint functions are below

***************************************/

/*! ************************************

	\brief Reports the size of a formatted output in bytes without actually
creating any formatted output.

	* * Step 1) Parse the format string and arguments
	* * Step 2) Report the format results and call back for an output buffer.
	* * Step 3) If no issues found in Step 1 and a buffer was returned in step
		2, execute the list of format & copy output commands.

	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero
	\return Negative number for error, number of bytes processed on success

***************************************/

intptr_t BURGER_API Burger::GetFormattedLength(const char* pFormat,
	uintptr_t uArgCount, const ArgumentType** ppArgs) BURGER_NOEXCEPT
{
	// Any invalid parameters passed in?
	if (IsStringEmpty(pFormat) || (uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine

	SafePrint::ProcessResults_t TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::kMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable, sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	const uint_t bParseOk = TheProcessResults.FormatPreProcess(
		SafePrint::PARSEOPTION_DEFAULT, pFormat, StringLength(pFormat),
		uArgCount, ppArgs, BURGER_ARRAYSIZE(ParamIntoTable), ParamIntoTable);

	// If there were no problems, return the output size... otherwise report 0
	if (bParseOk && (TheProcessResults.m_uPhaseResults == kErrorNone)) {
		return static_cast<intptr_t>(
			TheProcessResults.m_uFormattedOutputLength);
	}

	// An error must have occurred.  Handle it, and get the proper return value
	return TheProcessResults.ErrorHandler(pFormat, uArgCount, ppArgs);
}

/*! ************************************

	\brief Print into a buffer that a callback allocates

	Version of sprintf that calls back to an allocator to get the output buffer
	*AFTER* validating the formatting and determining the required size. A null
	terminator written to the end of the output is optional requested by the
	caller.

	* * Step 1) Parse the format string and arguments
	* * Step 2) Report the format results and call back for an output buffer.
	* * Step 3) If no issues found in Step 1 and a buffer was returned in step
		2, execute the list of format & copy output commands.

	\param pCallback Pointer to a callback function
	\param pContext Pointer to data the callback function requires, can be \ref
		NULL
	\param bAddNull \ref TRUE if a 0 should be appended to the end of the
		string, \ref FALSE if not.
	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Negative number for error, number of bytes processed on success

	\sa Sprintf(char *,const char *,uintptr_t,const ArgumentType **)

***************************************/

intptr_t BURGER_API Burger::SprintfUserAlloc(
	SafePrint::SprintfCallbackProc pCallback, void* pContext, uint_t bAddNull,
	const char* pFormat, uintptr_t uArgCount,
	const ArgumentType** ppArgs) BURGER_NOEXCEPT
{
	// Any invalid parameters passed in?
	if (!pCallback || IsStringEmpty(pFormat) ||
		(uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine
	const uintptr_t uFormatLength = StringLength(pFormat);

	SafePrint::ProcessResults_t TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::kMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable, sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	const uint_t bParseOk = TheProcessResults.FormatPreProcess(
		SafePrint::PARSEOPTION_DEFAULT, pFormat, uFormatLength, uArgCount,
		ppArgs, BURGER_ARRAYSIZE(ParamIntoTable), ParamIntoTable);

	// If there were no problems, format the output to the supplied buffer
	const uint_t bNoErrors =
		(bParseOk && TheProcessResults.m_uPhaseResults == kErrorNone);
	const uintptr_t uBufferSizeNeeded =
		TheProcessResults.m_uFormattedOutputLength + bAddNull;
	void* pOutputBuffer = nullptr;

	// Ask the external allocator for a buffer
	const uint_t bProceed =
		(pCallback)(bNoErrors, uBufferSizeNeeded, &pOutputBuffer, pContext);

	// Did we get back the OK and a buffer to use, and we're error free?
	if (bNoErrors && bProceed && IsPointerValid(pOutputBuffer)) {
		// format the output to the buffer
		char* pWorkBuffer = static_cast<char*>(pOutputBuffer);
		uintptr_t uProcessedCount =
			TheProcessResults.GenerateFormatOutputToBuffer(pWorkBuffer,
				uBufferSizeNeeded, uArgCount, ppArgs,
				TheProcessResults.m_uNumOutputCommands, ParamIntoTable);

		// Were we error free?
		if (TheProcessResults.m_uPhaseResults == kErrorNone) {
			// Were we asked to append a null?
			if (bAddNull && (uProcessedCount < uBufferSizeNeeded)) {
				pWorkBuffer[uProcessedCount] = 0;
				++uProcessedCount;
			}
			return static_cast<intptr_t>(uProcessedCount);
		}
	}

	// An error must have occurred.  Handle it, and get the proper return value
	return TheProcessResults.ErrorHandler(pFormat, uArgCount, ppArgs);
}

/*! ************************************

	\brief sprintf() clone

	Return value is the number of characters written to the buffer, or a
	negative value indicating an error occurred during the attempt.

	* * Step 1) Parse the format string and arguments
	* * Step 2) if no issues found in Step 1, execute the list of format & copy
		output commands.

	\note This function can easily overrun the buffer since no buffer
		is supplied.  It is recommended you use Snprintf() instead.

	\param pOutput Pointer to a buffer large enough to accept the output string
	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Negative number for error, number of bytes processed on success

	\sa Snprintf(char *,uintptr_t,const char *,uintptr_t,
		const ArgumentType **) or SprintfUserAlloc(
		SafePrint::SprintfCallbackProc,void *,uint_t,const char *,
		uintptr_t,const ArgumentType **)

***************************************/

intptr_t BURGER_API Burger::Sprintf(char* pOutput, const char* pFormat,
	uintptr_t uArgCount, const ArgumentType** ppArgs) BURGER_NOEXCEPT
{
	// Any invalid parameters passed in?
	if (IsPointerInvalid(pOutput) || IsStringEmpty(pFormat) ||
		(uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine
	SafePrint::ProcessResults_t TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::kMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable, sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	uint_t bParseOk = TheProcessResults.FormatPreProcess(
		SafePrint::PARSEOPTION_DEFAULT, pFormat, StringLength(pFormat),
		uArgCount, ppArgs, BURGER_ARRAYSIZE(ParamIntoTable), ParamIntoTable);

	// If there were no problems, format the output to the supplied buffer
	if (bParseOk && (TheProcessResults.m_uPhaseResults == kErrorNone)) {
		// need space for terminating null
		if (TheProcessResults.m_uFormattedOutputLength <
			SafePrint::kMAX_TOTAL_OUTPUT_SIZE) {
			uintptr_t uProcessedCount =
				TheProcessResults.GenerateFormatOutputToBuffer(pOutput,
					SafePrint::kMAX_TOTAL_OUTPUT_SIZE, uArgCount, ppArgs,
					TheProcessResults.m_uNumOutputCommands, ParamIntoTable);

			// Were we error free?
			if (TheProcessResults.m_uPhaseResults == kErrorNone) {
				pOutput[uProcessedCount] = 0;
				return static_cast<intptr_t>(uProcessedCount);
			}
		}
	}

	// An error must have occurred.  Handle it, and get the proper return value
	return TheProcessResults.ErrorHandler(pFormat, uArgCount, ppArgs);
}

/*! ************************************

	\brief snprintf() clone

	Return value is the number of characters to be written if the buffer is
	sufficiently large to hold the entire formatted output, or a
	negative value indicating an error occurred during the attempt.

	* * Step 1) Parse the format string and arguments
	* * Step 2) if no issues found in Step 1, execute the list of format & copy
		output commands.

	\param pOutput Pointer to the output buffer
	\param uOutputSize Size of the output buffer in bytes
	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Negative number for error, number of bytes processed on success

	\sa Sprintf(char *,const char *,uintptr_t,const ArgumentType **)

***************************************/

intptr_t BURGER_API Burger::Snprintf(char* pOutput, uintptr_t uOutputSize,
	const char* pFormat, uintptr_t uArgCount,
	const ArgumentType** ppArgs) BURGER_NOEXCEPT
{
	// Any invalid parameters passed in?
	if (IsPointerInvalid(pOutput) || !uOutputSize || IsStringEmpty(pFormat) ||
		(uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine

	SafePrint::ProcessResults_t TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::kMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable, sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	const uint_t bParseOk = TheProcessResults.FormatPreProcess(
		SafePrint::PARSEOPTION_DEFAULT, pFormat, StringLength(pFormat),
		uArgCount, ppArgs, BURGER_ARRAYSIZE(ParamIntoTable), ParamIntoTable);

	// If there were no problems, format the output to the supplied buffer
	if (bParseOk && (TheProcessResults.m_uPhaseResults == kErrorNone)) {
		// need space for terminating null
		if (TheProcessResults.m_uFormattedOutputLength < uOutputSize) {
			uintptr_t uProcessedCount =
				TheProcessResults.GenerateFormatOutputToBuffer(pOutput,
					uOutputSize, uArgCount, ppArgs,
					TheProcessResults.m_uNumOutputCommands, ParamIntoTable);

			// Were we error free?
			if (TheProcessResults.m_uPhaseResults == kErrorNone) {
				pOutput[uProcessedCount] = 0;
				return static_cast<intptr_t>(uProcessedCount); // this is wrong!
			}
		}
	}
	// An error must have occurred.  Handle it, and get the proper return value
	return TheProcessResults.ErrorHandler(pFormat, uArgCount, ppArgs);
}

/*! ************************************

	\brief printf() clone

	Return value is the number of characters transmitted, or a negative value
	indicating an error occurred during the attempt.

	* * Step 1) Parse the format string and arguments
	* * Step 2) if no issues found in Step 1, execute the list of format & copy
		output commands.

	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Negative number for error, number of bytes processed on success

***************************************/

intptr_t BURGER_API Burger::Printf(const char* pFormat, uintptr_t uArgCount,
	const ArgumentType** ppArgs) BURGER_NOEXCEPT
{
	// Any invalid parameters passed in?
	if (IsStringEmpty(pFormat) || (uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine
	SafePrint::ProcessResults_t TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::kMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable, sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	const uint_t bParseOk = TheProcessResults.FormatPreProcess(
		SafePrint::PARSEOPTION_DEFAULT, pFormat, StringLength(pFormat),
		uArgCount, ppArgs, BURGER_ARRAYSIZE(ParamIntoTable), ParamIntoTable);

	// If there were no problems, format the output and send the results out via
	// the RTL printf() function
	if (bParseOk && (TheProcessResults.m_uPhaseResults == kErrorNone)) {
		// format the output and send it along
		const uintptr_t uProcessedCount =
			TheProcessResults.GenerateFormattedOutputToFile(stdin, uArgCount,
				ppArgs, TheProcessResults.m_uNumOutputCommands, ParamIntoTable);

		// Were we error free?
		if (TheProcessResults.m_uPhaseResults == kErrorNone) {
			return static_cast<intptr_t>(uProcessedCount);
		}
	}

	// An error must have occurred.  Handle it, and get the proper return value
	return TheProcessResults.ErrorHandler(pFormat, uArgCount, ppArgs);
}

/*! ************************************

	\brief printf() clone

	Return value is the number of characters transmitted, or a negative value
	indicating an error occurred during the attempt.

	* * Step 1) Parse the format string and arguments
	* * Step 2) if no issues found in Step 1, execute the list of format & copy
		output commands.

	\param fp Pointer to a FILE stream
	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Negative number for error, number of bytes processed on success

***************************************/

intptr_t BURGER_API Burger::Fprintf(FILE* fp, const char* pFormat,
	uintptr_t uArgCount, const ArgumentType** ppArgs) BURGER_NOEXCEPT
{
	// Any invalid parameters passed in?
	if (IsStringEmpty(pFormat) || IsPointerInvalid(fp) ||
		(uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// Setup for parsing engine

	SafePrint::ProcessResults_t TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::kMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable, sizeof(ParamIntoTable));

	// Analyze everything and determine what we'll output
	const uint_t bParseOk = TheProcessResults.FormatPreProcess(
		SafePrint::PARSEOPTION_DEFAULT, pFormat, StringLength(pFormat),
		uArgCount, ppArgs, BURGER_ARRAYSIZE(ParamIntoTable), ParamIntoTable);

	// If there were no problems, format the output and send the results out via
	// the RTL fprintf() function
	if (bParseOk && (TheProcessResults.m_uPhaseResults == kErrorNone)) {
		// format the output and send it along
		const uintptr_t uProcessedCount =
			TheProcessResults.GenerateFormattedOutputToFile(fp, uArgCount,
				ppArgs, TheProcessResults.m_uNumOutputCommands, ParamIntoTable);

		// Were we error free?
		if (TheProcessResults.m_uPhaseResults == kErrorNone) {
			return static_cast<intptr_t>(uProcessedCount);
		}
	}

	// An error must have occurred.  Handle it, and get the proper return value
	return TheProcessResults.ErrorHandler(pFormat, uArgCount, ppArgs);
}

/*! ************************************

	\brief Debug snprintf()

	Parse the parameters and do a printf() dump of the contents, used
	exclusively for debugging.

	* * Step 1) Parse the format string and arguments
	* * Step 2) if no issues found in Step 1, execute the list of format & copy
		output commands.

	\param pOutput Pointer to a output buffer
	\param uOutputSize Number of bytes in the output buffer
	\param pFormat Pointer to a "C" format string
	\param uArgCount Number of arguments in the argument array
	\param ppArgs Argument array, can be \ref NULL if uArgCount is zero

	\return Always returns zero

***************************************/

intptr_t BURGER_API Burger::DebugSnprintf(char* pOutput, uintptr_t uOutputSize,
	const char* pFormat, uintptr_t uArgCount,
	const ArgumentType** ppArgs) BURGER_NOEXCEPT
{
	if (IsPointerInvalid(pOutput) || IsPointerInvalid(pFormat) ||
		(uArgCount && IsPointerInvalid(ppArgs))) {
		return 0;
	}

	// parse loop
	SafePrint::ProcessResults_t TheProcessResults;
	SafePrint::ParamInfo_t ParamIntoTable[SafePrint::kMAX_OUTPUT_COMMANDS];
	MemoryClear(ParamIntoTable, sizeof(ParamIntoTable));

	const uintptr_t uFormatLength = StringLength(pFormat);
	const uint_t bParseOk = TheProcessResults.FormatPreProcess(
		SafePrint::PARSEOPTION_DEFAULT, pFormat, uFormatLength, uArgCount,
		ppArgs, BURGER_ARRAYSIZE(ParamIntoTable), ParamIntoTable);

	printf("\n\nprintf parse check\nFormat String Len=%zd is '%s'\n",
		uFormatLength, pFormat);
	printf("printfFormatPreProcess returned success=%d\n",
		static_cast<int>(bParseOk));

	char ErrorBuffer[256];
	error_get_string(
		ErrorBuffer, sizeof(ErrorBuffer), TheProcessResults.m_uPhaseResults);

	printf("Results:  Error='%s' at char pos #%zd (char %zd in sequence)\n",
		ErrorBuffer, TheProcessResults.m_uErrorCharPos,
		TheProcessResults.m_uErrorFormatSequencePos);
	printf("          Num Output Commands =%zd (of %zd max)\n",
		TheProcessResults.m_uNumOutputCommands,
		BURGER_ARRAYSIZE(ParamIntoTable));
	printf("          output Size =%zd (of %zd buffer)\n",
		TheProcessResults.m_uFormattedOutputLength, uOutputSize);

	for (uintptr_t i = 0; i < TheProcessResults.m_uNumOutputCommands; i++) {
		printf("Output Command %u:   CMD=%s\n", static_cast<uint_t>(i),
			SafePrint::GetOutputCommandName(ParamIntoTable[i].GetCommand()));
		printf("   display arg as:   %s\n",
			SafePrint::GetOutputDisplayType(ParamIntoTable[i].GetConversion()));
		printf("        arg index:   %d\n",
			static_cast<int>(ParamIntoTable[i].GetArgIndex()));
		printf("     option flags:   %4.4x\n",
			static_cast<int>(ParamIntoTable[i].GetFlags()));
		printf("            width:   %d",
			static_cast<int>(ParamIntoTable[i].GetWidth()));
		printf(
			(ParamIntoTable[i].IsFlagSet(SafePrint::CONVFLAG_WIDTH_SPECIFIED)) ?
                "\n" :
                " (not specified)\n");
		printf("        precision:   %d",
			static_cast<int>(ParamIntoTable[i].GetPrecision()));
		printf((ParamIntoTable[i].IsFlagSet(
				   SafePrint::CONVFLAG_PRECISION_SPECIFIED)) ?
                "\n" :
                " (not specified)\n");
		printf("    formatted len:  %d\n",
			static_cast<int>(ParamIntoTable[i].GetFormattedLength()));
		printf("       output len:  %d\n",
			static_cast<int>(ParamIntoTable[i].GetOutputLength()));
		printf("\n");
	}

	return 0;
}
