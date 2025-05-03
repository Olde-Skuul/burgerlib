/***************************************

	Instance of DBus

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtypes.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)
#include <dbus/dbus.h>

#include "brglobalmemorymanager.h"
#include "brstringfunctions.h"
#include "unix_dbusinstance.h"

#if defined(BURGER_LINUX)
#include "linux_sandbox.h"
#endif

/*! ************************************

	\class Burger::Unix::DBusInstance
	\brief Handler for a dbus session

	This class handles the loading of the dbus library (If available) and create
	and manage a connection to dbus.

***************************************/

/*! ************************************

	\brief Initialize internal values.

	Initialize variables to power up states.

	\sa init()

***************************************/

Burger::Unix::DBusInstance::DBusInstance() BURGER_NOEXCEPT
	: m_pSystem(nullptr),
	  m_pSession(nullptr),
	  m_pDisableObject(nullptr),
	  m_uScreenSaverToken(0),
	  m_bScreenSaverDisable(FALSE)
{
}

/*! ************************************

	\brief Release the connection

	Call shutdown() to ensure the dbus connection, if any, is properly released.

	\sa shutdown()

***************************************/

Burger::Unix::DBusInstance::~DBusInstance() BURGER_NOEXCEPT
{
	shutdown();
}

/*! ************************************

	\fn Burger::Unix::DBusInstance::get_system() const
	\brief Return the dbus system connection

	\returns \ref nullptr or a valid dbus connection pointer

	\sa get_session() const

***************************************/

/*! ************************************

	\fn Burger::Unix::DBusInstance::get_session() const
	\brief Return the dbus session connection

	\returns \ref nullptr or a valid dbus connection pointer

	\sa get_system() const

***************************************/

/*! ************************************

	\brief Start up a dbus connection

	Ensure that dbus is present on the platform and if so, initialize the
	sessions.

	\sa shutdown()

***************************************/

Burger::eError Burger::Unix::DBusInstance::init(void) BURGER_NOEXCEPT
{
	eError uResult = DBus::init();
	if (!uResult) {

		// Don't init twice
		if (!m_pSession) {

			// Assume error
			uResult = kErrorNotSupportedOnThisPlatform;

			// Start up dbus, continue on success
			if (Unix::dbus_threads_init_default()) {

				// Set up error object
				DBusError ErrorObject;
				Unix::dbus_error_init(&ErrorObject);

				// Create the session bus
				m_pSession =
					Unix::dbus_bus_get_private(DBUS_BUS_SESSION, &ErrorObject);
				if (!Unix::dbus_error_is_set(&ErrorObject)) {

					Unix::dbus_connection_set_exit_on_disconnect(
						m_pSession, FALSE);

					// If the code got here, it's considered successful
					uResult = kErrorNone;

					// Get the optional system bus
					m_pSystem = Unix::dbus_bus_get_private(
						DBUS_BUS_SYSTEM, &ErrorObject);
					if (!Unix::dbus_error_is_set(&ErrorObject)) {
						Unix::dbus_connection_set_exit_on_disconnect(
							m_pSystem, FALSE);
					}
				}

				Unix::dbus_error_free(&ErrorObject);
			}
		}
	}
	return uResult;
}

/*! ************************************

	\brief Release the connection

	If there was any connection, release it.

	\sa init()

***************************************/

void Burger::Unix::DBusInstance::shutdown(void) BURGER_NOEXCEPT
{

	// Release the system connection
	if (m_pSystem) {
		Unix::dbus_connection_close(m_pSystem);
		Unix::dbus_connection_unref(m_pSystem);
		m_pSystem = nullptr;
	}

	// Release the session connection
	if (m_pSession) {
		Unix::dbus_connection_close(m_pSession);
		Unix::dbus_connection_unref(m_pSession);
		m_pSession = nullptr;
	}

	// Release the screen saver disable object
	free_memory(m_pDisableObject);
	m_pDisableObject = nullptr;
}

/*! ************************************

	\brief Internal function to send and receive a message

	Send a message on dbus and get the answer. If the connection is \ref
	nullptr, do nothing and return \ref FALSE

	See
	https://dbus.freedesktop.org/doc/api/html/group__DBusMessage.html#ga98ddc82450d818138ef326a284201ee0

	\param pConnection Pointer to active connection
	\param pBusName Pointer to "C" string of the bus to talk to
	\param pPath Pointer to "C" string to path
	\param pInterface Pointer to "C" string to interface
	\param pMethod Pointer to "C" string of method to call
	\param pArgList Argument list to send to the method

	\return \ref TRUE if successful

***************************************/

