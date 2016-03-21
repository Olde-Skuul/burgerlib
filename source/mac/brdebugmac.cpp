/***************************************

	Debug manager

	Copyright (c) 1995-2016 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#include "brdebug.h"

#if defined(BURGER_MAC)
#include "brstring16.h"
#include "broscursor.h"
#include "brcriticalsection.h"
#include "brfile.h"
#include <Dialogs.h>
#include <Quickdraw.h>

/***************************************

	Mac OS version

***************************************/

static Word8 Template[] = {
	0x00,0x01,			/* 2 items in the list */
	0x00,0x00,0x00,0x00,	/* Nothing*/
	0x00,160,0x00,141,0x00,180,0x00,209,	/* Rect for the OK button Width 68*/
	0x04,
	0x02,'O','K',
	0x00,0x00,0x00,0x00,
	0x00,20,0x00,20,0x00,140,0x01,330-256,	/* Width 310 */
	0x88,0x00
};

void BURGER_API Burger::OkAlertMessage(const char *pMessage,const char *pTitle)
{
	Word8 *TitleStr;		/* Pointer to the window title */
	DialogPtr MyDialog;	/* My dialog pointer */
	Handle ItemList;	/* Handle to the item list */
	Rect DialogRect;	/* Rect of the dialog window */
	Word TitleLen;		/* Length of the title */
	Word MessLen;		/* Length of the caption */
	short ItemHit;		/* Junk */
	Rect WorkRect;
	GrafPtr MyPort;	/* My grafport */
	//Word Foo;
	
	//Foo = InputSetState(FALSE);
		
	GetPort(&MyPort);	/* Save the current port */
	
	/* Center my dialog to the screen */
#if ACCESSOR_CALLS_ARE_FUNCTIONS
	GetPortBounds(MyPort,&WorkRect);
#else
	WorkRect = MyPort->portRect;
#endif
	DialogRect.top = static_cast<short>((((WorkRect.bottom-WorkRect.top)-190)/2)+WorkRect.top);
	DialogRect.left = static_cast<short>((((WorkRect.right-WorkRect.left)-350)/2)+WorkRect.left);
	DialogRect.bottom = static_cast<short>(DialogRect.top+190);
	DialogRect.right = static_cast<short>(DialogRect.left+350);

	TitleLen = 0;			/* Assume no length */
	if (pTitle) {
		TitleLen = Burger::StringLength(pTitle);		/* Get the length of the title string */
	}
	TitleStr = (Word8 *)Burger::Alloc(TitleLen+1);	/* Get memory of pascal string */
	if (TitleStr) {			/* Did I get the memory? */
		MemoryCopy(TitleStr+1,pTitle,TitleLen);
		TitleStr[0] = static_cast<Word8>(TitleLen);		/* Set the pascal length */
		
		MessLen = Burger::StringLength(pMessage);	/* Size of the message */
		ItemList = NewHandle(static_cast<Size>(sizeof(Template)+MessLen));
		if (ItemList) {				/* Ok? */
			Template[sizeof(Template)-1]=static_cast<Word8>(MessLen);	/* Save the message length */
			MemoryCopy(ItemList[0],Template,sizeof(Template));	/* Copy the template */
			MemoryCopy((ItemList[0])+sizeof(Template),pMessage,MessLen);	/* Copy the message */
			MyDialog = NewDialog(0,&DialogRect,(Word8 *)TitleStr,TRUE,5,(WindowPtr)-1,FALSE,0,ItemList);
			if (MyDialog) {
				SetDialogDefaultItem(MyDialog,1);	/* Default for OK button */
				ModalDialog(0,&ItemHit);			/* Handle the event */
				DisposeDialog(MyDialog);			/* Kill the dialog */
			} else {
				DisposeHandle(ItemList);			/* I must kill this myself! */
			}
		}
		Burger::Free(TitleStr);				/* Kill the title */
	}
	SetPort(MyPort);			/* Restore my grafport */
	//InputSetState(Foo);
}

/***************************************

	Mac OS version

***************************************/

