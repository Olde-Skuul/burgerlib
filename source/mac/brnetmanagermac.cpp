/***************************************

    MacOS version of Burger::NetworkManager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brnetmanager.h"

#if defined(BURGER_MAC) || defined(DOXYGEN)
#include "brendian.h"
#include "brmemoryfunctions.h"
#include <Files.h>
#include <Gestalt.h>
#include <OSUtils.h>
#include <OpenTptAppleTalk.h>
#include <OpenTptInternet.h>
#include <OpenTransport.h>
#include <time.h>


#if !defined(DOXYGEN)
static const int g_Protocols[4] = {
	gestaltOpenTptTCPPresentMask,	  // IPv4
	0,								   // IPv6 doesn't exist on this platform
	gestaltOpenTptIPXSPXPresentMask,   // IPX/SPX
	gestaltOpenTptAppleTalkPresentMask // AppleTalk
};
#endif

/*! ************************************

	\brief Convert a NetAddr_t into a OTAddress

	Convert a NetAddr_t into a OTAddress structure.

	Supports AppleTalk or IPv4

	\maconly
	\param pOutput Pointer to a OTAddress to receive the socket address
	\return Zero if no error, non-zero if not supported or unimplemented
protocol

***************************************/

uint_t BURGER_API Burger::NetAddr_t::ToOTAddress(OTAddress* pOutput) const BURGER_NOEXCEPT
{
	uint_t uResult = 0;
	switch (m_uType) {

	case TYPE_IPV4:
		pOutput->fAddressType = AF_INET;
		reinterpret_cast<InetAddress*>(pOutput)->fPort =
			BigEndian::Load(static_cast<uint16_t>(U.IPv4.m_uPort));
		reinterpret_cast<InetAddress*>(pOutput)->fHost =
			BigEndian::Load(U.IPv4.m_uIP);
		MemoryClear(reinterpret_cast<InetAddress*>(pOutput)->fUnused,
			sizeof(reinterpret_cast<InetAddress*>(pOutput)->fUnused));
		break;

	case TYPE_APPLETALK:
		pOutput->fAddressType = AF_ATALK_DDP;
		reinterpret_cast<DDPAddress*>(pOutput)->fNetwork =
			BigEndian::Load(static_cast<uint16_t>(U.APPLETALK.m_uNetwork));
		reinterpret_cast<DDPAddress*>(pOutput)->fNodeID =
			static_cast<uint8_t>(U.APPLETALK.m_uNodeID);
		reinterpret_cast<DDPAddress*>(pOutput)->fSocket =
			static_cast<uint8_t>(U.APPLETALK.m_uSocket);
		reinterpret_cast<DDPAddress*>(pOutput)->fDDPType =
			static_cast<uint8_t>(U.APPLETALK.m_uDDPType);
		break;

	// Unknown
	default:
		MemoryClear(pOutput, sizeof(*pOutput));
		uResult = 10;
		break;
	}
	return uResult;
}

/*! ************************************

	\brief Convert a OTAddress into a NetAddr_t

	Convert a OTAddress into a NetAddr_t structure.

	Supports AppleTalk and IPv4

	\maconly
	\param pInput Pointer to a OTAddress to read the socket address from
	\return Zero if no error, non-zero if not supported or unimplemented
protocol

***************************************/

