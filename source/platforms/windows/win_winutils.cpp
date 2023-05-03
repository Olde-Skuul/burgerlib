/***************************************

	Window helper functions for Microsoft Windows

	Copyright (c) 1995-2023 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#include "win_winutils.h"

#if defined(BURGER_WINDOWS) || defined(DOXYGEN)
#include "brdebug.h"
#include "brnumberto.h"
#include "brstringfunctions.h"

#if !defined(DOXYGEN)
//
// Handle some annoying defines that some windows SDKs may or may not have
//

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x0501 // Windows XP
#endif

#include <Windows.h>

//
// These defines are missing from some versions of windows.h
// especially when building against older versions of the
// windows SDK (Necessary, since some obscure compilers
// don't ship with up to date headers)
//

#if !defined(WM_KEYF1)
#define WM_KEYF1 0x004D
#endif

#if !defined(WM_UAHDESTROYWINDOW)
#define WM_UAHDESTROYWINDOW 0x0090
#endif

#if !defined(WM_UAHDRAWMENU)
#define WM_UAHDRAWMENU 0x0091
#endif

#if !defined(WM_UAHDRAWMENUITEM)
#define WM_UAHDRAWMENUITEM 0x0092
#endif

#if !defined(WM_UAHINITMENU)
#define WM_UAHINITMENU 0x0093
#endif

#if !defined(WM_UAHMEASUREMENUITEM)
#define WM_UAHMEASUREMENUITEM 0x0094
#endif

#if !defined(WM_UAHNCPAINTMENUPOPUP)
#define WM_UAHNCPAINTMENUPOPUP 0x0095
#endif

#if !defined(WM_NCUAHDRAWCAPTION)
#define WM_NCUAHDRAWCAPTION 0x00AE
#endif

#if !defined(WM_NCUAHDRAWFRAME)
#define WM_NCUAHDRAWFRAME 0x00AF
#endif

#if !defined(WM_INPUT_DEVICE_CHANGE)
#define WM_INPUT_DEVICE_CHANGE 0x00FE
#endif

#if !defined(WM_INPUT)
#define WM_INPUT 0x00FF
#endif

#if !defined(WM_GESTURE)
#define WM_GESTURE 0x0119
#endif

#if !defined(WM_GESTURENOTIFY)
#define WM_GESTURENOTIFY 0x011A
#endif

#if !defined(WM_MOUSEHWHEEL)
#define WM_MOUSEHWHEEL 0x020E
#endif

#if !defined(WM_TOUCH)
#define WM_TOUCH 0x0240
#endif

#if !defined(WM_NCMOUSEHOVER)
#define WM_NCMOUSEHOVER 0x02A0
#endif

#if !defined(WM_NCMOUSELEAVE)
#define WM_NCMOUSELEAVE 0x02A2
#endif

#if !defined(WM_CLIPBOARDUPDATE)
#define WM_CLIPBOARDUPDATE 0x031D
#endif

#if !defined(WM_DWMCOMPOSITIONCHANGED)
#define WM_DWMCOMPOSITIONCHANGED 0x031E
#endif

#if !defined(WM_DWMNCRENDERINGCHANGED)
#define WM_DWMNCRENDERINGCHANGED 0x031F
#endif

#if !defined(WM_DWMCOLORIZATIONCOLORCHANGED)
#define WM_DWMCOLORIZATIONCOLORCHANGED 0x0320
#endif

#if !defined(WM_DWMWINDOWMAXIMIZEDCHANGE)
#define WM_DWMWINDOWMAXIMIZEDCHANGE 0x0321
#endif

// Internal variables

// Main window for the application
static HWND__* s_hWindow;

#endif

/*! ************************************

	\brief Get the application window

	\windowsonly

	\return Window set by set_window()

	\sa set_window()

***************************************/

HWND__* BURGER_API Burger::Win32::get_window(void) BURGER_NOEXCEPT
{
	return s_hWindow;
}

/*! ************************************

	\brief Set the application window

	A window is created on application startup, it's pointer
	is stored via this call so other parts of Burgerlib
	can use this window for other systems.

	\windowsonly

	\param pInput Instance of the application

	\sa get_window()

***************************************/

void BURGER_API Burger::Win32::set_window(HWND__* pInput) BURGER_NOEXCEPT
{
	s_hWindow = pInput;
}

/*! ************************************

	\brief Get the registered class name

	When registering a window class for Burgerlib, this is the name
	used to declare it. It's a wchar_t string of "BurgerGameClass"

	\windowsonly

	\return Pointer to a wchar_t * compatible pointer

	\sa Globals::RegisterWindowClass(uint_t)

***************************************/

