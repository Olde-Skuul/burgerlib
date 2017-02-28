/***************************************

	Library to handle xml files (XML)

	Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEXML_H__
#define __BRFILEXML_H__

#ifndef __BRTYPES_H__
#include "brtypes.h"
#endif

#ifndef __BRBASE_H__
#include "brbase.h"
#endif

#ifndef __BRLINKEDLISTPOINTER_H__
#include "brlinkedlistpointer.h"
#endif

#ifndef __BRSTRING_H__
#include "brstring.h"
#endif

#ifndef __BRFILENAME_H__
#include "brfilename.h"
#endif

#ifndef __BRINPUTMEMORYSTREAM_H__
#include "brinputmemorystream.h"
#endif

#ifndef __BROUTPUTMEMORYSTREAM_H__
#include "broutputmemorystream.h"
#endif

/* BEGIN */
namespace Burger {
class FileXML {
public:
	class Element;
	class Attribute;
	class RawText;
	class Generic : public Base {
	public:
		enum eType {
			XML_ROOT,			///< Only used for root objects.
			XML_COMMENT,		///< "Comment" strings to retain user supplied information
			XML_CDATA,			///< "CDATA" strings of data that is ignored by the parser
			XML_ATTRIBUTE,		///< Attribute attached to a tag
			XML_DECLARATION,	///< XML declaration
			XML_DOCUMENT,		///< Document header (DTD)
			XML_ELEMENT,		///< Data entries attached to a document header
			XML_TEXT,			///< Text content attached to a document
			XML_UNKNOWN			///< Unknown XML entry
		};
	private:
		LinkedListPointer m_Link;	///< Linked list entry
		eType m_eType;				///< Type of derived class
	public:
		Generic(eType uType);
		virtual ~Generic();
		virtual Word Parse(InputMemoryStream *pInput);
		virtual Word Save(OutputMemoryStream *pOutput,Word uDepth) const;
		BURGER_INLINE Generic *GetNext(void) const { return static_cast<Generic *>(m_Link.GetNext()->GetData()); }
		BURGER_INLINE Generic *GetPrevious(void) const { return static_cast<Generic *>(m_Link.GetPrevious()->GetData()); }
		BURGER_INLINE eType GetType(void) const { return m_eType; }
		BURGER_INLINE void InsertBefore(Generic *pGeneric) { m_Link.InsertBefore(&pGeneric->m_Link); }
		BURGER_INLINE void InsertAfter(Generic *pGeneric) { m_Link.InsertAfter(&pGeneric->m_Link); }
		Element * BURGER_API GetNextElement(void) const;
		Element * BURGER_API GetNextElement(const char *pElementName) const;
		Generic * BURGER_API GetNextItem(void) const;
	};

	class Root : public Generic {
	public:
		Root();
		virtual ~Root();
		Word BURGER_API ParseList(InputMemoryStream *pInput,Word bAllowRawText=FALSE);
		Word BURGER_API SaveList(OutputMemoryStream *pOutput,Word uDepth) const;
		void BURGER_API DeleteList(void);
		Generic * BURGER_API FindType(eType uType) const;
		void BURGER_API DeleteType(eType uType);
		const Element * BURGER_API FindElement(const char *pElementName) const;
		Element * BURGER_API FindElement(const char *pElementName,Word bAlwaysCreate=FALSE);
		Element * BURGER_API AddElement(const char *pElementName);
		void BURGER_API DeleteElement(const char *pElementName);
		void BURGER_API DeleteElements(const char *pElementName);
		const Attribute * BURGER_API FindAttribute(const char *pAttributeName) const;
		Attribute * BURGER_API FindAttribute(const char *pAttributeName,Word bAlwaysCreate=FALSE);
		Attribute * BURGER_API AddAttribute(const char *pAttributeName,const char *pValue=NULL);
		void BURGER_API DeleteAttribute(const char *pAttributeName);
		const RawText * BURGER_API FindRawText(void) const;
		RawText * BURGER_API FindRawText(Word bAlwaysCreate=FALSE);
		RawText * BURGER_API AddRawText(const char *pValue=NULL);
		void BURGER_API DeleteRawText(void);
	};