uint_t BURGER_API Burger::NetAddr_t::FromOTAddress(const OTAddress* pInput) BURGER_NOEXCEPT
{
	uint_t uResult = 0;

	switch (pInput->fAddressType) {

	case AF_INET:
		m_uType = TYPE_IPV4;
		U.IPv4.m_uPort = BigEndian::Load(
			reinterpret_cast<const InetAddress*>(pInput)->fPort);
		U.IPv4.m_uIP = BigEndian::Load(static_cast<uint32_t>(
			reinterpret_cast<const InetAddress*>(pInput)->fHost));
		break;

	case AF_ATALK_DDP:
		m_uType = TYPE_APPLETALK;
		U.APPLETALK.m_uNetwork = BigEndian::Load(
			reinterpret_cast<const DDPAddress*>(pInput)->fNetwork);
		U.APPLETALK.m_uNodeID =
			reinterpret_cast<const DDPAddress*>(pInput)->fNodeID;
		U.APPLETALK.m_uSocket =
			reinterpret_cast<const DDPAddress*>(pInput)->fSocket;
		U.APPLETALK.m_uDDPType =
			reinterpret_cast<const DDPAddress*>(pInput)->fDDPType;
		break;

		// Unknown
	default:
		MemoryClear(this, sizeof(*this));
		uResult = 10;
		break;
	}
	return uResult;
}

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
	if (!m_bOpenTransportStarted) {
		uResult = kErrorNotSupportedOnThisPlatform;
		long iGestalt;
		// Is Open Transport installed?
		if (!Gestalt(gestaltOpenTpt, &iGestalt)
			&& (iGestalt & gestaltOpenTptPresentMask)) {

#if TARGET_API_MAC_CARBON
			uResult = static_cast<eError>(InitOpenTransportInContext(
				kInitOTForApplicationMask, &m_pOTContext));
#else
			uResult = static_cast<eError>(InitOpenTransport());
#endif
			if (!uResult) {
				m_bOpenTransportStarted = TRUE;

				// Iterate over the protocols to see which ones are
				// actually available

				uint_t i = 0;
				do {
					if (iGestalt & g_Protocols[i]) {
						m_uProtocolsFound |= 1U << (i + NetAddr_t::TYPE_IPV4);
					}
				} while (++i < BURGER_ARRAYSIZE(g_Protocols));

				// Obtain the host name
				m_HostName.Set("localhost");

				// Obtain the local addresses
				EnumerateLocalAddresses();
				++m_uStarted;
			}
		}
	} else {
		// Increment the startup counter
		++m_uStarted;
	}
	return uResult;
}

/***************************************

	\brief Shut down network code

	Shut down the operating system's network layer and
	release all allocated resources

***************************************/

void BURGER_API Burger::NetworkManager::Shutdown(void) BURGER_NOEXCEPT
{
	if (m_uStarted) {
		if (!--m_uStarted) {
			if (m_bOpenTransportStarted) {
				if (m_pEventHandler) {
					DisposeOTNotifyUPP((OTNotifyUPP)m_pEventHandler);
					m_pEventHandler = NULL;
				}

				// Open Transport is a context in Carbon
#if TARGET_API_MAC_CARBON
				CloseOpenTransportInContext(m_pOTContext);
				m_pOTContext = NULL;
#else
				// It's global on MacOS classic
				CloseOpenTransport();
#endif
				m_bOpenTransportStarted = FALSE;
			}

			// Release the local addresses
			Free(m_pLocalAddresses);
			m_pLocalAddresses = NULL;
			m_uLocalAddressCount = 0;
		}
	}
}

/***************************************

	\brief Resolve a DNS address to IPv4

	Given a DNS string with an optional port number, contact
	the connected DNS server to resolve the address.

	\note If the DNS address is a port literal, it will be decoded without
contacting any server.

	\param pOutput Pointer to the NetAddr_t structure that will contain the
resolved address \param pDNSName Pointer to a "C" string in UTF-8 encoding to
pass to the DNS server \return Zero if no error, non zero if the string couldn't
be resolved.

***************************************/

struct TMyOTInetSvcInfo { // Open Transport Internet services provider info
	InetSvcRef m_pRef;	// Provider reference
	void* m_pCookie;	  // Cookie
	uint_t m_bComplete;	 // TRUE when asynch operation has completed
	OTResult m_iResult;   // Result code
};

/***************************************

	This is called asynchronously by Open Transport

***************************************/