uint_t Burger::Unix::DBusInstance::send_and_receive(DBusConnection* pConnection,
	const char* pBusName, const char* pPath, const char* pInterface,
	const char* pMethod, va_list pArgList) BURGER_NOEXCEPT
{
	uint_t bResult = FALSE;

	// Was there a connection?
	if (pConnection) {

		// Create a new method call
		DBusMessage* pMessage = Unix::dbus_message_new_method_call(
			pBusName, pPath, pInterface, pMethod);

		// Successful?
		if (pMessage) {

			// Make a copy with the arg list just in case dbus modifies it

			va_list TempArgs;
			va_copy(TempArgs, pArgList);

			// Pull down a dbus opcode
			int iDBusType = va_arg(pArgList, int);
			if ((iDBusType == DBUS_TYPE_INVALID) ||
				Unix::dbus_message_append_args_valist(
					pMessage, iDBusType, pArgList)) {

				// Send the message and wait for a reply
				DBusMessage* pAnswer =
					Unix::dbus_connection_send_with_reply_and_block(
						pConnection, pMessage, kTimeout, nullptr);

				// Got an answer?
				if (pAnswer) {

					// Look for the output arguments
					while ((iDBusType = va_arg(TempArgs, int)) !=
						DBUS_TYPE_INVALID) {
						// Skip the pointer for the argument
						va_arg(TempArgs, void*);
						if (iDBusType == DBUS_TYPE_ARRAY) {
							// Skip the int * for the array size
							va_arg(TempArgs, int*);
						}
					}

					// Is this the answer token?
					iDBusType = va_arg(TempArgs, int);

					// Process it if so
					if ((iDBusType == DBUS_TYPE_INVALID) ||
						Unix::dbus_message_get_args_valist(
							pAnswer, nullptr, iDBusType, TempArgs)) {

						// Message was sent and answer received
						bResult = TRUE;
					}

					// Release the answer
					Unix::dbus_message_unref(pAnswer);
				}
			}

			// Clear up the parameter list
			va_end(TempArgs);

			// Release the message container
			Unix::dbus_message_unref(pMessage);
		}
	}

	// Return the result
	return bResult;
}

/*! ************************************

	\brief Internal function to send and receive a message

	Send a message on dbus and get the answer. If the connection is \ref
	nullptr, do nothing and return \ref FALSE\

	\param pConnection Pointer to active connection
	\param pMessage DBusMessage to send
	\param iType Type of data to receive
	\param pOutput Pointer to buffer to receive the data

	\return \ref TRUE if successful

***************************************/

uint_t Burger::Unix::DBusInstance::send_and_receive(DBusConnection* pConnection,
	DBusMessage* pMessage, const int iType, void* pOutput) BURGER_NOEXCEPT
{
	uint_t bResult = FALSE;

	// Set the message and wait for the reply
	DBusMessage* pAnswer = Unix::dbus_connection_send_with_reply_and_block(
		pConnection, pMessage, kTimeout, nullptr);
	if (pAnswer) {

		// Initialize
		DBusMessageIter pIterator;
		Unix::dbus_message_iter_init(pAnswer, &pIterator);

		// Search, or use the root
		DBusMessageIter pFinalIterator;
		if (Unix::dbus_message_iter_get_arg_type(&pIterator) !=
			DBUS_TYPE_VARIANT) {
			// Use the root
			pFinalIterator = pIterator;
		} else {
			// Use this as the iterator
			Unix::dbus_message_iter_recurse(&pIterator, &pFinalIterator);
		}

		// Match? If so, get the data
		if (Unix::dbus_message_iter_get_arg_type(&pFinalIterator) == iType) {
			Unix::dbus_message_iter_get_basic(&pFinalIterator, pOutput);
			bResult = TRUE;
		}

		// Release the answer
		Unix::dbus_message_unref(pAnswer);
	}

	// Return the result
	return bResult;
}

