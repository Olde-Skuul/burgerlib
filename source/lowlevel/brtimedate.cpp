/***************************************

	Time Manager Class

	Copyright (c) 1995-2020 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtimedate.h"
#include "brmemoryfunctions.h"
#include "brnumberto.h"
#include <time.h>

#if defined(BURGER_DS)
#include <nitro/rtc/ARM9/api.h>
#endif

#if !(defined(BURGER_MACOSX) || defined(BURGER_IOS) || defined(BURGER_PS4) || \
	defined(BURGER_SHIELD) || defined(BURGER_LINUX) || \
	defined(BURGER_INTEL_COMPILER) || defined(BURGER_CLANG)) && \
	!defined(DOXYGEN)
// Visual studio 2015 or higher has timespec defined
#if !(BURGER_MSVC >= 190000000)
struct timespec {
	time_t tv_sec;   // seconds
	int32_t tv_nsec; // and nanoseconds
};
#endif
#endif

/*! ************************************

	\struct Burger::TimeDate_t
	\brief Structure to hold a date and time.

	This structure contains all the information of the date and time in a human
	readable format

***************************************/

/*! ************************************

	\brief Zero out all entries

	Initialize all entries to zero. This is considered an error state.

***************************************/

void Burger::TimeDate_t::Clear(void) BURGER_NOEXCEPT
{
	BURGER_STATIC_ASSERT(sizeof(TimeDate_t) == 12);
	// Fast way of clearing the structure
	uint32_t *pThis = reinterpret_cast<uint32_t *>(this);
	pThis[0] = 0;
	pThis[1] = 0;
	pThis[2] = 0;

}

/*! ************************************

	\brief Obtain the current localized time.

	A query is made to the target platform and the structure is filled in with
	the current date and time.

***************************************/

#if defined(BURGER_DS) || defined(DOXYGEN)
void Burger::TimeDate_t::GetTime(void) BURGER_NOEXCEPT
{
	RTCDate MyDate;
	RTCTimeEx MyTime;
	if (RTC_GetDateTimeExByTick(&MyDate, &MyTime) != RTC_RESULT_SUCCESS) {
		Clear();
	} else {
		m_uYear = static_cast<uint32_t>(MyDate.year + 2000);
		m_usMilliseconds = static_cast<uint16_t>(MyTime.millisecond);
		m_bMonth = static_cast<uint8_t>(MyDate.month);
		m_bDay = static_cast<uint8_t>(MyDate.day);
		m_bDayOfWeek = static_cast<uint8_t>(MyDate.week);
		m_bHour = static_cast<uint8_t>(MyTime.hour);
		m_bMinute = static_cast<uint8_t>(MyTime.minute);
		m_bSecond = static_cast<uint8_t>(MyTime.second);
	}
}

#elif !(defined(BURGER_WINDOWS) || defined(BURGER_XBOX360) || \
	defined(BURGER_VITA) || defined(BURGER_MAC) || defined(BURGER_MACOSX) || \
	defined(BURGER_IOS))
void Burger::TimeDate_t::GetTime(void) BURGER_NOEXCEPT
{
	time_t mytime;
	time(&mytime); // Get the unix time
	LoadTimeT(static_cast<uintptr_t>(mytime));
}
#endif

/*! ************************************

	\brief Convert the time to 23:23:21

	Convert the time into a string that is in the format of HH:MM:SS.
	Uses a 24 hour (Military time) format.

	\param pOutput C char buffer at least 9 bytes long to receive the string.
	\sa Burger::TimeDate_t::TimeToStringPM(char *pOutput) const

***************************************/

void Burger::TimeDate_t::TimeToString(char* pOutput) const BURGER_NOEXCEPT
{
	pOutput = NumberToAscii(pOutput, static_cast<uint32_t>(m_bHour));
	pOutput[0] = ':';
	pOutput = NumberToAscii(
		pOutput + 1, static_cast<uint32_t>(m_bMinute), LEADINGZEROS | 2);
	pOutput[0] = ':';
	NumberToAscii(
		pOutput + 1, static_cast<uint32_t>(m_bSecond), LEADINGZEROS | 2);
}