static pascal void MyOTInetSvcNotifyProc(
	void* pServiceInfo, OTEventCode uCode, OTResult iResult, void* pCookie) BURGER_NOEXCEPT
{
	switch (uCode) {
	case T_OPENCOMPLETE:
	case T_DNRSTRINGTOADDRCOMPLETE:
	case T_DNRADDRTONAMECOMPLETE: // Did the operation complete?
		static_cast<TMyOTInetSvcInfo*>(pServiceInfo)->m_bComplete = TRUE;
		static_cast<TMyOTInetSvcInfo*>(pServiceInfo)->m_iResult = iResult;
		static_cast<TMyOTInetSvcInfo*>(pServiceInfo)->m_pCookie = pCookie;
		break;
	}
}

/***************************************

	Wait for the event to occur or the timeout

***************************************/

static uint_t MyOTInetSvcWait(TMyOTInetSvcInfo* pServiceInfo, clock_t uTime) BURGER_NOEXCEPT
{
	if (!pServiceInfo->m_bComplete) { // Not done yet?
		clock_t uMark = clock();	  // Get timer
		do {
			if ((clock() - uMark) >= uTime) {
				return 10; // Timeout!
			}
		} while (!pServiceInfo->m_bComplete); // Not yet!
	}
	// Return the error
	return static_cast<uint_t>(pServiceInfo->m_iResult);
}

