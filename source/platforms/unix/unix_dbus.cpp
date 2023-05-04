/***************************************

	Shims for the dbus library

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtypes.h"

#if defined(BURGER_UNIX) || defined(DOXYGEN)

#include <dbus/dbus.h>

#include "unix_dbus.h"

#if !defined(DOXYGEN)
static Burger::CodeLibrary g_DBus;

namespace Burger { namespace Unix {

// From dbus-errors.h
void (*dbus_error_init)(DBusError* error);
void (*dbus_error_free)(DBusError* error);
void (*dbus_set_error)(
	DBusError* error, const char* name, const char* message, ...);
void (*dbus_set_error_const)(
	DBusError* error, const char* name, const char* message);
void (*dbus_move_error)(DBusError* src, DBusError* dest);
uint_t (*dbus_error_has_name)(const DBusError* error, const char* name);
uint_t (*dbus_error_is_set)(const DBusError* error);

// From dbus-address.h
uint_t (*dbus_parse_address)(const char* address,
	DBusAddressEntry*** entry_result, int* array_len, DBusError* error);
const char* (*dbus_address_entry_get_value)(
	DBusAddressEntry* entry, const char* key);
const char* (*dbus_address_entry_get_method)(DBusAddressEntry* entry);
void (*dbus_address_entries_free)(DBusAddressEntry** entries);
char* (*dbus_address_escape_value)(const char* value);
char* (*dbus_address_unescape_value)(const char* value, DBusError* error);

// From dbus-memory.h
void* (*dbus_malloc)(uintptr_t bytes);
void* (*dbus_malloc0)(uintptr_t bytes);
void* (*dbus_realloc)(void* memory, uintptr_t bytes);
void (*dbus_free)(void* memory);
void (*dbus_free_string_array)(char** str_array);
void (*dbus_shutdown)(void);

// dbus-message.h
DBusMessage* (*dbus_message_new)(int message_type);
DBusMessage* (*dbus_message_new_method_call)(const char* bus_name,
	const char* path, const char* iface, const char* method);
DBusMessage* (*dbus_message_new_method_return)(DBusMessage* method_call);
DBusMessage* (*dbus_message_new_signal)(
	const char* path, const char* iface, const char* name);
DBusMessage* (*dbus_message_new_error)(
	DBusMessage* reply_to, const char* error_name, const char* error_message);
DBusMessage* (*dbus_message_new_error_printf)(DBusMessage* reply_to,
	const char* error_name, const char* error_format, ...);
DBusMessage* (*dbus_message_copy)(const DBusMessage* message);
DBusMessage* (*dbus_message_ref)(DBusMessage* message);
void (*dbus_message_unref)(DBusMessage* message);
int (*dbus_message_get_type)(DBusMessage* message);
uint_t (*dbus_message_set_path)(DBusMessage* message, const char* object_path);
const char* (*dbus_message_get_path)(DBusMessage* message);
uint_t (*dbus_message_has_path)(DBusMessage* message, const char* object_path);
uint_t (*dbus_message_set_interface)(DBusMessage* message, const char* iface);
const char* (*dbus_message_get_interface)(DBusMessage* message);
uint_t (*dbus_message_has_interface)(DBusMessage* message, const char* iface);
uint_t (*dbus_message_set_member)(DBusMessage* message, const char* member);
const char* (*dbus_message_get_member)(DBusMessage* message);
uint_t (*dbus_message_has_member)(DBusMessage* message, const char* member);
uint_t (*dbus_message_set_error_name)(DBusMessage* message, const char* name);
const char* (*dbus_message_get_error_name)(DBusMessage* message);
uint_t (*dbus_message_set_destination)(
	DBusMessage* message, const char* destination);
const char* (*dbus_message_get_destination)(DBusMessage* message);
uint_t (*dbus_message_set_sender)(DBusMessage* message, const char* sender);
const char* (*dbus_message_get_sender)(DBusMessage* message);
const char* (*dbus_message_get_signature)(DBusMessage* message);
void (*dbus_message_set_no_reply)(DBusMessage* message, uint_t no_reply);
uint_t (*dbus_message_get_no_reply)(DBusMessage* message);
uint_t (*dbus_message_is_method_call)(
	DBusMessage* message, const char* iface, const char* method);
uint_t (*dbus_message_is_signal)(
	DBusMessage* message, const char* iface, const char* signal_name);
uint_t (*dbus_message_is_error)(DBusMessage* message, const char* error_name);
uint_t (*dbus_message_has_destination)(
	DBusMessage* message, const char* bus_name);
uint_t (*dbus_message_has_sender)(
	DBusMessage* message, const char* unique_bus_name);
uint_t (*dbus_message_has_signature)(
	DBusMessage* message, const char* signature);
uint_t (*dbus_message_get_serial)(DBusMessage* message);
void (*dbus_message_set_serial)(DBusMessage* message, uint_t serial);
uint_t (*dbus_message_set_reply_serial)(
	DBusMessage* message, uint_t reply_serial);
uint_t (*dbus_message_get_reply_serial)(DBusMessage* message);
void (*dbus_message_set_auto_start)(DBusMessage* message, uint_t auto_start);
uint_t (*dbus_message_get_auto_start)(DBusMessage* message);
uint_t (*dbus_message_get_path_decomposed)(DBusMessage* message, char*** path);
uint_t (*dbus_message_append_args)(
	DBusMessage* message, int first_arg_type, ...);
uint_t (*dbus_message_append_args_valist)(
	DBusMessage* message, int first_arg_type, va_list var_args);
uint_t (*dbus_message_get_args)(
	DBusMessage* message, DBusError* error, int first_arg_type, ...);
uint_t (*dbus_message_get_args_valist)(DBusMessage* message, DBusError* error,
	int first_arg_type, va_list var_args);
uint_t (*dbus_message_contains_unix_fds)(DBusMessage* message);
void (*dbus_message_iter_init_closed)(DBusMessageIter* iter);
uint_t (*dbus_message_iter_init)(DBusMessage* message, DBusMessageIter* iter);
uint_t (*dbus_message_iter_has_next)(DBusMessageIter* iter);
uint_t (*dbus_message_iter_next)(DBusMessageIter* iter);
char* (*dbus_message_iter_get_signature)(DBusMessageIter* iter);
int (*dbus_message_iter_get_arg_type)(DBusMessageIter* iter);
int (*dbus_message_iter_get_element_type)(DBusMessageIter* iter);
void (*dbus_message_iter_recurse)(DBusMessageIter* iter, DBusMessageIter* sub);
void (*dbus_message_iter_get_basic)(DBusMessageIter* iter, void* value);
int (*dbus_message_iter_get_element_count)(DBusMessageIter* iter);
int (*dbus_message_iter_get_array_len)(DBusMessageIter* iter);
void (*dbus_message_iter_get_fixed_array)(
	DBusMessageIter* iter, void* value, int* n_elements);
void (*dbus_message_iter_init_append)(
	DBusMessage* message, DBusMessageIter* iter);
uint_t (*dbus_message_iter_append_basic)(
	DBusMessageIter* iter, int type, const void* value);
uint_t (*dbus_message_iter_append_fixed_array)(
	DBusMessageIter* iter, int element_type, const void* value, int n_elements);
uint_t (*dbus_message_iter_open_container)(DBusMessageIter* iter, int type,
	const char* contained_signature, DBusMessageIter* sub);
uint_t (*dbus_message_iter_close_container)(
	DBusMessageIter* iter, DBusMessageIter* sub);
void (*dbus_message_iter_abandon_container)(
	DBusMessageIter* iter, DBusMessageIter* sub);
void (*dbus_message_iter_abandon_container_if_open)(
	DBusMessageIter* iter, DBusMessageIter* sub);
void (*dbus_message_lock)(DBusMessage* message);
uint_t (*dbus_set_error_from_message)(DBusError* error, DBusMessage* message);
uint_t (*dbus_message_allocate_data_slot)(int_t* slot_p);
void (*dbus_message_free_data_slot)(int_t* slot_p);
uint_t (*dbus_message_set_data)(DBusMessage* message, int_t slot, void* data,
	DBusFreeFunction free_data_func);
void* (*dbus_message_get_data)(DBusMessage* message, int_t slot);
int (*dbus_message_type_from_string)(const char* type_str);
const char* (*dbus_message_type_to_string)(int type);
uint_t (*dbus_message_marshal)(
	DBusMessage* msg, char** marshalled_data_p, int* len_p);
DBusMessage* (*dbus_message_demarshal)(
	const char* str, int len, DBusError* error);
int (*dbus_message_demarshal_bytes_needed)(const char* str, int len);
void (*dbus_message_set_allow_interactive_authorization)(
	DBusMessage* message, uint_t allow);
uint_t (*dbus_message_get_allow_interactive_authorization)(
	DBusMessage* message);

// dbus-connection.h
DBusConnection* (*dbus_connection_open)(const char* address, DBusError* error);
DBusConnection* (*dbus_connection_open_private)(
	const char* address, DBusError* error);
DBusConnection* (*dbus_connection_ref)(DBusConnection* connection);
void (*dbus_connection_unref)(DBusConnection* connection);
void (*dbus_connection_close)(DBusConnection* connection);
uint_t (*dbus_connection_get_is_connected)(DBusConnection* connection);
uint_t (*dbus_connection_get_is_authenticated)(DBusConnection* connection);
uint_t (*dbus_connection_get_is_anonymous)(DBusConnection* connection);
char* (*dbus_connection_get_server_id)(DBusConnection* connection);
uint_t (*dbus_connection_can_send_type)(DBusConnection* connection, int type);
void (*dbus_connection_set_exit_on_disconnect)(
	DBusConnection* connection, uint_t exit_on_disconnect);
void (*dbus_connection_flush)(DBusConnection* connection);
uint_t (*dbus_connection_read_write_dispatch)(
	DBusConnection* connection, int timeout_milliseconds);
uint_t (*dbus_connection_read_write)(
	DBusConnection* connection, int timeout_milliseconds);
DBusMessage* (*dbus_connection_borrow_message)(DBusConnection* connection);
void (*dbus_connection_return_message)(
	DBusConnection* connection, DBusMessage* message);
void (*dbus_connection_steal_borrowed_message)(
	DBusConnection* connection, DBusMessage* message);
DBusMessage* (*dbus_connection_pop_message)(DBusConnection* connection);
DBusDispatchStatus (*dbus_connection_get_dispatch_status)(
	DBusConnection* connection);
DBusDispatchStatus (*dbus_connection_dispatch)(DBusConnection* connection);
uint_t (*dbus_connection_has_messages_to_send)(DBusConnection* connection);
uint_t (*dbus_connection_send)(
	DBusConnection* connection, DBusMessage* message, uint_t* client_serial);
uint_t (*dbus_connection_send_with_reply)(DBusConnection* connection,
	DBusMessage* message, DBusPendingCall** pending_return,
	int timeout_milliseconds);
DBusMessage* (*dbus_connection_send_with_reply_and_block)(
	DBusConnection* connection, DBusMessage* message, int timeout_milliseconds,
	DBusError* error);
uint_t (*dbus_connection_set_watch_functions)(DBusConnection* connection,
	DBusAddWatchFunction add_function, DBusRemoveWatchFunction remove_function,
	DBusWatchToggledFunction toggled_function, void* data,
	DBusFreeFunction free_data_function);
uint_t (*dbus_connection_set_timeout_functions)(DBusConnection* connection,
	DBusAddTimeoutFunction add_function,
	DBusRemoveTimeoutFunction remove_function,
	DBusTimeoutToggledFunction toggled_function, void* data,
	DBusFreeFunction free_data_function);
void (*dbus_connection_set_wakeup_main_function)(DBusConnection* connection,
	DBusWakeupMainFunction wakeup_main_function, void* data,
	DBusFreeFunction free_data_function);
void (*dbus_connection_set_dispatch_status_function)(DBusConnection* connection,
	DBusDispatchStatusFunction function, void* data,
	DBusFreeFunction free_data_function);
uint_t (*dbus_connection_get_unix_user)(
	DBusConnection* connection, unsigned long* uid);
uint_t (*dbus_connection_get_unix_process_id)(
	DBusConnection* connection, unsigned long* pid);
uint_t (*dbus_connection_get_adt_audit_session_data)(
	DBusConnection* connection, void** data, int_t* data_size);
void (*dbus_connection_set_unix_user_function)(DBusConnection* connection,
	DBusAllowUnixUserFunction function, void* data,
	DBusFreeFunction free_data_function);
uint_t (*dbus_connection_get_windows_user)(
	DBusConnection* connection, char** windows_sid_p);
void (*dbus_connection_set_windows_user_function)(DBusConnection* connection,
	DBusAllowWindowsUserFunction function, void* data,
	DBusFreeFunction free_data_function);
void (*dbus_connection_set_allow_anonymous)(
	DBusConnection* connection, uint_t value);
void (*dbus_connection_set_route_peer_messages)(
	DBusConnection* connection, uint_t value);
uint_t (*dbus_connection_add_filter)(DBusConnection* connection,
	DBusHandleMessageFunction function, void* user_data,
	DBusFreeFunction free_data_function);
void (*dbus_connection_remove_filter)(DBusConnection* connection,
	DBusHandleMessageFunction function, void* user_data);
uint_t (*dbus_connection_allocate_data_slot)(int_t* slot_p);
void (*dbus_connection_free_data_slot)(int_t* slot_p);
uint_t (*dbus_connection_set_data)(DBusConnection* connection, int_t slot,
	void* data, DBusFreeFunction free_data_func);
void* (*dbus_connection_get_data)(DBusConnection* connection, int_t slot);
void (*dbus_connection_set_change_sigpipe)(uint_t will_modify_sigpipe);
void (*dbus_connection_set_max_message_size)(
	DBusConnection* connection, long size);
long (*dbus_connection_get_max_message_size)(DBusConnection* connection);
void (*dbus_connection_set_max_received_size)(
	DBusConnection* connection, long size);
long (*dbus_connection_get_max_received_size)(DBusConnection* connection);
void (*dbus_connection_set_max_message_unix_fds)(
	DBusConnection* connection, long n);
long (*dbus_connection_get_max_message_unix_fds)(DBusConnection* connection);
void (*dbus_connection_set_max_received_unix_fds)(
	DBusConnection* connection, long n);
long (*dbus_connection_get_max_received_unix_fds)(DBusConnection* connection);
long (*dbus_connection_get_outgoing_size)(DBusConnection* connection);
long (*dbus_connection_get_outgoing_unix_fds)(DBusConnection* connection);
DBusPreallocatedSend* (*dbus_connection_preallocate_send)(
	DBusConnection* connection);
void (*dbus_connection_free_preallocated_send)(
	DBusConnection* connection, DBusPreallocatedSend* preallocated);
void (*dbus_connection_send_preallocated)(DBusConnection* connection,
	DBusPreallocatedSend* preallocated, DBusMessage* message,
	uint_t* client_serial);
uint_t (*dbus_connection_try_register_object_path)(DBusConnection* connection,
	const char* path, const DBusObjectPathVTable* vtable, void* user_data,
	DBusError* error);
uint_t (*dbus_connection_register_object_path)(DBusConnection* connection,
	const char* path, const DBusObjectPathVTable* vtable, void* user_data);
uint_t (*dbus_connection_try_register_fallback)(DBusConnection* connection,
	const char* path, const DBusObjectPathVTable* vtable, void* user_data,
	DBusError* error);
uint_t (*dbus_connection_register_fallback)(DBusConnection* connection,
	const char* path, const DBusObjectPathVTable* vtable, void* user_data);
uint_t (*dbus_connection_unregister_object_path)(
	DBusConnection* connection, const char* path);
uint_t (*dbus_connection_get_object_path_data)(
	DBusConnection* connection, const char* path, void** data_p);
uint_t (*dbus_connection_list_registered)(
	DBusConnection* connection, const char* parent_path, char*** child_entries);
uint_t (*dbus_connection_get_unix_fd)(DBusConnection* connection, int* fd);
uint_t (*dbus_connection_get_socket)(DBusConnection* connection, int* fd);
int (*dbus_watch_get_fd)(DBusWatch* watch);
int (*dbus_watch_get_unix_fd)(DBusWatch* watch);
int (*dbus_watch_get_socket)(DBusWatch* watch);
unsigned int (*dbus_watch_get_flags)(DBusWatch* watch);
void* (*dbus_watch_get_data)(DBusWatch* watch);
void (*dbus_watch_set_data)(
	DBusWatch* watch, void* data, DBusFreeFunction free_data_function);
uint_t (*dbus_watch_handle)(DBusWatch* watch, unsigned int flags);
uint_t (*dbus_watch_get_enabled)(DBusWatch* watch);
int (*dbus_timeout_get_interval)(DBusTimeout* timeout);
void* (*dbus_timeout_get_data)(DBusTimeout* timeout);
void (*dbus_timeout_set_data)(
	DBusTimeout* timeout, void* data, DBusFreeFunction free_data_function);
uint_t (*dbus_timeout_handle)(DBusTimeout* timeout);
uint_t (*dbus_timeout_get_enabled)(DBusTimeout* timeout);

// From dbus-bus.h
DBusConnection* (*dbus_bus_get)(DBusBusType type, DBusError* error);
DBusConnection* (*dbus_bus_get_private)(DBusBusType type, DBusError* error);
uint_t (*dbus_bus_register)(DBusConnection* connection, DBusError* error);
uint_t (*dbus_bus_set_unique_name)(
	DBusConnection* connection, const char* unique_name);
const char* (*dbus_bus_get_unique_name)(DBusConnection* connection);
unsigned long (*dbus_bus_get_unix_user)(
	DBusConnection* connection, const char* name, DBusError* error);
char* (*dbus_bus_get_id)(DBusConnection* connection, DBusError* error);
int (*dbus_bus_request_name)(DBusConnection* connection, const char* name,
	unsigned int flags, DBusError* error);
int (*dbus_bus_release_name)(
	DBusConnection* connection, const char* name, DBusError* error);
uint_t (*dbus_bus_name_has_owner)(
	DBusConnection* connection, const char* name, DBusError* error);
uint_t (*dbus_bus_start_service_by_name)(DBusConnection* connection,
	const char* name, uint_t flags, uint_t* reply, DBusError* error);
void (*dbus_bus_add_match)(
	DBusConnection* connection, const char* rule, DBusError* error);
void (*dbus_bus_remove_match)(
	DBusConnection* connection, const char* rule, DBusError* error);

// From dbus-misc.h
char* (*dbus_get_local_machine_id)(void);
void (*dbus_get_version)(
	int* major_version_p, int* minor_version_p, int* micro_version_p);
uint_t (*dbus_setenv)(const char* variable, const char* value);
char* (*dbus_try_get_local_machine_id)(DBusError* error);

// From dbus-pending-call
DBusPendingCall* (*dbus_pending_call_ref)(DBusPendingCall* pending);
void (*dbus_pending_call_unref)(DBusPendingCall* pending);
uint_t (*dbus_pending_call_set_notify)(DBusPendingCall* pending,
	DBusPendingCallNotifyFunction function, void* user_data,
	DBusFreeFunction free_user_data);
void (*dbus_pending_call_cancel)(DBusPendingCall* pending);
uint_t (*dbus_pending_call_get_completed)(DBusPendingCall* pending);
DBusMessage* (*dbus_pending_call_steal_reply)(DBusPendingCall* pending);
void (*dbus_pending_call_block)(DBusPendingCall* pending);
uint_t (*dbus_pending_call_allocate_data_slot)(int_t* slot_p);
void (*dbus_pending_call_free_data_slot)(int_t* slot_p);
uint_t (*dbus_pending_call_set_data)(DBusPendingCall* pending, int_t slot,
	void* data, DBusFreeFunction free_data_func);
void* (*dbus_pending_call_get_data)(DBusPendingCall* pending, int_t slot);

// From dbus-server.h
DBusServer* (*dbus_server_listen)(const char* address, DBusError* error);
DBusServer* (*dbus_server_ref)(DBusServer* server);
void (*dbus_server_unref)(DBusServer* server);
void (*dbus_server_disconnect)(DBusServer* server);
uint_t (*dbus_server_get_is_connected)(DBusServer* server);
char* (*dbus_server_get_address)(DBusServer* server);
char* (*dbus_server_get_id)(DBusServer* server);
void (*dbus_server_set_new_connection_function)(DBusServer* server,
	DBusNewConnectionFunction function, void* data,
	DBusFreeFunction free_data_function);
uint_t (*dbus_server_set_watch_functions)(DBusServer* server,
	DBusAddWatchFunction add_function, DBusRemoveWatchFunction remove_function,
	DBusWatchToggledFunction toggled_function, void* data,
	DBusFreeFunction free_data_function);
uint_t (*dbus_server_set_timeout_functions)(DBusServer* server,
	DBusAddTimeoutFunction add_function,
	DBusRemoveTimeoutFunction remove_function,
	DBusTimeoutToggledFunction toggled_function, void* data,
	DBusFreeFunction free_data_function);
uint_t (*dbus_server_set_auth_mechanisms)(
	DBusServer* server, const char** mechanisms);
uint_t (*dbus_server_allocate_data_slot)(int_t* slot_p);
void (*dbus_server_free_data_slot)(int_t* slot_p);
uint_t (*dbus_server_set_data)(
	DBusServer* server, int slot, void* data, DBusFreeFunction free_data_func);
void* (*dbus_server_get_data)(DBusServer* server, int slot);

// From dbus-signature.h
void (*dbus_signature_iter_init)(
	DBusSignatureIter* iter, const char* signature);
int (*dbus_signature_iter_get_current_type)(const DBusSignatureIter* iter);
char* (*dbus_signature_iter_get_signature)(const DBusSignatureIter* iter);
int (*dbus_signature_iter_get_element_type)(const DBusSignatureIter* iter);
uint_t (*dbus_signature_iter_next)(DBusSignatureIter* iter);
void (*dbus_signature_iter_recurse)(
	const DBusSignatureIter* iter, DBusSignatureIter* subiter);
uint_t (*dbus_signature_validate)(const char* signature, DBusError* error);
uint_t (*dbus_signature_validate_single)(
	const char* signature, DBusError* error);
uint_t (*dbus_type_is_valid)(int typecode);
uint_t (*dbus_type_is_basic)(int typecode);
uint_t (*dbus_type_is_container)(int typecode);
uint_t (*dbus_type_is_fixed)(int typecode);

// From dbus-syntax.h
uint_t (*dbus_validate_path)(const char* path, DBusError* error);
uint_t (*dbus_validate_interface)(const char* name, DBusError* error);
uint_t (*dbus_validate_member)(const char* name, DBusError* error);
uint_t (*dbus_validate_error_name)(const char* name, DBusError* error);
uint_t (*dbus_validate_bus_name)(const char* name, DBusError* error);
uint_t (*dbus_validate_utf8)(const char* alleged_utf8, DBusError* error);

// From dbus-threads.h
uint_t (*dbus_threads_init)(const DBusThreadFunctions* functions);
uint_t (*dbus_threads_init_default)(void);
}}

enum eCallIndex {
	kCall_dbus_error_init,
	kCall_dbus_error_free,
	kCall_dbus_set_error,
	kCall_dbus_set_error_const,
	kCall_dbus_move_error,
	kCall_dbus_error_has_name,
	kCall_dbus_error_is_set,
	kCall_dbus_parse_address,
	kCall_dbus_address_entry_get_value,
	kCall_dbus_address_entry_get_method,
	kCall_dbus_address_entries_free,
	kCall_dbus_address_escape_value,
	kCall_dbus_address_unescape_value,
	kCall_dbus_malloc,
	kCall_dbus_malloc0,
	kCall_dbus_realloc,
	kCall_dbus_free,
	kCall_dbus_free_string_array,
	kCall_dbus_shutdown,
	kCall_dbus_message_new,
	kCall_dbus_message_new_method_call,
	kCall_dbus_message_new_method_return,
	kCall_dbus_message_new_signal,
	kCall_dbus_message_new_error,
	kCall_dbus_message_new_error_printf,
	kCall_dbus_message_copy,
	kCall_dbus_message_ref,
	kCall_dbus_message_unref,
	kCall_dbus_message_get_type,
	kCall_dbus_message_set_path,
	kCall_dbus_message_get_path,
	kCall_dbus_message_has_path,
	kCall_dbus_message_set_interface,
	kCall_dbus_message_get_interface,
	kCall_dbus_message_has_interface,
	kCall_dbus_message_set_member,
	kCall_dbus_message_get_member,
	kCall_dbus_message_has_member,
	kCall_dbus_message_set_error_name,
	kCall_dbus_message_get_error_name,
	kCall_dbus_message_set_destination,
	kCall_dbus_message_get_destination,
	kCall_dbus_message_set_sender,
	kCall_dbus_message_get_sender,
	kCall_dbus_message_get_signature,
	kCall_dbus_message_set_no_reply,
	kCall_dbus_message_get_no_reply,
	kCall_dbus_message_is_method_call,
	kCall_dbus_message_is_signal,
	kCall_dbus_message_is_error,
	kCall_dbus_message_has_destination,
	kCall_dbus_message_has_sender,
	kCall_dbus_message_has_signature,
	kCall_dbus_message_get_serial,
	kCall_dbus_message_set_serial,
	kCall_dbus_message_set_reply_serial,
	kCall_dbus_message_get_reply_serial,
	kCall_dbus_message_set_auto_start,
	kCall_dbus_message_get_auto_start,
	kCall_dbus_message_get_path_decomposed,
	kCall_dbus_message_append_args,
	kCall_dbus_message_append_args_valist,
	kCall_dbus_message_get_args,
	kCall_dbus_message_get_args_valist,
	kCall_dbus_message_contains_unix_fds,
	kCall_dbus_message_iter_init_closed,
	kCall_dbus_message_iter_init,
	kCall_dbus_message_iter_has_next,
	kCall_dbus_message_iter_next,
	kCall_dbus_message_iter_get_signature,
	kCall_dbus_message_iter_get_arg_type,
	kCall_dbus_message_iter_get_element_type,
	kCall_dbus_message_iter_recurse,
	kCall_dbus_message_iter_get_basic,
	kCall_dbus_message_iter_get_element_count,
	kCall_dbus_message_iter_get_array_len,
	kCall_dbus_message_iter_get_fixed_array,
	kCall_dbus_message_iter_init_append,
	kCall_dbus_message_iter_append_basic,
	kCall_dbus_message_iter_append_fixed_array,
	kCall_dbus_message_iter_open_container,
	kCall_dbus_message_iter_close_container,
	kCall_dbus_message_iter_abandon_container,
	kCall_dbus_message_iter_abandon_container_if_open,
	kCall_dbus_message_lock,
	kCall_dbus_set_error_from_message,
	kCall_dbus_message_allocate_data_slot,
	kCall_dbus_message_free_data_slot,
	kCall_dbus_message_set_data,
	kCall_dbus_message_get_data,
	kCall_dbus_message_type_from_string,
	kCall_dbus_message_type_to_string,
	kCall_dbus_message_marshal,
	kCall_dbus_message_demarshal,
	kCall_dbus_message_demarshal_bytes_needed,
	kCall_dbus_message_set_allow_interactive_authorization,
	kCall_dbus_message_get_allow_interactive_authorization,
	kCall_dbus_connection_open,
	kCall_dbus_connection_open_private,
	kCall_dbus_connection_ref,
	kCall_dbus_connection_unref,
	kCall_dbus_connection_close,
	kCall_dbus_connection_get_is_connected,
	kCall_dbus_connection_get_is_authenticated,
	kCall_dbus_connection_get_is_anonymous,
	kCall_dbus_connection_get_server_id,
	kCall_dbus_connection_can_send_type,
	kCall_dbus_connection_set_exit_on_disconnect,
	kCall_dbus_connection_flush,
	kCall_dbus_connection_read_write_dispatch,
	kCall_dbus_connection_read_write,
	kCall_dbus_connection_borrow_message,
	kCall_dbus_connection_return_message,
	kCall_dbus_connection_steal_borrowed_message,
	kCall_dbus_connection_pop_message,
	kCall_dbus_connection_get_dispatch_status,
	kCall_dbus_connection_dispatch,
	kCall_dbus_connection_has_messages_to_send,
	kCall_dbus_connection_send,
	kCall_dbus_connection_send_with_reply,
	kCall_dbus_connection_send_with_reply_and_block,
	kCall_dbus_connection_set_watch_functions,
	kCall_dbus_connection_set_timeout_functions,
	kCall_dbus_connection_set_wakeup_main_function,
	kCall_dbus_connection_set_dispatch_status_function,
	kCall_dbus_connection_get_unix_user,
	kCall_dbus_connection_get_unix_process_id,
	kCall_dbus_connection_get_adt_audit_session_data,
	kCall_dbus_connection_set_unix_user_function,
	kCall_dbus_connection_get_windows_user,
	kCall_dbus_connection_set_windows_user_function,
	kCall_dbus_connection_set_allow_anonymous,
	kCall_dbus_connection_set_route_peer_messages,
	kCall_dbus_connection_add_filter,
	kCall_dbus_connection_remove_filter,
	kCall_dbus_connection_allocate_data_slot,
	kCall_dbus_connection_free_data_slot,
	kCall_dbus_connection_set_data,
	kCall_dbus_connection_get_data,
	kCall_dbus_connection_set_change_sigpipe,
	kCall_dbus_connection_set_max_message_size,
	kCall_dbus_connection_get_max_message_size,
	kCall_dbus_connection_set_max_received_size,
	kCall_dbus_connection_get_max_received_size,
	kCall_dbus_connection_set_max_message_unix_fds,
	kCall_dbus_connection_get_max_message_unix_fds,
	kCall_dbus_connection_set_max_received_unix_fds,
	kCall_dbus_connection_get_max_received_unix_fds,
	kCall_dbus_connection_get_outgoing_size,
	kCall_dbus_connection_get_outgoing_unix_fds,
	kCall_dbus_connection_preallocate_send,
	kCall_dbus_connection_free_preallocated_send,
	kCall_dbus_connection_send_preallocated,
	kCall_dbus_connection_try_register_object_path,
	kCall_dbus_connection_register_object_path,
	kCall_dbus_connection_try_register_fallback,
	kCall_dbus_connection_register_fallback,
	kCall_dbus_connection_unregister_object_path,
	kCall_dbus_connection_get_object_path_data,
	kCall_dbus_connection_list_registered,
	kCall_dbus_connection_get_unix_fd,
	kCall_dbus_connection_get_socket,
	kCall_dbus_watch_get_fd,
	kCall_dbus_watch_get_unix_fd,
	kCall_dbus_watch_get_socket,
	kCall_dbus_watch_get_flags,
	kCall_dbus_watch_get_data,
	kCall_dbus_watch_set_data,
	kCall_dbus_watch_handle,
	kCall_dbus_watch_get_enabled,
	kCall_dbus_timeout_get_interval,
	kCall_dbus_timeout_get_data,
	kCall_dbus_timeout_set_data,
	kCall_dbus_timeout_handle,
	kCall_dbus_timeout_get_enabled,
	kCall_dbus_bus_get,
	kCall_dbus_bus_get_private,
	kCall_dbus_bus_register,
	kCall_dbus_bus_set_unique_name,
	kCall_dbus_bus_get_unique_name,
	kCall_dbus_bus_get_unix_user,
	kCall_dbus_bus_get_id,
	kCall_dbus_bus_request_name,
	kCall_dbus_bus_release_name,
	kCall_dbus_bus_name_has_owner,
	kCall_dbus_bus_start_service_by_name,
	kCall_dbus_bus_add_match,
	kCall_dbus_bus_remove_match,
	kCall_dbus_get_local_machine_id,
	kCall_dbus_get_version,
	kCall_dbus_setenv,
	kCall_dbus_try_get_local_machine_id,
	kCall_dbus_pending_call_ref,
	kCall_dbus_pending_call_unref,
	kCall_dbus_pending_call_set_notify,
	kCall_dbus_pending_call_cancel,
	kCall_dbus_pending_call_get_completed,
	kCall_dbus_pending_call_steal_reply,
	kCall_dbus_pending_call_block,
	kCall_dbus_pending_call_allocate_data_slot,
	kCall_dbus_pending_call_free_data_slot,
	kCall_dbus_pending_call_set_data,
	kCall_dbus_pending_call_get_data,
	kCall_dbus_server_listen,
	kCall_dbus_server_ref,
	kCall_dbus_server_unref,
	kCall_dbus_server_disconnect,
	kCall_dbus_server_get_is_connected,
	kCall_dbus_server_get_address,
	kCall_dbus_server_get_id,
	kCall_dbus_server_set_new_connection_function,
	kCall_dbus_server_set_watch_functions,
	kCall_dbus_server_set_timeout_functions,
	kCall_dbus_server_set_auth_mechanisms,
	kCall_dbus_server_allocate_data_slot,
	kCall_dbus_server_free_data_slot,
	kCall_dbus_server_set_data,
	kCall_dbus_server_get_data,
	kCall_dbus_signature_iter_init,
	kCall_dbus_signature_iter_get_current_type,
	kCall_dbus_signature_iter_get_signature,
	kCall_dbus_signature_iter_get_element_type,
	kCall_dbus_signature_iter_next,
	kCall_dbus_signature_iter_recurse,
	kCall_dbus_signature_validate,
	kCall_dbus_signature_validate_single,
	kCall_dbus_type_is_valid,
	kCall_dbus_type_is_basic,
	kCall_dbus_type_is_container,
	kCall_dbus_type_is_fixed,
	kCall_dbus_validate_path,
	kCall_dbus_validate_interface,
	kCall_dbus_validate_member,
	kCall_dbus_validate_error_name,
	kCall_dbus_validate_bus_name,
	kCall_dbus_validate_utf8,
	kCall_dbus_threads_init,
	kCall_dbus_threads_init_default,
	/** Total number of function calls to be managed */
	kCallIndexCount
};

