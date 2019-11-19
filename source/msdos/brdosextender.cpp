/***************************************

	Dos extender functions

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdosextender.h"

/***************************************

	This code is DOS Only!

***************************************/

#if defined(BURGER_MSDOS) || defined(DOXYGEN)
#include "brdebug.h"
#include "brmemoryfunctions.h"
#include <stdlib.h>

// Force including the x32.library
// if using the x32 dos extender

#if defined(BURGER_X32)
#include <X32.h>
#pragma library ("x32.lib");
#endif

// Documentation to assembly functions

/*! ************************************

	\struct Burger::Regs16

	\brief Structure to contain all the registers for calling 80x86 assembly
	
	\msdosonly
	\sa Int86x() or CallRealProcX32()
	
***************************************/


/*! ************************************

	\fn BURGER_INLINE Word8 *ZeroBase(void)

	\brief Obtain the base pointer to the real memory
	
	\msdosonly
	\return void * where the real memory is mapped to
	
	\sa _x32_zero_base_ptr or _x32_zero_base_selector
	
***************************************/

/*! ************************************

	\fn void BURGER_API CallInt10(Word EAX)

	\brief Call interrupt 10H with AL/AH set to the passed value.
	
	Interrupt 10H is used to set the video display mode to CGA/EGA/VGA
	
	http://www.delorie.com/djgpp/doc/rbinter/ix/10/
	
	\msdosonly
	
	\sa CallInt14(), CallInt17(), Int86x() and CallRealProcX32()
	
***************************************/

/*! ************************************

	\fn Word BURGER_API CallInt14(Word EAX,Word EDX)

	\brief Call interrupt 14H with AL/AH and DL/DH set to the passed values.
	
	Interrupt 14H is used to call the serial BIOS
	
	http://www.delorie.com/djgpp/doc/rbinter/ix/14/
	
	\msdosonly
	
	\param EAX Value to set AL/AH
	\param EDX Value to set DL/DH
	\return Value of the AX register on return

	\sa CallInt10(), CallInt17(), Int86x() and CallRealProcX32()
	
***************************************/

/*! ************************************

	\fn Word BURGER_API CallInt17(Word EAX,Word EDX)

	\brief Call interrupt 17H with AL/AH and DL/DH set to the passed values.
	
	Interrupt 17H is used to call the printer BIOS
	
	http://www.delorie.com/djgpp/doc/rbinter/ix/17/
	
	\msdosonly
	
	\param EAX Value to set AL/AH
	\param EDX Value to set DL/DH
	\return Value of the AX register on return
	
	\sa CallInt10(), CallInt14(), Int86x() and CallRealProcX32()
	
***************************************/

/*! ************************************

	\fn Int32 BURGER_API Int86x(Word32 uInterrupt,const Burger::Regs16 *pInput,Burger::Regs16 *pOutput)

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
	
	\sa CallInt10(), CallInt14(), CallInt14() and CallRealProcX32()
	
***************************************/

/*! ************************************

	\fn void BURGER_API SetBothInts(Word32 uInterrupt,void far *pCode)

	\brief Intercept both the real the protected IRQ vectors
	
	Call the dos extender to intercept both the real and protected IRQ vector
	and route it through a protected mode IRQ
	
	\msdosonly
	
	\param uInterrupt Interrupt to intercept
	\param pCode Pointer to the function to call when the interrupt is tripped
	
	\sa SetProtInt(), SetRealInt(), GetProtInt() and GetRealInt()
	
***************************************/

/*! ************************************

	\fn void BURGER_API SetProtInt(Word32 uInterrupt,void far *pCode)

	\brief Intercept the protected IRQ vector
	
	Call the dos extender to intercept the protected IRQ vector
	and route it through a protected mode IRQ
	
	\msdosonly
	
	\param uInterrupt Interrupt to intercept
	\param pCode Pointer to the function to call when the interrupt is tripped
	
	\sa SetBothInts(), SetRealInt(), GetProtInt() and GetRealInt()
	
***************************************/

/*! ************************************

	\fn void BURGER_API SetRealInt(Word32 uInterrupt,Word32 pCode)

	\brief Intercept the real mode IRQ vector
	
	Call the dos extender to intercept the real mode IRQ vector
	and route it through a real mode IRQ
	
	\msdosonly
	
	\param uInterrupt Interrupt to intercept
	\param pCode Pointer to the function to call when the interrupt is tripped
	
	\sa SetBothInts(), SetProtInt(), GetProtInt() and GetRealInt()
	
***************************************/