Burger::eError BURGER_API Burger::NetworkManager::ResolveIPv4Address(
	NetAddr_t* pOutput, const char* pDNSName)
{
	// Clear the output
	MemoryClear(pOutput, sizeof(*pOutput));
	eError uResult = kErrorNotSupportedOnThisPlatform;
	if (m_uStarted && (m_uProtocolsFound & (1U << NetAddr_t::TYPE_IPV4))) {

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
				if (uPort < 65536U) {
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

					// It's a real DNS name, resolve it.
					OTNotifyUPP pNotifyProc =
						NewOTNotifyUPP(MyOTInetSvcNotifyProc);
					TMyOTInetSvcInfo svcInfo;	// Internet services
					svcInfo.m_bComplete = FALSE; // Init the wait proc
					if (
#if TARGET_API_MAC_CARBON
						!OTAsyncOpenInternetServicesInContext(
							kDefaultInternetServicesPath, 0, pNotifyProc,
							&svcInfo, m_pOTContext)
#else
						!OTAsyncOpenInternetServices(
							kDefaultInternetServicesPath, 0, pNotifyProc,
							&svcInfo)
#endif
					) {
						// Start services
						uResult = static_cast<eError>(
							MyOTInetSvcWait(&svcInfo, 10 * CLOCKS_PER_SEC));
						if (uResult == kErrorNone) { // Didn't time out?

							// Get the reference
							svcInfo.m_pRef = static_cast<TInternetServices*>(
								svcInfo.m_pCookie);

							// Init the flag
							svcInfo.m_bComplete = FALSE;

							// Internet host address returned
							InetHostInfo hInfoOT;

							// Resolve DNS
							uResult = static_cast<eError>(
								OTInetStringToAddress(svcInfo.m_pRef,
									TempDNS.c_str(), &hInfoOT));

							if (uResult == kErrorNone) {
								// Timeout?
								uResult =
									static_cast<eError>(MyOTInetSvcWait(
										&svcInfo, 10 * CLOCKS_PER_SEC));
							}

							// Release services
							OTCloseProvider(svcInfo.m_pRef);

							// Error?
							if (uResult == kErrorNone) {
								// Get the address
								uIPv4 = hInfoOT.addrs[0];
							}
						}
					}
					DisposeOTNotifyUPP(pNotifyProc);
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
	const NetAddr_t* pDestination, const void* pBuffer, uintptr_t uBufferSize) BURGER_NOEXCEPT
{
	eError uResult = kErrorSocketFailure;
	OSStatus err;
	EndpointRef uSocket = OTOpenEndpointInContext(
		OTCreateConfiguration(kUDPName), 0, NULL, &err, NULL);
	if (!err) {

		InetAddress Dest;
		pDestination->ToOTAddress((OTAddress*)&Dest);

		TBind BindOut;
		BindOut.addr.maxlen = sizeof(Dest);
		BindOut.addr.len = sizeof(Dest);
		BindOut.addr.buf = (uint8_t*)&Dest;
		BindOut.qlen = 0;

		uResult = kErrorSocketFailure;
		if (!OTBind(uSocket, 0, &BindOut)) {

			OTSetBlocking(uSocket);
			OTSetSynchronous(uSocket);

			// Destination address
			TUnitData d;
			pDestination->ToOTAddress((OTAddress*)&Dest);
			d.addr.len = sizeof(Dest);
			d.addr.maxlen = sizeof(Dest);
			d.addr.buf = (uint8_t*)&Dest;

			d.opt.len = 0;
			d.opt.maxlen = 0;
			d.opt.buf = NULL;

			d.udata.len = uBufferSize;
			d.udata.maxlen = uBufferSize;
			d.udata.buf = (uint8_t*)pBuffer;

			uResult = kErrorNotReady;
			err = OTSndUData(uSocket, &d);
			if (err != kOTNoError) {
				if (err == kOTLookErr) {
					if (OTLook(uSocket) == T_UDERR) {
						OTRcvUDErr(uSocket, 0);
					}
				}
				uResult = kErrorSocketFailure;
			} else {

				uResult = kErrorNone;
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
address \param pBuffer Pointer to the data to transmit \param uBufferSize Number
of bytes to transmit \return Zero if no error, non zero if an error had occurred
	\sa SendPacket(const NetAddr_t *,void *,uintptr_t)

***************************************/

Burger::eError BURGER_API Burger::NetworkManager::SendStream(
	const NetAddr_t* pDestination, const void* pBuffer, uintptr_t uBufferSize) BURGER_NOEXCEPT
{
	eError uResult = kErrorSocketFailure;
	OSStatus err;
	EndpointRef uSocket = OTOpenEndpointInContext(
		OTCreateConfiguration(kTCPName), 0, NULL, &err, NULL);
	if (!err) {

		InetAddress Dest;
		pDestination->ToOTAddress((OTAddress*)&Dest);

		TBind BindOut;
		BindOut.addr.maxlen = sizeof(Dest);
		BindOut.addr.len = sizeof(Dest);
		BindOut.addr.buf = (uint8_t*)&Dest;
		BindOut.qlen = 0;

		uResult = kErrorAddressNotFound;
		if (!OTBind(uSocket, 0, &BindOut)) {

			OTSetBlocking(uSocket);
			OTSetSynchronous(uSocket);
			uResult = kErrorSocketFailure;

			if (!err) {

				// Destination address
				TUnitData d;
				pDestination->ToOTAddress((OTAddress*)&Dest);

				d.addr.len = sizeof(Dest);
				d.addr.maxlen = sizeof(Dest);
				d.addr.buf = (uint8_t*)&Dest;

				d.opt.len = 0;
				d.opt.maxlen = 0;
				d.opt.buf = NULL;

				d.udata.len = uBufferSize;
				d.udata.maxlen = uBufferSize;
				d.udata.buf = (uint8_t*)pBuffer;

				uResult = kErrorSocketFailure;
				err =
					OTSnd(uSocket, const_cast<void*>(pBuffer), uBufferSize, 0);
				if (err != kOTNoError) {
				} else {

					uResult = kErrorNone;
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

	// Mac are simple, they only maintain a single IP address

	eError uError = kErrorNotSupportedOnThisPlatform;
	InetInterfaceInfo MyInfo;
	if (!OTInetGetInterfaceInfo(&MyInfo, kDefaultInetInterface)) {
		NetAddr_t* pNetAddr = static_cast<NetAddr_t*>(Alloc(sizeof(NetAddr_t)));
		if (pNetAddr) {
			m_pLocalAddresses = pNetAddr;
			pNetAddr->m_uType = NetAddr_t::TYPE_IPV4;
			pNetAddr->U.IPv4.m_uPort = 0;
			pNetAddr->U.IPv4.m_uIP = BigEndian::Load((uint32_t)MyInfo.fAddress);
			m_uLocalAddressCount = 1;
			uError = kErrorNone;
		}
	}
	return uError;
}

#endif