/*! ************************************

	\brief Convert the time to 11:23:21PM

	Convert the time into a string that is in the format of HH:MM:SSAM
	Uses a 12 hour (AM/PM) format.

	\param pOutput C char buffer at least 11 bytes long to receive the string.
	\sa Burger::TimeDate_t::TimeToString(char *pOutput) const

***************************************/

void Burger::TimeDate_t::TimeToStringPM(char* pOutput) const BURGER_NOEXCEPT
{
	char PM;
	uint32_t uHour = m_bHour;
	if (uHour < 12) {
		PM = 'A';
	} else {
		PM = 'P';
		uHour -= 12;
	}
	if (!uHour) {
		uHour = 12;
	}
	pOutput = NumberToAscii(pOutput, uHour);
	pOutput[0] = ':';
	pOutput = NumberToAscii(
		pOutput + 1, static_cast<uint32_t>(m_bMinute), LEADINGZEROS | 2);
	pOutput[0] = ':';
	pOutput = NumberToAscii(
		pOutput + 1, static_cast<uint32_t>(m_bSecond), LEADINGZEROS | 2);
	pOutput[0] = PM;
	pOutput[1] = 'M';
	pOutput[2] = 0;
}

/*! ************************************

	\brief Convert the date to 10/30/63

	Convert the date into a string that is in the format of MM/DD/YY

	\param pOutput C char buffer at least 9 bytes long to receive the string.
	\sa Burger::TimeDate_t::DateToStringVerbose(char *pOutput) const

***************************************/

void Burger::TimeDate_t::DateToString(char* pOutput) const BURGER_NOEXCEPT
{
	pOutput = NumberToAscii(pOutput, static_cast<uint32_t>(m_bMonth));
	pOutput[0] = '/';
	pOutput = NumberToAscii(
		pOutput + 1, static_cast<uint32_t>(m_bDay), LEADINGZEROS | 2);
	pOutput[0] = '/';
	NumberToAscii(
		pOutput + 1, static_cast<uint32_t>(m_uYear % 100U), LEADINGZEROS | 2);
}

/*! ************************************

	\brief Convert the date to Sunday, October 30, 1963

	Convert the date into a string that is in the format of "Sunday, October 30,
	1963"

	\param pOutput C char buffer at least 64 bytes long to receive the string.
	\sa Burger::TimeDate_t::DateToString(char *pOutput) const

***************************************/

void Burger::TimeDate_t::DateToStringVerbose(
	char* pOutput) const BURGER_NOEXCEPT
{
	uint_t uFailsafe = m_bDayOfWeek;
	if (uFailsafe >= 7) {
		uFailsafe = 0;
	}
	string_copy(pOutput, g_WeekDays[uFailsafe]);
	StringConcatenate(pOutput, ", ");
	uFailsafe = static_cast<uint_t>(m_bMonth - 1);
	if (uFailsafe >= 12) {
		uFailsafe = 0;
	}
	StringConcatenate(pOutput, g_Months[uFailsafe]);
	pOutput = pOutput + string_length(pOutput);
	pOutput[0] = ' ';
	pOutput = NumberToAscii(pOutput + 1, static_cast<uint32_t>(m_bDay));
	pOutput[0] = ',';
	pOutput[1] = ' ';
	NumberToAscii(pOutput + 2, static_cast<uint32_t>(m_uYear));
}

/*! ************************************

	\brief Read the time from a InputMemoryStream

	Read in a stream of 12 bytes and store it into the structure

	\param pInput Pointer to a valid InputMemoryStream

	\return \ref kErrorNone if no error, error code on error.

	\sa TimeDate_t::Write(OutputMemoryStream *pOutput) const

***************************************/

