/***************************************

    Unit tests for the Network manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "testbrnetwork.h"
#include "brnetmanager.h"
#include "brmemoryfunctions.h"
#include "common.h"

struct IPv4StringTest_t {
	const char* m_pString; // Pointer to string in human readable form
	uint32_t m_uIP;		   // IPv4 IP address
	uint_t m_uPort;		   // Port
};

struct IPv6StringTest_t {
	const char* m_pString; // Pointer to string in human readable form
	uint8_t m_IP[16];		   // IPv6 IP address
	uint_t m_uPort;		   // Port
};

static const char* g_AddressTypes[] = {
	"Unknown", "IPv4", "IPv6", "IPX", "AppleTalk"};

/***************************************

	Display IP addresses and names

***************************************/

static void BURGER_API DisplayStats(Burger::NetworkManager* pNet)
{
	Message("NetworkManager::GetHostName() = %s", pNet->GetHostName());

	uint_t uAddressCount = pNet->GetLocalAddressCount();
	if (uAddressCount) {
		Burger::String TempStringNetAddr;
		const Burger::NetAddr_t* pNetAddr = pNet->GetLocalAddresses();
		do {
			pNetAddr->ToString(&TempStringNetAddr);
			Message("Local Address = %s", TempStringNetAddr.GetPtr());
			++pNetAddr;
		} while (--uAddressCount);
	}

	//
	// List all of the detected protocols
	//

	uAddressCount = Burger::NetAddr_t::TYPE_UNKNOWN;
	do {
		if (pNet->HasAddrType(
				static_cast<Burger::NetAddr_t::eAddressType>(uAddressCount))) {
			Message("Has protocol %s", g_AddressTypes[uAddressCount]);
		}
	} while (++uAddressCount < Burger::NetAddr_t::TYPE_COUNT);
}

/***************************************

	Test string to IPv4 to string

***************************************/

static const IPv4StringTest_t g_IPv4StringTests[] = {
	{"255.255.255.255:90", 0xFFFFFFFFU, 90},
	{"10.0.1.204", 0x0A0001CCU, BURGER_MAXUINT},
	{"10.0.1.204:80", 0x0A0001CCU, 80},
	{"127.0.0.1", 0x7F000001U, BURGER_MAXUINT}};

