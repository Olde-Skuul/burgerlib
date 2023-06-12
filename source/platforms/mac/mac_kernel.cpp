/***************************************

	MacOS specific kernel functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "mac_kernel.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include <AEDataModel.h>
#include <AEInteraction.h>
#include <Gestalt.h>
#include <Patches.h>
#include <Processes.h>
#include <Traps.h>

/*! ************************************

	\enum Burger::MacOS::ePowerMacType
	\brief Type of detected Power Mac.

	The function get_power_mac_type() checks the mac if it's a specific type of
	Power Macintosh and returns this enumeration so a higher level function can
	make assumptions about the built-in hardware available on the motherboard.

	\maconly

	\sa get_power_mac_type()

***************************************/

/*! ************************************

	\brief Detect if a MacOS trap number exists.

	Detect if a specific 680x0 trap instruction is present.

	Due to the nature of this function, caching is impractical and it is not
	performed. This function is considered a performance bottleneck and should
	be called sparingly.

	\note This is a MacOS Classic function. It always returns \ref FALSE on
		Carbon targets.

	\param uTrapNum A valid MacOS 680x0 trap number from Traps.h.

	\return \ref TRUE if the requested trap index is present, \ref FALSE if not.

***************************************/

uint_t BURGER_API Burger::MacOS::is_trap_available(
	uint_t uTrapNum) BURGER_NOEXCEPT
{
	uint_t uResult = FALSE;

	//
	// Only matters when running 680x0 code or PowerPC code
	// with a 680x0 emulator
	//

#if !defined(BURGER_MACCARBON)
	do {

		// Failsafe
		if (uTrapNum < 0x10000U) {

			// Assume it's an OS trap!
			TrapType MyTrapType = OSTrap;

			// What trap is it?
			if (uTrapNum & 0x0800) {
				// Is it in the extended trap table?
				// Check if the extended trap table is even present by checking
				// if _InitGraf and _InitGraf+0x200 map to the same address.

				if (((uTrapNum & 0x03FF) >= 0x0200) &&
					(GetToolboxTrapAddress(_InitGraf) ==
						GetToolboxTrapAddress(_InitGraf + 0x200))) {
					// This trap can't be in the short table. I don't
					// support extended traps.
					break;
				}

				// Toolbox trap
				MyTrapType = ToolTrap;
			}

			if (NGetTrapAddress(static_cast<UInt16>(uTrapNum), MyTrapType) !=
				GetToolboxTrapAddress(_Unimplemented)) {
				// The trap is present
				uResult = TRUE;
			}
		}
	} while (0);

#else
	BURGER_UNUSED(uTrapNum);
#endif

	// Return the flag if I have the trap or not.
	return uResult;
}

/*! ************************************

	\brief Send a "Quit" event to the requested process

	Send an Apple Event to the process to tell the process to properly quit.

	\maconly

	\param pVictim Pointer to a process serial number of the process to kill

***************************************/

void BURGER_API Burger::MacOS::kill_process(
	ProcessSerialNumber* pVictim) BURGER_NOEXCEPT
{

	// Create a "Quit" event

	AEAddressDesc target;
	if (!AECreateDesc(typeProcessSerialNumber,
			static_cast<Ptr>(static_cast<void*>(pVictim)),
			sizeof(ProcessSerialNumber), &target)) {
		AppleEvent theEvent;
		OSErr err = AECreateAppleEvent('aevt', 'quit', &target,
			kAutoGenerateReturnID, kAnyTransactionID, &theEvent);
		AEDisposeDesc(&target);
		if (!err) {
			// Send the "Quit" event
			AESend(&theEvent, 0, kAENoReply + kAENeverInteract,
				kAENormalPriority, kAEDefaultTimeout, 0, 0);
			AEDisposeDesc(&theEvent);
		}
	}

	// Give some CPU time for the event to trigger

	uint_t uWaits = 7;
	do {
		EventRecord event;
		if (!WaitNextEvent(everyEvent, &event, 180, 0)) {
			break;
		}
	} while (--uWaits);
}

/*! ************************************

	\brief Send a "Quit" event to every other process

	Send a "Quit" event to every other app however, don't kill myself and kill
	the finder last

	\maconly

***************************************/