Burger::eError Burger::TimeDate_t::Read(
	Burger::InputMemoryStream* pInput) BURGER_NOEXCEPT
{
	eError uResult = kErrorReadFailure;
	if (pInput->BytesRemaining() < 12) {
		Clear();
	} else {
		m_uYear = pInput->GetWord32();
		m_usMilliseconds = pInput->GetShort();
		m_bMonth = pInput->GetByte();
		m_bDay = pInput->GetByte();
		m_bDayOfWeek = pInput->GetByte();
		m_bHour = pInput->GetByte();
		m_bMinute = pInput->GetByte();
		m_bSecond = pInput->GetByte();
		uResult = kErrorNone;
	}
	return uResult;
}

/*! ************************************

	\brief Write the time into a OutputMemoryStream

	Write 12 bytes into the stream to record the time

	\param pOutput Pointer to a valid OutputMemoryStream

	\return \ref kErrorNone if no error, error code on error.

	\sa TimeDate_t::Read(InputMemoryStream *pOutput)

***************************************/

Burger::eError Burger::TimeDate_t::Write(
	Burger::OutputMemoryStream* pOutput) const BURGER_NOEXCEPT
{
	pOutput->Append(m_uYear);
	pOutput->Append(m_usMilliseconds);
	pOutput->Append(m_bMonth);
	pOutput->Append(m_bDay);
	pOutput->Append(m_bDayOfWeek);
	pOutput->Append(m_bHour);
	pOutput->Append(m_bMinute);
	return pOutput->Append(m_bSecond);
}

/*! ************************************

	\brief Compares two \ref Burger::TimeDate_t records for equality.

	Compares two \ref TimeDate_t records and return -1 if the ``this`` is
	less than the record passed. Return 0 if they are the same and 1 if the
	parameter is less than the ``this``.

	\param pInput Pointer to an initialized \ref TimeDate_t record.
	\return -1 if less then, 0 if equal, 1 if greater than.

***************************************/

int Burger::TimeDate_t::Compare(const TimeDate_t* pInput) const BURGER_NOEXCEPT
{
	int iResult = 1;                                      // Assume it's greater
	if (m_uYear == pInput->m_uYear) {                     // Check years
		if (m_bMonth == pInput->m_bMonth) {               // Check months
			if (m_bDay == pInput->m_bDay) {               // Check days
				if (m_bHour == pInput->m_bHour) {         // Check hours
					if (m_bMinute == pInput->m_bMinute) { // Check minutes
						if (m_bSecond == pInput->m_bSecond) { // Check seconds
							if (m_usMilliseconds == pInput->m_usMilliseconds) {
								iResult = 0; // They are a match!!
							} else if (m_usMilliseconds <
								pInput->m_usMilliseconds) {
								iResult = -1;
							}
						} else if (m_bSecond < pInput->m_bSecond) {
							iResult = -1;
						}
					} else if (m_bMinute < pInput->m_bMinute) {
						iResult = -1;
					}
				} else if (m_bHour < pInput->m_bHour) {
					iResult = -1;
				}
			} else if (m_bDay < pInput->m_bDay) {
				iResult = -1;
			}
		} else if (m_bMonth < pInput->m_bMonth) {
			iResult = -1;
		}
	} else if (m_uYear < pInput->m_uYear) {
		iResult = -1;
	}
	return iResult;
}

/*! ************************************

	\brief Given a day, month and year, return the day of the week.

	0 = Sunday, 1 = Monday, 6 = Saturday.

	[Click here for information on how day was calculated.
	](https://web.archive.org/web/20161016225752/https://alcor.concordia.ca/~gpkatch/gdate-method.html)

	\return 0-6 for the day of the week

***************************************/