const uint16_t* BURGER_API Burger::Win32::get_window_class_name(
	void) BURGER_NOEXCEPT
{
	// "BurgerGameClass";
	static const uint16_t s_GameClass[] = {'B', 'u', 'r', 'g', 'e', 'r', 'G',
		'a', 'm', 'e', 'C', 'l', 'a', 's', 's', 0};

	return s_GameClass;
}

/*! ************************************

	\brief Change the style flags of a windows

	Set and clear the style and extended style flags. The flags to clear will be
	bit flipped before applying an AND operation on the bits.

	\windowsonly

	\param hWindow HWND of the window
	\param uAddStyle Bits to set in the GWL_STYLE entry
	\param uAddStyleEx Bits to set in the GWL_EXSTYLE entry
	\param uSubStyle Bits to clear in the GWL_STYLE entry
	\param uSubStyleEx Bits to clear in the GWL_EXSTYLE entry

***************************************/

void BURGER_API Burger::Win32::change_style(HWND__* hWindow, uint32_t uAddStyle,
	uint32_t uAddStyleEx, uint32_t uSubStyle,
	uint32_t uSubStyleEx) BURGER_NOEXCEPT
{
	SetWindowLongW(hWindow, GWL_STYLE,
		static_cast<LONG>(
			(GetWindowLongW(hWindow, GWL_STYLE) | uAddStyle) & (~uSubStyle)));
	SetWindowLongW(hWindow, GWL_EXSTYLE,
		static_cast<LONG>((GetWindowLongW(hWindow, GWL_EXSTYLE) | uAddStyleEx) &
			(~uSubStyleEx)));
}

/*! ************************************

	\brief Pump windows messages

	Windows requires a function to pump messages

	\windowsonly

	\sa RegisterWindowClass(uint_t)

***************************************/

void BURGER_API Burger::Win32::pump_messages(void) BURGER_NOEXCEPT
{
	MSG TempMessage;
	while (PeekMessageW(&TempMessage, nullptr, 0, 0, PM_REMOVE)) {

		// Translate the keyboard (Localize)
		TranslateMessage(&TempMessage);

		// Pass to the window event proc
		DispatchMessageW(&TempMessage);
	}
}

/*! ************************************

	\brief Print windows events to Debug::Message()

	This helper function will take a windows Window Event and print it to the
	debugging message system to allow a programmer to trace events going through
	a window procedure. This should not be called in released code.

	\windowsonly

	\param uMessage Windows message enumeration
	\param wParam wParam value passed to the windows callback
	\param lParam lParam value passed to the windows callback

	\sa Debug::Message()
	
***************************************/

#if !defined(DOXYGEN)
#define CASE(x) \
	{ \
#x, x \
	}

struct MessageLookup_t {
	const char* m_pName; // String of the enum
	uint_t m_uEnum;      // Enum value that matched the string
};

//
// All known event messages for a window are here.
//

