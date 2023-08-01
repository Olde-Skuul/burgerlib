/***************************************

	Android version of GUID

	Copyright (c) 2022-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#define BURGER_NONEW

#include "brguid.h"

#if defined(BURGER_ANDROID) || defined(DOXYGEN)

#include "brendian.h"

#include <jni.h>

// Structure that has the functions needed to call Java
struct AndroidGuidCalls_t {
	void init(JNIEnv* pJNIEnv) BURGER_NOEXCEPT;

	// Java environment
	JNIEnv* m_pJNIEnv;

	// "java/util/UUID" instance
	jclass m_pUuidClass;

	// randomUUID()
	jmethodID m_pNewGuidMethod;

	// getMostSignificantBits()
	jmethodID m_pMostSignificantBitsMethod;

	// getLeastSignificantBits()
	jmethodID m_pLeastSignificantBitsMethod;
};

AndroidGuidCalls_t androidInfo;

void AndroidGuidCalls_t::init(JNIEnv* pJNIEnv) BURGER_NOEXCEPT
{
	// Save the Java environment
	m_pJNIEnv = pJNIEnv;

	// Get the UUID class reference
	jclass pLocalUuidClass = pJNIEnv->FindClass("java/util/UUID");
	m_pUuidClass = static_cast<jclass>(pJNIEnv->NewGlobalRef(pLocalUuidClass));
	pJNIEnv->DeleteLocalRef(pLocalUuidClass);

	// Get the UUID class methods needed
	m_pNewGuidMethod = pJNIEnv->GetStaticMethodID(
		m_pUuidClass, "randomUUID", "()Ljava/util/UUID;");
	m_pMostSignificantBitsMethod =
		pJNIEnv->GetMethodID(m_pUuidClass, "getMostSignificantBits", "()J");
	m_pLeastSignificantBitsMethod =
		pJNIEnv->GetMethodID(m_pUuidClass, "getLeastSignificantBits", "()J");
}

/***************************************

	\brief Create a new GUID

	Using a MAC address, current time and deep voodoo, generate a unique 128 bit
	number for labeling a data object.

	http://en.wikipedia.org/wiki/Globally_unique_identifier

	http://tools.ietf.org/html/rfc4122.html

	\param pOutput Pointer to an uninitialized GUID buffer
	\sa GUID_from_string() or GUID_to_string()

***************************************/

void BURGER_API Burger::GUID_init(GUID* pOutput) BURGER_NOEXCEPT
{
	JNIEnv* pJNIEnv = androidInfo.m_pJNIEnv;

	// pJavaUuid = randomUUID()
	jobject pJavaUuid = pJNIEnv->CallStaticObjectMethod(
		androidInfo.m_pUuidClass, androidInfo.m_pNewGuidMethod);

	// Extract the data
	int64_t iMostSignificant = pJNIEnv->CallLongMethod(
		pJavaUuid, androidInfo.m_pMostSignificantBitsMethod);
	int64_t iLeastSignificant = pJNIEnv->CallLongMethod(
		pJavaUuid, androidInfo.m_pLeastSignificantBitsMethod);

	// Release the UUID
	pJNIEnv->DeleteLocalRef(pJavaUuid);

	// Store the 16 byte value as big endian
	int64_t* pWork = reinterpret_cast<int64_t*>(pOutput);
	BigEndian::store_unaligned(pWork, iMostSignificant);
	BigEndian::store_unaligned(pWork + 1, iLeastSignificant);
}

#endif