struct Function_t {
	/** Name of the function for loading from the library */
	const char* m_pName;
	/** Location of the function pointer for redirection */
	void* m_pGlobalFunction;
};

// Names of all the DBus functions that have been shimmed
static Function_t s_CallNames[kCallIndexCount] = {
	{"dbus_error_init", &Burger::Unix::dbus_error_init},
	{"dbus_error_free", &Burger::Unix::dbus_error_free},
	{"dbus_set_error", &Burger::Unix::dbus_set_error},
	{"dbus_set_error_const", &Burger::Unix::dbus_set_error_const},
	{"dbus_move_error", &Burger::Unix::dbus_move_error},
	{"dbus_error_has_name", &Burger::Unix::dbus_error_has_name},
	{"dbus_error_is_set", &Burger::Unix::dbus_error_is_set},

	{"dbus_parse_address", &Burger::Unix::dbus_parse_address},
	{"dbus_address_entry_get_value",
		&Burger::Unix::dbus_address_entry_get_value},
	{"dbus_address_entry_get_method",
		&Burger::Unix::dbus_address_entry_get_method},
	{"dbus_address_entries_free", &Burger::Unix::dbus_address_entries_free},
	{"dbus_address_escape_value", &Burger::Unix::dbus_address_escape_value},
	{"dbus_address_unescape_value", &Burger::Unix::dbus_address_unescape_value},

	{"dbus_malloc", &Burger::Unix::dbus_malloc},
	{"dbus_malloc0", &Burger::Unix::dbus_malloc0},
	{"dbus_realloc", &Burger::Unix::dbus_realloc},
	{"dbus_free", &Burger::Unix::dbus_free},
	{"dbus_free_string_array", &Burger::Unix::dbus_free_string_array},
	{"dbus_shutdown", &Burger::Unix::dbus_shutdown},

	{"dbus_message_new", &Burger::Unix::dbus_message_new},
	{"dbus_message_new_method_call",
		&Burger::Unix::dbus_message_new_method_call},
	{"dbus_message_new_method_return",
		&Burger::Unix::dbus_message_new_method_return},
	{"dbus_message_new_signal", &Burger::Unix::dbus_message_new_signal},
	{"dbus_message_new_error", &Burger::Unix::dbus_message_new_error},
	{"dbus_message_new_error_printf",
		&Burger::Unix::dbus_message_new_error_printf},
	{"dbus_message_copy", &Burger::Unix::dbus_message_copy},
	{"dbus_message_ref", &Burger::Unix::dbus_message_ref},
	{"dbus_message_unref", &Burger::Unix::dbus_message_unref},
	{"dbus_message_get_type", &Burger::Unix::dbus_message_get_type},
	{"dbus_message_set_path", &Burger::Unix::dbus_message_set_path},
	{"dbus_message_get_path", &Burger::Unix::dbus_message_get_path},
	{"dbus_message_has_path", &Burger::Unix::dbus_message_has_path},
	{"dbus_message_set_interface", &Burger::Unix::dbus_message_set_interface},
	{"dbus_message_get_interface", &Burger::Unix::dbus_message_get_interface},
	{"dbus_message_has_interface", &Burger::Unix::dbus_message_has_interface},
	{"dbus_message_set_member", &Burger::Unix::dbus_message_set_member},
	{"dbus_message_get_member", &Burger::Unix::dbus_message_get_member},
	{"dbus_message_has_member", &Burger::Unix::dbus_message_has_member},
	{"dbus_message_set_error_name", &Burger::Unix::dbus_message_set_error_name},
	{"dbus_message_get_error_name", &Burger::Unix::dbus_message_get_error_name},
	{"dbus_message_set_destination",
		&Burger::Unix::dbus_message_set_destination},
	{"dbus_message_get_destination",
		&Burger::Unix::dbus_message_get_destination},
	{"dbus_message_set_sender", &Burger::Unix::dbus_message_set_sender},
	{"dbus_message_get_sender", &Burger::Unix::dbus_message_get_sender},
	{"dbus_message_get_signature", &Burger::Unix::dbus_message_get_signature},
	{"dbus_message_set_no_reply", &Burger::Unix::dbus_message_set_no_reply},
	{"dbus_message_get_no_reply", &Burger::Unix::dbus_message_get_no_reply},
	{"dbus_message_is_method_call", &Burger::Unix::dbus_message_is_method_call},
	{"dbus_message_is_signal", &Burger::Unix::dbus_message_is_signal},
	{"dbus_message_is_error", &Burger::Unix::dbus_message_is_error},
	{"dbus_message_has_destination",
		&Burger::Unix::dbus_message_has_destination},
	{"dbus_message_has_sender", &Burger::Unix::dbus_message_has_sender},
	{"dbus_message_has_signature", &Burger::Unix::dbus_message_has_signature},
	{"dbus_message_get_serial", &Burger::Unix::dbus_message_get_serial},
	{"dbus_message_set_serial", &Burger::Unix::dbus_message_set_serial},
	{"dbus_message_set_reply_serial",
		&Burger::Unix::dbus_message_set_reply_serial},
	{"dbus_message_get_reply_serial",
		&Burger::Unix::dbus_message_get_reply_serial},
	{"dbus_message_set_auto_start", &Burger::Unix::dbus_message_set_auto_start},
	{"dbus_message_get_auto_start", &Burger::Unix::dbus_message_get_auto_start},
	{"dbus_message_get_path_decomposed",
		&Burger::Unix::dbus_message_get_path_decomposed},
	{"dbus_message_append_args", &Burger::Unix::dbus_message_append_args},
	{"dbus_message_append_args_valist",
		&Burger::Unix::dbus_message_append_args_valist},
	{"dbus_message_get_args", &Burger::Unix::dbus_message_get_args},
	{"dbus_message_get_args_valist",
		&Burger::Unix::dbus_message_get_args_valist},
	{"dbus_message_contains_unix_fds",
		&Burger::Unix::dbus_message_contains_unix_fds},
	{"dbus_message_iter_init_closed",
		&Burger::Unix::dbus_message_iter_init_closed},
	{"dbus_message_iter_init", &Burger::Unix::dbus_message_iter_init},
	{"dbus_message_iter_has_next", &Burger::Unix::dbus_message_iter_has_next},
	{"dbus_message_iter_next", &Burger::Unix::dbus_message_iter_next},
	{"dbus_message_iter_get_signature",
		&Burger::Unix::dbus_message_iter_get_signature},
	{"dbus_message_iter_get_arg_type",
		&Burger::Unix::dbus_message_iter_get_arg_type},
	{"dbus_message_iter_get_element_type",
		&Burger::Unix::dbus_message_iter_get_element_type},
	{"dbus_message_iter_recurse", &Burger::Unix::dbus_message_iter_recurse},
	{"dbus_message_iter_get_basic", &Burger::Unix::dbus_message_iter_get_basic},
	{"dbus_message_iter_get_element_count",
		&Burger::Unix::dbus_message_iter_get_element_count},
	{"dbus_message_iter_get_array_len",
		&Burger::Unix::dbus_message_iter_get_array_len},
	{"dbus_message_iter_get_fixed_array",
		&Burger::Unix::dbus_message_iter_get_fixed_array},
	{"dbus_message_iter_init_append",
		&Burger::Unix::dbus_message_iter_init_append},
	{"dbus_message_iter_append_basic",
		&Burger::Unix::dbus_message_iter_append_basic},
	{"dbus_message_iter_append_fixed_array",
		&Burger::Unix::dbus_message_iter_append_fixed_array},
	{"dbus_message_iter_open_container",
		&Burger::Unix::dbus_message_iter_open_container},
	{"dbus_message_iter_close_container",
		&Burger::Unix::dbus_message_iter_close_container},
	{"dbus_message_iter_abandon_container",
		&Burger::Unix::dbus_message_iter_abandon_container},
	{"dbus_message_iter_abandon_container_if_open",
		&Burger::Unix::dbus_message_iter_abandon_container_if_open},
	{"dbus_message_lock", &Burger::Unix::dbus_message_lock},
	{"dbus_set_error_from_message", &Burger::Unix::dbus_set_error_from_message},
	{"dbus_message_allocate_data_slot",
		&Burger::Unix::dbus_message_allocate_data_slot},
	{"dbus_message_free_data_slot", &Burger::Unix::dbus_message_free_data_slot},
	{"dbus_message_set_data", &Burger::Unix::dbus_message_set_data},
	{"dbus_message_get_data", &Burger::Unix::dbus_message_get_data},
	{"dbus_message_type_from_string",
		&Burger::Unix::dbus_message_type_from_string},
	{"dbus_message_type_to_string", &Burger::Unix::dbus_message_type_to_string},
	{"dbus_message_marshal", &Burger::Unix::dbus_message_marshal},
	{"dbus_message_demarshal", &Burger::Unix::dbus_message_demarshal},
	{"dbus_message_demarshal_bytes_needed",
		&Burger::Unix::dbus_message_demarshal_bytes_needed},
	{"dbus_message_set_allow_interactive_authorization",
		&Burger::Unix::dbus_message_set_allow_interactive_authorization},
	{"dbus_message_get_allow_interactive_authorization",
		&Burger::Unix::dbus_message_get_allow_interactive_authorization},

	{"dbus_connection_open", &Burger::Unix::dbus_connection_open},
	{"dbus_connection_open_private",
		&Burger::Unix::dbus_connection_open_private},
	{"dbus_connection_ref", &Burger::Unix::dbus_connection_ref},
	{"dbus_connection_unref", &Burger::Unix::dbus_connection_unref},
	{"dbus_connection_close", &Burger::Unix::dbus_connection_close},
	{"dbus_connection_get_is_connected",
		&Burger::Unix::dbus_connection_get_is_connected},
	{"dbus_connection_get_is_authenticated",
		&Burger::Unix::dbus_connection_get_is_authenticated},
	{"dbus_connection_get_is_anonymous",
		&Burger::Unix::dbus_connection_get_is_anonymous},
	{"dbus_connection_get_server_id",
		&Burger::Unix::dbus_connection_get_server_id},
	{"dbus_connection_can_send_type",
		&Burger::Unix::dbus_connection_can_send_type},
	{"dbus_connection_set_exit_on_disconnect",
		&Burger::Unix::dbus_connection_set_exit_on_disconnect},
	{"dbus_connection_flush", &Burger::Unix::dbus_connection_flush},
	{"dbus_connection_read_write_dispatch",
		&Burger::Unix::dbus_connection_read_write_dispatch},
	{"dbus_connection_read_write", &Burger::Unix::dbus_connection_read_write},
	{"dbus_connection_borrow_message",
		&Burger::Unix::dbus_connection_borrow_message},
	{"dbus_connection_return_message",
		&Burger::Unix::dbus_connection_return_message},
	{"dbus_connection_steal_borrowed_message",
		&Burger::Unix::dbus_connection_steal_borrowed_message},
	{"dbus_connection_pop_message", &Burger::Unix::dbus_connection_pop_message},
	{"dbus_connection_get_dispatch_status",
		&Burger::Unix::dbus_connection_get_dispatch_status},
	{"dbus_connection_dispatch", &Burger::Unix::dbus_connection_dispatch},
	{"dbus_connection_has_messages_to_send",
		&Burger::Unix::dbus_connection_has_messages_to_send},
	{"dbus_connection_send", &Burger::Unix::dbus_connection_send},
	{"dbus_connection_send_with_reply",
		&Burger::Unix::dbus_connection_send_with_reply},
	{"dbus_connection_send_with_reply_and_block",
		&Burger::Unix::dbus_connection_send_with_reply_and_block},
	{"dbus_connection_set_watch_functions",
		&Burger::Unix::dbus_connection_set_watch_functions},
	{"dbus_connection_set_timeout_functions",
		&Burger::Unix::dbus_connection_set_timeout_functions},
	{"dbus_connection_set_wakeup_main_function",
		&Burger::Unix::dbus_connection_set_wakeup_main_function},
	{"dbus_connection_set_dispatch_status_function",
		&Burger::Unix::dbus_connection_set_dispatch_status_function},
	{"dbus_connection_get_unix_user",
		&Burger::Unix::dbus_connection_get_unix_user},
	{"dbus_connection_get_unix_process_id",
		&Burger::Unix::dbus_connection_get_unix_process_id},
	{"dbus_connection_get_adt_audit_session_data",
		&Burger::Unix::dbus_connection_get_adt_audit_session_data},
	{"dbus_connection_set_unix_user_function",
		&Burger::Unix::dbus_connection_set_unix_user_function},
	{"dbus_connection_get_windows_user",
		&Burger::Unix::dbus_connection_get_windows_user},
	{"dbus_connection_set_windows_user_function",
		&Burger::Unix::dbus_connection_set_windows_user_function},
	{"dbus_connection_set_allow_anonymous",
		&Burger::Unix::dbus_connection_set_allow_anonymous},
	{"dbus_connection_set_route_peer_messages",
		&Burger::Unix::dbus_connection_set_route_peer_messages},
	{"dbus_connection_add_filter", &Burger::Unix::dbus_connection_add_filter},
	{"dbus_connection_remove_filter",
		&Burger::Unix::dbus_connection_remove_filter},
	{"dbus_connection_allocate_data_slot",
		&Burger::Unix::dbus_connection_allocate_data_slot},
	{"dbus_connection_free_data_slot",
		&Burger::Unix::dbus_connection_free_data_slot},
	{"dbus_connection_set_data", &Burger::Unix::dbus_connection_set_data},
	{"dbus_connection_get_data", &Burger::Unix::dbus_connection_get_data},
	{"dbus_connection_set_change_sigpipe",
		&Burger::Unix::dbus_connection_set_change_sigpipe},
	{"dbus_connection_set_max_message_size",
		&Burger::Unix::dbus_connection_set_max_message_size},
	{"dbus_connection_get_max_message_size",
		&Burger::Unix::dbus_connection_get_max_message_size},
	{"dbus_connection_set_max_received_size",
		&Burger::Unix::dbus_connection_set_max_received_size},
	{"dbus_connection_get_max_received_size",
		&Burger::Unix::dbus_connection_get_max_received_size},
	{"dbus_connection_set_max_message_unix_fds",
		&Burger::Unix::dbus_connection_set_max_message_unix_fds},
	{"dbus_connection_get_max_message_unix_fds",
		&Burger::Unix::dbus_connection_get_max_message_unix_fds},
	{"dbus_connection_set_max_received_unix_fds",
		&Burger::Unix::dbus_connection_set_max_received_unix_fds},
	{"dbus_connection_get_max_received_unix_fds",
		&Burger::Unix::dbus_connection_get_max_received_unix_fds},
	{"dbus_connection_get_outgoing_size",
		&Burger::Unix::dbus_connection_get_outgoing_size},
	{"dbus_connection_get_outgoing_unix_fds",
		&Burger::Unix::dbus_connection_get_outgoing_unix_fds},
	{"dbus_connection_preallocate_send",
		&Burger::Unix::dbus_connection_preallocate_send},
	{"dbus_connection_free_preallocated_send",
		&Burger::Unix::dbus_connection_free_preallocated_send},
	{"dbus_connection_send_preallocated",
		&Burger::Unix::dbus_connection_send_preallocated},
	{"dbus_connection_try_register_object_path",
		&Burger::Unix::dbus_connection_try_register_object_path},
	{"dbus_connection_register_object_path",
		&Burger::Unix::dbus_connection_register_object_path},
	{"dbus_connection_try_register_fallback",
		&Burger::Unix::dbus_connection_try_register_fallback},
	{"dbus_connection_register_fallback",
		&Burger::Unix::dbus_connection_register_fallback},
	{"dbus_connection_unregister_object_path",
		&Burger::Unix::dbus_connection_unregister_object_path},
	{"dbus_connection_get_object_path_data",
		&Burger::Unix::dbus_connection_get_object_path_data},
	{"dbus_connection_list_registered",
		&Burger::Unix::dbus_connection_list_registered},
	{"dbus_connection_get_unix_fd", &Burger::Unix::dbus_connection_get_unix_fd},
	{"dbus_connection_get_socket", &Burger::Unix::dbus_connection_get_socket},
	{"dbus_watch_get_fd", &Burger::Unix::dbus_watch_get_fd},
	{"dbus_watch_get_unix_fd", &Burger::Unix::dbus_watch_get_unix_fd},
	{"dbus_watch_get_socket", &Burger::Unix::dbus_watch_get_socket},
	{"dbus_watch_get_flags", &Burger::Unix::dbus_watch_get_flags},
	{"dbus_watch_get_data", &Burger::Unix::dbus_watch_get_data},
	{"dbus_watch_set_data", &Burger::Unix::dbus_watch_set_data},
	{"dbus_watch_handle", &Burger::Unix::dbus_watch_handle},
	{"dbus_watch_get_enabled", &Burger::Unix::dbus_watch_get_enabled},
	{"dbus_timeout_get_interval", &Burger::Unix::dbus_timeout_get_interval},
	{"dbus_timeout_get_data", &Burger::Unix::dbus_timeout_get_data},
	{"dbus_timeout_set_data", &Burger::Unix::dbus_timeout_set_data},
	{"dbus_timeout_handle", &Burger::Unix::dbus_timeout_handle},
	{"dbus_timeout_get_enabled", &Burger::Unix::dbus_timeout_get_enabled},

	{"dbus_bus_get", &Burger::Unix::dbus_bus_get},
	{"dbus_bus_get_private", &Burger::Unix::dbus_bus_get_private},
	{"dbus_bus_register", &Burger::Unix::dbus_bus_register},
	{"dbus_bus_set_unique_name", &Burger::Unix::dbus_bus_set_unique_name},
	{"dbus_bus_get_unique_name", &Burger::Unix::dbus_bus_get_unique_name},
	{"dbus_bus_get_unix_user", &Burger::Unix::dbus_bus_get_unix_user},
	{"dbus_bus_get_id", &Burger::Unix::dbus_bus_get_id},
	{"dbus_bus_request_name", &Burger::Unix::dbus_bus_request_name},
	{"dbus_bus_release_name", &Burger::Unix::dbus_bus_release_name},
	{"dbus_bus_name_has_owner", &Burger::Unix::dbus_bus_name_has_owner},
	{"dbus_bus_start_service_by_name",
		&Burger::Unix::dbus_bus_start_service_by_name},
	{"dbus_bus_add_match", &Burger::Unix::dbus_bus_add_match},
	{"dbus_bus_remove_match", &Burger::Unix::dbus_bus_remove_match},

	{"dbus_get_local_machine_id", &Burger::Unix::dbus_get_local_machine_id},
	{"dbus_get_version", &Burger::Unix::dbus_get_version},
	{"dbus_setenv", &Burger::Unix::dbus_setenv},
	{"dbus_try_get_local_machine_id",
		&Burger::Unix::dbus_try_get_local_machine_id},

	{"dbus_pending_call_ref", &Burger::Unix::dbus_pending_call_ref},
	{"dbus_pending_call_unref", &Burger::Unix::dbus_pending_call_unref},
	{"dbus_pending_call_set_notify",
		&Burger::Unix::dbus_pending_call_set_notify},
	{"dbus_pending_call_cancel", &Burger::Unix::dbus_pending_call_cancel},
	{"dbus_pending_call_get_completed",
		&Burger::Unix::dbus_pending_call_get_completed},
	{"dbus_pending_call_steal_reply",
		&Burger::Unix::dbus_pending_call_steal_reply},
	{"dbus_pending_call_block", &Burger::Unix::dbus_pending_call_block},
	{"dbus_pending_call_allocate_data_slot",
		&Burger::Unix::dbus_pending_call_allocate_data_slot},
	{"dbus_pending_call_free_data_slot",
		&Burger::Unix::dbus_pending_call_free_data_slot},
	{"dbus_pending_call_set_data", &Burger::Unix::dbus_pending_call_set_data},
	{"dbus_pending_call_get_data", &Burger::Unix::dbus_pending_call_get_data},

	{"dbus_server_listen", &Burger::Unix::dbus_server_listen},
	{"dbus_server_ref", &Burger::Unix::dbus_server_ref},
	{"dbus_server_unref", &Burger::Unix::dbus_server_unref},
	{"dbus_server_disconnect", &Burger::Unix::dbus_server_disconnect},
	{"dbus_server_get_is_connected",
		&Burger::Unix::dbus_server_get_is_connected},
	{"dbus_server_get_address", &Burger::Unix::dbus_server_get_address},
	{"dbus_server_get_id", &Burger::Unix::dbus_server_get_id},
	{"dbus_server_set_new_connection_function",
		&Burger::Unix::dbus_server_set_new_connection_function},
	{"dbus_server_set_watch_functions",
		&Burger::Unix::dbus_server_set_watch_functions},
	{"dbus_server_set_timeout_functions",
		&Burger::Unix::dbus_server_set_timeout_functions},
	{"dbus_server_set_auth_mechanisms",
		&Burger::Unix::dbus_server_set_auth_mechanisms},
	{"dbus_server_allocate_data_slot",
		&Burger::Unix::dbus_server_allocate_data_slot},
	{"dbus_server_free_data_slot", &Burger::Unix::dbus_server_free_data_slot},
	{"dbus_server_set_data", &Burger::Unix::dbus_server_set_data},
	{"dbus_server_get_data", &Burger::Unix::dbus_server_get_data},

	{"dbus_signature_iter_init", &Burger::Unix::dbus_signature_iter_init},
	{"dbus_signature_iter_get_current_type",
		&Burger::Unix::dbus_signature_iter_get_current_type},
	{"dbus_signature_iter_get_signature",
		&Burger::Unix::dbus_signature_iter_get_signature},
	{"dbus_signature_iter_get_element_type",
		&Burger::Unix::dbus_signature_iter_get_element_type},
	{"dbus_signature_iter_next", &Burger::Unix::dbus_signature_iter_next},
	{"dbus_signature_iter_recurse", &Burger::Unix::dbus_signature_iter_recurse},
	{"dbus_signature_validate", &Burger::Unix::dbus_signature_validate},
	{"dbus_signature_validate_single",
		&Burger::Unix::dbus_signature_validate_single},
	{"dbus_type_is_valid", &Burger::Unix::dbus_type_is_valid},
	{"dbus_type_is_basic", &Burger::Unix::dbus_type_is_basic},
	{"dbus_type_is_container", &Burger::Unix::dbus_type_is_container},
	{"dbus_type_is_fixed", &Burger::Unix::dbus_type_is_fixed},

	{"dbus_validate_path", &Burger::Unix::dbus_validate_path},
	{"dbus_validate_interface", &Burger::Unix::dbus_validate_interface},
	{"dbus_validate_member", &Burger::Unix::dbus_validate_member},
	{"dbus_validate_error_name", &Burger::Unix::dbus_validate_error_name},
	{"dbus_validate_bus_name", &Burger::Unix::dbus_validate_bus_name},
	{"dbus_validate_utf8", &Burger::Unix::dbus_validate_utf8},

	{"dbus_threads_init", &Burger::Unix::dbus_threads_init},
	{"dbus_threads_init_default", &Burger::Unix::dbus_threads_init_default}};