/*! ************************************

	\brief Internal function to send a message

	Send a message on dbus. If the connection is \ref nullptr, do nothing and
	return \ref FALSE

	See
	https://dbus.freedesktop.org/doc/api/html/group__DBusMessage.html#ga98ddc82450d818138ef326a284201ee0

	\param pConnection Pointer to active connection
	\param pBusName Pointer to "C" string of the bus to talk to
	\param pPath Pointer to "C" string to path
	\param pInterface Pointer to "C" string to interface
	\param pMethod Pointer to "C" string of method to call
	\param pArgList Argument list to send to the method

	\return \ref TRUE if successful

***************************************/

uint_t Burger::Unix::DBusInstance::send_message(DBusConnection* pConnection,
	const char* pBusName, const char* pPath, const char* pInterface,
	const char* pMethod, va_list pArgList) BURGER_NOEXCEPT
{
	uint_t bResult = FALSE;

	// Was there a connection?
	if (pConnection) {

		// Create a new method call
		DBusMessage* pMessage = Unix::dbus_message_new_method_call(
			pBusName, pPath, pInterface, pMethod);

		// Successful?
		if (pMessage) {

			// Pull down a dbus opcode
			int iDBusType = va_arg(pArgList, int);
			if ((iDBusType == DBUS_TYPE_INVALID) ||
				Unix::dbus_message_append_args_valist(
					pMessage, iDBusType, pArgList)) {

				// Send the message flush it
				if (Unix::dbus_connection_send(
						pConnection, pMessage, nullptr)) {
					Unix::dbus_connection_flush(pConnection);
					bResult = TRUE;
				}
			}

			// Release the message container
			Unix::dbus_message_unref(pMessage);
		}
	}

	// Return the result
	return bResult;
}

/*! ************************************

	\brief Send and receive a message using a dbug connection

	Send a message on dbus and get the answer. If the connection is \ref
	nullptr, do nothing and return \ref FALSE

	See
	https://dbus.freedesktop.org/doc/api/html/group__DBusMessage.html#ga98ddc82450d818138ef326a284201ee0

	\param pConnection Pointer to active connection
	\param pBusName Pointer to "C" string of the bus to talk to
	\param pPath Pointer to "C" string to path
	\param pInterface Pointer to "C" string to interface
	\param pMethod Pointer to "C" string of method to call
	\param ... Argument list to send to the method

	\return \ref TRUE if successful

	\sa send_and_receive(DBusConnection*, const char*, const char*, const char*,
		const char*, va_list)

***************************************/

uint_t Burger::Unix::DBusInstance::send_and_receive(DBusConnection* pConnection,
	const char* pBusName, const char* pPath, const char* pInterface,
	const char* pMethod, ...) BURGER_NOEXCEPT
{
	// Convert parameters to a list
	va_list TempArgs;
	va_start(TempArgs, pMethod);

	// Do the magic
	const uint_t bResult = send_and_receive(
		pConnection, pBusName, pPath, pInterface, pMethod, TempArgs);

	// Cleanup
	va_end(TempArgs);
	return bResult;
}

/*! ************************************

	\brief Send and receive a message using a dbug connection

	Send a message on dbus and get the answer from the default session.

	See
	https://dbus.freedesktop.org/doc/api/html/group__DBusMessage.html#ga98ddc82450d818138ef326a284201ee0

	\param pBusName Pointer to "C" string of the bus to talk to
	\param pPath Pointer to "C" string to path
	\param pInterface Pointer to "C" string to interface
	\param pMethod Pointer to "C" string of method to call
	\param ... Argument list to send to the method

	\return \ref TRUE if successful

	\sa send_and_receive(DBusConnection*, const char*, const char*, const char*,
		const char*, va_list)

***************************************/

uint_t Burger::Unix::DBusInstance::send_and_receive(const char* pBusName,
	const char* pPath, const char* pInterface, const char* pMethod,
	...) BURGER_NOEXCEPT
{
	// Convert parameters to a list
	va_list TempArgs;
	va_start(TempArgs, pMethod);

	// Do the magic
	const uint_t bResult = send_and_receive(
		m_pSession, pBusName, pPath, pInterface, pMethod, TempArgs);

	// Cleanup
	va_end(TempArgs);
	return bResult;
}

