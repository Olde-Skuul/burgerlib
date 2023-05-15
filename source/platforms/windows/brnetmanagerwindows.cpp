/***************************************

	Windows version of Burger::NetworkManager

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "brnetmanager.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brendian.h"
#include "brmemoryfunctions.h"

#if !defined(DOXYGEN)

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(NTDDI_VERSION)
#define NTDDI_VERSION 0x05010000
#endif

#include <Windows.h>

#include <WinSock2.h>

#include <WS2tcpip.h>
#include <WSipx.h>
#include <IPHlpApi.h>

#include <iptypes.h>

// Force linking in Winsock2 and internet helpers
#if defined(BURGER_WATCOM)
#pragma library("IPHLPAPI.lib");
#pragma library("ws2_32.lib");
#else
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "ws2_32.lib")
#endif

// Appletalk defines are not present in Watcom

// #if !defined(BURGER_WATCOM) && !defined(BURGER_ARM)
//  #include <AtalkWsh.h>
// #else

struct sockaddr_at {
	USHORT sat_family;
	USHORT sat_net;
	UCHAR sat_node;
	UCHAR sat_socket;
};
// #endif

#endif

#if !defined(DOXYGEN)

#if !defined(GAA_FLAG_SKIP_MULTICAST)
#define GAA_FLAG_SKIP_MULTICAST 0x0004
#endif

#if !defined(GAA_FLAG_SKIP_DNS_SERVER)
#define GAA_FLAG_SKIP_DNS_SERVER 0x0008
#endif

#if !defined(GAA_FLAG_INCLUDE_PREFIX)
#define GAA_FLAG_INCLUDE_PREFIX 0x0010
#endif

#if !defined(GAA_FLAG_SKIP_FRIENDLY_NAME)
#define GAA_FLAG_SKIP_FRIENDLY_NAME 0x0020
#endif

//
// Union of all supported socket types
//
union HybridSocketAddr_t {
	sockaddr m_Base;
	sockaddr_in m_IPv4;
	sockaddr_in6 m_IPv6;
	sockaddr_ipx m_IPX;
	sockaddr_at m_AppleTalk;
	sockaddr_storage m_Storage;
};

static const int g_Protocols[8] = {AF_INET, IPPROTO_UDP, AF_INET6, IPPROTO_UDP,
	AF_IPX, NSPROTO_IPX, AF_APPLETALK, 0};

#endif

/*! ************************************

	\brief Convert a NetAddr_t into a sockaddr

	Convert a NetAddr_t into a sockaddr structure.

	Supports AppleTalk, IPX, IPv4 and IPv6

	\windowsonly

	\param pOutput Pointer to a sockaddr to receive the socket address

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa FromSocketAddr(const sockaddr *), ToSocketAddr(sockaddr_at *) const,
		ToSocketAddr(sockaddr_ipx *) const, ToSocketAddr(sockaddr_in *) const or
		ToSocketAddr(sockaddr_in6 *) const

***************************************/

Burger::eError BURGER_API Burger::NetAddr_t::ToSocketAddr(
	sockaddr* pOutput) const
{
	eError uResult = kErrorNone;
	switch (m_uType) {

	case TYPE_IPV4:
		pOutput->sa_family = AF_INET;
		reinterpret_cast<sockaddr_in*>(pOutput)->sin_port =
			BigEndian::Load(static_cast<uint16_t>(U.IPv4.m_uPort));
		reinterpret_cast<sockaddr_in*>(pOutput)->sin_addr.s_addr =
			BigEndian::Load(U.IPv4.m_uIP);
		MemoryClear(reinterpret_cast<sockaddr_in*>(pOutput)->sin_zero,
			sizeof(reinterpret_cast<sockaddr_in*>(pOutput)->sin_zero));
		break;

	case TYPE_IPV6:
		pOutput->sa_family = AF_INET6;
		reinterpret_cast<sockaddr_in6*>(pOutput)->sin6_port =
			BigEndian::Load(static_cast<uint16_t>(U.IPv6.m_uPort));
		reinterpret_cast<sockaddr_in6*>(pOutput)->sin6_flowinfo = 0;
		MemoryCopy(reinterpret_cast<sockaddr_in6*>(pOutput)->sin6_addr.u.Byte,
			U.IPv6.m_IP,
			sizeof(reinterpret_cast<sockaddr_in6*>(pOutput)->sin6_addr.u.Byte));
		reinterpret_cast<sockaddr_in6*>(pOutput)->sin6_scope_id = 0;
		break;

	case TYPE_IPX:
		pOutput->sa_family = AF_IPX;
		reinterpret_cast<sockaddr_ipx*>(pOutput)->sa_socket =
			BigEndian::Load(static_cast<uint16_t>(U.IPX.m_uSocket));
		MemoryCopy(reinterpret_cast<sockaddr_ipx*>(pOutput)->sa_netnum,
			U.IPX.m_Net, 4);
		MemoryCopy(reinterpret_cast<sockaddr_ipx*>(pOutput)->sa_nodenum,
			U.IPX.m_Node, 6);
		break;

	case TYPE_APPLETALK:
		pOutput->sa_family = AF_APPLETALK;
		reinterpret_cast<sockaddr_at*>(pOutput)->sat_net =
			BigEndian::Load(static_cast<uint16_t>(U.APPLETALK.m_uNetwork));
		reinterpret_cast<sockaddr_at*>(pOutput)->sat_node =
			static_cast<uint8_t>(U.APPLETALK.m_uNodeID);
		reinterpret_cast<sockaddr_at*>(pOutput)->sat_socket =
			static_cast<uint8_t>(U.APPLETALK.m_uSocket);
		break;

	// Unknown
	default:
		MemoryClear(pOutput, sizeof(sockaddr));
		uResult = kErrorInvalidParameter;
		break;
	}
	return uResult;
}

