/***************************************

    Flash player manager

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

    Please? It's not like I'm asking you for money!

***************************************/

#include "brflashmanager.h"
#include "brfloatingpoint.h"

/*! ************************************

	\typedef Burger::Flash::FSCommandProc
	\brief Functions and classes to support playing Adobe Flash files

	Adobe flash requires subsystems to support ActionScript,
	textures, fonts and vectors to be able to play Flash files.

	\sa Flash::Manager

***************************************/

/*! ************************************

	\namespace Burger::Flash
	\brief Functions and classes to support playing Adobe Flash files

	Adobe flash requires subsystems to support ActionScript,
	textures, fonts and vectors to be able to play Flash files.

	\sa Flash::Manager

***************************************/

/*! ************************************

	\class Burger::Flash::Manager
	\brief Manager to support the playing of Flash files

	Adobe flash requires subsystems to support ActionScript,
	textures, fonts and vectors to be able to play Flash files.
	This manager is the main dispatcher to load, manager and
	play Flash files. The application must supply a renderer
	(Usually OpenGL or DirectX) for the low level draw functions
	and this manager and player will do the rest.

***************************************/

/*! ************************************

	\brief Initialize a Flash file manager

	Init to power up defaults

	\param pApp Pointer to the master application to the player can use low level systems

***************************************/

Burger::Flash::Manager::Manager(GameApp *pApp) :
	m_pGameApp(pApp),
	m_pFSCommmand(NULL),
	m_pCurrentObject(),
	m_fTextureLODBias(-1.2f),
	m_fCurvePixelError(1.0f),
	m_bUseRealtimeFrameRateFlag(FALSE),
	m_bVerboseActionFlag(FALSE),
	m_bVerboseParsingFlag(FALSE),
	m_bVerboseBitmapInfoFlag(FALSE),
	m_bAllowMultithreadingFlag(TRUE),
	m_Random(),
	m_BaseDirectory("9:"),
	m_CodeLibraryHash(),
	m_GlobalEnvironmentVariables(),
	m_ConstructorName("__constructor__"),
	m_CriticalSection()
{
}

/*! ************************************

	\brief Dispose of a Flash file manager

	Release everything that had been allocated and managed
	by the Adobe compatible Flash player.

***************************************/

/*! ************************************

	\fn GameApp *Burger::Flash::Manager::GetGameApp(void) const
	\brief Get the main application

	\return Pointer to the application to render the scene

***************************************/


/*! ************************************

	\fn FSCommandProc Burger::Flash::Manager::GetFSCallback(void) const
	\brief Get the Flash FSCommand handler

	\return Pointer to the FSCommand handler
	\sa SetFSCallback(FSCommandProc)

***************************************/

/*! ************************************

	\fn GameApp *Burger::Flash::Manager::SetFSCallback(FSCommandProc pFSCommand)
	\brief Set the Flash FSCommand handler

	\param pFSCommand Pointer to the FSCommand handler
	\sa GetFSCallback(void) const 

***************************************/



/*! ************************************

	\fn RootObject *Burger::Flash::Manager::GetRootObject(void) const
	\brief Get the pointer to the movie file that has focus

	\return Pointer to the main flash movie object
	\sa SetRootObject(RootObject *)

***************************************/

/*! ************************************

	\fn GameApp *Burger::Flash::Manager::SetRootObject(RootObject *pObject)
	\brief Set the pointer to the movie file that has focus

	\param pObject Pointer to the movie object to contain focus
	\sa GetRootObject(void) const 

***************************************/


/*! ************************************

	\fn Random * Burger::Flash::Manager::GetRandomGenerator(void)
	\brief Accessor to the random number generator

	\return A pointer to the random number generator used by the Flash system

***************************************/

/*! ************************************

	\fn Filename * Burger::Flash::Manager::GetDataDirectory(void)
	\brief Get the data directory

	Retrieve the directory that the flash player is using to load data
	files that the flash file is requesting.

	\return A pointer to the filename record
	\sa SetDataDirectory(const char *)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::SetDataDirectory(const char *pDirectory)
	\brief Set the data directory

	Set the directory that the flash player will use to load data
	files that the flash file is requesting. The directory must
	be in Burgerlib format

	\param pDirectory A pointer to the "C" string with the directory name
	\sa GetDataDirectory(void)

***************************************/