/*! ************************************

	\brief Send a message using a dbug connection

	Send a message on dbus. If the connection is \ref nullptr, do nothing and
	return \ref FALSE

	See
	https://dbus.freedesktop.org/doc/api/html/group__DBusMessage.html#ga98ddc82450d818138ef326a284201ee0

	\param pConnection Pointer to active connection
	\param pBusName Pointer to "C" string of the bus to talk to
	\param pPath Pointer to "C" string to path
	\param pInterface Pointer to "C" string to interface
	\param pMethod Pointer to "C" string of method to call
	\param ... Argument list to send to the method

	\return \ref TRUE if successful

	\sa send_message(DBusConnection*, const char*, const char*, const char*,
		const char*, va_list)

***************************************/

uint_t Burger::Unix::DBusInstance::send_message(DBusConnection* pConnection,
	const char* pBusName, const char* pPath, const char* pInterface,
	const char* pMethod, ...) BURGER_NOEXCEPT
{
	// Convert parameters to a list
	va_list TempArgs;
	va_start(TempArgs, pMethod);

	// Do the magic
	const uint_t bResult = send_message(
		pConnection, pBusName, pPath, pInterface, pMethod, TempArgs);

	// Cleanup
	va_end(TempArgs);
	return bResult;
}

/*! ************************************

	\brief Send a message using a dbug connection

	Send a message on dbus on the default session.

	See
	https://dbus.freedesktop.org/doc/api/html/group__DBusMessage.html#ga98ddc82450d818138ef326a284201ee0

	\param pBusName Pointer to "C" string of the bus to talk to
	\param pPath Pointer to "C" string to path
	\param pInterface Pointer to "C" string to interface
	\param pMethod Pointer to "C" string of method to call
	\param ... Argument list to send to the method

	\return \ref TRUE if successful

	\sa send_message(DBusConnection*, const char*, const char*, const char*,
		const char*, va_list)

***************************************/

uint_t Burger::Unix::DBusInstance::send_message(const char* pBusName,
	const char* pPath, const char* pInterface, const char* pMethod,
	...) BURGER_NOEXCEPT
{
	// Convert parameters to a list
	va_list TempArgs;
	va_start(TempArgs, pMethod);

	// Do the magic
	const uint_t bResult = send_message(
		m_pSession, pBusName, pPath, pInterface, pMethod, TempArgs);

	// Cleanup
	va_end(TempArgs);
	return bResult;
}

/*! ************************************

	\brief Get a property object from "org.freedesktop.DBus.Properties"

	Using dbus, query "org.freedesktop.DBus.Properties" for the requested
	object.

	\param pConnection Pointer to active connection
	\param pBusName Pointer to "C" string of the bus to talk to
	\param pPath Pointer to "C" string to path
	\param pInterface Pointer to "C" string to interface
	\param pObject Pointer to "C" string of object to query
	\param iType Type of object to obtain
	\param pOutput Pointer to a buffer to get a copy of the object

	\return \ref TRUE if successful

***************************************/

#if defined(BURGER_LINUX) || defined(DOXYGEN)
uint_t Burger::Unix::DBusInstance::query_object(DBusConnection* pConnection,
	const char* pBusName, const char* pPath, const char* pInterface,
	const char* pObject, const int iType, void* pOutput) BURGER_NOEXCEPT
{
	uint_t bResult = FALSE;

	// Was there a connection?
	if (pConnection) {

		// Create a new method call for freedesktop
		DBusMessage* pMessage = Unix::dbus_message_new_method_call(
			pBusName, pPath, "org.freedesktop.DBus.Properties", "Get");

		// Successful?
		if (pMessage) {

			// Create a response of the interface and object requested
			if (Unix::dbus_message_append_args(pMessage, DBUS_TYPE_STRING,
					&pInterface, DBUS_TYPE_STRING, &pObject,
					DBUS_TYPE_INVALID)) {

				// Send the message and wait for a reply
				DBusMessage* pAnswer =
					Unix::dbus_connection_send_with_reply_and_block(
						pConnection, pMessage, kTimeout, nullptr);

				// Got an answer?
				if (pAnswer) {

					// Let's iterate over the items in the properties
					DBusMessageIter pIterator;
					Unix::dbus_message_iter_init(pAnswer, &pIterator);

					// The first item must be VARIANT
					if (Unix::dbus_message_iter_get_arg_type(&pIterator) ==
						DBUS_TYPE_VARIANT) {

						DBusMessageIter pIterator2;
						Unix::dbus_message_iter_recurse(
							&pIterator, &pIterator2);

						// Match?
						if (Unix::dbus_message_iter_get_arg_type(&pIterator2) ==
							iType) {

							// Get the data
							Unix::dbus_message_iter_get_basic(
								&pIterator2, pOutput);

							// Success!
							bResult = TRUE;
						}
					}
					Unix::dbus_message_unref(pAnswer);
				}
			}

			// Release the message container
			Unix::dbus_message_unref(pMessage);
		}
	}

	return bResult;
}

