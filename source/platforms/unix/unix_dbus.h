/***************************************

	Shims for the dbus library

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __UNIX_DBUS_H__
#define __UNIX_DBUS_H__

// Needed for va_list
#include <stdarg.h>

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRERROR_H__
#include "brerror.h"
#endif

#ifndef __BRCODELIBRARY_H__
#include "brcodelibrary.h"
#endif

/* BEGIN */
#if !defined(DOXYGEN)

// Don't declare if <dbus/dbus.h> has already been included
#if !defined(DBUS_H)

// Defines need by the DBus API
typedef enum {
	DBUS_HANDLER_RESULT_HANDLED,
	DBUS_HANDLER_RESULT_NOT_YET_HANDLED,
	DBUS_HANDLER_RESULT_NEED_MEMORY
} DBusHandlerResult;

typedef enum {
	DBUS_BUS_SESSION,
	DBUS_BUS_SYSTEM,
	DBUS_BUS_STARTER
} DBusBusType;

struct DBusError;
struct DBusAddressEntry;
struct DBusMessage;
struct DBusMessageIter;
struct DBusConnection;
struct DBusDispatchStatus;
struct DBusPendingCall;
struct DBusWatch;
struct DBusTimeout;
struct DBusPreallocatedSend;
struct DBusObjectPathVTable;
struct DBusServer;
struct DBusSignatureIter;
struct DBusThreadFunctions;
typedef void (*DBusFreeFunction)(void* memory);
typedef uint_t (*DBusAddWatchFunction)(DBusWatch* watch, void* data);
typedef void (*DBusRemoveWatchFunction)(DBusWatch* watch, void* data);
typedef void (*DBusWatchToggledFunction)(DBusWatch* watch, void* data);
typedef uint_t (*DBusAddTimeoutFunction)(DBusTimeout* timeout, void* data);
typedef void (*DBusRemoveTimeoutFunction)(DBusTimeout* timeout, void* data);
typedef void (*DBusTimeoutToggledFunction)(DBusTimeout* timeout, void* data);
typedef void (*DBusWakeupMainFunction)(void* data);
typedef void (*DBusDispatchStatusFunction)(
	DBusConnection* connection, DBusDispatchStatus new_status, void* data);
typedef uint_t (*DBusAllowUnixUserFunction)(
	DBusConnection* connection, unsigned long uid, void* data);
typedef uint_t (*DBusAllowWindowsUserFunction)(
	DBusConnection* connection, const char* user_sid, void* data);
typedef DBusHandlerResult (*DBusHandleMessageFunction)(
	DBusConnection* connection, DBusMessage* message, void* user_data);
typedef void (*DBusPendingCallNotifyFunction)(
	DBusPendingCall* pending, void* user_data);
typedef void (*DBusNewConnectionFunction)(
	DBusServer* server, DBusConnection* new_connection, void* data);
#endif
#endif

