/***************************************

	MacOS Game Spockets code

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __MAC_SPROCKETS_H__
#define __MAC_SPROCKETS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger { namespace MacOS {
extern uint32_t BURGER_API get_input_sprocket_version(void) BURGER_NOEXCEPT;
extern uint32_t BURGER_API get_draw_sprocket_version(void) BURGER_NOEXCEPT;
extern uint32_t BURGER_API get_sound_manager_version(void) BURGER_NOEXCEPT;
extern uint_t BURGER_API start_draw_sprocket(void) BURGER_NOEXCEPT;
extern void BURGER_API stop_draw_sprocket(void) BURGER_NOEXCEPT;
}}

/* END */

#endif
