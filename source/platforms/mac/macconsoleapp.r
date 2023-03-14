/***************************************

	Burgerlib resource definitions
	
	Define common resource file types
	Used only for MacOS and MacOSX targets
	with Apple's Rez compiler
	
	Copyright 1995-2014 by Rebecca Ann Heineman becky@burgerbecky.com

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
	
***************************************/

#ifndef __BURGER_R__
#include <burger.r>
#endif

#ifndef __CONDITIONALMACROS_R__
#include <ConditionalMacros.r>
#endif

#ifndef __SCRIPT_R__
#include <Script.r>     // to get Region codes for 'vers' resource
#endif

#ifndef __MACTYPES_R__
#include <MacTypes.r>
#endif

#ifndef __BALLOONS_R__
#include <Balloons.r>
#endif

#ifndef __FINDER_R__
#include <Finder.r>
#endif

#ifndef __PICTUTILS_R__
#include <PictUtils.r>
#endif

#ifndef __QUICKDRAW_R__
#include <Quickdraw.r>
#endif

#ifndef __ICONS_R__
#include <Icons.r>
#endif

/***************************************

	Initialize defaults
	
***************************************/

#if !defined(APPDESCRIPTION)
#define APPDESCRIPTION ""
#endif

#if !defined(APPVERSIONBUILD)
#define APPVERSIONBUILD 0
#endif

#if !defined(APPVERSIONSTRING)
#define APPVERSIONSTRING "v1.0"
#define APPVERSIONMAJOR 1
#define APPVERSIONMINOR 0
#endif

#if !defined(APPSHORTCOPYRIGHT) && defined(APPCOPYRIGHT)
#define APPSHORTCOPYRIGHT APPCOPYRIGHT
#endif

#if !defined(APPREGION)
#define APPREGION verUS
#endif

/***************************************

	Create the application owner resource
	APPID = 'abcd' where abcd is the four letter 
	creator code
	
***************************************/

#if defined(APPID)
data APPID ( 0 ) {		// No data, just it's existance is all it needs
};
#endif

/***************************************

	If a Balloon help string is requested, create it
	
***************************************/

#if defined(APPBALLOONHELP)
resource 'hfdr' (kHMHelpID)
{
	hmBalloonHelpVersion,	// Current version
	hmDefaultOptions,		// Nothing special
	0,						// No custom ProcID
	0,						// No special data to be passed to the procedure
	{
		HMStringItem {		// Text string
			APPBALLOONHELP	// Insert the string here
		}
	}
};
#endif

/***************************************

	If this is a console app and it
	wants drag and drop functionality,
	create a simple finder reference so
	MacOS knows it's okay to send data 
	down the app's throat.
	
***************************************/

// Detect an error condition. You can't create
// a drag and drop file heuristic without a valid APPID

#if defined(APPDRAGANDDROP) && !defined(APPID)
#error APPDRAGANDDROP needs APPID defined to function. Please define APPID
#endif

#if defined(APPDRAGANDDROP)
#define kBundleIDApplication 128		// Application FREF ID
#define kBundleIDTextDocument 129		// Data FREF ID

resource 'BNDL' (128) {
	APPID,		// Signature
	0,			// Version #0
	{
		'FREF', {
			kBundleIDApplication,kBundleIDApplication,	// Local ID/Global ID
			kBundleIDTextDocument,kBundleIDTextDocument
		},
		'ICN#', {
			kBundleIDApplication,kBundleIDApplication,	// Local ID/Global ID
			kBundleIDTextDocument,kBundleIDTextDocument
		}
	}
};

resource 'FREF' (kBundleIDApplication) {
	'APPL',						// ID For the App
	kBundleIDApplication,		// Icon for the app, if any
	APPDESCRIPTION				// Application description
};

resource 'FREF' (kBundleIDTextDocument) {
	'****',						// Any data
	kBundleIDTextDocument,		// Data ID
	""							// No description on wildcard data
};

#endif

/***************************************

	Create a version resource if requested
	Note: APPVERSIONMAJOR and APPVERSIONMINOR are BCD style,
	so 1.2.2 = APPVERSIONMAJOR 1, APPVERSIONMINOR 0x22
	
***************************************/

#if defined(APPCOPYRIGHT)

resource 'vers' (1)
{
	APPVERSIONMAJOR, APPVERSIONMINOR, final, APPVERSIONBUILD,		//	1.0f
	APPREGION,
	APPVERSIONSTRING,
	APPVERSIONSTRING ", " APPCOPYRIGHT
};

resource 'vers' (2)
{
	APPVERSIONMAJOR, APPVERSIONMINOR, final, APPVERSIONBUILD,		//	1.0f
	APPREGION,
	APPVERSIONSTRING,
	APPVERSIONSTRING ", " APPSHORTCOPYRIGHT
};

#endif