/*! ************************************

	\fn float Burger::Flash::Manager::GetLODBias(void) const
	\brief Get the texture Level of Detail bias

	\return The current Mip Map Z bias value.
	\sa SetLODBias(float)

***************************************/

/*! ************************************

	\brief Set the texture Level of Detail bias

	For Flash files that use 3D graphics, this value is passed
	to the low level 3D system to set the Z bias for mip mapping.

	The default is -1.2f

	\param fTextureLODBias The new level of detail bias
	\sa GetLODBias(void) const

***************************************/

void BURGER_API Burger::Flash::Manager::SetLODBias(float fTextureLODBias)
{
	m_fTextureLODBias = fTextureLODBias;
}

/*! ************************************

	\fn float Burger::Flash::Manager::GetCurvePixelError(void) const
	\brief Get the curve detail level

	1.0f is the default, higher numbers generate courser curves which
	speeds up rendering, and lower number generates finer curves which
	can slow down rendering.

	\return The current curve pixel resolution value.
	\sa SetCurvePixelError(float)

***************************************/

/*! ************************************

	\brief Set the texture Level of Detail bias

	For Flash files that use vector graphics, this constant
	changes the resolution and refinement of the generation
	of curves. Larger numbers generate fewer vertices and
	smaller numbers generate more vertices.

	The default is 1.0f

	\param fCurvePixelError The new curve detail level.
	\sa GetCurvePixelError(void) const

***************************************/

void BURGER_API Burger::Flash::Manager::SetCurvePixelError(float fCurvePixelError)
{
	m_fCurvePixelError = Clamp(fCurvePixelError,1e-6f,1e6f);
}

/*! ************************************

	\fn uint_t Burger::Flash::Manager::GetRealtimeFrameRateFlag(void)
	\brief Get the real time frame rate flag.

	If non-zero, the player will call the update logic once or more times
	per frame to ensure that the logic is called as many times per second
	as the requested frame rate. On slow machines, frames may skip but
	the logic will work at the requested speed.

	\return Zero if \ref FALSE or non-zero for \ref TRUE
	\sa SetRealtimeFrameRateFlag(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::SetRealtimeFrameRateFlag(uint_t bUseRealtimeFrameRateFlag)
	\brief Set the real time frame rate flag.

	If non-zero, the player will call the update logic once or more times
	per frame to ensure that the logic is called as many times per second
	as the requested frame rate. On slow machines, frames may skip but
	the logic will work at the requested speed.

	\param bUseRealtimeFrameRateFlag The new flag
	\sa GetRealtimeFrameRateFlag(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::Flash::Manager::GetVerboseActionFlag(void)
	\brief Get the verbose action script debug flag

	If non-zero, the action script interpreter will output logging
	text to the console for debugging. 

	\return Zero if \ref FALSE or non-zero for \ref TRUE
	\sa SetVerboseActionFlag(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::SetVerboseActionFlag(uint_t bVerboseActionFlag)
	\brief Set the verbose action script debug flag

	If non-zero, the action script interpreter will output logging
	text to the console for debugging. This should not be set for
	shipping code, it's a performance hit.

	\param bVerboseActionFlag The new flag
	\sa GetVerboseActionFlag(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::Flash::Manager::GetVerboseParsingFlag(void)
	\brief Get the verbose data parsing debug flag

	If non-zero, internal data parsing will output logging
	text to the console for debugging. 

	\return Zero if \ref FALSE or non-zero for \ref TRUE
	\sa SetVerboseParsingFlag(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::SetVerboseParsingFlag(uint_t m_bVerboseParsingFlag)
	\brief Set the verbose action script debug flag

	If non-zero, internal data parsing will output logging
	text to the console for debugging. This should not be set for
	shipping code, it's a performance hit.

	\param m_bVerboseParsingFlag The new flag
	\sa GetVerboseParsingFlag(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::Flash::Manager::GetVerboseBitmapInfoFlag(void)
	\brief Get the bitmap generation debug flag

	If non-zero, bitmap generation will output logging
	text to the console for debugging. 

	\return Zero if \ref FALSE or non-zero for \ref TRUE
	\sa SetVerboseBitmapInfoFlag(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::SetVerboseBitmapInfoFlag(uint_t bVerboseBitmapInfoFlag)
	\brief Set the bitmap generation debug flag

	If non-zero, bitmap generation parsing will output logging
	text to the console for debugging. This should not be set for
	shipping code, it's a performance hit.

	\param bVerboseBitmapInfoFlag The new flag
	\sa GetVerboseBitmapInfoFlag(void) const

***************************************/