static Word8 TemplateX[] = {
	0x00,0x02,			/* 3 items in the list */
	0x00,0x00,0x00,0x00,	/* Nothing*/
	0x00, 160,0x01, 260-256,0x00, 180,0x01, 328-256,	/* Rect for the OK button Width 68*/
	0x04,
	0x02,'O','K',
	0x00,0x00,0x00,0x00,	/* Nothing*/
	0x00, 160,0x00, 180,0x00, 180,0x00, 248,	/* Rect for the OK button Width 68*/
	0x04,
	0x06,'C','a','n','c','e','l',
	0x00,0x00,0x00,0x00,
	0x00,  20,0x00,  20,0x00, 140,0x01, 330-256,	/* Width 310 */
	0x88,0x00
};

Word BURGER_API Burger::OkCancelAlertMessage(const char *Message,const char *Title)
{
	Word Result;
	Word8 *TitleStr;		/* Pointer to the window title */
	DialogPtr MyDialog;	/* My dialog pointer */
	Handle ItemList;	/* Handle to the item list */
	Rect DialogRect;	/* Rect of the dialog window */
	Rect WorkRect;
	Word TitleLen;		/* Length of the title */
	Word MessLen;		/* Length of the caption */
	short ItemHit;		/* Junk */
	GrafPtr MyPort;	/* My grafport */
	//Word Foo;
	
	//Foo = InputSetState(FALSE);
	
	Result = FALSE;		/* Assume cancel */
	GetPort(&MyPort);	/* Save the current port */
	
	/* Center my dialog to the screen */
#if ACCESSOR_CALLS_ARE_FUNCTIONS
	GetPortBounds(MyPort,&WorkRect);
#else
	WorkRect = MyPort->portRect;
#endif
	DialogRect.top = static_cast<short>((((WorkRect.bottom-WorkRect.top)-190)/2)+WorkRect.top);
	DialogRect.left = static_cast<short>((((WorkRect.right-WorkRect.left)-350)/2)+WorkRect.left);
	DialogRect.bottom = static_cast<short>(DialogRect.top+190);
	DialogRect.right = static_cast<short>(DialogRect.left+350);

	TitleLen = 0;			/* Assume no length */
	if (Title) {
		TitleLen = Burger::StringLength(Title);		/* Get the length of the title string */
	}
	TitleStr = (Word8 *)Burger::Alloc(TitleLen+1);	/* Get memory of pascal string */
	if (TitleStr) {			/* Did I get the memory? */
		Burger::MemoryCopy(TitleStr+1,Title,TitleLen);
		TitleStr[0] = static_cast<Word8>(TitleLen);		/* Set the pascal length */
		
		MessLen = Burger::StringLength(Message);	/* Size of the message */
		ItemList = NewHandle(static_cast<Size>(sizeof(TemplateX)+MessLen));
		if (ItemList) {				/* Ok? */
			TemplateX[sizeof(TemplateX)-1]=static_cast<Word8>(MessLen);	/* Save the message length */
			Burger::MemoryCopy(ItemList[0],TemplateX,sizeof(TemplateX));	/* Copy the template */
			Burger::MemoryCopy((ItemList[0])+sizeof(TemplateX),Message,MessLen);	/* Copy the message */
			MyDialog = NewDialog(0,&DialogRect,(Word8 *)TitleStr,TRUE,5,(WindowPtr)-1,FALSE,0,ItemList);
			if (MyDialog) {
				SetDialogDefaultItem(MyDialog,1);	/* Default for OK button */
				SetDialogCancelItem(MyDialog,2);	/* Default for cancel button */
				ModalDialog(0,&ItemHit);			/* Handle the event */
				if (ItemHit==1) {		/* Pressed ok? */
					Result = TRUE;
				}
				DisposeDialog(MyDialog);			/* Kill the dialog */
			} else {
				DisposeHandle(ItemList);			/* I must kill this myself! */
			}
		}
		Burger::Free(TitleStr);				/* Kill the title */
	}
	SetPort(MyPort);			/* Restore my grafport */
	//InputSetState(Foo);			/* Restore Inputsprocket */
	return Result;
}

#endif