/***************************************

	Instance of DBus

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __UNIX_DBUSINSTANCE_H__
#define __UNIX_DBUSINSTANCE_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __UNIX_DBUS_H__
#include "unix_dbus.h"
#endif

/* BEGIN */

namespace Burger { namespace Unix {
class DBusInstance {
	BURGER_DISABLE_COPY(DBusInstance);

public:
	/** Message timeout in milliseconds */
	static const int kTimeout = 333;

protected:
	/** DBus system connection */
	DBusConnection* m_pSystem;
	/** DBus session connection */
	DBusConnection* m_pSession;

	/** String for the screen saver disable */
	char* m_pDisableObject;
	/** Screen saver token from X11 */
	uint32_t m_uScreenSaverToken;
	/** Set to true to disable the screen saver */
	uint_t m_bScreenSaverDisable;

public:
	DBusInstance() BURGER_NOEXCEPT;
	~DBusInstance() BURGER_NOEXCEPT;
	BURGER_INLINE DBusConnection* get_system(void) const BURGER_NOEXCEPT
	{
		return m_pSystem;
	}

	BURGER_INLINE DBusConnection* get_session(void) const BURGER_NOEXCEPT
	{
		return m_pSession;
	}

	eError init(void) BURGER_NOEXCEPT;
	void shutdown(void) BURGER_NOEXCEPT;

protected:
	static uint_t send_and_receive(DBusConnection* pConnection,
		const char* pBusName, const char* pPath, const char* pInterface,
		const char* pMethod, va_list pArgList) BURGER_NOEXCEPT;
	static uint_t send_message(DBusConnection* pConnection,
		const char* pBusName, const char* pPath, const char* pInterface,
		const char* pMethod, va_list pArgList) BURGER_NOEXCEPT;
	static uint_t send_and_receive(DBusConnection* pConnection,
		DBusMessage* pMessage, const int iType, void* pOutput) BURGER_NOEXCEPT;
	static uint_t add_key_value(DBusMessageIter* iterInit, const char* key,
		const char* value) BURGER_NOEXCEPT;

public:
	static uint_t send_and_receive(DBusConnection* pConnection,
		const char* pBusName, const char* pPath, const char* pInterface,
		const char* pMethod, ...) BURGER_NOEXCEPT;
	uint_t send_and_receive(const char* pBusName, const char* pPath,
		const char* pInterface, const char* pMethod, ...) BURGER_NOEXCEPT;
	static uint_t send_message(DBusConnection* pConnection,
		const char* pBusName, const char* pPath, const char* pInterface,
		const char* pMethod, ...) BURGER_NOEXCEPT;
	uint_t send_message(const char* pBusName, const char* pPath,
		const char* pInterface, const char* pMethod, ...) BURGER_NOEXCEPT;

#if defined(BURGER_LINUX) || defined(DOXYGEN)
	static uint_t query_object(DBusConnection* pConnection,
		const char* pBusName, const char* pPath, const char* pInterface,
		const char* pObject, const int iType, void* pOutput) BURGER_NOEXCEPT;
	uint_t query_object(const char* pBusName, const char* pPath,
		const char* pInterface, const char* pObject, const int iType,
		void* pOutput) BURGER_NOEXCEPT;

	void x11_screen_saver_poll(void) BURGER_NOEXCEPT;
	uint_t x11_screen_saver_disable(uint_t bDisable) BURGER_NOEXCEPT;
#endif
};
}}
/* END */

#endif