namespace Burger { namespace Unix {
extern uint_t BURGER_API has_dbus(void) BURGER_NOEXCEPT;

namespace DBus {
extern eError BURGER_API init(void) BURGER_NOEXCEPT;
extern void BURGER_API shutdown(void) BURGER_NOEXCEPT;
}

#if !defined(DOXYGEN)
extern void (*dbus_error_init)(DBusError* error);
extern void (*dbus_error_free)(DBusError* error);
extern void (*dbus_set_error)(
	DBusError* error, const char* name, const char* message, ...);
extern void (*dbus_set_error_const)(
	DBusError* error, const char* name, const char* message);
extern void (*dbus_move_error)(DBusError* src, DBusError* dest);
extern uint_t (*dbus_error_has_name)(const DBusError* error, const char* name);
extern uint_t (*dbus_error_is_set)(const DBusError* error);

extern uint_t (*dbus_parse_address)(const char* address,
	DBusAddressEntry*** entry_result, int* array_len, DBusError* error);
extern const char* (*dbus_address_entry_get_value)(
	DBusAddressEntry* entry, const char* key);
extern const char* (*dbus_address_entry_get_method)(DBusAddressEntry* entry);
extern void (*dbus_address_entries_free)(DBusAddressEntry** entries);
extern char* (*dbus_address_escape_value)(const char* value);
extern char* (*dbus_address_unescape_value)(
	const char* value, DBusError* error);

extern void* (*dbus_malloc)(uintptr_t bytes);
extern void* (*dbus_malloc0)(uintptr_t bytes);
extern void* (*dbus_realloc)(void* memory, uintptr_t bytes);
extern void (*dbus_free)(void* memory);
extern void (*dbus_free_string_array)(char** str_array);
extern void (*dbus_shutdown)(void);

extern DBusMessage* (*dbus_message_new)(int message_type);
extern DBusMessage* (*dbus_message_new_method_call)(const char* bus_name,
	const char* path, const char* iface, const char* method);
extern DBusMessage* (*dbus_message_new_method_return)(DBusMessage* method_call);
extern DBusMessage* (*dbus_message_new_signal)(
	const char* path, const char* iface, const char* name);
extern DBusMessage* (*dbus_message_new_error)(
	DBusMessage* reply_to, const char* error_name, const char* error_message);
extern DBusMessage* (*dbus_message_new_error_printf)(DBusMessage* reply_to,
	const char* error_name, const char* error_format, ...);
extern DBusMessage* (*dbus_message_copy)(const DBusMessage* message);
extern DBusMessage* (*dbus_message_ref)(DBusMessage* message);
extern void (*dbus_message_unref)(DBusMessage* message);
extern int (*dbus_message_get_type)(DBusMessage* message);
extern uint_t (*dbus_message_set_path)(
	DBusMessage* message, const char* object_path);
extern const char* (*dbus_message_get_path)(DBusMessage* message);
extern uint_t (*dbus_message_has_path)(
	DBusMessage* message, const char* object_path);
extern uint_t (*dbus_message_set_interface)(
	DBusMessage* message, const char* iface);
extern const char* (*dbus_message_get_interface)(DBusMessage* message);
extern uint_t (*dbus_message_has_interface)(
	DBusMessage* message, const char* iface);
extern uint_t (*dbus_message_set_member)(
	DBusMessage* message, const char* member);
extern const char* (*dbus_message_get_member)(DBusMessage* message);
extern uint_t (*dbus_message_has_member)(
	DBusMessage* message, const char* member);
extern uint_t (*dbus_message_set_error_name)(
	DBusMessage* message, const char* name);
extern const char* (*dbus_message_get_error_name)(DBusMessage* message);
extern uint_t (*dbus_message_set_destination)(
	DBusMessage* message, const char* destination);
extern const char* (*dbus_message_get_destination)(DBusMessage* message);
extern uint_t (*dbus_message_set_sender)(
	DBusMessage* message, const char* sender);
extern const char* (*dbus_message_get_sender)(DBusMessage* message);
extern const char* (*dbus_message_get_signature)(DBusMessage* message);
extern void (*dbus_message_set_no_reply)(DBusMessage* message, uint_t no_reply);
extern uint_t (*dbus_message_get_no_reply)(DBusMessage* message);
extern uint_t (*dbus_message_is_method_call)(
	DBusMessage* message, const char* iface, const char* method);
extern uint_t (*dbus_message_is_signal)(
	DBusMessage* message, const char* iface, const char* signal_name);
extern uint_t (*dbus_message_is_error)(
	DBusMessage* message, const char* error_name);
extern uint_t (*dbus_message_has_destination)(
	DBusMessage* message, const char* bus_name);
extern uint_t (*dbus_message_has_sender)(
	DBusMessage* message, const char* unique_bus_name);
extern uint_t (*dbus_message_has_signature)(
	DBusMessage* message, const char* signature);
extern uint_t (*dbus_message_get_serial)(DBusMessage* message);
extern void (*dbus_message_set_serial)(DBusMessage* message, uint_t serial);
extern uint_t (*dbus_message_set_reply_serial)(
	DBusMessage* message, uint_t reply_serial);
extern uint_t (*dbus_message_get_reply_serial)(DBusMessage* message);
extern void (*dbus_message_set_auto_start)(
	DBusMessage* message, uint_t auto_start);
extern uint_t (*dbus_message_get_auto_start)(DBusMessage* message);
extern uint_t (*dbus_message_get_path_decomposed)(
	DBusMessage* message, char*** path);
extern uint_t (*dbus_message_append_args)(
	DBusMessage* message, int first_arg_type, ...);
extern uint_t (*dbus_message_append_args_valist)(
	DBusMessage* message, int first_arg_type, va_list var_args);
extern uint_t (*dbus_message_get_args)(
	DBusMessage* message, DBusError* error, int first_arg_type, ...);
extern uint_t (*dbus_message_get_args_valist)(DBusMessage* message,
	DBusError* error, int first_arg_type, va_list var_args);
extern uint_t (*dbus_message_contains_unix_fds)(DBusMessage* message);
extern void (*dbus_message_iter_init_closed)(DBusMessageIter* iter);
extern uint_t (*dbus_message_iter_init)(
	DBusMessage* message, DBusMessageIter* iter);
extern uint_t (*dbus_message_iter_has_next)(DBusMessageIter* iter);
extern uint_t (*dbus_message_iter_next)(DBusMessageIter* iter);
extern char* (*dbus_message_iter_get_signature)(DBusMessageIter* iter);
extern int (*dbus_message_iter_get_arg_type)(DBusMessageIter* iter);
extern int (*dbus_message_iter_get_element_type)(DBusMessageIter* iter);
extern void (*dbus_message_iter_recurse)(
	DBusMessageIter* iter, DBusMessageIter* sub);
extern void (*dbus_message_iter_get_basic)(DBusMessageIter* iter, void* value);
extern int (*dbus_message_iter_get_element_count)(DBusMessageIter* iter);
extern int (*dbus_message_iter_get_array_len)(DBusMessageIter* iter);
extern void (*dbus_message_iter_get_fixed_array)(
	DBusMessageIter* iter, void* value, int* n_elements);
extern void (*dbus_message_iter_init_append)(
	DBusMessage* message, DBusMessageIter* iter);
extern uint_t (*dbus_message_iter_append_basic)(
	DBusMessageIter* iter, int type, const void* value);
extern uint_t (*dbus_message_iter_append_fixed_array)(
	DBusMessageIter* iter, int element_type, const void* value, int n_elements);
extern uint_t (*dbus_message_iter_open_container)(DBusMessageIter* iter,
	int type, const char* contained_signature, DBusMessageIter* sub);
extern uint_t (*dbus_message_iter_close_container)(
	DBusMessageIter* iter, DBusMessageIter* sub);
extern void (*dbus_message_iter_abandon_container)(
	DBusMessageIter* iter, DBusMessageIter* sub);
extern void (*dbus_message_iter_abandon_container_if_open)(
	DBusMessageIter* iter, DBusMessageIter* sub);
extern void (*dbus_message_lock)(DBusMessage* message);
extern uint_t (*dbus_set_error_from_message)(
	DBusError* error, DBusMessage* message);
extern uint_t (*dbus_message_allocate_data_slot)(int_t* slot_p);
extern void (*dbus_message_free_data_slot)(int_t* slot_p);
extern uint_t (*dbus_message_set_data)(DBusMessage* message, int_t slot,
	void* data, DBusFreeFunction free_data_func);
extern void* (*dbus_message_get_data)(DBusMessage* message, int_t slot);
extern int (*dbus_message_type_from_string)(const char* type_str);
extern const char* (*dbus_message_type_to_string)(int type);
extern uint_t (*dbus_message_marshal)(
	DBusMessage* msg, char** marshalled_data_p, int* len_p);
extern DBusMessage* (*dbus_message_demarshal)(
	const char* str, int len, DBusError* error);
extern int (*dbus_message_demarshal_bytes_needed)(const char* str, int len);
extern void (*dbus_message_set_allow_interactive_authorization)(
	DBusMessage* message, uint_t allow);
extern uint_t (*dbus_message_get_allow_interactive_authorization)(
	DBusMessage* message);

extern DBusConnection* (*dbus_connection_open)(
	const char* address, DBusError* error);
extern DBusConnection* (*dbus_connection_open_private)(
	const char* address, DBusError* error);
extern DBusConnection* (*dbus_connection_ref)(DBusConnection* connection);
extern void (*dbus_connection_unref)(DBusConnection* connection);
extern void (*dbus_connection_close)(DBusConnection* connection);
extern uint_t (*dbus_connection_get_is_connected)(DBusConnection* connection);
extern uint_t (*dbus_connection_get_is_authenticated)(
	DBusConnection* connection);
extern uint_t (*dbus_connection_get_is_anonymous)(DBusConnection* connection);
extern char* (*dbus_connection_get_server_id)(DBusConnection* connection);
extern uint_t (*dbus_connection_can_send_type)(
	DBusConnection* connection, int type);
extern void (*dbus_connection_set_exit_on_disconnect)(
	DBusConnection* connection, uint_t exit_on_disconnect);
extern void (*dbus_connection_flush)(DBusConnection* connection);
extern uint_t (*dbus_connection_read_write_dispatch)(
	DBusConnection* connection, int timeout_milliseconds);
extern uint_t (*dbus_connection_read_write)(
	DBusConnection* connection, int timeout_milliseconds);
extern DBusMessage* (*dbus_connection_borrow_message)(
	DBusConnection* connection);
extern void (*dbus_connection_return_message)(
	DBusConnection* connection, DBusMessage* message);
extern void (*dbus_connection_steal_borrowed_message)(
	DBusConnection* connection, DBusMessage* message);
extern DBusMessage* (*dbus_connection_pop_message)(DBusConnection* connection);
extern DBusDispatchStatus (*dbus_connection_get_dispatch_status)(
	DBusConnection* connection);
extern DBusDispatchStatus (*dbus_connection_dispatch)(
	DBusConnection* connection);
extern uint_t (*dbus_connection_has_messages_to_send)(
	DBusConnection* connection);
extern uint_t (*dbus_connection_send)(
	DBusConnection* connection, DBusMessage* message, uint_t* client_serial);
extern uint_t (*dbus_connection_send_with_reply)(DBusConnection* connection,
	DBusMessage* message, DBusPendingCall** pending_return,
	int timeout_milliseconds);
extern DBusMessage* (*dbus_connection_send_with_reply_and_block)(
	DBusConnection* connection, DBusMessage* message, int timeout_milliseconds,
	DBusError* error);
extern uint_t (*dbus_connection_set_watch_functions)(DBusConnection* connection,
	DBusAddWatchFunction add_function, DBusRemoveWatchFunction remove_function,
	DBusWatchToggledFunction toggled_function, void* data,
	DBusFreeFunction free_data_function);
extern uint_t (*dbus_connection_set_timeout_functions)(
	DBusConnection* connection, DBusAddTimeoutFunction add_function,
	DBusRemoveTimeoutFunction remove_function,
	DBusTimeoutToggledFunction toggled_function, void* data,
	DBusFreeFunction free_data_function);
extern void (*dbus_connection_set_wakeup_main_function)(
	DBusConnection* connection, DBusWakeupMainFunction wakeup_main_function,
	void* data, DBusFreeFunction free_data_function);
extern void (*dbus_connection_set_dispatch_status_function)(
	DBusConnection* connection, DBusDispatchStatusFunction function, void* data,
	DBusFreeFunction free_data_function);
extern uint_t (*dbus_connection_get_unix_user)(
	DBusConnection* connection, unsigned long* uid);
extern uint_t (*dbus_connection_get_unix_process_id)(
	DBusConnection* connection, unsigned long* pid);
extern uint_t (*dbus_connection_get_adt_audit_session_data)(
	DBusConnection* connection, void** data, int_t* data_size);
extern void (*dbus_connection_set_unix_user_function)(
	DBusConnection* connection, DBusAllowUnixUserFunction function, void* data,
	DBusFreeFunction free_data_function);
extern uint_t (*dbus_connection_get_windows_user)(
	DBusConnection* connection, char** windows_sid_p);
extern void (*dbus_connection_set_windows_user_function)(
	DBusConnection* connection, DBusAllowWindowsUserFunction function,
	void* data, DBusFreeFunction free_data_function);
extern void (*dbus_connection_set_allow_anonymous)(
	DBusConnection* connection, uint_t value);
extern void (*dbus_connection_set_route_peer_messages)(
	DBusConnection* connection, uint_t value);
extern uint_t (*dbus_connection_add_filter)(DBusConnection* connection,
	DBusHandleMessageFunction function, void* user_data,
	DBusFreeFunction free_data_function);
extern void (*dbus_connection_remove_filter)(DBusConnection* connection,
	DBusHandleMessageFunction function, void* user_data);
extern uint_t (*dbus_connection_allocate_data_slot)(int_t* slot_p);
extern void (*dbus_connection_free_data_slot)(int_t* slot_p);
extern uint_t (*dbus_connection_set_data)(DBusConnection* connection,
	int_t slot, void* data, DBusFreeFunction free_data_func);
extern void* (*dbus_connection_get_data)(
	DBusConnection* connection, int_t slot);
extern void (*dbus_connection_set_change_sigpipe)(uint_t will_modify_sigpipe);
extern void (*dbus_connection_set_max_message_size)(
	DBusConnection* connection, long size);
extern long (*dbus_connection_get_max_message_size)(DBusConnection* connection);
extern void (*dbus_connection_set_max_received_size)(
	DBusConnection* connection, long size);
extern long (*dbus_connection_get_max_received_size)(
	DBusConnection* connection);
extern void (*dbus_connection_set_max_message_unix_fds)(
	DBusConnection* connection, long n);
extern long (*dbus_connection_get_max_message_unix_fds)(
	DBusConnection* connection);
extern void (*dbus_connection_set_max_received_unix_fds)(
	DBusConnection* connection, long n);
extern long (*dbus_connection_get_max_received_unix_fds)(
	DBusConnection* connection);
extern long (*dbus_connection_get_outgoing_size)(DBusConnection* connection);
extern long (*dbus_connection_get_outgoing_unix_fds)(
	DBusConnection* connection);
extern DBusPreallocatedSend* (*dbus_connection_preallocate_send)(
	DBusConnection* connection);
extern void (*dbus_connection_free_preallocated_send)(
	DBusConnection* connection, DBusPreallocatedSend* preallocated);
extern void (*dbus_connection_send_preallocated)(DBusConnection* connection,
	DBusPreallocatedSend* preallocated, DBusMessage* message,
	uint_t* client_serial);
extern uint_t (*dbus_connection_try_register_object_path)(
	DBusConnection* connection, const char* path,
	const DBusObjectPathVTable* vtable, void* user_data, DBusError* error);
extern uint_t (*dbus_connection_register_object_path)(
	DBusConnection* connection, const char* path,
	const DBusObjectPathVTable* vtable, void* user_data);
extern uint_t (*dbus_connection_try_register_fallback)(
	DBusConnection* connection, const char* path,
	const DBusObjectPathVTable* vtable, void* user_data, DBusError* error);
extern uint_t (*dbus_connection_register_fallback)(DBusConnection* connection,
	const char* path, const DBusObjectPathVTable* vtable, void* user_data);
extern uint_t (*dbus_connection_unregister_object_path)(
	DBusConnection* connection, const char* path);
extern uint_t (*dbus_connection_get_object_path_data)(
	DBusConnection* connection, const char* path, void** data_p);
extern uint_t (*dbus_connection_list_registered)(
	DBusConnection* connection, const char* parent_path, char*** child_entries);
extern uint_t (*dbus_connection_get_unix_fd)(
	DBusConnection* connection, int* fd);
extern uint_t (*dbus_connection_get_socket)(
	DBusConnection* connection, int* fd);
extern int (*dbus_watch_get_fd)(DBusWatch* watch);
extern int (*dbus_watch_get_unix_fd)(DBusWatch* watch);
extern int (*dbus_watch_get_socket)(DBusWatch* watch);
extern unsigned int (*dbus_watch_get_flags)(DBusWatch* watch);
extern void* (*dbus_watch_get_data)(DBusWatch* watch);
extern void (*dbus_watch_set_data)(
	DBusWatch* watch, void* data, DBusFreeFunction free_data_function);
extern uint_t (*dbus_watch_handle)(DBusWatch* watch, unsigned int flags);
extern uint_t (*dbus_watch_get_enabled)(DBusWatch* watch);
extern int (*dbus_timeout_get_interval)(DBusTimeout* timeout);
extern void* (*dbus_timeout_get_data)(DBusTimeout* timeout);
extern void (*dbus_timeout_set_data)(
	DBusTimeout* timeout, void* data, DBusFreeFunction free_data_function);
extern uint_t (*dbus_timeout_handle)(DBusTimeout* timeout);
extern uint_t (*dbus_timeout_get_enabled)(DBusTimeout* timeout);

extern DBusConnection* (*dbus_bus_get)(DBusBusType type, DBusError* error);
extern DBusConnection* (*dbus_bus_get_private)(
	DBusBusType type, DBusError* error);
extern uint_t (*dbus_bus_register)(
	DBusConnection* connection, DBusError* error);
extern uint_t (*dbus_bus_set_unique_name)(
	DBusConnection* connection, const char* unique_name);
extern const char* (*dbus_bus_get_unique_name)(DBusConnection* connection);
extern unsigned long (*dbus_bus_get_unix_user)(
	DBusConnection* connection, const char* name, DBusError* error);
extern char* (*dbus_bus_get_id)(DBusConnection* connection, DBusError* error);
extern int (*dbus_bus_request_name)(DBusConnection* connection,
	const char* name, unsigned int flags, DBusError* error);
extern int (*dbus_bus_release_name)(
	DBusConnection* connection, const char* name, DBusError* error);
extern uint_t (*dbus_bus_name_has_owner)(
	DBusConnection* connection, const char* name, DBusError* error);
extern uint_t (*dbus_bus_start_service_by_name)(DBusConnection* connection,
	const char* name, uint_t flags, uint_t* reply, DBusError* error);
extern void (*dbus_bus_add_match)(
	DBusConnection* connection, const char* rule, DBusError* error);
extern void (*dbus_bus_remove_match)(
	DBusConnection* connection, const char* rule, DBusError* error);

extern char* (*dbus_get_local_machine_id)(void);
extern void (*dbus_get_version)(
	int* major_version_p, int* minor_version_p, int* micro_version_p);
extern uint_t (*dbus_setenv)(const char* variable, const char* value);
extern char* (*dbus_try_get_local_machine_id)(DBusError* error);

extern DBusPendingCall* (*dbus_pending_call_ref)(DBusPendingCall* pending);
extern void (*dbus_pending_call_unref)(DBusPendingCall* pending);
extern uint_t (*dbus_pending_call_set_notify)(DBusPendingCall* pending,
	DBusPendingCallNotifyFunction function, void* user_data,
	DBusFreeFunction free_user_data);
extern void (*dbus_pending_call_cancel)(DBusPendingCall* pending);
extern uint_t (*dbus_pending_call_get_completed)(DBusPendingCall* pending);
extern DBusMessage* (*dbus_pending_call_steal_reply)(DBusPendingCall* pending);
extern void (*dbus_pending_call_block)(DBusPendingCall* pending);
extern uint_t (*dbus_pending_call_allocate_data_slot)(int_t* slot_p);
extern void (*dbus_pending_call_free_data_slot)(int_t* slot_p);
extern uint_t (*dbus_pending_call_set_data)(DBusPendingCall* pending,
	int_t slot, void* data, DBusFreeFunction free_data_func);
extern void* (*dbus_pending_call_get_data)(
	DBusPendingCall* pending, int_t slot);

extern DBusServer* (*dbus_server_listen)(const char* address, DBusError* error);
extern DBusServer* (*dbus_server_ref)(DBusServer* server);
extern void (*dbus_server_unref)(DBusServer* server);
extern void (*dbus_server_disconnect)(DBusServer* server);
extern uint_t (*dbus_server_get_is_connected)(DBusServer* server);
extern char* (*dbus_server_get_address)(DBusServer* server);
extern char* (*dbus_server_get_id)(DBusServer* server);
extern void (*dbus_server_set_new_connection_function)(DBusServer* server,
	DBusNewConnectionFunction function, void* data,
	DBusFreeFunction free_data_function);
extern uint_t (*dbus_server_set_watch_functions)(DBusServer* server,
	DBusAddWatchFunction add_function, DBusRemoveWatchFunction remove_function,
	DBusWatchToggledFunction toggled_function, void* data,
	DBusFreeFunction free_data_function);
extern uint_t (*dbus_server_set_timeout_functions)(DBusServer* server,
	DBusAddTimeoutFunction add_function,
	DBusRemoveTimeoutFunction remove_function,
	DBusTimeoutToggledFunction toggled_function, void* data,
	DBusFreeFunction free_data_function);
extern uint_t (*dbus_server_set_auth_mechanisms)(
	DBusServer* server, const char** mechanisms);
extern uint_t (*dbus_server_allocate_data_slot)(int_t* slot_p);
extern void (*dbus_server_free_data_slot)(int_t* slot_p);
extern uint_t (*dbus_server_set_data)(
	DBusServer* server, int slot, void* data, DBusFreeFunction free_data_func);
extern void* (*dbus_server_get_data)(DBusServer* server, int slot);

extern void (*dbus_signature_iter_init)(
	DBusSignatureIter* iter, const char* signature);
extern int (*dbus_signature_iter_get_current_type)(
	const DBusSignatureIter* iter);
extern char* (*dbus_signature_iter_get_signature)(
	const DBusSignatureIter* iter);
extern int (*dbus_signature_iter_get_element_type)(
	const DBusSignatureIter* iter);
extern uint_t (*dbus_signature_iter_next)(DBusSignatureIter* iter);
extern void (*dbus_signature_iter_recurse)(
	const DBusSignatureIter* iter, DBusSignatureIter* subiter);
extern uint_t (*dbus_signature_validate)(
	const char* signature, DBusError* error);
extern uint_t (*dbus_signature_validate_single)(
	const char* signature, DBusError* error);
extern uint_t (*dbus_type_is_valid)(int typecode);
extern uint_t (*dbus_type_is_basic)(int typecode);
extern uint_t (*dbus_type_is_container)(int typecode);
extern uint_t (*dbus_type_is_fixed)(int typecode);

extern uint_t (*dbus_validate_path)(const char* path, DBusError* error);
extern uint_t (*dbus_validate_interface)(const char* name, DBusError* error);
extern uint_t (*dbus_validate_member)(const char* name, DBusError* error);
extern uint_t (*dbus_validate_error_name)(const char* name, DBusError* error);
extern uint_t (*dbus_validate_bus_name)(const char* name, DBusError* error);
extern uint_t (*dbus_validate_utf8)(const char* alleged_utf8, DBusError* error);

extern uint_t (*dbus_threads_init)(const DBusThreadFunctions* functions);
extern uint_t (*dbus_threads_init_default)(void);
#endif

}}
/* END */

#endif