static uint_t BURGER_API TestIPv4ToString(void)
{
	Burger::String StringTest;
	uint_t uFailure = FALSE;
	const IPv4StringTest_t* pWork = g_IPv4StringTests;
	uintptr_t uCount = BURGER_ARRAYSIZE(g_IPv4StringTests);
	do {
		uint_t uTest = static_cast<uint_t>(
			Burger::IPv4ToString(&StringTest, pWork->m_uIP, pWork->m_uPort));
		if (!uTest) {
			uTest = StringTest.Compare(pWork->m_pString) != 0;
		}
		uFailure |= uTest;
		ReportFailure(
			"Burger::IPv4ToString(&StringTest,0x%08X,%u) = \"%s\", expected \"%s\"",
			uTest, pWork->m_uIP, pWork->m_uPort, StringTest.GetPtr(),
			pWork->m_pString);
		if (!uTest) {
			uint32_t IPv4;
			uint_t Port;
			uTest = static_cast<uint_t>(
				Burger::StringToIPv4(StringTest.GetPtr(), &IPv4, &Port));
			if (!uTest) {
				uTest = (IPv4 != pWork->m_uIP)
					|| ((pWork->m_uPort != BURGER_MAXUINT)
						   && (Port != pWork->m_uPort));
			}
			uFailure |= uTest;
			ReportFailure(
				"Burger::StringToIPv4(\"%s\") = 0x%08X,%u, expected 0x%08X,%u",
				uTest, StringTest.GetPtr(), IPv4, Port, pWork->m_uIP,
				pWork->m_uPort);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Test string to IPv6 to string

***************************************/

static const IPv6StringTest_t g_IPv6StringTests[] = {
	{"[::ffff:10.0.1.204]:80",
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 10, 0, 1, 204}, 80},
	{"::1", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, BURGER_MAXUINT},
	{"[::1]:90", {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}, 90},
	{"2001:db8:1f70::999:de8:7648:6e8",
		{0x20, 0x01, 0xd, 0xb8, 0x1f, 0x70, 0x0, 0x0, 0x9, 0x99, 0xd, 0xe8,
			0x76, 0x48, 0x6, 0xe8},
		BURGER_MAXUINT},
	{"[2001:d88:ac10:fe01::]:80",
		{0x20, 0x01, 0xd, 0x88, 0xac, 0x10, 0xFE, 0x01, 0x0, 0x0, 0x0, 0x0, 0x0,
			0x0, 0x0, 0x0},
		80},
	{"ff01::1", {0xFF, 0x01, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
		BURGER_MAXUINT},
	{"::ffff:10.0.1.204",
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 10, 0, 1, 204},
		BURGER_MAXUINT},
	{"[::ffff:10.0.1.204]:80",
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xff, 10, 0, 1, 204}, 80}};

static uint_t BURGER_API TestIPv6ToString(void)
{
	Burger::String StringTest;
	uint_t uFailure = FALSE;
    const IPv6StringTest_t *pWork = g_IPv6StringTests;
    uintptr_t uCount = BURGER_ARRAYSIZE(g_IPv6StringTests);
	do {
		uint_t uTest = static_cast<uint_t>(
			Burger::IPv6ToString(&StringTest, pWork->m_IP, pWork->m_uPort));
		if (!uTest) {
			uTest = StringTest.Compare(pWork->m_pString) != 0;
		}
		uFailure |= uTest;
		ReportFailure(
			"Burger::IPv6ToString(&StringTest,0x%08X,%u) = \"%s\", expected \"%s\"",
			uTest, pWork->m_IP[0], pWork->m_uPort, StringTest.GetPtr(),
			pWork->m_pString);
		if (!uTest) {
			uint8_t IPv6[16];
			uint_t Port;
			uTest = static_cast<uint_t>(
				Burger::StringToIPv6(StringTest.GetPtr(), IPv6, &Port));
			if (!uTest) {
				uTest = (Burger::MemoryCompare(IPv6, pWork->m_IP, 16)
					|| ((pWork->m_uPort != BURGER_MAXUINT)
						   && (Port != pWork->m_uPort)));
			}
			uFailure |= uTest;
			ReportFailure(
				"Burger::StringToIPv6(\"%s\") = 0x%08X,%u, expected 0x%08X,%u",
				uTest, StringTest.GetPtr(), IPv6[0], Port, pWork->m_IP[0],
				pWork->m_uPort);
		}
		++pWork;
	} while (--uCount);
	return uFailure;
}

/***************************************

	Resolve IPv4 addresses

***************************************/

static const char* g_TestIPv4Resolve[] = {
	"perforce.oldeskuul.com:80", "www.google.com:80",
	"www.sixxs.net" // Both IPv4 and IPv6
};

static void BURGER_API TestIPv4Resolve(Burger::NetworkManager* pNet)
{
	Burger::NetAddr_t IPAddress;
	Burger::String StringTest;

	const char** pWork = g_TestIPv4Resolve;
	uintptr_t uCount = BURGER_ARRAYSIZE(g_TestIPv4Resolve);
	do {
		uint_t uTest =
			static_cast<uint_t>(pNet->ResolveIPv4Address(&IPAddress, *pWork));
		if (!uTest) {
			Burger::IPv4ToString(
				&StringTest, IPAddress.U.IPv4.m_uIP, IPAddress.U.IPv4.m_uPort);
			Message("IPv4 %s resolved to IP %s", *pWork, StringTest.GetPtr());

		} else {
			ReportFailure("NetworkManager::ResolveIPv4Address(\"%s\") failed",
				uTest, *pWork);
		}
		++pWork;
	} while (--uCount);
}

/***************************************

	Resolve IPv6 addresses

***************************************/

static const char* g_TestIPv6Resolve[] = {
	"perforceipv6.oldeskuul.com:80", "ipv6.google.com:80",
	"www.sixxs.net" // Both IPv4 and IPv6
};

static void BURGER_API TestIPv6Resolve(Burger::NetworkManager* pNet)
{
	Burger::NetAddr_t IPAddress;
	Burger::String StringTest;

	const char** pWork = g_TestIPv6Resolve;
	uintptr_t uCount = BURGER_ARRAYSIZE(g_TestIPv6Resolve);
	do {
		uint_t uTest =
			static_cast<uint_t>(pNet->ResolveIPv6Address(&IPAddress, *pWork));
		if (!uTest) {
			Burger::IPv6ToString(
				&StringTest, IPAddress.U.IPv6.m_IP, IPAddress.U.IPv6.m_uPort);
			Message("IPv6 %s resolved to IP %s", *pWork, StringTest.GetPtr());
		} else {
			ReportFailure("NetworkManager::ResolveIPv6Address(\"%s\") failed",
				uTest, *pWork);
		}
		++pWork;
	} while (--uCount);
}

/***************************************

	Display host IPv4 address

***************************************/

static void BURGER_API TestGetIPv4Address(Burger::NetworkManager* pNet)
{
	Burger::NetAddr_t IPAddress;
	Burger::String StringTest;
	uint_t uTest = static_cast<uint_t>(pNet->GetIPv4Address(&IPAddress));
	if (!uTest) {
		Burger::IPv4ToString(&StringTest, IPAddress.U.IPv4.m_uIP);
		Message("IPv4 address for the host machine is %s", StringTest.GetPtr());
	} else {
		ReportFailure("NetworkManager::GetIPv4Address() failed", uTest);
	}
}

/***************************************

	Display host IPv6 address

***************************************/

static void BURGER_API TestGetIPv6Address(Burger::NetworkManager* pNet)
{
	Burger::NetAddr_t IPAddress;
	Burger::String StringTest;
	uint_t uTest = static_cast<uint_t>(pNet->GetIPv6Address(&IPAddress));
	if (!uTest) {
		Burger::IPv6ToString(&StringTest, IPAddress.U.IPv6.m_IP);
		Message("IPv6 address for the host machine is %s", StringTest.GetPtr());
	} else {
		ReportFailure("NetworkManager::GetIPv6Address() failed", uTest);
	}
}

/***************************************

	Perform the tests

***************************************/

int BURGER_API TestNetwork(uint_t uVerbose)
{
	if (uVerbose & VERBOSE_MSG) {
		Message("Running Network tests");
	}

	uint_t uResult = TestIPv4ToString();
	uResult |= TestIPv6ToString();
	if (uVerbose & VERBOSE_NETWORK) {
		Burger::NetworkManager Net;
		if (Net.Init() == Burger::kErrorNone) {

			DisplayStats(&Net);

			// IPv4 tests
			TestIPv4Resolve(&Net);
			TestGetIPv4Address(&Net);

			// IPv6 tests
#if defined(BURGER_MAC)
			Message("MacOS doesn't support IPv6");
#else
			TestIPv6Resolve(&Net);
			TestGetIPv6Address(&Net);
#endif

#if 0
			Burger::NetAddr_t NetAddress;
			NetAddress.m_uType = Burger::NetAddr_t::TYPE_IPV4;
			NetAddress.U.IPv4.m_uPort = 667;
			NetAddress.U.IPv4.m_uIP = (10<<24)+(1<<8)+(202);
			Message("Sent 1 %u",Net.SendPacket(&NetAddress,"Maccy UDP4 Burger",18));
			Message("Sent 1 %u",Net.SendStream(&NetAddress,"Maccy UDP6 Burger",18));
			Burger::String StringTest;
			Burger::IPv4ToString(&StringTest,NetAddress.U.IPv4.m_uIP,NetAddress.U.IPv4.m_uPort);
			Message("IPv4 address for the host machine is %s",StringTest.GetPtr());
#endif
		} else {
			uResult |= 10;
			ReportFailure("Network may not be supported", 10);
		}
	}
	if (!uResult && (uVerbose & VERBOSE_MSG)) {
		Message("Passed all Network tests!");
	}
	return static_cast<int>(uResult);
}
