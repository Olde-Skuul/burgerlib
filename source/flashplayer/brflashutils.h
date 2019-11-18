/***************************************

	Flash player utility functions
	
	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!
		
***************************************/

#ifndef __BRFLASHUTILS_H__
#define __BRFLASHUTILS_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRVECTOR2D_H__
#include "brvector2d.h"
#endif

#ifndef __BRSTATICRTTI_H__
#include "brstaticrtti.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRFILENAME_H__
#include "brfilename.h"
#endif

/* BEGIN */
namespace Burger { namespace Flash {
class CharacterObject;
enum eFileType {
	UNKNOWN,		///< Unknown file
	SWF,			///< Flash file
	JPG,			///< Jpeg file format
	X3DS,			///< 3D files
	TXT,			///< Raw text file
	URL				///< Web pages (XML)
};
enum eActionScriptStandardMember {
	M_INVALID_MEMBER = -1,		///< Error in the function Burger::Flash::EnumerateStandardMember(const char *)
	M_X,				///< "_x"
	M_Y,				///< "_y"
	M_XSCALE,			///< "_xscale"
	M_YSCALE,			///< "_yscale"
	M_CURRENTFRAME,		///< "_currentframe"
	M_TOTALFRAMES,		///< "_totalframes"
	M_ALPHA,			///< "_alpha"
	M_VISIBLE,			///< "_visible"
	M_WIDTH,			///< "_width"
	M_HEIGHT,			///< "_height"
	M_ROTATION,			///< "_rotation"	
	M_TARGET,			///< "_target"
	M_FRAMESLOADED,		///< "_framesloaded"
	M_NAME,				///< "_name"
	M_DROPTARGET,		///< "_droptarget"
	M_URL,				///< "_url"
	M_HIGHQUALITY,		///< "_highquality"
	M_FOCUSRECT,		///< "_focusrect"
	M_SOUNDBUFTIME,		///< "_soundbuftime"
	M_XMOUSE,			///< "_xmouse"
	M_YMOUSE,			///< "_ymouse"
	M_PARENT,			///< "_parent"
	M_TEXT,				///< "text"
	M_TEXTWIDTH,		///< "textWidth"
	M_TEXTCOLOR,		///< "textColor"
	M_BORDER,			///< "border"
	M_MULTILINE,		///< "multiline"
	M_WORDWRAP,			///< "wordWrap"
	M_TYPE,				///< "type"
	M_BACKGROUNDCOLOR,	///< "backgroundColor"
	M_THIS,				///< "_this"
	MTHIS,				///< "this"
	M_ROOT,				///< "_root"
	MDOT,				///< "."
	MDOT2,				///< ".."
	M_LEVEL0,			///< "_level0"
	M_GLOBAL,			///< "_global"	
	M_ENABLED,			///< "enabled"
	M_PASSWORD,			///< "password"
	M_MOUSE_MOVE,		///< "onMouseMove"
	ACTION_SCRIPT_STANDARD_MEMBER_COUNT	///< Size of the array
};

extern const char * BURGER_API GetVersion(void);
BURGER_INLINE float TwipsToPixels(float fInput) { return fInput * (1.0f / 20.f); }
BURGER_INLINE float PixelsToTwips(float fInput) { return fInput * 20.f; }
extern void BURGER_API TwipsToPixels(Vector2D_t *pInput);
extern void BURGER_API PixelsToTwips(Vector2D_t *pInput);
extern Word32 BURGER_API TextureSizePower2(Word32 uInput);
extern WordPtr BURGER_API GetEncodedU32Length(Word32 *pOutput,const Word8 *pInput);
extern eActionScriptStandardMember BURGER_API EnumerateStandardMember(const char *pName);
extern eFileType BURGER_API GetFileType(const char *pFilename);
extern void BURGER_API GetFullURL(Filename *pOutput,const Filename *pDirectory,const char *pInput);
class Multiname {
public:
	enum eKind {
		CONSTANT_UNDEFINED = 0x00,	///< Uninitialized data
		CONSTANT_QName = 0x07,		///< QName object
		CONSTANT_QNameA = 0x0D,		///< QName object for attributes
		CONSTANT_RTQName = 0x0F,	///< RTQName object
		CONSTANT_RTQNameA = 0x10,	///< RTQName object for attributes
		CONSTANT_RTQNameL = 0x11,	///< RTQName object for local data
		CONSTANT_RTQNameLA = 0x12,	///< RTQName object for local attributes
		CONSTANT_Multiname = 0x09,	///< Multiname object for namespace
		CONSTANT_MultinameA = 0x0E,	///< Multiname object for attribute namespace
		CONSTANT_MultinameL = 0x1B,	///< Multiname object for local namespace
		CONSTANT_MultinameLA = 0x1C	///< Multiname object for local attribute namespace
	};

	eKind m_eKind;			///< Type of multiname structures
	Word32 m_uNS;			///< Namespace index used for QName
	Word32 m_uNS_Set;		///< Index into a Namespace set used for Multiname
	Word32 m_uName;			///< Index into the string array for the name used for QName and RTQName

	Multiname() : 
		m_eKind(CONSTANT_UNDEFINED),
		m_uNS(0),
		m_uNS_Set(0),
		m_uName(0)
	{
	}
	BURGER_INLINE Word IsQName(void) const { return m_eKind == CONSTANT_QName; }
};
class Namespace {
public:
	enum eKind {
		CONSTANT_Undefined = 0x00,		///< Uninitialized data
		CONSTANT_Namespace = 0x08,		///< Global namespace
		CONSTANT_PackageNamespace = 0x16,	///< Namespace for a package
		CONSTANT_PackageInternalNs = 0x17,	///< Namespace private to a package
		CONSTANT_ProtectedNamespace = 0x18,	///< Namespace protected for a package
		CONSTANT_ExplicitNamespace = 0x19,	///< Specific namespace (Fully qualified)
		CONSTANT_StaticProtectedNs = 0x1A,	///< Global protected namespace
		CONSTANT_PrivateNs = 0x05			///< Global private namespace
	};

	eKind m_eKind;	///< Type of namespace this name represents
	Word32 m_uName;	///< Index into the string array for the name

	Namespace() :
		m_eKind(CONSTANT_Undefined),
		m_uName(0)
	{
	}
};
class BaseExecute : public Base {
	BURGER_RTTI_IN_CLASS();
public:
	BaseExecute() {}
	virtual ~BaseExecute();
	virtual void Execute(CharacterObject *pObject);
	virtual void ExecuteState(CharacterObject *pObject);
	virtual void ExecuteStateReverse(CharacterObject *pObject,Word uFrame);
	virtual Word32 GetDepthIDOfReplaceOrAddTag(void) const;
};
}}
/* END */

#endif