void BURGER_API Burger::MacOS::kill_other_processes(void) BURGER_NOEXCEPT
{
	// My apps own process number
	ProcessSerialNumber MyAppNumber;

	// Get my current app process number
	GetCurrentProcess(&MyAppNumber);

	// Next process in chain
	ProcessSerialNumber next;
	// Start following the process list
	next.highLongOfPSN = 0;
	next.lowLongOfPSN = kNoProcess;
	// Get my number
	GetNextProcess(&next);

	// Finder process ID
	ProcessSerialNumber finder;

	// Found the finder?
	uint_t bFoundFinder = FALSE;

	// Found another process?
	if (next.highLongOfPSN || next.lowLongOfPSN != kNoProcess) {
		do {
			ProcessSerialNumber current = next;

			// Preload the NEXT process since I may kill it now
			GetNextProcess(&next);

			Bool bIsFlag = FALSE;

			// Don't kill myself
			SameProcess(&current, &MyAppNumber, &bIsFlag);
			if (!bIsFlag) {

				// If I have found the finder, the rest are easy.
				// Otherwise...

				// Found it already?
				if (!bFoundFinder) {
					Str31 processName;
					FSSpec procSpec;
					ProcessInfoRec infoRec;

					infoRec.processInfoLength = sizeof(ProcessInfoRec);
					infoRec.processName = (StringPtr)&processName;
					infoRec.processAppSpec = &procSpec;

					// What process is this?
					GetProcessInformation(&current, &infoRec);
					if (infoRec.processSignature == 'MACS' &&
						infoRec.processType == 'FNDR') {

						// This is the finder!
						finder = current;
						bFoundFinder = TRUE;
						bIsFlag = TRUE;
					} else {
						bIsFlag = FALSE;
					}
				} else {

					// You may ask yourself, why check for finder when the
					// finder is already found? The finder has multiple
					// processes!!!

					// Just do a compare
					SameProcess(&current, &finder, &bIsFlag);
				}

				// Is this app not the finder?

				if (!bIsFlag) {
					kill_process(&current); // Kill it
				}
			}
		} while (next.highLongOfPSN || (next.lowLongOfPSN != kNoProcess));
	}

	// Now, did I locate the finder?

	if (bFoundFinder) {

		// Bye bye
		kill_process(&finder);
	}
}

/*! ************************************

	\brief Determine class of Power Mac

	Using Gestalt(), determine the type of Power Mac the code is running on.
	This is useful in determining the presence of a built-in ethernet port.

	\maconly

	\return Mac::ePowerMacType enumeration. ePowerMacType::kUnknown if not a
		Power Mac.

***************************************/

Burger::MacOS::ePowerMacType BURGER_API Burger::MacOS::get_power_mac_type(
	void) BURGER_NOEXCEPT
{

	// Assume failure
	ePowerMacType uResult = kUnknown;

	// What type of Power Mac is this?
	long lGestalt;
	if (!Gestalt(gestaltMachineType, &lGestalt)) {

		switch (lGestalt) {
		case gestaltAWS9150_80:
		case gestaltAWS9150_120:
		case gestaltPowerMac6100_60:
		case gestaltPowerMac6100_66:
		case gestaltPowerMac7100_66:
		case gestaltPowerMac7100_80:
		case gestaltPowerMac8100_80:
		case gestaltPowerMac8100_100:
		case gestaltPowerMac8100_110:
		case gestaltPowerMac8100_120:
			uResult = kPiltdownMan;
			break;

		case gestaltPowerMac7200:
		case gestaltPowerMac7300:
		case gestaltPowerMac7500:
		case gestaltPowerMac8500:
		case gestaltPowerMac9500:
		case gestaltPowerBook3400:
		case gestaltPowerBookG3:
		case gestaltPowerBookG3Series:
		case gestaltPowerBookG3Series2:
		case gestaltPowerMacG3:
		case gestaltPowerMacNewWorld:
			uResult = kPCIMachine;
			break;

		case gestaltPowerMac5200:
		case gestaltPowerMac6200:
			uResult = kCommSlotMachine;
			break;

		case gestaltPowerMac4400:
		case gestaltPowerMac4400_160:
		case gestaltPowerMac5400:
		case gestaltPowerMac5500:
		case gestaltPowerMac6400:
		case gestaltPowerMac6500:
			uResult = kPCIComm2Machine;
			break;

		// Check other machines
		default:
			if (!Gestalt(gestaltNameRegistryVersion, &lGestalt)) {
				// PowerMac G3 or higher
				uResult = kPCIMachine;
			}
		}
	}
	return uResult;
}

#endif