	class Comment : public Generic {
		String m_Comment;	///< Unmodified text for a comment line
	public:
		Comment();
		Comment(const char *pComment);
		virtual ~Comment();
		virtual Word Parse(InputMemoryStream *pInput);
		virtual Word Save(OutputMemoryStream *pOutput,Word uDepth) const;
		BURGER_INLINE const char *GetText(void) const { return m_Comment.GetPtr(); }
		BURGER_INLINE void SetText(const char *pInput) { m_Comment.Set(pInput); }
		BURGER_INLINE void SetText(const String *pInput) { m_Comment = pInput[0]; }
		static Comment * BURGER_API New(InputMemoryStream *pInput);
		static Comment * BURGER_API New(const char *pComment);
	};

	class CData : public Generic {
		String m_CData;				///< Unmodified text for a CData line
	public:
		CData();
		CData(const char *pCData);
		virtual ~CData();
		virtual Word Parse(InputMemoryStream *pInput);
		virtual Word Save(OutputMemoryStream *pOutput,Word uDepth) const;
		BURGER_INLINE const char *GetText(void) const { return m_CData.GetPtr(); }
		BURGER_INLINE void SetText(const char *pInput) { m_CData.Set(pInput); }
		BURGER_INLINE void SetText(const String *pInput) { m_CData = pInput[0]; }
		static CData * BURGER_API New(InputMemoryStream *pInput);
		static CData * BURGER_API New(const char *pCData);
	};

	class Attribute : public Generic {
		String m_Key;				///< Attribute keyword
		String m_Value;				///< Value attached to the attribute
	public:
		Attribute();
		virtual ~Attribute();
		virtual Word Parse(InputMemoryStream *pInput);
		virtual Word Save(OutputMemoryStream *pOutput,Word uDepth) const;
		BURGER_INLINE const char *GetKey(void) const { return m_Key.GetPtr(); }
		BURGER_INLINE void SetKey(const char *pInput) { m_Key.Set(pInput); }
		BURGER_INLINE void SetKey(const String *pInput) { m_Key = pInput[0]; }
		BURGER_INLINE const char *GetValue(void) const { return m_Value.GetPtr(); }
		BURGER_INLINE void SetValue(const char *pInput) { m_Value.Set(pInput); }
		BURGER_INLINE void SetValue(const String *pInput) { m_Value = pInput[0]; }
		BURGER_INLINE Word GetBoolean(Word bDefault) const { return m_Value.GetBoolean(bDefault); }
		BURGER_INLINE void SetBoolean(Word bValue) { m_Value.SetYesNo(bValue); }
		BURGER_INLINE Word GetWord(Word uDefault,Word uMin=0,Word uMax=BURGER_MAXUINT) const { return m_Value.GetWord(uDefault,uMin,uMax); }
		BURGER_INLINE void SetWord(Word uValue) { m_Value.SetWord(uValue); }
		BURGER_INLINE void SetWordHex(Word uValue) { m_Value.SetWordHex(uValue); }
		BURGER_INLINE Int GetInt(Int iDefault,Int iMin=(-BURGER_MAXINT)-1,Int iMax=BURGER_MAXINT) const { return m_Value.GetInt(iDefault,iMin,iMax); }
		BURGER_INLINE void SetInt(Int iValue) { m_Value.SetInt(iValue); }
		BURGER_INLINE float GetFloat(float fDefault) const { return m_Value.GetFloat(fDefault); }
		BURGER_INLINE float GetFloat(float fDefault,float fMin,float fMax) const { return m_Value.GetFloat(fDefault,fMin,fMax); }
		BURGER_INLINE void SetFloat(float fValue) { m_Value.SetFloat(fValue); }
		BURGER_INLINE double GetDouble(double dDefault) const { return m_Value.GetDouble(dDefault); }
		BURGER_INLINE double GetDouble(double dDefault,double dMin,double dMax) const { return m_Value.GetDouble(dDefault,dMin,dMax); }
		BURGER_INLINE void SetDouble(double dValue) { m_Value.SetDouble(dValue); }
		static Attribute * BURGER_API New(InputMemoryStream *pInput);
		static Attribute * BURGER_API New(const char *pInput,const char *pValue=NULL);
		Attribute * BURGER_API Update(Root *pRoot,const char *pKey,const char *pValue);
	};

