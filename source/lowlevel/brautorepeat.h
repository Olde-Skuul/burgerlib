/***************************************

    Autorepeat manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRAUTOREPEAT_H__
#define __BRAUTOREPEAT_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

/* BEGIN */
namespace Burger {
class AutoRepeat {
private:
	enum {
		HELDDOWNBEFORE=1,	///< \ref TRUE if the button(s) were previously held down
		SECONDDELAY=2,		///< \ref TRUE if secondary delay is active
		WAITFORKEYUP=4,		///< \ref TRUE if pending a key up due to button down on launch
		INITIALIZED=8		///< \ref TRUE if initialized
	};
	uint32_t m_uButtons;		///< Bit field of buttons to test against
	uint32_t m_uInitialTick;	///< Delay for initial joydown in 1/60ths of a second
	uint32_t m_uRepeatTick;	///< Delay for repeater in 1/60ths of a second
	uint32_t m_uTimeMark;		///< Internal time mark
	uint_t m_uFlags;			///< Current state flags
public:
	AutoRepeat(uint32_t uButtons,uint32_t uInitialTick,uint32_t uRepeatTick);
	void Init(uint32_t uButtons,uint32_t uInitialTick,uint32_t uRepeatTick);
	BURGER_INLINE void Reset(void) { m_uFlags = 0; }
	uint_t IsItRepeating(uint32_t uButtons);
};
}
/* END */

#endif