/*! ************************************

	\fn uint_t Burger::Flash::Manager::GetAllowMultithreadingFlag(void)
	\brief Get the multithreading flag

	If non-zero, the player will use multiple threads
	for background processing

	\return Zero if \ref FALSE or non-zero for \ref TRUE
	\sa SetAllowMultithreadingFlag(uint_t)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::SetAllowMultithreadingFlag(uint_t bAllowMultithreadingFlag)
	\brief Set the multithreading flag

	If non-zero, the player will use multiple CPU threads
	to spread out the processing of the Flash
	movie.

	\param bAllowMultithreadingFlag The new flag
	\sa GetAllowMultithreadingFlag(void) const

***************************************/

/*! ************************************

	\brief Release all of the loaded code libraries

	If any code libraries were loaded during the execution of
	the Flash movie, this function will release all of them

	\sa LoadCodeLibrary(const String *)

***************************************/

void Burger::Flash::Manager::ReleaseCodeLibraries(void)
{
	// Get the iterators for the hash
	HashMapString<CodeLibrary*>::iterator it = m_CodeLibraryHash.begin();
	HashMapString<CodeLibrary*>::iterator end = m_CodeLibraryHash.end();
	if (it!=end) {
		// Any valid entries in the hash will be disposed of
		do {
			Delete(it->second);
			++it;
		} while (it!=end);
	}
	// Dispose of the hash and its contents
	m_CodeLibraryHash.Clear();
}

/*! ************************************

	\brief Load a code library

	If a code library is in the cache, return the reference
	otherwise, load the library from the file system
	and if successful, add it to the cache.

	\param pName Pointer to a String with the filename of the code library
	\return \ref NULL if the library couldn't be loaded or a valid CodeLibrary to pull functions from.
	\sa ReleaseCodeLibraries(void)

***************************************/

Burger::CodeLibrary *Burger::Flash::Manager::LoadCodeLibrary(const String *pName)
{
	CodeLibrary *pResult = NULL;
	// Using the key, see if it's in the hash
	if (!m_CodeLibraryHash.GetData(pName[0],&pResult)) {
		// Not found, create a library object
		pResult = New<CodeLibrary>();
		if (pResult) {
			if (pResult->Init(pName->c_str())) {
				Delete(pResult);
				pResult = NULL;
			} else {
				m_CodeLibraryHash.add(pName[0],pResult);
			}
		}
	}
	return pResult;
}


/*! ************************************

	\fn const String *Burger::Flash::Manager::GetGlobalEnvironmentVariables(void) const
	\brief Get the global environment variables

	Retrieves the pointer to the internal copy of the
	global environment variables.

	\return Pointer to a String with list of environment variables
	\sa SetGlobalEnvironmentVariables(const char *)

***************************************/

/*! ************************************

	\brief Set the global environment variables

	Action script can access "global" variables that the
	interpreter generates to pass information from the
	host system to the movie. This function sets those variables
	so information can be passed to the movie
	before it's started up.

	The string is in the format of a variable name, followed
	by an '=' (Equals) character, and then the variable itself.
	Multiple variables are separated by commas.

	Example:
	VARIABLENAME=DATA,NEXTVARIABLE=DATA

	\note This function makes an internal copy of the string. If 
		changes are desired, call this function again with the updated
		data.

	\param pInput Pointer to a "C" string with list of environment variables
	\sa GetGlobalEnvironmentVariables(void) const

***************************************/

void Burger::Flash::Manager::SetGlobalEnvironmentVariables(const char *pInput)
{
	m_GlobalEnvironmentVariables = pInput;
}

/*! ************************************

	\fn const String *Burger::Flash::Manager::GetConstructorName(void) const
	\brief Get the string constant "__constructor__"

	This constant is used by ActionScript for invoking data constructors

	\return Pointer to a String "__constructor__"

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::Lock(void)
	\brief Lock the global CriticalSection

	The Flash player may spawn multiple threads, this
	CriticalSection is used to keep the threads
	in sync by locking.

	\sa Unlock(void)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::Unlock(void)
	\brief Lock the global CriticalSection

	The Flash player may spawn multiple threads, this
	CriticalSection is used to keep the threads
	in sync by unlocking.

	\sa Lock(void)

***************************************/