static const MessageLookup_t s_MessageLookup[] = {CASE(WM_NULL),
	CASE(WM_CREATE), CASE(WM_DESTROY), CASE(WM_MOVE), CASE(WM_SIZE),
	CASE(WM_ACTIVATE), CASE(WM_SETFOCUS), CASE(WM_KILLFOCUS), CASE(WM_ENABLE),
	CASE(WM_SETREDRAW), CASE(WM_SETTEXT), CASE(WM_GETTEXT),
	CASE(WM_GETTEXTLENGTH), CASE(WM_PAINT), CASE(WM_CLOSE),
	CASE(WM_QUERYENDSESSION), CASE(WM_QUIT), CASE(WM_QUERYOPEN),
	CASE(WM_ERASEBKGND), CASE(WM_SYSCOLORCHANGE), CASE(WM_SHOWWINDOW),
	CASE(WM_SETTINGCHANGE), CASE(WM_DEVMODECHANGE), CASE(WM_ACTIVATEAPP),
	CASE(WM_FONTCHANGE), CASE(WM_TIMECHANGE), CASE(WM_CANCELMODE),
	CASE(WM_SETCURSOR), CASE(WM_MOUSEACTIVATE), CASE(WM_CHILDACTIVATE),
	CASE(WM_QUEUESYNC), CASE(WM_GETMINMAXINFO), CASE(WM_PAINTICON),
	CASE(WM_ICONERASEBKGND), CASE(WM_NEXTDLGCTL), CASE(WM_SPOOLERSTATUS),
	CASE(WM_DRAWITEM), CASE(WM_MEASUREITEM), CASE(WM_DELETEITEM),
	CASE(WM_VKEYTOITEM), CASE(WM_CHARTOITEM), CASE(WM_SETFONT),
	CASE(WM_GETFONT), CASE(WM_SETHOTKEY), CASE(WM_GETHOTKEY),
	CASE(WM_QUERYDRAGICON), CASE(WM_COMPAREITEM), CASE(WM_GETOBJECT),
	CASE(WM_COMPACTING), CASE(WM_COMMNOTIFY), CASE(WM_WINDOWPOSCHANGING),
	CASE(WM_WINDOWPOSCHANGED), CASE(WM_POWER), CASE(WM_COPYDATA),
	CASE(WM_CANCELJOURNAL), CASE(WM_KEYF1), CASE(WM_NOTIFY),
	CASE(WM_INPUTLANGCHANGEREQUEST), CASE(WM_INPUTLANGCHANGE), CASE(WM_TCARD),
	CASE(WM_HELP), CASE(WM_USERCHANGED), CASE(WM_NOTIFYFORMAT),
	CASE(WM_CONTEXTMENU), CASE(WM_STYLECHANGING), CASE(WM_STYLECHANGED),
	CASE(WM_DISPLAYCHANGE), CASE(WM_GETICON), CASE(WM_SETICON),
	CASE(WM_NCCREATE), CASE(WM_NCDESTROY), CASE(WM_NCCALCSIZE),
	CASE(WM_NCHITTEST), CASE(WM_NCPAINT), CASE(WM_NCACTIVATE),
	CASE(WM_GETDLGCODE), CASE(WM_SYNCPAINT), CASE(WM_UAHDESTROYWINDOW),
	CASE(WM_UAHDRAWMENU), CASE(WM_UAHDRAWMENUITEM), CASE(WM_UAHINITMENU),
	CASE(WM_UAHMEASUREMENUITEM), CASE(WM_UAHNCPAINTMENUPOPUP),
	CASE(WM_NCMOUSEMOVE), CASE(WM_NCLBUTTONDOWN), CASE(WM_NCLBUTTONUP),
	CASE(WM_NCLBUTTONDBLCLK), CASE(WM_NCRBUTTONDOWN), CASE(WM_NCRBUTTONUP),
	CASE(WM_NCRBUTTONDBLCLK), CASE(WM_NCMBUTTONDOWN), CASE(WM_NCMBUTTONUP),
	CASE(WM_NCMBUTTONDBLCLK), CASE(WM_NCXBUTTONDOWN), CASE(WM_NCXBUTTONUP),
	CASE(WM_NCXBUTTONDBLCLK), CASE(WM_NCUAHDRAWCAPTION),
	CASE(WM_NCUAHDRAWFRAME), CASE(WM_INPUT_DEVICE_CHANGE), CASE(WM_INPUT),
	CASE(WM_KEYDOWN), CASE(WM_KEYUP), CASE(WM_CHAR), CASE(WM_DEADCHAR),
	CASE(WM_SYSKEYDOWN), CASE(WM_SYSKEYUP), CASE(WM_SYSCHAR),
	CASE(WM_SYSDEADCHAR), CASE(WM_UNICHAR), CASE(WM_IME_STARTCOMPOSITION),
	CASE(WM_IME_ENDCOMPOSITION), CASE(WM_IME_COMPOSITION), CASE(WM_INITDIALOG),
	CASE(WM_COMMAND), CASE(WM_SYSCOMMAND), CASE(WM_TIMER), CASE(WM_HSCROLL),
	CASE(WM_VSCROLL), CASE(WM_INITMENU), CASE(WM_INITMENUPOPUP),
	CASE(WM_GESTURE), CASE(WM_GESTURENOTIFY), CASE(WM_MENUSELECT),
	CASE(WM_MENUCHAR), CASE(WM_ENTERIDLE), CASE(WM_MENURBUTTONUP),
	CASE(WM_MENUDRAG), CASE(WM_MENUGETOBJECT), CASE(WM_UNINITMENUPOPUP),
	CASE(WM_MENUCOMMAND), CASE(WM_CHANGEUISTATE), CASE(WM_UPDATEUISTATE),
	CASE(WM_QUERYUISTATE), CASE(WM_CTLCOLORMSGBOX), CASE(WM_CTLCOLOREDIT),
	CASE(WM_CTLCOLORLISTBOX), CASE(WM_CTLCOLORBTN), CASE(WM_CTLCOLORDLG),
	CASE(WM_CTLCOLORSCROLLBAR), CASE(WM_CTLCOLORSTATIC), CASE(MN_GETHMENU),
	CASE(WM_MOUSEMOVE), CASE(WM_LBUTTONDOWN), CASE(WM_LBUTTONUP),
	CASE(WM_LBUTTONDBLCLK), CASE(WM_RBUTTONDOWN), CASE(WM_RBUTTONUP),
	CASE(WM_RBUTTONDBLCLK), CASE(WM_MBUTTONDOWN), CASE(WM_MBUTTONUP),
	CASE(WM_MBUTTONDBLCLK), CASE(WM_MOUSEWHEEL), CASE(WM_XBUTTONDOWN),
	CASE(WM_XBUTTONUP), CASE(WM_XBUTTONDBLCLK), CASE(WM_MOUSEHWHEEL),
	CASE(WM_PARENTNOTIFY), CASE(WM_ENTERMENULOOP), CASE(WM_EXITMENULOOP),
	CASE(WM_NEXTMENU), CASE(WM_SIZING), CASE(WM_CAPTURECHANGED),
	CASE(WM_MOVING), CASE(WM_POWERBROADCAST), CASE(WM_DEVICECHANGE),
	CASE(WM_MDICREATE), CASE(WM_MDIDESTROY), CASE(WM_MDIACTIVATE),
	CASE(WM_MDIRESTORE), CASE(WM_MDINEXT), CASE(WM_MDIMAXIMIZE),
	CASE(WM_MDITILE), CASE(WM_MDICASCADE), CASE(WM_MDIICONARRANGE),
	CASE(WM_MDIGETACTIVE), CASE(WM_MDISETMENU), CASE(WM_ENTERSIZEMOVE),
	CASE(WM_EXITSIZEMOVE), CASE(WM_DROPFILES), CASE(WM_MDIREFRESHMENU),
	CASE(WM_TOUCH), CASE(WM_IME_SETCONTEXT), CASE(WM_IME_NOTIFY),
	CASE(WM_IME_CONTROL), CASE(WM_IME_COMPOSITIONFULL), CASE(WM_IME_SELECT),
	CASE(WM_IME_CHAR), CASE(WM_IME_REQUEST), CASE(WM_IME_KEYDOWN),
	CASE(WM_IME_KEYUP), CASE(WM_NCMOUSEHOVER), CASE(WM_MOUSEHOVER),
	CASE(WM_NCMOUSELEAVE), CASE(WM_MOUSELEAVE), CASE(WM_WTSSESSION_CHANGE),
	CASE(WM_CUT), CASE(WM_COPY), CASE(WM_PASTE), CASE(WM_CLEAR), CASE(WM_UNDO),
	CASE(WM_RENDERFORMAT), CASE(WM_RENDERALLFORMATS), CASE(WM_DESTROYCLIPBOARD),
	CASE(WM_DRAWCLIPBOARD), CASE(WM_PAINTCLIPBOARD), CASE(WM_VSCROLLCLIPBOARD),
	CASE(WM_SIZECLIPBOARD), CASE(WM_ASKCBFORMATNAME), CASE(WM_CHANGECBCHAIN),
	CASE(WM_HSCROLLCLIPBOARD), CASE(WM_QUERYNEWPALETTE),
	CASE(WM_PALETTEISCHANGING), CASE(WM_PALETTECHANGED), CASE(WM_HOTKEY),
	CASE(WM_PRINT), CASE(WM_PRINTCLIENT), CASE(WM_APPCOMMAND),
	CASE(WM_THEMECHANGED), CASE(WM_CLIPBOARDUPDATE),
	CASE(WM_DWMCOMPOSITIONCHANGED), CASE(WM_DWMNCRENDERINGCHANGED),
	CASE(WM_DWMCOLORIZATIONCOLORCHANGED), CASE(WM_DWMWINDOWMAXIMIZEDCHANGE)};
