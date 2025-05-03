/***************************************

	Simple structures for MSDOS

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "msdos_structs.h"

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
#include "brmemoryfunctions.h"
#include "msdos_memory.h"

/*! ************************************

	\namespace Burger::MSDos

	\brief Functions specific to MS/DOS

	Functions and structures that exist in this namespace are only available on
	the MS/DOS version of Burgerlib. They should only be used on MS/DOS
	applications.

	\msdosonly

***************************************/

/*! ************************************

	\var _x32_zero_base_ptr

	\brief Pointer to the MSDOS real memory

	On the X32 dos extender, real memory is mapped to an address at runtime.
	This will retrieve that address.

	\note On DOS4GW, this is included in Burgerlib with a value of zero so code
	meant for X32 will work without changes on DOS4GW.

	\msdosonly
	\sa _x32_zero_base_selector

***************************************/

/*! ************************************

	\var _x32_zero_base_selector

	\brief Segment to the MSDOS real memory

	On the X32 dos extender, real memory is mapped to this segment at runtime.
	This will retrieve that segment value.

	\note On DOS4GW, this is included in Burgerlib with the _DATA segment so
	code meant for X32 will work without changes on DOS4GW.

	\msdosonly
	\sa _x32_zero_base_ptr

***************************************/

/*! ************************************

	\struct Burger::MSDos::Regs16_t

	\brief Structure to contain all the registers for calling 80x86 assembly

	\msdosonly
	\sa real_mode_interrupt() or x32_call_real_proc()

***************************************/

/*! ************************************

	\fn Burger::MSDos::zero_base(void)

	\brief Obtain the base pointer to the real memory

	\msdosonly

	\return uint8_t * where the real memory is mapped to

	\sa _x32_zero_base_ptr or _x32_zero_base_selector

***************************************/

/*! ************************************

	\fn Burger::MSDos::set_video_mode_int10h(uint32_t uMode)

	\brief Call interrupt 10H with AL set to the passed value.

	Interrupt 10H is used to set the video display mode to CGA/EGA/VGA. AH is
	set to zero. The value returned by the interrupt is returned.

	http://www.ctyme.com/intr/rb-0069.htm

	\msdosonly

	\sa init_serial_port_int14h(uint32_t, uint32_t),
		printer_port_int17h(uint32_t, uint32_t),
		real_mode_interrupt() and
		x32_call_real_proc()

***************************************/

/*! ************************************

	\fn Burger::MSDos::init_serial_port_int14h(
		uint32_t uPortFlags,uint32_t uPortNumber)

	\brief Call interrupt 14H with AL and DX set to the passed values.

	Interrupt 14H is used to call the serial BIOS, AH is set to zero.

	http://www.ctyme.com/intr/rb-0811.htm

	\msdosonly

	\param uPortFlags Value to set AL/AH
	\param uPortNumber Value to set DL/DH
	\return Value of the AX register on return

	\sa set_video_mode_int10h(uint32_t),
		printer_port_int17h(uint32_t, uint32_t),
		real_mode_interrupt() and x32_call_real_proc()

***************************************/

/*! ************************************

	\fn Burger::MSDos::printer_port_int17h(
		uint32_t uPrinterCommand, uint32_t uPrinterNumber)

	\brief Call interrupt 17H with AL/AH and DL/DH set to the passed values.

	Interrupt 17H is used to call the printer BIOS

	http://www.ctyme.com/intr/int-17.htm

	\msdosonly

	\param uPrinterCommand Value to set AL/AH
	\param uPrinterNumber Value to set DL/DH
	\return Value of the AX register on return

	\sa set_video_mode_int10h(uint32_t),
	init_serial_port_int14h(uint32_t, uint32_t),
	real_mode_interrupt() and x32_call_real_proc()

***************************************/

/*! ************************************

	\fn Burger::MSDos::real_mode_interrupt(
		uint32_t uInterrupt,const Regs16_t *pInput, Regs16_t *pOutput)

	\brief Call a MSDOS software interrupt

	Call a software interrupt with a set of register and
	return the state of the 80x86 after the call

	The pInput and pOutput pointers can point to the same buffer

	http://www.delorie.com/djgpp/doc/rbinter/ix/

	\msdosonly

	\param uInterrupt Interrupt to call
	\param pInput Pointer to registers to use when issuing the call
	\param pOutput Pointer to registers returned after issuing the call
	\return Contents of the AX register on return from the real mode call

	\sa set_video_mode_int10h(uint32_t),
		init_serial_port_int14h(uint32_t, uint32_t),
		printer_port_int17h(uint32_t, uint32_t),
		and x32_call_real_proc()

***************************************/