#endif

/*! ************************************

	\namespace Burger::Unix::DBus
	\brief Shim to call the posix/unix dbus library

	Many Posix based systems contain a shared library called dbus. This shim
	will automatically load in the library if needed and if present, will
	forward calls to the library through a set of shims.

	This way, high level code can call has_dbus() to determine if the library is
	present, and if so, use the functions within.

	\note DBus is NOT not included in Burgerlib. It must be installed
		separately. This shim exists because if this reason.

	\sa has_dbus()

***************************************/

/*! ************************************

	\brief Test if dbus is present

	Try to load the dbus library and return if it succeed or failed.

	A side effect of this function is that it will call DBus::init() so dbus
	will be initialized after this call.

	\note This function can be called multiple times. It will only load the
		library once.

	\sa DBus::init()

***************************************/

uint_t BURGER_API Burger::Unix::has_dbus(void) BURGER_NOEXCEPT
{
	if (!g_DBus.is_initialized()) {
		if (DBus::init()) {
			return FALSE;
		}
	}
	return TRUE;
}

/*! ************************************

	\brief Initialize the dbus library

	If the file ``libdbus-1.so.3`` exists, load it in and remap all function
	pointers to the functions within the library. **DO NOT CALL ANY DBUS
	FUNCTIONS BEFORE INITIALIZATION!**

	\sa shutdown(), Unix::has_dbus()

***************************************/