/*! ************************************

	\fn void far * BURGER_API GetProtInt(Word32 uInterrupt)

	\brief Call the dos extender to get the current protected mode IRQ vector
	
	Call the dos extender to retrieve the protected mode IRQ vector
	
	\msdosonly
	
	\param uInterrupt Interrupt to intercept
	\returns Pointer to the function that is called when the interrupt is tripped
	
	\sa SetBothInts(), SetProtInt(), SetRealInt() and GetRealInt()
	
***************************************/

/*! ************************************

	\fn Word32 BURGER_API GetRealInt(Word32 uInterrupt)

	\brief Call the dos extender to get the current real mode IRQ vector
	
	Call the dos extender to retrieve the real mode IRQ vector
	
	\msdosonly
	
	\param uInterrupt Interrupt to intercept
	\returns Real mode (SEGMENT:OFFSET) pointer to the function that is called when the interrupt is tripped
	
	\sa SetBothInts(), SetProtInt(), SetRealInt() and GetProtInt()
	
***************************************/

/*! ************************************

	\fn void * BURGER_API MapPhysicalAddress(void *pInput,Word32 uLength)

	\brief Map memory from a device into protected mode
	
	Call the dos extender to remap extended memory to protected mode
	
	\msdosonly
	
	\param pInput Pointer to extended memory
	\param uLength Length in bytes of the extended memory chunk
	\returns Protected mode pointer to the extended memory
		
	\sa RealToProtectedPtr()
	
***************************************/






/*! ************************************

	\fn int BURGER_API CallRealProcX32(Word32,const Burger::Regs16 *,Burger::Regs16 *)

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
	
	\sa Int86x()
	
***************************************/

static void CallMe(Word32 Address);

// The code runs at offset 0, the data is at offset 100
// There are 20 bytes of data. So the buffer needs to be 120 bytes
// to execute

#pragma aux CallMe = /* Invoke the X32 call proc routine */ \
	"PUSH EBP" \
	"MOV EAX,0x250EH"	\
	"XOR ECX,ECX" \
	"INT 021H" \
	"POP EBP" \
	parm [ebx] \
	modify [eax ebx ecx edx esi edi]

static const Word8 RealCode[] = {
	0xB8,0x00,0x01,		// MOV AX,0100 (0)
	0x8E,0xD8,			// MOV DS,AX (3)
	0xB8,0x00,0x01,		// MOV AX,0100 (5)
	0x8E,0xC0,			// MOV ES,AX (8)
	0xB8,0x00,0x01,		// MOV AX,0100 (10)
	0xBB,0x00,0x01,		// MOV BX,0100 (13)
	0xB9,0x00,0x01,		// MOV CX,0100 (16)
	0xBA,0x00,0x01,		// MOV DX,0100 (19)
	0xBF,0x00,0x01,		// MOV DI,0100 (22)
	0xBE,0x00,0x01,		// MOV SI,0100 (25)
	0xBD,0x00,0x01,		// MOV BP,0100 (28)
	0x9A,0x33,0x12,0x34,0x12,	// CALL 1234:1233 (31)
	0x2E,				// CS: (36)
	0x8C,0x1E,0x00,0x01,	// MOV [0100],DS (37)
	0x2E,				// CS: (41)
	0x8C,0x06,0x02,0x01,	// MOV [0102],ES (42)
	0x2E,				// CS: (46)
	0xA3,0x00,0x01,		// MOV [100],AX (47)
	0x2E,				// CS: (50)
	0x89,0x1E,0x00,0x01,	// MOV [0100],BX (51)
	0x2E,					// CS: (55)
	0x89,0x0E,0x00,0x01,    // MOV [0100],CX
	0x2E,					// CS: (60)
	0x89,0x16,0x00,0x01,    // MOV [0100],DX
	0x2E,					// CS: (65)
	0x89,0x3E,0x00,0x01,    // MOV [0100],DI
	0x2E,					// CS: (70)
	0x89,0x36,0x00,0x01,    // MOV [0100],SI
	0x2E,			   		// CS: (75)
	0x89,0x2E,0x00,0x01,	// MOV [0100],BP
	0x9C,				// PUSHF (80)
	0x58,				// POP AX (81)
	0x2E,				// CS: (82)
	0xA3,0x00,0x01,		// MOV [100],AX (83)
	0xCB					// RETF (86)
};