uint_t Burger::TimeDate_t::CalcDayOfWeek(void) BURGER_NOEXCEPT
{
	// The base month is March (3), so add 9 to adjust to wrap around making
	// March the value of zero

	const uint_t uBaseMonth = (m_bMonth + 9U) % 12U;

	// Use this trick to move the year back 2 months (10 and 11) will turn back
	// the year. Months 0-9 result in 0.

	const uint_t uYear = m_uYear - (uBaseMonth / 10U);

	// Let's go mad.

	// Start with the obvious, 365 days a year.
	uint_t uResult = 365U * uYear;

	// Add in the leap days
	uResult += uYear / 4U;

	// Remove the leap days not earned on centuries
	uResult -= uYear / 100U;

	// Psych! Add in the leap days for every 400 years.
	uResult += uYear / 400;

	// Add in the number of days for a month by using the MAGIC algoritm

	// Days  31,  28,  31,  30,  31,  31,  30,  31,  30,  31,  30,  31
	// Becomes...
	// New   31, 30, 31, 30,  31,  31,  30,  31,  30,  31,  31,  28
	// Total  0, 31, 61, 92, 122, 153, 184, 214, 245, 275, 306, 337
	// Fixed  0,306,612,918,1224,1530,1836,2142,2448,2754,3060,3366

	// This can be held in a fixed point number where 306 becomes 30.6 by
	// dividing by 10. Add 5 for a fractional round up.

	uResult += ((uBaseMonth * 306U) + 5U) / 10U;

	// Final touch, add in today (+2 adjusts to start on Sunday)
	uResult += (m_bDay + 2U);

	// 7 days a week and we're done.
	return uResult % 7U;
}

/*! ************************************

	\fn Burger::TimeDate_t::operator == (Burger::TimeDate_t const
		&rInput1,Burger::TimeDate_t const &rInput2)

	\brief Return \ref TRUE if the first is equal to the second

	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is equal to the second

***************************************/

/*! ************************************

	\fn Burger::TimeDate_t::operator != (Burger::TimeDate_t const
		&rInput1,Burger::TimeDate_t const &rInput2)

	\brief Return \ref TRUE if the first is not equal to the second

	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is not equal to the second

***************************************/

/*! ************************************

	\fn Burger::TimeDate_t::operator > (Burger::TimeDate_t const
		&rInput1,Burger::TimeDate_t const &rInput2)

	\brief Return \ref TRUE if the first is greater than to the second

	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is greater than to the second

***************************************/

/*! ************************************

	\fn Burger::TimeDate_t::operator >= (Burger::TimeDate_t const
		&rInput1,Burger::TimeDate_t const &rInput2)

	\brief Return \ref TRUE if the first is greater than or equal to the second

	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is greater than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::TimeDate_t::operator < (Burger::TimeDate_t const
		&rInput1,Burger::TimeDate_t const &rInput2)

	\brief Return \ref TRUE if the first is less than to the second
	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is less than to the second

***************************************/

/*! ************************************

	\fn Burger::TimeDate_t::operator <= (Burger::TimeDate_t const
		&rInput1,Burger::TimeDate_t const &rInput2)

	\brief Return \ref TRUE if the first is less than or equal to the second
	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is less than or equal to the second

***************************************/

/*! ************************************

	\brief Convert a time_t to a TimeDate_t.

	The time is assumed to be GMT and will be converted to local time before
	stored into the TimeDate_t structure

	\param uTimeT A time_t value obtained by a call to the "C" library.
	\return The structure is filled with the time as converted from the "C"
		library time.

***************************************/

