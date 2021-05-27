/***************************************

    Library to handle windows style configuration files (INI)

    Copyright (c) 1995-2017 by Rebecca Ann Heineman <becky@burgerbecky.com>

    It is released under an MIT Open Source license. Please see LICENSE for
    license details. Yes, you can use it in a commercial title without paying
    anything, just give me a credit.

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
		eType m_eType;				///< Type of derived class
	public:
		Generic(eType uType);
		virtual ~Generic();
		BURGER_INLINE Generic *GetNext(void) const { return static_cast<Generic *>(m_Link.GetNext()->GetData()); }
		BURGER_INLINE Generic *GetPrevious(void) const { return static_cast<Generic *>(m_Link.GetPrevious()->GetData()); }
		BURGER_INLINE eType GetType(void) const { return m_eType; }
		BURGER_INLINE void InsertBefore(Generic *pGeneric) { m_Link.InsertBefore(&pGeneric->m_Link); }
		BURGER_INLINE void InsertAfter(Generic *pGeneric) { m_Link.InsertAfter(&pGeneric->m_Link); }
	};

	class Comment : public Generic {
		String m_Comment;	///< Unmodified text for a comment line
	public:
		Comment();
		Comment(const char *pComment);
		virtual ~Comment();
		BURGER_INLINE const char *GetText(void) const { return m_Comment.GetPtr(); }
		BURGER_INLINE void SetText(const char *pInput) { m_Comment.Set(pInput); }
		BURGER_INLINE void SetText(const String *pInput) { m_Comment = pInput[0]; }
		static Comment * BURGER_API New(void);
		static Comment * BURGER_API New(const char *pComment);
	};

	class Entry : public Generic {
		String m_Key;			///< Value's key
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
		uint_t BURGER_API GetBoolean(uint_t bDefault) const;
		void BURGER_API SetBoolean(uint_t bValue);
		uint_t BURGER_API GetWord(uint_t uDefault,uint_t uMin=0,uint_t uMax=BURGER_MAXUINT) const;
		void BURGER_API SetWord(uint_t uValue);
		void BURGER_API SetWordHex(uint_t uValue);
		int_t BURGER_API GetInt(int_t iDefault,int_t iMin=(-BURGER_MAXINT)-1,int_t iMax=BURGER_MAXINT) const;
		void BURGER_API SetInt(int_t iValue);
		float BURGER_API GetFloat(float fDefault,float fMin,float fMax) const;
		void BURGER_API SetFloat(float fValue);
		double BURGER_API GetDouble(double dDefault,double dMin,double dMax) const;
		void BURGER_API SetDouble(double fValue);
		void BURGER_API GetString(String *pOutput,const char *pDefault) const;
		void BURGER_API SetString(const char *pValue);
		static Entry * BURGER_API New(void);
		static Entry * BURGER_API New(const char *pKey,const char *pValue);
	};

	class Section : public Generic {
		Generic m_Root;			///< Linked list root for all attached entries
		String m_SectionName;	///< Name of the section
	public:
		Section();
		Section(const char *pSection);
		virtual ~Section();
		uint_t BURGER_API Save(OutputMemoryStream *pOutput) const;
		BURGER_INLINE const char *GetName(void) const { return m_SectionName.GetPtr(); }
		BURGER_INLINE void SetName(const char *pInput) { m_SectionName.Set(pInput); }
		BURGER_INLINE void SetName(const String *pInput) { m_SectionName = pInput[0]; }
		Entry * BURGER_API AddEntry(const char *pKey,const char *pValue);
		Entry * BURGER_API FindEntry(const char *pKey) const;
		Entry * BURGER_API FindEntry(const char *pKey,uint_t bAlwaysCreate);
		Comment * BURGER_API AddComment(const char *pComment);
		const char * BURGER_API GetValue(const char *pKey) const;
		void BURGER_API SetValue(const char *pKey,const char *pInput);
		void BURGER_API SetValue(const char *pKey,const String *pInput);
		uint_t BURGER_API GetBoolean(const char *pKey,uint_t bDefault) const;
		void BURGER_API SetBoolean(const char *pKey,uint_t bValue);
		uint_t BURGER_API GetWord(const char *pKey,uint_t uDefault,uint_t uMin=0,uint_t uMax=BURGER_MAXUINT) const;
		void BURGER_API SetWord(const char *pKey,uint_t uValue);
		void BURGER_API SetWordHex(const char *pKey,uint_t uValue);
		int_t BURGER_API GetInt(const char *pKey,int_t iDefault,int_t iMin=(-BURGER_MAXINT)-1,int_t iMax=BURGER_MAXINT) const;
		void BURGER_API SetInt(const char *pKey,int_t iValue);
		float BURGER_API GetFloat(const char *pKey,float fDefault,float fMin,float fMax) const;
		void BURGER_API SetFloat(const char *pKey,float fValue);
		double BURGER_API GetDouble(const char *pKey,double dDefault,double dMin,double dMax) const;
		void BURGER_API SetDouble(const char *pKey,double dValue);
		void BURGER_API GetString(String *pOutput,const char *pKey,const char *pDefault) const;
		void BURGER_API SetString(const char *pKey,const char *pValue);
		static Section * BURGER_API New(void);
		static Section * BURGER_API New(const char *pSection);
	};

private:
	Generic m_Root;				///< Root entry for a list of sections
public:
	FileINI();
	~FileINI();
	static FileINI * BURGER_API New(void);
	static FileINI * BURGER_API New(const char *pFilename,uint_t bAlwaysCreate=FALSE);
	static FileINI * BURGER_API New(Filename *pFilename,uint_t bAlwaysCreate=FALSE);
	static FileINI * BURGER_API New(InputMemoryStream *pInput,uint_t bAlwaysCreate=FALSE);
	uint_t BURGER_API Init(const char *pFilename);
	uint_t BURGER_API Init(Filename *pFilename);
	uint_t BURGER_API Init(InputMemoryStream *pInput);
	void BURGER_API Shutdown(void);
	uint_t BURGER_API Save(const char *pFilename) const;
	uint_t BURGER_API Save(Filename *pFilename) const;
	uint_t BURGER_API Save(OutputMemoryStream *pOutput) const;
	Section * BURGER_API AddSection(const char *pSectionName);
	Section * BURGER_API FindSection(const char *pSectionName,uint_t bAlwaysCreate=FALSE);
	void BURGER_API DeleteSection(const char *pSectionName);
};
}
/* END */

#endif
