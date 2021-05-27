/***************************************

    MacOSX version of Burger::NetworkManager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brnetmanager.h"

#if defined(BURGER_MACOSX)
#include "brendian.h"
#include "brmemoryfunctions.h"
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#if !defined(DOXYGEN)

// Appletalk was removed in 10.6 or later

#include <AvailabilityMacros.h>
#if !defined(MAC_OS_X_VERSION_10_6)
#include <netat/appletalk.h>
#else
struct at_addr {
	u_short s_net;
	u_char s_node;
};
struct sockaddr_at {
	u_char sat_len;
	u_char sat_family;
	u_char sat_port;
	at_addr sat_addr;
	char sat_zero[8];
};
#endif

#if !defined(NSPROTO_IPX)
#define NSPROTO_IPX 1000
#endif

#if !defined(NSPROTO_SPX)
#define NSPROTO_SPX 1256
#endif

// IPX isn't supported in OSX, only MacOS
struct sockaddr_ipx {
	u_char sat_len;
	u_char sat_family;
	char sa_netnum[4];
	char sa_nodenum[6];
	u_short sa_socket;
};

//
// Union of all supported socket types
//
union HybridSocketAddr_t {
	sockaddr m_Base;
	sockaddr_in m_IPv4;
	sockaddr_in6 m_IPv6;
	sockaddr_ipx m_IPX;
	sockaddr_at m_AppleTalk;
};

static const int g_Protocols[8] = {AF_INET, IPPROTO_UDP, AF_INET6, IPPROTO_UDP,
	AF_IPX, NSPROTO_IPX, AF_APPLETALK, 0};

#endif

/***************************************

	\brief Convert a NetAddr_t into a sockaddr

	Convert a NetAddr_t into a sockaddr structure.

	Supports AppleTalk, IPX, IPv4 and IPv6

	\windowsonly
	\param pOutput Pointer to a sockaddr to receive the socket address
	\return Zero if no error, non-zero if not supported or unimplemented
		protocol
	\sa FromSocketAddr(const sockaddr *), ToSocketAddr(sockaddr_at *)
		const, ToSocketAddr(sockaddr_ipx *) const, ToSocketAddr(sockaddr_in *)
		const or ToSocketAddr(sockaddr_in6 *) const

***************************************/