/*! ************************************

	\brief Get a property object from "org.freedesktop.DBus.Properties"

	Using dbus via the default session, query "org.freedesktop.DBus.Properties"
	for the requested object.

	\param pBusName Pointer to "C" string of the bus to talk to
	\param pPath Pointer to "C" string to path
	\param pInterface Pointer to "C" string to interface
	\param pObject Pointer to "C" string of object to query
	\param iType Type of object to obtain
	\param pOutput Pointer to a buffer to get a copy of the object

	\return \ref TRUE if successful

***************************************/

uint_t Burger::Unix::DBusInstance::query_object(const char* pBusName,
	const char* pPath, const char* pInterface, const char* pObject,
	const int iType, void* pOutput) BURGER_NOEXCEPT
{
	return query_object(
		m_pSession, pBusName, pPath, pInterface, pObject, iType, pOutput);
}

/*! ************************************

	\brief Send a message to screen savers to keep them disabled

	If the feature is not turned off, send a dbus message to gnome and
	freedesktop to let the know that the screen saver should not activate

***************************************/

void Burger::Unix::DBusInstance::x11_screen_saver_poll(void) BURGER_NOEXCEPT
{
	// Is the screen saver to be disabled?
	if (!m_uScreenSaverToken && !m_pDisableObject) {

		// Poll the gnome screen saver system (Old)
		send_message("org.gnome.ScreenSaver", "/org/gnome/ScreenSaver",
			"org.gnome.ScreenSaver", "SimulateUserActivity", DBUS_TYPE_INVALID);

		// Poll the freedesktop screen saver
		send_message("org.freedesktop.ScreenSaver",
			"/org/freedesktop/ScreenSaver", "org.freedesktop.ScreenSaver",
			"SimulateUserActivity", DBUS_TYPE_INVALID);
	}
}

/*! ************************************

	\brief Update a dictionary with a value

	Given an iterator for a DBusMessage, add a data string attached to a key.

	\param pRoot Dictionary root pointer
	\param pKey Pointer to "C" string for the database key
	\param pInput Pointer to "C" string for the data to attach to the key

	\return \ref TRUE on success, \ref FALSE on failure

***************************************/

uint_t Burger::Unix::DBusInstance::add_key_value(DBusMessageIter* pRoot,
	const char* pKey, const char* pInput) BURGER_NOEXCEPT
{
	// Assume failure
	uint_t bResult = FALSE;

	// Open the dictionary container
	DBusMessageIter pDictionary;
	if (Unix::dbus_message_iter_open_container(
			pRoot, DBUS_TYPE_ARRAY, "{sv}", &pDictionary)) {

		// Get an entry iterator for the dictionary
		DBusMessageIter pDictEntry;
		if (Unix::dbus_message_iter_open_container(
				&pDictionary, DBUS_TYPE_DICT_ENTRY, nullptr, &pDictEntry)) {

			// Look up using the key
			if (Unix::dbus_message_iter_append_basic(
					&pDictEntry, DBUS_TYPE_STRING, &pKey)) {

				DBusMessageIter pVariant;
				if (Unix::dbus_message_iter_open_container(&pDictEntry,
						DBUS_TYPE_VARIANT, DBUS_TYPE_STRING_AS_STRING,
						&pVariant)) {

					// Finally! Append the data to the record
					if (Unix::dbus_message_iter_append_basic(
							&pVariant, DBUS_TYPE_STRING, &pInput)) {

						// Success!
						bResult = TRUE;
					}

					// Release the container
					Unix::dbus_message_iter_close_container(
						&pDictEntry, &pVariant);
				}
			}

			Unix::dbus_message_iter_close_container(&pDictionary, &pDictEntry);
		}
		Unix::dbus_message_iter_close_container(pRoot, &pDictionary);
	}
	return bResult;
}

