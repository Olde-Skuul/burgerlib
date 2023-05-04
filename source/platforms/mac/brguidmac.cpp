/***************************************

	MacOS version of GUID

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brguid.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)

#include "brerror.h"
#include "brglobals.h"
#include "brmd5.h"
#include "brmemoryfunctions.h"
#include "brnetmanager.h"

#include <DateTimeUtils.h>
#include <Events.h>
#include <Files.h>
#include <Folders.h>
#include <MacTypes.h>
#include <Timer.h>

#define UUID_C_100NS_PER_SEC 10000000U

/***************************************

	\brief Generate a fake ethernet address

	Get random data and time values and hash it up and use it as an ethernet
	address.

	\param pOutput Pointer to an uninitialized GUID buffer

	\sa GUID_init()

***************************************/

static void BURGER_API get_fake_mac_address(
	Burger::MacAddress_t* pOutput) BURGER_NOEXCEPT
{
	// Structure to hash
	struct Temp_t {
		// Powered up time
		UnsignedWide m_uMicroseconds;

		// Current time
		unsigned long m_uCurtime;

		// Mouse position
		Point m_MousePos;

		// Boot volume
		short vBootVol;

		// Volume attributes
		short vAttrib;

		// Directory ID of the system folder
		long vSysDirID;

		// Random data from the boot volume
		unsigned long vCreateDate;
		unsigned long vLastBackup;
		unsigned short vFileCount;
		unsigned short vDirStart;
		unsigned short vDirLength;
		unsigned short vAllocBlocks;
		unsigned long vAllocSize;
		unsigned long vClumpSize;
		unsigned short vBlockMap;
		unsigned short vFreeBlocks;
		unsigned long vNextFile;
	};

	Temp_t Stuff;
	Burger::MemoryClear(&Stuff, sizeof(Stuff));

	// Let's start by reading in random information to seed a random number

	// Get the power on time
	Microseconds(&Stuff.m_uMicroseconds);

	// Get the data/time
	GetDateTime(&Stuff.m_uCurtime);

	// Get the mouse location
	GetMouse(&Stuff.m_MousePos);

	// Get the boot volume information
	FindFolder(kOnSystemDisk, kSystemFolderType, kDontCreateFolder,
		&Stuff.vBootVol, &Stuff.vSysDirID);

	// Get information about the boot volume
	HParamBlockRec param;
	Burger::MemoryClear(&param, sizeof(param));
	param.volumeParam.ioVolIndex = 0;
	param.volumeParam.ioNamePtr = nullptr;
	param.volumeParam.ioVRefNum = Stuff.vBootVol;
	if (!PBHGetVInfoSync(&param)) {

		Stuff.vCreateDate = param.volumeParam.ioVCrDate;
		Stuff.vLastBackup = param.volumeParam.ioVLsMod;
		Stuff.vAttrib = param.volumeParam.ioVAtrb;
		Stuff.vFileCount = param.volumeParam.ioVNmFls;
		Stuff.vDirStart = param.volumeParam.ioVBitMap;
		Stuff.vDirLength = param.volumeParam.ioAllocPtr;
		Stuff.vAllocBlocks = param.volumeParam.ioVNmAlBlks;
		Stuff.vAllocSize = param.volumeParam.ioVAlBlkSiz;
		Stuff.vClumpSize = param.volumeParam.ioVClpSiz;
		Stuff.vBlockMap = param.volumeParam.ioAlBlSt;
		Stuff.vNextFile = param.volumeParam.ioVNxtCNID;
		Stuff.vFreeBlocks = param.volumeParam.ioVFrBlk;
	}

	// Let's mix it up to make a true mess
	Burger::MD5_t Mixer;
	Burger::Hash(&Mixer, &Stuff, sizeof(Stuff));
	Burger::MemoryCopy(pOutput, &Mixer, sizeof(*pOutput));
}

/***************************************

	Get the current time in 100ns increments

***************************************/

static void BURGER_API get_time100(uint64_t* pOutput) BURGER_NOEXCEPT
{
	// Internal values
	static uint_t bFirstTime = FALSE;
	static uint64_t uTimeAdjust;

	// Calculate the time in 100ns units since system boot
	if (!bFirstTime) {
		bFirstTime = TRUE;

		// Time in seconds
		unsigned long uDateTime;
		GetDateTime(&uDateTime);

		// Get the offset to GMT
		MachineLocation Location;
		ReadLocation(&Location);

		// Get the delta
		long lGMTDelta = Location.u.gmtDelta;
		// Handle wrap around
		lGMTDelta &= 0x00FFFFFF;
		if (lGMTDelta & 0x00800000) {
			lGMTDelta |= 0xFF000000;
		}
		uDateTime -= static_cast<unsigned long>(lGMTDelta);

		// Get microsecond time, but adjust to 100ns units
		uint64_t uMicroseconds;
		Microseconds(reinterpret_cast<UnsignedWide*>(&uMicroseconds));
		uMicroseconds *= 10U;

		// Seconds from 1904 to 1970
		uDateTime -= 2082844800;
		// Final time adjust value
		uTimeAdjust =
			(static_cast<uint64_t>(uDateTime) * UUID_C_100NS_PER_SEC) -
			uMicroseconds;
	}

	// Now get the current time in microseconds and add to boot time
	uint64_t uUSTime;
	Microseconds(reinterpret_cast<UnsignedWide*>(&uUSTime));
	uint64_t uUTC = (uUSTime * 10) + uTimeAdjust;

	// Offset between DTSS formatted times and Unix formatted times.
	*pOutput = uUTC + 0x01B21DD213814000ULL;
}

