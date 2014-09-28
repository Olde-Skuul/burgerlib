/***************************************

	Flash player manager
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#include "brflashmanager.h"
#include "brfloatingpoint.h"

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
	m_Random(),
	m_BaseDirectory("9:"),
	m_fTextureLODBias(-1.2f),
	m_fCurvePixelError(1.0f),
	m_bUseRealtimeFrameRateFlag(FALSE),
	m_bVerboseActionFlag(FALSE),
	m_bVerboseParsingFlag(FALSE),
	m_bVerboseBitmapInfoFlag(FALSE)
{
	m_Random.RandomInit();
}

/*! ************************************

	\brief Dispose of a Flash file manager

	Release everything that had been allocated and managed
	by the Adobe compatible Flash player.

***************************************/

Burger::Flash::Manager::~Manager()
{
}


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

	\fn Word Burger::Flash::Manager::GetRealtimeFrameRateFlag(void)
	\brief Get the real time frame rate flag.

	If non-zero, the player will call the update logic once or more times
	per frame to ensure that the logic is called as many times per second
	as the requested frame rate. On slow machines, frames may skip but
	the logic will work at the requested speed.

	\return Zero if \ref FALSE or non-zero for \ref TRUE
	\sa SetRealtimeFrameRateFlag(Word)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::SetRealtimeFrameRateFlag(Word bUseRealtimeFrameRateFlag)
	\brief Set the real time frame rate flag.

	If non-zero, the player will call the update logic once or more times
	per frame to ensure that the logic is called as many times per second
	as the requested frame rate. On slow machines, frames may skip but
	the logic will work at the requested speed.

	\param bUseRealtimeFrameRateFlag The new flag
	\sa GetRealtimeFrameRateFlag(void) const

***************************************/

/*! ************************************

	\fn Word Burger::Flash::Manager::GetVerboseActionFlag(void)
	\brief Get the verbose action script debug flag

	If non-zero, the action script interpreter will output logging
	text to the console for debugging. 

	\return Zero if \ref FALSE or non-zero for \ref TRUE
	\sa SetVerboseActionFlag(Word)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::SetVerboseActionFlag(Word bVerboseActionFlag)
	\brief Set the verbose action script debug flag

	If non-zero, the action script interpreter will output logging
	text to the console for debugging. This should not be set for
	shipping code, it's a performance hit.

	\param bVerboseActionFlag The new flag
	\sa GetVerboseActionFlag(void) const

***************************************/

/*! ************************************

	\fn Word Burger::Flash::Manager::GetVerboseParsingFlag(void)
	\brief Get the verbose data parsing debug flag

	If non-zero, internal data parsing will output logging
	text to the console for debugging. 

	\return Zero if \ref FALSE or non-zero for \ref TRUE
	\sa SetVerboseParsingFlag(Word)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::SetVerboseParsingFlag(Word m_bVerboseParsingFlag)
	\brief Set the verbose action script debug flag

	If non-zero, internal data parsing will output logging
	text to the console for debugging. This should not be set for
	shipping code, it's a performance hit.

	\param m_bVerboseParsingFlag The new flag
	\sa GetVerboseParsingFlag(void) const

***************************************/

/*! ************************************

	\fn Word Burger::Flash::Manager::GetVerboseBitmapInfoFlag(void)
	\brief Get the bitmap generation debug flag

	If non-zero, bitmap generation will output logging
	text to the console for debugging. 

	\return Zero if \ref FALSE or non-zero for \ref TRUE
	\sa SetVerboseBitmapInfoFlag(Word)

***************************************/

/*! ************************************

	\fn void Burger::Flash::Manager::SetVerboseBitmapInfoFlag(Word bVerboseBitmapInfoFlag)
	\brief Set the bitmap generation debug flag

	If non-zero, bitmap generation parsing will output logging
	text to the console for debugging. This should not be set for
	shipping code, it's a performance hit.

	\param bVerboseBitmapInfoFlag The new flag
	\sa GetVerboseBitmapInfoFlag(void) const

***************************************/