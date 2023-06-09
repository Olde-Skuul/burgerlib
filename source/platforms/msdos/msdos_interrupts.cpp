/***************************************

	MSDos interrupt functions

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "msdos_interrupts.h"

// Documentation to assembly functions

/*! ************************************

	\fn Burger::MSDos::get_interrupt_protected(uint32_t uInterrupt)

	\brief Call the dos extender to get the current protected mode IRQ vector

	Call the dos extender to retrieve the protected mode IRQ vector

	\msdosonly

	\param uInterrupt Interrupt to intercept
	\return Pointer to the function that is called when the interrupt is
		tripped

	\sa set_interrupt_both(), set_interrupt_protected(),
		set_interrupt_real() and get_interrupt_real()

***************************************/

/*! ************************************

	\fn Burger::MSDos::set_interrupt_protected(
		uint32_t uInterrupt,void far *pCode)

	\brief Intercept the protected IRQ vector

	Call the dos extender to intercept the protected IRQ vector
	and route it through a protected mode IRQ

	\msdosonly

	\param uInterrupt Interrupt to intercept
	\param pCode Pointer to the function to call when the interrupt is tripped

	\sa set_interrupt_both(), set_interrupt_real(),
		get_interrupt_protected() and get_interrupt_real()

***************************************/

/*! ************************************

	\fn Burger::MSDos::get_interrupt_real(uint32_t uInterrupt)

	\brief Call the dos extender to get the current real mode IRQ vector

	Call the dos extender to retrieve the real mode IRQ vector

	\msdosonly

	\param uInterrupt Interrupt to intercept
	\returns Real mode (SEGMENT:OFFSET) pointer to the function that is called
		when the interrupt is tripped

	\sa set_interrupt_both(), set_interrupt_protected(),
		set_interrupt_real() and get_interrupt_protected()

***************************************/

/*! ************************************

	\fn Burger::MSDos::set_interrupt_real(uint32_t uInterrupt, uint32_t pCode)

	\brief Intercept the real mode IRQ vector

	Call the dos extender to intercept the real mode IRQ vector
	and route it through a real mode IRQ

	\msdosonly

	\param uInterrupt Interrupt to intercept
	\param pCode Pointer to the function to call when the interrupt is tripped

	\sa set_interrupt_both(), set_interrupt_protected(),
		get_interrupt_protected() and get_interrupt_real()

***************************************/

/*! ************************************

	\fn Burger::MSDos::set_interrupt_both(uint32_t uInterrupt,void far *pCode)

	\brief Intercept both the real the protected IRQ vectors

	Call the dos extender to intercept both the real and protected IRQ vector
	and route it through a protected mode IRQ

	\msdosonly

	\param uInterrupt Interrupt to intercept
	\param pCode Pointer to the function to call when the interrupt is tripped

	\sa set_interrupt_protected(), set_interrupt_real(),
		get_interrupt_protected() and get_interrupt_real()

***************************************/

/*! ************************************

	\fn Burger::MSDos::map_physical_address(void *pInput, uint32_t uLength)

	\brief Map memory from a device into protected mode

	Call the dos extender to remap extended memory to protected mode

	\msdosonly

	\param pInput Pointer to extended memory
	\param uLength Length in bytes of the extended memory chunk
	\returns Protected mode pointer to the extended memory

	\sa real_to_protected()

***************************************/
