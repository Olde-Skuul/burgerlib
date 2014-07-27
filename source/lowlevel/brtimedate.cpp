/***************************************

	Time Manager Class

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brtimedate.h"
#include <time.h>

#if defined(BURGER_DS)
#include <nitro/rtc/ARM9/api.h>
#endif

#if !defined(BURGER_MACOSX) && !defined(BURGER_IOS) && !defined(BURGER_PS4) && !defined(BURGER_ANDROID) && !defined(DOXYGEN)
struct timespec {
	time_t tv_sec;	// seconds
	Int32 tv_nsec;	// and nanoseconds
};
#endif

/*! ************************************

	\struct Burger::TimeDate_t
	\brief Structure to hold a date and time.

	This structure contains all the information
	of the date and time in a human readable format

***************************************/

/*! ************************************

	\brief Zero out all entries

	Initialize all entries to zero. This is
	considered an error state.

	\return All entries are equal to zero.

***************************************/

void Burger::TimeDate_t::Clear(void)
{
	m_uYear = 0;
	reinterpret_cast<Word32 *>(&m_usMilliseconds)[0] = 0;
	reinterpret_cast<Word32 *>(&m_bDayOfWeek)[0] = 0;
}

/*! ************************************

	\brief Obtain the current localized time.

	A query is made to the target platform and the structure
	is filled in with the current date and time.

	\return The structure is set to the current localized time.

***************************************/

#if defined(BURGER_DS)
void Burger::TimeDate_t::GetTime(void)
{
	RTCDate MyDate;
	RTCTimeEx MyTime;
	if (RTC_GetDateTimeExByTick(&MyDate,&MyTime)!=RTC_RESULT_SUCCESS) {
		Clear();
	} else {
		m_uYear = static_cast<Word32>(MyDate.year+2000);
		m_usMilliseconds = static_cast<Word16>(MyTime.millisecond);
		m_bMonth = static_cast<Word8>(MyDate.month);
		m_bDay = static_cast<Word8>(MyDate.day);
		m_bDayOfWeek = static_cast<Word8>(MyDate.week);
		m_bHour = static_cast<Word8>(MyTime.hour);
		m_bMinute = static_cast<Word8>(MyTime.minute);
		m_bSecond = static_cast<Word8>(MyTime.second);
	}
}
#elif !defined(BURGER_WINDOWS) && !defined(BURGER_XBOX360) && !defined(BURGER_MAC)
void Burger::TimeDate_t::GetTime(void)
{
	time_t mytime;
	time(&mytime);			// Get the unix time
	LoadTimeT(static_cast<WordPtr>(mytime));
}
#endif

/*! ************************************

	\brief Convert the time to 23:23:21

	Convert the time into a string that is in the format of HH:MM:SS.
	Uses a 24 hour (Military time) format.

	\param pOutput C char buffer at least 9 bytes long to receive the string.
	\sa Burger::TimeDate_t::TimeToStringPM(char *pOutput) const

***************************************/

void Burger::TimeDate_t::TimeToString(char *pOutput) const
{
	pOutput = NumberToAscii(pOutput,static_cast<Word32>(m_bHour));
	pOutput[0] = ':';
	pOutput = NumberToAscii(pOutput+1,static_cast<Word32>(m_bMinute),LEADINGZEROS|2);
	pOutput[0] = ':';
	NumberToAscii(pOutput+1,static_cast<Word32>(m_bSecond),LEADINGZEROS|2);
}

/*! ************************************

	\brief Convert the time to 11:23:21PM

	Convert the time into a string that is in the format of HH:MM:SSAM
	Uses a 12 hour (AM/PM) format.

	\param pOutput C char buffer at least 11 bytes long to receive the string.
	\sa Burger::TimeDate_t::TimeToString(char *pOutput) const

***************************************/

