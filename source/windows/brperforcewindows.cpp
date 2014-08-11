/***************************************

	Class to add perforce integration to Windows tools

	Windows version

	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brperforce.h"

#if defined(BURGER_WINDOWS) && defined(BURGER_MSVC)

// Include the perforce header
#include "brglobalmemorymanager.h"
#include "brfilename.h"

// Some compiler settings are not present for Metrowerks
#if defined(BURGER_METROWERKS)
#ifndef bool
#define bool Word32
#endif

#ifndef true
#define true 1
#endif
#endif

#include <clientapi.h>

#if defined(NDEBUG)
#define BLD "rel"
#else
#define BLD "dbg"
#endif

#if defined(BURGER_WIN64)
#define B64 "w64"
#else
#define B64 "w32"
#endif

#if defined(BURGER_WATCOM)
#pragma library ("Ws2_32.lib")
#pragma library ("libclientv10" B64 BLD ".lib")
#pragma library ("librpcv10" B64 BLD ".lib")
#pragma library ("libsuppv10" B64 BLD ".lib")
#pragma library ("libp4sslstubv10" B64 BLD ".lib")
#else
#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"libclientv10" B64 BLD ".lib")
#pragma comment(lib,"librpcv10" B64 BLD ".lib")
#pragma comment(lib,"libsuppv10" B64 BLD ".lib")
#pragma comment(lib,"libp4sslstubv10" B64 BLD ".lib")
#endif

/***************************************

	This class is used to intercept error codes from the
	server so the tool can return warnings and errors properly

***************************************/

namespace Burger {
class PerforceClient : public ClientUser {
public:
	void Message(Error *pE);
	int m_iError;				///< Error code received from Perforce
};
}

/***************************************

	This function is called the moment the server has
	completed the action.

***************************************/

void Burger::PerforceClient::Message(Error *pE)
{
	// Grab the error code
	m_iError = pE->GetGeneric();
	ClientUser::Message(pE);
}

/*! ************************************

	\fn Burger::Perforce::~Perforce()
	\brief Release the perforce tool and clean up

	\sa Perforce::Init() and Perforce::Shutdown()

***************************************/

Burger::Perforce::~Perforce()
{
	Shutdown();
}

/*! ************************************

	\fn int Burger::Perforce::Init(void)
	\brief Open a connection to perforce

	Load the Perforce DLL and initialize it. This call is required before
	calling Perforce::Edit() or Perforce::RevertIfUnchanged()

	\return Zero if no error, non-zero if an perforce error had occurred
	\sa Perforce::Shutdown()
	
***************************************/

int Burger::Perforce::Init(void)
{
	// Already initialized?
	if (m_pClientApi) {
		return 0;
	}

	// Get memory for the client
	ClientApi *pClientApi = new (Alloc(sizeof(ClientApi))) ClientApi();
	if (!pClientApi) {
		return 20;
	}

	// Connect to Perforce
	Error PerforceError;
	pClientApi->Init(&PerforceError);
	// Was there an error?
	int iResult = PerforceError.Test();
	if (iResult) {
		// Dispose of the class
		pClientApi->~ClientApi();
		Free(pClientApi);
		pClientApi = NULL;
	}
	// Store NULL or a valid pointer
	m_pClientApi = pClientApi;
	return iResult;
}

/*! ************************************

	\fn int Burger::Perforce::Shutdown(void)
	\brief Shut down any pending commands from Perforce and release
		all resources
	\return Zero if no error, non-zero if an perforce error had occurred
	\sa Perforce::Init()

***************************************/

int Burger::Perforce::Shutdown(void)
{
	int iResult = 0;
	ClientApi *pClientApi = m_pClientApi;
	if (pClientApi) {
		Error PerforceError;
		// Get the error code(s) from the manager
		iResult = pClientApi->Final(&PerforceError);
		// Release the class
		pClientApi->~ClientApi();
		Free(pClientApi);
		m_pClientApi = NULL;
	}
	// Exit with any error code left from any asyn commands
	return iResult;
}

/*! ************************************

	\fn int Burger::Perforce::Edit(const char *pFilename)
	\brief Issue an "edit" command to open a file
	\param pFilename Pointer to a Burgerlib format filename
	\return Zero if no error, non-zero if an perforce error had occurred
	\sa Perforce::RevertIfUnchanged()

***************************************/

int Burger::Perforce::Edit(const char *pFilename)
{
	ClientApi *pClientApi = m_pClientApi;
	int iResult = 20;
	if (pClientApi) {
		Filename Native(pFilename);
		const char *argv = Native.GetNative();
		// Add the filename of interest
		pClientApi->SetArgv(1,const_cast<char * const *>(&argv));
		// Issue the command
		PerforceClient TheClient;
		pClientApi->Run("edit",&TheClient);
		// Was there an error?
		iResult = TheClient.m_iError;
	}
	return iResult;
}

/*! ************************************

	\fn int Burger::Perforce::RevertIfUnchanged(const char *pFilename)
	\brief Issue an "revert" command to revert a file if it hasn't changed

	Given a Burgerlib pathname, perform a "p4 -a revert" command on it to
	revert the file if it has not changed

	\param pFilename Pointer to a Burgerlib format filename
	\return Zero if no error, non-zero if an perforce error had occurred
	\sa Perforce::Edit()

***************************************/

int Burger::Perforce::RevertIfUnchanged(const char *pFilename)
{
	ClientApi *pClientApi = m_pClientApi;
	int iResult = 20;
	if (pClientApi) {
		Filename Native(pFilename);
		// Issue -a to revert if unchanged
		const char * argv[2] = { "-a",Native.GetNative() }; 
		pClientApi->SetArgv(2,const_cast<char **>(argv));
		// Issue the command
		PerforceClient TheClient;
		pClientApi->Run("revert",&TheClient);
		// Was there an error?
		iResult = TheClient.m_iError;
	}
	return iResult;
}

#endif
