/***************************************

	Typedefs specific to Unix/Posix

	Copyright (c) 2021-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRUNIXTYPES_H__
#define __BRUNIXTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

/* BEGIN */
#if defined(BURGER_UNIX) || defined(DOXYGEN)
namespace Burger {

class Uname {
public:
	/** Operating system name */
	String m_sysnam;
	/** Name within network */
	String m_nodename;
	/** OS release string */
	String m_release;
	/** OS version string */
	String m_version;
	/** Hardware identifier */
	String m_machine;
	/** (Optional) NIS or YP domain name */
	String m_domainname;

	eError Init(void) BURGER_NOEXCEPT;
};

enum eUnixFlavor {
	/** Unknown flavor of Linux */
	kUnixUnknown,
	/** Native Linux distribution */
	kUnixNative,
	/** Linux running under MSYS2 */
	kUnixMSYS2,
	/** Linux running under Windows Subsystem for Linux */
	kUnixWSL,
	/** Linux running under Cygwin */
	kUnixCygwin,
	/** Unix running as MacOSX */
	kUnixMacOSX,
	/** Unix running as iOS */
	kUnixiOS,
	/** Linux running under Android */
	kUnixAndroid,
	/** Linux running under Google Stadia */
	kUnixStadia
};

extern const char* g_VolumePrefixes[kUnixStadia + 1];

eUnixFlavor BURGER_API UnixGetFlavor(void) BURGER_NOEXCEPT;

}
#endif
/* END */

#endif
