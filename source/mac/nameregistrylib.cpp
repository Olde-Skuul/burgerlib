/***************************************

	MacOS Only!!!

	NameRegistry is available on macOS 9.0-9.2.2 only. It's not on MacOS X.
    When running in Carbon, the registery is not visible, so this glue will allow
    the registery to be used as is it's present when using the Carbon API.

	Copyright (c) 1999-2022 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brtypes.h"

#if defined(BURGER_POWERPC) && !defined(DOXYGEN)

#include "brcodelibrary.h"

#include <MacErrors.h>
#include <NameRegistry.h>

// ANSI "C" please...
#ifdef __cplusplus
extern "C" {
#endif

	// Insert the function prototypes, because NameRegistry.h will not define them
	// if compiling with Carbon

	extern OSStatus RegistryEntryIDCopy(const RegEntryID* src, RegEntryID* dst);
	extern OSStatus RegistryEntryIDDispose(RegEntryID* id);
	extern OSStatus RegistryCStrEntryToPath(const RegEntryID* entryID,
		RegCStrPathName* pathName, RegPathNameSize pathSize);
	extern OSStatus RegistryCStrEntryCreate(const RegEntryID* parentEntry,
		const RegCStrPathName* name, RegEntryID* newEntry);
	extern OSStatus RegistryEntryDelete(const RegEntryID* id);
	extern OSStatus RegistryEntryGetMod(
		const RegEntryID* entry, RegEntryModifiers* modifiers);
	extern OSStatus RegistryEntrySetMod(
		const RegEntryID* entry, RegEntryModifiers modifiers);
	extern OSStatus RegistryCStrEntryToName(const RegEntryID* entryID,
		RegEntryID* parentEntry, RegCStrEntryName* nameComponent, Boolean* done);
	extern OSStatus RegistryPropertySetMod(const RegEntryID* entry,
		const RegPropertyName* name, RegPropertyModifiers modifiers);
	extern OSStatus RegistryPropertyGetMod(const RegEntryID* entry,
		const RegPropertyName* name, RegPropertyModifiers* modifiers);
	extern OSStatus RegistryEntryIDInit(RegEntryID* id);
	extern OSStatus RegistryEntrySearch(RegEntryIter* cookie,
		RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done,
		const RegPropertyName* propertyName, const void* propertyValue,
		RegPropertyValueSize propertySize);
	extern OSStatus RegistryCStrEntryLookup(const RegEntryID* searchPointID,
		const RegCStrPathName* pathName, RegEntryID* foundEntry);
	extern OSStatus RegistryEntryIterateDispose(RegEntryIter* cookie);
	extern Boolean RegistryEntryIDCompare(
		const RegEntryID* id1, const RegEntryID* id2);
	extern OSStatus RegistryPropertySet(const RegEntryID* entryID,
		const RegPropertyName* propertyName, const void* propertyValue,
		RegPropertyValueSize propertySize);
	extern OSStatus RegistryPropertyIterate(
		RegPropertyIter* cookie, RegPropertyName* foundProperty, Boolean* done);
	extern OSStatus RegistryPropertyDelete(
		const RegEntryID* entryID, const RegPropertyName* propertyName);
	extern OSStatus RegistryEntryPropertyMod(RegEntryIter* cookie,
		RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done,
		RegPropertyModifiers matchingModifiers);
	extern OSStatus RegistryPropertyIterateDispose(RegPropertyIter* cookie);
	extern OSStatus RegistryEntryToPathSize(
		const RegEntryID* entryID, RegPathNameSize* pathSize);
	extern OSStatus RegistryEntryIterateCreate(RegEntryIter* cookie);
	extern OSStatus RegistryEntryMod(RegEntryIter* cookie,
		RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done,
		RegEntryModifiers matchingModifiers);
	extern OSStatus RegistryPropertyGet(const RegEntryID* entryID,
		const RegPropertyName* propertyName, void* propertyValue,
		RegPropertyValueSize* propertySize);
	extern OSStatus RegistryPropertyRename(const RegEntryID* entry,
		const RegPropertyName* oldName, const RegPropertyName* newName);
	extern OSStatus RegistryEntryIterateSet(
		RegEntryIter* cookie, const RegEntryID* startEntryID);
	extern OSStatus RegistryEntryCopy(RegEntryID* parentEntryID,
		RegEntryID* sourceDevice, RegEntryID* destDevice);
	extern OSStatus RegistryPropertyGetSize(const RegEntryID* entryID,
		const RegPropertyName* propertyName, RegPropertyValueSize* propertySize);
	extern OSStatus RegistryPropertyIterateCreate(
		const RegEntryID* entry, RegPropertyIter* cookie);
	extern OSStatus RegistryEntryIterate(RegEntryIter* cookie,
		RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done);

	struct Locals_t {
		/** If TRUE then it's ok to use the ROM */
		OSStatus m_uError;
		/** If TRUE then Gestalt was called */
		uint32_t m_bTested;

		// Function table
		OSStatus(*RegistryEntryIDCopy)(const RegEntryID* src, RegEntryID* dst);
		OSStatus(*RegistryEntryIDDispose)(RegEntryID* id);
		OSStatus(*RegistryCStrEntryToPath)(const RegEntryID* entryID,
			RegCStrPathName* pathName, RegPathNameSize pathSize);
		OSStatus(*RegistryCStrEntryCreate)(const RegEntryID* parentEntry,
			const RegCStrPathName* name, RegEntryID* newEntry);
		OSStatus(*RegistryEntryDelete)(const RegEntryID* id);
		OSStatus(*RegistryEntryGetMod)(
			const RegEntryID* entry, RegEntryModifiers* modifiers);
		OSStatus(*RegistryEntrySetMod)(
			const RegEntryID* entry, RegEntryModifiers modifiers);
		OSStatus(*RegistryCStrEntryToName)(const RegEntryID* entryID,
			RegEntryID* parentEntry, RegCStrEntryName* nameComponent,
			Boolean* done);
		OSStatus(*RegistryPropertySetMod)(const RegEntryID* entry,
			const RegPropertyName* name, RegPropertyModifiers modifiers);
		OSStatus(*RegistryPropertyGetMod)(const RegEntryID* entry,
			const RegPropertyName* name, RegPropertyModifiers* modifiers);
		OSStatus(*RegistryEntryIDInit)(RegEntryID* id);
		OSStatus(*RegistryEntrySearch)(RegEntryIter* cookie,
			RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done,
			const RegPropertyName* propertyName, const void* propertyValue,
			RegPropertyValueSize propertySize);
		OSStatus(*RegistryCStrEntryLookup)(const RegEntryID* searchPointID,
			const RegCStrPathName* pathName, RegEntryID* foundEntry);
		OSStatus(*RegistryEntryIterateDispose)(RegEntryIter* cookie);
		Boolean(*RegistryEntryIDCompare)(
			const RegEntryID* id1, const RegEntryID* id2);
		OSStatus(*RegistryPropertySet)(const RegEntryID* entryID,
			const RegPropertyName* propertyName, const void* propertyValue,
			RegPropertyValueSize propertySize);
		OSStatus(*RegistryPropertyIterate)(
			RegPropertyIter* cookie, RegPropertyName* foundProperty, Boolean* done);
		OSStatus(*RegistryPropertyDelete)(
			const RegEntryID* entryID, const RegPropertyName* propertyName);
		OSStatus(*RegistryEntryPropertyMod)(RegEntryIter* cookie,
			RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done,
			RegPropertyModifiers matchingModifiers);
		OSStatus(*RegistryPropertyIterateDispose)(RegPropertyIter* cookie);
		OSStatus(*RegistryEntryToPathSize)(
			const RegEntryID* entryID, RegPathNameSize* pathSize);
		OSStatus(*RegistryEntryIterateCreate)(RegEntryIter* cookie);
		OSStatus(*RegistryEntryMod)(RegEntryIter* cookie,
			RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done,
			RegEntryModifiers matchingModifiers);
		OSStatus(*RegistryPropertyGet)(const RegEntryID* entryID,
			const RegPropertyName* propertyName, void* propertyValue,
			RegPropertyValueSize* propertySize);
		OSStatus(*RegistryPropertyRename)(const RegEntryID* entry,
			const RegPropertyName* oldName, const RegPropertyName* newName);
		OSStatus(*RegistryEntryIterateSet)(
			RegEntryIter* cookie, const RegEntryID* startEntryID);
		OSStatus(*RegistryEntryCopy)(RegEntryID* parentEntryID,
			RegEntryID* sourceDevice, RegEntryID* destDevice);
		OSStatus(*RegistryPropertyGetSize)(const RegEntryID* entryID,
			const RegPropertyName* propertyName,
			RegPropertyValueSize* propertySize);
		OSStatus(*RegistryPropertyIterateCreate)(
			const RegEntryID* entry, RegPropertyIter* cookie);
		OSStatus(*RegistryEntryIterate)(RegEntryIter* cookie,
			RegEntryIterationOp relationship, RegEntryID* foundEntry,
			Boolean* done);
	};

	// Function dispatch table
	static Locals_t g_Locals;

	/***************************************

			This is a local routine

			Return TRUE if the CDM code is not fixed

	***************************************/

	static OSStatus BURGER_API CheckDispatchTable(Locals_t* pLocals)
	{
		// Was it tested?
		if (!pLocals->m_bTested) {
			// Assume ok
			pLocals->m_uError = kUnsupportedFunctionErr;

			Burger::CodeLibrary* pNameRegistryLib = Burger::GetNameRegistryLib();
			// Will succeed on PowerPC Macs using MacOS 8.1 through 9.2.2
			if (pNameRegistryLib) {
				pLocals->RegistryEntryIDCopy =
					(OSStatus(*)(const RegEntryID*, RegEntryID*))
					pNameRegistryLib->GetFunction("RegistryEntryIDCopy");

				pLocals->RegistryEntryIDDispose =
					(OSStatus(*)(RegEntryID * id))
					pNameRegistryLib->GetFunction("RegistryEntryIDDispose");

				pLocals->RegistryCStrEntryToPath =
					(OSStatus(*)(
						const RegEntryID*, RegCStrPathName*, RegPathNameSize))
					pNameRegistryLib->GetFunction("RegistryCStrEntryToPath");

				pLocals->RegistryCStrEntryCreate =
					(OSStatus(*)(const RegEntryID * parentEntry,
						const RegCStrPathName * name, RegEntryID * newEntry))
					pNameRegistryLib->GetFunction("RegistryCStrEntryCreate");

				pLocals->RegistryEntryDelete =
					(OSStatus(*)(const RegEntryID * id))
					pNameRegistryLib->GetFunction("RegistryEntryDelete");

				pLocals->RegistryEntryGetMod =
					(OSStatus(*)(
						const RegEntryID * entry, RegEntryModifiers * modifiers))
					pNameRegistryLib->GetFunction("RegistryEntryGetMod");

				pLocals->RegistryEntrySetMod =
					(OSStatus(*)(const RegEntryID*, RegEntryModifiers))
					pNameRegistryLib->GetFunction("RegistryEntrySetMod");

				pLocals->RegistryCStrEntryToName =
					(OSStatus(*)(const RegEntryID*, RegEntryID*, RegCStrEntryName*,
						Boolean*))
					pNameRegistryLib->GetFunction("RegistryCStrEntryToName");

				pLocals->RegistryPropertySetMod =
					(OSStatus(*)(const RegEntryID*, const RegPropertyName*,
						RegPropertyModifiers))
					pNameRegistryLib->GetFunction("RegistryPropertySetMod");

				pLocals->RegistryPropertyGetMod =
					(OSStatus(*)(const RegEntryID*, const RegPropertyName*,
						RegPropertyModifiers*))
					pNameRegistryLib->GetFunction("RegistryPropertyGetMod");

				pLocals->RegistryEntryIDInit =
					(OSStatus(*)(RegEntryID*))pNameRegistryLib->GetFunction(
						"RegistryEntryIDInit");

				pLocals->RegistryEntrySearch =
					(OSStatus(*)(RegEntryIter*, RegEntryIterationOp, RegEntryID*,
						Boolean*, const RegPropertyName*, const void*,
						RegPropertyValueSize propertySize))
					pNameRegistryLib->GetFunction("RegistryEntrySearch");

				pLocals->RegistryCStrEntryLookup =
					(OSStatus(*)(const RegEntryID*, const RegCStrPathName * pathName,
						RegEntryID*))
					pNameRegistryLib->GetFunction("RegistryCStrEntryLookup");

				pLocals->RegistryEntryIterateDispose =
					(OSStatus(*)(RegEntryIter*))pNameRegistryLib->GetFunction(
						"RegistryEntryIterateDispose");

				pLocals->RegistryEntryIDCompare =
					(Boolean(*)(const RegEntryID*, const RegEntryID*))
					pNameRegistryLib->GetFunction("RegistryEntryIDCompare");

				pLocals->RegistryPropertySet =
					(OSStatus(*)(const RegEntryID*, const RegPropertyName*,
						const void*, RegPropertyValueSize))
					pNameRegistryLib->GetFunction("RegistryPropertySet");

				pLocals->RegistryPropertyIterate =
					(OSStatus(*)(RegPropertyIter*, RegPropertyName*, Boolean*))
					pNameRegistryLib->GetFunction("RegistryPropertyIterate");

				pLocals->RegistryPropertyDelete =
					(OSStatus(*)(const RegEntryID*, const RegPropertyName*))
					pNameRegistryLib->GetFunction("RegistryPropertyDelete");

				pLocals->RegistryEntryPropertyMod =
					(OSStatus(*)(RegEntryIter*, RegEntryIterationOp, RegEntryID*,
						Boolean*, RegPropertyModifiers))
					pNameRegistryLib->GetFunction("RegistryEntryPropertyMod");

				pLocals->RegistryPropertyIterateDispose =
					(OSStatus(*)(RegPropertyIter*))pNameRegistryLib->GetFunction(
						"RegistryPropertyIterateDispose");

				pLocals->RegistryEntryToPathSize =
					(OSStatus(*)(const RegEntryID*, RegPathNameSize*))
					pNameRegistryLib->GetFunction("RegistryEntryToPathSize");

				pLocals->RegistryEntryIterateCreate =
					(OSStatus(*)(RegEntryIter*))pNameRegistryLib->GetFunction(
						"RegistryEntryIterateCreate");

				pLocals->RegistryEntryMod =
					(OSStatus(*)(RegEntryIter*, RegEntryIterationOp, RegEntryID*,
						Boolean*, RegEntryModifiers))
					pNameRegistryLib->GetFunction("RegistryEntryMod");

				pLocals->RegistryPropertyGet =
					(OSStatus(*)(const RegEntryID*, const RegPropertyName*, void*,
						RegPropertyValueSize*))
					pNameRegistryLib->GetFunction("RegistryPropertyGet");

				pLocals->RegistryPropertyRename =
					(OSStatus(*)(const RegEntryID*, const RegPropertyName*,
						const RegPropertyName*))
					pNameRegistryLib->GetFunction("RegistryPropertyRename");

				pLocals->RegistryEntryIterateSet =
					(OSStatus(*)(RegEntryIter*, const RegEntryID*))
					pNameRegistryLib->GetFunction("RegistryEntryIterateSet");

				pLocals->RegistryEntryCopy = (OSStatus(*)(RegEntryID*, RegEntryID*,
					RegEntryID*))pNameRegistryLib->GetFunction("RegistryEntryCopy");

				pLocals->RegistryPropertyGetSize =
					(OSStatus(*)(const RegEntryID*, const RegPropertyName*,
						RegPropertyValueSize*))
					pNameRegistryLib->GetFunction("RegistryPropertyGetSize");

				pLocals->RegistryPropertyIterateCreate =
					(OSStatus(*)(const RegEntryID*, RegPropertyIter*))
					pNameRegistryLib->GetFunction(
						"RegistryPropertyIterateCreate");

				pLocals->RegistryEntryIterate = (OSStatus(*)(RegEntryIter*,
					RegEntryIterationOp, RegEntryID*,
					Boolean*))pNameRegistryLib->GetFunction("RegistryEntryIterate");

				pLocals->m_uError = 0;
			}

			// Don't call gestalt again
			pLocals->m_bTested = TRUE;
		}

		// Return the error code, or zero if everything is ready
		return pLocals->m_uError;
	}

	/***************************************

			Dispatch RegistryEntryIDCopy()

	***************************************/

	OSStatus RegistryEntryIDCopy(const RegEntryID* src, RegEntryID* dst)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryIDCopy(src, dst);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryIDDispose(RegEntryID* id)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryIDDispose(id);
		}
		return uOSStatus;
	}

	OSStatus RegistryCStrEntryToPath(const RegEntryID* entryID,
		RegCStrPathName* pathName, RegPathNameSize pathSize)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus =
				pLocals->RegistryCStrEntryToPath(entryID, pathName, pathSize);
		}
		return uOSStatus;
	}

	OSStatus RegistryCStrEntryCreate(const RegEntryID* parentEntry,
		const RegCStrPathName* name, RegEntryID* newEntry)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus =
				pLocals->RegistryCStrEntryCreate(parentEntry, name, newEntry);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryDelete(const RegEntryID* id)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryDelete(id);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryGetMod(
		const RegEntryID* entry, RegEntryModifiers* modifiers)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryGetMod(entry, modifiers);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntrySetMod(
		const RegEntryID* entry, RegEntryModifiers modifiers)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntrySetMod(entry, modifiers);
		}
		return uOSStatus;
	}

	OSStatus RegistryCStrEntryToName(const RegEntryID* entryID,
		RegEntryID* parentEntry, RegCStrEntryName* nameComponent, Boolean* done)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryCStrEntryToName(
				entryID, parentEntry, nameComponent, done);
		}
		return uOSStatus;
	}

	OSStatus RegistryPropertySetMod(const RegEntryID* entry,
		const RegPropertyName* name, RegPropertyModifiers modifiers)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryPropertySetMod(entry, name, modifiers);
		}
		return uOSStatus;
	}

	OSStatus RegistryPropertyGetMod(const RegEntryID* entry,
		const RegPropertyName* name, RegPropertyModifiers* modifiers)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryPropertyGetMod(entry, name, modifiers);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryIDInit(RegEntryID* id)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryIDInit(id);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntrySearch(RegEntryIter* cookie,
		RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done,
		const RegPropertyName* propertyName, const void* propertyValue,
		RegPropertyValueSize propertySize)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntrySearch(cookie, relationship,
				foundEntry, done, propertyName, propertyValue, propertySize);
		}
		return uOSStatus;
	}

	OSStatus RegistryCStrEntryLookup(const RegEntryID* searchPointID,
		const RegCStrPathName* pathName, RegEntryID* foundEntry)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryCStrEntryLookup(
				searchPointID, pathName, foundEntry);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryIterateDispose(RegEntryIter* cookie)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryIterateDispose(cookie);
		}
		return uOSStatus;
	}

	Boolean RegistryEntryIDCompare(const RegEntryID* id1, const RegEntryID* id2)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		Boolean uResult = FALSE;
		if (!uOSStatus) {
			uResult = pLocals->RegistryEntryIDCompare(id1, id2);
		}
		return uResult;
	}

	OSStatus RegistryPropertySet(const RegEntryID* entryID,
		const RegPropertyName* propertyName, const void* propertyValue,
		RegPropertyValueSize propertySize)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryPropertySet(
				entryID, propertyName, propertyValue, propertySize);
		}
		return uOSStatus;
	}

	OSStatus RegistryPropertyIterate(
		RegPropertyIter* cookie, RegPropertyName* foundProperty, Boolean* done)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus =
				pLocals->RegistryPropertyIterate(cookie, foundProperty, done);
		}
		return uOSStatus;
	}

	OSStatus RegistryPropertyDelete(
		const RegEntryID* entryID, const RegPropertyName* propertyName)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryPropertyDelete(entryID, propertyName);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryPropertyMod(RegEntryIter* cookie,
		RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done,
		RegPropertyModifiers matchingModifiers)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryPropertyMod(
				cookie, relationship, foundEntry, done, matchingModifiers);
		}
		return uOSStatus;
	}

	OSStatus RegistryPropertyIterateDispose(RegPropertyIter* cookie)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryPropertyIterateDispose(cookie);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryToPathSize(
		const RegEntryID* entryID, RegPathNameSize* pathSize)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryToPathSize(entryID, pathSize);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryIterateCreate(RegEntryIter* cookie)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryIterateCreate(cookie);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryMod(RegEntryIter* cookie,
		RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done,
		RegEntryModifiers matchingModifiers)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryMod(
				cookie, relationship, foundEntry, done, matchingModifiers);
		}
		return uOSStatus;
	}

	OSStatus RegistryPropertyGet(const RegEntryID* entryID,
		const RegPropertyName* propertyName, void* propertyValue,
		RegPropertyValueSize* propertySize)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryPropertyGet(
				entryID, propertyName, propertyValue, propertySize);
		}
		return uOSStatus;
	}

	OSStatus RegistryPropertyRename(const RegEntryID* entry,
		const RegPropertyName* oldName, const RegPropertyName* newName)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryPropertyRename(entry, oldName, newName);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryIterateSet(
		RegEntryIter* cookie, const RegEntryID* startEntryID)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryIterateSet(cookie, startEntryID);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryCopy(
		RegEntryID* parentEntryID, RegEntryID* sourceDevice, RegEntryID* destDevice)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus =
				pLocals->RegistryEntryCopy(parentEntryID, sourceDevice, destDevice);
		}
		return uOSStatus;
	}

	OSStatus RegistryPropertyGetSize(const RegEntryID* entryID,
		const RegPropertyName* propertyName, RegPropertyValueSize* propertySize)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryPropertyGetSize(
				entryID, propertyName, propertySize);
		}
		return uOSStatus;
	}

	OSStatus RegistryPropertyIterateCreate(
		const RegEntryID* entry, RegPropertyIter* cookie)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryPropertyIterateCreate(entry, cookie);
		}
		return uOSStatus;
	}

	OSStatus RegistryEntryIterate(RegEntryIter* cookie,
		RegEntryIterationOp relationship, RegEntryID* foundEntry, Boolean* done)
	{
		Locals_t* pLocals = &g_Locals;
		OSStatus uOSStatus = CheckDispatchTable(pLocals);
		if (!uOSStatus) {
			uOSStatus = pLocals->RegistryEntryIterate(
				cookie, relationship, foundEntry, done);
		}
		return uOSStatus;
	}

#ifdef __cplusplus
}
#endif

#endif