int BURGER_API CallRealProcX32(Word32 pAddress,const Burger::Regs16 *pInput,Burger::Regs16 *pOutput)
{
	Word32 pRealMemory = AllocRealMemory(120);		// Get real memory
	if (pRealMemory) {
		Word8 *pFlatMemory = static_cast<Word8 *>(RealToProtectedPtr(pRealMemory));
		Burger::MemoryCopy(pFlatMemory,RealCode,sizeof(RealCode));

		// Pass the input registers
		reinterpret_cast<Word16 *>(pFlatMemory+1)[0] = pInput->ds;		
		reinterpret_cast<Word16 *>(pFlatMemory+6)[0] = pInput->es;
		reinterpret_cast<Word16 *>(pFlatMemory+11)[0] = pInput->ax;
		reinterpret_cast<Word16 *>(pFlatMemory+14)[0] = pInput->bx;
		reinterpret_cast<Word16 *>(pFlatMemory+17)[0] = pInput->cx;
		reinterpret_cast<Word16 *>(pFlatMemory+20)[0] = pInput->dx;
		reinterpret_cast<Word16 *>(pFlatMemory+23)[0] = pInput->di;
		reinterpret_cast<Word16 *>(pFlatMemory+26)[0] = pInput->si;
		reinterpret_cast<Word16 *>(pFlatMemory+29)[0] = pInput->bp;
		reinterpret_cast<Word32 *>(pFlatMemory+32)[0] = pAddress;

		// Set the return data
		reinterpret_cast<Word16 *>(pFlatMemory+39)[0] = static_cast<Word16>(pRealMemory+100);	
		reinterpret_cast<Word16 *>(pFlatMemory+44)[0] = static_cast<Word16>(pRealMemory+102);
		reinterpret_cast<Word16 *>(pFlatMemory+48)[0] = static_cast<Word16>(pRealMemory+104);
		reinterpret_cast<Word16 *>(pFlatMemory+53)[0] = static_cast<Word16>(pRealMemory+106);
		reinterpret_cast<Word16 *>(pFlatMemory+58)[0] = static_cast<Word16>(pRealMemory+108);
		reinterpret_cast<Word16 *>(pFlatMemory+63)[0] = static_cast<Word16>(pRealMemory+110);
		reinterpret_cast<Word16 *>(pFlatMemory+68)[0] = static_cast<Word16>(pRealMemory+112);
		reinterpret_cast<Word16 *>(pFlatMemory+73)[0] = static_cast<Word16>(pRealMemory+114);
		reinterpret_cast<Word16 *>(pFlatMemory+78)[0] = static_cast<Word16>(pRealMemory+116);
		reinterpret_cast<Word16 *>(pFlatMemory+84)[0] = static_cast<Word16>(pRealMemory+118);
		
		// Call it via the shim
		CallMe(pRealMemory);	

		// Get the result
		pOutput->ds = reinterpret_cast<Word16 *>(pFlatMemory+100)[0];	
		pOutput->es = reinterpret_cast<Word16 *>(pFlatMemory+102)[0];
		pOutput->ax = reinterpret_cast<Word16 *>(pFlatMemory+104)[0];
		pOutput->bx = reinterpret_cast<Word16 *>(pFlatMemory+106)[0];
		pOutput->cx = reinterpret_cast<Word16 *>(pFlatMemory+108)[0];
		pOutput->dx = reinterpret_cast<Word16 *>(pFlatMemory+110)[0];
		pOutput->di = reinterpret_cast<Word16 *>(pFlatMemory+112)[0];
		pOutput->si = reinterpret_cast<Word16 *>(pFlatMemory+114)[0];
		pOutput->bp = reinterpret_cast<Word16 *>(pFlatMemory+116)[0];
		pOutput->flags = reinterpret_cast<Word16 *>(pFlatMemory+118)[0];
	
		// Release the memory
		DeallocRealMemory(pRealMemory);
		// Return the ax register
		return pOutput->ax;
	}
	Burger::MemoryClear(pOutput,sizeof(pOutput[0]));
	// Set the carry flag (Fake an error)
	pOutput->flags = 1;
	return 0;
}

/*! ************************************

	\brief Convert a real mode address to a flat address
	
	Convert an encoded real mode address (16:16) (Segment:Offset)
	into a protected mode pointer. The protected pointer
	allows the 32 bit flat code to access the memory.

	\note Only available on MSDOS
	
	\param pReal Real memory pointer (Segment/Offset)
	\return Protected mode pointer that points to the same memory
	
	\sa GetRealBufferProtectedPtr() or GetRealBufferPtr()

***************************************/

