/***************************************

	Flash player manager
	
	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BRFLASHMANAGER_H__
#define __BRFLASHMANAGER_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRRANDOMMANAGER_H__
#include "brrandommanager.h"
#endif

#ifndef __BRFILENAME_H__
#include "brfilename.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#ifndef __BRHASHMAP_H__
#include "brhashmap.h"
#endif

#ifndef __BRCODELIBRARY_H__
#include "brcodelibrary.h"
#endif

#ifndef __BRSMARTPOINTER_H__
#include "brsmartpointer.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
class CharacterObject;
class FunctionCallParms;
class RootObject;
class ActionScriptObject;
typedef Word (BURGER_API *FSCommandProc)(CharacterObject *pMovie,const char *pCommand,const char *pArgs);		///< Function prototype for user supplied FSCommand function
typedef void (BURGER_API *ActionScriptCFunctionProc)(const FunctionCallParms *pParms);				///< Function prototype to implement an ActionScript command
class Manager {
	BURGER_DISABLECOPYCONSTRUCTORS(Manager);
	GameApp *m_pGameApp;				///< Pointer to the application instance
	FSCommandProc m_pFSCommmand;		///< Callback for FSCommand support
	WeakPointer<RootObject> m_pCurrentObject;
	float m_fTextureLODBias;			///< Texture Level of detail bias
	float m_fCurvePixelError;			///< Allowable error on generating curves. Higher numbers means coarser curves.
	Word m_bUseRealtimeFrameRateFlag;	///< Set to \ref TRUE if flash logic MUST be called once per frame in slow situations
	Word m_bVerboseActionFlag;			///< Print debug messages for Action Script
	Word m_bVerboseParsingFlag;			///< Print debug messages for data parsing
	Word m_bVerboseBitmapInfoFlag;		///< Print debug messages for bitmap information
	Word m_bAllowMultithreadingFlag;	///< \ref TRUE if multithreading is allowed
	Random m_Random;					///< Random number generator instance
	Filename m_BaseDirectory;			///< Directory to load files from
	HashMapString<CodeLibrary*> m_CodeLibraryHash;		///< Hash of code libraries and pointers to their data
	String m_GlobalEnvironmentVariables;	///< A list of environment variables to pass to all Flash movies
	const String m_ConstructorName;		///< "__constructor__" global for internal use
	CriticalSection m_CriticalSection;	///< Critical section for the flash player
public:
	Manager(GameApp *pApp);
	~Manager();
	BURGER_INLINE GameApp *GetGameApp(void) const { return m_pGameApp; }
	BURGER_INLINE FSCommandProc GetFSCallback(void) const { return m_pFSCommmand; }
	BURGER_INLINE void SetFSCallback(FSCommandProc pFSCommand) { m_pFSCommmand = pFSCommand; }
	BURGER_INLINE RootObject* GetRootObject(void) const { return m_pCurrentObject.GetPtr(); }
	void BURGER_API SetRootObject(RootObject *pObject);
	BURGER_INLINE Random *GetRandomGenerator(void) { return &m_Random; }
	BURGER_INLINE Filename *GetDataDirectory(void) { return &m_BaseDirectory; }
	BURGER_INLINE void SetDataDirectory(const char *pDirectory) { m_BaseDirectory.Set(pDirectory); }
	BURGER_INLINE float GetLODBias(void) const { return m_fTextureLODBias; }
	void BURGER_API SetLODBias(float fTextureLODBias=-1.2f);
	BURGER_INLINE float GetCurvePixelError(void) const { return m_fCurvePixelError; }
	void BURGER_API SetCurvePixelError(float fCurvePixelError=1.0f);
	BURGER_INLINE Word GetRealtimeFrameRateFlag(void) const { return m_bUseRealtimeFrameRateFlag; }
	BURGER_INLINE void SetRealtimeFrameRateFlag(Word bUseRealtimeFrameRateFlag) { m_bUseRealtimeFrameRateFlag = bUseRealtimeFrameRateFlag; }
	BURGER_INLINE Word GetVerboseActionFlag(void) const { return m_bVerboseActionFlag; }
	BURGER_INLINE void SetVerboseActionFlag(Word bVerboseActionFlag) { m_bVerboseActionFlag = bVerboseActionFlag; }
	BURGER_INLINE Word GetVerboseParsingFlag(void) const { return m_bVerboseParsingFlag; }
	BURGER_INLINE void SetVerboseParsingFlag(Word bVerboseParsingFlag) { m_bVerboseParsingFlag = bVerboseParsingFlag; }
	BURGER_INLINE Word GetVerboseBitmapInfoFlag(void) const { return m_bVerboseBitmapInfoFlag; }
	BURGER_INLINE void SetVerboseBitmapInfoFlag(Word bVerboseBitmapInfoFlag) { m_bVerboseBitmapInfoFlag = bVerboseBitmapInfoFlag; }
	BURGER_INLINE Word GetAllowMultithreadingFlag(void) const { return m_bAllowMultithreadingFlag; }
	BURGER_INLINE void SetAllowMultithreadingFlag(Word bAllowMultithreadingFlag) { m_bAllowMultithreadingFlag = bAllowMultithreadingFlag; }
	CodeLibrary * BURGER_API LoadCodeLibrary(const String *pName);
	void BURGER_API ReleaseCodeLibraries(void);
	BURGER_INLINE const String *GetGlobalEnvironmentVariables(void) const { return &m_GlobalEnvironmentVariables; }
	void BURGER_API SetGlobalEnvironmentVariables(const char *pInput);
	BURGER_INLINE const String *GetConstructorName(void) const { return &m_ConstructorName; }
	BURGER_INLINE void Lock(void) { m_CriticalSection.Lock(); }
	BURGER_INLINE void Unlock(void) { m_CriticalSection.Unlock(); }
};
}}
/* END */

#endif
