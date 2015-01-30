/***************************************

	Windows application manager

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRWINDOWSAPP_H__
#define __BRWINDOWSAPP_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRGAMEAPP_H__
#include "brgameapp.h"
#endif

#if defined(BURGER_WINDOWS) && !defined(__BRWINDOWSTYPES_H__)
#include "brwindowstypes.h"
#endif

/* BEGIN */
#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
namespace Burger {
class WindowsApp : public GameApp {
public:
	typedef Word (BURGER_API *MainWindowProc)(WindowsApp *pThis,HWND__ *hWnd,Word uMessage,WordPtr wParam,WordPtr lParam,WordPtr *pOutput);	///< Callback custom window message handler
private:
	HINSTANCE__ *m_hInstance;		///< Current instance of the application
	HWND__ *m_hWindow;				///< Main window for the application
	HICON__ *m_pDefaultCursor;		///< Default cursor when not in the game area
	const char **m_ppOldArgv;		///< Saved pointer to the global __argv for restoring on exit
	MainWindowProc m_pCallBack;		///< Custom application window proc
	int m_iPreviousWindowX;			///< Previous X coordinate of the window
	int m_iPreviousWindowY;			///< Previous Y coordinate of the window
	Word m_bPreviousWindowXYValid;	///< \ref TRUE if m_iPreviousWindowX was initialized
	Word16 m_uAtom;					///< Atom assigned to my class
	BURGER_DISABLECOPYCONSTRUCTORS(WindowsApp);
public:
	static void BURGER_API OutputWindowsMessage(Word uMessage,WordPtr wParam,WordPtr lParam);
	WindowsApp(HINSTANCE__ *hInstance,const char *pGameName,MainWindowProc pCallBack=NULL,WordPtr uDefaultMemorySize=Burger::MemoryManagerHandle::DEFAULTMEMORYCHUNK,Word uDefaultHandleCount=Burger::MemoryManagerHandle::DEFAULTHANDLECOUNT,WordPtr uMinReserveSize=Burger::MemoryManagerHandle::DEFAULTMINIMUMRESERVE);
	~WindowsApp();
	BURGER_INLINE HINSTANCE__ *GetInstance(void) const { return m_hInstance; }
	BURGER_INLINE HWND__ *GetWindow(void) const { return m_hWindow; }
	BURGER_INLINE MainWindowProc GetCallBack(void) const { return m_pCallBack; }
	BURGER_INLINE void ResetWindowLocation(void) { m_bPreviousWindowXYValid = FALSE; }
	int BURGER_API InitWindow(const char *pGameName,MainWindowProc pCallBack);
	int BURGER_API SetWindowSize(Word uWidth,Word uHeight);
	void BURGER_API RecordWindowLocation(void);
	Word BURGER_API HandleCursor(Word uParam);
	void BURGER_API GetInputFocus(void);
	void BURGER_API KillInputFocus(void);
	static RunQueue::eReturnCode BURGER_API Poll(void *pSelf);
};
}
#endif
/* END */

#endif