void * BURGER_API RealToProtectedPtr(Word32 pReal)
{
	Word32 uFlattened = ((pReal>>12)&0xFFFFFFF0U)+static_cast<Word16>(pReal);
#if defined(BURGER_X32)
	return ZeroBase()+uFlattened;
#else
	return reinterpret_cast<void *>(uFlattened);
#endif
}

/*! ************************************

	\brief Get the pointer to the shared protected memory buffer
	
	Allocate an 8K buffer in real memory for use in DOS calls.
	Return the pointer in protected memory.
	
	If the memory allocation fails, the function will return \ref NULL
	
	\note Only available on MSDOS
	
	\return Protected mode pointer to the 8K shared real mode buffer
	\sa GetRealBufferPtr() and RealToProtectedPtr()
	
***************************************/

void * BURGER_API GetRealBufferProtectedPtr(void)
{
	// Get the buffer pointer
	Word32 pReal = GetRealBufferPtr();
	// Did it allocate?
	if (pReal) {
		// Convert to real pointer
		return RealToProtectedPtr(pReal);
	}
	// Error!
	return NULL;
}

/*! ************************************

	\fn Word32 BURGER_API GetRealBufferPtr(void)
	\brief Return pointer to shared real buffer
	
	Allocate an 8K buffer in real memory for use in DOS calls.
	Return the pointer in SEGMENT:OFFSET format

	\note Only available on MSDOS
	
	\return 0 if buffer allocation failed, or SEGMENT:OFFSET of the buffer
	\sa GetRealBufferProtectedPtr()

***************************************/

static Word32 pRealBuffer;	// Cached pointer to real memory

static void ReleaseBuff(void)
{
	DeallocRealMemory(pRealBuffer);		// Release the memory
	pRealBuffer = 0;
}

Word32 BURGER_API GetRealBufferPtr(void)
{
	// Is the buffer already allocated?
	Word32 pReal = pRealBuffer;
	if (!pReal) {
		// Get some REAL memory
		pReal = AllocRealMemory(8192);
		if (pReal) {
			// Save in global
			pRealBuffer = pReal;
			// Allow release on exit
			atexit(ReleaseBuff);
		}
	}
	// Return value
	return pReal;
}

/*! ************************************

	\brief Allocate a chunk of real mode memory.

	Memory is extremely limited in real mode, keep allocations
	to a minimum and in small sizes

	\note Only available on MSDOS
	
	\param uSize Number of bytes to allocate
	\return Segment:Offset of real mode memory or 0 on failure
	\sa DeallocRealMemory()
	
***************************************/

Word32 BURGER_API AllocRealMemory(Word32 uSize)
{
#if defined(BURGER_X32)
	// Call X32 to allocate memory
	return _x32_real_alloc(uSize);	
#else
	Burger::Regs16 Regs;
	uSize = uSize+15;		// Round to the nearest 16 bytes
	uSize = uSize>>4;		// Number of paragraphs to allocate
	Regs.ax = 0x4800;		// DOS allocate memory command
	Regs.bx = static_cast<Word16>(uSize);	// Number of paragraphs to allocate
	Int86x(0x21,&Regs,&Regs);		// Allocate
	if (Regs.flags&1) {		// Error??
#if defined(_DEBUG)
		Burger::Debug::Warning("Can't allocate real memory\n");
#endif
		return 0;
	}
	uSize = Regs.ax;	// Get the segment
	return uSize<<16;	// Return as a real mode pointer with offset 0
#endif
}

/*! ************************************

	\brief Release real mode memory back to DOS

	\note Only available on MSDOS
	
	\param pReal Segment:Offset of allocated memory. Zero performs no action
	\sa AllocRealMemory()
	
***************************************/

void BURGER_API DeallocRealMemory(Word32 pReal)
{
	if (pReal) {
#if defined(BURGER_X32)
		// Call X32 to allocate memory
		_x32_real_free(pReal);
#else
		Burger::Regs16 Regs;

		pReal = pReal>>16;			// Isolate the segment
		Regs.ax = 0x4900;			// DOS release memory command
		Regs.es = static_cast<Word16>(pReal);	// Get the segment
		Int86x(0x21,&Regs,&Regs);	// Release it
#if defined(_DEBUG)
		if (Regs.flags&1) {			// Always an error under X32
			Burger::Debug::Warning("Can't release real memory\n");
		}
#endif
#endif
	}
}

#endif