/*! ************************************

	\fn Burger::eError Burger::NetAddr_t::ToSocketAddr(
		sockaddr_at *pOutput) const

	\brief Calls ToSocketAddr(sockaddr *) const

	Convenience routine to map a sockaddr_at to a sockaddr to call
	ToSocketAddr(sockaddr *) const

	\windowsonly

	\param pOutput Pointer to a sockaddr_at to receive the socket address

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa ToSocketAddr(sockaddr *) const

***************************************/

/*! ************************************

	\fn Burger::eError Burger::NetAddr_t::ToSocketAddr(
		sockaddr_in *pOutput) const
	\brief Calls ToSocketAddr(sockaddr *) const

	Convenience routine to map a sockaddr_in to a sockaddr to call
	ToSocketAddr(sockaddr *) const

	\windowsonly

	\param pOutput Pointer to a sockaddr_in to receive the socket address

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa ToSocketAddr(sockaddr *) const

***************************************/

/*! ************************************

	\fn Burger::eError Burger::NetAddr_t::ToSocketAddr(
		sockaddr_in6 *pOutput) const
	\brief Calls ToSocketAddr(sockaddr *) const

	Convenience routine to map a sockaddr_in6 to a sockaddr to call
	ToSocketAddr(sockaddr *) const

	\windowsonly

	\param pOutput Pointer to a sockaddr_in6 to receive the socket address

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa ToSocketAddr(sockaddr *) const

***************************************/

/*! ************************************

	\fn Burger::eError Burger::NetAddr_t::ToSocketAddr(
		sockaddr_ipx *pOutput) const
	\brief Calls ToSocketAddr(sockaddr *) const

	Convenience routine to map a sockaddr_ipx to a sockaddr to call
	ToSocketAddr(sockaddr *) const

	\windowsonly

	\param pOutput Pointer to a sockaddr_ipx to receive the socket address

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa ToSocketAddr(sockaddr *) const

***************************************/

/*! ************************************

	\fn Burger::eError Burger::NetAddr_t::ToSocketAddr(
		sockaddr_storage *pOutput) const
	\brief Calls ToSocketAddr(sockaddr *) const

	Convenience routine to map a sockaddr_storage to a sockaddr to call
	ToSocketAddr(sockaddr *) const

	\windowsonly

	\param pOutput Pointer to a sockaddr_storage to receive the socket address

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol
	\sa ToSocketAddr(sockaddr *) const

***************************************/

/*! ************************************

	\brief Convert a sockaddr into a NetAddr_t

	Convert a sockaddr into a NetAddr_t structure.

	Supports AppleTalk, IPX, IPv4 and IPv6

	\windowsonly

	\param pInput Pointer to a sockaddr to read the socket address from

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa ToSocketAddr(sockaddr *) const, FromSocketAddr(const sockaddr_at*),
		FromSocketAddr(const sockaddr_ipx *), FromSocketAddr(
		const sockaddr_in*) or FromSocketAddr(const sockaddr_in6 *)

***************************************/

