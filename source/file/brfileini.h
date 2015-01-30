/***************************************

	Library to handle windows style configuration files (INI)

	Copyright (c) 1995-2015 by Rebecca Ann Heineman <becky@burgerbecky.com>

	It is released under an MIT Open Source license. Please see LICENSE
	for license details. Yes, you can use it in a
	commercial title without paying anything, just give me a credit.
	Please? It's not like I'm asking you for money!

***************************************/

#ifndef __BRFILEINI_H__
#define __BRFILEINI_H__

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
class FileINI {
public:
	class Generic : public Base {
	public:
		enum eType {
			LINEENTRY_ROOT,			///< Only used for root objects.
			LINEENTRY_SECTION,		///< INI section header
			LINEENTRY_ENTRY,		///< Data entries attached to a section
			LINEENTRY_COMMENT		///< "Comment" strings to retain user supplied information
		};
	private:
		LinkedListPointer m_Link;	///< Linked list entry
		eType m_eType; 				///< Type of derived class
	public:
		Generic(eType uType);
		virtual ~Generic();
		BURGER_INLINE Generic *GetNext(void) const { return static_cast<Generic *>(m_Link.GetNext()->GetData()); }
		BURGER_INLINE Generic *GetPrevious(void) const { return static_cast<Generic *>(m_Link.GetPrevious()->GetData()); }
		BURGER_INLINE eType GetType(void) const { return m_eType; }
		BURGER_INLINE void InsertBefore(Generic *pGeneric) { m_Link.InsertBefore(&pGeneric->m_Link); }
		BURGER_INLINE void InsertAfter(Generic *pGeneric) { m_Link.InsertAfter(&pGeneric->m_Link); }
	};

	class Comment : public Generic  {
		String m_Comment;	///< Unmodified text for a comment line
	public:
		Comment();
		Comment(const char *pComment);
		virtual ~Comment();
		BURGER_INLINE const char *GetText(void) const { return m_Comment.GetPtr(); }
		BURGER_INLINE void SetText(const char *pInput) { m_Comment.Set(pInput); }
		BURGER_INLINE void SetText(const String *pInput) { m_Comment = pInput[0]; }
	};

	class Entry : public Generic {
		String m_Key;	   		///< Value's key
		String m_Value;			///< UTF-8 form of the value
	public:
		Entry();
		Entry(const char *pKey,const char *pValue);
		virtual ~Entry();
		BURGER_INLINE const char *GetKey(void) const { return m_Key.GetPtr(); }
		BURGER_INLINE void SetKey(const char *pInput) { m_Key.Set(pInput); }
		BURGER_INLINE void SetKey(const String *pInput) { m_Key = pInput[0]; }
		BURGER_INLINE const char *GetValue(void) const { return m_Value.GetPtr(); }
		BURGER_INLINE void SetValue(const char *pInput) { m_Value.Set(pInput); }
		BURGER_INLINE void SetValue(const String *pInput) { m_Value = pInput[0]; }
		Word GetBoolean(Word bDefault) const;
		void SetBoolean(Word bValue);
		Word GetWord(Word uDefault,Word uMin=0,Word uMax=BURGER_MAXUINT) const;
		void SetWord(Word uValue);
		void SetWordHex(Word uValue);
		Int GetInt(Int iDefault,Int iMin=(-BURGER_MAXINT)-1,Int iMax=BURGER_MAXINT) const;
		void SetInt(Int iValue);
		float GetFloat(float fDefault,float fMin,float fMax) const;
		void SetFloat(float fValue);
		double GetDouble(double dDefault,double dMin,double dMax) const;
		void SetDouble(double fValue);
		void GetString(String *pOutput,const char *pDefault) const;
		void SetString(const char *pValue);
	};

	class Section : public Generic  {
		Generic m_Root;			///< Linked list root for all attached entries
		String m_SectionName;	///< Name of the section
	public:
		Section();
		Section(const char *pSection);
		virtual ~Section();
		Word Save(OutputMemoryStream *pOutput) const;
		BURGER_INLINE const char *GetName(void) const { return m_SectionName.GetPtr(); }
		BURGER_INLINE void SetName(const char *pInput) { m_SectionName.Set(pInput); }
		BURGER_INLINE void SetName(const String *pInput) { m_SectionName = pInput[0]; }
		Entry *AddEntry(const char *pKey,const char *pValue);
		Entry *FindEntry(const char *pKey) const;
		Entry *FindEntry(const char *pKey,Word bAlwaysCreate);
		Comment *AddComment(const char *pComment);
		const char *GetValue(const char *pKey) const;
		void SetValue(const char *pKey,const char *pInput);
		void SetValue(const char *pKey,const String *pInput);
		Word GetBoolean(const char *pKey,Word bDefault) const;
		void SetBoolean(const char *pKey,Word bValue);
		Word GetWord(const char *pKey,Word uDefault,Word uMin=0,Word uMax=BURGER_MAXUINT) const;
		void SetWord(const char *pKey,Word uValue);
		void SetWordHex(const char *pKey,Word uValue);
		Int GetInt(const char *pKey,Int iDefault,Int iMin=(-BURGER_MAXINT)-1,Int iMax=BURGER_MAXINT) const;
		void SetInt(const char *pKey,Int iValue);
		float GetFloat(const char *pKey,float fDefault,float fMin,float fMax) const;
		void SetFloat(const char *pKey,float fValue);
		double GetDouble(const char *pKey,double dDefault,double dMin,double dMax) const;
		void SetDouble(const char *pKey,double dValue);
		void GetString(String *pOutput,const char *pKey,const char *pDefault) const;
		void SetString(const char *pKey,const char *pValue);
	};

private:
	Generic m_Root;				///< Root entry for a list of sections
public:
	FileINI();
	~FileINI();
	static FileINI * BURGER_API New(void);
	static FileINI * BURGER_API New(const char *pFilename,Word bAlwaysCreate=FALSE);
	static FileINI * BURGER_API New(Filename *pFilename,Word bAlwaysCreate=FALSE);
	static FileINI * BURGER_API New(InputMemoryStream *pInput,Word bAlwaysCreate=FALSE);
	Word Init(const char *pFilename);
	Word Init(Filename *pFilename);
	Word Init(InputMemoryStream *pInput);
	void Shutdown(void);
	Word Save(OutputMemoryStream *pOutput) const;
	Section *AddSection(const char *pSectionName);
	Section *FindSection(const char *pSectionName,Word bAlwaysCreate=FALSE);
	void DeleteSection(const char *pSectionName);
};
}
/* END */

#endif