/*! ************************************

	\brief Call a real mode function (16 bit)
	
	This routine will allow a DOS application to call a real mode procedure
	routine via the X32 DOS extender.

	The input and output pointers can be the same value.

	http://www.delorie.com/djgpp/doc/rbinter/id/27/26.html

	\msdosonly and with the X32 dos extender and it is NOT reentrant

	\param pAddress Pointer to real memory to call (Segment/Offset)
	\param pInput Pointer to register set to use as input
	\param pOutput Pointer tp register set to use as output
	\return Value returned in the AX register after issuing the call
		to the real mode code

	\sa MSDos::real_mode_interrupt()

***************************************/

#if defined(BURGER_X32) || defined(DOXYGEN)

#if !defined(DOXYGEN)
static void CallMe(uint32_t Address);

// The code runs at offset 0, the data is at offset 100
// There are 20 bytes of data. So the buffer needs to be 120 bytes
// to execute

// Invoke the X32 call proc routine
#pragma aux CallMe = \
	"PUSH EBP" \
	"MOV EAX,0x250EH" \
	"XOR ECX,ECX" \
	"INT 021H" \
	"POP EBP" parm[ebx] modify[eax ebx ecx edx esi edi]

static const uint8_t RealCode[] = {
	0xB8, 0x00, 0x01,             // MOV AX,0100 (0)
	0x8E, 0xD8,                   // MOV DS,AX (3)
	0xB8, 0x00, 0x01,             // MOV AX,0100 (5)
	0x8E, 0xC0,                   // MOV ES,AX (8)
	0xB8, 0x00, 0x01,             // MOV AX,0100 (10)
	0xBB, 0x00, 0x01,             // MOV BX,0100 (13)
	0xB9, 0x00, 0x01,             // MOV CX,0100 (16)
	0xBA, 0x00, 0x01,             // MOV DX,0100 (19)
	0xBF, 0x00, 0x01,             // MOV DI,0100 (22)
	0xBE, 0x00, 0x01,             // MOV SI,0100 (25)
	0xBD, 0x00, 0x01,             // MOV BP,0100 (28)
	0x9A, 0x33, 0x12, 0x34, 0x12, // CALL 1234:1233 (31)
	0x2E,                         // CS: (36)
	0x8C, 0x1E, 0x00, 0x01,       // MOV [0100],DS (37)
	0x2E,                         // CS: (41)
	0x8C, 0x06, 0x02, 0x01,       // MOV [0102],ES (42)
	0x2E,                         // CS: (46)
	0xA3, 0x00, 0x01,             // MOV [100],AX (47)
	0x2E,                         // CS: (50)
	0x89, 0x1E, 0x00, 0x01,       // MOV [0100],BX (51)
	0x2E,                         // CS: (55)
	0x89, 0x0E, 0x00, 0x01,       // MOV [0100],CX
	0x2E,                         // CS: (60)
	0x89, 0x16, 0x00, 0x01,       // MOV [0100],DX
	0x2E,                         // CS: (65)
	0x89, 0x3E, 0x00, 0x01,       // MOV [0100],DI
	0x2E,                         // CS: (70)
	0x89, 0x36, 0x00, 0x01,       // MOV [0100],SI
	0x2E,                         // CS: (75)
	0x89, 0x2E, 0x00, 0x01,       // MOV [0100],BP
	0x9C,                         // PUSHF (80)
	0x58,                         // POP AX (81)
	0x2E,                         // CS: (82)
	0xA3, 0x00, 0x01,             // MOV [100],AX (83)
	0xCB                          // RETF (86)
};
#endif