/***************************************

	\brief Create a new GUID

	Using a MAC address, current time and deep voodoo, generate a unique 128 bit
	number for labeling a data object.

	http://en.wikipedia.org/wiki/Globally_unique_identifier

	http://tools.ietf.org/html/rfc4122.html

	\param pOutput Pointer to an uninitialized GUID buffer
	\sa GUID_from_string() or GUID_to_string()

***************************************/

void BURGER_API Burger::GUID_init(GUID* pOutput) BURGER_NOEXCEPT
{
#define kMaxTimeAdjust 9

	MemoryClear(pOutput, sizeof(GUID));

#if defined(BURGER_CARBON)
	// Mac OS Carbon has a function in MacOSX that will do the job
	if (Globals::GetMacOSVersion() >= 0x1000U) {
		CFUUIDRef rUUID = CFUUIDCreate(kCFAllocatorDefault);
		reinterpret_cast<CFUUIDBytes*>(pOutput)[0] = CFUUIDGetUUIDBytes(rUUID);
		CFRelease(rUUID);
		return;
	}
#endif

	// Since MacOS 9 and earlier doesn't have a GUID generator, let's make one
	// from known data from the machine and the Ethernet MAC address to ensure
	// it's unique

	// Getting the MAC address is slow, so cache it.

	struct GUIDCache_t {
		// Last reading from Microseconds()
		uint64_t m_uPreviousTime;

		// Loaded or generated MAC address
		MacAddress_t m_Address;

		// Timer reverse sequence, starts random, is reset if time goes
		// backwards due to time zone change
		uint16_t m_uSequence;

		// Time adjust in case GUID_init() is called too often
		uint16_t m_uTimeAdjust;

		// TRUE if this structure is initialized
		uint8_t m_bAddressValid;
	};

	// Make sure it's all zeros
	static GUIDCache_t gCache = {0};

	// Only use one global for PowerPC efficiency
	GUIDCache_t* pCache = &gCache;

	// Initialized?
	if (!pCache->m_bAddressValid) {
		pCache->m_bAddressValid = TRUE;

		// Get the ethernet node address
		if (get_default_mac_address(&pCache->m_Address)) {

			// Can't find one? Make one up.
			get_fake_mac_address(&pCache->m_Address);
		}

		// Init the timer
		get_time100(&pCache->m_uPreviousTime);

		// Seed the random number generator
		UnsignedWide uWide;
		Microseconds(&uWide);

		// Start the sequencer
		pCache->m_uSequence = static_cast<uint16_t>(uWide.lo);
		pCache->m_uTimeAdjust = 0;
	}

	// At this point, all the hard stuff has been set up.
	// Let's make a GUID

	// Get the time mark
	uint64_t uNewTime;
	for (;;) {
		get_time100(&uNewTime);

		// Time went backwards? Reset
		if (uNewTime < pCache->m_uPreviousTime) {
			++pCache->m_uSequence;
			pCache->m_uTimeAdjust = 0;
			break;
		}

		// Next tick, use the time value
		if (uNewTime > pCache->m_uPreviousTime) {
			pCache->m_uTimeAdjust = 0;
			break;
		}

		// Time is the same?
		if (pCache->m_uTimeAdjust != kMaxTimeAdjust) {
			// Add an adjuster since GUID_init() is being called too fast
			++pCache->m_uTimeAdjust;
			break;
		}

		// Keep looping until the timer changes
	}

	// Update the last time mark
	pCache->m_uPreviousTime = uNewTime;

	// Adjust the time, to make sure GUIDs are different
	uNewTime += pCache->m_uTimeAdjust;

	// Create the GUID
	pOutput->Data1 = static_cast<uint32_t>(uNewTime);
	pOutput->Data2 = static_cast<uint16_t>(uNewTime >> 32U);
	pOutput->Data3 =
		static_cast<uint16_t>(static_cast<uint16_t>(uNewTime >> 48U) |
			static_cast<uint16_t>(1U << 12U));

	// Note, UUID requires the upper 2 bits to be %10
	const uint16_t uTempSequence = pCache->m_uSequence;
	pOutput->Data4[0] = static_cast<uint8_t>(
		(static_cast<uint8_t>(uTempSequence >> 8U) & 0x3FU) | 0x80U);
	pOutput->Data4[1] = static_cast<uint8_t>(uTempSequence);

	// Save off the ethernet address for the suffix
	MemoryCopy(&pOutput->Data4[2], &pCache->m_Address, 6);
}

#endif