	class Declaration : public Generic {
		String m_Encoding;		///< ASCII of the encoding (Normally it's UTF-8)
		float m_fVersion;		///< Version number of the declaration
		Word m_bStandalone;		///< \ref TRUE if it's a standalone XML file
	public:
		Declaration();
		Declaration(float fVersion,const char *pEncoding=NULL,Word bStandalone=2);
		virtual ~Declaration();
		virtual Word Parse(InputMemoryStream *pInput);
		virtual Word Save(OutputMemoryStream *pOutput,Word uDepth) const;
		BURGER_INLINE float GetVersion(void) const { return m_fVersion; }
		void BURGER_API SetVersion(float fInput=1.0f);
		BURGER_INLINE const char *GetEncoding(void) const { return m_Encoding.GetPtr(); }
		BURGER_INLINE void SetEncoding(const char *pInput) { m_Encoding.Set(pInput); }
		BURGER_INLINE void SetEncoding(const String *pInput) { m_Encoding = pInput[0]; }
		Word BURGER_INLINE GetStandalone(void) const { return m_bStandalone==1;}
		void BURGER_INLINE SetStandalone(Word bInput) { m_bStandalone = bInput; }
		static Declaration * BURGER_API New(InputMemoryStream *pInput);
		static Declaration * BURGER_API New(float fVersion,const char *pEncoding=NULL,Word bStandalone=2);
	};

	class RawText : public Generic {
		String m_Text;			///< Name of the element
	public:
		RawText();
		RawText(const char *pText);
		virtual ~RawText();
		virtual Word Parse(InputMemoryStream *pInput);
		virtual Word Save(OutputMemoryStream *pOutput,Word uDepth) const;
		BURGER_INLINE const char *GetText(void) const { return m_Text.GetPtr(); }
		BURGER_INLINE void SetText(const char *pInput) { m_Text.Set(pInput); }
		BURGER_INLINE void SetText(const String *pInput) { m_Text = pInput[0]; }
		BURGER_INLINE Word GetBoolean(Word bDefault) const { return m_Text.GetBoolean(bDefault); }
		BURGER_INLINE void SetBoolean(Word bValue) { m_Text.SetYesNo(bValue); }
		BURGER_INLINE Word GetWord(Word uDefault,Word uMin=0,Word uMax=BURGER_MAXUINT) const { return m_Text.GetWord(uDefault,uMin,uMax); }
		BURGER_INLINE void SetWord(Word uValue) { m_Text.SetWord(uValue); }
		BURGER_INLINE void SetWordHex(Word uValue) { m_Text.SetWordHex(uValue); }
		BURGER_INLINE Int GetInt(Int iDefault,Int iMin=(-BURGER_MAXINT)-1,Int iMax=BURGER_MAXINT) const { return m_Text.GetInt(iDefault,iMin,iMax); }
		BURGER_INLINE void SetInt(Int iValue) { m_Text.SetInt(iValue); }
		BURGER_INLINE float GetFloat(float fDefault) const { return m_Text.GetFloat(fDefault); }
		BURGER_INLINE float GetFloat(float fDefault,float fMin,float fMax) const { return m_Text.GetFloat(fDefault,fMin,fMax); }
		BURGER_INLINE void SetFloat(float fValue) { m_Text.SetFloat(fValue); }
		BURGER_INLINE double GetDouble(double dDefault) const { return m_Text.GetDouble(dDefault); }
		BURGER_INLINE double GetDouble(double dDefault,double dMin,double dMax) const { return m_Text.GetDouble(dDefault,dMin,dMax); }
		BURGER_INLINE void SetDouble(double dValue) { m_Text.SetDouble(dValue); }
		static RawText * BURGER_API New(InputMemoryStream *pInput);
		static RawText * BURGER_API New(const char *pText);
	};