Burger::eError BURGER_API Burger::NetAddr_t::FromSocketAddr(
	const sockaddr* pInput)
{
	eError uResult = kErrorNone;
	switch (pInput->sa_family) {

	case AF_INET:
		m_uType = TYPE_IPV4;
		U.IPv4.m_uPort = BigEndian::Load(
			reinterpret_cast<const sockaddr_in*>(pInput)->sin_port);
		U.IPv4.m_uIP = BigEndian::Load(static_cast<uint32_t>(
			reinterpret_cast<const sockaddr_in*>(pInput)->sin_addr.s_addr));
		break;

	case AF_INET6:
		m_uType = TYPE_IPV6;
		U.IPv6.m_uPort = BigEndian::Load(
			reinterpret_cast<const sockaddr_in6*>(pInput)->sin6_port);
		MemoryCopy(U.IPv6.m_IP,
			reinterpret_cast<const sockaddr_in6*>(pInput)->sin6_addr.u.Byte,
			sizeof(U.IPv6.m_IP));
		break;

	case AF_IPX:
		m_uType = TYPE_IPX;
		U.IPX.m_uSocket = BigEndian::Load(
			reinterpret_cast<const sockaddr_ipx*>(pInput)->sa_socket);
		MemoryCopy(U.IPX.m_Net,
			reinterpret_cast<const sockaddr_ipx*>(pInput)->sa_netnum, 4);
		MemoryCopy(U.IPX.m_Node,
			reinterpret_cast<const sockaddr_ipx*>(pInput)->sa_nodenum, 6);
		break;

	case AF_APPLETALK:
		m_uType = TYPE_APPLETALK;
		U.APPLETALK.m_uNetwork = BigEndian::Load(
			reinterpret_cast<const sockaddr_at*>(pInput)->sat_net);
		U.APPLETALK.m_uNodeID =
			reinterpret_cast<const sockaddr_at*>(pInput)->sat_node;
		U.APPLETALK.m_uSocket =
			reinterpret_cast<const sockaddr_at*>(pInput)->sat_socket;
		break;

		// Unknown
	default:
		MemoryClear(this, sizeof(*this));
		uResult = kErrorInvalidParameter;
		break;
	}
	return uResult;
}

/*! ************************************

	\fn Burger::eError Burger::NetAddr_t::FromSocketAddr(
		const sockaddr_in *pInput)
	\brief Calls FromSocketAddr(const sockaddr *)

	Convenience routine to map a sockaddr_in to a sockaddr to call
	FromSocketAddr(const sockaddr *)

	\windowsonly

	\param pInput Pointer to a sockaddr_in to read the socket address from

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa FromSocketAddr(const sockaddr *)

***************************************/

/*! ************************************

	\fn Burger::eError Burger::NetAddr_t::FromSocketAddr(
		const sockaddr_in6 *pInput)
	\brief Calls FromSocketAddr(const sockaddr *)

	Convenience routine to map a sockaddr_in6 to a sockaddr to call
	FromSocketAddr(const sockaddr *)

	\windowsonly

	\param pInput Pointer to a sockaddr_in6 to read the socket address from

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa FromSocketAddr(const sockaddr *)

***************************************/

/*! ************************************

	\fn Burger::eError Burger::NetAddr_t::FromSocketAddr(
		const sockaddr_at *pInput)
	\brief Calls FromSocketAddr(const sockaddr *)

	Convenience routine to map a sockaddr_at to a sockaddr to call
	FromSocketAddr(const sockaddr *)

	\windowsonly

	\param pInput Pointer to a sockaddr_at to read the socket address from

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa FromSocketAddr(const sockaddr *)

***************************************/

/*! ************************************

	\fn Burger::eError Burger::NetAddr_t::FromSocketAddr(
		const sockaddr_ipx *pInput)
	\brief Calls FromSocketAddr(const sockaddr *)

	Convenience routine to map a sockaddr_ipx to a sockaddr to call
	FromSocketAddr(const sockaddr *)

	\windowsonly

	\param pInput Pointer to a sockaddr_ipx to read the socket address from

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa FromSocketAddr(const sockaddr *)

***************************************/

/*! ************************************

	\fn Burger::eError Burger::NetAddr_t::FromSocketAddr(
		const sockaddr_storage *pInput)
	\brief Calls FromSocketAddr(const sockaddr *)

	Convenience routine to map a sockaddr_storage to a sockaddr to call
	FromSocketAddr(const sockaddr *)

	\windowsonly

	\param pInput Pointer to a sockaddr_storage to read the socket address from

	\return Zero if no error, non-zero if not supported or unimplemented
		protocol

	\sa FromSocketAddr(const sockaddr *)

***************************************/