Burger::eError BURGER_API Burger::Unix::DBus::init(void) BURGER_NOEXCEPT
{
	// Check if already initialized
	eError uResult = kErrorNone;
	if (!g_DBus.is_initialized()) {

		// Load it in
		uResult = g_DBus.init("libdbus-1.so.3");
		if (!uResult) {
			uintptr_t uCount = BURGER_ARRAYSIZE(s_CallNames);
			const Function_t* pWork = s_CallNames;
			do {
				// Get the function from the dynamic library
				void* pFunction = g_DBus.get_function(pWork->m_pName);
				if (pFunction) {
					// Update the function pointer with this one
					*reinterpret_cast<void**>(pWork->m_pGlobalFunction) =
						pFunction;
				}
				++pWork;
			} while (--uCount);
		}
	}
	return uResult;
}

/*! ************************************

	\brief Shut down the dbus library

	If the dbus library was loaded, this function will unload it and set all the
	function pointers to \ref nullptr. **DO NOT CALL ANY DBUS FUNCTIONS AFTER
	CALLING THIS FUNCTION!**

	\sa init(), Unix::has_dbus()

***************************************/

void BURGER_API Burger::Unix::DBus::shutdown(void) BURGER_NOEXCEPT
{
	// Release the dynamic library
	g_DBus.shutdown();
	uintptr_t uCount = BURGER_ARRAYSIZE(s_CallNames);
	const Function_t* pWork = s_CallNames;
	do {
		// Zap the function pointers, since they are invalid
		*reinterpret_cast<void**>(pWork->m_pGlobalFunction) = nullptr;
		++pWork;
	} while (--uCount);
}

#endif