int BURGER_API Burger::MSDos::x32_call_real_proc(uint32_t pAddress,
	const Regs16_t* pInput, Regs16_t* pOutput) BURGER_NOEXCEPT
{
	// Get real memory
	uint32_t pRealMemory = alloc_real(120);
	if (pRealMemory) {

		uint8_t* pFlatMemory =
			static_cast<uint8_t*>(real_to_protected(pRealMemory));
			memory_copy(pFlatMemory, RealCode, sizeof(RealCode));

		// Pass the input registers
		reinterpret_cast<uint16_t*>(pFlatMemory + 1)[0] = pInput->ds;
		reinterpret_cast<uint16_t*>(pFlatMemory + 6)[0] = pInput->es;
		reinterpret_cast<uint16_t*>(pFlatMemory + 11)[0] = pInput->ax;
		reinterpret_cast<uint16_t*>(pFlatMemory + 14)[0] = pInput->bx;
		reinterpret_cast<uint16_t*>(pFlatMemory + 17)[0] = pInput->cx;
		reinterpret_cast<uint16_t*>(pFlatMemory + 20)[0] = pInput->dx;
		reinterpret_cast<uint16_t*>(pFlatMemory + 23)[0] = pInput->di;
		reinterpret_cast<uint16_t*>(pFlatMemory + 26)[0] = pInput->si;
		reinterpret_cast<uint16_t*>(pFlatMemory + 29)[0] = pInput->bp;
		reinterpret_cast<uint32_t*>(pFlatMemory + 32)[0] = pAddress;

		// Set the return data
		reinterpret_cast<uint16_t*>(pFlatMemory + 39)[0] =
			static_cast<uint16_t>(pRealMemory + 100);
		reinterpret_cast<uint16_t*>(pFlatMemory + 44)[0] =
			static_cast<uint16_t>(pRealMemory + 102);
		reinterpret_cast<uint16_t*>(pFlatMemory + 48)[0] =
			static_cast<uint16_t>(pRealMemory + 104);
		reinterpret_cast<uint16_t*>(pFlatMemory + 53)[0] =
			static_cast<uint16_t>(pRealMemory + 106);
		reinterpret_cast<uint16_t*>(pFlatMemory + 58)[0] =
			static_cast<uint16_t>(pRealMemory + 108);
		reinterpret_cast<uint16_t*>(pFlatMemory + 63)[0] =
			static_cast<uint16_t>(pRealMemory + 110);
		reinterpret_cast<uint16_t*>(pFlatMemory + 68)[0] =
			static_cast<uint16_t>(pRealMemory + 112);
		reinterpret_cast<uint16_t*>(pFlatMemory + 73)[0] =
			static_cast<uint16_t>(pRealMemory + 114);
		reinterpret_cast<uint16_t*>(pFlatMemory + 78)[0] =
			static_cast<uint16_t>(pRealMemory + 116);
		reinterpret_cast<uint16_t*>(pFlatMemory + 84)[0] =
			static_cast<uint16_t>(pRealMemory + 118);

		// Call it via the shim
		CallMe(pRealMemory);

		// Get the result
		pOutput->ds = reinterpret_cast<uint16_t*>(pFlatMemory + 100)[0];
		pOutput->es = reinterpret_cast<uint16_t*>(pFlatMemory + 102)[0];
		pOutput->ax = reinterpret_cast<uint16_t*>(pFlatMemory + 104)[0];
		pOutput->bx = reinterpret_cast<uint16_t*>(pFlatMemory + 106)[0];
		pOutput->cx = reinterpret_cast<uint16_t*>(pFlatMemory + 108)[0];
		pOutput->dx = reinterpret_cast<uint16_t*>(pFlatMemory + 110)[0];
		pOutput->di = reinterpret_cast<uint16_t*>(pFlatMemory + 112)[0];
		pOutput->si = reinterpret_cast<uint16_t*>(pFlatMemory + 114)[0];
		pOutput->bp = reinterpret_cast<uint16_t*>(pFlatMemory + 116)[0];
		pOutput->flags = reinterpret_cast<uint16_t*>(pFlatMemory + 118)[0];

		// Release the memory
		free_real(pRealMemory);
		// Return the ax register
		return pOutput->ax;
	}
	memory_clear(pOutput, sizeof(pOutput[0]));
	// Set the carry flag (Fake an error)
	pOutput->flags = 1;
	return 0;
}

#endif

#endif