/***************************************

	\brief Initialize network code

	Start up the operating system's network layer and return
	any error code if it failed.

	\return Zero on success, non-zero if an error had occurred

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::Init(void) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;

	// Already initialized?
	if (!m_bWSAStarted) {
		WSADATA WSAData;
		uResult = static_cast<eError>(WSAStartup(0x0202, &WSAData));
		if (uResult == kErrorNone) {
			//  Started just fine!
			m_bWSAStarted = TRUE;

			// Iterate over the protocols to see which ones are
			// actually available

			uint_t i = 0;
			do {
				SocketType uSocket =
					socket(g_Protocols[i], SOCK_DGRAM, g_Protocols[i + 1]);
				if (uSocket != INVALID_SOCKET) {
					m_uProtocolsFound |= 1U
						<< ((i >> 1U) + NetAddr_t::TYPE_IPV4);
					closesocket(uSocket);
				}
				i += 2;
			} while (i < BURGER_ARRAYSIZE(g_Protocols));

			// Obtain the host name

			// Microsoft says 256 is good enough, but I'm paranoid
			// https://msdn.microsoft.com/en-us/library/windows/desktop/ms738527(v=vs.85).aspx
			char TempHostName[512];
			if (!gethostname(TempHostName, sizeof(TempHostName))) {
				m_HostName.assign(TempHostName);
			}

			// Obtain the local addresses
			EnumerateLocalAddresses();
			++m_uStarted;
		}
	} else {
		// Increment the startup counter
		++m_uStarted;
	}
	// Zero on success
	return uResult;
}

/***************************************

	\brief Shut down network code

	Shut down the operating system's network layer and
	release all allocated resources

***************************************/

void BURGER_API Burger::NetworkManager::Shutdown(void) BURGER_NOEXCEPT
{
	// Release WinSock2 when counted down to zero
	if (m_uStarted) {
		if (!--m_uStarted) {
			if (m_bWSAStarted) {
				WSACleanup();
				m_bWSAStarted = FALSE;
			}

			// Release the local addresses
			Free(m_pLocalAddresses);
			m_pLocalAddresses = NULL;
			m_uLocalAddressCount = 0;
		}
	}
}