	class Element : public Generic {
		Root m_Attributes;	///< Linked list root for all attached attributes
		Root m_Root;		///< Linked list root for all contained objects
		String m_Name;		///< Name of the element
	public:
		Element();
		Element(const char *pName);
		virtual ~Element();
		virtual Word Parse(InputMemoryStream *pInput);
		virtual Word Save(OutputMemoryStream *pOutput,Word uDepth) const;
		BURGER_INLINE Root * GetRoot(void) { return &m_Root; }
		BURGER_INLINE const Root * GetRoot(void) const { return &m_Root; }
		BURGER_INLINE const Element * FindElement(const char *pElementName) const { return m_Root.FindElement(pElementName); }
		BURGER_INLINE Element * FindElement(const char *pElementName,Word bAlwaysCreate=FALSE) { return m_Root.FindElement(pElementName,bAlwaysCreate); }
		BURGER_INLINE Element * AddElement(const char *pElementName) { return m_Root.AddElement(pElementName); }
		BURGER_INLINE void DeleteElement(const char *pElementName) { return m_Root.DeleteElement(pElementName); }
		BURGER_INLINE void DeleteElements(const char *pElementName) { return m_Root.DeleteElements(pElementName); }
		BURGER_INLINE Attribute * FindAttribute(const char *pAttributeName,Word bAlwaysCreate=FALSE) { return m_Attributes.FindAttribute(pAttributeName,bAlwaysCreate); }
		BURGER_INLINE Attribute * AddAttribute(const char *pAttributeName,const char *pValue=NULL) { return m_Attributes.AddAttribute(pAttributeName,pValue); }
		BURGER_INLINE void DeleteAttribute(const char *pAttributeName) { return m_Attributes.DeleteAttribute(pAttributeName); }
		BURGER_INLINE const char *GetName(void) const { return m_Name.GetPtr(); }
		BURGER_INLINE void SetName(const char *pInput) { m_Name.Set(pInput); }
		BURGER_INLINE void SetName(const String *pInput) { m_Name = pInput[0]; }
		Word BURGER_API GetBoolean(Word bDefault) const;
		void BURGER_API SetBoolean(Word bValue);
		Word BURGER_API GetWord(Word uDefault,Word uMin=0,Word uMax=BURGER_MAXUINT) const;
		void BURGER_API SetWord(Word uValue);
		void BURGER_API SetWordHex(Word uValue);
		Int BURGER_API GetInt(Int iDefault,Int iMin=(-BURGER_MAXINT)-1,Int iMax=BURGER_MAXINT) const;
		void BURGER_API SetInt(Int iValue);
		float BURGER_API GetFloat(float fDefault) const;
		float BURGER_API GetFloat(float fDefault,float fMin,float fMax) const;
		void BURGER_API SetFloat(float fValue);
		double BURGER_API GetDouble(double dDefault) const;
		double BURGER_API GetDouble(double dDefault,double dMin,double dMax) const;
		void BURGER_API SetDouble(double dValue);
		const char *BURGER_API GetString(const char *pDefault) const;
		void BURGER_API SetString(const char *pValue);
		Word BURGER_API AttributeGetBoolean(const char *pAttributeName,Word bDefault) const;
		void BURGER_API AttributeSetBoolean(const char *pAttributeName,Word bValue);
		Word BURGER_API AttributeGetWord(const char *pAttributeName,Word uDefault,Word uMin=0,Word uMax=BURGER_MAXUINT) const;
		void BURGER_API AttributeSetWord(const char *pAttributeName,Word uValue);
		void BURGER_API AttributeSetWordHex(const char *pAttributeName,Word uValue);
		Int BURGER_API AttributeGetInt(const char *pAttributeName,Int iDefault,Int iMin=(-BURGER_MAXINT)-1,Int iMax=BURGER_MAXINT) const;
		void BURGER_API AttributeSetInt(const char *pAttributeName,Int iValue);
		float BURGER_API AttributeGetFloat(const char *pAttributeName,float fDefault) const;
		float BURGER_API AttributeGetFloat(const char *pAttributeName,float fDefault,float fMin,float fMax) const;
		void BURGER_API AttributeSetFloat(const char *pAttributeName,float fValue);
		double BURGER_API AttributeGetDouble(const char *pAttributeName,double dDefault) const;
		double BURGER_API AttributeGetDouble(const char *pAttributeName,double dDefault,double dMin,double dMax) const;
		void BURGER_API AttributeSetDouble(const char *pAttributeName,double dValue);
		const char *BURGER_API AttributeGetString(const char *pAttributeName,const char *pDefault) const;
		void BURGER_API AttributeSetString(const char *pAttributeName,const char *pValue);
		Word BURGER_API ElementGetBoolean(const char *pElementName,Word bDefault) const;
		void BURGER_API ElementSetBoolean(const char *pElementName,Word bValue);
		Word BURGER_API ElementGetWord(const char *pElementName,Word uDefault,Word uMin=0,Word uMax=BURGER_MAXUINT) const;
		void BURGER_API ElementSetWord(const char *pElementName,Word uValue);
		void BURGER_API ElementSetWordHex(const char *pElementName,Word uValue);
		Int BURGER_API ElementGetInt(const char *pElementName,Int iDefault,Int iMin=(-BURGER_MAXINT)-1,Int iMax=BURGER_MAXINT) const;
		void BURGER_API ElementSetInt(const char *pElementName,Int iValue);
		float BURGER_API ElementGetFloat(const char *pElementName,float fDefault) const;
		float BURGER_API ElementGetFloat(const char *pElementName,float fDefault,float fMin,float fMax) const;
		void BURGER_API ElementSetFloat(const char *pElementName,float fValue);
		double BURGER_API ElementGetDouble(const char *pElementName,double dDefault) const;
		double BURGER_API ElementGetDouble(const char *pElementName,double dDefault,double dMin,double dMax) const;
		void BURGER_API ElementSetDouble(const char *pElementName,double dValue);
		const char *BURGER_API ElementGetString(const char *pElementName,const char *pDefault) const;
		void BURGER_API ElementSetString(const char *pElementName,const char *pValue);
		static Element * BURGER_API New(InputMemoryStream *pInput);
		static Element * BURGER_API New(const char *pName);
	};

private:
	Root m_Root;				///< Root entry for a list of XML objects
	Word m_bUTF8ByteMark;		///< TRUE if the UTF-8 byte mark should be emitted on write
public:
	FileXML();
	~FileXML();
	static FileXML * BURGER_API New(void);
	static FileXML * BURGER_API New(const char *pFilename,Word bAlwaysCreate=FALSE);
	static FileXML * BURGER_API New(Filename *pFilename,Word bAlwaysCreate=FALSE);
	static FileXML * BURGER_API New(InputMemoryStream *pInput,Word bAlwaysCreate=FALSE);
	Word BURGER_API Init(const char *pFilename);
	Word BURGER_API Init(Filename *pFilename);
	Word BURGER_API Init(InputMemoryStream *pInput);
	void BURGER_API Shutdown(void);
	Word BURGER_API Save(OutputMemoryStream *pOutput) const;
	BURGER_INLINE Word GetUTF8ByteMark(void) const { return m_bUTF8ByteMark; }
	BURGER_INLINE void SetUTF8ByteMark(Word bUTF8ByteMark) { m_bUTF8ByteMark = bUTF8ByteMark; }
	static Word BURGER_API ReadXMLName(String *pOutput,InputMemoryStream *pInput);
	static Word BURGER_API ReadXMLText(String *pOutput,InputMemoryStream *pInput);
	static Word BURGER_API SaveXMLString(OutputMemoryStream *pOutput,const char *pInput);
	static Word BURGER_API DecodeXMLString(String *pInput);
	Declaration *BURGER_API FindDeclaration(Word bAlwaysCreate=FALSE);
	Declaration *BURGER_API AddDeclaration(float fVersion=1.0f,const char *pEncoding=NULL,Word bStandalone=2);
	BURGER_INLINE void DeleteDeclaration(void) { m_Root.DeleteType(Generic::XML_DECLARATION); }
	BURGER_INLINE Root * GetRoot(void) { return &m_Root; }
	BURGER_INLINE const Root * GetRoot(void) const { return &m_Root; }
	BURGER_INLINE const Element *FindElement(const char *pElementName) const { return m_Root.FindElement(pElementName); }
	BURGER_INLINE Element *FindElement(const char *pElementName,Word bAlwaysCreate=FALSE) { return m_Root.FindElement(pElementName,bAlwaysCreate); }
	BURGER_INLINE Element *AddElement(const char *pElementName) { return m_Root.AddElement(pElementName); }
	BURGER_INLINE void DeleteElement(const char *pElementName) { m_Root.DeleteElement(pElementName); }
	BURGER_INLINE void DeleteElements(const char *pElementName) { m_Root.DeleteElements(pElementName); }
	BURGER_INLINE Element * GetFirstElement(void) const { return m_Root.GetNextElement(); }
	BURGER_INLINE Generic * GetFirstItem(void) const { return m_Root.GetNextItem(); }
};
}
/* END */

#endif