Burger::eError Burger::TimeDate_t::LoadTimeT(uintptr_t uTimeT) BURGER_NOEXCEPT
{

#if defined(BURGER_DS)
	BURGER_UNUSED(uTimeT);
	Clear();
	return kErrorNotSupportedOnThisPlatform;
#else

	const time_t uInput = static_cast<time_t>(uTimeT);
	// Convert the time value to local time
	const struct tm* pTime = localtime(&uInput);
	eError uResult = kErrorInvalidParameter;
	if (!pTime) {
		Clear(); // Error, clear out the structure
	} else {
		m_uYear = static_cast<uint32_t>(pTime->tm_year + 1900);
		m_usMilliseconds = 0; // Not supported
		m_bMonth = static_cast<uint8_t>(pTime->tm_mon + 1);
		m_bDay = static_cast<uint8_t>(pTime->tm_mday);
		m_bDayOfWeek = static_cast<uint8_t>(pTime->tm_wday);
		m_bHour = static_cast<uint8_t>(pTime->tm_hour);
		m_bMinute = static_cast<uint8_t>(pTime->tm_min);
		m_bSecond = static_cast<uint8_t>(pTime->tm_sec);
		uResult = kErrorNone;
	}
	return uResult;
#endif
}

/*! ************************************

	\brief Convert a TimeDate_t to a time_t.

	The time is assumed to be local and will be converted to GMT time before
	stored into the time_t value

	\param pTimeT A time_t value to receive the new time
	\return The structure is filled with the time as converted from the "C"
		library time.

***************************************/

Burger::eError Burger::TimeDate_t::StoreTimeT(
	uintptr_t* pTimeT) const BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	pTimeT[0] = 0;
	return kErrorNotSupportedOnThisPlatform;
#else
	struct tm TheTime;
	MemoryClear(&TheTime, sizeof(TheTime));
	TheTime.tm_year = static_cast<int>(m_uYear - 1900);
	TheTime.tm_mon = static_cast<int>(m_bMonth - 1);
	TheTime.tm_mday = static_cast<int>(m_bDay);
	TheTime.tm_wday = static_cast<int>(m_bDayOfWeek);
	TheTime.tm_hour = static_cast<int>(m_bHour);
	TheTime.tm_min = static_cast<int>(m_bMinute);
	TheTime.tm_sec = static_cast<int>(m_bSecond);

	const time_t uTime = mktime(&TheTime);
	pTimeT[0] = static_cast<uintptr_t>(uTime);
	eError uResult = kErrorInvalidParameter;
	if (uTime != static_cast<time_t>(-1)) {
		uResult = kErrorNone;
	}
	return uResult;
#endif
}

/*! ************************************

	\brief Convert a timespec to a Burger::TimeDate_t.

	The time is assumed to be GMT and will be converted to local time before
	stored into the Burger::TimeDate_t structure

	\param pTimeSpec A timespec value to retrieve the time from
	\return Zero if no error, non-zero if the time is invalid

***************************************/

Burger::eError Burger::TimeDate_t::Load(
	const timespec* pTimeSpec) BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	BURGER_UNUSED(pTimeSpec);
	Clear();
	return kErrorNotSupportedOnThisPlatform;
#else
	const eError uResult = LoadTimeT(static_cast<uintptr_t>(pTimeSpec->tv_sec));
	if (!uResult) {
		m_usMilliseconds = static_cast<uint16_t>(pTimeSpec->tv_nsec / 1000000);
	}
	return uResult;
#endif
}

/*! ************************************

	\brief Convert a Burger::TimeDate_t to a timespec.

	The time is assumed to be local and will be converted to GMT before stored
	into the timespec structure

	\param pTimeSpec A timespec value to receive the time
	\return Zero if no error, non-zero if the time is invalid

***************************************/

Burger::eError Burger::TimeDate_t::Store(
	timespec* pTimeSpec) const BURGER_NOEXCEPT
{
#if defined(BURGER_DS)
	MemoryClear(pTimeSpec, sizeof(timespec));
	return kErrorNotSupportedOnThisPlatform;
#else
	uintptr_t TheTime;
	const eError uResult = StoreTimeT(&TheTime);
	if (!uResult) {
		pTimeSpec->tv_sec = static_cast<time_t>(TheTime);
		pTimeSpec->tv_nsec = m_usMilliseconds * 1000000;
	}
	return uResult;
#endif
}