Burger::eError BURGER_API Burger::NetAddr_t::ToSocketAddr(
	sockaddr* pOutput) const
{
	eError uResult = kErrorNone;
	switch (m_uType) {

	case TYPE_IPV4:
		pOutput->sa_len = sizeof(sockaddr_in);
		pOutput->sa_family = AF_INET;
		reinterpret_cast<sockaddr_in*>(pOutput)->sin_port =
			BigEndian::Load(static_cast<uint16_t>(U.IPv4.m_uPort));
		reinterpret_cast<sockaddr_in*>(pOutput)->sin_addr.s_addr =
			BigEndian::Load(U.IPv4.m_uIP);
		MemoryClear(reinterpret_cast<sockaddr_in*>(pOutput)->sin_zero,
			sizeof(reinterpret_cast<sockaddr_in*>(pOutput)->sin_zero));
		break;

	case TYPE_IPV6:
		pOutput->sa_len = sizeof(sockaddr_in6);
		pOutput->sa_family = AF_INET6;
		reinterpret_cast<sockaddr_in6*>(pOutput)->sin6_port =
			BigEndian::Load(static_cast<uint16_t>(U.IPv6.m_uPort));
		reinterpret_cast<sockaddr_in6*>(pOutput)->sin6_flowinfo = 0;
		MemoryCopy(reinterpret_cast<sockaddr_in6*>(pOutput)->sin6_addr.s6_addr,
			U.IPv6.m_IP,
			sizeof(
				reinterpret_cast<sockaddr_in6*>(pOutput)->sin6_addr.s6_addr));
		reinterpret_cast<sockaddr_in6*>(pOutput)->sin6_scope_id = 0;
		break;

	case TYPE_IPX:
		pOutput->sa_len = sizeof(sockaddr_ipx);
		pOutput->sa_family = AF_IPX;
		reinterpret_cast<sockaddr_ipx*>(pOutput)->sa_socket =
			BigEndian::Load(static_cast<uint16_t>(U.IPX.m_uSocket));
		MemoryCopy(reinterpret_cast<sockaddr_ipx*>(pOutput)->sa_netnum,
			U.IPX.m_Net, 4);
		MemoryCopy(reinterpret_cast<sockaddr_ipx*>(pOutput)->sa_nodenum,
			U.IPX.m_Node, 6);
		break;

	case TYPE_APPLETALK:
		pOutput->sa_len = sizeof(sockaddr_at);
		pOutput->sa_family = AF_APPLETALK;
		reinterpret_cast<sockaddr_at*>(pOutput)->sat_addr.s_net =
			BigEndian::Load(static_cast<uint16_t>(U.APPLETALK.m_uNetwork));
		reinterpret_cast<sockaddr_at*>(pOutput)->sat_addr.s_node =
			static_cast<uint8_t>(U.APPLETALK.m_uNodeID);
		reinterpret_cast<sockaddr_at*>(pOutput)->sat_port =
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

/***************************************

	\brief Convert a sockaddr into a NetAddr_t

	Convert a sockaddr into a NetAddr_t structure.

	Supports AppleTalk, IPX, IPv4 and IPv6

	\windowsonly
	\param pInput Pointer to a sockaddr to read the socket address from
	\return Zero if no error, non-zero if not supported or unimplemented
		protocol
	\sa ToSocketAddr(sockaddr *) const, FromSocketAddr(const sockaddr_at*),
		FromSocketAddr(const sockaddr_ipx *), FromSocketAddr(const sockaddr_in
		*) or FromSocketAddr(const sockaddr_in6 *)

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
			reinterpret_cast<const sockaddr_in6*>(pInput)->sin6_addr.s6_addr,
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
			reinterpret_cast<const sockaddr_at*>(pInput)->sat_addr.s_net);
		U.APPLETALK.m_uNodeID =
			reinterpret_cast<const sockaddr_at*>(pInput)->sat_addr.s_node;
		U.APPLETALK.m_uSocket =
			reinterpret_cast<const sockaddr_at*>(pInput)->sat_port;
		break;

		// Unknown
	default:
		MemoryClear(this, sizeof(*this));
		uResult = kErrorInvalidParameter;
		break;
	}
	return uResult;
}

/***************************************

	\brief Initialize network code

	Start up the operating system's network layer and return any error code if
	it failed.

	\return Zero on success, non-zero if an error had occurred

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::Init(void)
{
	eError uResult = kErrorNone;

	// Already initialized?
	if (!m_uStarted) {

		// Iterate over the protocols to see which ones are
		// actually available

		uint_t i = 0;
		do {
			SocketType uSocket =
				socket(g_Protocols[i], SOCK_DGRAM, g_Protocols[i + 1]);
			if (uSocket != INVALID_SOCKET) {
				m_uProtocolsFound |= 1U << ((i >> 1U) + NetAddr_t::TYPE_IPV4);
				close(uSocket);
			}
			i += 2;
		} while (i < BURGER_ARRAYSIZE(g_Protocols));

		// Obtain the host name

		// Microsoft says 256 is good enough, but I'm paranoid
		// https://msdn.microsoft.com/en-us/library/windows/desktop/ms738527(v=vs.85).aspx
		char TempHostName[512];
		if (!gethostname(TempHostName, sizeof(TempHostName))) {
			m_HostName.Set(TempHostName);
		}

		// Obtain the local addresses
		EnumerateLocalAddresses();
	}

	// Increment the startup counter
	++m_uStarted;

	// Zero on success
	return uResult;
}

/***************************************

	\brief Shut down network code

	Shut down the operating system's network layer and release all allocated
	resources

***************************************/

void BURGER_API Burger::NetworkManager::Shutdown(void)
{
	// Release when counted down to zero
	if (m_uStarted) {
		if (!--m_uStarted) {
			// Release the local addresses
			Free(m_pLocalAddresses);
			m_pLocalAddresses = NULL;
			m_uLocalAddressCount = 0;
		}
	}
}

/***************************************

	\brief Resolve a DNS address

	Given a DNS string with an optional port number, contact the connected DNS
	server to resolve the address.

	\note If the DNS address is a port literal, it will be decoded without
		contacting any server.

	\param pOutput Pointer to the NetAddr_t structure that will contain the
		resolved address
	\param pDNSName Pointer to a "C" string in UTF-8 encoding to pass to the DNS
		server
	\return Zero if no error, non zero if the string couldn't be resolved.

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::ResolveIPv4Address(
	NetAddr_t* pOutput, const char* pDNSName)
{
	// Clear the output
	MemoryClear(pOutput, sizeof(*pOutput));
	eError uResult = kErrorInvalidParameter;

	// Make a copy of the DNS Name
	String TempDNS(pDNSName);

	// Valid string?
	if (TempDNS.IsValid()) {

		// Was there a port #?

		// Scan for the ending colon
		char* pColon = StringCharacterReverse(TempDNS.GetPtr(), ':');
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
			uResult = StringToIPv4(TempDNS.GetPtr(), &uIPv4);
			if (uResult != kErrorNone) {

				// Try www.oldeskuul.com
				addrinfo Hints;
				MemoryClear(&Hints, sizeof(addrinfo));
				Hints.ai_family = PF_INET;
				// Hints.ai_socktype = SOCK_STREAM;

				addrinfo* pResult = NULL;
				uResult = static_cast<eError>(
					getaddrinfo(TempDNS.GetPtr(), NULL, &Hints, &pResult));
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
	return uResult;
}

/***************************************

	\brief Resolve a DNS address to IPv6

	Given a DNS string with an optional port number, contact the connected DNS
	server to resolve the address.

	\note If the DNS address is a port literal, it will be decoded without
		contacting any server.

	\param pOutput Pointer to the NetAddr_t structure that will contain the
		resolved address
	\param pDNSName Pointer to a "C" string in UTF-8 encoding to pass to the DNS
		server
	\return Zero if no error, non zero if the string couldn't be resolved.

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::ResolveIPv6Address(
	NetAddr_t* pOutput, const char* pDNSName)
{
	// Clear the output
	MemoryClear(pOutput, sizeof(*pOutput));
	eError uResult = kErrorInvalidParameter;

	// Make a copy of the DNS Name
	String TempDNS(pDNSName);

	// Valid string?
	if (TempDNS.IsValid()) {

		// Try as a numeric value

		uResult = StringToIPv6(
			TempDNS.GetPtr(), pOutput->U.IPv6.m_IP, &pOutput->U.IPv6.m_uPort);
		if (uResult != kErrorNone) {

			// Try as a DNS entry

			char* pColon = StringCharacterReverse(TempDNS.GetPtr(), ':');
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
					getaddrinfo(TempDNS.GetPtr(), NULL, &Hints, &pResult));
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

	return uResult;
}

/***************************************

	\brief Send a data packet to another network address

	Send data to another machine using UDP or other non-guaranteed transport
	method.

	\param pDestination Pointer to the NetAddr_t that has the destination
		address
	\param pBuffer Pointer to the data to transmit
	\param uBufferSize Number of bytes to transmit
	\return Zero if no error, non zero if an error had occurred
	\sa SendStream(const NetAddr_t *,void *,uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::SendPacket(
	const NetAddr_t* pDestination, const void* pBuffer, uintptr_t uBufferSize)
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

			// While IPv6 can support larger than 65507 sized packets, if any
			// router in the path is IPv4 only, it can truncate to this limit
			// and force the packet to fail.

			if (uBufferSize > (65535U - 28U)) {
				uResult = kErrorBufferTooLarge; // Packet is too big for this
												  // protocol
			}

			break;

		case NetAddr_t::TYPE_IPV4:
			uFamily = AF_INET;
			iAddressSize = sizeof(sockaddr_in);
			if (uBufferSize > (65535U - 28U)) {
				uResult = kErrorBufferTooLarge; // Packet is too big for this
												  // protocol
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
			SocketType uSocket = socket(uFamily, SOCK_DGRAM, iProtocol);
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
							sizeof(iOption)) == -1) {
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
					if ((iSendtoError == -1) ||
						(iSendtoError != static_cast<int>(uBufferSize))) {
						uResult = kErrorSocketFailure;
					}
				}

				// Close the socket (It will be transmitted async)
				if (close(uSocket) == -1) {
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

	Send data to another machine using TCP or other guaranteed transport method.

	\param pDestination Pointer to the NetAddr_t that has the destination
		address
	\param pBuffer Pointer to the data to transmit
	\param uBufferSize Number of bytes to transmit
	\return Zero if no error, non zero if an error had occurred
	\sa SendPacket(const NetAddr_t *,void *,uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::SendStream(
	const NetAddr_t* pDestination, const void* pBuffer, uintptr_t uBufferSize)
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

			// While IPv6 can support larger than 65507 sized packets, if any
			// router in the path is IPv4 only, it can truncate to this limit
			// and force the packet to fail.

			if (uBufferSize > 1048575) {
				uResult = kErrorBufferTooLarge; // Packet is too big for this
												  // protocol
			}

			break;

		case NetAddr_t::TYPE_IPV4:
			uFamily = AF_INET;
			iAddressSize = sizeof(sockaddr_in);
			if (uBufferSize > 1048575) {
				uResult = kErrorBufferTooLarge; // Packet is too big for this
												  // protocol
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
			SocketType uSocket = socket(uFamily, SOCK_STREAM, iProtocol);
			if (uSocket == INVALID_SOCKET) {
				// Looks like the driver isn't available
				uResult = kErrorSocketFailure;

			} else {

				// Perform the transmit
				HybridSocketAddr_t Address;

				// Translate to an address
				pDestination->ToSocketAddr(&Address.m_Base);

				// Connect to the other point
				if (connect(uSocket, &Address.m_Base, iAddressSize) == -1) {
					// If already connected, ignore the error
					if (errno != EISCONN) {
						uResult = kErrorSocketFailure;
					}
				}

				// If the connection succeeded, proceed to transmit the data
				if (uResult == kErrorNone) {
					// Perform the transmit

					ssize_t iSendError =
						send(uSocket, static_cast<const char*>(pBuffer),
							static_cast<int>(uBufferSize), 0);
					if ((iSendError == -1) ||
						(iSendError != static_cast<int>(uBufferSize))) {
						uResult = kErrorSocketFailure;
					}
				}

				// Close the socket (It will be transmitted async)
				if (close(uSocket) == -1) {
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

	Scan all the network adapters and collect all of the local addresses for
	this machine. Only addresses that are "DNS eligible" will be enumerated, so
	things like the loopback or other special purpose addresses are not placed
	in the list.

	\return Zero if no error, non zero if an error had occurred

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::EnumerateLocalAddresses(
	void)
{
	// Release any previous list
	m_uLocalAddressCount = 0;
	Free(m_pLocalAddresses);
	m_pLocalAddresses = NULL;

	eError uError = kErrorNotSupportedOnThisPlatform;

	// Hint for addresses associated
	// with this host machine.

	addrinfo Hints;
	MemoryClear(&Hints, sizeof(addrinfo));
	Hints.ai_family = AF_UNSPEC;
	Hints.ai_flags = 0;
	Hints.ai_socktype = SOCK_DGRAM; // Prevent address duplication

	addrinfo* pResult = NULL;
	int iResult = getaddrinfo(m_HostName.GetPtr(), NULL, &Hints, &pResult);
	if (!iResult) {

		// Iterate over the list to get the number of entries that will be
		// generated
		uint_t uCount = 0;
		uError = kErrorNone;
		const addrinfo* pLoop = pResult;
		if (pLoop) {
			do {
				if (pLoop->ai_family == AF_INET) {
					if (!IsSelfAssignedIPv4(BigEndian::Load(
							&reinterpret_cast<const sockaddr_in*>(
								pLoop->ai_addr)
								 ->sin_addr.s_addr))) {
						++uCount;
					}
				} else if (pLoop->ai_family == AF_INET6) {
					if (!IsIPv6LinkLocal(reinterpret_cast<const sockaddr_in6*>(
							pLoop->ai_addr)
											 ->sin6_addr.s6_addr)) {
						++uCount;
					}
				}
				pLoop = pLoop->ai_next;
			} while (pLoop);

			// A count has been obtained!

			if (uCount) {

				// Get memory for the address list
				NetAddr_t* pNetAddr =
					static_cast<NetAddr_t*>(Alloc(sizeof(NetAddr_t) * uCount));
				uError = kErrorOutOfMemory;
				if (pNetAddr) {

					// Store the values into the NetManager
					m_pLocalAddresses = pNetAddr;
					m_uLocalAddressCount = uCount;

					// Fill in the array
					pLoop = pResult;
					do {
						if (pLoop->ai_family == AF_INET) {
							if (!IsSelfAssignedIPv4(BigEndian::Load(
									&reinterpret_cast<const sockaddr_in*>(
										pLoop->ai_addr)
										 ->sin_addr.s_addr))) {
								pNetAddr->FromSocketAddr(pLoop->ai_addr);
								++pNetAddr;
							}
						} else if (pLoop->ai_family == AF_INET6) {
							if (!IsIPv6LinkLocal(
									reinterpret_cast<const sockaddr_in6*>(
										pLoop->ai_addr)
										->sin6_addr.s6_addr)) {
								pNetAddr->FromSocketAddr(pLoop->ai_addr);
								++pNetAddr;
							}
						}
						pLoop = pLoop->ai_next;
					} while (pLoop);

					// All good!
					uError = kErrorNone;
				}
			}
		}

		// Release the addrinfo chain
		freeaddrinfo(pResult);
	}

	return uError;
}

/***************************************

	\brief Set a posix socket to blocking mode

	Given a valid socket from a BSD socket library and market it as blocking.

	\param iSocket SOCKET from windows or socket from BSD socket systems

	\return Zero if no error, non-zero if not supported or an error
	\sa SocketSetNonBlocking(uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::SocketSetBlocking(SocketType iSocket)
{
	eError uResult = kErrorNone;

	// Get the socket flags
	int iFlags = fcntl(static_cast<int>(iSocket), F_GETFL, 0);

	if (iFlags < 0) {
		uResult = static_cast<eError>(errno);

	} else {
		// Turn off non-blocking
		iFlags &= (~O_NONBLOCK);

		// Set the flag
		if (fcntl(static_cast<int>(iSocket), F_SETFL, iFlags) >= 0) {
			uResult = static_cast<eError>(errno);
		}
	}
	return uResult;
}

/***************************************

	\brief Set a posix socket to non-blocking mode

	Given a valid socket from a BSD socket library and market it as
	non-blocking.

	\param iSocket SOCKET from windows or socket from BSD socket systems

	\return Zero if no error, non-zero if not supported or an error

***************************************/

Burger::eError BURGER_API Burger::SocketSetNonBlocking(SocketType iSocket)
{
	eError uResult = kErrorNone;

	// Get the socket flags
	int iFlags = fcntl(static_cast<int>(iSocket), F_GETFL, 0);

	if (iFlags < 0) {
		uResult = static_cast<eError>(errno);

	} else {
		// Turn on non-blocking
		iFlags |= O_NONBLOCK;

		// Set the flag
		if (fcntl(static_cast<int>(iSocket), F_SETFL, iFlags) >= 0) {
			uResult = static_cast<eError>(errno);
		}
	}
	return uResult;
}

/***************************************

	\brief Test a posix socket if there's pending data

	Given a valid socket from a BSD socket library, test it if there's data
	pending. Return an error if the socket is disconnected.

	\param uSocket SOCKET from windows or socket from BSD socket systems

	\return Zero if no error, non-zero if not supported or an error

***************************************/

Burger::eError BURGER_API Burger::SocketTestForReading(SocketType uSocket)
{
	char Buffer[4];

	// Attempt to read data from the socket to see if either it has data or has
	// been disconnected
	ssize_t iAvailable =
		recvfrom(uSocket, Buffer, sizeof(Buffer), MSG_PEEK, NULL, NULL);

	// Assume okay
	eError uResult = kErrorNone;

	// Error?
	if (iAvailable == -1) {
		int iError = errno;

		// Acceptable errors
		if ((iError != EMSGSIZE) && (iError != EAGAIN)) {
			uResult = static_cast<eError>(iError);
			// Force clearing the socket, but make sure the error is returned
			iAvailable =
				recvfrom(uSocket, Buffer, sizeof(Buffer), 0, NULL, NULL);
		}
	}

	// If iAvailable is set to zero, the socket is disconnected. Return a kErrorNoData
	// error to alert the caller of this failure condition

	if ((uResult == kErrorNone) && !iAvailable) {
		uResult = kErrorNoData;
	}
	return uResult;
}

#endif
