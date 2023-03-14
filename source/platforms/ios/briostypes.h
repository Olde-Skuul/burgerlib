/***************************************

	Typedefs specific to iOS

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE for
	license details. Yes, you can use it in a commercial title without paying
	anything, just give me a credit.

	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRIOSTYPES_H__
#define __BRIOSTYPES_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#if defined(BURGER_DARWIN) && !defined(__BRDARWINTYPES_H__)
#include "brdarwintypes.h"
#endif

/* BEGIN */
#if defined(BURGER_IOS) && !defined(DOXYGEN)

@class EAGLContext;
@class UIView;
@class UIWindow;
@class UIImage;
@class UIImageView;
@class UIViewController;

#endif
/* END */

#endif