/***************************************

	\brief Resolve a DNS address

	Given a DNS string with an optional port number, contact
	the connected DNS server to resolve the address.

	\note If the DNS address is a port literal, it will be decoded without
		contacting any server.

	\param pOutput Pointer to the NetAddr_t structure that will contain the
		resolved address
	\param pDNSName Pointer to a "C" string in UTF-8 encoding to pass to the DNS
		server

	\return Zero if no error, non zero if the string couldn't be resolved.

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::ResolveIPv4Address(
	NetAddr_t* pOutput, const char* pDNSName) BURGER_NOEXCEPT
{
	// Clear the output
	MemoryClear(pOutput, sizeof(*pOutput));
	eError uResult = kErrorInvalidParameter;
	if (m_bWSAStarted) {

		// Make a copy of the DNS Name
		String TempDNS(pDNSName);

		// Valid string?
		if (TempDNS.IsValid()) {

			// Was there a port #?

			// Scan for the ending colon
			char* pColon = StringCharacterReverse(TempDNS.c_str(), ':');
			uResult = kErrorNone;
			if (pColon) {
				pColon[0] = 0; // Force a null string
				uint_t uPort = AsciiToInteger(pColon + 1);
				if (uPort < 65536) {
					pOutput->U.IPv4.m_uPort = uPort;
				} else {
					uResult = kErrorInvalidParameter;
				}
			}

			// Port not found or in bounds
			if (uResult == kErrorNone) {

				// Try parsing as a string
				uint32_t uIPv4;

				// Try 206.55.132.154
				uResult = StringToIPv4(TempDNS.c_str(), &uIPv4);
				if (uResult != kErrorNone) {

					// Try www.oldeskuul.com
					addrinfo Hints;
					MemoryClear(&Hints, sizeof(addrinfo));
					Hints.ai_family = PF_INET;
					// Hints.ai_socktype = SOCK_STREAM;

					addrinfo* pResult = NULL;
					uResult = static_cast<eError>(
						getaddrinfo(TempDNS.c_str(), NULL, &Hints, &pResult));
					if (uResult == kErrorNone) {
						uIPv4 = BigEndian::Load(static_cast<uint32_t>(
							reinterpret_cast<sockaddr_in*>(pResult->ai_addr)
								->sin_addr.s_addr));
						// Release the addrinfo chain
						freeaddrinfo(pResult);
					}
				}

				// No error? Store the result
				if (uResult == kErrorNone) {
					pOutput->m_uType = NetAddr_t::TYPE_IPV4;
					pOutput->U.IPv4.m_uIP = uIPv4;
				}
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Resolve a DNS address to IPv6

	Given a DNS string with an optional port number, contact
	the connected DNS server to resolve the address.

	\note If the DNS address is a port literal, it will be decoded without
		contacting any server.

	\param pOutput Pointer to the NetAddr_t structure that will contain the
		resolved address
	\param pDNSName Pointer to a "C" string in UTF-8 encoding to pass to the DNS
		server

	\return Zero if no error, non zero if the string couldn't be resolved.

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::ResolveIPv6Address(
	NetAddr_t* pOutput, const char* pDNSName) BURGER_NOEXCEPT
{
	// Clear the output
	MemoryClear(pOutput, sizeof(*pOutput));
	eError uResult = kErrorInvalidParameter;
	if (m_bWSAStarted) {

		// Make a copy of the DNS Name
		String TempDNS(pDNSName);

		// Valid string?
		if (TempDNS.IsValid()) {

			// Try as a numeric value

			uResult = StringToIPv6(TempDNS.c_str(), pOutput->U.IPv6.m_IP,
				&pOutput->U.IPv6.m_uPort);
			if (uResult != kErrorNone) {

				// Try as a DNS entry

				char* pColon = StringCharacterReverse(TempDNS.c_str(), ':');
				uResult = kErrorNone;
				if (pColon) {
					pColon[0] = 0; // Force a null string
					uint_t uPort = AsciiToInteger(pColon + 1);
					if (uPort < 65536) {
						pOutput->U.IPv6.m_uPort = uPort;
					} else {
						uResult = kErrorInvalidParameter;
					}
				}

				// Port not found or in bounds
				if (uResult == kErrorNone) {

					// Try www.oldeskuul.com
					addrinfo Hints;
					MemoryClear(&Hints, sizeof(addrinfo));
					Hints.ai_family = PF_INET6;
					// Hints.ai_socktype = SOCK_STREAM;

					addrinfo* pResult = NULL;
					uResult = static_cast<eError>(
						getaddrinfo(TempDNS.c_str(), NULL, &Hints, &pResult));
					if (uResult == kErrorNone) {
						uResult = kErrorInvalidParameter;
						const addrinfo* pWork = pResult;
						if (pWork) {
							do {
								if (pWork->ai_family == AF_INET6) {
									const sockaddr_in6* pIP6Sock =
										reinterpret_cast<const sockaddr_in6*>(
											pWork->ai_addr);
									if (pIP6Sock) {
										// Copy the address
										MemoryCopy(pOutput->U.IPv6.m_IP,
											&pIP6Sock->sin6_addr, 16);
										// No error
										uResult = kErrorNone;
										break;
									}
								}
								pWork = pWork->ai_next;
							} while (pWork);
						}

						// Release the addrinfo chain
						freeaddrinfo(pResult);
					}
				}
			}
			// No error? Mark as IPV6
			if (uResult == kErrorNone) {
				pOutput->m_uType = NetAddr_t::TYPE_IPV6;
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Send a data packet to another network address

	Send data to another machine using UDP or other non-guaranteed
	transport method.

	\param pDestination Pointer to the NetAddr_t that has the destination
		address
	\param pBuffer Pointer to the data to transmit
	\param uBufferSize Number of bytes to transmit

	\return Zero if no error, non zero if an error had occurred

	\sa SendStream(const NetAddr_t *,void *,uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::SendPacket(
	const NetAddr_t* pDestination, const void* pBuffer,
	uintptr_t uBufferSize) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;

	// Sanity checks

	// Any data to send?
	if (uBufferSize) {

		// No pointer to the data?
		if (!pBuffer) {
			uResult = kErrorInvalidParameter;
		}

		// Acceptable protocol?
		NetAddr_t::eAddressType uType = pDestination->m_uType;
		uint16_t uFamily = 0;
		int iAddressSize = 0;
		int iProtocol = IPPROTO_UDP;
		switch (uType) {
		default:
			uResult = kErrorNotSupportedOnThisPlatform; // This isn't a type
														// supported on Windows
			break;

		// Supported protocols
		case NetAddr_t::TYPE_IPV6:
			uFamily = AF_INET6;
			iAddressSize = sizeof(sockaddr_in6);

			// While IPv6 can support larger than 65507 sized packets, if
			// any router in the path is IPv4 only, it can truncate to
			// this limit and force the packet to fail.

			if (uBufferSize > (65535U - 28U)) {
				uResult =
					kErrorBufferTooLarge; // Packet is too big for this protocol
			}

			break;

		case NetAddr_t::TYPE_IPV4:
			uFamily = AF_INET;
			iAddressSize = sizeof(sockaddr_in);
			if (uBufferSize > (65535U - 28U)) {
				uResult =
					kErrorBufferTooLarge; // Packet is too big for this protocol
			}
			break;

		case NetAddr_t::TYPE_IPX:
			uFamily = AF_IPX;
			iAddressSize = sizeof(sockaddr_ipx);
			iProtocol = NSPROTO_IPX;

			// IPX has a larger header (30 bytes)
			if (uBufferSize > (65535U - 30U)) {
				uResult = kErrorBufferTooLarge;
			}
			break;
		}

		if (uResult == kErrorNone) {

			// Open the socket
			SOCKET uSocket = socket(uFamily, SOCK_DGRAM, iProtocol);
			if (uSocket == INVALID_SOCKET) {
				// Looks like the driver isn't available
				uResult = kErrorSocketFailure;

			} else {

				// Is this a broadcast packet?
				if (pDestination->IsBroadcast()) {
					// Set the SO_BROADCAST flag for the socket
					int iOption = TRUE;
					if (setsockopt(uSocket, SOL_SOCKET, SO_BROADCAST,
							reinterpret_cast<char*>(&iOption),
							sizeof(iOption)) == SOCKET_ERROR) {
						// Woah, the call FAILED?!?!? How?
						uResult = kErrorSocketFailure;
					}
				}

				// If broadcast upgrade wasn't needed or succeeded, proceed to
				// transmit the data
				if (uResult == kErrorNone) {

					// Perform the transmit
					HybridSocketAddr_t Address;

					// Translate to an address
					pDestination->ToSocketAddr(&Address.m_Base);

					int iSendtoError =
						sendto(uSocket, static_cast<const char*>(pBuffer),
							static_cast<int>(uBufferSize), 0, &Address.m_Base,
							iAddressSize);

					// Error or successful transmission?
					if ((iSendtoError == SOCKET_ERROR) ||
						(iSendtoError != static_cast<int>(uBufferSize))) {
						uResult = kErrorSocketFailure;
					}
				}

				// Close the socket (It will be transmitted async)
				if (closesocket(uSocket) == SOCKET_ERROR) {
					// Was there already a previous error?
					if (uResult == kErrorNone) {
						// Mark the error (Couldn't post it in the transmission
						// queue?)
						uResult = kErrorSocketFailure;
					}
				}
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Send a data packet to another network address using guaranteed
		protocol

	Send data to another machine using TCP or other guaranteed
	transport method.

	\param pDestination Pointer to the NetAddr_t that has the destination
		address
	\param pBuffer Pointer to the data to transmit
	\param uBufferSize Number of bytes to transmit

	\return Zero if no error, non zero if an error had occurred

	\sa SendPacket(const NetAddr_t *,void *,uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::SendStream(
	const NetAddr_t* pDestination, const void* pBuffer,
	uintptr_t uBufferSize) BURGER_NOEXCEPT
{
	eError uResult = kErrorNone;

	// Sanity checks

	// Any data to send?
	if (uBufferSize) {

		// No pointer to the data?
		if (!pBuffer) {
			uResult = kErrorInvalidParameter;
		}

		// Acceptable protocol?
		NetAddr_t::eAddressType uType = pDestination->m_uType;
		uint16_t uFamily = 0;
		int iAddressSize = 0;
		int iProtocol = IPPROTO_TCP;
		switch (uType) {
		default:
			uResult = kErrorNotSupportedOnThisPlatform; // This isn't a type
														// supported on Windows
			break;

			// Supported protocols
		case NetAddr_t::TYPE_IPV6:
			uFamily = AF_INET6;
			iAddressSize = sizeof(sockaddr_in6);

			// While IPv6 can support larger than 65507 sized packets, if
			// any router in the path is IPv4 only, it can truncate to
			// this limit and force the packet to fail.

			if (uBufferSize > 1048575) {
				uResult =
					kErrorBufferTooLarge; // Packet is too big for this protocol
			}

			break;

		case NetAddr_t::TYPE_IPV4:
			uFamily = AF_INET;
			iAddressSize = sizeof(sockaddr_in);
			if (uBufferSize > 1048575) {
				uResult =
					kErrorBufferTooLarge; // Packet is too big for this protocol
			}
			break;

		case NetAddr_t::TYPE_IPX:
			uFamily = AF_IPX;
			iAddressSize = sizeof(sockaddr_ipx);
			iProtocol = NSPROTO_SPX;
			// IPX has a larger header (30 bytes)
			if (uBufferSize > 1048575) {
				uResult = kErrorBufferTooLarge;
			}
			break;
		}

		if (uResult == kErrorNone) {

			// Open the socket
			SOCKET uSocket = socket(uFamily, SOCK_STREAM, iProtocol);
			if (uSocket == INVALID_SOCKET) {
				// Looks like the driver isn't available
				uResult = kErrorSocketFailure;

			} else {

				// Perform the transmit
				HybridSocketAddr_t Address;

				// Translate to an address
				pDestination->ToSocketAddr(&Address.m_Base);

				// Connect to the other point
				if (connect(uSocket, &Address.m_Base, iAddressSize) ==
					SOCKET_ERROR) {
					// If already connected, ignore the error
					if (WSAGetLastError() != WSAEISCONN) {
						uResult = kErrorSocketFailure;
					}
				}

				// If the connection succeeded, proceed to transmit the data
				if (uResult == kErrorNone) {
					// Perform the transmit

					int iSendError =
						send(uSocket, static_cast<const char*>(pBuffer),
							static_cast<int>(uBufferSize), 0);
					if ((iSendError == SOCKET_ERROR) ||
						(iSendError != static_cast<int>(uBufferSize))) {
						uResult = kErrorSocketFailure;
					}
				}

				// Close the socket (It will be transmitted async)
				if (closesocket(uSocket) == SOCKET_ERROR) {
					// Was there already a previous error?
					if (uResult == kErrorNone) {
						// Mark the error (Couldn't post it in the transmission
						// queue?)
						uResult = kErrorSocketFailure;
					}
				}
			}
		}
	}
	return uResult;
}

/***************************************

	\brief Enumerate all local network addresses

	Scan all the network adapters and collect all of the
	local addresses for this machine. Only addresses that
	are "DNS eligible" will be enumerated, so things
	like the loopback or other special purpose addresses
	are not placed in the list.

	\return Zero if no error, non zero if an error had occurred

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::EnumerateLocalAddresses(
	void) BURGER_NOEXCEPT
{
	// Release any previous list
	m_uLocalAddressCount = 0;
	Free(m_pLocalAddresses);
	m_pLocalAddresses = NULL;

	// These are the addresses that are useful
	const ULONG uFlags = GAA_FLAG_INCLUDE_PREFIX | GAA_FLAG_SKIP_MULTICAST |
		GAA_FLAG_SKIP_DNS_SERVER | GAA_FLAG_SKIP_FRIENDLY_NAME;

	// How much memory is needed for the address list?
	ULONG uSize = 0;
	ULONG uResult = GetAdaptersAddresses(AF_UNSPEC, uFlags, NULL, NULL, &uSize);

	eError uError = kErrorNotSupportedOnThisPlatform;

	// This is the expected error
	if (uResult == ERROR_BUFFER_OVERFLOW) {

		// Nothing? (Should never happen, but you never know)
		uError = kErrorNone;
		if (uSize) {

			uError = kErrorOutOfMemory;

			// Got the size, allocate a buffer
			IP_ADAPTER_ADDRESSES* pAdapterAddresses =
				static_cast<IP_ADAPTER_ADDRESSES*>(Alloc(uSize));
			if (pAdapterAddresses) {

				// Grab the list
				uResult = GetAdaptersAddresses(
					AF_UNSPEC, uFlags, NULL, pAdapterAddresses, &uSize);
				if (uResult == ERROR_SUCCESS) {

					// Iterate over the list to get the number of entries that
					// will be generated

					uint_t uCount = 0;
					const IP_ADAPTER_ADDRESSES* pWork = pAdapterAddresses;
					do {
						// Allowable adapters?
						if ((pWork->IfType == IF_TYPE_ETHERNET_CSMACD) ||
							(pWork->IfType == IF_TYPE_IEEE80211)) {

							// Count the desired unicast addresses
							const IP_ADAPTER_UNICAST_ADDRESS* pUnicast =
								pWork->FirstUnicastAddress;
							if (pUnicast) {
								do {
									if (pUnicast->Flags &
										IP_ADAPTER_ADDRESS_DNS_ELIGIBLE) {
										++uCount;
									}
									pUnicast = pUnicast->Next;
								} while (pUnicast);
							}
						}
						pWork = pWork->Next;
					} while (pWork);

					// A count has been obtained!

					uError = kErrorNone;
					if (uCount) {

						// Get memory for the address list
						NetAddr_t* pNetAddr = static_cast<NetAddr_t*>(
							Alloc(sizeof(NetAddr_t) * uCount));
						uError = kErrorOutOfMemory;
						if (pNetAddr) {

							// Store the values into the NetManager
							m_pLocalAddresses = pNetAddr;
							m_uLocalAddressCount = uCount;

							// Fill in the array
							pWork = pAdapterAddresses;
							do {
								// Allowable adapters?
								if ((pWork->IfType ==
										IF_TYPE_ETHERNET_CSMACD) ||
									(pWork->IfType == IF_TYPE_IEEE80211)) {

									// Count the desired unicast addresses
									const IP_ADAPTER_UNICAST_ADDRESS* pUnicast =
										pWork->FirstUnicastAddress;
									if (pUnicast) {
										do {
											if (pUnicast->Flags &
												IP_ADAPTER_ADDRESS_DNS_ELIGIBLE) {
												// Convert the sockaddr into a
												// NetAddr_t
												pNetAddr->FromSocketAddr(
													pUnicast->Address
														.lpSockaddr);
												++pNetAddr;
											}
											pUnicast = pUnicast->Next;
										} while (pUnicast);
									}
								}
								pWork = pWork->Next;
							} while (pWork);

							// All good!
							uError = kErrorNone;
						}
					}
				}
				// Release the buffer that was used to get data from Windows
				Free(pAdapterAddresses);
			}
		}
	}
	return uError;
}

/***************************************

	\brief Set a posix socket to blocking mode

	Given a valid socket from a BSD socket library,
	mark it as blocking.

	\param uSocket SOCKET from windows or socket from BSD socket systems

	\return Zero if no error, non-zero if not supported or an error
	\sa SocketSetNonBlocking(uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::SocketSetBlocking(
	SocketType uSocket) BURGER_NOEXCEPT
{
	// Turn off "Non-blocking" mode
	u_long uData = FALSE;
	return static_cast<eError>(ioctlsocket(uSocket, FIONBIO, &uData));
}

/***************************************

	\brief Set a posix socket to non-blocking mode

	Given a valid socket from a BSD socket library,
	mark it as non-blocking.

	\param uSocket SOCKET from windows or socket from BSD socket systems

	\return Zero if no error, non-zero if not supported or an error

***************************************/