/*! ************************************

	\brief Enable/disable the screen saver

	On linux systems, the ``freedesktop`` window system supports messages that
	will enable and disable the activation of the screen saver. This function
	will enable Burgerlib to turn on or off this feature.

	\linuxonly

	\param bDisable \ref TRUE to disable the screen saver

	\return \ref TRUE on success, \ref FALSE on failure

***************************************/

uint_t Burger::Unix::DBusInstance::x11_screen_saver_disable(
	uint_t bDisable) BURGER_NOEXCEPT
{
	// Reason to display why the screen saver is disabled
	static const char* s_pReason = "Game in progress";
	static const char* s_pWindowTitle = "";

	// No change?
	if ((bDisable && (m_uScreenSaverToken || m_pDisableObject)) ||
		(!bDisable && (!m_uScreenSaverToken && !m_pDisableObject))) {
		return TRUE;
	}

	// Is DBus active?
	if (!m_pSession) {
		return FALSE;
	}

	// I love standards. There are so many to choose from

	// Assume failure
	uint_t bResult = FALSE;
	// Is the app running in a sandbox?
	if (Linux::detect_x11_sandbox() != Linux::kX11SandboxNone) {

		// Sandbox means that the application is running on the freedesktop
		// This code will handle the screen saver for it.

		static const char* s_pFreeDesktopDomain =
			"org.freedesktop.portal.Desktop";

		if (bDisable) {

			// Connect to the inhibit flag
			DBusMessage* pAnswer = Unix::dbus_message_new_method_call(
				s_pFreeDesktopDomain, "/org/freedesktop/portal/desktop",
				"org.freedesktop.portal.Inhibit", "Inhibit");

			// Found it?
			if (pAnswer) {

				// Set to idle mode
				const uint_t uIdle = 8;
				if (Unix::dbus_message_append_args(pAnswer, DBUS_TYPE_STRING,
						&s_pWindowTitle, DBUS_TYPE_UINT32, &uIdle,
						DBUS_TYPE_INVALID)) {

					// Set the reason
					DBusMessageIter pMessageIterator;
					Unix::dbus_message_iter_init_append(
						pAnswer, &pMessageIterator);

					if (add_key_value(&pMessageIterator, "reason", s_pReason)) {

						// Get the disable object data
						const char* pReply = nullptr;
						if (send_and_receive(m_pSession, pAnswer,
								DBUS_TYPE_OBJECT_PATH, &pReply)) {
							m_pDisableObject = StringDuplicate(pReply);
							bResult = TRUE;
						}
					}
				}
				Unix::dbus_message_unref(pAnswer);
			}

		} else {

			// Close out the screen saver disable object
			if (send_message(s_pFreeDesktopDomain, m_pDisableObject,
					"org.freedesktop.portal.Request", "Close",
					DBUS_TYPE_INVALID)) {

				// No longer need this string
				free_memory(m_pDisableObject);
				m_pDisableObject = nullptr;
				bResult = TRUE;
			}
		}
	} else {

		// No sandbox? Just talk to the sandbox
		static const char* s_pSaverDomain = "org.freedesktop.ScreenSaver";
		static const char* s_pSaverPath = "/org/freedesktop/ScreenSaver";
		static const char* s_pSaverInterface = "org.freedesktop.ScreenSaver";
		static const char* s_pAppName = "Burgerlib Application";
		if (bDisable) {

			// Get the token
			if (send_and_receive(s_pSaverDomain, s_pSaverPath,
					s_pSaverInterface, "Inhibit", DBUS_TYPE_STRING, &s_pAppName,
					DBUS_TYPE_STRING, &s_pReason, DBUS_TYPE_INVALID,
					DBUS_TYPE_UINT32, &m_uScreenSaverToken,
					DBUS_TYPE_INVALID)) {
				// If non-zero, success
				bResult = m_uScreenSaverToken != 0;
			}

		} else {
			// Release the saver
			if (send_message(s_pSaverDomain, s_pSaverPath, s_pSaverInterface,
					"UnInhibit", DBUS_TYPE_UINT32, &m_uScreenSaverToken,
					DBUS_TYPE_INVALID)) {
				m_uScreenSaverToken = 0;
				bResult = TRUE;
			}
		}
	}
	return bResult;
}

#endif

#endif