#endif

void BURGER_API Burger::Win32::output_windows_message(
	uint_t uMessage, uintptr_t wParam, uintptr_t lParam) BURGER_NOEXCEPT
{
	// Static global value containing the number of times this function was
	// called
	static uint32_t uMessageCount = 0;

	// Scan the table for a match
	uintptr_t uCount = BURGER_ARRAYSIZE(s_MessageLookup);
	const MessageLookup_t* pLookup = s_MessageLookup;
	const char* pMessage = nullptr;
	do {
		if (uMessage == pLookup->m_uEnum) {
			pMessage = pLookup->m_pName;
			break;
		}
		++pLookup;
	} while (--uCount);

	// If a message wasn't found, convert the unknown code to an
	// ASCII message

	char HexAsASCII[32];
	if (!pMessage) {
		NumberToAsciiHex(
			HexAsASCII, static_cast<uint32_t>(uMessage), LEADINGZEROS | 8);
		pMessage = HexAsASCII;
	}
	// Output the message and parameter values. It's not 64 bit clean for the
	// parameters, however, this is only for information to give clues on how
	// events are sent to a window from the operating system

	Debug::Message("Message %08X is %s with parms %08X, %08X\n", uMessageCount,
		pMessage, static_cast<uint_t>(wParam), static_cast<uint_t>(lParam));
	++uMessageCount;
}

#endif