void Burger::TimeDate_t::TimeToStringPM(char *pOutput) const
{
	char PM;
	Word32 uHour = m_bHour;
	if (uHour<12) {
		PM = 'A';
	} else {
		PM = 'P';
		uHour -= 12;
	}
	if (!uHour) {
		uHour = 12;
	}
	pOutput = NumberToAscii(pOutput,uHour);
	pOutput[0] = ':';
	pOutput = NumberToAscii(pOutput+1,static_cast<Word32>(m_bMinute),LEADINGZEROS|2);
	pOutput[0] = ':';
	pOutput = NumberToAscii(pOutput+1,static_cast<Word32>(m_bSecond),LEADINGZEROS|2);
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

void Burger::TimeDate_t::DateToString(char *pOutput) const
{
	pOutput = NumberToAscii(pOutput,static_cast<Word32>(m_bMonth));
	pOutput[0] = '/';
	pOutput = NumberToAscii(pOutput+1,static_cast<Word32>(m_bDay),LEADINGZEROS|2);
	pOutput[0] = '/';
	NumberToAscii(pOutput+1,static_cast<Word32>(m_uYear%100U),LEADINGZEROS|2);
}

/*! ************************************

	\brief Convert the date to Sunday, October 30, 1963

	Convert the date into a string that is in the format of "Sunday, October 30, 1963"

	\param pOutput C char buffer at least 64 bytes long to receive the string.
	\sa Burger::TimeDate_t::DateToString(char *pOutput) const

***************************************/

void Burger::TimeDate_t::DateToStringVerbose(char *pOutput) const
{
	Word uFailsafe = m_bDayOfWeek;
	if (uFailsafe>=7) {
		uFailsafe = 0;
	}
	StringCopy(pOutput,g_WeekDays[uFailsafe]);
	StringConcatenate(pOutput,", ");
	uFailsafe = static_cast<Word>(m_bMonth-1);
	if (uFailsafe>=12) {
		uFailsafe = 0;
	}
	StringConcatenate(pOutput,g_Months[uFailsafe]);
	pOutput = pOutput+StringLength(pOutput);
	pOutput[0] = ' ';
	pOutput = NumberToAscii(pOutput+1,static_cast<Word32>(m_bDay));
	pOutput[0] = ',';
	pOutput[1] = ' ';
	NumberToAscii(pOutput+2,static_cast<Word32>(m_uYear));
}

/*! ************************************

	\brief Read the time from a Burger::InputMemoryStream

	Read in a stream of 12 bytes and store it into the structure

	\param pInput Pointer to a valid Burger::InputMemoryStream
	\sa Burger::TimeDate_t::Write(Burger::OutputMemoryStream *pOutput) const

***************************************/

Word Burger::TimeDate_t::Read(Burger::InputMemoryStream *pInput)
{
	Word uResult;
	if (pInput->BytesRemaining()<12) {
		Clear();
		uResult = TRUE;
	} else {
		m_uYear = pInput->GetWord32();
		m_usMilliseconds = pInput->GetShort();
		m_bMonth = pInput->GetByte();
		m_bDay = pInput->GetByte();
		m_bDayOfWeek = pInput->GetByte();
		m_bHour = pInput->GetByte();
		m_bMinute = pInput->GetByte();
		m_bSecond = pInput->GetByte();
		uResult = FALSE;
	}
	return uResult;
}

/*! ************************************

	\brief Write the time into a Burger::InputMemoryStream

	Write 12 bytes into the stream to record the time

	\param pOutput Pointer to a valid Burger::OutputMemoryStream
	\sa Burger::TimeDate_t::Read(Burger::InputMemoryStream *pOutput)

***************************************/

Word Burger::TimeDate_t::Write(Burger::OutputMemoryStream *pOutput) const
{
	Word uResult = pOutput->Append(m_uYear);
	uResult |= pOutput->Append(m_usMilliseconds);
	uResult |= pOutput->Append(m_bMonth);
	uResult |= pOutput->Append(m_bDay);
	uResult |= pOutput->Append(m_bDayOfWeek);
	uResult |= pOutput->Append(m_bHour);
	uResult |= pOutput->Append(m_bMinute);
	uResult |= pOutput->Append(m_bSecond);
	return uResult;
}

/*! ************************************

	\brief Compares two \ref Burger::TimeDate_t records for equality.

	Compares two \ref Burger::TimeDate_t records and
	return -1 if the <b>this</b> is less than the record passed.
	return 0 if they are the same and 1 if the parameter
	is less than the <b>this</b>.

	\param pInput Pointer to an initialized \ref Burger::TimeDate_t record.
	\return -1 if less then, 0 if equal, 1 if greater than.

***************************************/

int Burger::TimeDate_t::Compare(const TimeDate_t *pInput) const
{
	int iResult = 1;				// Assume it's greater
	if (m_uYear==pInput->m_uYear) {	// Check years
		if (m_bMonth==pInput->m_bMonth) {	// Check months
			if (m_bDay==pInput->m_bDay) {		// Check days
				if (m_bHour==pInput->m_bHour) {	// Check hours
					if (m_bMinute==pInput->m_bMinute) {	// Check minutes
						if (m_bSecond==pInput->m_bSecond) {	// Check seconds
							if (m_usMilliseconds==pInput->m_usMilliseconds) {
								iResult = 0;		// They are a match!!
							} else if (m_usMilliseconds<pInput->m_usMilliseconds) {
								iResult = -1;
							}
						} else if (m_bSecond<pInput->m_bSecond) {
							iResult = -1;
						}
					} else if (m_bMinute<pInput->m_bMinute) {
						iResult = -1;
					}
				} else if (m_bHour<pInput->m_bHour) {
					iResult = -1;
				}
			} else if (m_bDay<pInput->m_bDay) {
				iResult = -1;
			}
		} else if (m_bMonth<pInput->m_bMonth) {
			iResult = -1;
		}
	} else if (m_uYear<pInput->m_uYear) {
		iResult = -1;
	}
	return iResult;
}

/*! ************************************

	\fn Burger::TimeDate_t::operator == (Burger::TimeDate_t const &rInput1,Burger::TimeDate_t const &rInput2)
	\brief Return \ref TRUE if the first is equal to the second
	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is equal to the second

***************************************/

/*! ************************************

	\fn Burger::TimeDate_t::operator != (Burger::TimeDate_t const &rInput1,Burger::TimeDate_t const &rInput2)
	\brief Return \ref TRUE if the first is not equal to the second
	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is not equal to the second

***************************************/

/*! ************************************

	\fn Burger::TimeDate_t::operator > (Burger::TimeDate_t const &rInput1,Burger::TimeDate_t const &rInput2)
	\brief Return \ref TRUE if the first is greater than to the second
	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is greater than to the second

***************************************/

/*! ************************************

	\fn Burger::TimeDate_t::operator >= (Burger::TimeDate_t const &rInput1,Burger::TimeDate_t const &rInput2)
	\brief Return \ref TRUE if the first is greater than or equal to the second
	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is greater than or equal to the second

***************************************/

/*! ************************************

	\fn Burger::TimeDate_t::operator < (Burger::TimeDate_t const &rInput1,Burger::TimeDate_t const &rInput2)
	\brief Return \ref TRUE if the first is less than to the second
	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is less than to the second

***************************************/

/*! ************************************

	\fn Burger::TimeDate_t::operator <= (Burger::TimeDate_t const &rInput1,Burger::TimeDate_t const &rInput2)
	\brief Return \ref TRUE if the first is less than or equal to the second
	\param rInput1 Burger::TimeDate_t to test
	\param rInput2 Burger::TimeDate_t to test against
	\return \ref TRUE if the first is less than or equal to the second

***************************************/


/*! ************************************

	\brief Convert a time_t to a TimeDate_t.

	The time is assumed to be GMT and will be converted to 
	local time before stored into the TimeDate_t structure

	\param uTimeT A time_t value obtained by a call to the "C" library.
	\return The structure is filled with the time as converted from the "C" library time.

***************************************/

Word Burger::TimeDate_t::LoadTimeT(WordPtr uTimeT)
{
	Word uResult = TRUE;
#if defined(BURGER_DS)
	uTimeT=0;
	Clear();
#else
	time_t uInput = static_cast<time_t>(uTimeT);
	const struct tm *pTime = localtime(&uInput);	// Convert the time value to local time
	if (!pTime) {
		Clear();		// Error, clear out the structure
	} else {
		m_uYear = static_cast<Word32>(pTime->tm_year+1900);
		m_usMilliseconds = 0;		// Not supported
		m_bMonth = static_cast<Word8>(pTime->tm_mon+1);
		m_bDay = static_cast<Word8>(pTime->tm_mday);
		m_bDayOfWeek = static_cast<Word8>(pTime->tm_wday);
		m_bHour = static_cast<Word8>(pTime->tm_hour);
		m_bMinute = static_cast<Word8>(pTime->tm_min);
		m_bSecond = static_cast<Word8>(pTime->tm_sec);
		uResult = FALSE;
	}
#endif
	return uResult;
}

/*! ************************************

	\brief Convert a TimeDate_t to a time_t.

	The time is assumed to be local and will be converted to 
	GMT time before stored into the time_t value

	\param pTimeT A time_t value to receive the new time
	\return The structure is filled with the time as converted from the "C" library time.

***************************************/

Word Burger::TimeDate_t::StoreTimeT(WordPtr *pTimeT) const
{
	Word uResult = TRUE;
#if defined(BURGER_DS)
	pTimeT[0]=0;
#else
	struct tm TheTime;
	TheTime.tm_year = static_cast<int>(m_uYear-1900);
	TheTime.tm_mon = static_cast<int>(m_bMonth-1);
	TheTime.tm_mday = static_cast<int>(m_bDay);
	TheTime.tm_wday = static_cast<int>(m_bDayOfWeek);
	TheTime.tm_hour = static_cast<int>(m_bHour);
	TheTime.tm_min = static_cast<int>(m_bMinute);
	TheTime.tm_sec = static_cast<int>(m_bSecond);
	time_t uTime = mktime(&TheTime);
	pTimeT[0] = static_cast<WordPtr>(uTime);
	if (uTime!=static_cast<time_t>(-1)) {
		uResult = FALSE;
	}
#endif
	return uResult;
}

/*! ************************************

	\brief Convert a timespec to a Burger::TimeDate_t.

	The time is assumed to be GMT and will be converted to 
	local time before stored into the Burger::TimeDate_t structure

	\param pTimeSpec A timespec value to retrieve the time from
	\return Zero if no error, non-zero if the time is invalid

***************************************/

Word Burger::TimeDate_t::Load(const timespec *pTimeSpec)
{
#if defined(BURGER_DS)
	Word uResult = TRUE;
	pTimeSpec=NULL;
	Clear();
#else
	Word uResult = LoadTimeT(static_cast<WordPtr>(pTimeSpec->tv_sec));
	if (!uResult) {
		m_usMilliseconds = static_cast<Word16>(pTimeSpec->tv_nsec/1000000);
	}
#endif
	return uResult;
}

/*! ************************************

	\brief Convert a Burger::TimeDate_t to a timespec.

	The time is assumed to be local and will be converted to 
	GMT before stored into the timespec structure

	\param pTimeSpec A timespec value to receive the time
	\return Zero if no error, non-zero if the time is invalid

***************************************/

Word Burger::TimeDate_t::Store(timespec *pTimeSpec) const
{
#if defined(BURGER_DS)
	Word uResult = TRUE;
	pTimeSpec = NULL;
#else
	WordPtr TheTime;
	Word uResult = StoreTimeT(&TheTime);
	if (!uResult) {
		pTimeSpec->tv_sec = static_cast<time_t>(TheTime);
		pTimeSpec->tv_nsec = m_usMilliseconds*1000000;
	}
#endif
	return uResult;
}