Burger::eError BURGER_API Burger::SocketSetNonBlocking(
	SocketType uSocket) BURGER_NOEXCEPT
{
	// Turn on "Non-blocking" mode
	u_long uData = TRUE;
	return static_cast<eError>(ioctlsocket(uSocket, FIONBIO, &uData));
}

/***************************************

	\brief Test a posix socket if there's pending data

	Given a valid socket from a BSD socket library,
	test it if there's data pending. Return an error
	if the socket is disconnected.

	\param uSocket SOCKET from windows or socket from BSD socket systems

	\return Zero if no error, non-zero if not supported or an error

***************************************/

Burger::eError BURGER_API Burger::SocketTestForReading(
	SocketType uSocket) BURGER_NOEXCEPT
{
	char Buffer[4];

	// Attempt to read data from the socket to see if either
	// it has data or has been disconnected
	int iAvailable =
		recvfrom(uSocket, Buffer, sizeof(Buffer), MSG_PEEK, NULL, NULL);

	// Assume okay
	eError uResult = kErrorNone;

	// Error?
	if (iAvailable == -1) {
		int iError = WSAGetLastError();

		// Acceptable errors
		if ((iError != WSAEMSGSIZE) && (iError != WSATRY_AGAIN) &&
			(iError != WSASYSNOTREADY)) {
			uResult = static_cast<eError>(iError);
			// Force clearing the socket, but make sure the error is returned
			iAvailable =
				recvfrom(uSocket, Buffer, sizeof(Buffer), 0, NULL, NULL);
		}
	}

	// If iAvailable is set to zero, the socket is
	// disconnected. Return a kErrorNoData error to
	// alert the caller of this failure condition

	if ((uResult == kErrorNone) && !iAvailable) {
		uResult = kErrorNoData;
	}
	return uResult;
}

#endif
